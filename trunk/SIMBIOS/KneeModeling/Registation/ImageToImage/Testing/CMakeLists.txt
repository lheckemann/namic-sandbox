
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)


FIND_PATH(KNEE_DATA_DIR README.txt)

SET(SEEDS_DATA_DIR ${PROJECT_SOURCE_DIR}/Testing/Data)

MACRO(PREPAREMASK DATASETID )

ADD_TEST(ShapeDetection${DATASETID}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/Knees/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Mask.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   5.0 # Beta
  25.0 # Initial radius in mm
  ${SEEDS_DATA_DIR}/${DATASETID}_Seeds.txt
  )
 
ADD_TEST(Masking${DATASETID}
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${KNEE_DATA_DIR}/Knees/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Mask.mhd
  ${TEMP}/${DATASETID}_Masked.mhd
  ) 

ENDMACRO(PREPAREMASK)


MACRO(PREPAREMASK3T DATASETID )

ADD_TEST(ShapeDetection${DATASETID}_3T
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/${DATASETID}_3T_MRI.mhd
  ${TEMP}/${DATASETID}_3T_MRI_Mask.mhd
     0.7 # Sigma in mm
    -1.0 # Alpha
  1500.0 # Beta
    10.0 # Initial radius in mm
   400.0 # Maximum number of iterations
  ${SEEDS_DATA_DIR}/${DATASETID}_3T_MRI_Seeds.txt
  )
 
ADD_TEST(Masking${DATASETID}_3T
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${KNEE_DATA_DIR}/${DATASETID}_3T_MRI.mhd
  ${TEMP}/${DATASETID}_3T_MRI_Mask.mhd
  ${TEMP}/${DATASETID}_3T_MRI_Masked.mhd
  ) 

ENDMACRO(PREPAREMASK3T)


MACRO(PREPAREMASK3TPD DATASETID )

ADD_TEST(ShapeDetection${DATASETID}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetFilter
  ${KNEE_DATA_DIR}/3TPDImages/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Mask.mhd
  ${TEMP}/${DATASETID}_Sigmoid.mhd
     0.6 # Sigma in mm
    -1.0 # Alpha
    50.0 # Beta
    10.0 # Initial radius in mm
     1.0 # curvature weight
    80.0 # propagation weight
   200.0 # Maximum number of iterations
  ${SEEDS_DATA_DIR}/${DATASETID}_Seeds.txt
  )
  
ADD_TEST(ShapeDetectionHoleFilling${DATASETID}
  ${CXX_TEST_PATH}/HoleFillingFilter
  ${TEMP}/${DATASETID}_Mask.mhd
  ${TEMP}/${DATASETID}_Filled_Mask.mhd
     2 # Manhattan Radius
     2 # Majority
   400 # Maximum number of iterations
  )

ADD_TEST(ShapeDetectionRefinement${DATASETID}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetRefinementFilter
  ${TEMP}/${DATASETID}_Sigmoid.mhd
  ${TEMP}/${DATASETID}_Filled_Mask.mhd
  ${TEMP}/${DATASETID}_Refined_Mask.mhd
    10.0 # curvature weight
    10.0 # propagation weight
   100.0 # Maximum number of iterations
  )
 
ADD_TEST(InvertIntensity${DATASETID}
  ${CXX_TEST_PATH}/InvertIntensityImageFilter
  ${TEMP}/${DATASETID}_Refined_Mask.mhd
  ${TEMP}/${DATASETID}_Refined_Inverted_Mask.mhd
  )

ADD_TEST(ThresholdRefinement${DATASETID}
  ${CXX_TEST_PATH}/ThresholdSegmentationLevelSetImageFilter
  ${KNEE_DATA_DIR}/3TPDImages/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Refined_Mask.mhd
  ${TEMP}/${DATASETID}_Refined_UnLeaked_Mask.mhd
     100.0 # lower threshold
    3000.0 # upper threshold
     10.0 # curvature weight
  )
 
ADD_TEST(Masking${DATASETID}
  ${CXX_TEST_PATH}/MaskImageForRegistration
  ${KNEE_DATA_DIR}/3TPDImages/${DATASETID}.mhd
  ${TEMP}/${DATASETID}_Mask.mhd
  ${TEMP}/${DATASETID}_Masked.mhd
  ) 

ENDMACRO(PREPAREMASK3TPD)

MACRO(RESAMPLE_IMAGE DATASET1 DATASET2 )

ADD_TEST(Resample_${DATASET1}_${DATASET2}
  ${CXX_TEST_PATH}/ResampleImageFilter
  ${KNEE_DATA_DIR}/3TPDImages/${DATASET1}.mhd
  ${KNEE_DATA_DIR}/3TImages/${DATASET2}.mhd
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  )
 
ENDMACRO(RESAMPLE_IMAGE)


MACRO(REFINE_SEGMENTATION DATASET1 DATASET2 )

ADD_TEST(Sigmoid_${DATASET2}
  ${CXX_TEST_PATH}/GradientMagnitudeSigmoidFilter
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_Sigmoid.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   1000.0 # Beta
  )

ADD_TEST(TwoChannelSigmoid_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelGradientMagnitudeSigmoidFilter
  ${KNEE_DATA_DIR}/3TPDImages/${DATASET1}.mhd
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_GradientMagnitude.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Sigmoid.mhd
   0.3 # Sigma in mm
  -1.0 # Alpha
   200.0 # Beta
   0.953 # Weight Default value : 0.953
  )

ADD_TEST(TwoChannelCrop_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/CropImage
  ${TEMP}/${DATASET2}_${DATASET1}_Sigmoid.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_Sigmoid.mhd
  41  # startX
  46  # startY
   4  # startZ
  400 # sizeX
  400 # sizeY
  160 # sizeZ
  )



ADD_TEST(TwoChannelShapeDetection_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelShapeDetectionLevelSetFilter
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_Sigmoid.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_ShapeDetection.mhd
    25.0 # Initial radius in mm
     1.0 # curvature weight
    80.0 # propagation weight
   200.0 # Maximum number of iterations
   ${SEEDS_DATA_DIR}/temp_seeds.txt # Default ${SEEDS_DATA_DIR}/${DATASET1}_Seeds.txt
  )
  
ADD_TEST(TwoChannelShapeDetectionRefinement_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetRefinementFilter
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_Sigmoid.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_ShapeDetection.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_ShapeDetection_Refined.mhd
    10.0 # curvature weight
    10.0 # propagation weight
   100.0 # Maximum number of iterations
  )
 
ADD_TEST(TwoChannelHoleFilling${DATASETID}
  ${CXX_TEST_PATH}/HoleFillingFilter
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_ShapeDetection_Refined.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Cropped_ShapeDetection_Refined_Filled_Mask.mhd
     2 # Manhattan Radius
     2 # Majority
   400 # Maximum number of iterations
  )

ADD_TEST(Refinement_of_${DATASET1}_with_${DATASET2}
  ${CXX_TEST_PATH}/ShapeDetectionLevelSetRefinementFilter
  ${TEMP}/${DATASET2}_Sigmoid.mhd
  ${TEMP}/${DATASET1}_Filled_Mask.mhd
  ${TEMP}/${DATASET2}_Refined_Mask.mhd
   100.0 # curvature weight
     1.0 # propagation weight
   200.0 # Maximum number of iterations
  )

ADD_TEST(TwoChannelKnnAlgo_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelKnnAlgo
  ${KNEE_DATA_DIR}/3TPDImages/${DATASET1}.mhd
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  ${TEMP}/OutputKnn.mhd
  ${SEEDS_DATA_DIR}/Seeds1.txt
  ${SEEDS_DATA_DIR}/Seeds2.txt
  ${SEEDS_DATA_DIR}/Seeds3.txt
  ${SEEDS_DATA_DIR}/Seeds4.txt
  )

ADD_TEST(TwoChannelKnnAlgoRGB_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGB
  ${KNEE_DATA_DIR}/3TPDImages/${DATASET1}.mhd
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  ${TEMP}/OutputKnnRGB.mhd
  ${SEEDS_DATA_DIR}/Seeds1.txt
  ${SEEDS_DATA_DIR}/Seeds2.txt
  ${SEEDS_DATA_DIR}/Seeds3.txt
  ${SEEDS_DATA_DIR}/Seeds4.txt
  )

ADD_TEST(TwoChannelKnnAlgoInteractiveRGB2_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGB
  ${KNEE_DATA_DIR}/3TPDImages/${DATASET1}.mhd
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  ${TEMP}/OutputKnnRGB.mhd
  5 # Represents number of seed files
  ${SEEDS_DATA_DIR}/Background.txt
  ${SEEDS_DATA_DIR}/Fat.txt
  ${SEEDS_DATA_DIR}/Muscle.txt
  ${SEEDS_DATA_DIR}/Bones.txt
  ${SEEDS_DATA_DIR}/Cartilage.txt
  )

ADD_TEST(MattTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/OutputKnnRGBInteractive.mhd
  4 # Represents number of seed files
  /home/harishd/MattDemer/Background.txt
  /home/harishd/MattDemer/Femur.txt
  /home/harishd/MattDemer/Cartilage.txt
  /home/harishd/MattDemer/Fat.txt
  )

ADD_TEST(TwoChannelLabelMapKnnAlgo_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  4
  /home/harishd/MattDemer/Background.txt
  /home/harishd/MattDemer/Femur.txt
  /home/harishd/MattDemer/Cartilage.txt
  /home/harishd/MattDemer/Fat.txt
  )




ADD_TEST(MattViewTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoViewPixels_Knee
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  )

ADD_TEST(MattSDTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/OutputKnnRGBInteractive.mhd
  4 # Represents number of seed files
  /home/harishd/Background.txt
  /home/harishd/Femur.txt
  /home/harishd/Cartilage.txt
  /home/harishd/Fat.txt
  )

ADD_TEST(MattSlice100TwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/Slice100/I0100.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/Slice100/I0100.mhd
  /home/harishd/MattDemer/Slice100.mhd
  4 # Represents number of seed files
  /home/harishd/100_Background.txt
  /home/harishd/100_Femur.txt
  /home/harishd/100_Cartilage.txt
  /home/harishd/100_Fat.txt
  )

ADD_TEST(MattSD_OnlyBonesTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/OnlyBones.mhd
  4 # Represents number of seed files
  /home/harishd/MattDemer/Background.txt
  /home/harishd/MattDemer/Femur.txt
  /home/harishd/MattDemer/Cartilage.txt
  /home/harishd/MattDemer/Fat.txt
  )


ADD_TEST(HipTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_Hip
  /home/harishd/ChristieHip/PD-IDEAL-FSE-FAT/pd-ideal-fse-fat.mhd
  /home/harishd/ChristieHip/PD-IDEAL-FSE-WATER/pd-ideal-fse-water.mhd
  /home/harishd/ChristieHip/Output.mhd
  5 # Represents number of seed files
  /home/harishd/ChristieHip/Background.txt
  /home/harishd/ChristieHip/Bone1.txt
  /home/harishd/ChristieHip/Bone2.txt
  /home/harishd/ChristieHip/Muscle.txt
  /home/harishd/ChristieHip/Fat.txt
  )

ADD_TEST(HipAsTwoChannelKnnAlgoInteractiveRGB_spgrOutPhase_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_Hip
  /home/harishd/AsHip/02_AS/IDEAL-SPGR-OUTPHASE/gradientOutPhase.mhd
  /home/harishd/AsHip/02_AS/IDEAL-SPGR-WATER/gradientWater.mhd
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap.mhd
  3 # Represents number of seed files
  /home/harishd/AsHip/02_AS/Background.txt
  /home/harishd/AsHip/02_AS/Bone.txt
  /home/harishd/AsHip/02_AS/Muscle.txt
  )

ADD_TEST(HipAs91TwoChannelCreateLabelMap
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap_Hip_Slice
  /home/harishd/AsHip/02_AS/IDEAL-SPGR-OUTPHASE/Slice91/91.mhd
  /home/harishd/AsHip/02_AS/IDEAL-SPGR-WATER/Slice91/91.mhd
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap91.mhd
  3 # Represents number of seed files
  /home/harishd/AsHip/02_AS/Background.txt
  /home/harishd/AsHip/02_AS/Bone.txt
  /home/harishd/AsHip/02_AS/Muscle.txt
  )

ADD_TEST(HipAs91TwoChannelHoleFilling
  ${CXX_TEST_PATH}/HoleFillingFilter_Slice
  /home/harishd/AsHip/02_AS/ROI91.mhd
  /home/harishd/AsHip/02_AS/ROI91_Filled.mhd
     5 # Manhattan Radius
     5 # Majority
   400 # Maximum number of iterations
  )


ADD_TEST(HipAs91TwoChannelIterativeBinaryOpeningTest
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter_Hip_Slice
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap91.mhd
  /home/harishd/AsHip/02_AS/ConnectedOutput91_BridgeBreaker.mhd
  1
  428
  319
  406
  297
  )

ADD_TEST(HipAs91TwoChannelVotingIterativeBinaryImageTest
  ${CXX_TEST_PATH}/VotingBinaryIterativeHoleFillingImageFilter
  /home/harishd/AsHip/02_AS/ROI91.mhd
  /home/harishd/AsHip/02_AS/VotingOutputROI91.mhd
  2
  2
  400
  )

ADD_TEST(TwoChannelVotingIterativeBinaryImageTest_New
  ${CXX_TEST_PATH}/VotingBinaryIterativeHoleFillingImageFilter
  /home/harishd/MattDemer/BonesBinaryHoleFillingOutput.mhd
  /home/harishd/MattDemer/BonesBinaryVotingOutput.mhd
  1
  1
  10
  )


ADD_TEST(HipAsTwoChannelCreateLabelMap
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap_Hip
  /home/harishd/AsHip/02_AS/IDEAL-SPGR-OUTPHASE/gradientOutPhase.mhd
  /home/harishd/AsHip/02_AS/IDEAL-SPGR-WATER/gradientWater.mhd
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap.mhd
  3 # Represents number of seed files
  /home/harishd/AsHip/02_AS/Background.txt
  /home/harishd/AsHip/02_AS/Bone.txt
  /home/harishd/AsHip/02_AS/Muscle.txt
  )

ADD_TEST(HipAsTwoChannelConnectedImageFilterTestExtractIntensity80
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter_Hip
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap.mhd
  /home/harishd/AsHip/02_AS/ConnectedOutput80.mhd
  75
  85
  )

ADD_TEST(HipAsTwoChannelConnectedImageFilterTestExtractIntensity250
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter_Hip
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap.mhd
  /home/harishd/AsHip/02_AS/ConnectedOutput250.mhd
  245
  255
  )

ADD_TEST(HipAsTwoChannelExtractROIWrite
  ${CXX_TEST_PATH}/ImageROIWrite
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap91.mhd
  /home/harishd/AsHip/02_AS/ROI91.mhd
  50
  290
  150
  220
  )

ADD_TEST(HipAsTwoChannelConnectedImageFilterTestExtractIntensity200
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter_Hip
  /home/harishd/AsHip/02_AS/ROI91.mhd
  /home/harishd/AsHip/02_AS/ConnectedOutput200_91.mhd
  195
  205
  )

ADD_TEST(HipAsTwoChannel91BinaryOpeningFilterTest
  ${CXX_TEST_PATH}/TwoChannelBinaryOpeningFilter_Slice
  /home/harishd/AsHip/02_AS/ROI91.mhd
  /home/harishd/AsHip/02_AS/BinaryOpening_91.mhd
  195
  205
  )



ADD_TEST(HipAsTwoChannelSlice_AddLabelMap
  ${CXX_TEST_PATH}/TwoChannelLabelMapUnion_Slice
  /home/harishd/AsHip/02_AS/ROI91.mhd
  /home/harishd/AsHip/02_AS/ROI91_New.mhd
  /home/harishd/AsHip/02_AS/Seed_128.txt
  /home/harishd/AsHip/02_AS/Seed_200.txt
  )

ADD_TEST(HipAsTwoChannelConnectedImageFilterTestExtractIntensity10
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter_Hip
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap.mhd
  /home/harishd/AsHip/02_AS/ConnectedOutput10.mhd
  5
  15
  )


ADD_TEST(HipAsTwoChannelIterativeBinaryOpeningTest
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter_Hip
  /home/harishd/AsHip/02_AS/OutputKnnLabelMap.mhd
  /home/harishd/AsHip/02_AS/ConnectedOutput250_BridgeBreaker.mhd
  1
  428
  319
  88
  425
  302
  88
  )

ADD_TEST(HipSpgrTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_Hip_Spgr
  /home/harishd/ChristieHip/IDEAL-SPGR-OUTPHASE/ideal-spgr-outphase.mhd
  /home/harishd/ChristieHip/IDEAL-SPGR-WATER/ideal-spgr-water.mhd
  /home/harishd/ChristieHip/Output.mhd
  4 # Represents number of seed files
  /home/harishd/ChristieHip/Background_Spgr.txt
  /home/harishd/ChristieHip/Bone_Spgr.txt
  /home/harishd/ChristieHip/Muscle_Spgr.txt
  /home/harishd/ChristieHip/Fat_Spgr.txt
  )

ADD_TEST(HipViewTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoViewPixels_Hip
  /home/harishd/ChristieHip/PD-IDEAL-FSE-FAT/pd-ideal-fse-fat.mhd
  /home/harishd/ChristieHip/PD-IDEAL-FSE-WATER/pd-ideal-fse-water.mhd
  33
  )

ADD_TEST(HipSpgrViewTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoViewPixels_Hip
  /home/harishd/ChristieHip/IDEAL-SPGR-OUTPHASE/ideal-spgr-outphase.mhd
  /home/harishd/ChristieHip/IDEAL-SPGR-WATER/ideal-spgr-water.mhd
  100
  )

ADD_TEST(HipPrintTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_PrintPixelValues_SD
  /home/harishd/ChristieHip/PD-IDEAL-FSE-FAT/pd-ideal-fse-fat.mhd
  /home/harishd/ChristieHip/PD-IDEAL-FSE-WATER/pd-ideal-fse-water.mhd
  5 # Represents number of seed files
  /home/harishd/ChristieHip/Background.txt
  /home/harishd/ChristieHip/Bone1.txt
  /home/harishd/ChristieHip/Bone2.txt
  /home/harishd/ChristieHip/Muscle.txt
  /home/harishd/ChristieHip/Fat.txt
  )

ADD_TEST(HipSpgrPrintTwoChannelKnnAlgoInteractiveRGB_spgrFat_spgrWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_PrintPixelValues_SD
  /home/harishd/ChristieHip/IDEAL-SPGR-OUTPHASE/ideal-spgr-outphase.mhd
  /home/harishd/ChristieHip/IDEAL-SPGR-WATER/ideal-spgr-water.mhd
  4 # Represents number of seed files
  /home/harishd/ChristieHip/Background_Spgr.txt
  /home/harishd/ChristieHip/Bone_Spgr.txt
  /home/harishd/ChristieHip/Muscle_Spgr.txt
  /home/harishd/ChristieHip/Fat_Spgr.txt
  )



ADD_TEST(ConnectedThreshold_LabelMap_Bones
  ${CXX_TEST_PATH}/ConnectedThresholdImageFilter
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/BonesOnly.mhd
  211 # seedX
  178 # seedY
  71 # seedZ
  79
  81
  )

ADD_TEST(TwoChannelHoleFilling
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/OutputKnnLabelMap_Filled.mhd
     2 # Manhattan Radius
     2 # Majority
   400 # Maximum number of iterations
  )

ADD_TEST(TwoChannelMorphOpeningFilter1
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/gradientFatOutput.mhd
  /home/harishd/MattDemer/gradientFatMorphOpeningOutput.mhd
  1
  )

ADD_TEST(TwoChannelMorphOpeningFilter2
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/gradientWaterOutput.mhd
  /home/harishd/MattDemer/gradientWaterMorphOpeningOutput.mhd
  1
  )

ADD_TEST(NewTwoChannelMorphOpeningFilter
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/MorphOpeningOutput1.mhd
  2
  )

ADD_TEST(TwoChannelMorphBinaryOpeningFilter
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/BinaryOpeningOutput.mhd
  /home/harishd/MattDemer/MorphBinaryOpeningOutput.mhd
  2
  )

ADD_TEST(TwoChannelMorphClosingFilter
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalClosingImageFilter
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/MorphClosingOutput.mhd
  )

ADD_TEST(RelabelComponentImageFilterTest
  ${CXX_TEST_PATH}/RelabelComponentImageFilter
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/RelabelFilterOutput.mhd
  )



ADD_TEST(TwoChannelGrayScaleGetROI
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/OutputROI.mhd
  80
  )




ADD_TEST(OnlyBonesTwoChannelMorphOpeningFilter
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/OutputROI.mhd
  /home/harishd/MattDemer/MorphOpeningOutputBones.mhd
  )

ADD_TEST(TwoChannelSignedMaurerDistance
  ${CXX_TEST_PATH}/TwoChannelSignedMaurerDistanceMapImageFilter
  /home/harishd/MattDemer/OutputKnnLabelMap.mhd
  /home/harishd/MattDemer/MaurerOutput.mhd
  )

ADD_TEST(TwoChannelBonesSignedMaurerDistance
  ${CXX_TEST_PATH}/TwoChannelSignedMaurerDistanceMapImageFilter
  /home/harishd/MattDemer/BonesBinaryOpeningOutput.mhd
  /home/harishd/MattDemer/BonesMaurerOutput.mhd
  )

ADD_TEST(TwoChannelGradMorphOpeningFilter
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/GradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/GradientMorphOpeningOutput.mhd
  )
# The cycle for knee segmentation starts here
# The cycle for femur starts here #
ADD_TEST(TwoChannelGradientLabelMapKnnAlgo_Femur_Tibia
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap
#  /home/harishd/MattDemer/gradientFatOutput.mhd
#  /home/harishd/MattDemer/gradientWaterOutput.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/GradientOutputKnnLabelMap.mhd
  4
  /home/harishd/MattDemer/Background.txt
  /home/harishd/MattDemer/Femur.txt
  /home/harishd/MattDemer/Cartilage.txt
  /home/harishd/MattDemer/Fat.txt
  )

######## CYCLE FOR TIBIA BEGINS HERE ################################

ADD_TEST(TwoChannelCropImage_Tibia
  ${CXX_TEST_PATH}/CropImage
  /home/harishd/MattDemer/GradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Tibia.mhd
  156  # startX
  253  # startY
    0  # startZ
  235 # sizeX
  107 # sizeY
  156 # sizeZ
  )


ADD_TEST(TwoChannelGetTibia
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Tibia.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_TibiaOnly.mhd
  80
  )


ADD_TEST(TwoChannelIterativeBinaryOpeningTest1_Tibia
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_TibiaOnly.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Tibia.mhd
  1
  56
  38
  93
  50
  31
  93
  3
  )


ADD_TEST(TwoChannelIterativeBinaryOpeningTest2_Tibia
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Tibia.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Tibia2.mhd
  1
  23
  63
  63
  28
  60
  63
  1
  )

ADD_TEST(TwoChannelOrImagesTest_Tibia
  ${CXX_TEST_PATH}/TwoChannelOrImages
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Tibia2.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Tibia.mhd
  /home/harishd/MattDemer/TibiaOutputWithBackground.mhd
  10
  )

ADD_TEST(TwoChannelConnectedImageFilterTest_Tibia
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentFilter
  /home/harishd/MattDemer/TibiaOutputWithBackground.mhd
  /home/harishd/MattDemer/TibiaOutputWithBackground_Connected.mhd
  95
  86
  77
  )

ADD_TEST(TwoChannelConnectedImageFilterTest2_Tibia
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentFilter
  /home/harishd/MattDemer/TibiaOutputWithBackground_Connected.mhd
  /home/harishd/MattDemer/TibiaOutputWithBackground_Connected1.mhd
  95
  86
  77
  )


ADD_TEST(TwoChannelConvertToBinaryTest8_Tibia
  ${CXX_TEST_PATH}/TwoChannelConvertToBinaryImage
  /home/harishd/MattDemer/TibiaOutputWithBackground_Connected.mhd
  /home/harishd/MattDemer/TibiaOutputWithBackground_Binary.mhd
  )

ADD_TEST(TwoChannelHoleFillingTestNew9_Tibia
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/TibiaOutputWithBackground_Binary.mhd
  /home/harishd/MattDemer/TibiaOutputWithBackground_HoleFilled.mhd
     1 # Manhattan Radius
     1 # Majority
     20 # Maximum number of iterations
  )


ADD_TEST(TwoChannelHoleFillingTestNew10_Tibia
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/TibiaOutputWithBackground_HoleFilled.mhd
  /home/harishd/MattDemer/TibiaOutputWithBackground_HoleFilledNew.mhd
     2 # Manhattan Radius
     1 # Majority
     10 # Maximum number of iterations
  )

############ CYCLE FOR TIBIA ENDS HERE #########################

######## CYCLE FOR PATELLA STARTS HERE #######################

ADD_TEST(TwoChannelGradientLabelMapKnnAlgo_Patella
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap_Patella
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/GradientOutputKnnLabelMap_Patella.mhd
  4
  /home/harishd/MattDemer/Background.txt
  /home/harishd/MattDemer/Femur.txt
  /home/harishd/MattDemer/Cartilage.txt
  /home/harishd/MattDemer/Fat.txt
  )




ADD_TEST(TwoChannelCropImage_Patella
  ${CXX_TEST_PATH}/CropImage
  /home/harishd/MattDemer/GradientOutputKnnLabelMap_Patella.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Patella.mhd
  59  # startX
  86  # startY
   0  # startZ
  100 # sizeX
  160 # sizeY
  156 # sizeZ
  )


ADD_TEST(TwoChannelGetPatella
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Patella.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_PatellaOnly.mhd
  80
  )


ADD_TEST(TwoChannelIterativeBinaryOpeningTest1_Patella
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_PatellaOnly.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella.mhd
  1
  40
  66
  85
  88
  21
  85
  4
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest2_Patella
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella2.mhd
  1
  57
  27
  91
  62
  7
  91
  4
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest3_Patella
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella2.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella3.mhd
  1
   36
   86
   76
   50
  105
   76
  3
  )


ADD_TEST(TwoChannelIterativeBinaryOpeningTest4_Patella
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella3.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella4.mhd
  1
   30
   79
   77
   45
   106
   77
   4
  )


ADD_TEST(TwoChannelOrImagesTest_Patella
  ${CXX_TEST_PATH}/TwoChannelOrImages
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap_Patella4.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Patella.mhd
  /home/harishd/MattDemer/PatellaOutputWithBackground.mhd
  10
  )

ADD_TEST(TwoChannelConnectedImageFilterTest_Patella
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentFilter
  /home/harishd/MattDemer/PatellaOutputWithBackground.mhd
  /home/harishd/MattDemer/PatellaOutputWithBackground_Connected.mhd
  25
  72
  77
  )


ADD_TEST(TwoChannelConvertToBinaryTest8_Patella
  ${CXX_TEST_PATH}/TwoChannelConvertToBinaryImage
  /home/harishd/MattDemer/PatellaOutputWithBackground_Connected.mhd
  /home/harishd/MattDemer/PatellaOutputWithBackground_Binary.mhd
  )

ADD_TEST(TwoChannelHoleFillingTestNew9_Patella
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/PatellaOutputWithBackground_Binary.mhd
  /home/harishd/MattDemer/PatellaOutputWithBackground_HoleFilled.mhd
     1 # Manhattan Radius
     1 # Majority
     20 # Maximum number of iterations
  )



######### CYCLE FOR PATELLA ENDS HERE ###########################


ADD_TEST(TwoChannelConnectedImageFilterTest2
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentFilter
  /home/harishd/MattDemer/DilationHoleFillingOutput5.mhd
  /home/harishd/MattDemer/DilationHoleFillingConnectedOutput5.mhd
  95
  86
  77
  )


##################CYCLE FOR FEMUR STARTS HERE ###############

ADD_TEST(TwoChannelCropImage_Femur
  ${CXX_TEST_PATH}/CropImage
  /home/harishd/MattDemer/GradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap.mhd
  130  # startX
    7  # startY
    0  # startZ
  300 # sizeX
  280 # sizeY
  156 # sizeZ
  )

ADD_TEST(TwoChannelGetFemur
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Femur.mhd
  80
  )


ADD_TEST(TwoChannelIterativeBinaryOpeningTest1_Femur
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap_Femur.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap.mhd
  1
  74
  81
  90
  25
  106
  90
  2
  )


ADD_TEST(TwoChannelIterativeBinaryOpeningTest3_Femur
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap2.mhd
  1
  18
  146
  69
  21
  153
  69
  2
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest4_Femur
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/DilationOutput3.mhd
  /home/harishd/MattDemer/DilationOutput3_New.mhd
  1
  48
  125
  129
  60
  137
  129
  2
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest5_Femur
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/DilationOutput3_New.mhd
  /home/harishd/MattDemer/DilationOutput3_New1.mhd
  1
  60
  113
  128
  64
  122
  128
  2
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest6_Femur
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/DilationOutput3_New1.mhd
  /home/harishd/MattDemer/DilationOutput3_New2.mhd
  1
  69
  106
  128
  73
  116
  128
  2
  )

ADD_TEST(TwoChannelOrImagesTest_Femur
  ${CXX_TEST_PATH}/TwoChannelOrImages
#  /home/harishd/MattDemer/DilationOutput3_New2.mhd
  /home/harishd/MattDemer/BridgeBreakerCroppedGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/CroppedGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/DilationOutput3_New3.mhd
  10
  )


ADD_TEST(TwoChannelConnectedImageFilterTest7_Femur
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentFilter
  /home/harishd/MattDemer/DilationOutput3_New3.mhd
  /home/harishd/MattDemer/DilationOutput3_New4.mhd
  95
  86
  77
  )


ADD_TEST(TwoChannelConvertToBinaryTest8_Femur
  ${CXX_TEST_PATH}/TwoChannelConvertToBinaryImage
  /home/harishd/MattDemer/DilationOutput3_New4.mhd
  /home/harishd/MattDemer/DilationOutput3_New5.mhd
  )

ADD_TEST(TwoChannelHoleFillingTestNew9_Femur
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/DilationOutput3_New5.mhd
  /home/harishd/MattDemer/DilationOutput3_New6.mhd
     2 # Manhattan Radius
     1 # Majority
     100 # Maximum number of iterations
  )

###########################CYCLE ENDS HERE ##############################################

ADD_TEST(TwoChannelConnectedImageFilterTest2
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentFilter
  /home/harishd/MattDemer/DilationHoleFillingOutput5.mhd
  /home/harishd/MattDemer/DilationHoleFillingConnectedOutput5.mhd
  95
  86
  77
  )


ADD_TEST(TwoChannelBinaryOpening
  ${CXX_TEST_PATH}/TwoChannelBinaryOpeningFilter
  /home/harishd/MattDemer/GradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/BinaryOpeningOutput.mhd
  2
  )

ADD_TEST(TwoChannelBonesGrayScaleGetROI
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/MattDemer/BinaryOpeningOutput.mhd
  /home/harishd/MattDemer/BonesBinaryOpeningOutput.mhd
  80
  )

ADD_TEST(TwoChannelConnectedImageFilterTest
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter
  /home/harishd/MattDemer/BonesBinaryOpeningOutput.mhd
  /home/harishd/MattDemer/ConnectedOutput.mhd
  75
  255
  )

ADD_TEST(TwoChannelConnectedBinaryImageFilterTest
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter
  /home/harishd/MattDemer/BinaryOpeningOutput.mhd
  /home/harishd/MattDemer/ConnectedOutput1.mhd
  75
  255
  )

ADD_TEST(TwoChannelHoleFillingTestNew
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/BonesBinaryOpeningOutput.mhd
  /home/harishd/MattDemer/BonesBinaryHoleFillingOutput.mhd
     1 # Manhattan Radius
     1 # Majority
     1 # Maximum number of iterations
  )


ADD_TEST(TwoChannelHoleFillingTest
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/ConnectedOutput.mhd
  /home/harishd/MattDemer/HoleFillingOutput.mhd
     1 # Manhattan Radius
     1 # Majority
     5 # Maximum number of iterations
  )

ADD_TEST(TwoChannelBakIterativeBinaryOpeningTest
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilterBak
  /home/harishd/MattDemer/HoleFillingOutput.mhd
  /home/harishd/MattDemer/IterativeBinaryOpeningOutput.mhd
  1
  238
  84
  87
  149
  17
  87
  )
# The cycle ends here #

ADD_TEST(TwoChannelLabelMapKnnAlgo_JustFat
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap_JustFat
  /home/harishd/MattDemer/IDEAL-SPGR-Fat/ideal-spgr-fat.mhd
  /home/harishd/MattDemer/JustFatGradientOutputKnnLabelMap.mhd
  2
  /home/harishd/MattDemer/Background_JustFat.txt
  /home/harishd/MattDemer/Bone_JustFat.txt
  )

ADD_TEST(TwoChannelBinaryOpening_JustFat
  ${CXX_TEST_PATH}/TwoChannelBinaryOpeningFilter
  /home/harishd/MattDemer/JustFatGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/JustFatBinaryOpeningOutput.mhd
  10
  )

ADD_TEST(TwoChannelMorphOpeningFilter_JustFat
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalOpeningImageFilter
  /home/harishd/MattDemer/JustFatGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/JustFatMorphOpeningOutput.mhd
  1
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest_JustFat
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/JustFatGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/JustFatBridgeBreakerOutput.mhd
  1
  159
  175
  72
  143
  135
  72
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest_JustFat2
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/JustFatBridgeBreakerOutput.mhd
  /home/harishd/MattDemer/JustFatBridgeBreakerOutput1.mhd
  1
  256
  125
  77
  291
  104
  77
  )

ADD_TEST(TwoChannelMorphClosingFilter_JustFat
  ${CXX_TEST_PATH}/TwoChannelGrayscaleMorphologicalClosingImageFilter
  /home/harishd/MattDemer/JustFatGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/JustFatMorphClosingOutput.mhd
  )

ADD_TEST(TwoChannelHoleFilling_JustFat
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/JustFatGradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/JustFatHoleFilledOutput.mhd
     1 # Manhattan Radius
     1 # Majority
     1 # Maximum number of iterations
  )


ADD_TEST(TwoChannelLabelMapKnnAlgo_JustWater
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap_JustFat
  /home/harishd/MattDemer/IDEAL-SPGR-Water/ideal-spgr-water.mhd
  /home/harishd/MattDemer/JustWaterGradientOutputKnnLabelMap.mhd
  2
  /home/harishd/MattDemer/Background_JustFat.txt
  /home/harishd/MattDemer/Cartilage_JustFat.txt
  )



ADD_TEST(TwoChannelIterativeBinaryOpeningTest
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/HoleFillingOutput.mhd
  /home/harishd/MattDemer/IterativeBinaryOpeningOutput1.mhd
  1
  238
  84
  87
  149
  17
  87
  )

ADD_TEST(TwoChannelIterativeBinaryOpeningTest2
  ${CXX_TEST_PATH}/TwoChannelIterativeBinaryOpeningFilter
  /home/harishd/MattDemer/GradientOutputKnnLabelMap.mhd
  /home/harishd/MattDemer/IterativeBinaryOpeningOutput2.mhd
  1
  256
  176
  129
  256
  109
  140
  )


ADD_TEST(TwoChannelFinalHoleFilling
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/MattDemer/DilationOutput3.mhd
  /home/harishd/MattDemer/FinalOutput_Filled.mhd
     1 # Manhattan Radius
     1 # Majority
   400 # Maximum number of iterations
  )

ADD_TEST(TwoChannelHipGradientLabelMapKnnAlgo
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_CreateLabelMap
  /home/harishd/ChristieHip/gradientOutPhaseOutput.mhd
  /home/harishd/ChristieHip/gradientWaterOutput.mhd
  /home/harishd/ChristieHip/GradientOutputKnnLabelMap.mhd
  5
  /home/harishd/ChristieHip/Background.txt
  /home/harishd/ChristieHip/Bone1.txt
  /home/harishd/ChristieHip/Bone2.txt
  /home/harishd/ChristieHip/Muscle.txt
  /home/harishd/ChristieHip/Fat.txt
 )


ADD_TEST(TwoChannelHipBinaryOpening
  ${CXX_TEST_PATH}/TwoChannelBinaryOpeningFilter
  /home/harishd/ChristieHip/GradientOutputKnnLabelMap.mhd
  /home/harishd/ChristieHip/BinaryOpeningOutput.mhd
  2
 )

ADD_TEST(TwoChannelHipBonesGrayScaleGetROI
  ${CXX_TEST_PATH}/TwoChannelKnnAlgoRGBInteractive_SD_OnlyBones
  /home/harishd/ChristieHip/BinaryOpeningOutput.mhd
  /home/harishd/ChristieHip/BonesBinaryOpeningOutput.mhd
  80
 )

ADD_TEST(TwoChannelHipConnectedImageFilterTest
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter
  /home/harishd/ChristieHip/BonesBinaryOpeningOutput.mhd
  /home/harishd/ChristieHip/ConnectedOutput.mhd
  75
  255
 )

ADD_TEST(TwoChannelHipHoleFillingTest
  ${CXX_TEST_PATH}/HoleFillingFilter
  /home/harishd/ChristieHip/ConnectedOutput.mhd
  /home/harishd/ChristieHip/HoleFillingOutput.mhd
  1 # Manhattan Radius
  1 # Majority
  5 # Maximum number of iterations
 )

ADD_TEST(TwoChannelNewConnectedImageFilterTest
  ${CXX_TEST_PATH}/TwoChannelConnectedComponentImageFilter
  /home/harishd/MattDemer/BinaryOpeningOutput.mhd
  /home/harishd/MattDemer/ConnectedOutputNew.mhd
  75
  255
  )



ENDMACRO(REFINE_SEGMENTATION)


 
MACRO(DEFORM DATASETID )

ADD_TEST(Deform_${DATASETID}
  ${CXX_TEST_PATH}/BSplineWarping
  ${PROJECT_SOURCE_DIR}/Testing/BSplineDeformationCoefficients01.txt
  ${TEMP}/${DATASETID}_Masked.mhd
  ${TEMP}/${DATASETID}_Masked.mhd
  ${TEMP}/${DATASETID}_Masked_Deformed.mhd
  ${TEMP}/${DATASETID}_Masked_Deformed_DeformationField.mhd
  ) 

ENDMACRO(DEFORM)


MACRO(REGISTER DATASET1 DATASET2)

ADD_TEST(RegistrationRigid_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationRigid
  ${TEMP}/${DATASET1}_Masked.mhd
  ${TEMP}/${DATASET2}_Masked.mhd
  ${TEMP}/RegisteredRigid_${DATASET2}_${DATASET1}.mhd
  ${TEMP}/DifferenceAfterRigidRegistration.mhd
  ${TEMP}/RigidRegistrationScreenshots
  ${TEMP}/initialRigidTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalRigidTransform_${DATASET1}_${DATASET2}.txt
  )

ADD_TEST(RegistrationAffine_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/ImageRegistrationAffine
  ${TEMP}/${DATASET1}_Masked.mhd
  ${TEMP}/${DATASET2}_Masked.mhd
  ${TEMP}/finalRigidTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/RegisteredAffine_${DATASET2}_${DATASET1}.mhd
  ${TEMP}/DifferenceAfterAffineRegistration.mhd
  ${TEMP}/AffineRegistrationScreenshots
  ${TEMP}/initialAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  )

ADD_TEST(RegistrationBSpline_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/DeformableRegistration01
  ${TEMP}/${DATASET1}_Masked.mhd
  ${TEMP}/${DATASET2}_Masked.mhd
  ${TEMP}/${DATASET1}_Mask.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceAfter.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceBefore.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField.mhd
  1    # Use explicit PDF derivatives
  1    # Cache BSpline weights
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
   5   # Number of BSpline nodes in coarse grid
  10   # Number of BSpline nodes in coarse grid
  1.0  # Initial step length
  200  # Number of iterations
  )

ADD_TEST(BSplineResampleImage_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/BSplineResampleImage
  ${KNEE_DATA_DIR}/${DATASET1}.mhd
  ${KNEE_DATA_DIR}/${DATASET2}.mhd
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/${DATASET2}_RegisteredTo_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mhd
  )

ADD_TEST(EvaluateMetric_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/MeanSquaresImageMetric
  ${KNEE_DATA_DIR}/${DATASET1}.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mhd
  ${DATASET2}_To_${DATASET1} 
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_MeanSquaresMetric.txt
  )

ADD_TEST(ExtractSlice_${DATASET1}_${DATASET2}
  ${EXECUTABLE_OUTPUT_PATH}/ExtractSlice
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceAfter.mhd
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceAfter_Slice.png
  2  # Z orientation
  30 # Slice number
  )

ENDMACRO(REGISTER)

PREPAREMASK( 1 )
PREPAREMASK( 42 )
PREPAREMASK( 56 )
PREPAREMASK( 58 )
PREPAREMASK( 64 )

PREPAREMASK3T( 58 )

PREPAREMASK3TPD( SagittalPD-MRImages )

DEFORM( 42 )

REGISTER( 64 42 )

RESAMPLE_IMAGE( SagittalPD-MRImages 58_3T_MRI )

REFINE_SEGMENTATION( SagittalPD-MRImages 58_3T_MRI )

