#
#  Configure the Executables
#
ADD_EXECUTABLE(SliceContiguousImageTest01 SliceContiguousImageTest01.cxx )
TARGET_LINK_LIBRARIES(SliceContiguousImageTest01 ITKCommon ITKIO)

ADD_EXECUTABLE(SparseImageTest01 SparseImageTest01.cxx )
TARGET_LINK_LIBRARIES(SparseImageTest01 ITKCommon ITKIO)

ADD_EXECUTABLE(SingleBitBinaryImageTest01 SingleBitBinaryImageTest01.cxx )
TARGET_LINK_LIBRARIES(SingleBitBinaryImageTest01 ITKCommon ITKIO)

#
# Configure the Tests
#
ADD_TEST(SliceContiguousImageTest01 SliceContiguousImageTest01)
ADD_TEST(SparseImageTest01 SparseImageTest01)
ADD_TEST(SingleBitBinaryImageTest01 SingleBitBinaryImageTest01)
