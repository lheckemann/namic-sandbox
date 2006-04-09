PROJECT(Exercise28)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_EXECUTABLE(ReflectDiagonalWithIterators  main.cxx       )

TARGET_LINK_LIBRARIES(ReflectDiagonalWithIterators   ITKIO ITKNumerics ITKCommon )
