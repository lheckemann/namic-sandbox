PROJECT(Exercise24)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(DividerByTwoImageFilter  main.cxx       )

TARGET_LINK_LIBRARIES(DividerByTwoImageFilter   ITKIO ITKNumerics ITKCommon )
