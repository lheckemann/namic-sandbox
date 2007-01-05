PROJECT(Exercise27)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(ImageRedAdaptor  main.cxx       )

TARGET_LINK_LIBRARIES(ImageRedAdaptor   ITKIO ITKNumerics ITKCommon )

ADD_EXECUTABLE(ImageInvertAdaptor  main2.cxx       )

TARGET_LINK_LIBRARIES(ImageInvertAdaptor   ITKIO ITKNumerics ITKCommon )
