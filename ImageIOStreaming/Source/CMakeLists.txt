
SET(ITKIO_SRCS
  itkIOCommon.cxx
  itkNiftiImageIO.cxx
  itkNiftiImageIOFactory.cxx
  itkAnalyzeObjectLabelMapImageIO.cxx
  itkAnalyzeObjectLabelMapImageIOFactory.cxx
  itkMetaImageIO.cxx
  itkMetaImageIOFactory.cxx
  itkImageIOBase.cxx
  itkImageIOFactory.cxx
  itkImageIORegion.cxx
  objectentry.cxx
  objectmap.cxx
  ImageToObjectmap.cxx
)

# Add ITKIO library
ADD_LIBRARY(ITKIOStreaming ${ITKIO_SRCS})


#
# Illustration on how to create a dynamically loadable factory
#
ADD_LIBRARY(ITKIOYAFFStreaming SHARED
  itkYAFFImageIO.cxx
  itkYAFFImageIOFactory.cxx
  )

TARGET_LINK_LIBRARIES(ITKIOStreaming
  ITKniftiio
  ${ITK_ZLIB_LIBRARIES}
  ITKCommon
  ITKMetaIO
  )

TARGET_LINK_LIBRARIES(ITKIOYAFFStreaming
  ITKIOStreaming
  ITKCommon
  )

IF(ITK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(ITKIO PROPERTIES ${ITK_LIBRARY_PROPERTIES})
ENDIF(ITK_LIBRARY_PROPERTIES)

