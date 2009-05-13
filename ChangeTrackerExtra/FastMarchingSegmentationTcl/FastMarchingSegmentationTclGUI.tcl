#
# FastMarchingSegmentationTcl GUI Procs
# - the 'this' argument to all procs is a vtkScriptedModuleGUI
#

proc FastMarchingSegmentationTclConstructor {this} {
}

proc FastMarchingSegmentationTclDestructor {this} {
}

proc FastMarchingSegmentationTclTearDownGUI {this} {


  # nodeSelector  ;# disabled for now
  set widgets {
    run volumesSelect volumeOutputSelect fiducialsSelect expectedVolume
    timeScroll
  }

  foreach w $widgets {
    $::FastMarchingSegmentationTcl($this,$w) SetParent ""
    $::FastMarchingSegmentationTcl($this,$w) Delete
  }

  $::FastMarchingSegmentationTcl($this,cast) Delete

  if { [[$this GetUIPanel] GetUserInterfaceManager] != "" } {
    set pageWidget [[$this GetUIPanel] GetPageWidget "FastMarchingSegmentationTcl"]
    [$this GetUIPanel] RemovePage "FastMarchingSegmentationTcl"
  }

  unset ::FastMarchingSegmentationTcl(singleton)

}

proc FastMarchingSegmentationTclBuildGUI {this} {
  load $::Slicer3_HOME/bin/libFastMarching.so
  puts "Library loaded"
  package require FastMarching

  if { [info exists ::FastMarchingSegmentationTcl(singleton)] } {
    error "FastMarchingSegmentationTcl singleton already created"
  }
  set ::FastMarchingSegmentationTcl(singleton) $this

  #
  # create and register the node class
  # - since this is a generic type of node, only do it if 
  #   it hasn't already been done by another module
  #

  set mrmlScene [[$this GetLogic] GetMRMLScene]
  set tag [$mrmlScene GetTagByClassName "vtkMRMLScriptedModuleNode"]
  if { $tag == "" } {
    set node [vtkMRMLScriptedModuleNode New]
    $mrmlScene RegisterNodeClass $node
    $node Delete
  }


  $this SetCategory "Segmentation"
  [$this GetUIPanel] AddPage "FastMarchingSegmentationTcl" "FastMarchingSegmentationTcl" ""
  set pageWidget [[$this GetUIPanel] GetPageWidget "FastMarchingSegmentationTcl"]

  #
  # help frame
  #
  set helptext "The FastMarchingSegmentationTcl performs segmentation using fast marching method with automatic estimation of region statistics. This module has been originally implemented by Eric Pichon in slicer2."
  set abouttext "This work is supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See <a>http://www.slicer.org</a> for details."
  $this BuildHelpAndAboutFrame $pageWidget $helptext $abouttext

  #
  # FastMarchingSegmentationTcl Volumes
  #
  set ::FastMarchingSegmentationTcl($this,dataFrame) [vtkSlicerModuleCollapsibleFrame New]
  set frame $::FastMarchingSegmentationTcl($this,dataFrame)
  $frame SetParent $pageWidget
  $frame Create
  $frame SetLabelText "Volumes"
  pack [$frame GetWidgetName] \
    -side top -anchor nw -fill x -padx 2 -pady 2 -in [$pageWidget GetWidgetName]

  set ::FastMarchingSegmentationTcl($this,volumesSelect) [vtkSlicerNodeSelectorWidget New]
  set select $::FastMarchingSegmentationTcl($this,volumesSelect)
  $select SetParent [$frame GetFrame]
  $select Create
  $select SetNodeClass "vtkMRMLScalarVolumeNode" "" "" ""
  $select SetMRMLScene [[$this GetLogic] GetMRMLScene]
  $select UpdateMenu
  $select SetLabelText "Source Volume:"
  $select SetBalloonHelpString "The Source Volume to operate on"
  pack [$select GetWidgetName] -side top -anchor e -padx 2 -pady 2 

  set ::FastMarchingSegmentationTcl($this,fiducialsSelect) [vtkSlicerNodeSelectorWidget New]
  set fiducials $::FastMarchingSegmentationTcl($this,fiducialsSelect)
  $fiducials SetParent [$frame GetFrame]
  $fiducials Create
  $fiducials NewNodeEnabledOn
  $fiducials SetNodeClass "vtkMRMLFiducialListNode" "" "" ""
  $fiducials SetMRMLScene [[$this GetLogic] GetMRMLScene]
  $fiducials UpdateMenu
  $fiducials SetLabelText "Seeds:"
  $fiducials SetBalloonHelpString "List of fiducials to be used as seeds for segmentation"
  pack [$fiducials GetWidgetName] -side top -anchor e -padx 2 -pady 2 

  set ::FastMarchingSegmentationTcl($this,volumeOutputSelect) [vtkSlicerNodeSelectorWidget New]
  set outSelect $::FastMarchingSegmentationTcl($this,volumeOutputSelect)
  $outSelect SetParent [$frame GetFrame]
  $outSelect Create
  $outSelect NewNodeEnabledOn
  $outSelect SetNodeClass "vtkMRMLScalarVolumeNode" "" "" ""
  $outSelect SetMRMLScene [[$this GetLogic] GetMRMLScene]
  $outSelect UpdateMenu
  $outSelect SetLabelText "Output Segmented Volume:"
  $outSelect SetBalloonHelpString "Result of the segmentation"
  pack [$outSelect GetWidgetName] -side top -anchor e -padx 2 -pady 2 

  set ::FastMarchingSegmentationTcl($this,expectedVolume) [vtkKWThumbWheel New]
  set segvolume $::FastMarchingSegmentationTcl($this,expectedVolume)
  $segvolume SetParent [$frame GetFrame]
  $segvolume PopupModeOff
  $segvolume Create
  $segvolume DisplayEntryAndLabelOnTopOn
  $segvolume DisplayEntryOn
  $segvolume DisplayLabelOn
  $segvolume SetResolution 10
  $segvolume SetMinimumValue 0
  $segvolume SetClampMinimumValue 0
  $segvolume SetValue 0
  [$segvolume GetLabel] SetText "Target segmented volume (mm^3):"
  $segvolume SetBalloonHelpString "Maximum expected volume of the segmented structure"
  pack [$segvolume GetWidgetName] -side top -anchor e -padx 2 -pady 2 

  set ::FastMarchingSegmentationTcl($this,timeScroll) [vtkKWScale New]
  set timescroll $::FastMarchingSegmentationTcl($this,timeScroll)
  $timescroll SetParent [$frame GetFrame]
  $timescroll Create
  $timescroll SetRange 0.0 1.0
  $timescroll SetResolution 0.001
  $timescroll SetLength 150
  $timescroll SetLabelText "Timepoint:"
  $timescroll SetBalloonHelpString "Scroll back in segmentation process"
  pack [$timescroll GetWidgetName] -side top -anchor e -padx 2 -pady 2

  set ::FastMarchingSegmentationTcl($this,run) [vtkKWPushButton New]
  set run $::FastMarchingSegmentationTcl($this,run)
  $run SetParent [$frame GetFrame]
  $run Create
  $run SetText "Run"
  $run SetBalloonHelpString "Apply algorithm."
  pack [$run GetWidgetName] -side top -anchor e -padx 2 -pady 2 


  set ::FastMarchingSegmentationTcl($this,cast) [vtkImageCast New]
  set ::FastMarchingSegmentationTcl($this,fastMarchingFilter) ""
}

proc FastMarchingSegmentationTclAddGUIObservers {this} {
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,run) 10000 
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,expectedVolume) 10000
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,timeScroll) 10000
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,volumesSelect)  11000
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,volumeOutputSelect)  11000
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,volumeOutputSelect)  11001
  $this AddObserverByNumber $::FastMarchingSegmentationTcl($this,fiducialsSelect)  11000
  $this AddMRMLObserverByNumber [[[$this GetLogic] GetApplicationLogic] GetSelectionNode] 31
    
}

proc FastMarchingSegmentationTclRemoveGUIObservers {this} {
  $this RemoveMRMLObserverByNumber [[[$this GetLogic] GetApplicationLogic] GetSelectionNode] 31
}

proc FastMarchingSegmentationTclRemoveLogicObservers {this} {
}

proc FastMarchingSegmentationTclRemoveMRMLNodeObservers {this} {
}

proc FastMarchingSegmentationTclProcessLogicEvents {this caller event} {
}

proc FastMarchingSegmentationTclProcessGUIEvents {this caller event} {
  
  if { $caller == $::FastMarchingSegmentationTcl($this,run) } {
    puts "Run button clicked"
    FastMarchingSegmentationTclExpand $this 
  } 

  if { $caller == $::FastMarchingSegmentationTcl($this,expectedVolume) } {
        set expectedVolumeValue [$::FastMarchingSegmentationTcl($this,expectedVolume) GetValue]
        puts "Current value of expected segmentation volume: $expectedVolumeValue"
  } 

  if { $caller == $::FastMarchingSegmentationTcl($this,timeScroll) } {
    set requestedTime [$::FastMarchingSegmentationTcl($this,timeScroll) GetValue]
    puts "Current value of time scroll: $requestedTime"
    FastMarchingSegmentationTclUpdateTime $this
  } 

  if {$caller == $::FastMarchingSegmentationTcl($this,volumesSelect) } {
    puts "volumesSelect event received $event"
    set selVol [[$::FastMarchingSegmentationTcl($this,volumesSelect) GetSelected] GetID]
    puts "selected volume ID: $selVol"
    FastMarchingSegmentationTclInitializeFilter $this \
     [[$::FastMarchingSegmentationTcl($this,volumesSelect) GetSelected] GetID] \
     [[$::FastMarchingSegmentationTcl($this,fiducialsSelect) GetSelected] GetID]
  }

  if {$caller == $::FastMarchingSegmentationTcl($this,fiducialsSelect) } {
    puts "fiducialsSelect event received"
  }

  if {$caller == $::FastMarchingSegmentationTcl($this,volumeOutputSelect) } {
    switch $event {
      11000 {
        puts "volumesSelect event received"
      }
      11001 {
        puts "volumesSelect new node created"
      }
    }
  }

  FastMarchingSegmentationTclUpdateMRML $this
}

#
# Accessors to FastMarchingSegmentationTcl state
#


# get the FastMarchingSegmentationTcl parameter node, or create one if it doesn't exist
proc FastMarchingSegmentationTclCreateParameterNode {} {
  set node [vtkMRMLScriptedModuleNode New]
  $node SetModuleName "FastMarchingSegmentationTcl"

  # set node defaults
  $node SetParameter label 1

  $::slicer3::MRMLScene AddNode $node
  $node Delete
}

# get the FastMarchingSegmentationTcl parameter node, or create one if it doesn't exist
proc FastMarchingSegmentationTclGetParameterNode {} {

  set node ""
  set nNodes [$::slicer3::MRMLScene GetNumberOfNodesByClass "vtkMRMLScriptedModuleNode"]
  for {set i 0} {$i < $nNodes} {incr i} {
    set n [$::slicer3::MRMLScene GetNthNodeByClass $i "vtkMRMLScriptedModuleNode"]
    if { [$n GetModuleName] == "FastMarchingSegmentationTcl" } {
      set node $n
      break;
    }
  }

  if { $node == "" } {
    FastMarchingSegmentationTclCreateParameterNode
    set node [FastMarchingSegmentationTclGetParameterNode]
  }

  return $node
}


proc FastMarchingSegmentationTclGetLabel {} {
  set node [FastMarchingSegmentationTclGetParameterNode]
  if { [$node GetParameter "label"] == "" } {
    $node SetParameter "label" 1
  }
  return [$node GetParameter "label"]
}

proc FastMarchingSegmentationTclSetLabel {index} {
  set node [FastMarchingSegmentationTclGetParameterNode]
  $node SetParameter "label" $index
}

#
# MRML Event processing
#

proc FastMarchingSegmentationTclUpdateMRML {this} {
}

proc FastMarchingSegmentationTclProcessMRMLEvents {this callerID event} {

    set caller [[[$this GetLogic] GetMRMLScene] GetNodeByID $callerID]
    if { $caller == "" } {
        return
    }
}

proc FastMarchingSegmentationTclEnter {this} {
}

proc FastMarchingSegmentationTclExit {this} {
}

proc FastMarchingSegmentationTclProgressEventCallback {filter} {

  set mainWindow [$::slicer3::ApplicationGUI GetMainSlicerWindow]
  set progressGauge [$mainWindow GetProgressGauge]
  set renderWidget [[$::slicer3::ApplicationGUI GetViewControlGUI] GetNavigationWidget]

  if { $filter == "" } {
    $mainWindow SetStatusText ""
    $progressGauge SetValue 0
    $renderWidget SetRendererGradientBackground 0
  } else {
    # TODO: this causes a tcl 'update' which re-triggers the module (possibly changing
    # values while it is executing!  Talk about evil...
    #$mainWindow SetStatusText [$filter GetClassName]
    #$progressGauge SetValue [expr 100 * [$filter GetProgress]]

    set progress [$filter GetProgress]
    set remaining [expr 1.0 - $progress]

    #$renderWidget SetRendererGradientBackground 1
    #$renderWidget SetRendererBackgroundColor $progress $progress $progress
    #$renderWidget SetRendererBackgroundColor2 $remaining $remaining $remaining
  }
}

proc FastMarchingSegmentationTclApply {this} {

  if { ![info exists ::FastMarchingSegmentationTcl($this,processing)] } { 
    set ::FastMarchingSegmentationTcl($this,processing) 0
  }

  if { $::FastMarchingSegmentationTcl($this,processing) } {
    return
  }

  set volumeNode [$::FastMarchingSegmentationTcl($this,volumesSelect) GetSelected]
  set ijkToRAS [vtkMatrix4x4 New]
  $volumeNode GetIJKToRASMatrix $ijkToRAS
  set outVolumeNode [$::FastMarchingSegmentationTcl($this,volumeOutputSelect) GetSelected]
  set outModelNode [$::FastMarchingSegmentationTcl($this,modelOutputSelect) GetSelected]
  set value [$::FastMarchingSegmentationTcl($this,thresholdSelect) GetValue]
  set minFactor [$::FastMarchingSegmentationTcl($this,minFactor) GetValue]
  set cast $::FastMarchingSegmentationTcl($this,cast)
  set dt $::FastMarchingSegmentationTcl($this,distanceTransform)
  set resample $::FastMarchingSegmentationTcl($this,resample)
  set cubes $::FastMarchingSegmentationTcl($this,marchingCubes)
  set changeIn $::FastMarchingSegmentationTcl($this,changeIn)
  set changeOut $::FastMarchingSegmentationTcl($this,changeOut)
  set polyTransformFilter $::FastMarchingSegmentationTcl($this,polyTransformFilter)
  set polyTransform $::FastMarchingSegmentationTcl($this,polyTransform)

  #
  # check that inputs are valid
  #
  set errorText ""
  if { $volumeNode == "" || [$volumeNode GetImageData] == "" } {
    set errorText "No input volume data..."
  }
  if { $outVolumeNode == "" } {
    set errorText "No output volume node..."
  }
  if { $outModelNode == "" } {
    set errorText "No output model node..."
  }

  if { $errorText != "" } {
    set dialog [vtkKWMessageDialog New]
    $dialog SetParent [$::slicer3::ApplicationGUI GetMainSlicerWindow]
    $dialog SetMasterWindow [$::slicer3::ApplicationGUI GetMainSlicerWindow]
    $dialog SetStyleToMessage
    $dialog SetText $errorText
    $dialog Create
    $dialog Invoke
    $dialog Delete
    return
  }

  set ::FastMarchingSegmentationTcl($this,processing) 1

  #
  # configure the pipeline
  #
  $changeIn SetInput [$volumeNode GetImageData]
  eval $changeIn SetOutputSpacing [$volumeNode GetSpacing]
  $cast SetInput [$changeIn GetOutput]
  $cast SetOutputScalarTypeToFloat
  $dt SetInput [$cast GetOutput]
  $dt SetSquaredDistance 0
  $dt SetUseImageSpacing 1
  $dt SetInsideIsPositive 0
  $changeOut SetInput [$dt GetOutput]
  $changeOut SetOutputSpacing 1 1 1
  $resample SetInput [$dt GetOutput]
  $resample SetAxisMagnificationFactor 0 $minFactor
  $resample SetAxisMagnificationFactor 1 $minFactor
  $resample SetAxisMagnificationFactor 2 $minFactor
  $cubes SetInput [$resample GetOutput]
  $cubes SetValue 0 $value
  $polyTransformFilter SetInput [$cubes GetOutput]
  $polyTransformFilter SetTransform $polyTransform
  set magFactor [expr 1.0 / $minFactor]
  $polyTransform Identity
  $polyTransform Concatenate $ijkToRAS
  foreach sp [$volumeNode GetSpacing] {
    lappend invSpacing [expr 1. / $sp]
  }
  eval $polyTransform Scale $invSpacing

  #
  # set up progress observers
  #
  set observerRecords ""
  set filters "$changeIn $resample $dt $changeOut $cubes"
  foreach filter $filters {
    set tag [$filter AddObserver ProgressEvent "FastMarchingSegmentationTclProgressEventCallback $filter"]
    lappend observerRecords "$filter $tag"
  }

  #
  # activate the pipeline
  #
  $polyTransformFilter Update

  # remove progress observers
  foreach record $observerRecords {
    foreach {filter tag} $record {
      $filter RemoveObserver $tag
    }
  }
  FastMarchingSegmentationTclProgressEventCallback ""

  #
  # create a mrml model display node if needed
  #
  if { [$outModelNode GetDisplayNode] == "" } {
    set modelDisplayNode [vtkMRMLModelDisplayNode New]
    $outModelNode SetScene $::slicer3::MRMLScene
    eval $modelDisplayNode SetColor .5 1 1
    $::slicer3::MRMLScene AddNode $modelDisplayNode
    $outModelNode SetAndObserveDisplayNodeID [$modelDisplayNode GetID]
  }

  #
  # set the output into the MRML scene
  #
  $outModelNode SetAndObservePolyData [$polyTransformFilter GetOutput]

  $outVolumeNode SetAndObserveImageData [$changeOut GetOutput]
  $outVolumeNode SetIJKToRASMatrix $ijkToRAS
  $ijkToRAS Delete


  set ::FastMarchingSegmentationTcl($this,processing) 0
}

#proc FastMarchingSegmentationTclInitializeFilter {this inputVolumeID} {
#  puts "Input volume id is $inputVolumeID"
#
#  if { $::FastMarchingSegmentationTcl($this, fastMarchingFilter) } {
#    $::FastMarchingSegmentationTcl($this,fastMarchingFilter) Delete
#  }
#
#  set $::FastMarchingSegmentationTcl($this,fastMarchingFilter) [new vtkFastMarching]
#  
#}
