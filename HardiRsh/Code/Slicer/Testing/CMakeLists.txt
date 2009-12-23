
#FIND_PACKAGE(ITK REQUIRED)
#INCLUDE(${ITK_USE_FILE})

#ADD_EXECUTABLE(SphereHarmonicTest SphereHarmonicTest.cxx)
#TARGET_LINK_LIBRARIES( SphereHarmonicTest vtkRendering ${lib_name} )

ADD_EXECUTABLE(GlyphTest GlyphTest.cxx )
TARGET_LINK_LIBRARIES( GlyphTest vtkRendering ${lib_name} ITKIO ${VTK_LIBRARIES} )

