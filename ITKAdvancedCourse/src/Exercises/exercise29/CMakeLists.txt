PROJECT(Exercise29)

FIND_PACKAGE(ITK REQUIRED)

INCLUDE(${ITK_USE_FILE})

ADD_LIBRARY(MyOwnDynamicFactory SHARED  itkMyOwnFactory.cxx )
ADD_EXECUTABLE(UseMyOwnFactory UseMyOwnFactory.cxx       )

TARGET_LINK_LIBRARIES(MyOwnDynamicFactory  ITKIO ITKNumerics ITKCommon )
TARGET_LINK_LIBRARIES(UseMyOwnFactory  ITKIO ITKNumerics ITKCommon )



ADD_LIBRARY(MyRealTimeClockDynamicFactory SHARED  itkMyRealTimeClockFactory.cxx )
ADD_EXECUTABLE(UseMyRealTimeClockFactory UseMyRealTimeClockFactory.cxx       )

TARGET_LINK_LIBRARIES(MyRealTimeClockDynamicFactory  ITKIO ITKNumerics ITKCommon )
TARGET_LINK_LIBRARIES(UseMyRealTimeClockFactory  ITKIO ITKNumerics ITKCommon )

