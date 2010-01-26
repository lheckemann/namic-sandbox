#
# Test for the Multi-Image Registration
#


#
# Define directory for output produced from the tests.
#
SET(MultiImageRegistration_TEST_OUTPUT_DIR "${MultiImageRegistration_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR})

SET(MultiImageRegistration_TEST_INPUT_DIR "${MultiImageRegistration_SOURCE_DIR}/Testing/Data")

CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/README.txt
)

#
MACRO(MULTIIMAGEREGISTRATION INPUTIMAGE NUMBEROFIMAGES TRANSFORMTYPE  DATAID)
  ADD_TEST(CreateRandImages${DATAID} ${EXECUTABLE_OUTPUT_PATH}/CreateImageSet${TRANSFORMTYPE}
    ${INPUTIMAGE}
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData${DATAID}
    ${NUMBEROFIMAGES} on
    )
  # Create directory for registration
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID})
  # generate filenames for transform-based registration
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames${DATAID}.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/filenames${DATAID}.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters${TRANSFORMTYPE}.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/parameters${DATAID}.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README${DATAID}.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/README.txt
  )
  # Run groupwise registration
  ADD_TEST(RegistrationTest${DATAID} ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/filenames${DATAID}.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/parameters${DATAID}.txt
    )
  SET_TESTS_PROPERTIES(RegistrationTest${DATAID} PROPERTIES DEPENDS CreateRandImages${DATAID})
  # Create output files
  ADD_TEST(WriteOutputsTest${DATAID} ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/filenames${DATAID}.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/parameters${DATAID}.txt
    )
  SET_TESTS_PROPERTIES(WriteOutputsTest${DATAID} PROPERTIES DEPENDS RegistrationTest${DATAID})
  ADD_TEST(CompareDeformation${DATAID} ${EXECUTABLE_OUTPUT_PATH}/CompareDeformationFields
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/filenames${DATAID}.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration${DATAID}/parameters${DATAID}.txt
    )
  SET_TESTS_PROPERTIES(CompareDeformation${DATAID} PROPERTIES DEPENDS WriteOutputsTest${DATAID} )
ENDMACRO(MULTIIMAGEREGISTRATION)

#
#
#  Find the BrainWeb image datasets to use them as input
#
FIND_PATH(BrainWeb_DATA_ROOT NAMES brainweb165a10f17.mha  DOC "http://public.kitware.com/pub/itk/Data/BrainWeb/BrainPart1.tgz")


  SET(T1Processing ON)
  SET(T2Processing OFF)
  SET(PDProcessing OFF)

  ## Included locally
  SET(T1Data ${MultiImageRegistration_TEST_INPUT_DIR}/T1.nii.gz)
  SET(T2Data ${MultiImageRegistration_TEST_INPUT_DIR}/T2.nii.gz)
  SET(PDData ${MultiImageRegistration_TEST_INPUT_DIR}/PD.nii.gz)

IF(BrainWeb_DATA_ROOT)
  ## From BrainWeb
  #SET(T1Data ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha)
  #SET(T2Data ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha)
  #SET(PDData ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha)
ENDIF(BrainWeb_DATA_ROOT)

  SET(TestingMaxData 3)

IF(T1Processing)
  #  Affine test with 2 input images for BrainPart2.tgz
  MULTIIMAGEREGISTRATION(${T1Data} 2 Affine 002)
  #  B-spline test with 2 images for BrainPart2.tgz
  MULTIIMAGEREGISTRATION(${T1Data} 2 Bspline 005)
  IF(${TestingMaxData} GREATER 10)
    #  Affine test with 10 input images for BrainPart2.tgz
    MULTIIMAGEREGISTRATION(${T1Data} 10 Affine 008)
    #  B-spline test with 10 images for BrainPart2.tgz
    MULTIIMAGEREGISTRATION(${T1Data} 10 Bspline 011)
  ENDIF(${TestingMaxData} GREATER 10)
  IF(${TestingMaxData} GREATER 30)
    #  Affine test with 30 input images for BrainPart2.tgz
    MULTIIMAGEREGISTRATION(${T1Data} 30 Affine 014)
    #  B-spline test with 30 images for BrainPart2.tgz
    MULTIIMAGEREGISTRATION(${T1Data} 30 Bspline 017)
  ENDIF(${TestingMaxData} GREATER 30)
ENDIF(T1Processing)
IF(T2Processing)
  #  Affine test with 1 input images for BrainPart1.tgz
  MULTIIMAGEREGISTRATION(${T2Data} 2 Affine 001)
  #  B-spline test with 2 images for BrainPart1.tgz
  MULTIIMAGEREGISTRATION(${T2Data} 2 Bspline 004)
  IF(${TestingMaxData} GREATER 10)
    #  Affine test with 10 input images for BrainPart1.tgz
    MULTIIMAGEREGISTRATION(${T2Data} 10 Affine 007)
    #  B-spline test with 10 images for BrainPart1.tgz
    MULTIIMAGEREGISTRATION(${T2Data} 10 Bspline 010)
  ENDIF(${TestingMaxData} GREATER 10)
  IF(${TestingMaxData} GREATER 30)
    #  Affine test with 30 input images for BrainPart1.tgz
    MULTIIMAGEREGISTRATION(${T2Data} 30 Affine 013)
    #  B-spline test with 30 images for BrainPart1.tgz
    MULTIIMAGEREGISTRATION(${T2Data} 30 Bspline 016)
  ENDIF(${TestingMaxData} GREATER 30)
ENDIF(T2Processing)
IF(PDProcessing)
  #  Affine test with 2 input images for BrainPart3.tgz
  MULTIIMAGEREGISTRATION(${PDData} 2 Affine 003)
  #  B-spline test with 2 images for BrainPart3.tgz
  MULTIIMAGEREGISTRATION(${PDData} 2 Bspline 006)
  IF(${TestingMaxData} GREATER 10)
    #  Affine test with 10 input images for BrainPart3.tgz
    MULTIIMAGEREGISTRATION(${PDData} 10 Affine 009)
    #  B-spline test with 10 images for BrainPart3.tgz
    MULTIIMAGEREGISTRATION(${PDData} 10 Bspline 012)
  ENDIF(${TestingMaxData} GREATER 10)
  IF(${TestingMaxData} GREATER 30)
    #  Affine test with 30 input images for BrainPart3.tgz
    MULTIIMAGEREGISTRATION(${PDData} 30 Affine 015)
    #  B-spline test with 30 images for BrainPart3.tgz
    MULTIIMAGEREGISTRATION(${PDData} 30 Bspline 018)
  ENDIF(${TestingMaxData} GREATER 30)
ENDIF(PDProcessing)

#
#  Find the FAImage image datasets to use them as input
#
FIND_PATH(FAImage_DATA_ROOT NAMES FA.mhd DOC "Directory containing image from FAImage")

IF(FAImage_DATA_ROOT)

    #  Affine test with 2 input images for FA.mhd
    MULTIIMAGEREGISTRATION(${FAImage_DATA_ROOT}/FA.mhd 2 Affine 101)
    #  B-spline test with 2 images for FA.mhd
    MULTIIMAGEREGISTRATION(${FAImage_DATA_ROOT}/FA.mhd 2 Bspline 102)
  IF(${TestingMaxData} GREATER 10)
    #  Affine test with 10 input images for FA.mhd
    MULTIIMAGEREGISTRATION(${FAImage_DATA_ROOT}/FA.mhd 10 Affine 103)
    #  B-spline test with 10 images for FA.mhd
    MULTIIMAGEREGISTRATION(${FAImage_DATA_ROOT}/FA.mhd 10 Bspline 104)
  ENDIF(${TestingMaxData} GREATER 10)
  IF(${TestingMaxData} GREATER 30)
    #  Affine test with 30 input images for FA.mhd
    MULTIIMAGEREGISTRATION(${FAImage_DATA_ROOT}/FA.mhd 30 Affine 105)
    #  B-spline test with 30 images for FA.mhd
    MULTIIMAGEREGISTRATION(${FAImage_DATA_ROOT}/FA.mhd 30 Bspline 106)
  ENDIF(${TestingMaxData} GREATER 30)

ENDIF(FAImage_DATA_ROOT)
