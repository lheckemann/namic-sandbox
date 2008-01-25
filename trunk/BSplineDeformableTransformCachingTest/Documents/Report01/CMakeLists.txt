PROJECT(MattesMetricCachingTestReport)

FIND_PACKAGE(LATEX)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
ENDIF(NOT PDFLATEX_COMPILER)

FIND_PACKAGE(Gnuplot)

#
# Set some directories that we use below.
#
SET(PLOT_DIR ${MattesMetricCachingTestReport_BINARY_DIR})
# FIXME - Testing/release
# Not the Report binary dir
# CMAKE_BUILD_TYPE
IF(WIN32)
SET(MATTES_EXECUTABLE_DIR ${MattesMetricCachingTest_BINARY_DIR}/Testing/release)
ELSE(WIN32)
SET(MATTES_EXECUTABLE_DIR ${MattesMetricCachingTest_BINARY_DIR}/Testing)
ENDIF(WIN32)
SET(MATTES_EXECUTABLE ${MATTES_EXECUTABLE_DIR}/MattesMetricCachingTest)
#GET_TARGET_PROPERTY(MattesMetricCachingTest MATTES_EXECUTABLE LOCATION)

#
# Add custom targets that delete output files.
#
ADD_CUSTOM_COMMAND( OUTPUT EraseCachingResults
  COMMAND ${CMAKE_COMMAND} -E remove ${PLOT_DIR}/AllCachingResults.txt
)

ADD_CUSTOM_COMMAND( OUTPUT EraseNoCachingResults
  COMMAND ${CMAKE_COMMAND} -E remove ${PLOT_DIR}/AllNoCachingResults.txt 
)

#
# Here we walk through the combination of volume sizes and caching/nocaching
# to generate a custom command for each experiment. For example, we create a
# custom command that runs a specific volume size with caching off. Each
# custom command outputs a unique text file. A list of these text files 
# is created and used as a dependency for the plot generation. Each of 
# the custom commands depends on the MattesMetricCachingTest
#
SET(CACHING_LIST 0 1)
FOREACH(USE_CACHING ${CACHING_LIST})

  IF(${USE_CACHING} EQUAL 1)
    SET(CACHING_STRING CachingResults)
  ELSE(${USE_CACHING} EQUAL 1)
    SET(CACHING_STRING NoCachingResults)
  ENDIF(${USE_CACHING} EQUAL 1)
  
  SET( DEPENDENCY_VAR MattesMetricCachingTest Erase${CACHING_STRING})
  
  FOREACH(SIZE ${SIZE_LIST})
  
    SET(IMAGE_SIZE ${SIZE} ${SIZE} ${SIZE})

    SET(OUTPUT_FILE "All${CACHING_STRING}.txt" )
    SET(EXPERIMENT_TARGET_NAME "${CACHING_STRING}${SIZE}" )
    
    ADD_CUSTOM_COMMAND( 
      OUTPUT  ${EXPERIMENT_TARGET_NAME}
      COMMAND ${CMAKE_COMMAND} -E chdir ${PLOT_DIR} ${MATTES_EXECUTABLE} ${IMAGE_SIZE} ${NUMBER_OF_CALLS} ${USE_CACHING} ${OUTPUT_FILE}
      DEPENDS ${DEPENDENCY_VAR}
    )
    
    # Make experiments run in order.
    SET(DEPENDENCY_VAR ${EXPERIMENT_TARGET_NAME})
    
    IF(${USE_CACHING} EQUAL 1)
      SET(CACHING_TARGETS ${CACHING_TARGET} ${EXPERIMENT_TARGET_NAME})
    ELSE(${USE_CACHING} EQUAL 1)
      SET(NOCACHING_TARGETS ${NOCACHING_TARGETS} ${EXPERIMENT_TARGET_NAME})
    ENDIF(${USE_CACHING} EQUAL 1)

  ENDFOREACH(SIZE ${SIZE_LIST})

  SET (EXPERIMENT_DEPENDENCIES ${EXPERIMENT_DEPENDENCIES} ${DEPENDENCY_VAR})
ENDFOREACH(USE_CACHING ${CACHING_LIST})

#
# This custom command makes the CachingTiming plot
#
ADD_CUSTOM_COMMAND(
   OUTPUT ${PLOT_DIR}/CachingTiming.pdf
   COMMAND ${GNUPLOT} ${MattesMetricCachingTestReport_SOURCE_DIR}/MakeCachingPlots.plt
   COMMAND ${PS2PDF_CONVERTER} CachingTiming.ps CachingTiming.pdf
   WORKING_DIRECTORY ${PLOT_DIR}
   DEPENDS ${EXPERIMENT_DEPENDENCIES}
)

ADD_CUSTOM_TARGET(Plots ALL
  DEPENDS ${PLOT_DIR}/CachingTiming.pdf
)

#
# These are source files needed for the report generation.
#
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
  picins.sty
  )

#
# This adds a custom command for each source file in REPORT_SRCS
# that copies the file from the source directory to the binary
# directory where the pdf will be generated.
#
FOREACH(SOURCE_FILE ${REPORT_SRCS})
   ADD_CUSTOM_COMMAND(
     OUTPUT   ${MattesMetricCachingTestReport_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${MattesMetricCachingTestReport_SOURCE_DIR}/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MattesMetricCachingTestReport_SOURCE_DIR}/${SOURCE_FILE} ${MattesMetricCachingTestReport_BINARY_DIR}/${SOURCE_FILE}
    )
   SET(COPY_RESULTS ${COPY_RESULTS} ${MattesMetricCachingTestReport_BINARY_DIR}/${SOURCE_FILE})
ENDFOREACH(SOURCE_FILE ${REPORT_SRCS})

#
# This adds a custom target that generates Report01.pdf
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
ADD_CUSTOM_TARGET(ReportIJ ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${MattesMetricCachingTestReport_SOURCE_DIR}/Report01.tex 
        -output-directory ${MattesMetricCachingTestReport_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER} 
        ${MattesMetricCachingTestReport_SOURCE_DIR}/Report01.tex 
        -output-directory ${MattesMetricCachingTestReport_BINARY_DIR}
   DEPENDS ${COPY_RESULTS} ${PLOT_DIR}/CachingTiming.pdf
   WORKING_DIRECTORY ${MattesMetricCachingTestReport_BINARY_DIR}
   )
 
#  
# This adds a custom target that will display the report.
#   
IF(WIN32)
SET(PDF_READER "c:\\Program Files\\Adobe\\Reader 8.0\\Reader\\AcroRd32.exe")
ELSE(WIN32)
SET(PDF_READER "/usr/bin/acroread")
ENDIF(WIN32)

#MESSAGE("${MattesMetricCachingTestReport_BINARY_DIR}/Report01.pdf")
#ADD_CUSTOM_TARGET(ViewReport ALL
#  COMMAND ${PDF_READER} ${MattesMetricCachingTestReport_BINARY_DIR}/Report01.pdf
#  DEPENDS ReportIJ
#)

