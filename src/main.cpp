#include <stdio.h>
#include <errno.h>
#include <inttypes.h>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libavutil/pixdesc.h>
}

static enum AVPixelFormat hw_pix_fmt;
static FILE *output_file = NULL;

// 打印所有支持的硬件加速器
void print_hw_decoders() {
   void *opaque = NULL;
   const AVCodec *codec = NULL;
   printf("Available HW decoders:\n");
   while ((codec = av_codec_iterate(&opaque))) {
       if (!av_codec_is_decoder(codec)) {
           continue;
       }
       
       // 检查是否支持硬件加速
       if (codec->capabilities & AV_CODEC_CAP_HARDWARE) {
           printf("- %s\n", codec->name);
       }
   }
}

/**
 * 硬件解码的格式协商回调
 * @note 个人理解：将想要的硬件格式，与编码器实际支持的硬件格式做比对，选择一个合适的硬件格式
 */
static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,                // 解码器上下文
                                     const enum AVPixelFormat *pix_fmts)    // 解码器支持的像素格式列表
{
   const enum AVPixelFormat *p;
   for (p = pix_fmts; *p != -1; p++) {
       if (*p == hw_pix_fmt)
           return *p;
   }
   fprintf(stderr, "Failed to get HW surface format.\n");
   return AV_PIX_FMT_NONE;
}

// 尝试初始化解码器
const AVCodec* init_decoder() {
   const AVCodec* decoder = NULL;           // 解码器
   AVBufferRef *temp_hw_device_ctx = NULL;  // 临时硬件设备上下文

   // 首先尝试 VideoToolbox
   decoder = avcodec_find_decoder_by_name("h264_videotoolbox");
   if (decoder) {
       // 测试是否真的可以创建硬件设备上下文
       int ret = av_hwdevice_ctx_create(&temp_hw_device_ctx, 
                                      AV_HWDEVICE_TYPE_VIDEOTOOLBOX,
                                      NULL, NULL, 0);
       if (ret >= 0) {
           printf("Successfully initialized VideoToolbox decoder\n");
           av_buffer_unref(&temp_hw_device_ctx);
           return decoder;
       }
       printf("Found VideoToolbox decoder but failed to initialize hardware context\n");
   }
   
   // 回退到软解码
   decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
   if (decoder) {
       printf("Using software H264 decoder: %s\n", decoder->name);
       return decoder;
   }
   
   printf("No H264 decoder found\n");
   return NULL;
}

static int decode_write(AVCodecContext *avctx, AVPacket *packet)
{
    AVFrame *frame = NULL, *sw_frame = NULL;
    AVFrame *tmp_frame = NULL;
    int ret = 0;
    bool using_hw_decoder = (strcmp(avctx->codec->name, "h264_videotoolbox") == 0);

    printf("\nSending packet to decoder...");
    // 发送数据包到解码器
    ret = avcodec_send_packet(avctx, packet);    
    if (ret < 0) {
       fprintf(stderr, "Error during sending packet: %d\n", ret);
       return ret;
    }

    frame = av_frame_alloc();
    if (!frame) {
       fprintf(stderr, "Can not alloc frame\n");
       return AVERROR(ENOMEM);
    }

    printf("\nReceiving frame...");
    // 接收解码后的帧
    ret = avcodec_receive_frame(avctx, frame);
    if (ret == AVERROR(EAGAIN)) {
       printf("\nNeed more data");
       av_frame_free(&frame);
       return 0;
    } else if (ret == AVERROR_EOF) {
       printf("\nEnd of stream");
       av_frame_free(&frame);
       return 0;
    } else if (ret < 0) {
       fprintf(stderr, "\nError while receiving frame: %d\n", ret);
       av_frame_free(&frame);
       return ret;
    }

    printf("\nFrame received successfully: %dx%d, format: %d", 
          frame->width, frame->height, frame->format);

    // 判断是否使用硬件解码器，且当前帧的像素格式是硬件像素格式
    if (using_hw_decoder && frame->format == hw_pix_fmt) {
        // 分配一个新的软件帧，用于存储从硬件帧转换来的数据
        sw_frame = av_frame_alloc();
        if (!sw_frame) {
            // 如果软件帧分配失败
            av_frame_free(&frame);// 释放原始硬件帧
            fprintf(stderr, "Can not alloc sw_frame\n");
            return AVERROR(ENOMEM);
        }
        // 硬件帧到软件帧的传输—— important!!!  这是硬件解码后将数据转换为可在 CPU 上处理的格式的关键步骤
        if ((ret = av_hwframe_transfer_data(sw_frame, frame, 0)) < 0) { 
           fprintf(stderr, "Error transferring the data to system memory\n");
           av_frame_free(&frame);    // 释放硬件帧
           av_frame_free(&sw_frame); // 释放软件帧
           return ret;
        }
        // 将 tmp_frame 指向新创建的软件帧
        tmp_frame = sw_frame;
    } else {
        // 如果不是硬件解码帧，直接使用原始帧
        tmp_frame = frame;
    }

    // 写入 YUV 数据
    for (int i = 0; i < tmp_frame->height; i++) {
        fwrite(tmp_frame->data[0] + i * tmp_frame->linesize[0], 
              1, tmp_frame->width, output_file);
    }
   
    for (int i = 0; i < tmp_frame->height/2; i++) {
        fwrite(tmp_frame->data[1] + i * tmp_frame->linesize[1], 
             1, tmp_frame->width/2, output_file);
    }
   
    for (int i = 0; i < tmp_frame->height/2; i++) {
       fwrite(tmp_frame->data[2] + i * tmp_frame->linesize[2], 
              1, tmp_frame->width/2, output_file);
    }

    av_frame_free(&frame);
    if (sw_frame) av_frame_free(&sw_frame);
    return 0;
}

int main(int argc, char *argv[])
{
   AVFormatContext *input_ctx = NULL;       // 输入文件上下文
   AVStream *video_stream = NULL;           // 视频流
   AVCodecContext *decoder_ctx = NULL;      // 解码器上下文
   AVBufferRef *hw_device_ctx = NULL;       // 硬件设备上下文
   int video_stream_index = -1;             // 视频流索引
   AVPacket *packet = NULL;                 // 输入包
   int ret = 0;
   int frame_count = 0;

   if (argc < 3) {
       fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
       return -1;
   }

   // 打印可用的硬件解码器
   //print_hw_decoders();

   // 初始化解码器
   const AVCodec *decoder = init_decoder();
   if (!decoder) {
       fprintf(stderr, "Failed to find any H264 decoder.\n");
       return -1;
   }

   // 打开输入文件
   if ((ret = avformat_open_input(&input_ctx, argv[1], NULL, NULL)) < 0) {
       fprintf(stderr, "Cannot open input file: %s\n", argv[1]);
       return ret;
   }

   // 查找流信息
   if ((ret = avformat_find_stream_info(input_ctx, NULL)) < 0) {
       fprintf(stderr, "Cannot find input stream information.\n");
       goto cleanup;
   }

   // 查找视频流
   for (int i = 0; i < input_ctx->nb_streams; i++) {
       if (input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
           video_stream_index = i;
           video_stream = input_ctx->streams[i];
           break;
       }
   }

   if (video_stream_index == -1) {
       fprintf(stderr, "No video stream found\n");
       goto cleanup;
   }

   // 打印视频信息
   printf("Input file: %s\n", argv[1]);
   printf("Video stream index: %d\n", video_stream_index);
   printf("Video dimensions: %dx%d\n", 
          video_stream->codecpar->width,
          video_stream->codecpar->height);
   printf("Codec ID: %d\n", video_stream->codecpar->codec_id);
   printf("Bit rate: %" PRId64 "\n", video_stream->codecpar->bit_rate);

   // 创建解码器上下文
   decoder_ctx = avcodec_alloc_context3(decoder);
   if (!decoder_ctx) {
       fprintf(stderr, "Failed to allocate decoder context.\n");
       ret = AVERROR(ENOMEM);
       goto cleanup;
   }

   // 复制编解码器参数
   if ((ret = avcodec_parameters_to_context(decoder_ctx, video_stream->codecpar)) < 0) {
       fprintf(stderr, "Failed to copy decoder params.\n");
       goto cleanup;
   }

   // 如果是硬件解码器，设置硬件上下文
   if (strcmp(decoder->name, "h264_videotoolbox") == 0) {
       ret = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_VIDEOTOOLBOX,
                                   NULL, NULL, 0);
       if (ret < 0) {
           fprintf(stderr, "Failed to create hardware device context\n");
           goto cleanup;
       }
       /**
        * @param hw_device_ctx 硬件设备上下文
        * @param decoder_ctx 解码器上下文
        * @param av_buffer_ref()  保证了 decoder_ctx->hw_device_ctx 和 hw_device_ctx 指向相同的硬件设备上下文，但增加了引用计数。
        */
       decoder_ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
       hw_pix_fmt = AV_PIX_FMT_VIDEOTOOLBOX;
       // 当解码器需要确定输出格式时会调用这个回调
       // 通常在初始化时设置
       decoder_ctx->get_format = get_hw_format;
   }

   // 打开解码器
   if ((ret = avcodec_open2(decoder_ctx, decoder, NULL)) < 0) {
       fprintf(stderr, "Failed to open codec.\n");
       goto cleanup;
   }

   // 打开输出文件
   output_file = fopen(argv[2], "wb");
   if (!output_file) {
       fprintf(stderr, "Failed to open output file '%s': %s\n", 
               argv[2], strerror(errno));
       ret = -errno;
       goto cleanup;
   }

   // 分配数据包
   packet = av_packet_alloc();
   if (!packet) {
       fprintf(stderr, "Failed to allocate packet.\n");
       ret = AVERROR(ENOMEM);
       goto cleanup;
   }

   printf("Starting decoding...\n");
   
   while (1) {
       ret = av_read_frame(input_ctx, packet); // 其实应该是read_packet
       if (ret < 0) {
           printf("\nav_read_frame returned %d\n", ret);
           break;
       }

       if (packet->stream_index == video_stream_index) {
           printf("\rProcessing packet %d, size: %d bytes", frame_count++, packet->size);
           fflush(stdout);
           
           ret = decode_write(decoder_ctx, packet); // 解码并写入文件
           if (ret < 0) {
               fprintf(stderr, "\nError during decoding: %d\n", ret);
               char errbuf[AV_ERROR_MAX_STRING_SIZE];
               av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
               fprintf(stderr, "Error message: %s\n", errbuf);
               break;
           }
       }

       av_packet_unref(packet);
   }

   printf("\nTotal packets processed: %d\n", frame_count);

   // 冲刷解码器
   ret = avcodec_send_packet(decoder_ctx, NULL);
   if (ret >= 0) {
       while (ret >= 0) {
           ret = decode_write(decoder_ctx, packet);
           if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
               break;
           } else if (ret < 0) {
               fprintf(stderr, "Error during flushing: %d\n", ret);
               break;
           }
       }
   }

cleanup:
   if (output_file)
       fclose(output_file);
   av_packet_free(&packet);
   avcodec_free_context(&decoder_ctx);
   av_buffer_unref(&hw_device_ctx);
   avformat_close_input(&input_ctx);

   return ret;
}