PROJECT(Exercise29)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_LIBRARY(MyOwnDynamicFactory29 SHARED  itkMyOwnFactory.cxx )
ADD_EXECUTABLE(UseMyOwnFactory29 UseMyOwnFactory.cxx       )

TARGET_LINK_LIBRARIES(MyOwnDynamicFactory29  ITKIO ITKNumerics ITKCommon )
TARGET_LINK_LIBRARIES(UseMyOwnFactory29  ITKIO ITKNumerics ITKCommon )



ADD_LIBRARY(MyRealTimeClockDynamicFactory29 SHARED  itkMyRealTimeClockFactory.cxx )
ADD_EXECUTABLE(UseMyRealTimeClockFactory29 UseMyRealTimeClockFactory.cxx       )

TARGET_LINK_LIBRARIES(MyRealTimeClockDynamicFactory29  ITKIO ITKNumerics ITKCommon )
TARGET_LINK_LIBRARIES(UseMyRealTimeClockFactory29  ITKIO ITKNumerics ITKCommon )

