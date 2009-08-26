PROJECT(BRAINSFit)
cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)
#INCLUDE(CPack)

ENABLE_TESTING()
INCLUDE(Dart)

#-----------------------------------------------------------------------------
# Output directories.
#
## Only do this if it is a stand-alone build
IF( NOT BRAINS_BUILD )
IF( NOT BUILD_AGAINST_SLICER3 )
# Replace CMAKE_INSTALL_PREFIX to Currently used one,
# If it wasn't overridden from command line / cache.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/opt/BRAINSFit" CACHE PATH "Install path prefix,
     prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
OPTION(BRAINSFit_BUILD_LOCAL_CLP "If ON, then build self contained version that only depends on ITK ." ON)
MARK_AS_ADVANCED(BRAINSFit_BUILD_LOCAL_CLP)
ENDIF( NOT BUILD_AGAINST_SLICER3 )
ENDIF( NOT BRAINS_BUILD )

#------------------Setup slicer builds.
IF(BUILD_AGAINST_SLICER3)
  FIND_PACKAGE(Slicer3 REQUIRED)
  IF (Slicer3_FOUND)
    INCLUDE(${Slicer3_USE_FILE})
  ELSE (Slicer3_FOUND)
    MESSAGE(FATAL_ERROR "Cannot build without a Slicer3 build tree or a Slicer3 installation. Please set Slicer3_DIR.")
  ENDIF (Slicer3_FOUND)
  slicer3_set_default_install_prefix_for_external_projects()
ELSE(BUILD_AGAINST_SLICER3)
  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)
  IF(NOT SlicerExecutionModel_SOURCE_DIR)
    ##  In many cases sub-projects depending on SlicerExectuion Model
    ##  that can be built stand alone are combined in larger packages
    ##  This logic will include SlicerExectionModel only if it
    ##  has not already been built.
    ADD_SUBDIRECTORY(SlicerExecutionModel)
  ENDIF(NOT SlicerExecutionModel_SOURCE_DIR)
ENDIF(BUILD_AGAINST_SLICER3)

IF(BRAINS_BUILD)
  #OPTION(USE_DEBUG_IMAGE_VIEWER "Use the DEBUG_IMAGE_VIEWER for debugging" ON)
   OPTION(USE_DEBUG_IMAGE_VIEWER "Use the DEBUG_IMAGE_VIEWER for debugging" OFF) #HACK -- this does not yet work, so just turn it off
ELSE(BRAINS_BUILD)
OPTION(USE_DEBUG_IMAGE_VIEWER "Use the DEBUG_IMAGE_VIEWER for debugging" OFF)
ENDIF(BRAINS_BUILD)
MARK_AS_ADVANCED(USE_DEBUG_IMAGE_VIEWER)
IF( USE_DEBUG_IMAGE_VIEWER )
    IF(NOT KWWidgets_SOURCE_DIR)
      FIND_PACKAGE(KWWidgets REQUIRED)
      INCLUDE(${KWWidgets_USE_FILE})
    ENDIF(NOT KWWidgets_SOURCE_DIR)
   ADD_DEFINITIONS(-DUSE_DEBUG_IMAGE_VIEWER)
   FIND_PATH(DEBUG_IMAGE_VIEWER_INCLUDE_DIR DebugImageViewerClient.h ${CMAKE_INSTALL_PREFIX}/include)
   INCLUDE_DIRECTORIES(${DEBUG_IMAGE_VIEWER_INCLUDE_DIR})
ENDIF( USE_DEBUG_IMAGE_VIEWER )



ADD_SUBDIRECTORY(BRAINSFit_Common)
INCLUDE_DIRECTORIES(${BRAINSFit_SOURCE_DIR}/BRAINSFit_Common)

FIND_PACKAGE(GenerateCLP NO_MODULE REQUIRED)
IF(GenerateCLP_DIR)
  INCLUDE(${GenerateCLP_USE_FILE})
ELSE(GenerateCLP_DIR)
  MESSAGE(FATAL_ERROR "Can't build without GenerateCLP. Please set GenerateCLP_DIR")
ENDIF(GenerateCLP_DIR)

INCLUDE(${BRAINSFit_SOURCE_DIR}/IJMacros.txt)
SET(BRAINSFitPrimary_SRC BRAINSFitPrimary.cxx)
GENERATECLP(BRAINSFitPrimary_SRC BRAINSFitPrimary.xml)

CONFIGURE_FILE(${BRAINSFit_SOURCE_DIR}/CTestCustom.ctest ${BRAINSFit_BINARY_DIR}/CTestCustom.ctest COPYONLY)
ADD_LIBRARY(BRAINSFITCOMMONLIB ${BRAINSFitPrimary_SRC} CenterOfBrain.cxx)

#SET(DEBUGIMAGEVIEWERLIB DebugImageViewerClient)
IF(USE_DEBUG_IMAGE_VIEWER)
  #IF(NOT KWWidgets_SOURCE_DIR)
  #  FIND_PACKAGE(KWWidgets REQUIRED)
  #  INCLUDE(${KWWidgets_USE_FILE})
  #ENDIF(NOT KWWidgets_SOURCE_DIR)
  ADD_DEFINITIONS(-DUSE_DEBUG_IMAGE_VIEWER)
  TARGET_LINK_LIBRARIES(BRAINSFITCOMMONLIB ${KWWidgets_LIBRARIES})
ENDIF(USE_DEBUG_IMAGE_VIEWER)


TARGET_LINK_LIBRARIES(BRAINSFITCOMMONLIB ITKAlgorithms ITKStatistics ITKIO ITKBasicFilters ITKCommon ITKNumerics )


ADD_EXECUTABLE(BRAINSFit BRAINSFit.cxx )
TARGET_LINK_LIBRARIES(BRAINSFit BRAINSFITCOMMONLIB)

ADD_EXECUTABLE(BRAINSFitTest BRAINSFitTest.cxx)
TARGET_LINK_LIBRARIES(BRAINSFitTest BRAINSFITCOMMONLIB)

IF(BUILD_AGAINST_SLICER3)
  ADD_LIBRARY(BRAINSFitLib SHARED BRAINSFit.cxx)
  SET_TARGET_PROPERTIES (BRAINSFitLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
  TARGET_LINK_LIBRARIES (BRAINSFitLib BRAINSFITCOMMONLIB ITKNumerics ITKIO)
  slicer3_install_plugins(BRAINSFitLib)

  ADD_EXECUTABLE(BRAINSFitExe BRAINSFit.cxx BRAINSFitPrimary.cxx CenterOfBrain.cxx)
  TARGET_LINK_LIBRARIES (BRAINSFitExe ITKNumerics ITKIO ITKAlgorithms ITKStatistics ITKBasicFilters ITKCommon)
  slicer3_install_plugins(BRAINSFitExe)
ENDIF(BUILD_AGAINST_SLICER3)

# ##  ADD_EXECUTABLE(ApplyTransform ApplyTransform.cxx)
# ##  IF (ITK_USE_TRANSFORM_IO_FACTORIES)
# ##    TARGET_LINK_LIBRARIES(ApplyTransform ITKIO ITKTransformIOReview)
# ##  ELSE(ITK_USE_TRANSFORM_IO_FACTORIES)
# ##    TARGET_LINK_LIBRARIES(ApplyTransform ITKIO)
# ##  ENDIF(ITK_USE_TRANSFORM_IO_FACTORIES)
#
# The following program was used to generate TestData/test2.nii.gz
# from test.nii.gz
IF(MakeMakeXfrmImage)
  ADD_EXECUTABLE(makexfrmedImage makexfrmedImage.cxx)
  TARGET_LINK_LIBRARIES(makexfrmedImage ITKIO)
ENDIF(MakeMakeXfrmImage)

# The test driver is only needed to help generate new testcases
ADD_EXECUTABLE(BRAINSFitTestDriver BRAINSFitTestDriver.cxx)
TARGET_LINK_LIBRARIES(BRAINSFitTestDriver BRAINSFITCOMMONLIB)

##  Test validated both with and without OPTIMIZED REGISTRATION
##  Let's keep the tests in alphabetical order by TestName.

SET(BRAINSFitTestName BRAINSFitTest_AffineRotationMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 131072
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Affine
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/BRAINSFitTest_Initializer_RigidRotationNoMasks.mat
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_AffineRotationNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 131072
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Affine
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_AffineScaleMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Affine
  --initializeTransformMode MomentsOn
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_AffineScaleNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Affine
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_AffineTranslationMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Affine
  --initializeTransformMode MomentsOn
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/translation.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/translation.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_AffineTranslationNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Affine
  --initializeTransformMode MomentsOn
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/translation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)


SET(BRAINSFitTestName BRAINSFitTest_BSplineAnteScaleRotationRescaleHeadMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500,2500
  --numberOfHistogramBins 200
  --splineGridSize 7,5,6
  --numberOfSamples 144000
  --spatialScale 250
  --minimumStepSize 0.01,0.003
  --outputVolumePixelType short
  --maskProcessingMode ROIAUTO
  --initializeTransformMode CenterOfHead
  --transformType Rigid,ScaleVersor3D
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.rescale.rigid.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_BSplineOnlyRescaleHeadMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 1500
  --numberOfHistogramBins 200
  --splineGridSize 7,5,6
  --numberOfSamples 144000
  --spatialScale 250
  --minimumStepSize 0.01
  --outputVolumePixelType short
  --maskProcessingMode ROIAUTO
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_BRAINSFitTest_BSplineAnteScaleRotationRescaleHeadMasks.mat
  --transformType BSpline
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.rescale.rigid.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_BSplineScaleRotationRescaleHeadMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500,2500,500
  --numberOfHistogramBins 200
  --splineGridSize 7,5,6
  --numberOfSamples 144000
  --spatialScale 250
  --minimumStepSize 0.01,0.003,0.01
  --outputVolumePixelType short
  --maskProcessingMode ROIAUTO
  --initializeTransformMode CenterOfHead
  --transformType Rigid,ScaleVersor3D,BSpline
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.rescale.rigid.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)


SET(BRAINSFitTestName BRAINSFitTest_BSplineScaleRotationHistogramHeadMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500,2500,500
  --numberOfHistogramBins 50
  --numberOfMatchPoints 10
  --histogramMatch
  --splineGridSize 7,5,6
  --numberOfSamples 144000
  --spatialScale 250
  --minimumStepSize 0.01,0.003,0.01
  --outputVolumePixelType short
  --maskProcessingMode ROIAUTO
  --initializeTransformMode CenterOfHead
  --transformType Rigid,ScaleVersor3D,BSpline
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.rescale.rigid.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)


SET(BRAINSFitTestName BRAINSFitTest_RigidAnisotropicMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/ANON0006_20_T1_dbg_splayed.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/ANON0006_20_T1_sag_twisted.nii.gz
  --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0
  --minimumStepSize 0.001 --numberOfSamples 100000 --numberOfIterations 1500
  --numberOfHistogramBins 200
  --transformType Rigid --initializeTransformMode MomentsOn --spatialScale 1000
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  )

SET(BRAINSFitTestName BRAINSFitTest_RigidMedianRotationNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Rigid
  --initializeTransformMode MomentsOn
  --medianFilterSize 1,1,1
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_RigidRotGeomNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 144000
  --spatialScale 500
  --minimumStepSize 0.05,0.0003
  --outputVolumePixelType short
  --transformType Rigid,Rigid
  --initializeTransformMode GeometryOn
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.geom.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_RigidRotaRotaRotNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.05,0.001,0.0000316
  --outputVolumePixelType uchar
  --transformType Rigid,Rigid,Rigid
  --initializeTransformMode CenterOfHead
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_RigidRotationHeadMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Rigid
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --maskProcessingMode ROIAUTO
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_RigidRotationMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Rigid
  --initializeTransformMode MomentsOn
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_RigidRotationNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType Rigid
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleRotationRescaleHeadMasksNoInit)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500,2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.01,0.003
  --outputVolumePixelType uchar
  --maskProcessingMode ROIAUTO
  --initializeTransformMode CenterOfHead
  --transformType Rigid,ScaleVersor3D
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.rescale.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleSkewVersorRotationMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 11
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 131072
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleSkewVersor3D
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/BRAINSFitTest_Initializer_RigidRotationNoMasks.mat
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleSkewVersorRotationNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 11
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 131072
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleSkewVersor3D
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleSkewVersorScaleMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1200
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleSkewVersor3D
  --initializeTransformMode MomentsOn
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleSkewVersorScaleNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleSkewVersor3D
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleTranslationRescaleHeadMasksNoInit)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001,0.0001
  --outputVolumePixelType uchar
  --maskProcessingMode ROIAUTO
  --initializeTransformMode CenterOfHead
  --transformType Rigid,ScaleVersor3D
  --permitParameterVariation 0,0,0
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/translation.rescale.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleTranslationRescaleHeadMasksInit)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 7
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 777
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.0001
  --outputVolumePixelType uchar
  --maskProcessingMode ROIAUTO
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_BRAINSFitTest_TranslationRescaleHeadMasks.mat
  --transformType ScaleVersor3D
  --permitParameterVariation 0,0,0
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/translation.rescale.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleVersorRotationMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 11
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 131072
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleVersor3D
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/BRAINSFitTest_Initializer_RigidRotationNoMasks.mat
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleVersorRotationNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 11
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 131072
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleVersor3D
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/rotation.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleVersorScaleMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleVersor3D
  --initializeTransformMode MomentsOn
  --maskProcessingMode ROI
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --fixedBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.mask
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.nii.gz
  --movingBinaryVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.mask
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

SET(BRAINSFitTestName BRAINSFitTest_ScaleVersorScaleNoMasks)
ADD_TEST(${BRAINSFitTestName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/${BRAINSFitTestName}.result.nii.gz
            ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --compareIntensityTolerance 9
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 1000
  BRAINSFitTest
  --failureExitCode 0 --writeTransformOnFailure
  --numberOfIterations 2500
  --numberOfHistogramBins 200
  --numberOfSamples 72000
  --spatialScale 250
  --minimumStepSize 0.001
  --outputVolumePixelType uchar
  --transformType ScaleVersor3D
  --initialTransform ${BRAINSFit_SOURCE_DIR}/TestData/Initializer_0.05_${BRAINSFitTestName}.mat
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/test.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/scale.test.nii.gz
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.test.nii.gz
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/${BRAINSFitTestName}.mat
)

IF(Test_SignedDistanceData)
ADD_TEST(BRAINSFitTest_SignedDistanceData ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/BRAINSFitTest
  --compare ${BRAINSFit_SOURCE_DIR}/TestData/TEST_output.nii.gz ${BRAINSFit_BINARY_DIR}/Testing/TEST_output.nii.gz
  BRAINSFitTest
  --failureExitCode 1
  --fixedVolume ${BRAINSFit_SOURCE_DIR}/TestData/AtlasBrain_SignedDistance.nii.gz
  --movingVolume ${BRAINSFit_SOURCE_DIR}/TestData/Unoriented_RawBrain_SignedDistance.nii.gz
  --fixedVolumeTimeIndex 0 --movingVolumeTimeIndex 0
  --minimumStepSize 0.001 --numberOfSamples 75000 --numberOfIterations 1500
  --numberOfHistogramBins 200
  --transformType Rigid --initializeTransformMode MomentsOn --spatialScale 1000
  --outputTransform ${BRAINSFit_BINARY_DIR}/Testing/TEST.mat
  --outputVolume ${BRAINSFit_BINARY_DIR}/Testing/TEST_output.nii.gz
  )
ENDIF(Test_SignedDistanceData)

IF(BUILD_AGAINST_SLICER3)
slicer3_set_plugins_output_path(BRAINSFit)
ELSEIF(BUILD_AGAINST_SLICER3)
### The last section is concerned with installing the binaries and making distributions.
INSTALL(TARGETS BRAINSFit
                 RUNTIME DESTINATION bin
                 LIBRARY DESTINATION lib
                 ARCHIVE DESTINATION lib/static)
ENDIF(BUILD_AGAINST_SLICER3)

INCLUDE(InstallRequiredSystemLibraries)


SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "4")
SET(CPACK_PACKAGE_VERSION_PATCH "1")

SET(CPACK_PACKAGE_NAME "BRAINSFit")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-dev")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_NAME}-A program for medical image registration with mutual information metric.")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${BRAINSFit_SOURCE_DIR}/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${BRAINSFit_SOURCE_DIR}/Copyright.txt")

SET(CPACK_PACKAGE_DEFAULT_LOCATION "/opt/${CPACK_PACKAGE_NAME}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
SET(CPACK_PACKAGING_INSTALL_PREFIX "/")
SET(CPACK_SET_DESTDIR ON)

SET(CPACK_PACKAGE_VENDOR "NAMIC Tool Developed at The University of Iowa")

SET(CPACK_SOURCE_GENERATOR "TGZ;TZ")
#SET(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;\\.swp$;\\.#;/#;\\.*~")
SET(CPACK_SOURCE_IGNORE_FILES "")
SET(CPACK_PACKAGE_EXECUTABLES "BRAINSFit";"A program for registering medical images with mutual information.")

IF(WIN32 AND NOT UNIX)
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.nitrc.org")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.nitrc.org")
  SET(CPACK_NSIS_CONTACT "hans-johnson@uiowa.edu")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES OFF)
  SET(CPACK_SOURCE_STRIP_FILES OFF)
ENDIF(WIN32 AND NOT UNIX)

INCLUDE(CPack)
