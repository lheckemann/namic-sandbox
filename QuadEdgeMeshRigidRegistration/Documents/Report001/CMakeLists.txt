PROJECT(QuadEdgeMeshRigidRegistrationReport001)

FIND_PACKAGE(LATEX)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
ENDIF(NOT PDFLATEX_COMPILER)

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
  QuadEdgeMeshRigidRegistration.tex
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
     OUTPUT   ${QuadEdgeMeshRigidRegistrationReport001_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${QuadEdgeMeshRigidRegistrationReport001_SOURCE_DIR}/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${QuadEdgeMeshRigidRegistrationReport001_SOURCE_DIR}/${SOURCE_FILE} 
        ${QuadEdgeMeshRigidRegistrationReport001_BINARY_DIR}/${SOURCE_FILE}
    )
   SET(COPY_RESULTS ${COPY_RESULTS} ${QuadEdgeMeshRigidRegistrationReport001_BINARY_DIR}/${SOURCE_FILE})
ENDFOREACH(SOURCE_FILE ${REPORT_SRCS})


SET(REPORT_ELEMENTS 
  )

#
# This adds a custom target that generates QuadEdgeMeshRigidRegistrationReport001.pdf
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
ADD_CUSTOM_TARGET(ReportIJ ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${QuadEdgeMeshRigidRegistrationReport001_SOURCE_DIR}/QuadEdgeMeshRigidRegistration.tex 
        -output-directory ${QuadEdgeMeshRigidRegistrationReport001_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER} 
        ${QuadEdgeMeshRigidRegistrationReport001_SOURCE_DIR}/QuadEdgeMeshRigidRegistration.tex 
        -output-directory ${QuadEdgeMeshRigidRegistrationReport001_BINARY_DIR}
   DEPENDS ${COPY_RESULTS} ${REPORT_ELEMENTS} 
   WORKING_DIRECTORY ${QuadEdgeMeshRigidRegistrationReport001_BINARY_DIR}
   )
 
