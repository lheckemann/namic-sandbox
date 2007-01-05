PROJECT(Exercise25)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(DividerImageFilter  main.cxx       )

TARGET_LINK_LIBRARIES(DividerImageFilter   ITKIO ITKNumerics ITKCommon )
