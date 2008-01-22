PROJECT(MattesMetricCachingTestReport)

FIND_PACKAGE(LATEX)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
ENDIF(NOT PDFLATEX_COMPILER)

SET(REPORT_SRCS
  algorithmic.sty
  algorithm.sty
  amssymb.sty
  fancyhdr.sty
  floatflt.sty
  fncychap.sty
  InsightArticle.cls
  InsightJournal.bib
  InsightJournal.ist
  InsightJournal.sty
  Report01.tex
  times.sty
  )

FOREACH(SOURCE_FILE ${REPORT_SRCS})
#   ${CMAKE_COMMAND} "-E copy  ${MattesMetricCachingTestReport_SOURCE_DIR}/${SOURCE_FILE} ${MattesMetricCachingTestReport_BINARY_DIR}/${SOURCE_FILE}"
#   MESSAGE(${SOURCE_FILE})
   CONFIGURE_FILE("${MattesMetricCachingTestReport_SOURCE_DIR}/${SOURCE_FILE}" "${MattesMetricCachingTestReport_BINARY_DIR}/${SOURCE_FILE}" COPYONLY)
ENDFOREACH(SOURCE_FILE ${REPORT_SRCS})

ADD_CUSTOM_TARGET(ReportIJ ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${MattesMetricCachingTestReport_SOURCE_DIR}/Report01.tex 
        -output-directory ${MattesMetricCachingTestReport_BINARY_DIR}
   DEPENDS ${MattesMetricCachingTestReport_SOURCE_DIR}/Report01.tex
   WORKING_DIRECTORY ${MattesMetricCachingTestReport_BINARY_DIR}
   )

