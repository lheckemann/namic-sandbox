
#### UGLY GLOBAL VARIABLES
## If in gui mode
#if { HaveSlicerGUI then use that } {
  set Scene $::slicer3::MRMLScene
  set volumesLogic [ $::slicer3::VolumesGUI GetLogic]
  set colorLogic [$::slicer3::ColorGUI GetLogic]

## If not in gui mode (--nospash -f )
#  set Scene [ vtkMRMLScene New ]
#  set volumesLogic [ vtkSlicerVolumesLogic New ]
#  ${volumesLogic} SetMRMLScene $Scene

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
  puts "HERE4"
  ${GFilter} SetRadiusFactor ${RadiusFactor}
  ${GFilter} Update
  puts "HERE5"
  set GFilterOutput [ ${GFilter} GetOutput ]
  set NewObjName NewObjName
  puts "HERE6"
##Clone the input information
  set outputVolumeNode [ [ GetBatchVolumesLogic ] CloneVolume [ GetBatchMRMLScene ] ${volumeNode} ${NewObjName} ]
  puts "HERE7"
##Attach the image info
  ${outputVolumeNode} SetAndObserveImageData ${GFilterOutput}
  puts "HERE8"
  ${GFilter} Delete
  puts "HERE9"
  return ${outputVolumeNode}
}

proc b3_save_image { ImageToSave outputFileName } {
    set VolumeStorageNode [ vtkMRMLVolumeArchetypeStorageNode New ]
    $VolumeStorageNode SetFileName ${outputFileName}
    $VolumeStorageNode WriteData ${ImageToSave}
    $VolumeStorageNode Delete
}


#######
####### The Real Work
#######
#######  Copy Image
set fileName /Users/hjohnson/Desktop/TRACKHD_3TStudy/HDNI1/2007.05.04_09.22/ANONRAW/AVG_T1.nii.gz
set inputImage [ b3_load_image ${fileName} ]
set outputCopyFileName /Users/hjohnson/Desktop/NewCopyFile.nii.gz
b3_save_image ${inputImage} ${outputCopyFileName}
######   Create Gaussian Image
set GaussianImage [b3_gaussian_filter ${inputImage} ]
set outputGaussianFileName /Users/hjohnson/Desktop/NewGaussianFile.nii.gz
b3_save_image ${GaussianImage} ${outputGaussianFileName}

${inputImage} Delete
${GaussianImage} Delete

[ GetBatchMRMLScene ]  Delete
${volumesLogic} Delete
exit 0

proc dummy {} {
    [ vtkMRMLScalarVolumeNode New ]
### Need to copy the display informaiton.
  set newmatrix [ vtkMatrix4x4 New ]
  ${volumeNode} GetIJKToRASMatrix ${newmatrix}
  ${outputVolumeNode} SetIJKToRASMatrix ${newmatrix}
#  ${outputVolumeNode} Copy ${volumeNode}; ## This causes a bus error
## First let the Scene know about the Node
  [ GetBatchMRMLScene ] AddNode ${outputVolumeNode}
## Now let the node know about the Scene
#  ${outputVolumeNode} SetScene [ GetBatchMRMLScene ]
#  set OutputStorageNode [ vtkMRMLVolumeArchetypeStorageNode New ]
#  [ GetBatchMRMLScene ]  AddNode ${OutputStorageNode}
#  set OutputStorageNodeID [ ${OutputStorageNode} GetID ]
#  ${outputVolumeNode} SetReferenceStorageNodeID ${OutputStorageNodeID}
}
