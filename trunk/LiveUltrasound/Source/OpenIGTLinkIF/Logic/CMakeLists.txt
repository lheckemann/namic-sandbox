project(OpenIGTLinkIFModuleLogic)


# --------------------------------------------------------------------------
# Check if version 2 protocol is available
# Features that depnd on OpenIGTLink Protocol 2 are only available 
# if the OpenIGTLink library is compiled with OpenIGTLink_PROTOCOL_VERSION_2
# option.

if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_USE_VERSION_2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 2")
else (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 1")
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2) 

#
# See CMake/SlicerMacroBuildModuleLogic.cmake for details
#

SET(module_logic_name "${PROJECT_NAME}")

# The header '${PROJECT_NAME}Export.h' will be automatically configured.
SET(module_logic_export_directive "VTK_SLICER_OPENIGTLINKIF_MODULE_LOGIC_EXPORT")

# Additional directories to include
SET(module_logic_include_directories
  ${OpenIGTLink_INCLUDE_DIRS}
  )

# Source files
SET(module_logic_SRCS
  vtkIGTLCircularBuffer.cxx 
  vtkIGTLToMRMLBase.cxx 
  vtkIGTLToMRMLImage.cxx 
  vtkIGTLToMRMLLinearTransform.cxx
  #vtkIGTLToMRMLPoint.cxx   
  vtkIGTLToMRMLPosition.cxx 
  vtkMRMLIGTLConnectorNode.cxx 
  vtkOpenIGTLinkIFLogic.cxx 
  )

if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  set(module_logic_SRCS 
    ${module_logic_SRCS}
    vtkMRMLIGTLQueryNode.cxx
    vtkMRMLImageMetaListNode.cxx
    vtkIGTLToMRMLImageMetaList.cxx
    #vtkMRMLIGTLImageMetaListQueryNode.cxx
    vtkIGTLToMRMLTrackingData.cxx
    vtkMRMLIGTLTrackingDataQueryNode.cxx 
    vtkMRMLIGTLTrackingDataBundleNode.cxx
    )
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)

# Additional Target libraries
SET(module_logic_target_libraries
  ${ITK_LIBRARIES}
  ${OpenIGTLink_LIBRARIES}
  )


SlicerMacroBuildModuleLogic(
  NAME ${module_logic_name}
  EXPORT_DIRECTIVE ${module_logic_export_directive}
  INCLUDE_DIRECTORIES ${module_logic_include_directories}
  SRCS ${module_logic_SRCS}
  TARGET_LIBRARIES ${module_logic_target_libraries}
  )
