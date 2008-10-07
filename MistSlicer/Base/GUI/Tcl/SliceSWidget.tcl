
#########################################################
#
if {0} { ;# comment

  SliceSWidget  - manages slice plane interactions

# TODO : 

}
#
#########################################################
# ------------------------------------------------------------------
#                             SliceSWidget
# ------------------------------------------------------------------
#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class SliceSWidget] == "" } {

  itcl::class SliceSWidget {

    inherit SWidget

    constructor {args} {}
    destructor {}

    public variable sliceStep 1  ;# the size of the slice increment/decrement

    variable _actionStartRAS "0 0 0"
    variable _actionStartXY "0 0"
    variable _actionStartViewportOrigin "0 0"
    variable _actionStartWindowXY "0 0"
    variable _actionStartFOV "250 250 250"
    variable _fiducialsSWidget ""
    variable _gridSWidget ""
    variable _crosshairSWidget ""
    variable _regionsSWidget ""
    variable _slicePlaneSWidget ""

    # methods
    method resizeSliceNode {} {}
    method processEvent {{caller ""} {event ""}} {}
    method updateAnnotation {x y r a s} {}
    method updateAnnotations {x y r a s} {}
    method incrementSlice {} {}
    method decrementSlice {} {}
    method moveSlice { delta } {}
    method jumpSlice { r a s } {}
    method jumpOtherSlices { r a s } {}
    method getLinkedSliceLogics {} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body SliceSWidget::constructor {sliceGUI} {

  $this configure -sliceGUI $sliceGUI

  set _fiducialsSWidget [FiducialsSWidget #auto $sliceGUI]
  set _gridSWidget [GridSWidget #auto $sliceGUI]
  $_gridSWidget configure -layer "label"
  set _crosshairSWidget [CrosshairSWidget #auto $sliceGUI]
  set _regionsSWidget [RegionsSWidget #auto $sliceGUI]
  set _slicePlaneSWidget [SlicePlaneSWidget #auto $sliceGUI]
 
  # create matrices to store transform state
  set o(storeXYToRAS) [$this vtkNew vtkMatrix4x4]
  set o(storeSliceToRAS) [$this vtkNew vtkMatrix4x4]
  set o(scratchMatrix) [$this vtkNew vtkMatrix4x4]

  # set the annotation property
  [$_annotation GetTextProperty] SetColor 1 1 1
  [$_annotation GetTextProperty] SetShadow 1

  # create an actor as feedback for the focus state
  set o(focusIcon) [vtkNew vtkSphereSource]
  $o(focusIcon) SetRadius 2
  set o(focusMapper) [vtkNew vtkPolyDataMapper2D]
  set o(focusActor) [vtkNew vtkActor2D]
  $o(focusMapper) SetInput [$o(focusIcon) GetOutput]
  $o(focusActor) SetMapper $o(focusMapper)
  [$o(focusActor) GetProperty] SetColor 1 1 0
  $o(focusActor) VisibilityOff
  $o(focusActor) SetPosition 3 3
  # We'll need to change this or come up with a different way to indicate which active frame
  [$_renderWidget GetRenderer] AddActor2D $o(focusActor)
  lappend _actors $o(focusActor)

  # TODO:
  # create text actors for L/R, I/S, P/A


  $this processEvent

  #
  # set up observers on sliceGUI and on sliceNode
  #

  $::slicer3::Broker AddObservation $sliceGUI DeleteEvent "::SWidget::ProtectedDelete $this"

  set events {  "MouseMoveEvent" "RightButtonPressEvent" "RightButtonReleaseEvent"
    "LeftButtonPressEvent" "LeftButtonReleaseEvent" "MiddleButtonPressEvent"
    "MiddleButtonReleaseEvent" "MouseWheelForwardEvent" "MouseWheelBackwardEvent"
    "ExposeEvent" "ConfigureEvent" "EnterEvent" "LeaveEvent"
    "TimerEvent" "KeyPressEvent" "KeyReleaseEvent"
    "CharEvent" "ExitEvent" "UserEvent" }
  foreach event $events {
    $::slicer3::Broker AddObservation $sliceGUI $event "::SWidget::ProtectedCallback $this processEvent $sliceGUI $event"
  }

  set node [[$sliceGUI GetLogic] GetSliceNode]
  $::slicer3::Broker AddObservation $node DeleteEvent "::SWidget::ProtectedDelete $this"
#  $::slicer3::Broker AddObservation $node AnyEvent "::SWidget::ProtectedCallback $this processEvent $node AnyEvent"

}


itcl::body SliceSWidget::destructor {} {

  ::SWidget::ProtectedDelete $_fiducialsSWidget

  ::SWidget::ProtectedDelete $_gridSWidget

  ::SWidget::ProtectedDelete $_crosshairSWidget

  ::SWidget::ProtectedDelete $_regionsSWidget

  ::SWidget::ProtectedDelete $_slicePlaneSWidget

  $_renderWidget RemoveAllRenderers
}



# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

#
# make sure the size of the slice matches the window size of the widget
#
itcl::body SliceSWidget::resizeSliceNode {} {

  set epsilon 1.0e-6

  if { $_layers(background,node) != "" } {
    set logic [$sliceGUI GetLogic]
    set sliceSpacing [$logic GetLowestVolumeSliceSpacing]

    $this configure -sliceStep [lindex $sliceSpacing 2]
  }

  foreach {windoww windowh} [[$_interactor GetRenderWindow] GetSize] {}
  foreach {windowx windowy} [$_interactor GetEventPosition] {}
  # We should really use the pokedrenderer's size for these calculations.
  # However, viewerports in the LightBox can differ in size by a pixel.  So 
  # set the image size based on the size of renderer zero.
  #
  ###set pokedRenderer [$_interactor FindPokedRenderer $windowx $windowy]
  set pokedRenderer [$_renderWidget GetRenderer]  
  foreach {w h} [$pokedRenderer GetSize] {}

  foreach {nodeW nodeH nodeD} [$_sliceNode GetDimensions] {}
  foreach {nodefovx nodefovy nodefovz} [$_sliceNode GetFieldOfView] {}
  if { $w == $nodeW && $h == $nodeH && [expr abs($sliceStep - ($nodefovz / (1. * $nodeD)))] < $epsilon} {
    return
  }

  if { $windoww == "10" && $windowh == "10" } {
    puts "ignoring bogus resize"
  } else {
    set scaling0 [expr $w / (1. * $nodeW)]
    set scaling1 [expr $h / (1. * $nodeH)]

    set sMagnitude0 $scaling0
    if { $sMagnitude0 < 1.0 } {
       set sMagnitude0 [expr 1. / $sMagnitude0]
    }

    set sMagnitude1 $scaling1
    if { $sMagnitude1 < 1.0 } {
       set sMagnitude1 [expr 1. / $sMagnitude1]
    }

    if {$sMagnitude0 < $sMagnitude1} {
       # keep x fov the same, adjust y
       set fovx $nodefovx
       set fovy [expr $nodefovy * $scaling1 / $scaling0]
       set fovz [expr $sliceStep * $nodeD]
    } else {
       # keep y fov the same, adjust x
       set fovx [expr $nodefovx * $scaling0 / $scaling1]
       set fovy $nodefovy
       set fovz [expr $sliceStep * $nodeD]
    }

    $_sliceNode DisableModifiedEventOn
    $_sliceNode SetDimensions $w $h $nodeD
    $_sliceNode SetFieldOfView $fovx $fovy $fovz
    $_sliceNode DisableModifiedEventOff
    $_sliceNode InvokePendingModifiedEvent
  }
}

#
# handle interactor events
#
itcl::body SliceSWidget::processEvent { {caller ""} {event ""} } {
  # puts "$this $_sliceNode [$_sliceNode GetLayoutName] $caller $event"

  if { [info command $sliceGUI] == "" } {
    # the sliceGUI was deleted behind our back, so we need to 
    # self destruct
    ::SWidget::ProtectedDelete $this
    return
  }

  if { [info command $_sliceNode] == "" } {
    # somehow our slice node is corrupted - we need to bail out
    return
  }

  #
  # get the current event info
  # - the event type
  # - check to see if another widget is grabbing
  #   (we'll process some events even if we don't have grab)
  # - the actual x y location of the event
  # - fill the layers info with local info
  # - get the RAS space location of the event
  #
  if { $event != "ConfigureEvent" } {
    set tkwindow [$_renderWidget  GetWidgetName]
    $_interactor UpdateSize [winfo width $tkwindow] [winfo height $tkwindow]
  }


  #
  # if another widget has the grab, let this go unless
  # it is a focus event, in which case we want to update
  # out display icon
  #
  set grabID [$sliceGUI GetGrabID]
  if { ($grabID != "" && $grabID != $this) } {
    if { ![string match "Focus*Event" $event] } {
      return ;# some other widget wants these events
    }
  }

  # To support the LightBox, the event locations sometimes need to be
  # relative to a renderer (or viewport or pane of the lightbox).
  # Currently, these are relative to the viewport of the "start" action. 
  # We may need to change this in some cases to be relative to an "active" 
  # viewport.


  #
  # get the event position and make it relative to a renderer/viewport
  #
  foreach {windowx windowy} [$_interactor GetEventPosition] {}
  foreach {lastwindowx lastwindowy} [$_interactor GetLastEventPosition] {}
  foreach {windoww windowh} [[$_interactor GetRenderWindow] GetSize] {}

  set pokedRenderer [$_interactor FindPokedRenderer $windowx $windowy]
  set renderer0 [$_renderWidget GetRenderer]

  foreach {x y z} [$this dcToXYZ $windowx $windowy] {}  

  if { $x < 0 } { 
    set x 0
  }
  if { $y < 0 } { 
    set y 0
  }

  # We should really use the pokedrenderer's size for these calculations.
  # However, viewports in the LightBox can differ in size by a pixel.  So 
  # set the image size based on the size of renderer zero.
  #
  ###foreach {w h} [$pokedRenderer GetSize] {}
  foreach {w h} [$renderer0 GetSize] {}
  foreach {rox roy} [$pokedRenderer GetOrigin] {}

  $this queryLayers $x $y $z
  set xyToRAS [$_sliceNode GetXYToRAS]
  set ras [$xyToRAS MultiplyPoint $x $y $z 1]

  foreach {r a s t} $ras {}

  #
  # check that the window size is correct for the node and resize if needed
  #
  $this resizeSliceNode


  switch $event {

    "MouseMoveEvent" {
      #
      # Mouse move behavior governed by _actionState mode
      # - first update the annotation
      # - then handle modifying the view
      #
      set link [$_sliceCompositeNode GetLinkedControl]
      if { $link == 1 } {
        $this updateAnnotations $x $y $r $a $s
      } else {
        $this updateAnnotation $x $y $r $a $s
      }

      if { [$_interactor GetShiftKey] } {
        $this jumpOtherSlices $r $a $s
        # need to render to show the annotation
        [$sliceGUI GetSliceViewer] RequestRender
      } else {
        switch $_actionState {
          "Translate" {
            #
            # Translate
            # TODO: move calculation to vtkSlicerSliceLogic
            $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
            set tx [expr $windowx - [lindex $_actionStartViewportOrigin 0]]
            set ty [expr $windowy - [lindex $_actionStartViewportOrigin 1]]
            
            set currentRAS [$o(storeXYToRAS) MultiplyPoint $tx $ty 0 1]
            foreach d {dr da ds} start $_actionStartRAS current $currentRAS {
              set $d [expr $current - $start]
            }
            $o(scratchMatrix) DeepCopy $o(storeSliceToRAS)
            foreach d {dr da ds} i {0 1 2} {
              set v [$o(scratchMatrix) GetElement $i 3]
              $o(scratchMatrix) SetElement $i 3 [expr $v - [set $d]]
            }

            # get the linked logics (including self)
            set sliceLogics [$this getLinkedSliceLogics]
            # save state for undo
            
            # set the SliceToRAS on each slice node
            foreach logic $sliceLogics {
              set snode [$logic GetSliceNode]
              [$snode GetSliceToRAS] DeepCopy $o(scratchMatrix)
              $snode UpdateMatrices
            }

            $sliceGUI SetGUICommandAbortFlag 1
            $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
          }
          "Zoom" {
            #
            # Zoom
            # TODO: move calculation to vtkSslicerSliceLogic
            $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
            set deltay [expr $windowy - [lindex $_actionStartWindowXY 1]]

            set percent [expr ($windowh + $deltay) / (1.0 * $windowh)]

            # the factor operation is so 'z' isn't changed and the 
            # slider can still move through the full range
            if { $percent > 0. } {
              set newFOV ""
              foreach f $_actionStartFOV factor "$percent $percent 1" {
                lappend newFOV [expr $f * $factor]
              }

              # get the linked logics (including self)
              set sliceLogics [$this getLinkedSliceLogics]
              # save state for undo
                
              # set the field of view on each slice node
              foreach logic $sliceLogics {
                  set snode [$logic GetSliceNode]
                  eval $snode SetFieldOfView $newFOV
              }
            }
            $sliceGUI SetGUICommandAbortFlag 1
          }
          "Rotate" {
            #
            # Rotate
            # TODO: move calculation to vtkSlicerSliceLogic
            $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
            set dx [expr $windowx - $lastwindowx]
            set dy [expr $windowy - $lastwindowy]

            set dazimuth   [expr 20.0 / $w]
            set delevation [expr 20.0 / $h]

            set rx [expr $dx * $dazimuth * 10.0]
            set ry [expr $dy * $delevation * 10.0]

            # puts "rx = $rx"

            set tfm [$this vtkNew vtkTransform]
            $tfm PreMultiply
            $tfm Identity
            $tfm SetMatrix [$_sliceNode GetSliceToRAS] 
            $tfm RotateX $ry
            $tfm RotateY $rx

            # get the linked logics (including self)
            set sliceLogics [$this getLinkedSliceLogics]
            # save state for undo
                
            # set the SliceToRAS on each slice node
            foreach logic $sliceLogics {
              set snode [$logic GetSliceNode]
              [$snode GetSliceToRAS] DeepCopy [$tfm GetMatrix]
              $snode UpdateMatrices
            }
            $tfm Delete

            $sliceGUI SetGUICommandAbortFlag 1
           }
          default {
            # need to render to show the annotation
            [$sliceGUI GetSliceViewer] RequestRender
          }
        }
      }
    }

    "RightButtonPressEvent" {
        if { [$_sliceNode GetOrientationString] == "Reformat" && [$_interactor GetControlKey] } {
        set _actionState "Rotate"
      } else {
        set _actionState "Zoom"
      }
      set _actionStartXY "$x $y"
      set _actionStartWindowXY "$windowx $windowy"
      set _actionStartViewportOrigin "$rox $roy"
      set _actionStartRAS $ras
      $sliceGUI SetGrabID $this
      $sliceGUI SetGUICommandAbortFlag 1
      set _actionStartFOV [$_sliceNode GetFieldOfView]

      $::slicer3::MRMLScene SaveStateForUndo $_sliceNode
    }
    "RightButtonReleaseEvent" { 
      set _actionState ""
      $sliceGUI SetGrabID ""
      $sliceGUI SetGUICommandAbortFlag 1
    }
    "LeftButtonPressEvent" {
      if { [info command SeedSWidget] != "" } {
        set interactionNode [$::slicer3::MRMLScene GetNthNodeByClass 0 vtkMRMLInteractionNode]
        if { $interactionNode != "" } {
          set mode [$interactionNode GetCurrentInteractionMode]
          set modeString [$interactionNode GetInteractionModeAsString $mode]
          set modifier [expr [$_interactor GetControlKey] && [$_interactor GetShiftKey]]
          if { $modeString == "Place" || $modifier } {
            FiducialsSWidget::AddFiducial $r $a $s
          }
        }
      }
    }
    "LeftButtonReleaseEvent" { 
    }
    "MiddleButtonPressEvent" {
      set _actionState "Translate"
      set _actionStartXY "$x $y"
      set _actionStartWindowXY "$windowx $windowy"
      set _actionStartViewportOrigin "$rox $roy"
      set _actionStartRAS $ras
      $sliceGUI SetGrabID $this
      $sliceGUI SetGUICommandAbortFlag 1
      $o(storeXYToRAS) DeepCopy [$_sliceNode GetXYToRAS]
      $o(storeSliceToRAS) DeepCopy [$_sliceNode GetSliceToRAS]
      $::slicer3::MRMLScene SaveStateForUndo $_sliceNode
    }
    "MiddleButtonReleaseEvent" { 
      set _actionState ""
      $sliceGUI SetGrabID ""
      $sliceGUI SetGUICommandAbortFlag 1
    }
    "MouseWheelForwardEvent" { 
      $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
      $this incrementSlice 
    }
    "MouseWheelBackwardEvent" {
      $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
      $this decrementSlice 
    }
    "ExposeEvent" { 
      [$sliceGUI GetSliceViewer] RequestRender
    }
    "ConfigureEvent" {
      $this resizeSliceNode
    }
    "EnterEvent" { 
      $_renderWidget CornerAnnotationVisibilityOn
      [$::slicer3::ApplicationGUI GetMainSlicerWindow]  SetStatusText "Middle Button: Pan; Right Button: Zoom"
    }
    "LeaveEvent" { 
      $_renderWidget CornerAnnotationVisibilityOff
      [$::slicer3::ApplicationGUI GetMainSlicerWindow]  SetStatusText ""
    }
    "TimerEvent" { }
    "KeyPressEvent" { 
      set key [$_interactor GetKeySym]
      if { [lsearch "v r b f space c e Up Down Left Right" $key] != -1 } {
        $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
        $sliceGUI SetGUICommandAbortFlag 1
        switch [$_interactor GetKeySym] {
          "v" {
            $_sliceNode SetSliceVisible [expr ![$_sliceNode GetSliceVisible]]
          }
          "r" {
            # use c++ version of calculation

            # get the linked logics (including self)
            set sliceLogics [$this getLinkedSliceLogics]
            # save state for undo
            
            # fit the slice to the background for each logic/slice node
            foreach logic $sliceLogics {
              set snode [$logic GetSliceNode]
              $logic FitSliceToBackground $w $h
              $snode UpdateMatrices
            }
          }
          "b" - "Left" - "Down" {
            $this decrementSlice
          }
          "f" - "Right" - "Up" {
            $this incrementSlice
          }
          "space" {
            ::Box::ShowDialog EditBox
          }
          "c" {
            ::Box::ShowDialog ColorBox
          }
          "e" {
            EditorToggleErasePaintLabel
          }
          default {
            set capture 0
          }
        }
      } else {
        # puts "slice ignoring $key"
      }
    }
    "KeyReleaseEvent" { 
    }
    "FocusInEvent" {
      $o(focusActor) VisibilityOn
      [$sliceGUI GetSliceViewer] RequestRender
    }
    "FocusOutEvent" {
      $o(focusActor) VisibilityOff
      [$sliceGUI GetSliceViewer] RequestRender
    }
    "ExitEvent" { }
  }

  set xyToRAS [$_sliceNode GetXYToRAS]
  set ras [$xyToRAS MultiplyPoint $x $y $z 1]
  foreach {r a s t} $ras {}
  $this updateAnnotation $x $y $r $a $s

}

itcl::body SliceSWidget::updateAnnotations {x y r a s} {
  set ssgui [[$::slicer3::ApplicationGUI GetApplication] GetModuleGUIByName "Slices"]
  set numsgui [$ssgui GetNumberOfSliceGUI]
  for { set i 0 } { $i < $numsgui } { incr i } {
    if { $i == 0} {
      set sgui [$ssgui GetFirstSliceGUI]
      set lname [$ssgui GetFirstSliceGUILayoutName]
    } else {
      set sgui [$ssgui GetNextSliceGUI $lname]
      set lname [$ssgui GetFirstSliceGUILayoutName]
    }
    set logic [$sgui GetLogic]
    set sliceCompositeNode [$logic GetSliceCompositeNode]

  set colorName ""
  if {[info command $_layers(label,node)] != "" && \
      $_layers(label,node) != "" && \
      $_layers(label,pixel) != "" && \
      $_layers(label,pixel) != "Unknown" && \
      $_layers(label,pixel) != "Out of Frame"} {
      set labelDisplayNode [$_layers(label,node) GetDisplayNode]
      if {$labelDisplayNode != "" && [$labelDisplayNode GetColorNodeID] != ""} {
          set colorNode [$labelDisplayNode GetColorNode]
          if {$colorNode != ""} {
              if {[string is integer $_layers(label,pixel)]} {
                  set colorName [$colorNode GetColorName $_layers(label,pixel)]
              }
          }
      }
  }
  set labelText "Lb: $_layers(label,pixel) $colorName"
  set voxelText "Fg: $_layers(foreground,pixel)\nBg: $_layers(background,pixel)"
  set ijkText [format "Bg I: %d\nBg J: %d\nBg K: %d" \
                $_layers(background,i) $_layers(background,j) $_layers(background,k)]
  set xyText "X: $x\nY:$y"
  set rasText [format "R: %.1f\nA: %.1f\nS: %.1f" $r $a $s]

  set spaceText0 ""
  set spaceText1 ""
  switch [$sliceCompositeNode GetAnnotationSpace] {
    "0" {set spaceText0 $xyText}
    "1" {set spaceText0 $ijkText}
    "2" {set spaceText0 $rasText}
    "3" {set spaceText0 $rasText; set spaceText1 $ijkText}
  }

  switch [$sliceCompositeNode GetAnnotationMode] {
    "0" {
      $_annotation SetText 0 ""
      $_annotation SetText 1 ""
      $_annotation SetText 2 ""
      $_annotation SetText 3 ""
    }
    "1" {
      $_annotation SetText 0 "${labelText}\n${voxelText}"
      $_annotation SetText 1 $spaceText0
      $_annotation SetText 2 $spaceText1
      $_annotation SetText 3 ""
    }
    "2" {
      $_annotation SetText 0 "${labelText}"
      $_annotation SetText 1 ""
      $_annotation SetText 2 ""
      $_annotation SetText 3 ""
    }
    "3" {
      $_annotation SetText 0 "${labelText}\n${voxelText}"
      $_annotation SetText 1 ""
      $_annotation SetText 2 ""
      $_annotation SetText 3 ""
    }
  }
    
  }
}

itcl::body SliceSWidget::updateAnnotation {x y r a s} {

  set logic [$sliceGUI GetLogic]
  set sliceCompositeNode [$logic GetSliceCompositeNode]

  
  # get the display node for the label volume, extract the name of the colour used to represent the label pixel
  set colorName ""
  if {[info command $_layers(label,node)] != "" && \
      $_layers(label,node) != "" && \
      $_layers(label,pixel) != "" && \
      $_layers(label,pixel) != "Unknown" && \
      $_layers(label,pixel) != "Out of Frame"} {
      set labelDisplayNode [$_layers(label,node) GetDisplayNode]
      if {$labelDisplayNode != "" && [$labelDisplayNode GetColorNodeID] != ""} {
          set colorNode [$labelDisplayNode GetColorNode]
          if {$colorNode != ""} {
              if {[string is integer $_layers(label,pixel)]} {
                  set colorName [$colorNode GetColorName $_layers(label,pixel)]
              }
          }
      }
  }
  set labelText "Lb: $_layers(label,pixel) $colorName"
  set voxelText "Fg: $_layers(foreground,pixel)\nBg: $_layers(background,pixel)"
  set ijkText [format "Bg I: %d\nBg J: %d\nBg K: %d" \
                $_layers(background,i) $_layers(background,j) $_layers(background,k)]
  set xyText "X: $x\nY:$y"
  set rasText [format "R: %.1f\nA: %.1f\nS: %.1f" $r $a $s]

  set spaceText0 ""
  set spaceText1 ""
  switch [$sliceCompositeNode GetAnnotationSpace] {
    "0" {set spaceText0 $xyText}
    "1" {set spaceText0 $ijkText}
    "2" {set spaceText0 $rasText}
    "3" {set spaceText0 $rasText; set spaceText1 $ijkText}
  }

  switch [$sliceCompositeNode GetAnnotationMode] {
    "0" {
      $_annotation SetText 0 ""
      $_annotation SetText 1 ""
      $_annotation SetText 2 ""
      $_annotation SetText 3 ""
    }
    "1" {
      $_annotation SetText 0 "${labelText}\n${voxelText}"
      $_annotation SetText 1 $spaceText0
      $_annotation SetText 2 $spaceText1
      $_annotation SetText 3 ""
    }
    "2" {
      $_annotation SetText 0 "${labelText}"
      $_annotation SetText 1 ""
      $_annotation SetText 2 ""
      $_annotation SetText 3 ""
    }
    "3" {
      $_annotation SetText 0 "${labelText}\n${voxelText}"
      $_annotation SetText 1 ""
      $_annotation SetText 2 ""
      $_annotation SetText 3 ""
    }
  }
}


itcl::body SliceSWidget::incrementSlice {} {
  $this moveSlice $sliceStep
}

itcl::body SliceSWidget::decrementSlice {} {
  $this moveSlice [expr -1.0 * $sliceStep]
}

itcl::body SliceSWidget::moveSlice { delta } {
    set logic [$sliceGUI GetLogic]
    set sliceNode [$logic GetSliceNode]
    set orientString [$sliceNode GetOrientationString]

    set offset [$logic GetSliceOffset]

    set logics ""
    set link [$_sliceCompositeNode GetLinkedControl]
    if { $link == 1 } {
        set ssgui [[$::slicer3::ApplicationGUI GetApplication] GetModuleGUIByName "Slices"]
        set numsgui [$ssgui GetNumberOfSliceGUI]

        for { set i 0 } { $i < $numsgui } { incr i } {
            if { $i == 0} {
                set sgui [$ssgui GetFirstSliceGUI]
                set lname [$ssgui GetFirstSliceGUILayoutName]
            } else {
                set sgui [$ssgui GetNextSliceGUI $lname]
                set lname [$ssgui GetNextSliceGUILayoutName $lname]
            }
            
            if { [string compare $lname "Red"] == 0 || 
             [string compare $lname "Yellow"] == 0 ||
             [string compare $lname "Green"] == 0 } {
                continue
            }

            set currSliceNode [$sgui GetSliceNode]
            set currOrientString [$currSliceNode GetOrientationString]
            if { [string compare $orientString $currOrientString] == 0 } {
                lappend logics [$sgui GetLogic]
            }
        }
    } else {
        lappend logics [$sliceGUI GetLogic]
    }

    # set the slice offset for all slice logics
    foreach logic $logics {
        $logic SetSliceOffset [expr $offset + $delta]
    }    
}



itcl::body SliceSWidget::jumpSlice { r a s } {
  set logic [$sliceGUI GetLogic]
  set sliceNode [$logic GetSliceNode]
  $sliceNode JumpSlice $r $a $s
}

itcl::body SliceSWidget::jumpOtherSlices { r a s } {
  set logic [$sliceGUI GetLogic]
  set sliceNode [$logic GetSliceNode]
  $sliceNode JumpAllSlices $r $a $s
}

# Return the SliceLogics that are linked to the current 
# SliceNode/SliceCompositeNode.  
#
# The list of linked logics either contains the current slice (because 
# linking is off) or a list of logics (one for the current slice and others 
# for linked slices).
#
itcl::body SliceSWidget::getLinkedSliceLogics { } {
    set logic [$sliceGUI GetLogic]
    set sliceNode [$logic GetSliceNode]
    set orientString [$sliceNode GetOrientationString]

    set logics ""
    set link [$_sliceCompositeNode GetLinkedControl]
    if { $link == 1 } {
        set ssgui [[$::slicer3::ApplicationGUI GetApplication] GetModuleGUIByName "Slices"]
        set layout [$::slicer3::ApplicationGUI GetGUILayoutNode]
        set viewArrangement [$layout GetViewArrangement]

        set numsgui [$ssgui GetNumberOfSliceGUI]

        for { set i 0 } { $i < $numsgui } { incr i } {
            if { $i == 0} {
                set sgui [$ssgui GetFirstSliceGUI]
                set lname [$ssgui GetFirstSliceGUILayoutName]
            } else {
                set sgui [$ssgui GetNextSliceGUI $lname]
                set lname [$ssgui GetNextSliceGUILayoutName $lname]
            }

            if { $viewArrangement==12 } {
                if { [string compare $lname "Red"] == 0 || 
                     [string compare $lname "Yellow"] == 0 ||
                     [string compare $lname "Green"] == 0 } {
                    continue
                }
            }

            set currSliceNode [$sgui GetSliceNode]
            set currOrientString [$currSliceNode GetOrientationString]
            if { [string compare $orientString $currOrientString] == 0 } {
                lappend logics [$sgui GetLogic]
            }
        }
    } else {
        lappend logics [$sliceGUI GetLogic]
    }

  return $logics
}
