#
# Test for the Multi-Image Registration
#


# 
# Define directory for output produced from the tests.
#
SET(MultiImageRegistration_TEST_OUTPUT_DIR "${MultiImageRegistration_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR})

SET(MultiImageRegistration_TEST_INPUT_DIR "${MultiImageRegistration_SOURCE_DIR}/Testing/Data")



#
#
#  Find the BrainWeb image datasets to use them as input
#
FIND_PATH(BrainWeb_DATA_ROOT NAMES brainweb165a10f17.mha DOC "Directory containing image from BrainWeb")


IF(BrainWeb_DATA_ROOT)



  #
  #  Affine test with two input images
  #
  ADD_TEST(CreateDeformedImage001 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine  
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineImages_2
    2
    )

  # Create directory for affine registration 001
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001)

  # generate filenames for affine
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames001.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/filenames001.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/parameters001.txt
  )

  # Run groupwise registration
  ADD_TEST(RegistrationTest001 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/filenames001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/parameters001.txt
    )
    
  # Create output files
  ADD_TEST(ComputeOutputs001 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/filenames001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/parameters001.txt
    )

  # Increase test times
  set_tests_properties(CreateDeformedImage001 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest001 PROPERTIES TIMEOUT 3600)
  set_tests_properties(ComputeOutputs001 PROPERTIES TIMEOUT 3600)

    
  #
  #  B-spline test with two images 
  #
  ADD_TEST(CreateDeformedImage002 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline  
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineImages_2
    2
    )

  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002)

  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames002.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/filenames002.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/parameters002.txt
  )

  ADD_TEST(RegistrationTest002 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/filenames002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/parameters002.txt
    )
    
  ADD_TEST(ComputeOutputs002 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/filenames002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/parameters002.txt
    )
  set_tests_properties(CreateDeformedImage002 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest002 PROPERTIES TIMEOUT 7200)
  set_tests_properties(ComputeOutputs002 PROPERTIES TIMEOUT 3600)


  #
  #  Affine test with ten input images
  #
  ADD_TEST(CreateDeformedImage003 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineImages_10
    10
    )

  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003)

  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames003.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/filenames003.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters003.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/parameters003.txt
  )

  ADD_TEST(RegistrationTest003 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/filenames003.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/parameters003.txt
    )
    
  ADD_TEST(ComputeOutputs003 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/filenames003.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/parameters003.txt
    )

  set_tests_properties(CreateDeformedImage003 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest003 PROPERTIES TIMEOUT 36000)
  set_tests_properties(ComputeOutputs003 PROPERTIES TIMEOUT 36000)

    
  #
  #  B-spline test with 10 images
  #
  ADD_TEST(CreateDeformedImage004 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineImages_10
    10
    )

  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004)

  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames004.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/filenames004.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters004.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/parameters004.txt
  )

  ADD_TEST(RegistrationTest004 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/filenames004.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/parameters004.txt
    )
    
  ADD_TEST(ComputeOutputs004 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/filenames004.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/parameters004.txt
    )
  set_tests_properties(CreateDeformedImage004 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest004 PROPERTIES TIMEOUT 72000)
  set_tests_properties(ComputeOutputs004 PROPERTIES TIMEOUT 36000)


  #
  #  Affine test with 30 input images
  #
  ADD_TEST(CreateDeformedImage005 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineImages_30
    30
    )

  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005)

  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames005.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/filenames005.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters005.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/parameters005.txt
  )

  ADD_TEST(RegistrationTest005 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/filenames005.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/parameters005.txt
    )
    
  ADD_TEST(ComputeOutputs005 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/filenames005.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/parameters005.txt
    )

  set_tests_properties(CreateDeformedImage003 PROPERTIES TIMEOUT 360000)
  set_tests_properties(RegistrationTest003 PROPERTIES TIMEOUT 360000)
  set_tests_properties(ComputeOutputs003 PROPERTIES TIMEOUT 360000)

    
  #
  #  B-spline test with 30 images
  #
  ADD_TEST(CreateDeformedImage006 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineImages_30
    30
    )

  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006)

  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames006.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/filenames006.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters006.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/parameters006.txt
  )

  ADD_TEST(RegistrationTest006 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/filenames006.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/parameters006.txt
    )
    
  ADD_TEST(ComputeOutputs006 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/filenames006.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/parameters006.txt
    )
  set_tests_properties(CreateDeformedImage004 PROPERTIES TIMEOUT 360000)
  set_tests_properties(RegistrationTest004 PROPERTIES TIMEOUT 720000)
  set_tests_properties(ComputeOutputs004 PROPERTIES TIMEOUT 360000)
ENDIF(BrainWeb_DATA_ROOT)
