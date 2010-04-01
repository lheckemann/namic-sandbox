# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.4

#=============================================================================
# Special targets provided by cmake.

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

.SUFFIXES: .hpux_make_needs_suffix_list

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /software/slicer2/Lib/linux-x86_64/CMake-build/bin/cmake

# The command to remove a file.
RM = /software/slicer2/Lib/linux-x86_64/CMake-build/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /software/slicer2/Lib/linux-x86_64/CMake-build/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /software/slicer2/Modules/vtkRuleBasedSegmentation

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64

cxx/vtkITKBayesianClassificationImageFilterTcl.cxx: /software/slicer2/Lib/linux-x86_64/VTK-build/bin/vtkWrapTcl
cxx/vtkITKBayesianClassificationImageFilterTcl.cxx: /software/slicer2/Lib/linux-x86_64/VTK/Wrapping/hints
cxx/vtkITKBayesianClassificationImageFilterTcl.cxx: /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKBayesianClassificationImageFilter.h
	@echo "Generating vtkITKBayesianClassificationImageFilterTcl.cxx"
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && /software/slicer2/Lib/linux-x86_64/VTK-build/bin/vtkWrapTcl /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKBayesianClassificationImageFilter.h /software/slicer2/Lib/linux-x86_64/VTK/Wrapping/hints 1 /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx/vtkITKBayesianClassificationImageFilterTcl.cxx

cxx/vtkITKImageToImageFilterULULTcl.cxx: /software/slicer2/Lib/linux-x86_64/VTK-build/bin/vtkWrapTcl
cxx/vtkITKImageToImageFilterULULTcl.cxx: /software/slicer2/Lib/linux-x86_64/VTK/Wrapping/hints
cxx/vtkITKImageToImageFilterULULTcl.cxx: /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKImageToImageFilterULUL.h
	@echo "Generating vtkITKImageToImageFilterULULTcl.cxx"
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && /software/slicer2/Lib/linux-x86_64/VTK-build/bin/vtkWrapTcl /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKImageToImageFilterULUL.h /software/slicer2/Lib/linux-x86_64/VTK/Wrapping/hints 1 /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx/vtkITKImageToImageFilterULULTcl.cxx

# Include any dependencies generated for this target.
include cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/depend.make

# Include the compile flags for this target's objects.
include cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/flags.make

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/depend.make.mark: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/flags.make
cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/depend.make.mark: /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKBayesianClassificationImageFilter.cxx

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/flags.make
cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o: /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKBayesianClassificationImageFilter.cxx
	@echo "Building CXX object cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o"
	/usr/bin/g++   $(CXX_FLAGS) -o cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o -c /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/vtkITKBayesianClassificationImageFilter.cxx

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o.requires:

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o.provides: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o.requires
	$(MAKE) -f cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/build.make cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o.provides.build

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o.provides.build: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/depend: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/depend.make.mark

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/depend.make.mark:
	@echo "Scanning dependencies of target vtkRuleBasedSegmentation"
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64 && $(CMAKE_COMMAND) -E cmake_depends  "Unix Makefiles" /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64 /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/DependInfo.cmake

# Object files for target vtkRuleBasedSegmentation
vtkRuleBasedSegmentation_OBJECTS = \
"CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o"

# External object files for target vtkRuleBasedSegmentation
vtkRuleBasedSegmentation_EXTERNAL_OBJECTS =

bin/libvtkRuleBasedSegmentation.so: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o
bin/libvtkRuleBasedSegmentation.so: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/build.make
	@echo "Linking CXX shared library ../bin/libvtkRuleBasedSegmentation.so"
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && $(CMAKE_COMMAND) -P CMakeFiles/vtkRuleBasedSegmentation.dir/cmake_clean_target.cmake
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && /usr/bin/g++  -fPIC -O2   -Wno-deprecated  -ftemplate-depth-50   -shared -Wl,-soname,libvtkRuleBasedSegmentation.so -o ../bin/libvtkRuleBasedSegmentation.so $(vtkRuleBasedSegmentation_OBJECTS) $(vtkRuleBasedSegmentation_EXTERNAL_OBJECTS) -L/software/slicer2/Lib/linux-x86_64/VTK-build/bin -L/software/slicer2/Lib/linux-x86_64/Insight-build/bin -L/software/slicer2/Modules/vtkITK/builds/linux-x86_64/bin -L/usr/lib64 -L/usr/X11R6/lib64 -lITKAlgorithms -lITKNumerics -lITKCommon -lITKStatistics -lITKBasicFilters -lvtkITK -lvtkIO -lvtkGraphics -lvtkImaging -lvtkFiltering -lvtkCommon -lvtkHybrid -lvtkPatented -lvtkRendering -lITKCommon -litkvnl_inst -litkvnl_algo -litkvnl -litkvcl -litknetlib -lm -litksys -lpthread -ldl -lm -lvtkGraphics -lvtkImaging -lvtkIO -lvtkDICOMParser -lvtkpng -lvtktiff -lvtkzlib -lvtkjpeg -lvtkexpat -lvtkftgl -lvtkfreetype -lGLU -lGL -lSM -lICE -lSM -lICE -lX11 -lXext -lX11 -lXext -lXt -lSM -lICE -lX11 -lXext 

# Convenience name for target.
cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/build: bin/libvtkRuleBasedSegmentation.so

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/requires: cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/vtkITKBayesianClassificationImageFilter.o.requires

cxx/CMakeFiles/vtkRuleBasedSegmentation.dir/clean:
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && $(CMAKE_COMMAND) -P CMakeFiles/vtkRuleBasedSegmentation.dir/cmake_clean.cmake
