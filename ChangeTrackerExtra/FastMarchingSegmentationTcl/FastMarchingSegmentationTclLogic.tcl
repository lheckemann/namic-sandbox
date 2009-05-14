#
# FastMarchingSegmentationTcl logic procs
#

#
# make a clone of the scripted module example with a new name
#
proc FastMarchingSegmentationTclClone { moduleName {slicerSourceDir ""} {targetDir ""} } {

  if { $slicerSourceDir == "" } {
    set slicerSourceDir $::env(Slicer3_HOME)/../Slicer3
  }
  if { $targetDir == "" } {
    set targetDir $slicerSourceDir/Modules/$moduleName
  }
  if { [file exists $targetDir] } {
    error "Target dir exists - please delete $targetDir"
  }
  puts "Making $targetDir"
  file mkdir $targetDir

  set files [glob $slicerSourceDir/Modules/FastMarchingSegmentationTcl/*]
  foreach f $files {
    set ff [file tail $f]
    if { [string match ".*" $ff] || [string match "*~" $ff] } {
      continue
    }
    set fp [open $f "r"]
    set contents [read $fp]
    close $fp
    regsub -all "FastMarchingSegmentationTcl" $contents $moduleName newContents

    regsub -all "FastMarchingSegmentationTcl" $ff $moduleName newFileName
    set fp [open $targetDir/$newFileName "w"]
    puts -nonewline $fp $newContents
    close $fp

    puts "  created file $newFileName"
  }
  puts "New module created - add entry to $slicerSourceDir/Modules/CMakeLists.txt"
}

#
# make a model of the current label map for the given slice logic
#
proc FastMarchingSegmentationTclAddQuickModel { sliceLogic } {

  #
  # get the image data for the label layer
  #
  set layerLogic [$sliceLogic GetLabelLayer]
  set volumeNode [$layerLogic GetVolumeNode]
  if { $volumeNode == "" } {
    puts "cannot make quick model - no volume node for $layerLogic in $sliceLogic"
    return
  }
  set imageData [$volumeNode GetImageData]

  #
  # make a poly data in RAS space
  #
  set dsm [vtkDiscreteMarchingCubes New]
  $dsm SetInput $imageData
  set tpdf [vtkTransformPolyDataFilter New]
  set ijkToRAS [vtkTransform New]
  $volumeNode GetRASToIJKMatrix [$ijkToRAS GetMatrix] 
  $ijkToRAS Inverse
  $tpdf SetInput [$dsm GetOutput]
  $tpdf SetTransform $ijkToRAS
  $tpdf Update

  #
  # create a mrml model for the new data
  #
  set modelNode [vtkMRMLModelNode New]
  set modelDisplayNode [vtkMRMLModelDisplayNode New]
  $modelNode SetName "QuickModel"
  $modelNode SetScene $::slicer3::MRMLScene
  $modelDisplayNode SetScene $::slicer3::MRMLScene
  eval $modelDisplayNode SetColor [lrange [FastMarchingSegmentationTclGetPaintColor $::FastMarchingSegmentationTcl(singleton)] 0 2]
  $::slicer3::MRMLScene AddNode $modelDisplayNode
  $modelNode SetAndObserveDisplayNodeID [$modelDisplayNode GetID]
  $::slicer3::MRMLScene AddNode $modelNode
  $modelNode SetAndObservePolyData [$tpdf GetOutput]

  #
  # clean up
  #
  $dsm Delete
  $ijkToRAS Delete
  $tpdf Delete
  $modelNode Delete
  $modelDisplayNode Delete

}

#
# TODO: flesh this out...
# - copy works, but image data is not correct somehow.
# - also need a GUI to access this function
#
proc FastMarchingSegmentationTclLabelCheckpoint {} {

  #
  # get the image data for the label layer
  #
  set sliceLogic [$::slicer3::ApplicationLogic GetSliceLogic "Red"]
  set layerLogic [$sliceLogic GetLabelLayer]
  set volumeNode [$layerLogic GetVolumeNode]
  if { $volumeNode == "" } {
    puts "cannot make label checkpoint - no volume node"
    return
  }

  # find a unique name for this copy
  set sourceName [$volumeNode GetName]
  set id 0
  while {1} {
    set targetName $sourceName-$id
    set nodes [$::slicer3::MRMLScene GetNodesByName $targetName]
    if { [$nodes GetNumberOfItems] == 0 } {
      break
    }
    incr id
  }

  set volumesLogic [$::slicer3::VolumesGUI GetLogic]
  set labelNode [$volumesLogic CloneVolume $::slicer3::MRMLScene $volumeNode $targetName]
}



#
# make it easier to test the model by looking for the first slice logic
#
proc FastMarchingSegmentationTclTestQuickModel {} {
  set sliceLogic [lindex [vtkSlicerSliceLogic ListInstances] 0]
  FastMarchingSegmentationTclAddQuickModel $sliceLogic
}

proc FastMarchingSegmentationTclInitializeFilter {this} {

  if { $::FastMarchingSegmentationTcl($this,fastMarchingFilter) != ""} {
    $::FastMarchingSegmentationTcl($this,fastMarchingFilter) Delete
  }

  set ::FastMarchingSegmentationTcl($this,fastMarchingFilter) [vtkFastMarching New]
  set ::FastMarchingSegmentationTcl($this,inputVolume) [ [[$this GetLogic] GetMRMLScene] \
    GetNodeByID [[$::FastMarchingSegmentationTcl($this,inputSelector) \
    GetSelected ] GetID] ]
  set inputVolume $::FastMarchingSegmentationTcl($this,inputVolume)

  FastMarchingSegmentationTclCreateLabelVolume $this
  set labelVolume $::FastMarchingSegmentationTcl($this,labelVolume)

  if { $inputVolume == ""} {
    puts "Something is wrong with the input volume"
    return
  }

  set inputImageData [$inputVolume GetImageData]
  scan [$inputImageData GetScalarRange] "%f%f" rangeLow rangeHigh
  scan [$inputVolume GetSpacing] "%f%f%f" dx dy dz
  puts "Input spacing: [$inputVolume GetSpacing]"
  set dim [$inputImageData GetWholeExtent]

  puts "Input image range: $rangeLow to $rangeHigh"
  
  set cast $::FastMarchingSegmentationTcl($this,cast) 
  set fmFilter $::FastMarchingSegmentationTcl($this,fastMarchingFilter)
  $cast SetOutputScalarTypeToShort
  $cast SetInput $inputImageData

  $fmFilter SetOutput [$labelVolume GetImageData]

  $::FastMarchingSegmentationTcl($this,fastMarchingFilter) SetInput [$cast GetOutput]
  
  puts "Passing the following parameters: [expr [lindex \
    $dim 1] + 1] [expr [lindex $dim 3] + 1] [expr [lindex $dim 5] + 1] [expr int($rangeHigh)] $dx $dy $dz"

  $fmFilter init [expr [lindex $dim 1] + 1] [expr [lindex $dim 3] + 1] \
    [expr [lindex $dim 5] + 1] [ expr int($rangeHigh)] $dx $dy $dz

  $fmFilter SetInput [$cast GetOutput]
  $fmFilter Modified
  $fmFilter Update
  puts "fmFilter completed"

  $fmFilter setActiveLabel 1
  $fmFilter initNewExpansion
  puts "New expansion inited"
}

proc ConversiontomL {this Voxels} {
  set inputVolume $::FastMarchingSegmentationTcl($this,inputVolume)

  scan [$inputVolume GetSpacing] "%f%f%f" dx dy dz
  set voxelvolume [expr $dx * $dy * $dz]
  set conversion 1000
      
  set voxelamount [expr $Voxels * $voxelvolume]
  set mL [expr round($voxelamount) / $conversion]

  return $mL
}

proc ConversiontoVoxels {this mL} {
  set inputVolume $::FastMarchingSegmentationTcl($this,inputVolume)

  scan [$inputVolume GetSpacing] "%f %f %f" dx dy dz
  set voxelvolume [expr $dx * $dy * $dz]
  set conversion 1000
      
  set voxelamount [expr $mL / $voxelvolume]
  set Voxels [expr round($voxelamount) * $conversion]

  return $Voxels
}

proc FastMarchingSegmentationTclExpand {this} {
  set expectedVolumeValue [$::FastMarchingSegmentationTcl($this,expectedVolume) GetValue]
  set voxelnumber [ConversiontoVoxels $this $expectedVolumeValue]
  set fmFilter $::FastMarchingSegmentationTcl($this,fastMarchingFilter)
  $fmFilter setNPointsEvolution $voxelnumber
  puts "Before calling Segment"
  FastMarchingSegmentationTclSegment $this
}

proc FastMarchingSegmentationTclSegment {this} {
  set fmFilter $::FastMarchingSegmentationTcl($this,fastMarchingFilter)
  set cast $::FastMarchingSegmentationTcl($this,cast)
  set inputVolume $::FastMarchingSegmentationTcl($this,inputVolume)
  set inputFiducials $::FastMarchingSegmentationTcl($this,inputFiducials)
  set labelVolume $::FastMarchingSegmentationTcl($this,labelVolume)
  puts "Inside Segment"
  set ras2ijk [vtkMatrix4x4 New]
  $inputVolume GetRASToIJKMatrix $ras2ijk
  puts "Here"
  
  $fmFilter setRAStoIJKmatrix \
    [$ras2ijk GetElement 0 1]  [$ras2ijk GetElement 0 2]  [$ras2ijk GetElement 0 3] [$ras2ijk GetElement 0 4] \
    [$ras2ijk GetElement 1 1]  [$ras2ijk GetElement 1 2]  [$ras2ijk GetElement 1 3] [$ras2ijk GetElement 1 4] \
    [$ras2ijk GetElement 2 1]  [$ras2ijk GetElement 2 2]  [$ras2ijk GetElement 2 3] [$ras2ijk GetElement 2 4] \
    [$ras2ijk GetElement 3 1]  [$ras2ijk GetElement 3 2]  [$ras2ijk GetElement 3 3] [$ras2ijk GetElement 3 4];
  puts "Here 2"
  set numFiducials [$inputFiducials GetNumberOfFiducials]
  puts "Fiducial list has $numFiducials fiducials"
  
  for {set i 0} {$i<$numFiducials} {incr i} {
    scan [$inputFiducials GetNthFiducialXYZ $i] "%f %f %f" fx fy fz
    puts "Coordinates of $i th fiducial: $fx $fy $fz"
    set fIJK [lrange [eval $ras2ijk MultiplyPoint $fx $fy $fz 1] 0 2]
    $fmFilter addSeedIJK [expr int([expr [lindex $fIJK 0]])] [expr int([expr [lindex $fIJK 1]])] \
      [expr int([expr [lindex $fIJK 2]])]
  }

  puts "Fiducials initialized"
  $cast SetInput [$inputVolume GetImageData]
  $fmFilter Modified
  $fmFilter SetInput [$cast GetOutput]
  $fmFilter SetOutput [$labelVolume GetImageData]
  $fmFilter Modified
  puts "Before fm update"
  $fmFilter Update
  puts "After fm update"

  $fmFilter show 1
  $fmFilter Modified
  $fmFilter Update
  puts "After show update"

  vtkNRRDWriter fmwriter
  fmwriter SetInput [$fmFilter GetOutput]
  fmwriter SetFileName "/tmp/fm_output.nrrd"
  fmwriter Write
  fmwriter Delete
}

proc FastMarchingSegmentationTclUpdateTime {this} {
  set requestedTime [$::FastMarchingSegmentationTcl($this,timeScroll) GetValue]
  set fmFilter $::FastMarchingSegmentationTcl($this,fastMarchingFilter)
  set labelVolume $::FastMarchingSegmentationTcl($this,labelVolume)
  $fmFilter show $requestedTime
  $fmFilter Modified
  $fmFilter Update
  puts "After updating for time scroll"
#  [[$this GetLogic] GetApplicationLogic]  PropagateVolumeSelection
  $labelVolume Modified
}

proc FastMarchingSegmentationTclCreateLabelVolume {this} {
  set volumeNode $::FastMarchingSegmentationTcl($this,inputVolume)
  set scene [[$this GetLogic] GetMRMLScene]

  set volumesLogic [$::slicer3::VolumesGUI GetLogic]
  set labelVolumeName [[$::FastMarchingSegmentationTcl($this,outputLabelText) GetWidget] GetValue]
  set labelNode [$volumesLogic CreateLabelVolume $scene $volumeNode $labelVolumeName]

  # make the source node the active background, and the label node the active label
  set selectionNode [[[$this GetLogic] GetApplicationLogic]  GetSelectionNode]
  $selectionNode SetReferenceActiveVolumeID [$volumeNode GetID]
  $selectionNode SetReferenceActiveLabelVolumeID [$labelNode GetID]
  $selectionNode Modified
#  [[$this GetLogic] GetApplicationLogic]  PropagateVolumeSelection

  set ::FastMarchingSegmentationTcl($this,labelVolume) $labelNode
  
  set labelVolumeName [$labelNode GetName]
  $::FastMarchingSegmentationTcl($this,currentOutputText) SetText "Current ouput volume: $labelVolumeName"

  # update the editor range to be the full range of the background image
  set range [[$volumeNode GetImageData] GetScalarRange]
  eval ::Labler::SetPaintRange $range
}

