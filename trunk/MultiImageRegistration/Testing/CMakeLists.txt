#
# List of  Tests
#
ADD_EXECUTABLE(mirReadImage ReadImage.cxx)
TARGET_LINK_LIBRARIES(mirReadImage ITKNumerics ITKIO)


ADD_TEST(mirReadImage01 ${EXECUTABLE_OUTPUT_PATH}/mirReadImage image1.png)
ADD_TEST(mirReadImage02 ${EXECUTABLE_OUTPUT_PATH}/mirReadImage image2.png)
ADD_TEST(mirReadImage03 ${EXECUTABLE_OUTPUT_PATH}/mirReadImage image3.png)
ADD_TEST(mirReadImage04 ${EXECUTABLE_OUTPUT_PATH}/mirReadImage image4.png)
