##Note: This needs to be sourced first.

## Run with ~/src/Slicer3-build/Slicer3 -f ~/NewScript.tcl
#### UGLY GLOBAL VARIABLES
## If in gui mode
#if { HaveSlicerGUI then use that } {
#  set Scene $::slicer3::MRMLScene
#  set volumesLogic [ $::slicer3::VolumesGUI GetLogic]
#  set colorLogic [$::slicer3::ColorGUI GetLogic]

## If not in gui mode (--no_splash -f )
  set Scene [ vtkMRMLScene New ]
  set volumesLogic [ vtkSlicerVolumesLogic New ]
  ${volumesLogic} SetMRMLScene $Scene

#  puts "-----${volumesLogic}----${volumesLogic}----"

#### CREATE A BATCHMODE MRML FOR DOING ALL PROCESSING like a singleton
proc GetBatchVolumesLogic {} {
  global volumesLogic
  return ${volumesLogic}
}

proc GetBatchMRMLScene {} {
  global Scene
  return ${Scene}
}

###
#  All "Volumes" in this simplified interface, ensure that only
#  vtkMRMLScalarVolumeNode is a replacement to brains2.
############
############
############
## Simple Load image
proc b3_load_image { fileName {centered 1} {lableimage 0} {NodeName brain} } {
#  puts [ GetBatchVolumesLogic ]
  set volumeNode [ [ GetBatchVolumesLogic ] AddArchetypeVolume $fileName $centered ${lableimage} ${NodeName} ]
  return ${volumeNode}
}

## Simple Gaussian
proc b3_gaussian_filter { volumeNode {RadiusFactor 2.0} } {
  set GFilter [ vtkImageGaussianSmooth  New ]
  ${GFilter} SetInput [ ${volumeNode} GetImageData ]
  ${GFilter} SetRadiusFactor ${RadiusFactor}
  ${GFilter} Update
  set GFilterOutput [ ${GFilter} GetOutput ]
  set NewObjName NewObjName
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

