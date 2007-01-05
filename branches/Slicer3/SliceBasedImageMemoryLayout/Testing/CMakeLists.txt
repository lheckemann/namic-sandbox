#
#  Configure the Executables
#
ADD_EXECUTABLE(SliceBasedImageTest01 SliceBasedImageTest01.cxx )
TARGET_LINK_LIBRARIES(SliceBasedImageTest01 ITKCommon)


ADD_EXECUTABLE(MultiModalityRegistration1 MultiModalityRegistration1.cxx )
TARGET_LINK_LIBRARIES(MultiModalityRegistration1 
  ITKAlgorithms 
  ITKNumerics 
  ITKIO 
  ITKCommon
  )



#
#  Configure the Tests
#
ADD_TEST(SliceBasedImageTest01 SliceBasedImageTest01)

ADD_TEST(MultiModalityRegistration1 MultiModalityRegistration1)


