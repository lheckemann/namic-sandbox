#PROJECT( StatNonParamTest ) 
# Include itk
#INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
#INCLUDE(${USE_ITK_FILE})
  
NeuroLib_USE_ITK()

SET(StatNonParamTestPDM_SRCS createPerm.cxx main.cxx
computeStatPDM.cxx computeStatPDM_vector.cxx
)

# USE_ITK_STAT
ADD_DEFINITIONS( -DUSE_ITK_STAT )

ADD_EXECUTABLE( StatNonParamTestPDM StatNonParamTestPDM_SRCS)

TARGET_LINK_LIBRARIES ( StatNonParamTestPDM ${ITK_LIBRARIES} )

ADD_EXECUTABLE( PvalToRGB PvalToRGB.cxx )
ADD_EXECUTABLE( DistToRGB DistToRGB.cxx )
