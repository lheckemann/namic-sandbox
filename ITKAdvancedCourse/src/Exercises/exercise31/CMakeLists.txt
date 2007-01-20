PROJECT(Exercise31)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(MultThreader31_1  main1.cxx       )

TARGET_LINK_LIBRARIES(MultThreader31_1   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultThreader31_2  main2.cxx       )

TARGET_LINK_LIBRARIES(MultThreader31_2   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultThreader31_3  main3.cxx       )

TARGET_LINK_LIBRARIES(MultThreader31_3   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultThreader31_4  main4.cxx       )

TARGET_LINK_LIBRARIES(MultThreader31_4   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultThreader31_5  main5.cxx       )

TARGET_LINK_LIBRARIES(MultThreader31_5   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultThreader31_6  main6.cxx       )

TARGET_LINK_LIBRARIES(MultThreader31_6   ITKIO ITKNumerics ITKCommon )


