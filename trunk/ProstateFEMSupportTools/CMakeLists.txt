project(ProstateFEMSupportTools)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(mesher-reader mesher-reader.cxx)
target_link_libraries(mesher-reader ITKCommon ITKIO vtkIO vtkGraphics)

add_executable(prostateAddSurfDispl prostateAddSurfDispl.cxx)
target_link_libraries(prostateAddSurfDispl ITKCommon ITKIO vtkIO vtkGraphics)

add_executable(prostateAddVolDispl prostateAddVolDispl.cxx)
target_link_libraries(prostateAddVolDispl ITKCommon ITKIO vtkIO vtkGraphics)

add_executable(imageMeshResampler imageMeshResampler.cxx)
target_link_libraries(imageMeshResampler ITKCommon ITKIO vtkIO vtkGraphics)

add_executable(ProstateRegionIdentifier ProstateRegionIdentifier.cxx)
target_link_libraries(ProstateRegionIdentifier ITKCommon ITKIO vtkIO vtkGraphics)
