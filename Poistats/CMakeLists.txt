PROJECT (Poistats)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
  INCLUDE_DIRECTORIES("${ITK_DIR}/Utilities/itkzlib")  
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# GenerateCLP is normally used via CMake where it is implemented as a
# CUSTOM_COMMAND. To use GenerateCLP from CMake include the following in your
# CMakeLists.txt file:
#INCLUDE(/home/djen/working/workspace/GenerateCLP/UseGenerateCLP.cmake)
INCLUDE(/space/heraclitus/1/users/dsjen/working/workspace/Slicer3/Libs/GenerateCLP/UseGenerateCLP.cmake)

# For each executable, include the following, replacing MyFilter with the name
# of your C++ source:
SET ( POISTATS_SOURCE 
  TensorReaderStrategy.cxx
  SymmetricTensorReaderStrategy.cxx
  AsymmetricTensorReaderStrategy.cxx
  AsymmetricTensorVectorReaderStrategy.cxx
  CommandUpdate.cxx
  PoistatsCLI.cxx
  itkPoistatsFilter.txx
  PoistatsReplicas.cxx
  PoistatsReplica.cxx
  PoistatsModel.cxx
  itkBSplineKernelFunction.txx
  itkBSplineScatteredDataPointSetToImageFilter.txx
  itkPointSetToImageFilter.txx
  itkMGHImageIO.cxx
  itkMGHImageIOFactory.cxx
)
GENERATECLP(POISTATS_SOURCE PoistatsCLI.xml)

# To generate a stand-alone executable add the lines:
ADD_EXECUTABLE ( poistats ${POISTATS_SOURCE})
TARGET_LINK_LIBRARIES ( poistats 
  ITKIO 
  ITKAlgorithms 
  ITKBasicFilters 
  ITKCommon
)

# To generate a pluggable library add the lines:
#ADD_LIBRARY(PoistatsCLILib SHARED ${POISTATS_SOURCE})
#SET_TARGET_PROPERTIES (PoistatsCLILib PROPERTIES COMPILE_FLAGS
#"-Dmain=SlicerModuleEntryPoint")
#TARGET_LINK_LIBRARIES (PoistatsCLILib ITKAlgorithms ITKIO ITKBasicFilters)

# enable "make test"
ENABLE_TESTING()
SUBDIRS(test)
