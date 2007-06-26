#
# Test for the Multi-Image Registration
#


# 
# Define directory for output produced from the tests.
#
SET(MultiImageRegistration_TEST_OUTPUT_DIR "${MultiImageRegistration_BINARY_DIR}/Testing/Temporary")
MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR})

SET(MultiImageRegistration_TEST_INPUT_DIR "${MultiImageRegistration_SOURCE_DIR}/Testing/Data/Input")


CONFIGURE_FILE( 
  ${MultiImageRegistration_TEST_INPUT_DIR}/filenamesExperiment001.txt.in
  ${MultiImageRegistration_TEST_OUTPUT_DIR}/filenamesExperiment001.txt
  )

#
#
#  Find the BrainWeb image datasets to use them as input
#
FIND_PATH(BrainWeb_DATA_ROOT NAMES brainweb165a10f17.mha DOC "Directory containing image from BrainWeb")


IF(BrainWeb_DATA_ROOT)

  #
  #  First, take an input image and generate many instances of artificial deformations.
  #
  ADD_TEST(CreateDeformedImage001 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline  
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}
    )


  ADD_TEST(AffineRegistrationTest0001 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/filenamesExperiment001.txt  
    ${MultiImageRegistration_TEST_INPUT_DIR}/parametersExperiment001.txt  
    )

ENDIF(BrainWeb_DATA_ROOT)
