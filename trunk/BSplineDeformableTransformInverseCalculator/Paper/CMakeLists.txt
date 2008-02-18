PROJECT(BSplineDeformableTransformInverseCalculatorsReport)

FIND_PACKAGE(LATEX)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
ENDIF(NOT PDFLATEX_COMPILER)

FIND_PACKAGE(Gnuplot)

#
# Set some directories that we use below.
#
SET(PLOT_DIR ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR})


GET_TARGET_PROPERTY(MachineInformation_EXECUTABLE MachineInformationExample LOCATION)

#
# Add custom command to make the system information file.
#
ADD_CUSTOM_COMMAND( 
  OUTPUT ${PLOT_DIR}/SystemInformation.tex
  COMMAND ${MachineInformation_EXECUTABLE} SystemInformation.tex
  WORKING_DIRECTORY ${PLOT_DIR}
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
  BSplineDeformableTransformInverseCalculators.tex
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
     OUTPUT   ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${BSplineDeformableTransformInverseCalculatorsReport_SOURCE_DIR}/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${BSplineDeformableTransformInverseCalculatorsReport_SOURCE_DIR}/${SOURCE_FILE} 
        ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR}/${SOURCE_FILE}
    )
   SET(COPY_RESULTS ${COPY_RESULTS} ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR}/${SOURCE_FILE})
ENDFOREACH(SOURCE_FILE ${REPORT_SRCS})


SET(REPORT_ELEMENTS 
  ${PLOT_DIR}/SystemInformation.tex
  )

#
# This adds a custom target that generates BSplineDeformableTransformInverseCalculators.pdf
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
ADD_CUSTOM_TARGET(ReportIJ ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${BSplineDeformableTransformInverseCalculatorsReport_SOURCE_DIR}/BSplineDeformableTransformInverseCalculators.tex 
        -output-directory ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER} 
        ${BSplineDeformableTransformInverseCalculatorsReport_SOURCE_DIR}/BSplineDeformableTransformInverseCalculators.tex 
        -output-directory ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR}
   DEPENDS ${COPY_RESULTS} ${REPORT_ELEMENTS} 
   WORKING_DIRECTORY ${BSplineDeformableTransformInverseCalculatorsReport_BINARY_DIR}
   )
 
