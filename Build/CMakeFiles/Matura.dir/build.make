# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /run/media/elias/01D5A94471A88F00/GitHub/Matura

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /run/media/elias/01D5A94471A88F00/GitHub/Matura/Build

# Include any dependencies generated for this target.
include CMakeFiles/Matura.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Matura.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Matura.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Matura.dir/flags.make

CMakeFiles/Matura.dir/main.cpp.o: CMakeFiles/Matura.dir/flags.make
CMakeFiles/Matura.dir/main.cpp.o: ../main.cpp
CMakeFiles/Matura.dir/main.cpp.o: CMakeFiles/Matura.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/run/media/elias/01D5A94471A88F00/GitHub/Matura/Build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Matura.dir/main.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Matura.dir/main.cpp.o -MF CMakeFiles/Matura.dir/main.cpp.o.d -o CMakeFiles/Matura.dir/main.cpp.o -c /run/media/elias/01D5A94471A88F00/GitHub/Matura/main.cpp

CMakeFiles/Matura.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Matura.dir/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /run/media/elias/01D5A94471A88F00/GitHub/Matura/main.cpp > CMakeFiles/Matura.dir/main.cpp.i

CMakeFiles/Matura.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Matura.dir/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /run/media/elias/01D5A94471A88F00/GitHub/Matura/main.cpp -o CMakeFiles/Matura.dir/main.cpp.s

# Object files for target Matura
Matura_OBJECTS = \
"CMakeFiles/Matura.dir/main.cpp.o"

# External object files for target Matura
Matura_EXTERNAL_OBJECTS =

Matura: CMakeFiles/Matura.dir/main.cpp.o
Matura: CMakeFiles/Matura.dir/build.make
Matura: /usr/local/lib/libopencv_gapi.so.4.6.0
Matura: /usr/local/lib/libopencv_highgui.so.4.6.0
Matura: /usr/local/lib/libopencv_ml.so.4.6.0
Matura: /usr/local/lib/libopencv_objdetect.so.4.6.0
Matura: /usr/local/lib/libopencv_photo.so.4.6.0
Matura: /usr/local/lib/libopencv_stitching.so.4.6.0
Matura: /usr/local/lib/libopencv_video.so.4.6.0
Matura: /usr/local/lib/libopencv_videoio.so.4.6.0
Matura: /usr/lib/libSDL2main.a
Matura: /usr/lib/libSDL2-2.0.so.0.22.0
Matura: /usr/lib/libMagnumSdl2Application.a
Matura: /usr/lib/libMagnumGL.so
Matura: /usr/lib/libMagnum.so
Matura: /usr/lib/libMagnumMeshTools.so
Matura: /usr/lib/libMagnumSceneGraph.so
Matura: /usr/lib/libMagnumShaders.so
Matura: /usr/lib/libMagnumTrade.so
Matura: /usr/lib/magnum/importers/AnySceneImporter.so
Matura: /usr/lib/magnum/importers/GltfImporter.so
Matura: /usr/local/lib/libopencv_imgcodecs.so.4.6.0
Matura: /usr/local/lib/libopencv_dnn.so.4.6.0
Matura: /usr/local/lib/libopencv_calib3d.so.4.6.0
Matura: /usr/local/lib/libopencv_features2d.so.4.6.0
Matura: /usr/local/lib/libopencv_flann.so.4.6.0
Matura: /usr/local/lib/libopencv_imgproc.so.4.6.0
Matura: /usr/local/lib/libopencv_core.so.4.6.0
Matura: /usr/lib/libSDL2-2.0.so.0.22.0
Matura: /usr/lib/libGLX.so
Matura: /usr/lib/libMagnumGL.so
Matura: /usr/lib/libOpenGL.so
Matura: /usr/lib/magnum/importers/AnyImageImporter.so
Matura: /usr/lib/libMagnumTrade.so
Matura: /usr/lib/libMagnum.so
Matura: /usr/lib/libCorradePluginManager.so
Matura: /usr/lib/libCorradeUtility.so
Matura: /usr/lib/libCorradeUtility.so
Matura: CMakeFiles/Matura.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/run/media/elias/01D5A94471A88F00/GitHub/Matura/Build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Matura"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Matura.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Matura.dir/build: Matura
.PHONY : CMakeFiles/Matura.dir/build

CMakeFiles/Matura.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Matura.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Matura.dir/clean

CMakeFiles/Matura.dir/depend:
	cd /run/media/elias/01D5A94471A88F00/GitHub/Matura/Build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /run/media/elias/01D5A94471A88F00/GitHub/Matura /run/media/elias/01D5A94471A88F00/GitHub/Matura /run/media/elias/01D5A94471A88F00/GitHub/Matura/Build /run/media/elias/01D5A94471A88F00/GitHub/Matura/Build /run/media/elias/01D5A94471A88F00/GitHub/Matura/Build/CMakeFiles/Matura.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Matura.dir/depend

