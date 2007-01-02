PROJECT(WFEngine)

SET(SRCS
WFXmlManager.cxx DOMTreeErrorReporter.cxx
WFXmlConfigManager.cxx
)

SET(LINK_LIBS
${XERCESC_LIBRARY}
)

ADD_LIBRARY(WFXmlManager ${SRCS})
TARGET_LINK_LIBRARIES(WFXmlManager ${LINK_LIBS})
