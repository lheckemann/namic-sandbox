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
  #  First, take an input image and generate many instances of artificial deformations.
  #
  ADD_TEST(CreateDeformedImage001 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine  
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineSyntheticSet
    2
    )

  # Create directory for affine registration 001
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001)

  # generate filenames for affine
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenamesExperiment001.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001/filenamesExperiment001.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001/parameters001.txt
  )

  # Run groupwise registration
  ADD_TEST(RegistrationTest001 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001/filenamesExperiment001.txt  
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001/parameters001.txt  
    )
    
  # Run groupwise registration
  ADD_TEST(ComputeOutputs001 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001/filenamesExperiment001.txt  
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/AffineRegistration001/parameters001.txt  
    )
    
    
  #
  #  B-spline test
  #
  ADD_TEST(CreateDeformedImage002 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline  
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineSyntheticSet
    2
    )

  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001)

  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenamesExperiment002.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001/filenamesExperiment002.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001/parameters002.txt
  )

  ADD_TEST(RegistrationTest002 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001/filenamesExperiment002.txt  
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001/parameters002.txt  
    )
    
  ADD_TEST(ComputeOutputs002 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001/filenamesExperiment002.txt  
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/BsplineRegistration001/parameters002.txt  
    )

ENDIF(BrainWeb_DATA_ROOT)
