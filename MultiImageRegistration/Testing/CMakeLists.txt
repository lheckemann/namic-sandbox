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
#
#  Find the BrainWeb image datasets to use them as input
#
FIND_PATH(BrainWeb_DATA_ROOT NAMES brainweb165a10f17.mha  DOC "http://public.kitware.com/pub/itk/Data/BrainWeb/BrainPart1.tgz")


IF(BrainWeb_DATA_ROOT)


  #
  #  Affine test with 2 input images for BrainPart1.tgz
  #
  ADD_TEST(CreateRandImages001 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData001
    2 on
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
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/README.txt
  )
  # Run groupwise registration
  ADD_TEST(RegistrationTest001 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/filenames001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/parameters001.txt
    ) 
  # Create output files
  ADD_TEST(WriteOutputsTest001 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/filenames001.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration001/parameters001.txt
    )
  # Increase test times
  set_tests_properties(CreateRandImages001 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest001 PROPERTIES TIMEOUT 3600)
  set_tests_properties(WriteOutputsTest001 PROPERTIES TIMEOUT 3600)


  #
  #  Affine test with 2 input images for BrainPart2.tgz
  #
  ADD_TEST(CreateRandImages002 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData002
    2 on
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
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/README.txt
  )
  ADD_TEST(RegistrationTest002 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/filenames002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/parameters002.txt
    )
  ADD_TEST(WriteOutputsTest002 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/filenames002.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration002/parameters002.txt
    )
  set_tests_properties(CreateRandImages002 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest002 PROPERTIES TIMEOUT 3600)
  set_tests_properties(WriteOutputsTest002 PROPERTIES TIMEOUT 3600)

  #
  #  Affine test with 2 input images for BrainPart3.tgz
  #
  ADD_TEST(CreateRandImages003 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData003
    2 on
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
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README003.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/README.txt
  )
  ADD_TEST(RegistrationTest003 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/filenames003.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/parameters003.txt
    )
  ADD_TEST(WriteOutputsTest003 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/filenames003.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration003/parameters003.txt
    )
  set_tests_properties(CreateRandImages003 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest003 PROPERTIES TIMEOUT 3600)
  set_tests_properties(WriteOutputsTest003 PROPERTIES TIMEOUT 3600)
  
    
  #
  #  B-spline test with 2 images for BrainPart1.tgz
  #
  ADD_TEST(CreateRandImages004 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData004
    2 on
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
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README004.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/README.txt
  )
  ADD_TEST(RegistrationTest004 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/filenames004.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/parameters004.txt
    ) 
  ADD_TEST(WriteOutputsTest004 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/filenames004.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration004/parameters004.txt
    )
  set_tests_properties(CreateRandImages004 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest004 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest004 PROPERTIES TIMEOUT 3600)


    
  #
  #  B-spline test with 2 images for BrainPart2.tgz
  #
  ADD_TEST(CreateRandImages005 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData005
    2 on
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
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README005.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/README.txt
  )
  ADD_TEST(RegistrationTest005 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/filenames005.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/parameters005.txt
    ) 
  ADD_TEST(WriteOutputsTest005 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/filenames005.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration005/parameters005.txt
    )
  set_tests_properties(CreateRandImages005 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest005 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest005 PROPERTIES TIMEOUT 3600)

    
  #
  #  B-spline test with 2 images for BrainPart3.tgz
  #
  ADD_TEST(CreateRandImages006 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData006
    2 on
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
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README006.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/README.txt
  )
  ADD_TEST(RegistrationTest006 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/filenames006.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/parameters006.txt
    ) 
  ADD_TEST(WriteOutputsTest006 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/filenames006.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration006/parameters006.txt
    )
  set_tests_properties(CreateRandImages006 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest006 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest006 PROPERTIES TIMEOUT 3600)





  #
  #  Affine test with 10 input images for BrainPart1.tgz
  #
  ADD_TEST(CreateRandImages007 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData007
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames007.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/filenames007.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters007.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/parameters007.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README007.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/README.txt
  )
  ADD_TEST(RegistrationTest007 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/filenames007.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/parameters007.txt
    ) 
  ADD_TEST(WriteOutputsTest007 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/filenames007.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration007/parameters007.txt
    )
  set_tests_properties(CreateRandImages007 PROPERTIES TIMEOUT 7200)
  set_tests_properties(RegistrationTest007 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest007 PROPERTIES TIMEOUT 7200)


  #
  #  Affine test with 10 input images for BrainPart2.tgz
  #
  ADD_TEST(CreateRandImages008 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData008
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008)
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames008.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/filenames008.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters008.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/parameters008.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README008.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/README.txt
  )
  ADD_TEST(RegistrationTest008 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/filenames008.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/parameters008.txt
    )
  ADD_TEST(WriteOutputsTest008 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/filenames008.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration008/parameters008.txt
    )
  set_tests_properties(CreateRandImages008 PROPERTIES TIMEOUT 7200)
  set_tests_properties(RegistrationTest008 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest008 PROPERTIES TIMEOUT 7200)

  #
  #  Affine test with 10 input images for BrainPart3.tgz
  #
  ADD_TEST(CreateRandImages009 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData009
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames009.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/filenames009.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters009.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/parameters009.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README009.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/README.txt
  )
  ADD_TEST(RegistrationTest009 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/filenames009.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/parameters009.txt
    )
  ADD_TEST(WriteOutputsTest009 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/filenames009.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration009/parameters009.txt
    )
  set_tests_properties(CreateRandImages009 PROPERTIES TIMEOUT 7200)
  set_tests_properties(RegistrationTest009 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest009 PROPERTIES TIMEOUT 7200)
  
    
  #
  #  B-spline test with 10 images for BrainPart1.tgz
  #
  ADD_TEST(CreateRandImages010 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData010
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames010.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/filenames010.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters010.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/parameters010.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README010.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/README.txt
  )
  ADD_TEST(RegistrationTest010 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/filenames010.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/parameters010.txt
    ) 
  ADD_TEST(WriteOutputsTest010 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/filenames010.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration010/parameters010.txt
    )
  set_tests_properties(CreateRandImages010 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest010 PROPERTIES TIMEOUT 36000)
  set_tests_properties(WriteOutputsTest010 PROPERTIES TIMEOUT 36000)


    
  #
  #  B-spline test with 10 images for BrainPart2.tgz
  #
  ADD_TEST(CreateRandImages011 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData011
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames011.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/filenames011.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters011.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/parameters011.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README011.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/README.txt
  )
  ADD_TEST(RegistrationTest011 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/filenames011.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/parameters011.txt
    ) 
  ADD_TEST(WriteOutputsTest011 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/filenames011.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration011/parameters011.txt
    )
  set_tests_properties(CreateRandImages011 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest011 PROPERTIES TIMEOUT 36000)
  set_tests_properties(WriteOutputsTest011 PROPERTIES TIMEOUT 36000)

    
  #
  #  B-spline test with 10 images for BrainPart3.tgz
  #
  ADD_TEST(CreateRandImages012 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData012
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames012.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/filenames012.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters012.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/parameters012.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README012.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/README.txt
  )
  ADD_TEST(RegistrationTest012 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/filenames012.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/parameters012.txt
    ) 
  ADD_TEST(WriteOutputsTest012 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/filenames012.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration012/parameters012.txt
    )
  set_tests_properties(CreateRandImages012 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest012 PROPERTIES TIMEOUT 36000)
  set_tests_properties(WriteOutputsTest012 PROPERTIES TIMEOUT 36000)




  #
  #  Affine test with 30 input images for BrainPart1.tgz
  #
  ADD_TEST(CreateRandImages013 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData013
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames013.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/filenames013.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters013.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/parameters013.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README013.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/README.txt
  )
  ADD_TEST(RegistrationTest013 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/filenames013.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/parameters013.txt
    ) 
  ADD_TEST(WriteOutputsTest013 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/filenames013.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration013/parameters013.txt
    )
  set_tests_properties(CreateRandImages013 PROPERTIES TIMEOUT 72000)
  set_tests_properties(RegistrationTest013 PROPERTIES TIMEOUT 72000)
  set_tests_properties(WriteOutputsTest013 PROPERTIES TIMEOUT 72000)


  #
  #  Affine test with 30 input images for BrainPart2.tgz
  #
  ADD_TEST(CreateRandImages014 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData014
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014)
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames014.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/filenames014.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters014.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/parameters014.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README014.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/README.txt
  )
  ADD_TEST(RegistrationTest014 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/filenames014.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/parameters014.txt
    )
  ADD_TEST(WriteOutputsTest014 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/filenames014.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration014/parameters014.txt
    )
  set_tests_properties(CreateRandImages014 PROPERTIES TIMEOUT 72000)
  set_tests_properties(RegistrationTest014 PROPERTIES TIMEOUT 72000)
  set_tests_properties(WriteOutputsTest014 PROPERTIES TIMEOUT 72000)

  #
  #  Affine test with 30 input images for BrainPart3.tgz
  #
  ADD_TEST(CreateRandImages015 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData015
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames015.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/filenames015.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters015.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/parameters015.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README015.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/README.txt
  )
  ADD_TEST(RegistrationTest015 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/filenames015.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/parameters015.txt
    )
  ADD_TEST(WriteOutputsTest015 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/filenames015.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration015/parameters015.txt
    )
  set_tests_properties(CreateRandImages015 PROPERTIES TIMEOUT 72000)
  set_tests_properties(RegistrationTest015 PROPERTIES TIMEOUT 72000)
  set_tests_properties(WriteOutputsTest015 PROPERTIES TIMEOUT 72000)
  
    
  #
  #  B-spline test with 30 images for BrainPart1.tgz
  #
  ADD_TEST(CreateRandImages016 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb1e1a10f20.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData016
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames016.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/filenames016.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters016.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/parameters016.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README016.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/README.txt
  )
  ADD_TEST(RegistrationTest016 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/filenames016.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/parameters016.txt
    ) 
  ADD_TEST(WriteOutputsTest016 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/filenames016.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration016/parameters016.txt
    )
  set_tests_properties(CreateRandImages016 PROPERTIES TIMEOUT 360000)
  set_tests_properties(RegistrationTest016 PROPERTIES TIMEOUT 360000)
  set_tests_properties(WriteOutputsTest016 PROPERTIES TIMEOUT 360000)


    
  #
  #  B-spline test with 30 images for BrainPart2.tgz
  #
  ADD_TEST(CreateRandImages017 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb165a10f17.mha 
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData017
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames017.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/filenames017.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters017.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/parameters017.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README017.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/README.txt
  )
  ADD_TEST(RegistrationTest017 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/filenames017.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/parameters017.txt
    ) 
  ADD_TEST(WriteOutputsTest017 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/filenames017.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration017/parameters017.txt
    )
  set_tests_properties(CreateRandImages017 PROPERTIES TIMEOUT 360000)
  set_tests_properties(RegistrationTest017 PROPERTIES TIMEOUT 360000)
  set_tests_properties(WriteOutputsTest017 PROPERTIES TIMEOUT 360000)

    
  #
  #  B-spline test with 30 images for BrainPart3.tgz
  #
  ADD_TEST(CreateRandImages018 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${BrainWeb_DATA_ROOT}/brainweb1e5a10f17.mha
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData018
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames018.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/filenames018.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters018.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/parameters018.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README018.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/README.txt
  )
  ADD_TEST(RegistrationTest018 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/filenames018.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/parameters018.txt
    ) 
  ADD_TEST(WriteOutputsTest018 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/filenames018.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration018/parameters018.txt
    )
  set_tests_properties(CreateRandImages018 PROPERTIES TIMEOUT 360000)
  set_tests_properties(RegistrationTest018 PROPERTIES TIMEOUT 360000)
  set_tests_properties(WriteOutputsTest018 PROPERTIES TIMEOUT 360000)



ENDIF(BrainWeb_DATA_ROOT)




#
#
#  Find the FAImage image datasets to use them as input
#
FIND_PATH(FAImage_DATA_ROOT NAMES FA.mhd DOC "Directory containing image from FAImage")

IF(FAImage_DATA_ROOT)


  #
  #  Affine test with 2 input images for FA.mhd
  #
  ADD_TEST(CreateRandImages101 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${FAImage_DATA_ROOT}/FA.mhd
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData101
    2 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames101.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/filenames101.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters101.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/parameters101.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README101.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/README.txt
  )
  ADD_TEST(RegistrationTest101 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/filenames101.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/parameters101.txt
    )
  ADD_TEST(WriteOutputsTest101 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/filenames101.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration101/parameters101.txt
    )
  set_tests_properties(CreateRandImages101 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest101 PROPERTIES TIMEOUT 3600)
  set_tests_properties(WriteOutputsTest101 PROPERTIES TIMEOUT 3600)
  
    
  #
  #  B-spline test with 2 images for FA.mhd
  #
  ADD_TEST(CreateRandImages102 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${FAImage_DATA_ROOT}/FA.mhd
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData102
    2 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames102.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/filenames102.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters102.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/parameters102.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README102.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/README.txt
  )
  ADD_TEST(RegistrationTest102 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/filenames102.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/parameters102.txt
    ) 
  ADD_TEST(WriteOutputsTest102 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/filenames102.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration102/parameters102.txt
    )
  set_tests_properties(CreateRandImages102 PROPERTIES TIMEOUT 3600)
  set_tests_properties(RegistrationTest102 PROPERTIES TIMEOUT 7200)
  set_tests_properties(WriteOutputsTest102 PROPERTIES TIMEOUT 3600)


  #
  #  Affine test with 10 input images for FA.mhd
  #
  ADD_TEST(CreateRandImages103 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${FAImage_DATA_ROOT}/FA.mhd
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData103
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames103.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/filenames103.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters103.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/parameters103.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README103.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/README.txt
  )
  ADD_TEST(RegistrationTest103 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/filenames103.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/parameters103.txt
    )
  ADD_TEST(WriteOutputsTest103 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/filenames103.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration103/parameters103.txt
    )
  set_tests_properties(CreateRandImages103 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest103 PROPERTIES TIMEOUT 36000)
  set_tests_properties(WriteOutputsTest103 PROPERTIES TIMEOUT 36000)
  
    
  #
  #  B-spline test with 10 images for FA.mhd
  #
  ADD_TEST(CreateRandImages104 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${FAImage_DATA_ROOT}/FA.mhd
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData104
    10 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames104.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/filenames104.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters104.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/parameters104.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README104.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/README.txt
  )
  ADD_TEST(RegistrationTest104 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/filenames104.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/parameters104.txt
    ) 
  ADD_TEST(WriteOutputsTest104 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/filenames104.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration104/parameters104.txt
    )
  set_tests_properties(CreateRandImages104 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest104 PROPERTIES TIMEOUT 72000)
  set_tests_properties(WriteOutputsTest104 PROPERTIES TIMEOUT 36000)


  #
  #  Affine test with 30 input images for FA.mhd
  #
  ADD_TEST(CreateRandImages105 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetAffine
    ${FAImage_DATA_ROOT}/FA.mhd
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData105
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames105.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/filenames105.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters105.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/parameters105.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README105.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/README.txt
  )
  ADD_TEST(RegistrationTest105 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/filenames105.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/parameters105.txt
    )
  ADD_TEST(WriteOutputsTest105 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/filenames105.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration105/parameters105.txt
    )
  set_tests_properties(CreateRandImages105 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest105 PROPERTIES TIMEOUT 36000)
  set_tests_properties(WriteOutputsTest105 PROPERTIES TIMEOUT 36000)
  
    
  #
  #  B-spline test with 30 images for FA.mhd
  #
  ADD_TEST(CreateRandImages106 ${EXECUTABLE_OUTPUT_PATH}/CreateImageSetBspline
    ${FAImage_DATA_ROOT}/FA.mhd
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/ImageData106
    30 on
    )
  MAKE_DIRECTORY(${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106)
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/filenames106.txt.in
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/filenames106.txt
  )
  CONFIGURE_FILE( 
    ${MultiImageRegistration_TEST_INPUT_DIR}/parameters106.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/parameters106.txt
  )
  CONFIGURE_FILE(
    ${MultiImageRegistration_TEST_INPUT_DIR}/README106.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/README.txt
  )
  ADD_TEST(RegistrationTest106 ${EXECUTABLE_OUTPUT_PATH}/GroupwiseRegistration
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/filenames106.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/parameters106.txt
    ) 
  ADD_TEST(WriteOutputsTest106 ${EXECUTABLE_OUTPUT_PATH}/ComputeOutputs
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/filenames106.txt
    ${MultiImageRegistration_TEST_OUTPUT_DIR}/Registration106/parameters106.txt
    )
  set_tests_properties(CreateRandImages106 PROPERTIES TIMEOUT 36000)
  set_tests_properties(RegistrationTest106 PROPERTIES TIMEOUT 72000)
  set_tests_properties(WriteOutputsTest106 PROPERTIES TIMEOUT 36000)

  
ENDIF(FAImage_DATA_ROOT)
