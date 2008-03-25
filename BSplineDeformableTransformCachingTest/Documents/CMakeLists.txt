
#HKEY_LOCAL_MACHINE\SOFTWARE\Adobe\Acrobat Reader\8.0\InstallPath
GET_FILENAME_COMPONENT(ACROREAD_BINARY_PATH_FROM_REGISTERY_8_00
   "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe\\Acrobat Reader\\8.0\\InstallPath]" ABSOLUTE
)

#HKEY_LOCAL_MACHINE\SOFTWARE\Adobe\Adobe Acrobat\8.0\InstallPath
GET_FILENAME_COMPONENT(ACROBAT_BINARY_PATH_FROM_REGISTERY_8_00
   "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Adobe\\Adobe Acrobat\\8.0\\InstallPath]" ABSOLUTE
)

IF(WIN32)
  SET(WINDOWS_PDF_BINARY_PATH ${ACROREAD_BINARY_PATH_FROM_REGISTERY_8_00} ${ACROBAT_BINARY_PATH_FROM_REGISTERY_8_00})
ENDIF(WIN32)

#MESSAGE( "Registry path: ${ACROREAD_BINARY_PATH_FROM_REGISTERY_8_00}" )
#MESSAGE( "Registry path: ${ACROBAT_BINARY_PATH_FROM_REGISTERY_8_00}" )

FIND_PROGRAM(
  PDF_READER
  NAMES AcroRd32 acrobat acroread
  PATHS ${WINDOWS_PDF_BINARY_PATH}
        /usr/bin
)

IF(NOT PDF_READER)
  MESSAGE("A pdf reader was not found. You will not be able to view the reports without it. Please provide the full path to a pdf reader executable in the PDF_READER variable.")
ENDIF(NOT PDF_READER)

#FindLATEX
FIND_PACKAGE(LATEX REQUIRED)

IF(NOT PDFLATEX_COMPILER)
  MESSAGE("pdflatex compiler was not found. Please a full path to a pdflatex compiler in the PDFLATEX_COMPILER variable")
ENDIF(NOT PDFLATEX_COMPILER)

#FindGnuplot, GNUPLOT_FOUND, GNUPLOT_EXECUTABLE
FIND_PACKAGE(Gnuplot REQUIRED)

MARK_AS_ADVANCED(
  CLEAR GNUPLOT
)

IF(NOT GNUPLOT)
  MESSAGE("Gnuplot was not found. Please provide the full path to a gnuplot executable in the GNUPLOT variable.")
ENDIF(NOT GNUPLOT)


SUBDIRS(
  Report01
  Report02
  Report03
  )
