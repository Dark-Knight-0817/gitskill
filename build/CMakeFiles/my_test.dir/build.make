# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/darkknight/Documents/learning resource/code/test_env"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/darkknight/Documents/learning resource/code/test_env/build"

# Include any dependencies generated for this target.
include CMakeFiles/my_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/my_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/my_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/my_test.dir/flags.make

CMakeFiles/my_test.dir/codegen:
.PHONY : CMakeFiles/my_test.dir/codegen

CMakeFiles/my_test.dir/src/main.cpp.o: CMakeFiles/my_test.dir/flags.make
CMakeFiles/my_test.dir/src/main.cpp.o: /Users/darkknight/Documents/learning\ resource/code/test_env/src/main.cpp
CMakeFiles/my_test.dir/src/main.cpp.o: CMakeFiles/my_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/darkknight/Documents/learning resource/code/test_env/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/my_test.dir/src/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/my_test.dir/src/main.cpp.o -MF CMakeFiles/my_test.dir/src/main.cpp.o.d -o CMakeFiles/my_test.dir/src/main.cpp.o -c "/Users/darkknight/Documents/learning resource/code/test_env/src/main.cpp"

CMakeFiles/my_test.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/my_test.dir/src/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/darkknight/Documents/learning resource/code/test_env/src/main.cpp" > CMakeFiles/my_test.dir/src/main.cpp.i

CMakeFiles/my_test.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/my_test.dir/src/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/darkknight/Documents/learning resource/code/test_env/src/main.cpp" -o CMakeFiles/my_test.dir/src/main.cpp.s

# Object files for target my_test
my_test_OBJECTS = \
"CMakeFiles/my_test.dir/src/main.cpp.o"

# External object files for target my_test
my_test_EXTERNAL_OBJECTS =

my_test: CMakeFiles/my_test.dir/src/main.cpp.o
my_test: CMakeFiles/my_test.dir/build.make
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libavcodec.dylib
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libavformat.dylib
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libavutil.dylib
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libavdevice.dylib
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libswresample.dylib
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libpostproc.dylib
my_test: /opt/homebrew/Cellar/ffmpeg/7.1_3/lib/libswscale.dylib
my_test: /opt/homebrew/lib/libSDL2.dylib
my_test: CMakeFiles/my_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/Users/darkknight/Documents/learning resource/code/test_env/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable my_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/my_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/my_test.dir/build: my_test
.PHONY : CMakeFiles/my_test.dir/build

CMakeFiles/my_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/my_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/my_test.dir/clean

CMakeFiles/my_test.dir/depend:
	cd "/Users/darkknight/Documents/learning resource/code/test_env/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/darkknight/Documents/learning resource/code/test_env" "/Users/darkknight/Documents/learning resource/code/test_env" "/Users/darkknight/Documents/learning resource/code/test_env/build" "/Users/darkknight/Documents/learning resource/code/test_env/build" "/Users/darkknight/Documents/learning resource/code/test_env/build/CMakeFiles/my_test.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/my_test.dir/depend

