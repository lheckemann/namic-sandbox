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


set filename /Users/hjohnson/TESTER/BRAINSToSlicer3Scripting/Data/AVG_T1.nii.gz
set NodeName GoodBrain
set centered 1
set labelimage 0
##set logic [ $::slicer3::VolumesGUI GetLogic]
set logic ${BRAINSScriptingVolumesLogic}
set volumeNode [${logic} AddArchetypeVolume $fileName $centered ${labelimage} ${NodeName} ]
