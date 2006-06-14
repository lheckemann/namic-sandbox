SET(f "conformalFlatteningITK")
SET(a "conformalFlatteningITK.out")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

PROJECT(${f})
INCLUDE_REGULAR_EXPRESSION("^.*$")

SET(VTK_DIR ${VTK_BINARY_DIR})

##this is the setting on ygao's computer...
#SET(VTK_DIR /u4/home/gth818n/usr/local/lib/vtk-5.0)
#SET(ITK_DIR /u4/home/gth818n/usr/local/lib/InsightToolkit)

SET(TEMP ${ITK_BINARY_DIR}/Testing/Temporary)

INCLUDE_DIRECTORIES (${InsightApplications_Source_DIR}/Auxiliary/vtk/)

FIND_PACKAGE (ITK)
IF (ITK_FOUND)
   INCLUDE(${USE_ITK_FILE})
ENDIF(ITK_FOUND) 


FIND_PACKAGE (VTK)
IF (VTK_FOUND)
   INCLUDE(${USE_VTK_FILE})
ENDIF(VTK_FOUND) 


#SET(FILES "Cone;ImportExport")
#FOREACH(f ${FILES})
 
  ADD_EXECUTABLE(${a} ${f}.cxx)
  TARGET_LINK_LIBRARIES(${a} vtkRendering vtkIO ITKCommon ITKBasicFilters ITKIO)

#ENDFOREACH(f)




# ADD_EXECUTABLE(${f} ${f}.cpp)
#TARGET_LINK_LIBRARIES(${f} vtkRendering vtkIO) # ITKCommon ITKBasicFilters ITKIO)
# INCLUDE_DIRECTORIES(/usr/local/include/InsightToolkit/Common /usr/local/include/InsightToolkit )
# ADD_EXECUTABLE(Image3 Image3.cxx )
#TARGET_LINK_LIBRARIES(Image3 ITKCommon ITKBasicFilters ITKIO)


