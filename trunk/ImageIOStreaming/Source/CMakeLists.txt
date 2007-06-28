
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

ADD_LIBRARY(ITKIOMetaStreaming SHARED 
  itkMetaImageIO.cxx
  itkMetaImageIOFactory.cxx
  )

ADD_LIBRARY(ITKIOYAFFStreaming SHARED 
  itkYAFFImageIO.cxx
  itkYAFFImageIOFactory.cxx
  )


TARGET_LINK_LIBRARIES(ITKIOStreaming
  ITKniftiio
  ${ITK_ZLIB_LIBRARIES}
  ITKCommon
  )

TARGET_LINK_LIBRARIES(ITKIOMetaStreaming
  ITKIOStreaming
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

