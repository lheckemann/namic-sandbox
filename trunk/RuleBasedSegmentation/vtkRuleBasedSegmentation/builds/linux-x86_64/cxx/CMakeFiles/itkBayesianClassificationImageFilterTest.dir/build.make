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
include cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/depend.make

# Include the compile flags for this target's objects.
include cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/flags.make

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/depend.make.mark: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/flags.make
cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/depend.make.mark: /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/itkBayesianClassificationImageFilterTest.cxx

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/flags.make
cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o: /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/itkBayesianClassificationImageFilterTest.cxx
	@echo "Building CXX object cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o"
	/usr/bin/g++   $(CXX_FLAGS) -o cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o -c /software/slicer2/Modules/vtkRuleBasedSegmentation/cxx/itkBayesianClassificationImageFilterTest.cxx

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o.requires:

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o.provides: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o.requires
	$(MAKE) -f cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/build.make cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o.provides.build

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o.provides.build: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/depend: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/depend.make.mark

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/depend.make.mark:
	@echo "Scanning dependencies of target itkBayesianClassificationImageFilterTest"
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64 && $(CMAKE_COMMAND) -E cmake_depends  "Unix Makefiles" /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64 /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/DependInfo.cmake

# Object files for target itkBayesianClassificationImageFilterTest
itkBayesianClassificationImageFilterTest_OBJECTS = \
"CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o"

# External object files for target itkBayesianClassificationImageFilterTest
itkBayesianClassificationImageFilterTest_EXTERNAL_OBJECTS =

bin/itkBayesianClassificationImageFilterTest: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o
bin/itkBayesianClassificationImageFilterTest: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/build.make
	@echo "Linking CXX executable ../bin/itkBayesianClassificationImageFilterTest"
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && $(CMAKE_COMMAND) -P CMakeFiles/itkBayesianClassificationImageFilterTest.dir/cmake_clean_target.cmake
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && /usr/bin/g++   -O2   -Wno-deprecated  -ftemplate-depth-50  -fPIC    $(itkBayesianClassificationImageFilterTest_OBJECTS) $(itkBayesianClassificationImageFilterTest_EXTERNAL_OBJECTS)  -o ../bin/itkBayesianClassificationImageFilterTest -rdynamic -L/software/slicer2/Lib/linux-x86_64/VTK-build/bin -L/software/slicer2/Lib/linux-x86_64/Insight-build/bin -lITKStatistics -lITKIO -lITKCommon -lITKNrrdIO -litkgdcm -litkjpeg12 -litkjpeg16 -litkpng -litktiff -litkjpeg8 -lITKSpatialObject -lITKNumerics -lITKCommon -lITKMetaIO -lITKDICOMParser -lITKEXPAT -lITKniftiio -lITKznz -litkzlib -litkvnl_inst -litkvnl_algo -litkvnl -litkvcl -litknetlib -lm -litksys -lpthread -ldl -lm 

# Convenience name for target.
cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/build: bin/itkBayesianClassificationImageFilterTest

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/requires: cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/itkBayesianClassificationImageFilterTest.o.requires

cxx/CMakeFiles/itkBayesianClassificationImageFilterTest.dir/clean:
	cd /software/RuleBasedSegmentation/vtkRuleBasedSegmentation/builds/linux-x86_64/cxx && $(CMAKE_COMMAND) -P CMakeFiles/itkBayesianClassificationImageFilterTest.dir/cmake_clean.cmake
