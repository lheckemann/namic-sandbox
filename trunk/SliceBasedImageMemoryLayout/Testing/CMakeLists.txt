#
#  Configure the Executables
#
ADD_EXECUTABLE(SliceContiguousImageTest01 SliceContiguousImageTest01.cxx )
TARGET_LINK_LIBRARIES(SliceContiguousImageTest01 ITKCommon)


#
#  Configure the Tests
#
ADD_TEST(SliceContiguousImageTest01 SliceContiguousImageTest01)


