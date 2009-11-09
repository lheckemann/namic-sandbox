
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

ADD_TEST(SigmoidDual_${DATASET2}_${DATASET1}
  ${CXX_TEST_PATH}/TwoChannelGradientMagnitudeSigmoidFilter
  ${KNEE_DATA_DIR}/3TPDImages/${DATASET1}.mhd
  ${TEMP}/${DATASET2}_resampled_to_${DATASET1}.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_GradientMagnitude.mhd
  ${TEMP}/${DATASET2}_${DATASET1}_Sigmoid.mhd
   0.7 # Sigma in mm
  -1.0 # Alpha
   75.0 # Beta
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

