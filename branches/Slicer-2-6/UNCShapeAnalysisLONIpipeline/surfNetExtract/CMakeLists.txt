PROJECT( surfNetExtract ) 

# Include itk
INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
INCLUDE(${USE_ITK_FILE})  


#TLIB STUFF
ADD_DEFINITIONS(-DNO_AVS)

ADD_EXECUTABLE( surfnetExtract  surfnetExtract.cc util.cc TIntset.cc
TVertex.cc TVoxelVolume.cc TSurfacePatch.cc TNetExtractor.cc
TVertexArray.cc TIntList.cc TVertexList.cc TSurfaceNet.cc
TSegmentInfoList.cc)

TARGET_LINK_LIBRARIES ( surfnetExtract ${ITK_LIBRARIES} )



