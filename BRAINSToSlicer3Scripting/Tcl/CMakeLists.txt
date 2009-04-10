# Build a testing suite for Slicer-3
#
PROJECT(B2COMMANDTEST)


MESSAGE(STATUS "Configuring Slicer Tests")

#SET(SLICER3_COMMAND_INVOCATION "--no-modules --no-splash -f")

SET(COMMON_INCLUDE_SCRIPT ${BRAINS_SLICER3_TEST_SUITE_SOURCE_DIR}/Common/BRAINSWrappers.tcl )
SET(COMMON_DATA_DIR ${BRAINS_SLICER3_TEST_SUITE_SOURCE_DIR}/Data )

##  SET(SLICER3_EXEC_real ${EXECUTABLE_OUTPUT_PATH}/${BRAINS_REAL_EXEC})
##  SET(SLICER3_EXEC ${EXECUTABLE_OUTPUT_PATH}/brains2)
SET(BRAINS_LIBDIR_NOT_USED "NOT_USED" )

SET(ALL_TEST_LIST GaussianSmoothAndWrite InvalidFileNameRead ReadNiftiFile )

IF(BRAINS_TESTING_DATA)
   SET(BRAINS_DATA_TEST_LIST ReadManyImages)
   SET(ALL_TEST_LIST ${ALL_TEST_LIST} ${BRAINS_DATA_TEST_LIST})

SET(BATCH_TEST_LIST
  getDimsResImage
  getDimsResMask
  loadMask
  loadImage-BadParameters           loadImage-GEDICOM                 loadImageAnalyzeT1                loadImageNIfTIFileTypes           loadImageNIfTITissueClassified
  loadImage-GE4X                    loadImage-PetFlow                 loadImageAnalyzeT1Strict          loadImageNIfTIFormats             loadImageTrimodal
  loadImage-GE5X                    loadImage-minc                    loadImageAnalyzeT2                loadImageNIfTIT1                  loadImageVarianEPI
  loadImage-GEADW                   loadImageAnalyzeFileTypes         loadImageAnalyzeTissueClassified  loadImageNIfTIT1Strict            loadImageVarianT1
  loadImage-GEDICOM-ALTFILTERS      loadImageAnalyzeFormats           loadImageBrainsResampled          loadImageNIfTIT2                  loadImageVarianT2FSE
)
#SET(FUTURE_TESTS  loadImageNIfTIT1Strict loadImageNIfTITissueClassified loadImageNIfTIT2 loadImageNIfTIT1 loadImageNIfTIFormats loadImageNIfTIFileTypes convertTalairachMask measureStudentT setStandardSize setDebugLevel createTableMask getLandmarkLocation loadImageAnalyzeT1Strict loadImageAnalyzeTissueClassified loadImageAnalyzeT2 loadImageAnalyzeT1 loadImageAnalyzeFormats loadImageAnalyzeFileTypes loadTissueClass getLandmarkNames getDimsResTalairach getHistogramData getPaletteType loadTable getDimsResTransform loadPalette checkCloseROI getTissueClassKappa loadTalairachBox hanning-filter2D-pet loadHistogram convertTalairachParameters averageLandmarks loadImage-PetFlow loadImage-BadParameters getTransformType getPaletteData savePalette saveTalairachParameters loadLandmarks loadTalairachParameters getSetThreshold loadTransform saveHistogram loadImageBrainsResampled getDimsResRoi saveTalairachBox getSetTransform saveLandmark convertLandmarksToLandmarks createBulletImage statImages createROIHull talairachEffectSize booleanMasks computeWbbf createHistogram cleanMask saveTable fillBrainSegMask measureVolume b2alignlinearRegistration loadImageTrimodal loadImage-GE4X loadImage-GEADW getTableData loadRoi convertRoiToMask saveMask getName getId loadImageVarianT1 clipImage createWitelsonRois createBoxedImage erodeSurfaceMask talairachBoxPartition saveRoi loadImageVarianT2FSE measureClassVolume thresholdImage createHemRoi convertMaskToRoi loadImage-GE5X convertClassImageToDiscrete thinROI measureImageVolume createBoundedImage divideImages maxImages mutualInformationRegistration createEnhancedImages minImages itkFilters equalImages subtractConstantImages sumImages resampleMask subtractImages resampleImage saveTissueClass extrudeROI saveTransform multiplyImages convertRGBImageToHSI loadImage-GEDICOM generateTissueClassify codeImageAndMasks loadImageVarianEPI fitInterleaved worsleyTraditionalAnalysis saveImage )

##Generate the scripts for all cases, even if not used, this allows out of
##ctest running of the scripts.
SET(SHELL_BIN /bin/bash)
FOREACH(SCRIPT_TO_GENERATE ${BATCH_TEST_LIST} )
  #  MESSAGE(STATUS "Running:  ${SHELL_BIN} ${B2COMMANDTEST_SOURCE_DIR}/regressionmodule/brains_ctest_wrapper.sh ${SLICER3_EXEC} ${BRAINS_LIBDIR_NOT_USED} ${B2COMMANDTEST_SOURCE_DIR}/regressionmodule  ${B2COMMANDTEST_BINARY_DIR}/Testing/results ${BrainsRegressionData_DATA_PATH} ${SCRIPT_TO_GENERATE}")
  ADD_CUSTOM_COMMAND(OUTPUT ${B2COMMANDTEST_BINARY_DIR}/Testing/results/${SCRIPT_TO_GENERATE}_wrapper.b2
                   PRE_BUILD
                   COMMAND ${SHELL_BIN} ${B2COMMANDTEST_SOURCE_DIR}/regressionmodule/brains_ctest_wrapper.sh
                   ARGS  ${SLICER3_EXEC} ${BRAINS_LIBDIR_NOT_USED} ${B2COMMANDTEST_SOURCE_DIR}/regressionmodule
                   ${B2COMMANDTEST_BINARY_DIR}/Testing/results ${BrainsRegressionData_DATA_PATH} ${SCRIPT_TO_GENERATE}
                   COMMENT Generating stub  file for testing ${B2COMMANDTEST_BINARY_DIR}/Testing/results/${SCRIPT_TO_GENERATE}_wrapper.b2
                   DEPENDS ${B2COMMANDTEST_SOURCE_DIR}/regressionmodule/brains_ctest_wrapper.sh ${B2COMMANDTEST_SOURCE_DIR}/regressionmodule/module/${SCRIPT_TO_GENERATE}.tcl )
  SET(GENERATED_FILES_LIST ${GENERATED_FILES_LIST} ${B2COMMANDTEST_BINARY_DIR}/Testing/results/${SCRIPT_TO_GENERATE}_wrapper.b2 )
ENDFOREACH(SCRIPT_TO_GENERATE)
ADD_CUSTOM_TARGET(test_wrapper_scripts ALL DEPENDS ${GENERATED_FILES_LIST} )

SET(SLICER_RUN_MODE "-f")  ## No   gui
SET(SLICER_RUN_MODE "-p")  ## With gui
FOREACH(MODULE_TO_RUN ${BATCH_TEST_LIST} )
#  MESSAGE(STATUS "Adding Test: ${MODULE_TO_RUN}_test ${SLICER3_EXEC} ${SLICER3_COMMAND_INVOCATION} ${B2COMMANDTEST_BINARY_DIR}/Testing/results/${MODULE_TO_RUN}_wrapper.b2")
#  ADD_TEST(${MODULE_TO_RUN}_test ${SLICER3_EXEC} --no-modules --no-splash ${SLICER_RUN_MODE} ${B2COMMANDTEST_BINARY_DIR}/Testing/results/${MODULE_TO_RUN}_wrapper.b2 )
  ADD_TEST(${MODULE_TO_RUN}_test ${SLICER3_EXEC} /ipldev/scratch/hjohnson/src/Slicer3-build/Slicer3 --test-mode --script ${B2COMMANDTEST_BINARY_DIR}/Testing/results/${MODULE_TO_RUN}_wrapper.b2 )
ENDFOREACH(MODULE_TO_RUN)

ENDIF(BRAINS_TESTING_DATA)

FOREACH(SCRIPT_TO_GENERATE ${ALL_TEST_LIST} )
  CONFIGURE_FILE(${B2COMMANDTEST_SOURCE_DIR}/${SCRIPT_TO_GENERATE}.tcl.in ${EXECUTABLE_OUTPUT_PATH}/${SCRIPT_TO_GENERATE}.tcl IMMEDIATE @ONLY )
  ADD_TEST(${SCRIPT_TO_GENERATE}_test  ${SLICER3_EXEC} --no-modules --no-splash ${SLICER_RUN_MODE} ${EXECUTABLE_OUTPUT_PATH}/${SCRIPT_TO_GENERATE}.tcl )
ENDFOREACH(SCRIPT_TO_GENERATE)

