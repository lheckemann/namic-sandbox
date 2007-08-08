## Note: This needs to be sourced first.
## NOTE:  The DISPLAY environmental variable MUST BE SET TO A VALID DISPLAY!
## Run with ~/src/Slicer3-build/Slicer3 --no_splash -f ~/NewScript.tcl
#### UGLY GLOBAL VARIABLES
## The following logic seems wrong, but appears to work in GUI mode, but not batch mode.
if { [ info exists ::slicer3::MRMLScene ] == 1 } {
  ## If in gui mode
  puts "SETTING MRML TO THE DEFAULT SLICER SCENE"
  puts "SETTING MRML TO THE DEFAULT SLICER SCENE"
  puts "SETTING MRML TO THE DEFAULT SLICER SCENE"
  puts "SETTING MRML TO THE DEFAULT SLICER SCENE"
  set BRAINSScriptingScene $::slicer3::MRMLScene
  set BRAINSScriptingVolumesLogic [ $::slicer3::VolumesGUI GetLogic]
  set BRAINSScriptingColorLogic [$::slicer3::ColorGUI GetLogic]
  set BRAINSScriptingMRMLisGUI "true"
} else {
  ## If not in gui mode (--no_splash -f )
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  set BRAINSScriptingScene [ vtkMRMLScene New ]
  set BRAINSScriptingVolumesLogic [ vtkSlicerVolumesLogic New ]
  ${BRAINSScriptingVolumesLogic} SetMRMLScene $BRAINSScriptingScene
  set BRAINSScriptingMRMLisGUI "false"
}

#### CREATE A BATCHMODE MRML FOR DOING ALL PROCESSING like a singleton
proc GetBatchVolumesLogic {} {
#  global BRAINSScriptingVolumesLogic
#  ${BRAINSScriptingVolumesLogic}
  return $::BRAINSScriptingVolumesLogic
}

proc GetBatchMRMLScene {} {
#  global BRAINSScriptingScene
#  ${BRAINSScriptingScene}
  return $::BRAINSScriptingScene
}

proc DestroyBatchMRMLScene {} {
## Only delete if not in gui mode, else let the gui handle destruction.
  if {$::BRAINSScriptingMRMLisGUI == "false" } {
     [ GetBatchMRMLScene ]  Delete
     if { [ info exists BRAINSScriptingVolumesLogic ] } {
       puts "Deleting BRAINSScriptingVolumesLogic"
       ${BRAINSScriptingVolumesLogic} Delete
     } else {
       puts "Not deleting BRAINSScriptingVolumesLogic"
     }
  }
}

###
#  All "Volumes" in this simplified interface, ensure that only
#  vtkMRMLScalarVolumeNode is a replacement to brains2.
############
############
############
## Simple Load image
proc b3_load_image { fileName {centered 1} {labelimage 0} {NodeName brain} } {
#  puts [ GetBatchVolumesLogic ]
  puts "set volumeNode [ [ GetBatchVolumesLogic ] AddArchetypeVolume $fileName $centered ${labelimage} ${NodeName} ]"
  set volumeNode [ [ GetBatchVolumesLogic ] AddArchetypeVolume $fileName $centered ${labelimage} ${NodeName} ]
  return ${volumeNode}
}

## Simple Gaussian
proc b3_gaussian_filter { volumeNode {RadiusFactor 2.0} } {
  set GFilter [ vtkImageGaussianSmooth  New ]
  ${GFilter} SetInput [ ${volumeNode} GetImageData ]
  ${GFilter} SetRadiusFactor ${RadiusFactor}
  ${GFilter} Update
  set GFilterOutput [ ${GFilter} GetOutput ]
  set NewObjName GaussianObjectOutput
##Clone the input information
  set outputVolumeNode [ [ GetBatchVolumesLogic ] CloneVolume [ GetBatchMRMLScene ] ${volumeNode} ${NewObjName} ]
##Attach the image info
  ${outputVolumeNode} SetAndObserveImageData ${GFilterOutput}
  ${GFilter} Delete
  return ${outputVolumeNode}
}

proc b3_save_image { ImageToSave outputFileName } {
    set VolumeStorageNode [ vtkMRMLVolumeArchetypeStorageNode New ]
    $VolumeStorageNode SetFileName ${outputFileName}
    $VolumeStorageNode WriteData ${ImageToSave}
    $VolumeStorageNode Delete
}

proc b2_get_image_res { volumeNode } {
set res [ $volumeNode GetSpacing ];
}

proc b2_get_image_dims { volumeNode } {
set dims [ [ $volumeNode GetImageData ] GetDimensions ];
}



