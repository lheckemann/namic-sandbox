#
#  Configure the Executables
#
ADD_EXECUTABLE(SliceBasedImageTest01 SliceBasedImageTest01.cxx )
TARGET_LINK_LIBRARIES(SliceBasedImageTest01 ITKCommon)


#
#  Configure the Tests
#
ADD_TEST(SliceBasedImageTest01 SliceBasedImageTest01)


