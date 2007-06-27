
SET(ITKIO_SRCS
  itkIOCommon.cxx
  itkNiftiImageIO.cxx
  itkNiftiImageIOFactory.cxx
  itkImageIOBase.cxx
  itkImageIOFactory.cxx
  itkImageIORegion.cxx
)

# Add ITKIO library
ADD_LIBRARY(ITKIOStreaming ${ITKIO_SRCS})

TARGET_LINK_LIBRARIES(ITKIOStreaming
  ITKniftiio
  ${ITK_ZLIB_LIBRARIES}
  ITKCommon
  )

IF(ITK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(ITKIO PROPERTIES ${ITK_LIBRARY_PROPERTIES})
ENDIF(ITK_LIBRARY_PROPERTIES)

