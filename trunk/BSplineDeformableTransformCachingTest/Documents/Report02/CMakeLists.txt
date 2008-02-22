PROJECT(MultiThreadedMIMetricTestReport)

FIND_PACKAGE(LATEX)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
ENDIF(NOT PDFLATEX_COMPILER)

FIND_PACKAGE(Gnuplot)

SET(REPORT_NAME Report02.pdf)
#
# Set some directories that we use below.
#
SET(PLOT_DIR ${MultiThreadedMIMetricTestReport_BINARY_DIR})
# FIXME - Testing/release
# Not the Report binary dir
# CMAKE_BUILD_TYPE
IF(WIN32)
SET(TEST_EXECUTABLE_DIR ${MattesMetricCachingTest_BINARY_DIR}/Testing/release)
ELSE(WIN32)
SET(TEST_EXECUTABLE_DIR ${MattesMetricCachingTest_BINARY_DIR}/Testing)
ENDIF(WIN32)
SET(TEST_EXECUTABLE ${TEST_EXECUTABLE_DIR}/MultiThreadedMIMetricTest)
#GET_TARGET_PROPERTY(MultiThreadedMIMetricTestReport TEST_EXECUTABLE LOCATION)

#
# Add custom targets that delete output files.
#
ADD_CUSTOM_COMMAND( OUTPUT EraseThreadingResults
  COMMAND ${CMAKE_COMMAND} -E remove ${PLOT_DIR}/AllThreadingResults.txt
)

ADD_CUSTOM_COMMAND( OUTPUT EraseDefaultResults
  COMMAND ${CMAKE_COMMAND} -E remove ${PLOT_DIR}/AllDefaultResults.txt 
)

ADD_CUSTOM_COMMAND( OUTPUT EraseThreadingMemoryResults
  COMMAND ${CMAKE_COMMAND} -E remove ${PLOT_DIR}/AllThreadingMemoryResults.txt
)

ADD_CUSTOM_COMMAND( OUTPUT EraseDefaultMemoryResults
  COMMAND ${CMAKE_COMMAND} -E remove ${PLOT_DIR}/AllDefaultMemoryResults.txt 
)
#
# Add custom command to make the system information file.
#
ADD_CUSTOM_COMMAND( OUTPUT ${PLOT_DIR}/SystemInformation.tex
  COMMAND ${TEST_EXECUTABLE_DIR}/MachineInformationExample SystemInformation.tex
  WORKING_DIRECTORY ${PLOT_DIR}
)


#
# Here we walk through the combination of volume sizes and threading/nothreading
# to generate a custom command for each experiment. For example, we create a
# custom command that runs a specific volume size with threading off. Each
# custom command outputs a unique text file. A list of these text files 
# is created and used as a dependency for the plot generation. Each of 
# the custom commands depends on the MultiThreadedMIMetricTestReport
#
SET(THREADING_LIST 0 1)
FOREACH(USE_THREADING ${THREADING_LIST})

  IF(${USE_THREADING} EQUAL 1)
    #SET(THREADING_STRING ThreadingResults)
    SET(THREADING_STRING Threading)    
  ELSE(${USE_THREADING} EQUAL 1)
    #SET(THREADING_STRING DefaultResults)
    SET(THREADING_STRING Default)
  ENDIF(${USE_THREADING} EQUAL 1)
  
  SET( DEPENDENCY_VAR MultiThreadedMIMetricTestReport Erase${THREADING_STRING}Results Erase${THREADING_STRING}MemoryResults )
  
  FOREACH(SIZE ${SIZE_LIST})
  
    SET(IMAGE_SIZE ${SIZE} ${SIZE} ${SIZE})

    SET(OUTPUT_FILE "All${THREADING_STRING}Results.txt" )
    SET(MEMORY_OUTPUT_FILE "All${THREADING_STRING}MemoryResults.txt" )
    
    SET(EXPERIMENT_TARGET_NAME "${THREADING_STRING}${SIZE}" )
    
    ADD_CUSTOM_COMMAND( 
      OUTPUT  ${EXPERIMENT_TARGET_NAME}
      COMMAND ${CMAKE_COMMAND} -E chdir ${PLOT_DIR} ${TEST_EXECUTABLE} ${IMAGE_SIZE} ${NUMBER_OF_CALLS} ${USE_THREADING} ${OUTPUT_FILE} ${MEMORY_OUTPUT_FILE}
      DEPENDS ${DEPENDENCY_VAR}
    )
    
    # Make experiments run in order.
    SET(DEPENDENCY_VAR ${EXPERIMENT_TARGET_NAME})
    
    IF(${USE_THREADING} EQUAL 1)
      SET(THREADING_TARGETS ${THREADING_TARGETS} ${EXPERIMENT_TARGET_NAME})
    ELSE(${USE_THREADING} EQUAL 1)
      SET(NOTHREADING_TARGETS ${NOTHREADING_TARGETS} ${EXPERIMENT_TARGET_NAME})
    ENDIF(${USE_THREADING} EQUAL 1)

  ENDFOREACH(SIZE ${SIZE_LIST})

  SET (EXPERIMENT_DEPENDENCIES ${EXPERIMENT_DEPENDENCIES} ${DEPENDENCY_VAR})
ENDFOREACH(USE_THREADING ${THREADING_LIST})

#
# This custom command makes the ThreadingTiming plot
#
ADD_CUSTOM_COMMAND(
   OUTPUT ${PLOT_DIR}/ThreadingTiming.pdf
   COMMAND ${GNUPLOT} ${MultiThreadedMIMetricTestReport_SOURCE_DIR}/MakeThreadingPlots.plt
   COMMAND ${PS2PDF_CONVERTER} ThreadingTiming.ps ThreadingTiming.pdf
   WORKING_DIRECTORY ${PLOT_DIR}
   DEPENDS ${EXPERIMENT_DEPENDENCIES}
)

ADD_CUSTOM_COMMAND(
   OUTPUT ${PLOT_DIR}/ThreadingMemory.pdf
   COMMAND ${GNUPLOT} ${MultiThreadedMIMetricTestReport_SOURCE_DIR}/MakeThreadingMemoryPlots.plt
   COMMAND ${PS2PDF_CONVERTER} ThreadingMemory.ps ThreadingMemory.pdf
   WORKING_DIRECTORY ${PLOT_DIR}
   DEPENDS ${EXPERIMENT_DEPENDENCIES}
)

ADD_CUSTOM_TARGET(Plots02 ALL
  DEPENDS ${PLOT_DIR}/ThreadingTiming.pdf ${PLOT_DIR}/ThreadingMemory.pdf
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
  Report02.tex
  times.sty
  picins.sty
  authors.tex
  )

#
# This adds a custom command for each source file in REPORT_SRCS
# that copies the file from the source directory to the binary
# directory where the pdf will be generated.
#
FOREACH(SOURCE_FILE ${REPORT_SRCS})
   ADD_CUSTOM_COMMAND(
     OUTPUT   ${MultiThreadedMIMetricTestReport_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${MultiThreadedMIMetricTestReport_SOURCE_DIR}/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MultiThreadedMIMetricTestReport_SOURCE_DIR}/${SOURCE_FILE} ${MultiThreadedMIMetricTestReport_BINARY_DIR}/${SOURCE_FILE}
    )
   SET(COPY_RESULTS ${COPY_RESULTS} ${MultiThreadedMIMetricTestReport_BINARY_DIR}/${SOURCE_FILE})
ENDFOREACH(SOURCE_FILE ${REPORT_SRCS})

#
# This adds a custom target that generates Report02.pdf
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
ADD_CUSTOM_TARGET(Report02 ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${MultiThreadedMIMetricTestReport_SOURCE_DIR}/Report02.tex 
        -output-directory ${MultiThreadedMIMetricTestReport_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER} 
        ${MultiThreadedMIMetricTestReport_SOURCE_DIR}/Report02.tex 
        -output-directory ${MultiThreadedMIMetricTestReport_BINARY_DIR}
   # Should depend on plots target?
   DEPENDS ${COPY_RESULTS} ${PLOT_DIR}/ThreadingTiming.pdf ${PLOT_DIR}/ThreadingMemory.pdf ${PLOT_DIR}/SystemInformation.tex
   WORKING_DIRECTORY ${MultiThreadedMIMetricTestReport_BINARY_DIR}
   )
 
#  
# This adds a custom target that will display the report.
#   
IF(WIN32)
SET(PDF_READER "c:\\Program Files\\Adobe\\Reader 8.0\\Reader\\AcroRd32.exe")
ELSE(WIN32)
SET(PDF_READER "/usr/bin/acroread")
ENDIF(WIN32)

#MESSAGE("${MultiThreadedMIMetricTestReport_BINARY_DIR}/${REPORT_NAME}")
ADD_CUSTOM_TARGET(ViewReport02 ALL
  COMMAND ${CMAKE_COMMAND} -E echo ${MultiThreadedMIMetricTestReport_BINARY_DIR}/${REPORT_NAME}
#  COMMAND ${PDF_READER} 
#  ARGS ${MultiThreadedMIMetricTestReport_BINARY_DIR}/${REPORT_NAME}
)

