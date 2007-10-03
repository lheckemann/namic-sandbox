PROJECT ( ThreadIT )

FIND_PACKAGE ( ITK )
IF ( ITK_FOUND )
  INCLUDE(${ITK_USE_FILE})
ELSE ( ITK_FOUND )
  MESSAGE ( FATAL_ERROR "Cannot build without ITK" )
ENDIF ( ITK_FOUND )

FIND_PATH(ZTHREAD_LIB_PATH libZThread.a /projects/mion/Linux/lib)
FIND_PATH(ZTHREAD_INCLUDE_PATH zthread/ZThread.h /projects/mion/Linux/include/)

LINK_LIBRARIES(
# dependent libraries
ITKNumerics
ITKBasicFilters
ITKIO
ITKCommon
ITKStatistics
ITKAlgorithms
ZThread
)

INCLUDE_DIRECTORIES ( ${ZTHREAD_INCLUDE_PATH} )
LINK_DIRECTORIES ( ${ZTHREAD_LIB_PATH} )
ADD_EXECUTABLE ( ThreadIT ThreadIT.cxx )
