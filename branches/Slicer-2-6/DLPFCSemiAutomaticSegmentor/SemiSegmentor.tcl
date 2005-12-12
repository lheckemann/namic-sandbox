#-------------------------------------------------------------------------------
# .PROC SemiSegmentorInit
#  The "Init" procedure is called automatically by the slicer.
#  It puts information about the module into a global array called Module,
#  and it also initializes module-level variables.
# .ARGS
# .END
y#-------------------------------------------------------------------------------
proc SemiSegmentorInit {} {
    global SemiSegmentor Module Volume Model

    set m SemiSegmentor

    # Module Summary Info
    #------------------------------------
    # Description:
    #  Give a brief overview of what your module does, for inclusion in the 
    #  Help->Module Summaries menu item.
    set Module($m,overview) "This module is for Semi_Automatic Segmentation."
    #  Provide your name, affiliation and contact information so you can be 
    #  reached for any questions people may have regarding your module. 
    #  This is included in the  Help->Module Credits menu item.
    set Module($m,author) "Ramsey Al-Hakim, GaTech gtg226r@mail.gatech.edu"

    #  Set the level of development that this module falls under, from the list defined in Main.tcl,
    #  Module(categories) or pick your own
    #  This is included in the Help->Module Categories menu item
    set Module($m,category) "Segmentation"

    # Define Tabs
    #------------------------------------
    # Description:
    #   Each module is given a button on the Slicer's main menu.
    #   When that button is pressed a row of tabs appear, and there is a panel
    #   on the user interface for each tab.  If all the tabs do not fit on one
    #   row, then the last tab is automatically created to say "More", and 
    #   clicking it reveals a second row of tabs.
    #
    #   Define your tabs here as shown below.  The options are:
    #   row1List = list of ID's for tabs. (ID's must be unique single words)
    #   row1Name = list of Names for tabs. (Names appear on the user interface
    #              and can be non-unique with multiple words.)
    #   row1,tab = ID of initial tab
    #   row2List = an optional second row of tabs if the first row is too small
    #   row2Name = like row1
    #   row2,tab = like row1 
    #

    set Module($m,row1List) "Help DLPFC Merge"
    set Module($m,row1Name) "{Help} {DLPFC} {Future Segmentors}"
    set Module($m,row1,tab) DLPFC

    # Define Procedures
    #------------------------------------
    # Description:
    #   The Slicer sources *.tcl files, and then it calls the Init
    #   functions of each module, followed by the VTK functions, and finally
    #   the GUI functions. A MRML function is called whenever the MRML tree
    #   changes due to the creation/deletion of nodes.
    #   
    #   While the Init procedure is required for each module, the other 
    #   procedures are optional.  If they exist, then their name (which
    #   can be anything) is registered with a line like this:
    #
    #   set Module($m,procVTK) SemiSegmentorBuildVTK
    #
    #   All the options are:

    #   procGUI   = Build the graphical user interface
    #   procVTK   = Construct VTK objects
    #   procMRML  = Update after the MRML tree changes due to the creation
    #               of deletion of nodes.
    #   procEnter = Called when the user enters this module by clicking
    #               its button on the main menu
    #   procExit  = Called when the user leaves this module by clicking
    #               another modules button
    #   procCameraMotion = Called right before the camera of the active 
    #                      renderer is about to move 
    #   procStorePresets  = Called when the user holds down one of the Presets
    #               buttons.
    #               
    #   Note: if you use presets, make sure to give a preset defaults
    #   string in your init function, of the form: 
    #   set Module($m,presets) "key1='val1' key2='val2' ..."
    #   
    set Module($m,procGUI) SemiSegmentorBuildGUI
    set Module($m,procVTK) SemiSegmentorBuildVTK
    set Module($m,procEnter) SemiSegmentorEnter
    set Module($m,procExit) SemiSegmentorExit
    set Module($m,procMRML) SemiSegmentorUpdateGUI

    # Define Dependencies
    #------------------------------------
    # Description:
    #   Record any other modules that this one depends on.  This is used 
    #   to check that all necessary modules are loaded when Slicer runs.
    #   
    set Module($m,depend) ""

    # Set version info
    #------------------------------------
    # Description:
    #   Record the version number for display under Help->Version Info.
    #   The strings with the $ symbol tell CVS to automatically insert the
    #   appropriate revision number and date when the module is checked in.
    #   
    lappend Module(versions) [ParseCVSInfo $m \
        {$Revision: 1.6.2.3 $} {$Date: 2004/11/30 20:49:54 $}]

    # Initialize module-level variables
    #------------------------------------
    # Description:
    #   Keep a global array with the same name as the module.
    #   This is a handy method for organizing the global variables that
    #   the procedures in this module and others need to access.
    #
    set SemiSegmentor(VolumeIn) $Volume(idNone)
    set SemiSegmentor(LabelVolumeIn) $Volume(idNone)
    set SemiSegmentor(OutputName) "roi"
    set SemiSegmentor(OutputName2) "labelmap_roi"
 
    set SemiSegmentor(eventManager) ""
    
    
    set SemiSegmentor(InOrigin) "0 0 0"
    set SemiSegmentor(InExtent) "0 0 0 0 0 0"
    set SemiSegmentor(InSpacing) "0 0 0"

    #2D case variables
    set SemiSegmentor(Ext2D,TypeList) "Ax Sag Cor"
    set SemiSegmentor(Ext2D,Type) "All"
    set SemiSegmentor(Ext2D,Ax,init) 0
    set SemiSegmentor(Ext2D,Ax,end) 1
    set SemiSegmentor(Ext2D,Sag,init) 1
    set SemiSegmentor(Ext2D,Sag,end) 2
    set SemiSegmentor(Ext2D,Cor,init) 2
    set SemiSegmentor(Ext2D,Cor,end) 3

    #3D case variable
    foreach type "AxMin AxMax SagMin SagMax CorMin CorMax" {
      set SemiSegmentor(Ext3D,$type) 0
      set SemiSegmentor(Ext3D,$type,min) 0
      set SemiSegmentor(Ext3D,$type,max) 0
    }
    
    set SemiSegmentor(Ext3D,volId) $Volume(idNone)
    set SemiSegmentor(Ext3D,volId2) $Volume(idNone)
    set SemiSegmentor(Ext3D,AxMax,title) "Superior Tip:"
    set SemiSegmentor(Ext3D,AxMin,title) "Inferior Tip:"
    set SemiSegmentor(Ext3D,SagMax,title) "Sag Max (R):"
    set SemiSegmentor(Ext3D,SagMin,title) "Sag Min (L):"
    set SemiSegmentor(Ext3D,CorMax,title) "Frontal Pole:"
    set SemiSegmentor(Ext3D,CorMin,title) "Temporal Tip:"

    set SemiSegmentor(Ext3D,AxMin,Id) 0 
    set SemiSegmentor(Ext3D,AxMax,Id) 0
    set SemiSegmentor(Ext3D,SagMin,Id) 1
    set SemiSegmentor(Ext3D,SagMax,Id) 1
    set SemiSegmentor(Ext3D,CorMin,Id) 2
    set SemiSegmentor(Ext3D,CorMax,Id) 2
    
    
    set SemiSegmentor(Ext3D,CubeColor) "1 0 0"
    set SemiSegmentor(Ext3D,CubeOpacity) 0.5
    set SemiSegmentor(Ext3D,OutlineColor) "0 0 1"
    set SemiSegmentor(Ext3D,OutlineOpacity) 1
    set SemiSegmentor(Ext3D,RenderCube) 1
}

# NAMING CONVENTION:
#-------------------------------------------------------------------------------
#
# Use the following starting letters for names:
# t  = toplevel
# f  = frame
# mb = menubutton
# m  = menu
# b  = button
# l  = label
# s  = slider
# i  = image
# c  = checkbox
# r  = radiobutton
# e  = entry
#
#-------------------------------------------------------------------------------


proc SemiSegmentorBuildGUI {} {
    global Gui SemiSegmentor Module Volume Model
    
    # A frame has already been constructed automatically for each tab.
    # A frame named "Stuff" can be referenced as follows:
    #   
    #     $Module(<Module name>,f<Tab name>)
    #
    # ie: $Module(SemiSegmentor,fStuff)
    
    # This is a useful comment block that makes reading this easy for all:
    #-------------------------------------------
    # Frame Hierarchy:
    #-------------------------------------------
    # Help
    # Stuff
    #   Top
    #   Middle
    #   Bottom
    #     FileLabel
    #     CountDemo
    #     TextBox
    #-------------------------------------------
   #-------------------------------------------
    # Help frame
    #-------------------------------------------
    
    # Write the "help" in the form of psuedo-html.  
    # Refer to the documentation for details on the syntax.
    #
    set help "
    The SemiSegmentor module allows to extract a volume of interest for further processing
    Note:  The extraction of subvolume must be done in RAS space and the slice direction must match the following:
      Pink = Axial
      Yellow = Sagittal
      Green = Coronal
    *DO NOT USE AxiSlice,CorSlice, OR SagSlice*
    <P>
    Description by tab:
    <BR>
    <UL>
    <LI><B>DLPFC:</B> Extract the DLPFC
    <LI><B>Future Areas:</B> Extract other areas of the brain.
    "
    regsub -all "\n" $help {} help
    MainHelpApplyTags SemiSegmentor $help
    MainHelpBuildGUI SemiSegmentor

    #-------------------------------------------
    # DLPFC frame
    #-------------------------------------------
    set fDLPFC $Module(SemiSegmentor,fDLPFC)
    set f $fDLPFC
    
    foreach frame "IO 3D Apply Render DLPFCLabelmapInput DLPFCLabelmapOutput" {
    frame $f.f$frame -bg $Gui(activeWorkspace) -relief groove -bd 3
    pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
     #-------------------------------------------
    # DLPFC->I/O frame
    #-------------------------------------------   
    set f $fDLPFC.fIO
    
    foreach frame "Input Output" {
      frame $f.f$frame -bg $Gui(activeWorkspace)
      pack $f.f$frame -side top -padx 0 -pady $Gui(pad) -fill x
    }
    
    set f $fDLPFC.fIO.fInput
    
    DevAddSelectButton SemiSegmentor $f VolumeIn "Input:"   Grid "Input Volume"

    set f $fDLPFC.fIO.fOutput
    
    DevAddLabel $f.l "Output Name for NEW Volume:"      
    eval {entry $f.e -justify right -width 10 \
          -textvariable  SemiSegmentor(OutputName)  } $Gui(WEA)
    pack $f.l $f.e -side left -padx $Gui(pad) -pady 0


#WORKING HERE TO PUT INPUT OUTPUT VOLUMES:
   # eval {label $f.f2.lVolume -text "Volume:"} $Gui(WLA)
   # eval {menubutton $f.f2.mbVolume -text "Select" -menu $f.f2.mbVolume.m} $Gui(WMBA)
   # eval {menu $f.f2.mbVolume.m} $Gui(WMA)
   # eval {label $f.f2.lLabelMap -text "Label map:"} $Gui(WLA)
   # eval {menubutton $f.f2.mbLabel -text "Select" -menu $f.f2.mbLabel.m} $Gui(WMBA)
   # eval {menu $f.f2.mbLabel.m} $Gui(WMA)

  


    
    #-------------------------------------------
    # DLPFC->3D
    #-------------------------------------------
    set f $fDLPFC.f3D
    
    frame $f.fLabel  -bg $Gui(activeWorkspace) -relief groove -bd 3
    frame $f.fControl -bg $Gui(activeWorkspace) -bd 3
    pack  \
          $f.fLabel \
          $f.fControl \
      -side top -padx 0 -pady 1 -fill x
    
    set f $fDLPFC.f3D.fLabel
    
    eval {label $f.l3D -text "Boundaries of the DLPFC"} $Gui(WLA)
    pack $f.l3D -side left -padx $Gui(pad) -pady 0
    
    set fControl $fDLPFC.f3D.fControl
    
   #--------------------------------------------------
   foreach type "AxMin AxMax CorMin CorMax" {
     frame $fControl.f$type -bg $Gui(activeWorkspace)
   }
   pack $fControl.fAxMin $fControl.fAxMax $fControl.fCorMin $fControl.fCorMax \
        -side top -padx 0 -pady 1 -fill x
   
   foreach type "AxMin AxMax CorMin CorMax" {
     set f $fControl.f$type 
     eval {label $f.l$type -text $SemiSegmentor(Ext3D,$type,title) -width 10 } $Gui(WLA)
     eval {label $f.r$type -text $SemiSegmentor(Ext3D,$type,title) -width 10 } $Gui(WLA)
   }  
   
   set f $fControl.fAxMin
   eval {button $f.bAxMin -text "Pick" -width 4 -command {SemiSegmentorPick3D AxMin} } $Gui(WBA)
   TooltipAdd $f.bAxMin "Choose the current Axial slice"
   set f $fControl.fAxMax
   eval {button $f.bAxMax -text "Pick" -width 4 -command {SemiSegmentorPick3D AxMax} } $Gui(WBA)
   TooltipAdd $f.bAxMax "Choose the current Axial slice"
   #set f $fControl.fSagMin
   #eval {button $f.bSagMin -text "Pick" -width 4 -command {SemiSegmentorPick3D SagMin} } $Gui(WBA)
   #TooltipAdd $f.bSagMin "Choose the current Sagittal slice"
   #set f $fControl.fSagMax
   #eval {button $f.bSagMax -text "Pick" -width 4 -command {SemiSegmentorPick3D SagMax} } $Gui(WBA)
   #TooltipAdd $f.bSagMax "Choose the current Sagittal slice"
   set f $fControl.fCorMin
   eval {button $f.bCorMin -text "Pick" -width 4 -command {SemiSegmentorPick3D CorMin} } $Gui(WBA)
   TooltipAdd $f.bCorMin "Choose the current Coronal slice"
   set f $fControl.fCorMax
   eval {button $f.bCorMax -text "Pick" -width 4 -command {SemiSegmentorPick3D CorMax} } $Gui(WBA)
   TooltipAdd $f.bCorMax "Choose the current Coronal slice"
        
   foreach type "AxMin AxMax CorMin CorMax" {  
     set f $fControl.f$type
     eval {entry $f.e$type -justify right -width 4 \
          -textvariable  SemiSegmentor(Ext3D,$type)  } $Gui(WEA)

     eval {scale $f.s$type -from $SemiSegmentor(Ext3D,$type,min) -to $SemiSegmentor(Ext3D,$type,max)        \
          -variable  SemiSegmentor(Ext3D,$type) -command SemiSegmentorUpdate3DScales \
          -orient vertical     \
          -resolution 1      \
          } $Gui(WSA)

        
     pack $f.l$type $f.b$type $f.e$type $f.s$type -side left -padx 0 -pady 0 -fill x
   } 
   
   #-------------------------------------------
   # DLPFC->Apply
   #-------------------------------------------
   set f $fDLPFC.fApply
    
   DevAddButton $f.bApply Apply SemiSegmentorApply
    
    # Tooltip example: Add a tooltip for the button
    TooltipAdd $f.bApply "Apply the SemiSegmentor extraction"

    pack $f.bApply -side top -pady $Gui(pad) -padx $Gui(pad) \
        -fill x -expand true
    
   #-------------------------------------------
   # DLPFC->Render
   #-------------------------------------------
   set f $fDLPFC.fRender
   
   eval {checkbutton $f.c3D -text "Render 3D Bounding Box" -variable SemiSegmentor(Ext3D,RenderCube) -indicatoron 1 -command SemiSegmentorRenderCube} $Gui(WCA)
   pack $f.c3D -side top -pady 2 -padx 1
   
   eval {label $f.lOpacity -text "Opacity:"\
          -width 11 -justify right } $Gui(WTA)

    eval {entry $f.eOpacity -justify right -width 4 \
          -textvariable SemiSegmentor(Ext3D,CubeOpacity)  } $Gui(WEA)

    eval {scale $f.sOpacity -from 0 -to 1     \
          -variable  SemiSegmentor(Ext3D,CubeOpacity)\
          -orient vertical     \
          -resolution 0.1      \
      -command { SemiSegmentor3DOpacity } \
          } $Gui(WSA)
     
     pack  $f.lOpacity $f.eOpacity $f.sOpacity -side left -padx 3 -pady 2 -expand 0
      
   #-------------------------------------------
   # DLPFC->DLPFCLabelmapInput
   #-------------------------------------------
   set f $fDLPFC.fDLPFCLabelmapInput
  
   DevAddSelectButton SemiSegmentor $f LabelVolumeIn "Input Label:"   Grid "Input Volume"

#   DevAddSelectButton SemiSegmentor $f LabelVolumeIn "Input Label:"   Grid "Input Label Volume"

   #-------------------------------------------
   # DLPFC->DLPFCLabelmapOutput
   #-------------------------------------------
   set f $fDLPFC.fDLPFCLabelmapOutput

       DevAddLabel $f.r "Output Name for NEW Labelmap:"      
    eval {entry $f.e -justify right -width 10 \
          -textvariable  SemiSegmentor(OutputName2)  } $Gui(WEA)
    pack $f.r $f.e -side left -padx $Gui(pad) -pady 0

}

proc SemiSegmentor3DOpacity {opacity} {
 global SemiSegmentor
 
 if { [info command SemiSegmentor(Ext3D,CubeActor)] != "" } {
     eval [SemiSegmentor(Ext3D,CubeActor) GetProperty] SetOpacity $opacity
     Render3D
 }
}

#----------------------------------------------------------------------
#
#
#
#----------------------------------------------------------------------
proc SemiSegmentorApply {} {
#    --------------

#REPEAT ALL OF THE FOLLOWING FOR THE ORIGINAL VOLUME
#START HERE..


global SemiSegmentor Volume

set volID $SemiSegmentor(VolumeIn)

set x1 [expr round([lindex $SemiSegmentor(Ext3D,Ijk) 0])]
set x2 [expr round([lindex $SemiSegmentor(Ext3D,Ijk) 1])]

set u1 [expr round([lindex $SemiSegmentor(Ext3D,Ijk) 2])]
set y2 [expr round([lindex $SemiSegmentor(Ext3D,Ijk) 3])]

set w1 [expr round([lindex $SemiSegmentor(Ext3D,Ijk) 4])]
set w2 [expr round([lindex $SemiSegmentor(Ext3D,Ijk) 5])]

set z2 [expr round($w1 + 0.6*(abs($w2-$w1)))]
set z1 [expr round($w1 + 0.2*(abs($w2-$w1)))]

set y1 [expr round(($y2 + $u1)/2)]

  vtkExtractVOI op
  op SetInput [Volume($volID,vol) GetOutput]
  op SetVOI $x1 $x2 $y1 $y2 $z1 $z2
  op Update
 
  set newvol [SemiSegmentorAddMrmlImage $volID $SemiSegmentor(OutputName)]
  set res [op GetOutput]


  $res SetExtent 0 [expr $x2-$x1] 0 [expr $y2-$y1] 0 [expr $z2-$z1]
  Volume($newvol,vol) SetImageData  $res
  # DISCONNECT the VTK PIPELINE !!!!....
  op SetOutput ""
  op Delete


    if {$::Module(verbose) == 1} {
        puts [[Volume($newvol,vol) GetOutput] GetExtent]
    }
  #Reseting the Extent so goes from 0.
  [Volume($newvol,vol) GetOutput] SetExtent 0 [expr $x2-$x1] 0 [expr $y2-$y1] 0 [expr $z2-$z1]
    if {$::Module(verbose) == 1} {
        puts [[Volume($newvol,vol) GetOutput] GetExtent]
    }

  # Set  new dimensions
  set dim [Volume($volID,node) GetDimensions]
  Volume($newvol,node) SetDimensions [expr $x2-$x1+1]  [expr $y2-$y1+1]

  # Set  new range
  set range   [Volume($volID,node) GetImageRange]
  Volume($newvol,node) SetImageRange $z1 $z2


  MainUpdateMRML
  MainVolumesUpdate $newvol

  # update matrices
  Volume($newvol,node) ComputeRasToIjkFromScanOrder [Volume($volID,node) GetScanOrder]

  # Set the RasToWld matrix
  # Ras2ToWld = Ras2ToIjk2 x Ijk2ToIjk1 x Ijk1ToRas1 x Ras1ToWld
    if {$::Module(verbose) == 1} {
        puts "Set the RasToWld matrix\n"
    }
  set ras1wld1 [Volume($volID,node)   GetRasToWld]

  # It's weird ... : I need to call SetRasToWld in order to update RasToIjk !!!
  Volume($newvol,node) SetRasToWld $ras1wld1

  MainVolumesUpdate $newvol

  MainMrmlUpdateMRML
  #
  # Add a Transform 
  #

  set tid [DataAddTransform 0 Volume($newvol,node) Volume($newvol,node)]

  #
  # Set the Transform
  #
  set n Matrix($tid,node)

  set Dx  [lindex  [Volume($volID,node) GetDimensions] 0]
  set Dy  [lindex  [Volume($volID,node) GetDimensions] 1]
  set Dz1 [lindex  [Volume($volID,node) GetImageRange] 0]
  set Dz2 [lindex  [Volume($volID,node) GetImageRange] 1]

  set dx  [lindex  [Volume($newvol,node) GetDimensions] 0]
  set dy  [lindex  [Volume($newvol,node) GetDimensions] 1]
  set dz1 [lindex  [Volume($newvol,node) GetImageRange] 0]
  set dz2 [lindex  [Volume($newvol,node) GetImageRange] 1]

  set ras2ijk2 [Volume($newvol,node) GetRasToIjk]

  vtkMatrix4x4 ijk2ijk1
  ijk2ijk1 Identity
  ijk2ijk1 SetElement 0 3 $x1
  ijk2ijk1 SetElement 1 3 $y1
  ijk2ijk1 SetElement 2 3 $z1

  vtkMatrix4x4 ijk1ras1 
  ijk1ras1 DeepCopy [Volume($volID,node) GetRasToIjk]
  ijk1ras1 Invert

  vtkMatrix4x4 ras2ras1
  ras2ras1 Identity
  ras2ras1 Multiply4x4 ijk2ijk1  $ras2ijk2  ras2ras1
  ras2ras1 Multiply4x4 ijk1ras1  ras2ras1   ras2ras1

  vtkTransform transf
  transf SetMatrix ras2ras1
  $n SetTransform transf

  MainMrmlUpdateMRML

  ijk2ijk1    Delete
  ijk1ras1    Delete
  ras2ras1    Delete
  transf      Delete

# END HERE


}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorAddMrmlImage
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorAddMrmlImage {volID resname } {

  global SemiSegmentor Volume

  set newvol [DevCreateNewCopiedVolume $volID ""  $resname ]
  set node [Volume($newvol,vol) GetMrmlNode]
  Mrml(dataTree) RemoveItem $node 
  set nodeBefore [Volume($volID,vol) GetMrmlNode]
  Mrml(dataTree) InsertAfterItem $nodeBefore $node
  MainUpdateMRML

  return $newvol
}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorBuildVTK
# Build any vtk objects you wish here
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorBuildVTK {} {
global SemiSegmentor

vtkMatrix4x4 SemiSegmentor(tmpMatrix)
vtkMatrix4x4 SemiSegmentor(tmp2Matrix)

}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorEnter
# Called when this module is entered by the user.  Pushes the event manager
# for this module. 
# .ARGS
# .END
#-------------------------------------------------------------------------------

proc SemiSegmentorEnter {} {
    global SemiSegmentor
    
    # Push event manager
    #------------------------------------
    # Description:
    #   So that this module's event bindings don't conflict with other 
    #   modules, use our bindings only when the user is in this module.
    #   The pushEventManager routine saves the previous bindings on 
    #   a stack and binds our new ones.
    #   (See slicer/program/tcl-shared/Events.tcl for more details.)
    pushEventManager $SemiSegmentor(eventManager)
    
    #Set all slicer windows to have slices orientation
    #MainSlicesSetOrientAll Slices
    
    #Create 3D cube
    SemiSegmentorCreate3DCube

    #Change welcome logo if it exits under ./image
    set modulepath $::PACKAGE_DIR_VTKSemiSegmentor/../../../images
    if {[file exist [ExpandPath [file join \
                     $modulepath "welcome.ppm"]]]} {
        image create photo iWelcome \
        -file [ExpandPath [file join $modulepath "welcome.ppm"]]
    }    


}


#-------------------------------------------------------------------------------
# .PROC SemiSegmentorExit
# Called when this module is exited by the user.  Pops the event manager
# for this module.  
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorExit {} {

    # Pop event manager
    #------------------------------------
    # Description:
    #   Use this with pushEventManager.  popEventManager removes our 
    #   bindings when the user exits the module, and replaces the 
    #   previous ones.
    #
    popEventManager
    
    #Remove 3D Cube    y************************IMPORTANT!!!
    SemiSegmentorDelete3DCube

    #Restore standard slicer logo
    image create photo iWelcome \
        -file [ExpandPath [file join gui "welcome.ppm"]]
}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorUpdateGUI
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorUpdateGUI {} {
    global SemiSegmentor Volume

    puts "in update GUI for Semi Segmentor module";
    #Hanifa
    #I changed the following line so that the GUI picks up the initial extent origin and spacing
    #values as soon as they are selected on the menu. This also required a change in Developer.tcl
    DevUpdateNodeSelectButton Volume SemiSegmentor VolumeIn VolumeIn DevSelectNode 1 0 1 SemiSegmentorGetInitParams
    DevUpdateNodeSelectButton Volume SemiSegmentor LabelVolumeIn LabelVolumeIn DevSelectNode 1 0 1 SemiSegmentorGetInitParams
     
   #DevUpdateNodeSelectButton Volume SemiSegmentor VolumeOut VolumeOut DevSelectNode 0 1 1
}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorGetInitParams
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorGetInitParams {} {
    global SemiSegmentor Volume Module    
    
    set volID $SemiSegmentor(VolumeIn)
    set volID2 $SemiSegmentor(LabelVolumeIn)

    set SemiSegmentor(InOrigin) [[Volume($volID,vol) GetOutput] GetOrigin]
    set SemiSegmentor(InSpacing) [[Volume($volID,vol) GetOutput] GetSpacing]
    set SemiSegmentor(InExtent) [[Volume($volID,vol) GetOutput] GetExtent] 
    
    set SemiSegmentor(OutputName) "[Volume($volID,node) GetName]_roi"
    #set SemiSegmentor(OutputName2) "[Volume($volID2,node) GetName]labelmap_roi"
    
    set Ext $SemiSegmentor(InExtent)
    
    SemiSegmentor(tmpMatrix) DeepCopy [Volume($volID,node) GetWldToIjk]
    
    
    SemiSegmentor(tmpMatrix) Invert SemiSegmentor(tmpMatrix) SemiSegmentor(tmpMatrix)    
    
    set IJKtoRASmin [SemiSegmentor(tmpMatrix) MultiplyPoint [lindex $Ext 0] [lindex $Ext 2] [lindex $Ext 4] 1]
    set IJKtoRASmax [SemiSegmentor(tmpMatrix) MultiplyPoint [lindex $Ext 1] [lindex $Ext 3] [lindex $Ext 5] 1] 
    

    if {$::Module(verbose) == 1} {
        puts $IJKtoRASmin

        puts $IJKtoRASmax
    }
            
    
    #Resort values
    foreach e "0 1 2" {
        if {[lindex $IJKtoRASmin $e] <= [lindex $IJKtoRASmax $e]} {
           lappend RASmin [lindex $IJKtoRASmin $e]
           lappend RASmax [lindex $IJKtoRASmax $e]
        } else {
           lappend RASmax [lindex $IJKtoRASmin $e]
           lappend RASmin [lindex $IJKtoRASmax $e]
        }      
   }
   
    if {$::Module(verbose) == 1} {
        puts $RASmin
        puts $RASmax 
    }

    set fDLPFC $Module(SemiSegmentor,fDLPFC)
    
    if {$volID == $SemiSegmentor(Ext3D,volId)} {
       #Nothing to do, same volume
    } else { 
    
       set SemiSegmentor(Ext3D,AxMin,min) [lindex $RASmin 2]
       set SemiSegmentor(Ext3D,AxMin,max) [lindex $RASmax 2]
       
       set SemiSegmentor(Ext3D,AxMax,min) [lindex $RASmin 2]
       set SemiSegmentor(Ext3D,AxMax,max) [lindex $RASmax 2]
       
       set SemiSegmentor(Ext3D,SagMin,min) [lindex $RASmin 0]
       set SemiSegmentor(Ext3D,SagMin,max) [lindex $RASmax 0]
       
       set SemiSegmentor(Ext3D,SagMax,min) [lindex $RASmin 0]
       set SemiSegmentor(Ext3D,SagMax,max) [lindex $RASmax 0]
       
       set SemiSegmentor(Ext3D,CorMin,min) [lindex $RASmin 1]
       set SemiSegmentor(Ext3D,CorMin,max) [lindex $RASmax 1]
       
       set SemiSegmentor(Ext3D,CorMax,min) [lindex $RASmin 1]
       set SemiSegmentor(Ext3D,CorMax,max) [lindex $RASmax 1] 
    
       set SemiSegmentor(Ext3D,volId) $volID
       
       foreach type "AxMin AxMax CorMin CorMax" {
         $fDLPFC.f3D.fControl.f$type.s$type configure -from $SemiSegmentor(Ext3D,$type,min) -to $SemiSegmentor(Ext3D,$type,max)
       }
       
       set SemiSegmentor(Ext3D,AxMin) [lindex $RASmin 2]
       set SemiSegmentor(Ext3D,AxMax) [lindex $RASmax 2]
       set SemiSegmentor(Ext3D,SagMax) [lindex $RASmax 0]
       set SemiSegmentor(Ext3D,SagMin) [lindex $RASmin 0]
       set SemiSegmentor(Ext3D,CorMin) [lindex $RASmin 1]
       set SemiSegmentor(Ext3D,CorMax) [lindex $RASmax 1]
    }


 if {$volID2 == $SemiSegmentor(Ext3D,volId2)} {
       #Nothing to do, same volume
    } else { 
    
       set SemiSegmentor(Ext3D,AxMin,min) [lindex $RASmin 2]
       set SemiSegmentor(Ext3D,AxMin,max) [lindex $RASmax 2]
       
       set SemiSegmentor(Ext3D,AxMax,min) [lindex $RASmin 2]
       set SemiSegmentor(Ext3D,AxMax,max) [lindex $RASmax 2]
       
       set SemiSegmentor(Ext3D,SagMin,min) [lindex $RASmin 0]
       set SemiSegmentor(Ext3D,SagMin,max) [lindex $RASmax 0]
       
       set SemiSegmentor(Ext3D,SagMax,min) [lindex $RASmin 0]
       set SemiSegmentor(Ext3D,SagMax,max) [lindex $RASmax 0]
       
       set SemiSegmentor(Ext3D,CorMin,min) [lindex $RASmin 1]
       set SemiSegmentor(Ext3D,CorMin,max) [lindex $RASmax 1]
       
       set SemiSegmentor(Ext3D,CorMax,min) [lindex $RASmin 1]
       set SemiSegmentor(Ext3D,CorMax,max) [lindex $RASmax 1] 
    
       set SemiSegmentor(Ext3D,volId2) $volID2
       
       foreach type "AxMin AxMax CorMin CorMax" {
         $fDLPFC.f3D.fControl.f$type.s$type configure -from $SemiSegmentor(Ext3D,$type,min) -to $SemiSegmentor(Ext3D,$type,max)
       }
       
       set SemiSegmentor(Ext3D,AxMin) [lindex $RASmin 2]
       set SemiSegmentor(Ext3D,AxMax) [lindex $RASmax 2]
       set SemiSegmentor(Ext3D,SagMax) [lindex $RASmax 0]
       set SemiSegmentor(Ext3D,SagMin) [lindex $RASmin 0]
       set SemiSegmentor(Ext3D,CorMin) [lindex $RASmin 1]
       set SemiSegmentor(Ext3D,CorMax) [lindex $RASmax 1]
    }




 SemiSegmentorRenderCube  

}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorPick3D
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorPick3D { type } {

  global SemiSegmentor 
  set SemiSegmentor(Ext3D,$type) [Slicer GetOffset $SemiSegmentor(Ext3D,$type,Id)]
  SemiSegmentorUpdate3DScales 0
}  

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorUpdate3DScales
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorUpdate3DScales { notUsed } {

  global SemiSegmentor Interactor
  
  set Ext $SemiSegmentor(InExtent)
  
  set volID $SemiSegmentor(VolumeIn)
  
  #Convert RAS ext to ijk ext
  
  SemiSegmentor(tmpMatrix) DeepCopy [Volume($volID,node) GetWldToIjk]
    
  #   set val [Volume($volID,node) GetRasToVtkMatrix]
  #  foreach r "0 1 2 3" {
  #     foreach c "0 1 2 3" {
  #       SemiSegmentor(tmpMatrix) SetElement $r $c [lindex $val [expr 4*$r + $c]]
  #      }
  #  }    
  
  set RAStoIJKmin [SemiSegmentor(tmpMatrix) MultiplyPoint $SemiSegmentor(Ext3D,SagMin) $SemiSegmentor(Ext3D,CorMin) $SemiSegmentor(Ext3D,AxMin) 1]
  set RAStoIJKmax [SemiSegmentor(tmpMatrix) MultiplyPoint $SemiSegmentor(Ext3D,SagMax) $SemiSegmentor(Ext3D,CorMax) $SemiSegmentor(Ext3D,AxMax) 1]
  
  
  
  foreach e "0 1 2" {
        if {[lindex $RAStoIJKmin $e] <= [lindex $RAStoIJKmax $e]} {
           lappend IJKmin [lindex $RAStoIJKmin $e]
           lappend IJKmax [lindex $RAStoIJKmax $e]
        } else {
           lappend IJKmax [lindex $RAStoIJKmin $e]
           lappend IJKmin [lindex $RAStoIJKmax $e]
        }      
   }
   
  set SemiSegmentor(Ext3D,Ijk) "[lindex $IJKmin 0] [lindex $IJKmax 0] [lindex $IJKmin 1] [lindex $IJKmax 1] [lindex $IJKmin 2] [lindex $IJKmax 2]"

    if {$::Module(verbose) == 1} {
        puts $SemiSegmentor(Ext3D,Ijk)
    }
  
    if { [info command SemiSegmentor(Ext3D,CubeActor)] == "" } {
        return
    }

  
  if {$SemiSegmentor(Ext3D,RenderCube) == 1} {
                           #[expr [lindex $Ext 1] - $SemiSegmentor(Ext3D,SagMax)] \
               #[expr [lindex $Ext 1] - $SemiSegmentor(Ext3D,SagMin)] \

    SemiSegmentor(Ext3D,Cube) SetBounds \
               $SemiSegmentor(Ext3D,SagMin) $SemiSegmentor(Ext3D,SagMax) \
               $SemiSegmentor(Ext3D,CorMin) $SemiSegmentor(Ext3D,CorMax) \
               $SemiSegmentor(Ext3D,AxMin) $SemiSegmentor(Ext3D,AxMax)
                     
    SemiSegmentor(tmp2Matrix) Identity
    
    SemiSegmentor(Ext3D,CubeXform) SetMatrix SemiSegmentor(tmp2Matrix)
    SemiSegmentor(Ext3D,CubeXform) Inverse
    
    SemiSegmentor(Ext3D,XformFilter) SetTransform SemiSegmentor(Ext3D,CubeXform)
  
    SemiSegmentor(Ext3D,CubeMapper) Update
    SemiSegmentor(Ext3D,OutlineMapper) Update
  
    eval [SemiSegmentor(Ext3D,CubeActor) GetProperty] SetColor $SemiSegmentor(Ext3D,CubeColor)
    eval [SemiSegmentor(Ext3D,CubeActor) GetProperty] SetOpacity $SemiSegmentor(Ext3D,CubeOpacity)
    
    eval [SemiSegmentor(Ext3D,OutlineActor) GetProperty] SetColor $SemiSegmentor(Ext3D,OutlineColor)
    eval [SemiSegmentor(Ext3D,OutlineActor) GetProperty] SetOpacity $SemiSegmentor(Ext3D,OutlineOpacity)  
  
  } else {
    eval [SemiSegmentor(Ext3D,CubeActor) GetProperty] SetOpacity 0
    eval [SemiSegmentor(Ext3D,OutlineActor) GetProperty] SetOpacity 0
  }  
  Render3D 

}



#-------------------------------------------------------------------------------
# .PROC SemiSegmentorCreate3DCube
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorCreate3DCube {} {

  global SemiSegmentor
  
  vtkCubeSource SemiSegmentor(Ext3D,Cube)
  vtkOutlineFilter SemiSegmentor(Ext3D,Outline)
  vtkTubeFilter SemiSegmentor(Ext3D,Tube)
  vtkTransform  SemiSegmentor(Ext3D,CubeXform)
  vtkTransformPolyDataFilter SemiSegmentor(Ext3D,XformFilter)
  vtkPolyDataMapper SemiSegmentor(Ext3D,CubeMapper)
  vtkPolyDataMapper SemiSegmentor(Ext3D,OutlineMapper)
  vtkActor SemiSegmentor(Ext3D,CubeActor)
  vtkActor SemiSegmentor(Ext3D,OutlineActor)
  #Create Pipeline
  
  SemiSegmentor(Ext3D,XformFilter) SetTransform SemiSegmentor(Ext3D,CubeXform)
  SemiSegmentor(Ext3D,XformFilter) SetInput [SemiSegmentor(Ext3D,Cube) GetOutput]
  SemiSegmentor(Ext3D,Outline) SetInput [SemiSegmentor(Ext3D,XformFilter) GetOutput]
  SemiSegmentor(Ext3D,Tube) SetInput [SemiSegmentor(Ext3D,Outline) GetOutput]
  SemiSegmentor(Ext3D,Tube) SetRadius 0.1
  SemiSegmentor(Ext3D,CubeMapper) SetInput [SemiSegmentor(Ext3D,XformFilter) GetOutput]
  SemiSegmentor(Ext3D,CubeActor) SetMapper SemiSegmentor(Ext3D,CubeMapper)
  SemiSegmentor(Ext3D,OutlineMapper) SetInput [SemiSegmentor(Ext3D,Tube) GetOutput]
  SemiSegmentor(Ext3D,OutlineActor) SetMapper SemiSegmentor(Ext3D,OutlineMapper)
  #Set up default Actor properties
  eval "[SemiSegmentor(Ext3D,CubeActor) GetProperty] SetColor" $SemiSegmentor(Ext3D,CubeColor)
  eval "[SemiSegmentor(Ext3D,CubeActor) GetProperty] SetOpacity" 0
  SemiSegmentor(Ext3D,CubeActor) PickableOff
  eval "[SemiSegmentor(Ext3D,OutlineActor) GetProperty] SetColor" $SemiSegmentor(Ext3D,OutlineColor)
  eval "[SemiSegmentor(Ext3D,OutlineActor) GetProperty] SetOpacity" 0
  SemiSegmentor(Ext3D,OutlineActor) PickableOff
  
  
  #Add to the renderer
  MainAddActor SemiSegmentor(Ext3D,CubeActor)
  MainAddActor SemiSegmentor(Ext3D,OutlineActor)

}

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorDelete3DCube
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorDelete3DCube {} {
  
  MainRemoveActor SemiSegmentor(Ext3D,CubeActor)
  MainRemoveActor SemiSegmentor(Ext3D,OutlineActor)
  Render3D
  
  #Delete Objects
  SemiSegmentor(Ext3D,Cube) Delete
  SemiSegmentor(Ext3D,Outline) Delete
  SemiSegmentor(Ext3D,Tube) Delete
  SemiSegmentor(Ext3D,CubeXform) Delete
  SemiSegmentor(Ext3D,XformFilter) Delete
  SemiSegmentor(Ext3D,CubeMapper) Delete
  SemiSegmentor(Ext3D,CubeActor) Delete
  SemiSegmentor(Ext3D,OutlineMapper) Delete
  SemiSegmentor(Ext3D,OutlineActor) Delete
}  

#-------------------------------------------------------------------------------
# .PROC SemiSegmentorRenderCube
# 
# .ARGS
# .END
#-------------------------------------------------------------------------------
proc SemiSegmentorRenderCube { } {

  global SemiSegmentor
  
  if { $SemiSegmentor(Ext3D,RenderCube) == 1} {
    SemiSegmentorUpdate3DScales 0
  } else {
    eval [SemiSegmentor(Ext3D,CubeActor) GetProperty] SetOpacity 0
    eval [SemiSegmentor(Ext3D,OutlineActor) GetProperty] SetOpacity 0
    Render3D  
  }
}

##### VOLUMES TO PASS TO BAYESIAN SEGMENTATION
# 1. LABELMAP VOLUME:
