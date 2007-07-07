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
