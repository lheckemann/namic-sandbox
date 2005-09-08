
NeuroLib_USE_ITK()

SET(StatNonParamTestPDMSRCS createPerm.cxx main.cxx
computeStatPDM.cxx computeStatPDM_vector.cxx
)

ADD_EXECUTABLE( StatNonParamTestPDM ${StatNonParamTestPDMSRCS} )

TARGET_LINK_LIBRARIES ( StatNonParamTestPDM ${ITK_LIBRARIES} )

ADD_EXECUTABLE( PvalToRGB PvalToRGB.cxx )
ADD_EXECUTABLE( DistToRGB DistToRGB.cxx )
