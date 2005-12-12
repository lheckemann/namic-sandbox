
NeuroLib_USE_ITK()
NeuroLib_USE_LIBXML2()

SET(XMLIO_SRCS 
xmlHierarchy.cxx xmlIObase.cxx xmlIOlib.cxx xmlIOxml2priv.cxx
xmlIOxml2public.cxx xmlIOxml2STL.cxx StudyInfo.cxx
PopulationInfo.cxx SubjectInfo.cxx

)

ADD_LIBRARY(ShapeXmlIO ${XMLIO_SRCS} )
