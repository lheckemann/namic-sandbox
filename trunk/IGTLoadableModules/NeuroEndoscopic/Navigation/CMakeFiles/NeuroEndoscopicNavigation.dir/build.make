# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/neuroendo/Slicer3-lib/CMake-build/bin/cmake

# The command to remove a file.
RM = /home/neuroendo/Slicer3-lib/CMake-build/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /home/neuroendo/Slicer3-lib/CMake-build/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/neuroendo/IGTLoadableModules/NeuroEndoscopic

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/neuroendo/IGTLoadableModules/NeuroEndoscopic

# Include any dependencies generated for this target.
include CMakeFiles/NeuroEndoscopicNavigation.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/NeuroEndoscopicNavigation.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/NeuroEndoscopicNavigation.dir/flags.make

CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o: CMakeFiles/NeuroEndoscopicNavigation.dir/flags.make
CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o: NeuroEndoscopicNavigation.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /home/neuroendo/IGTLoadableModules/NeuroEndoscopic/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o -c /home/neuroendo/IGTLoadableModules/NeuroEndoscopic/NeuroEndoscopicNavigation.cxx

CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/neuroendo/IGTLoadableModules/NeuroEndoscopic/NeuroEndoscopicNavigation.cxx > CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.i

CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/neuroendo/IGTLoadableModules/NeuroEndoscopic/NeuroEndoscopicNavigation.cxx -o CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.s

CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.requires:
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.requires

CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.provides: CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.requires
	$(MAKE) -f CMakeFiles/NeuroEndoscopicNavigation.dir/build.make CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.provides.build
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.provides

CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.provides.build: CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.provides.build

# Object files for target NeuroEndoscopicNavigation
NeuroEndoscopicNavigation_OBJECTS = \
"CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o"

# External object files for target NeuroEndoscopicNavigation
NeuroEndoscopicNavigation_EXTERNAL_OBJECTS =

bin/NeuroEndoscopicNavigation: CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o
bin/NeuroEndoscopicNavigation: CMakeFiles/NeuroEndoscopicNavigation.dir/build.make
bin/NeuroEndoscopicNavigation: CMakeFiles/NeuroEndoscopicNavigation.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable bin/NeuroEndoscopicNavigation"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/NeuroEndoscopicNavigation.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/NeuroEndoscopicNavigation.dir/build: bin/NeuroEndoscopicNavigation
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/build

CMakeFiles/NeuroEndoscopicNavigation.dir/requires: CMakeFiles/NeuroEndoscopicNavigation.dir/NeuroEndoscopicNavigation.o.requires
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/requires

CMakeFiles/NeuroEndoscopicNavigation.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/NeuroEndoscopicNavigation.dir/cmake_clean.cmake
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/clean

CMakeFiles/NeuroEndoscopicNavigation.dir/depend:
	cd /home/neuroendo/IGTLoadableModules/NeuroEndoscopic && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/neuroendo/IGTLoadableModules/NeuroEndoscopic /home/neuroendo/IGTLoadableModules/NeuroEndoscopic /home/neuroendo/IGTLoadableModules/NeuroEndoscopic /home/neuroendo/IGTLoadableModules/NeuroEndoscopic /home/neuroendo/IGTLoadableModules/NeuroEndoscopic/CMakeFiles/NeuroEndoscopicNavigation.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/NeuroEndoscopicNavigation.dir/depend

