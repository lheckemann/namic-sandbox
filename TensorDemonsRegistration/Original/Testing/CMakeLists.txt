
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${TensorDemonsRegistration_SOURCE_DIR})
SET(TEMP ${TensorDemonsRegistration_BINARY_DIR}/Testing/Temporary)

########################################
########################################
#
#  Define Executables
#
########################################
########################################

ADD_EXECUTABLE(vtkImageWarpTest 
  vtkImageWarpTest.cxx 
  ${TensorDemonsRegistration_SOURCE_DIR}/Original/Cxx/vtkImageWarp.cxx
  )

TARGET_LINK_LIBRARIES(vtkImageWarpTest vtkIO vtkCommon)

ADD_EXECUTABLE(itkGenerateSyntheticTensorImage itkGenerateSyntheticTensorImage.cxx )
TARGET_LINK_LIBRARIES(itkGenerateSyntheticTensorImage ITKIO )




########################################
########################################
#
#  Define Tests
#
########################################
########################################

ADD_TEST(vtkImageWarpTest01 
  ${CXX_TEST_PATH}/vtkImageWarpTest
 )

ADD_TEST(itkGenerateSyntheticTensorImage 
  ${CXX_TEST_PATH}/itkGenerateSyntheticTensorImage
  ${TEMP}/SmallTensorImage.vtk
  )

