

SET(StatNonParamTestPDMSRCS createPerm.cxx main.cxx
computeStatPDM.cxx computeStatPDM_vector.cxx
)

ADD_EXECUTABLE( StatNonParamTestPDM ${StatNonParamTestPDMSRCS} )

TARGET_LINK_LIBRARIES ( StatNonParamTestPDM ${ITK_LIBRARIES} )
