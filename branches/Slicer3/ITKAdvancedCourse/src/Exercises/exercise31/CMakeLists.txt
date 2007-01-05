PROJECT(Exercise31)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(MultiThreader1  main1.cxx       )

TARGET_LINK_LIBRARIES(MultiThreader1   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultiThreader2  main2.cxx       )

TARGET_LINK_LIBRARIES(MultiThreader2   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultiThreader3  main3.cxx       )

TARGET_LINK_LIBRARIES(MultiThreader3   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultiThreader4  main4.cxx       )

TARGET_LINK_LIBRARIES(MultiThreader4   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultiThreader5  main5.cxx       )

TARGET_LINK_LIBRARIES(MultiThreader5   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(MultiThreader6  main6.cxx       )

TARGET_LINK_LIBRARIES(MultiThreader6   ITKIO ITKNumerics ITKCommon )


