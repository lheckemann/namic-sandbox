######## Adapted from http://wiki.tcl.tk/14770
# The argument processor.
proc userargs {_arglist _args} {
 set _init [list]
 set _initv [list]

#START-INIT
 foreach _a $_arglist {
   lappend _names [lindex $_a 0]
   switch [llength $_a] {
     1 {
     }
     2 {
       foreach {_n _d} $_a {break}
       lappend _init $_n
       lappend _initv $_d
     }
     3 {
       foreach {_n _d _e} $_a {break}
       if {$_d ne {-}} {
         lappend _init $_n
         lappend _initv $_d
       }
       switch $_e {
         required {
           lappend _rq $_n
         }
       }
     }
     default {
       error "Dont know what to do with 3+ options for $a"
     }
   }
 }

#END-INIT

 if {[llength $_init]} {
   uplevel 1   [list foreach $_init  $_initv {break}]
 }

#START-ARGS
 set _ai -1
 set _fnd [list]
 for {set _i 0} {$_i < [llength $_args]} {incr _i} {
   switch -- [set _v [lindex $_args $_i]] {
     - {
       incr _ai
     }
     -- {
       uplevel 1 [list set [lindex $_names [set _ai [expr {($_ai+1) % [llength $_names]}]]] [lindex $_args [incr _i]] ]
       lappend _fnd [lindex $_names $_ai]
     }
     default {
       if {[lsearch -exact $_names $_v] != -1 } {
         uplevel 1 [list set $_v [lindex $_args [incr _i]]]
         lappend _fnd $_v
       } else {
         uplevel 1 [list set [lindex $_names [set _ai [expr {($_ai+1) % [llength $_names]}]]] $_v ]
         lappend _fnd [lindex $_names $_ai]

       }
     }
   }
 }

#END-ARGS
 if {[llength $_rq]} {
#START-REQUIRED
   foreach _i $_rq {
     if {[lsearch $_fnd $_i] == -1} {
##--## uplevel 1 [list error "missing argument $_i"]
##--##
       uplevel 1 [ list return -1 ]
     }
   }
#END-REQUIRED
 }
}
# The TCL Way
proc b2_proc_generator {name _arglist body} {
# get the text of the userargs procedure
  set b [info body userargs]
## Initialize to empty lists so that they exists
  set _init [list]
  set _rq [list]
# pull out the two sections of interest
  foreach {- init} [regexp -inline {#START-INIT(.*?)#END-INIT} $b] {break}
  foreach {- loop} [regexp -inline {#START-ARGS(.*?)#END-ARGS} $b] {break}
  foreach {- required} [regexp -inline {#START-REQUIRED(.*?)#END-REQUIRED} $b] {break}

# process the arguments using the code from userargs
  eval $init

  set newbody {}
  if {[llength $_init]} {
    append newbody "\n#Initialise arguments\n"
      append newbody "foreach {$_init} {$_initv} {break}\n\n"
##--##      puts "init $_init val $_initv"
  }

# specialise length of named arguments
  regsub -all {[[]llength [$]_names[]]} $loop [llength $_names] loop
# redefine the args variable
  regsub -all {_args} $loop {args} loop
# replace names with actual names
  regsub -all {\$_names} $loop "{$_names}" loop
# remove uplevel
  regsub -all -lineanchor {uplevel 1 [[]list (.*?)[]]$} $loop {\1} loop
  append newbody "# Process arguments\n"
  append newbody $loop
  if {[llength $_rq]} {
    regsub -all {\$_rq} $required "{$_rq}" required
    regsub -all -lineanchor {uplevel 1 [[]list (.*?)[]]$} $required {\1} required
    append newbody "\n#Check on required arguments"
    append newbody $required
  }
  append newbody "\n#Start of body code\n\n"
  append newbody $body
# Now define the proc
  proc $name {args} $newbody
##--##  puts "$name:\n[info body $name]"
}

## Each of imageNodeID, printstring1, and printstring2 may be assumed as first unnamed positional, or as named arguments
## imageNodeID is required
## printstring1 is defined always, but does is not explicitly required (defaults to p1)
## printstring2 is defined always, but does is not explicitly required (defaults to p2)
## printstring3 is only defined when the the argument is named
## printstring4 is only defined when the the argument is named
b2_proc_generator b2_tester { {imageNodeID - required} {printstring1 - required} {printstring2 p2 } printstring3 printstring4 } {
##### Body stuff here
  puts "imageNodeID ${imageNodeID}"
    puts "printstring1 ${printstring1}"
    puts "printstring2 ${printstring2}"
    if { [ info exists printstring3 ] } then {
      puts "printstring3 ${printstring3}"
    }
  if { [ info exists printstring4 ] } then {
    puts "printstring4 ${printstring4}"
  }
}

b2_proc_generator b2_tester2 { {imageNodeID - required} {printstring1 - required} } {
##### Body stuff here
  puts "imageNodeID ${imageNodeID}"
  puts "printstring1 ${printstring1}"
}

b2_proc_generator b2_tester3 { {imageNodeID - required} } {
##### Body stuff here
  puts "imageNodeID ${imageNodeID}"
  puts "printstring1 ${printstring1}"
}

## Each of imageNodeID, printstring1, and printstring2 may be assumed as first unnamed positional, or as named arguments
## imageNodeID is required
## printstring1 is defined always, but does is not explicitly required (defaults to p1)
## printstring2 is defined always, but does is not explicitly required (defaults to p2)
## printstring3 is only defined when the the argument is named
## printstring4 is only defined when the the argument is named
b2_proc_generator b2_tester { {imageNodeID - required} {printstring1 p1 } {printstring2 p2 } printstring3 printstring4 } {
##### Body stuff here
  puts "imageNodeID ${imageNodeID}"
    puts "printstring1 ${printstring1}"
    puts "printstring2 ${printstring2}"
    if { [ info exists printstring3 ] } then {
      puts "printstring3 ${printstring3}"
    }
  if { [ info exists printstring4 ] } then {
    puts "printstring4 ${printstring4}"
  }
}


####################### Josiah From Here down is where you need to pay attention.
## Note: This needs to be sourced first.
## NOTE:  The DISPLAY environmental variable MUST BE SET TO A VALID DISPLAY!
## Run with ~/src/Slicer3-build/Slicer3 --no-splash -f ~/NewScript.tcl
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
#  set BRAINSScriptingColorLogic [$::slicer3::ColorGUI GetLogic]
#  set BRAINSScriptingFiducialsLogic [ $::slicer3::FiducialsGUI GetLogic]
  set BRAINSScriptingMRMLisGUI "true"
} else {
  ## If not in gui mode (--no-splash -f )
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  puts "SETTING MRML TO THE BATCH MODE New Instance of a SLICER SCENE"
  set BRAINSScriptingScene [ vtkMRMLScene New ]
  set BRAINSScriptingVolumesLogic [ $::slicer3::VolumesGUI GetLogic]
#  set BRAINSScriptingColorLogic [$::slicer3::ColorGUI GetLogic]
#  set BRAINSScriptingFiducialsLogic [ $::slicer3::FiducialsGUI GetLogic]
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
  if {$::BRAINSScriptingMRMLisGUI == "false" } then {
     [ GetBatchMRMLScene ]  Delete
     if { [ info exists BRAINSScriptingVolumesLogic ] } then {
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
set FileReaderIncrementUniqueIdCounter 0;

## Simple Gaussian
b2_proc_generator b2_gaussian_filter { { volumeNode - required } { RadiusFactor 2.0 } } {
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


###############################################
###############################################
###############################################
###############################################

#Usage  b2 load blobs  <blob-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  This will load blobs that are the areas of
#       significant activation in functional images. They have
#       a threshold and a magnitude (i.e. negative or
#       positive) associated with them.
#<blob-file>  The full or relative path to the blob file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created blobs
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_blobs { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load histogram  <histogram-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads an image histogram.
#<histogram-file>  The full or relative path to the histogram file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created histogram
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_histogram { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load image  <<image-file1>
#Description  Loads a small set of images for 1- or 3-color display.
#<<image-file1> ...>  up to 5 (IPL_MAX_CHANNELS) of display information.
#       A single name counts as a singleton list.
#Return  Upon successful completion a newly created image
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_image { { fileName - required }  {centered 0} {labelimage 0} {NodeName EmptyStringValue} } {
  if { "${NodeName}" == "EmptyStringValue" } then {
    set NodeName [file tail $fileName]_$::FileReaderIncrementUniqueIdCounter;
    incr ::FileReaderIncrementUniqueIdCounter;
  }
# load_options is a bit masks defining how to treat the loaded image
  set centered 0;
  set orient 0;
  set label 0;
  set singleFile 0;
  set loadingOptions [expr $label * 1 + $centered * 2 + $singleFile * 4 + $orient * 16];

  set volumeNode [ [ GetBatchVolumesLogic ] AddArchetypeVolume "$fileName" ${NodeName} ${loadingOptions} ]

  set selNode [$::slicer3::ApplicationLogic GetSelectionNode]
  if { $volumeNode == "" } {
    $this errorDialog "Could not load $fileName as a volume"
    return -1
  } else {
    if { $label } {
      $selNode SetReferenceActiveLabelVolumeID [$volumeNode GetID]
    } else {
      $selNode SetReferenceActiveVolumeID [$volumeNode GetID]
    }
    $::slicer3::ApplicationLogic PropagateVolumeSelection
  }
  if { [ llength $volumeNode ] == 0 } {
    puts "ERROR in reading file $fileName"
    return -1;
  }
  return ${volumeNode}
}

#Usage  b2 load landmark  <landmark-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a landmark file-object containing a set of
#       named positions in the 3D image space.
#<landmark-file>  The full or relative path to the landmark file to
#       load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created landmark
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load mask  <mask-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a 'mask' for selecting a portion of image space.
#<mask-file>  The full or relative path to the mask file to load.
#Return  Upon successful completion a newly created mask
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_mask { { fileName - required } {NodeName EmptyStringValue} } {
  if { "${NodeName}" == "EmptyStringValue" } then {
    set NodeName [file tail $fileName]_m$::FileReaderIncrementUniqueIdCounter;
    incr ::FileReaderIncrementUniqueIdCounter;
  }
  return [ b2_load_image  ${fileName} 1 1 ${NodeName} ];
}

#Usage  b2 load palette  <palette-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a color lookup palette for adjusting image
#       colors.
#<palette-file>  The full or relative path to the palette file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created palette
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_palette { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load roi  <roi-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a Region Of Interest (roi) file containing
#       line traces to generate masks.
#<roi-file>  The full or relative path to the ROI file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created roi
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load GTSurface  <gts-surface-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a sub-pixel triangle isosurface for defining
#       the cortex based on the tissue-classified image.
#<surface-file>  The full or relative path to the GTS surface file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created GTS surface
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load table  <table-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a 2D table of floating point data.
#<table-file>  The full or relative path to the table file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created table
#       object identifier is returned,  otherwise an error
#       status of -1 is returned.
b2_proc_generator b2_load_table { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load talairach-box  <talairach-box-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Loads a definition of a portion of Talairach-mm space
#       built up out of boxes.
#<talairach-box-file>  The full or relative path to the talairach-box file
#       to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created
#       talairach-box object identifier is returned,
#       otherwise an error status of -1 is returned.
b2_proc_generator b2_load_talairach-box { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load talairach-parameters  <talairach-par-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Load a file defining Talairach atlas space
#       with respect to a specific image.
#<talairach-par-file>  The full or relative path to the talairach-parameters
#       file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created
#       talairach-parameters object identifier is returned,
#       otherwise an error status of -1 is returned.
b2_proc_generator b2_load_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load tissue-class  <tissue-class-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Load the tissue classifier's model definition file.
#<tissue-class-file>  The full or relative path to the tissue class model
#       file to load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created
#       tissue-class model object identifier is returned,
#       otherwise an error status of -1 is returned.
b2_proc_generator b2_load_tissue-class { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 load transform  <transform-file> <filter= filter-name> <filter-suffix= filter-suffix>
#Description  Load an image transform file specifying a spatial
#       affine transform, thin-plate warp transform,  etc.
#<transform-file>  The full or relative path to the transform file to
#       load.
#<filter= ..>  Used to explicitly specify a non-default filter
#       for loading or saving a file from disk.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return  Upon successful completion a newly created
#       transform object identifier is returned,
#       otherwise an error status of -1 is returned.
b2_proc_generator b2_load_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save GTSurface <file-name> <filter-name>  <surfaceID> <filter-suffix= filter-suffix>
#Description  This command will save a GTS Surface object to the
#       specified file in the specified format.  The GTS Surface
#       is composed of triangles that represent an iso-surface
#       in an image.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<surfaceID>  An identifier for a surface
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save blobs <file-name> <filter-name>  <blobID> <filter-suffix= filter-suffix>
#Description  This command will save blobs that are the areas of
#        significant activation in functional imaging study.
#        These contain the location of the center of mass and
#        the voxel with the largest absolute value.
#     Note:  saving the blobs object to a file is the way
#        to produce a human-readable text list of the blobs in either
#        of two possible formats, according to filter choice.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#        specifies the format of the data).
#<blobID>  An identifier for a blob object
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_blobs { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save histogram <file-name> <filter-name>  <histogramID> <filter-suffix= filter-suffix>
#Description  This command will save a histogram generated from an
#       image. The histogram represents the number of voxels
#       that are within various grey levels.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<histogramID>  An identifier for a histogram object
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_histogram { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save image <file-name> <filter-name>  <imageID> <data-type= data-type> <plane= plane> <channel= channel> <min-slice= min-slice> <max-slice= max-slice> <min-time= min-time> <max-time= max-time> <interp= interpolation> <filter-suffix= filter-suffix>
#Description  This command will save an image. Various options for
#       this command will control the resulting data type and
#       orientation of the image. Portions of the image can be
#       saved as well.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<imageID>  An identifier for an image
#<data-type= ..>  The data type for the resulting file. Valid types:
#          signed-8bit
#          unsigned-8bit
#          signed-16bit
#          unsigned-16bit
#          signed-32bit
#          unsigned-32bit
#          signed-64bit
#          unsigned-64bit
#          float-single
#          float-double
#<plane= ..>  The orientation of the image in the resulting file.
#       Axial | Coronal | Sagittal
#<channel= ..>  The channel of the image to write to a file. The
#       default is all channels.
#<min-slice= ..>  The minimum slice location of the image to include
#       in the file for the specified orientation.
#<max-slice= ..>  The maximum slice location of the image to include
#       in the file for the specified orientation.
#<min-time= ..>  The minimum time index of the image to include in
#       the file.
#<max-time= ..>  The maximum time index of the image to include in
#       the file.
#<interp= >  The interpolation method to be used if a transform is
#       applied to the image. Trilinear | nearest-neighbor.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_image { { ImageToSave - required } { outputFileName - required } } {
    set VolumeStorageNode [ vtkMRMLVolumeArchetypeStorageNode New ]
    $VolumeStorageNode SetFileName ${outputFileName}
    $VolumeStorageNode WriteData ${ImageToSave}
    $VolumeStorageNode Delete
}

#Usage  b2 save landmark <file-name> <filter-name>  <landmarkID> <filter-suffix= filter-suffix>
#Description  This command will save a landmark object to the
#       specified file in the specified format.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<landmarkID>  An identifier for a landmark object
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save mask <file-name> <filter-name>  <maskID> <filter-suffix= filter-suffix>
#Description  This command will save a mask object to the specified
#       file in the specified format.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<maskID>  An identifier for a mask
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save palette <file-name> <filter-name>  <paletteID> <filter-suffix= filter-suffix>
#Description  This command will save a palette object to the
#       specified file in the specified format.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<paletteID>  An identifier for a palette
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_palette { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save roi <file-name> <filter-name>  <roiID> <plane= plane> <filter-suffix= filter-suffix>
#Description  This command will save an ROI object to the specified
#       file in the specified format. The plane option
#       appends an orientation onto the filename if one does
#       not already exist.  The ROI contains a series of
#       points that define the boundary of a region.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<roiID>   An identifier for an roi
#<plane= >  The extention to add to the file name if one is not
#       provided. Valid planes: axial, coronal, sagittal.
#       These correspond to .yroi, .zroi, and .xroi.
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save table <file-name> <filter-name>  <tableID> <filter-suffix= filter-suffix>
#Description  This command will save a Table object to the
#       specified file in the specified format. The Table is
#       a two-dimensional matrix of floating-point numbers.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<tableID>  An identifier for a table
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_table { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save talairach-box <file-name> <filter-name>  <talairach-boxID> <filter-suffix= filter-suffix>
#Description  This command will save a Talairach-Box object to the
#       specified file in the specified format.  The box
#       defines a region of the Talairach atlas and allows
#       for automated region identification, using the
#       talairach piece-wise linear warping algorithm.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<talairach-boxID>  An identifier for a talairach-box object
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_talairach-box { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save talairach-parameters <file-name> <filter-name>  <talairach-parametersID> <filter-suffix= filter-suffix>
#Description  This command will save a Talairach-Parameters object
#       to the specified file in the specified format.  The
#       parameters define four points AC, PC, Superior-Left-
#       Anterior and Inferior-Right-Posterior aspect of the
#       brain. Thes points are used to define the talairach
#       piece-wise linear warping algorithm.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<talairach-parametersID>  An identifier for a talairach-parameter object
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save tissue-class <file-name> <filter-name>  <tissue-classID> <filter-suffix= filter-suffix>
#Description  This command will save a Tissue-Class object to the
#       specified file in the specified format.  This defines
#       the model that is used to generate the tissue
#       classified image.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<tissue-classID>  An identifier for a tissue-class object
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_tissue-class { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 save transform <file-name> <filter-name>  <transformID> <filter-suffix= filter-suffix>
#Description  This command will save a Transform object to the
#       specified file in the specified format. The transform
#       can be applied to an image to resample the original
#       image (reslice) into a new space (standard). This may
#       involve a different matrix size and resolutions as
#       well.
#<file-name>  The full or relative path of the resulting file.
#<filter-name>  The filter to be used to save the file (i.e.
#       specifies the format of the data).
#<transformID>  An identifier for a transform
#<filter-suffix= ..>  Used to specify a filter command line suffix
#        when creating the I/O pipe.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_save_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create GTSurface <parameter-list> <bound-list> <decimation-list> <compute-depths= yes | no> <compute-convexities= yes | no> <singly-connected= yes | no> <copy-surface= prior-surface> <copy-curvature-surface= prior-curvature-surface> <depth-surface-list= <list GTSurface-1 ... GTSurface-N>> <depth-layer-intensity-list= <list float-1 ... float-N>>
#Description  Command to generate a GTS iso-surface for the specified
#       image and mask. The mask limits the region where the
#       surface will be generated in the image.  If only an
#       image is specified then the entire image is used to
#       generate the surface. If only a mask is specified then
#       a surface will be generated from the mask.
#<parameter-list>  <image mask high-mask cube-size threshold far-threshold>
#       <image | -1>  : Parameter defining in what image the
#           intensity-based iso-surface should be created.
#           If the image is specified as -1 then the surface
#           is created using the mask only.
#       <mask  | -1>  : Parameter defining where surface may
#           be created.  If the mask is specified as
#           -1 then the surface is created using the
#           entire image.
#       <high-mask | -1>: Parameter defining where surface should
#           not be created.  If the mask is specified
#           as -1 then the no such guide is used.
#       <cube-size>   : The size of the cube in voxels used for
#           the Wyvill surface generation. This specifies
#           the cube considered by the iso-surface generation.
#           Usually chosen to be equivalent to 1 mm.
#       <threshold>   : The image iso-surface threshold used to
#           define the triangle surface.  Usually 130.
#       <far-threshold>: The image iso-surface threshold used to
#           define the triangle surface depth.  Usually 190.
#<bound-list>  <xmin xmax ymin ymax zmin zmax>
#       <xmin>  : The minimum x coordinate used to positionally
#           limit iso-surface generation.  -1 means to use
#           the entire image.
#       <xmax  | -1>  : The maximum x coordinate used to positionally
#           limit iso-surface generation.  -1 means to use
#           the entire image.
#       <ymin>  : The minimum y coordinate used to positionally
#           limit iso-surface generation.  -1 means to use
#           the entire image.
#       <ymax  | -1>  : The maximum y coordinate used to positionally
#           limit iso-surface generation.  -1 means to use
#           the entire image.
#       <zmin>  : The minimum z coordinate used to positionally
#           limit iso-surface generation.  -1 means to use
#           the entire image.
#       <zmax  | -1>  : The maximum z coordinate used to positionally
#           limit iso-surface generation.  -1 means to use
#           the entire image.
#<decimation-list>
#       <'GTS' minFaceAmgle reduce volumeWeight boundaryWeight
#         shapeWeight minRad maxRad>
#       <'GTS'>   : An identifier specifying the use of GTS
#           volume optimized decimation.
#       <minFaceAmgle>: The minimum angular spread between
#           adjacent faces. Suggested value: 0.174533,
#           that is 10 degrees in radians.
#       <reduce>  : The percentage of edges to keep after
#           decimation, if less than 1.0;  otherwise,
#           the number of edges to keep after decimation.
#           Suggested value: 240000.0 or 0.35.
#           To turn off decimation, use value: 1.0
#       <volumeWeight>: A parameter governing surface coarsening.
#           Suggested value: 0.5.
#       <boundaryWeight>: A parameter governing surface coarsening.
#           Suggested value: 0.5.
#       <shapeWeight> : A parameter governing surface coarsening.
#           Suggested value: 0.0.
#<copy-surface= prior-surface>  An optional surface to start from.
#<copy-curvature-surface= prior-curvature-surface>  An optional surface to copy curvatures from.
#<depth-surface-list= <list GTSurface-1 ... GTSurface-N>>
#         A possibly very short list of inner surfaces to
#         measure the shortest distance to, defining a curved
#         path through the cortical plate.  'far-threshold' in
#         'parameter-list' above should be the iso-threshold
#         used to generate the last surface in the list.
#<depth-layer-intensity-list= <list float-1 ... float-N>>
#         A possibly very short list of inner surfaces to
#         generate surfaces on the fly for use as in
#         'depth-surface-list=' above.  'far-threshold' in
#         'parameter-list' above should be the last
#         iso-threshold in this list.
#<compute-depths= yes | no>  An optional request to turn depth
#         generation on or off.
#<compute-convexities= yes | no>  An optional request to turn
#         convexity generation on or off.
#<singly-connected= yes | no>  An optional request to choose the
#         largest singly connected piece of the main surface.
#       If you have given a copy-surface= parameter to start from,
#         your copy will not have curvatures and depths
#         unless you recompute them.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create gts-rois <surface>  <plane> <offset= value> <name= roi-name>
#Description  This command generate ROIs for a given surface for each slice
#       in the specified plane. The offset is the location in the slice
#       where the ROI is generate. By default this is in the center of the plane.
#<surface>  Surface from which ROIs will be generated
#<plane>  The plane of the resulting ROI. Valid values are:
#       x | y | z | Axial | Coronal | Sagittal
#&ltoffset=&gt  Offset to the surface plane intersection (default is 0.5)
#&ltname=&gt  Name of the resulting ROIs
#Return  Returns the resulting ROI.  Otherwise an error status of
#       -1 is returned.
b2_proc_generator b2_create_gts-rois { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create affine-transform <standard-dims-list> <standard-res-list> <reslice-dims-list> <reslice-res-list> <rx= F32> <ry= F32> <rz= F32> <dx= F32> <dy= F32> <dz= F32>
#Description  This command will create a resampling transform
#        that maps from the reslice image space to the standard image space.
#        You are supposed to set the resulting transform on the reslice image.
#<standard-dims-list>  A string list beginning with the xdim, ydim and zdim
#            integers for defining the standard image space.
#<standard-res-list>  A string list beginning with the xres, yres and zres
#            decimal numbers for defining the standard image space.
#<reslice-dims-list>  A string list beginning with the xdim, ydim and zdim
#            integers for defining the reslice image space.
#<reslice-res-list>  A string list beginning with the xres, yres and zres
#            decimal numbers for defining the reslice image space.
#<rx= F32>  A rotation about the x axis, pitch, given in radians.
#<ry= F32>  A rotation about the y axis, yaw, given in radians.
#<rz= F32>  A rotation about the z axis, roll, given in radians.
#<dx= F32>  A translation along the x axis, lateral, given in mm.
#<dy= F32>  A translation along the y axis, inferior-superior, given in mm.
#<dz= F32>  A translation along the z axis, rostro-caudal, given in mm.
#Hint:      You can generate the identity tranform for an image with
#        b2 create affine-transform <b2 get dims image $i> <b2 get res image $i> <b2 get dims image $i> <b2 get res image $i>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_affine-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create bounded-image <imageID> <xmin> <xmax> <ymin> <ymax> <zmin> <zmax>
#Description  Create a rectangularly clipped sub-image with an
#       affine transform to store and locate it efficiently.
#<imageID>  An identifier for an image
#<xmin>  xmin gives minimum corner of rectangular bounds.
#<xmax>  xmax gives maximum corner of rectangular bounds.
#<ymin>  ymin gives minimum corner of rectangular bounds.
#<ymax>  ymax gives maximum corner of rectangular bounds.
#<zmin>  zmin gives minimum corner of rectangular bounds.
#<zmax>  zmax gives maximum corner of rectangular bounds.
#Return  Returns a list of image-index and transform-index.
#       The image transform is set to the given transform.
#       Otherwise an error status of -1 is returned.
b2_proc_generator b2_create_bounded-image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create talairach-parameters  <reslice-dims-list> <reslice-res-list>  <AC-Point-list> <PC-Point-list> <SLA-Point-list> <IRP-Point-list><PatId= pid><ScanId= m rqid>
#Description  This command will create a resampling transform
#        that maps from the reslice image space to the standard image space.
#        You are supposed to set the resulting transform on the reslice image.
#<reslice-dims-list>  A string list beginning with the xdim, ydim and zdim
#            integers for defining the reslice image space.
#<reslice-res-list>  A string list beginning with the xres, yres and zres
#            decimal numbers for defining the reslice image space.
#<AC-Point-list>  A string list beginning with the x, y and z
#            floats for defining the reslice image's AC Point.
#<PC-Point-list>  A string list beginning with the x, y and z
#            floats for defining the reslice image's PC Point.
#<SLA-Point-list>  A string list beginning with the x, y and z
#            floats for defining the reslice image's SLA Point.
#<IRP-Point-list>  A string list beginning with the x, y and z
#            floats for defining the reslice image's IRP Point.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create boxed-image  <imageID> <threshold= threshold>
#Description  Threshold the image to extract a sub-image with an
#       affine transform to store and locate it efficiently.
#<imageID>  An identifier for an image
#<threshold= ..>  Floating point value (0.0) <= selected image voxels.
#Return  Returns a list of image-index and transform-index.
#       The image transform is set to the given transform.
#       Otherwise an error status of -1 is returned.
b2_proc_generator b2_create_boxed-image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create bullet-image <foreground_byte> <background_byte> <xDim> <yDim> <zDim> <xRes> <yRes> <zRes> <xPos> <yPos> <zPos> <xSize> <ySize> <zSize>
#Description  Create a phantom image with rectangular foreground
#       with an affine transform to store and locate it
#       efficiently.
#<foreground_byte>  Image value within the 'bullet'.
#<background_byte>  Usually 0.
#<xDim>  xDim gives image size in voxels.
#<yDim>  yDim gives image size in voxels.
#<zDim>  zDim gives image size in voxels.
#<xRes>  xRes gives voxel size in mm.
#<yRes>  yRes gives voxel size in mm.
#<zRes>  zRes gives voxel size in mm.
#<xPos>  xPos specifies the minimum-coordinate 'bullet' corner.
#<yPos>  yPos specifies the minimum-coordinate 'bullet' corner.
#<zPos>  zPos specifies the minimum-coordinate 'bullet' corner.
#<xSize>  xSize gives size of bullet rectangle in voxels.
#<ySize>  ySize gives size of bullet rectangle in voxels.
#<zSize>  zSize gives size of bullet rectangle in voxels.
#Return  Returns a list of image-index and transform-index.
#       The image transform is set to the given transform.
#       Otherwise an error status of -1 is returned.
b2_proc_generator b2_create_bullet-image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create event-table  <event-table> <mask-table>
#Description  Creates a new table containing only the event times
#       of interest. This is generated by masking the event
#       table with the mask table. The mask table can be
#       generated with the command 'b2 create table-mask'.
#<event-table>  Table containing all of the events.
#<mask-table>  Table containing a mask of events of interest.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_event-table { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create hem-roi <imageID> <x-loc= integer>
#Description  Creates an axial ROI that divides the brain into
#       left and right.  This ROI is edited for separating
#       the hemispheres when generating the cortical surface.
#<imageID>  An identifier for an image
#<x-loc= integer>  The x (sagittal) position for the resulting ROI
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_hem-roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create histogram <imageID>  <maskID> <bin-size= value>
#Description  Creates a histogram from an image within the
#       specified mask. Areas outside the mask are not
#       considered in the histogram.
#<imageID>  An identifier for an image
#<maskID>  An identifier for a mask
#<bin-size= value>  The step size for creating the histogram. Must be greater than 0.0.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_histogram { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create roi-hull <roi>  <2D | 3D>
#Description  Only 2D presently works.  Converts planar roi into
#       planar convex hull roi.
#<roiID>   An identifier for an roi
#<2D | 3D>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_roi-hull { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create table-mask <nRows> <nColumns> <list of row column lists>
#Description  Creates a new table containing 1.0 at the specified row
#       and column locations and 0.0 everywhere else.
#<nRows>  Number of rows in the resulting table.
#<list of <row column>>  A lists of lists containing the elements of the table to be set to
#       1.0. Each sublists contains the row and column in that order of the
#       element to be set to 1.0.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_table-mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create witelson-rois <roi>  <landmarkID>
#Description  This command will divide a corpus callosum ROI into
#       7 segments baed on the Witelson method.
#       The ladnamrk point represents the notch of the Genu.
#<roiID>   An identifier for an roi
#<landmarkID>  A landmark points defining the notch of the Genu.
#Return  Returns a list of rois.  Otherwise an error status of
#       -1 is returned.
b2_proc_generator b2_create_witelson-rois { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create viewer <axi | sag| cor | surf | tracker>
#Description  This command will create the specified viewer(s). At
#       least one viewer must be specified.  This command is
#       not supported for user interaction and is run by
#       BRAINS2 when in graphics mode. The valid viewers are:
#       axi | sag| cor | surf | tracker.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_create_viewer { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create mask  <image> <shape= rectangle|ellipsoid > <name= mask-name > <size= <xsize ysize zsize> >< location= <xloc yloc zloc>>
#Description  Generates a mask with the user specified shape, location and size.
#<maskID>  An identifier for a mask
#<shape= rectangle|ellipsoid >  Shape of the created mask. Currently support rectangles and ellipses
#<size= <size list> >  Size of the structure in x, y, z
#<location= <location list>>  The center of the shape in x, y, z.
#<name= mask-name>  The basename for the resulting mask
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create image-table <T1-image> <T2-image>  <PD-image> <Count-table= table Sum-table= table> <mask= mask>
#Description  Accumulate the tables to define image reconstruction
#       of a third image (PD) based on the first two images
#       (T1 and T2).  If tables are omitted, it starts new
#       tables.  Limits the transfer function to voxels in
#       the given mask.
#       See Also: 'b2 finish image-table'.
#<T1-image>
#<T2-image>
#<PD-image>
#<Count-table= >
#<Sum-table= >
#<mask= mask>
#Return  Upon successful completion, returns a list of the
#       updated or created count table and sum table.  If an
#       error occurs, then the error status of -1 is returned.
b2_proc_generator b2_create_image-table { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 create roi  <roi-name> <plane= plane>
#Description  This command will generate an empty ROI in the
#       specified plane if an ROI with the same name does not
#       already exist. This is used to facilitate renaming
#       ROI strands where the renamed ROI must already exist.
#<roi-name>  Name for the empty ROI to create.
#<plane= plane>  Appends a direction postfix on the name if one does
#       not already exist. It can be be one of:
#         Axial | Coronal | Sagittal
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_create_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 color GTSurface labels <GTSurface_index> <labelFamilyNumber> <<red green blue alpha>, ...>
#Description  Color code regions of the surface by decoding one
#       of the surface's label families. Any udefined regions
#       remain the original gold color.
#<surfaceID>  An identifier for a surface
#<labelFamilyNumber>  Usually 0 since there is only one label family possible
#<color-list>  An optional of colors for coding the surface.
#       The list is of the form <<red green blue alpha>, ...>
#       When omitted the colors are copied from a standard
#       table supplied with FreeSurfer.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_color_GTSurface_labels { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 color surface masks <GTSurface_index> <mask_index, ...> <<red green blue alpha>, ...>
#Description  Color code regions of the surface by intersecting
#       the surface with a mask. Any udefined regions
#       remain the original gold color. If a triangle is
#       contained within multiple masks the last mask
#       color is used for the color.
#<surfaceID>  An identifier for a surface
#<mask-list>  A lists of mask that define various segments of
#       the surface
#<color-list>  A lists of colos for coding the surface.
#       The list is of the form <<red green blue alpha>, ...>
#        where both lists (mask and color) have the same length.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_color_GTSurface_masks { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 color surface normals  <GTSurface_index>
#Description  This command will display the surface normals
#       as a color for each triangle. This can be
#       visualized with the user color
#       option on the display properties dialog box.
#<surface_index>  The surface to colorize the surface normals.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_color_GTSurface_normals { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 color surface standard-depth  <GTSurface_index> <effect-size= value>
#Description  This command standardizes the color coding
#       used to display cortical depth. The routine
#       computes the mean and standard deviation over
#       the entire surface and displays the specified
#       number of standard deviations using the color
#       scale. This can be displayed with the user color
#       option on the display properties dialog box.
#<surface_index>  The surface to generate a normalized depth color
#       scale for.
#<effect-size= value>  The number of standard deviations to scale in the
#       color palette.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_color_GTSurface_standard-depth { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 color surface image <GTSurface_index> <image> <palette>
#Description  Color code regions of the surface by intersecting
#       the surface with an image. The color label
#        is based upon the palette.
#<surfaceID>  An identifier for a surface
#<imageID>  An identifier for an image
#<paletteID>  An identifier for a palette
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_color_GTSurface_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure GTSurface mask <maskId> <surface-1> <surface-2>... <surface-N> <subject-id= text> <scan-id= text>
#Description  Command to measure GTSurfaces characteristics within a
#       mask.  It returns a string that contains the measured
#       GTSurface characteristics.  The result is written to a
#       file b2_saved_triangle_curvatures, as well as an
#       alternate file if specified with 'b2 file' command.
#<maskID>  An identifier for a mask
#<surface-i>  One or more surface identifiers must be listed.
#Return  The command returns a list of lists that define the
#       measurement label and measurement result.
b2_proc_generator b2_measure_GTSurface_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure GTSurface roi <roiID> <surface-1> <surface-2>... <surface-N> <subject-id= text> <scan-id= text>
#Description  Command to measure GTSurfaces characteristics within an
#       ROI.  The ROI is converted into a mask before the
#       measurement is made.  It returns a string that
#       contains the measured GTSurface characteristics.  The
#       result is written to the file
#       b2_saved_triangle_curvatures, as well as an alternate
#       file if specified with 'b2 file' command.
#<roiID>   An identifier for an roi
#<surface-i>  One or more surface identifiers must be listed.
#Return  The command returns a list of lists that define the
#       measurement label and measurement result.
b2_proc_generator b2_measure_GTSurface_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure bounds mask <maskID>
#Description  Command to measure the spatial glb and lub of a mask. It
#       returns a list of lists containing the label, and the
#       upper or lower bound.
#<maskID>  An identifier for a mask
#Return  It returns a list of lists containing the bounds of the
#       region in voxels.
b2_proc_generator b2_measure_bounds_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure class-volume mask <maskID>  <imageID> <subject-id= text> <scan-id= text>
#Description  Command to measure tissue class volumes in classified
#       images contained inside the specified mask. The result
#       contains the tissue volumes (grey, white, csf, blood,
#       other). The result is also written to a file called
#       b2_saved_class_volumes and an alternate file if
#       specified with 'b2 file' command.
#<maskID>  An identifier for a mask
#<imageID>  An identifier for an image
#Return  This function returns a list of lists that contains
#       the tissue volumes (grey, white, csf, blood, other).
#       If an error occurs, then the error status of -1 is
#       returned
b2_proc_generator b2_measure_class-volume_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure class-volume roi <roiID>  <imageID> <min= min-slice> <max= max-slice> <subject-id= text> <scan-id= text>
#Description  Command to measure tissue class volumes in classified
#       images within a specified ROI. The ROI is first
#       converted into a mask before the measurement is made.
#       The result contains the tissue volumes (grey, white,
#       csf, blood, other). The result is also written to a
#       file called  b2_saved_class_volumes and an alternate
#       file if specified with 'b2 file' command.
#<roiID>   An identifier for an roi
#<imageID>  An identifier for an image
#<min= >  The minimum slice location for the ROI conversion into
#       a mask and resulting measurement.
#<max= >  The maximum slice location for the ROI conversion into
#       a mask and resulting measurement.
#Return  It returns a list of lists that contains the tissue
#       volumes (grey, white, csf, blood, other). The sublists
#       contain a description string, the measurement volume,
#       and the measurement units If an error occurs, then the
#       error status of -1 is returned
b2_proc_generator b2_measure_class-volume_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure image mask <maskID>  <imageID> <functional-flag= true|false> <time-index= index> <subject-id= text> <scan-id= text>
#Description  Command to measure statistics about the image in the
#       region defined by the mask. This information includes
#       the average value, standard deviation, variance,
#       kurtosis, skewness, minimum, maximum, location of
#       minimum, and location of maximum. The output is
#       written to the file b2_saved_image_stats as well as an
#       alternate file if specified with 'b2 file' command.
#<maskID>  An identifier for a mask
#<imageID>  An identifier for an image
#<functional-flag= >  Flag signaling if voxels in the mask with value of 0.0
#       should be computed.  When set to true, voxels in mask
#       with value of 0.0 are treated as if they are not in
#       the mask.   Default value is false, i.e. all voxels in
#       the mask are computed regardless of value.
#<time-index= >  Time index into a 4D dataset. By default the zero
#       index time point is used
#Return  The a list of lists containing the measurement
#       results.  The sublists contain the result description
#       and measurement value.  If an error occurs, then the
#       error status of -1 is returned
b2_proc_generator b2_measure_image_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure image roi <roiID>  <imageID> <min= min-slice> <max= max-slice> <functional-flag= true|false> <time-index= index> <subject-id= text> <scan-id= text>
#Description  Command to measure statistics about the image in the
#       region defined by the ROI. The ROI is first converted
#       to a mask before the measurement occurs.  This image
#       measurement information includes the average value,
#       standard deviation, variance, kurtosis, skewness,
#       minimum, maximum, location of minimum, and location of
#       maximum. The output is written to the file
#       b2_saved_image_stats as well as an alternate file if
#       specified with 'b2 file' command.
#<roiID>   An identifier for an roi
#<imageID>  An identifier for an image
#<min= >  The minimum slice for the conversion of the ROI to
#       mask.  All ROI strands before this slice location are
#       ignored.
#<max= >  The maximum slice for the conversion of the ROI to
#       mask.  All ROI strands after this slice location are
#       ignored.
#<functional-flag= >  Flag signalling that values of 0.0 should be ignored
#       and not included in the measurement. The valid values
#       are: true | false
#<time-index= >  Time index into a 4D dataset. By default the zero
#       index time point is used
#Return  The a list of lists containing the measurement
#       results.  The sublists contain the result description
#       and measurement value. If an error occurs, then the
#       error status of -1 is returned
b2_proc_generator b2_measure_image_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure landmark distance <landmarksetID> <point1-name> <point2-name> <subject-id= text> <scan-id= text>
#Description  Command to measure the Euclidean and plane distance
#       between two points in a landmark set. The output is
#       written to b2_saved_distance as well as an alternate
#       distance file if specified with 'b2 file' command.
#<landmarksetID>  An identifier for a landmark set
#<point1-name>  Starting desired landmark name
#<point2-name>  Ending desired landmark name
#Return  A lists of lists containing the measurement label and
#       distance in each of the sublists. If an error occurs,
#       then the error status of -1 is returned.
b2_proc_generator b2_measure_landmark_distance { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure resel-size mask <maskID>  <imageID>
#Description  Command to measure resel size and also the mask
#       volume in image-specific resels.
#<maskID>  An identifier for a mask
#<imageID>  An identifier for an image
#Return  This function returns a list of lists that contains
#       the resel-size and num-resels.
#       If an error occurs, then the error status of -1 is
#       returned
b2_proc_generator b2_measure_resel-size_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure volume mask <maskID> <subject-id= text> <scan-id= text>
#Description  Command to measure the volume of a mask. It returns a
#       list of lists containing the measurement units and the
#       volume of the region.  The result is also
#       written to a file called b2_saved_volumes and an
#       alternate file if specified by the 'b2 file' command.
#<maskID>  An identifier for a mask
#Return  It returns a list of list containing the volume of the
#       region in CubicCentimeters.
b2_proc_generator b2_measure_volume_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure volume roi <roiID> <min= min-slice> <max= max-slice> <subject-id= text> <scan-id= text>
#Description  Command to measure the volume of an ROI. The ROI is
#       converted to a mask before the measurement is made.
#       The result is also  written to a file called
#       b2_saved_volumes and an alternate file if specified by
#       the 'b2 file' command.
#<roiID>   An identifier for an roi
#<min= min-slice>  The minimum slice location for the ROI conversion into
#       a mask and resulting measurement.
#<max= max-slice>  The maximum slice location for the ROI conversion into
#       a mask and resulting measurement.
#Return  The command returns a list of lists that define the
#       measurement label and measurement result, and
#       measurement units. If an error occurs, then the error
#       status of -1 is returned.
b2_proc_generator b2_measure_volume_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 measure roi perimeter <roi> <slice-index> <subject-id= text> <scan-id= text>
#Description  Command to measure the Euclidean and plane distance
#       between two points in a landmark set. The output is
#       written to b2_saved_distance as well as an alternate
#       distance file if specified with 'b2 file' command.
#<roiID>   An identifier for an roi
#<slice-index>  Slice index for the perimeter measurement. (-1=All slices)
#Return  A lists of lists containing the measurement label and
#       distance. If an error occurs,
#       then the error status of -1 is returned.
b2_proc_generator b2_measure_roi_perimeter { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert mask to GTSurface-gm-mask <radius> <image> <mask> <surface-1> ...  <surface-N>
#Description  This is one of the ways to find a cortical
#       ribbon-of-interest, in this case based on a set of
#       GTSurfaces, a selection mask, AND most importantly,
#       a tissue-classified segmentation image to define
#       the gray matter.
#<radius>  Normally 5 (voxels).
#<imageID>  An identifier for an image
#<maskID>  An identifier for a mask
#<surface-i>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_mask_to_GTSurface-gm-mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert mask-set to code-image  <mask-list>
#Description  (for cortical parcellation v. II)
#       Creates a code image fit for nearest-neighbor warping.
#       See Also: 'b2 convert code-image to mask-set'.
#<mask-list>  A disjoint set of masks.  Voxel labeling conflicts
#       will be resolved (somewhat) arbitrarily:  each list
#       member will over-write the record of its predecessors
#       should they share voxels.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_mask-set_to_code-image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert code-image to mask-set <image>  <mask-name-list>
#Description  (for cortical parcellation v. II)
#       Decodes a (warped,  nearest-neighbor-resampled) atlas
#       mask partition image.
#       See Also: 'b2 convert mask-set to code-image'.
#<imageID>  An identifier for an image
#<mask-name-list>  Tcl list of names to give the code-image regions.
#       If you give 'standard', you get the FreeSurfer Simple
#       atlas names.  The list's elements indexed 0 through N-1
#       are assigned to image codes 1 through N, respectively.
#Return  Returns a list of disjoint masks in sublist pairs beginning with
#       the name in question.  If the method 'fails', an error
#       status of -1 is returned.
b2_proc_generator b2_convert_code-image_to_mask-set { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert class-image to discrete  <imageID> <mask= mask>
#Description  Converts a continuous tissue classified image into a
#       discrete tissue classified image. The resulting image
#       contains tissue codes from 0-9 (blood, other, etc),
#       10 (CSF), 130 (GM), 250 (WM). The mask defines the
#       subcortical region where the alternate threshold
#       between GM and WM should be applied. If no mask is
#       specified then 190 is used for the entire image.
#<imageID>  An identifier for an image
#<mask= mask>  The mask used to define the subcortical region.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_class-image_to_discrete { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert landmark to talairach-landmark <landmarksetID> <talairach-parameters>
#Description  Changes landmark point set into talairach atlas
#       coordinates. Each talairach voxel is 1 mm.
#<landmarksetID>  An identifier for a landmark set
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_landmark_to_talairach-landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert landmarks to bookstein-transform <from-landmark>  <to-landmark>
#Description  Creates a thin-plate spline warp transform based on
#       name-correlated from and to points.  An inverse warp
#       may be created by swapping from and to.
#<from-landmark>
#<to-landmark>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_landmarks_to_bookstein-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert location to talairach <location-list>  <talairach-parameters>
#Description  Change points in list to Talairach atlas coordinates.
#       Each voxel is 1 mm.
#       See Also: 'b2 convert talairach-mm to location'.
#<location-list>
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_location_to_talairach-mm { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert mask to roi <plane>  <mask> <min= min-slice> <max= max-slice> <skip= slice-skip> <skip-phase= periodicity> <minimum-overlap= minimum-relative-overlap> <name= roi-name>
#Description  Converts a mask that a measurable representation of
#       a region into an ROI that can be edited by the user.
#       The ROI can be generated in the plane specified by
#       the user as well as a slice gap between ROI segments.
#<plane>  The plane of the resulting ROI. Valid values are:
#       x | y | z | Axial | Coronal | Sagittal
#<maskID>  An identifier for a mask
#<min= min-slice>  The minimum slice location to generate an ROI
#       representation of the mask.
#<max= max-slice>  The maximum slice location to generate an ROI
#       representation of the mask.
#<skip= slice-skip>  The maximum number of slices to skip between
#       ROI segments.  (Eg, 2 means consider omitting every other one.)
#<skip-phase= periodicity>  If provided, requires that at least those
#       slices numbered congruent to periodicity modulo slice-skip
#       will be included in the thinned roi.
#<minimum-overlap= minimum-relative-overlap>  The minimum mismatch
#       between the ends of an ROI segment when skipping slices.
#<name= roi-name>  The name of the resulting ROI.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_mask_to_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert mask to talairach-mask <mask>  <talairach-parameters>
#Description  Change mask to Talairach atlas coordinates.
#       Each voxel is 1 mm.
#<maskID>  An identifier for a mask
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_mask_to_talairach-mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert rgb-image to hsi <image>  <saturation-scale (1, -1)> <palette= palette>
#Description  Allows a numerical study of a 3-channel color image
#       in terms of hue, saturation, and intensity.
#<imageID>  An identifier for an image
#<saturation-scale>  1 | -1
#<palette= palette>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_rgb-image_to_hsi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert roi to mask  <roiID> <interpolate= True|False> <min= min-slice> <max= max-slice> <name= mask-name>
#Description  This commands converts an ROI into a mask that can
#       be measured.
#<roiID>   An identifier for an roi
#<interpolate= True|False>  Specify False to turn off filling in voxels to compensate
#       for skipped tracing planes.
#<min= min-slice>  The minimum slice for the conversion of the ROI into
#       mask. All ROI segments before this slice are ignored.
#<max= max-slice>  The maximum slice for the conversion of the ROI into
#       mask. All ROI segments before this slice are ignored.
#<name= mask-name>  The name of the resulting mask.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_roi_to_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach to location <location-list>  <talairach-parameters>
#Description  Change points in list from Talairach atlas coordinates
#       to acquisition coordinates.
#       See Also: 'b2 convert location to talairach-mm'.
#<location-list>
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-mm_to_location { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-box to mask <talairach-box> <talairach-parameters> <expand= expand-flag>
#Description  One step in using talairach-box measurements.
#       See Also: 'b2 convert talairach-mask to mask'.
#<talairach-box>
#<talairach-parameters>
#<expand= expand-flag>  yes | no
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-box_to_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-landmark to landmark <landmarksetID>  <talairach-parameters>
#Description  Change points in landmark set from Talairach atlas
#       coordinates to acquisition coordinates.
#<landmarksetID>  An identifier for a landmark set
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-landmark_to_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-mask to mask <mask>  <talairach-parameters>
#Description  Change mask from Talairach atlas coordinates to
#       acquisition coordinates.
#<maskID>  An identifier for a mask
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-mask_to_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-parameters to acquisition-transform  <talairach-parameters>
#Description  Creates the transform for converting an image from
#       talairach.  For use in stringing together transforms
#       with 'b2 compose transforms'.  See Also: 'b2 convert
#       talairach-parameters to acquisition-transform'.
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-parameters_to_acquisition-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-parameters to cerebellum-landmark  <talairach-parameters>
#Description  For use in starting a tracer on a project to pick
#       cerebellar landmarks. The talairach portion of the
#       landmark table (the bounding cube) will be pre-set.
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-parameters_to_cerebellum-landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-parameters to cortical-landmark  <talairach-parameters>
#Description  For use in starting a tracer on a project to pick
#       cortical landmarks. The talairach portion of the
#       landmark table (the bounding cube) will be pre-set.
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-parameters_to_cortical-landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-parameters to montreal-transform  <talairach-parameters>
#Description  Creates the transform for converting an image to
#       a Montreal-style image.  For use in stringing together transforms
#       with 'b2 compose transforms' and then saving the boxed image
#       in '.raw' format.
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-parameters_to_montreal-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert talairach-parameters to talairach-transform  <talairach-parameters>
#Description  Creates the transform for converting an image to
#       talairach.  For use in stringing together transforms
#       with 'b2 compose transforms'. See Also: 'b2 convert
#       talairach-parameters to acquisition-transform'.
#<talairach-parameters>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_talairach-parameters_to_talairach-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 convert GTSurface-set to code-image  <maskID> <labelFamilyNumber> <GTSurface-list>
#Description  This is one of the ways to find a cortical
#       ribbon-of-interest, in this case based on a set of
#       GTSurfaces, a selection mask, AND most importantly,
#       a parcellation label family to define the set of regions.
#       See Also: 'b2 convert code-image to mask-set'.
#<maskID>  An identifier for a mask
#<labelFamilyNumber>  Usually 0 since there is only one label family possible
#<GTSurface-list>  A set of GTSurfaces.  Voxel labeling will be
#       based on the FreeSurfer-generated vertex labeling.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_convert_GTSurface-set_to_code-image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 nudge affine-transform <transform> <rx= F32> <ry= F32> <rz= F32> <dx= F32> <dy= F32> <dz= F32>
#Description  This command will nudge a resampling transform
#        and return a nudged copy without changing the original.
#        You are supposed to set the resulting nudged transform on the
#        reslice image to view the effect of each successive nudge.
#<transform>  The affine transform to modify, probably created by
#        'b2 create affine-transform'.
#<rx= F32>  A rotation about the x axis, pitch, given in radians.
#<ry= F32>  A rotation about the y axis, yaw, given in radians.
#<rz= F32>  A rotation about the z axis, roll, given in radians.
#<dx= F32>  A translation along the x axis, lateral, given in mm.
#<dy= F32>  A translation along the y axis, inferior-superior, given in mm.
#<dz= F32>  A translation along the z axis, rostro-caudal, given in mm.
#Hint:      You can provide rotation nudges in degrees not radians with
#        set inRadians <expr 3.14159 / 180>
#        b2 nudge affine-transform $tx rx= <expr 5 * $inRadians> . . . etc.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_nudge_affine-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 invert affine-transform <transform>
#Description  This command will invert an affine resampling transform
#        and return the inverse transform without changing the original.
#        You are supposed to set the resulting inverse transform on the
#        reslice image.
#<transform>  The affine transform to invert, probably created by
#        'b2 create affine-transform'.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_invert_affine-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 decompose affine-transform <transform>
#Description  This command will decompose an affine resampling transform
#        into its separate numeric parameters.
#<transform>  The affine transform to decompose.
b2_proc_generator b2_decompose_affine-transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy everything
#Description  Remove all objects currently defined in BRAINS2 from
#       memory. This effectively resets the object table.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_everything { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy every <object-type | viewer>
#Description  Deletes from the BRAINS2 program all of the currently
#       loaded and created objects of the specified type.
#       Object Types: blob| image | roi | mask | landmark |
#         talairach-parameters | net | surface |
#         talairach-box | histogram | transform |
#         loop-matcher | palette | table | blob | GTSurface| viewer
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_every { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy blob  <blobID>
#Description  Removes the specified Blob object from the
#       current BRAINS2 session.
#<blobID>  An identifier for a blob object
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_blob { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy histogram  <histogramID>
#Description  Removes the specified Histogram object from the
#       current BRAINS2 session.
#<histogramID>  An identifier for a histogram object
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_histogram { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy image  <image>
#Description  Removes the specified image object from the current
#       BRAINS2 session.
#<imageID>  An identifier for an image
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_image { { volumeNode - required } } {
  set statusvalue [ catch { $volumeNode Delete  } returnvalue ];
  if { $statusvalue } then {
    puts "b2_destroy_image failed";
    return -1;
  }
  puts "Removed ${volumeNode} with message $returnvalue"
  return 0;
}

#Usage  b2 destroy landmark  <landmarkID>
#Description  Removes the specified landmark object from the current
#       BRAINS2 session.
#<landmarkID>  An identifier for a landmark object
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy mask  <mask>
#Description  Removes the specified mask object from the current
#       BRAINS2 session.
#<maskID>  An identifier for a mask
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_mask { { volumeNode - required } } {
  return [b2_destroy_image ${volumeNode} ];
}

#Usage  b2 destroy palette  <paletteID>
#Description  Removes the specified palette object from the current
#       BRAINS2 session.
#<paletteID>  An identifier for a palette
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_palette { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy roi  <roi>
#Description  Removes the specified ROI object from the current
#       BRAINS2 session.
#<roiID>   An identifier for an roi
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy GTSurface  <GTSurface>
#Description  Removes the specified GTSurface object from the current
#       BRAINS2 session.
#<GTSurfaceID>  An identifier for a GTSurface
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy table  <tableID>
#Description  Removes the specified table object from the current
#       BRAINS2 session.
#<tableID>  An identifier for a table
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_table { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy talairach-parameters <talairach-parametersID>
#Description  Removes the specified talairach-parameters object
#       from the current BRAINS2 session.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy talairach-box  <talairach-boxID>
#Description  Removes the specified talairach-box object from the
#       current BRAINS2 session.
#<talairach-boxID>  An identifier for a talairach-box object
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_talairach-box { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy tissue-class  <tissue-classID>
#Description  Removes the specified tissue-class object from the
#       current BRAINS2 session.
#<tissue-classID>  An identifier for a tissue-class object
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_tissue-class { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 destroy transform  <transformID>
#Description  Removes the specified transform object from the
#       current BRAINS2 session.
#<transformID>  An identifier for a transform
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_destroy_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get image value <imageID> <X> <Y> <Z> <millimeter-flag= true|false> <time-index= index>
#Description  Command to interpolate an intensity for the image in
#       the region defined by the given X, Y, and Z coordinates.
#<imageID>  An identifier for an image
#<X>  X-Y-Z coordinates to sample an interpolation in the space of the image.
#<Y>
#<Z>
#<millimeter-flag= >  Flag signaling if the user is supplying
#       a position in millimeters rather than voxels.
#<time-index= >  Time index into a 4D dataset. By default the zero
#       index time point is used
#Return  A list of lists containing the measurement
#       results.  The sublists contain the result description
#       and measurement value.  If an error occurs, then the
#       error status of -1 is returned
b2_proc_generator b2_get_image_value { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get location
#Description  Returns the present location of the cross-hairs in a list
#Return  Upon successful completion, the current cross hair location is returned in
#       a list, otherwise an error status of -1 is returned.
b2_proc_generator b2_get_location { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get mask color  <maskID>
#Description  This command returns the color of a mask in a list.
#       This command is only supported if BRAINS2 is in
#       graphical mode.
#<maskID>  An identifier for a mask
#Return  Upon successful completion, the mask color is returned
#       in a list containing the red, green, blue, and
#       opacity values for the mask. If an error occurs, then
#       the error status of -1 is returned.
b2_proc_generator b2_get_mask_color { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get roi color  <roiID>
#Description  This command returns the color of an ROI in a list.
#       This command is only supported if BRAINS2 is in
#       graphical mode.
#<roiID>   An identifier for an roi
#Return  Upon successful completion, the ROI color is returned
#       in a list containing the red, green, and blue values
#       for the mask. If an error occurs, then the error
#       status of -1 is returned.
b2_proc_generator b2_get_roi_color { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get palette data  <paletteID>
#Description  This command returns the palette lookup table in a
#       list of lists. Three lists are returned representing
#       the lookup tables for red, green and blue.
#<paletteID>  An identifier for a palette
#Return  Upon successful completion, the palette lookup table
#       is returned in a list of lists. If an error occurs,
#       then the error status of -1 is returned.
b2_proc_generator b2_get_palette_data { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get palette type <paletteID>
#Description  This command returns the palette type. This can be
#       RGB or Index.
#<paletteID>  An identifier for a palette
#Return  Upon successful completion, palette type is returned
#       (either RGB or Index).  If an error occurs, then the
#       error status of -1 is returned.
b2_proc_generator b2_get_palette_type { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get res GTSurface <GTSurfaceID>
#Description  This command returns the resolution of the GTSurface object in a
#       list. These represent the voxel size in each dimension.
#<GTSurfaceID>  An identifier for a GTSurface
#Return  Upon successful completion, GTSurface object resolutions are
#       returned in a list. The length of the list corresponds
#       to the number of dimensions in the GTSurface object.  If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_res_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get dims GTSurface  <GTSurfaceID>
#Description  This command returns the dimension of a GTSurface
#       object (i.e. size in voxels) in a list. The length of the list is
#       the number of dimensions in the GTSurface.
#<GTSurfaceID>  An identifier for a GTSurface
#Return  Upon successful completion, a list of dimensions are
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_dims_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}


## A procedure to remove trailing ones from list
proc remove_trailing_ones_from_list { mylist } {
  for { set idx [ expr [ llength $mylist ] -1 ] } { $idx > 0 } { incr idx -1 } {
    if { [ lindex $mylist $idx ] > 1 } {
      break;
    }
  }
  return [ lrange $mylist 0 $idx ];
}

#Usage  b2 get dims image  <imageID>
#Description  This command returns the dimension of an image
#       (i.e. size in voxels) in a list. The length of the
#       list is the number of dimensions in the image.
#<imageID>  An identifier for an image
#Return  Upon successful completion, a list of dimensions are
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_dims_image { { volumeNode - required } } {
  set statusvalue [catch { [ $volumeNode GetImageData ] GetDimensions } dims ];
  if { $statusvalue } then {
    puts "b2_get_dims_image failed";
    return -1;
  }
  return [ remove_trailing_ones_from_list $dims ];
}

#Usage  b2 get dims mask  <maskID>
#Description  This command returns the dimension of an mask
#       (i.e. size in voxels) in a list. The length of the
#       list is the number of dimensions in the mask.
#<maskID>  An identifier for a mask
#Return  Upon successful completion, a list of dimensions are
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_dims_mask { { volumeNode - required } } {
  return [ b2_get_dims_image ${volumeNode} ];
}

#Usage  b2 get dims landmark  <landmarkID>
#Description  This command returns the dimension of a landmark
#       object (i.e. size in voxels) in a list. The length of the list is
#       the number of dimensions in the landmark.
#<landmarkID>  An identifier for a landmark object
#Return  Upon successful completion, a list of dimensions are
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_dims_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get dims roi  <roiID>
#Description  This command returns the dimension of an ROI (i.e.
#       size in voxels) in a list. The length of the list is
#       the number of dimensions in the ROI.
#<roiID>   An identifier for an roi
#Return  Upon successful completion, a list of dimensions are
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_dims_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get dims talairach-parameters  <talairach-parametersID>
#Description  This command returns the dimension of a talairach-parameters
#       object (i.e. size in voxels) in a list. The length of the list is
#       the number of dimensions in the talairach-parameters.
#       Note:  this is the same as 'b2 get reslice-dims talairach-parameters'.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, a list of dimensions are
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_dims_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get histogram data  <histogram>
#Description  This command returns the histogram data in a list of
#       lists. Each sublist contains the minimum value,
#       maximum value, and the number of counts in a bin.
#       The length of the outer list is the number of bins in
#       the histogram.
#<histogram>  The histogram for which data is being returned.
#Return  Upon successful completion, a list of lists is
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_histogram_data { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get image talairach-parameter  <image>
#Description  This command returns the talairach parameters
#       associated with the specified image.
#<imageID>  An identifier for an image
#Return  Upon successful completion, the talairach parameters
#       are returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_image_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get image dataType  <image>
#Description  This command returns the internal storage type
#       of the image.
#<imageID>  An identifier for an image
#Return  Upon successful completion, a string describing
#       the storage type is returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_image_dataType { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get image type <image>  <channel>
#Description  This command returns the image type description used
#       for the specified channel of an image. This is a
#       simple sort description of what the data represents.
#<image>    : The image object that is being queried
#<channel>  The channel of the image to return a type description
#       for.
#Return  Upon successful completion, the image type is
#       returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_image_type { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get image transform  <image>
#Description  This command returns the transform associated with the
#       specified image.
#<imageID>  An identifier for an image
#Return  Upon successful completion, the associated transform
#       is returned, otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_get_image_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get landmark count  <landmarksetID>
#Description  This command returns the number of landmarks are
#       defined in the specifed landmark group.
#<landmarksetID>  An identifier for a landmark set
#Return  Upon successful completion, the number of points in
#       the landmark group is returned, otherwise and error
#       status of -1 is returned.
b2_proc_generator b2_get_landmark_count { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get landmark location <landmarksetID>  <index | -1 = all>
#Description  This command returns the location of the specified
#       landmark point, or all points if specified. The result
#       is a list of points or a list of lists containing the
#       point locations. The resulting sublists contain the
#       x, y, z locations of the point.
#<landmarksetID>  An identifier for a landmark set
#<index>  Index of the point location to return,  -1 = all points
#Return  Upon successful completion, the point location(s) are returned in a list
#       of lists, otherwise an error status of -1 is returned.
b2_proc_generator b2_get_landmark_location { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get landmark names  <landmarksetID>
#Description  This command returns the landmark names in a group in a list.
#<landmarksetID>  An identifier for a landmark set
#Return  Upon successful completion, the landmark names are
#       returned in a list, otherwise an error status of -1
#       is returned.
b2_proc_generator b2_get_landmark_names { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get name <object-type> <object>
#Description  This command returns the name of the specified object.
#<object-type>  The type of object being queried. Valid object types
#       are:  image | roi | mask | surface | landmark |
#         transform | talairach-parameters |
#         talairach-box | histogram | palette | table |
#         tissue-class
#<object>  The object being queried for its name.
#Return  Upon successful completion, the object name is
#       returned. If an error occurs, then the error status of
#       -1 is returned.
b2_proc_generator b2_get_name { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get patient-id <object-type> <object>
#Description  This command returns the patient ID of the specified
#       object.
#<object-type>  The type of object being queried. Valid object types
#       are:  image | roi | mask | surface | landmark |
#         transform | talairach-parameters |
#         talairach-box | histogram | palette | table |
#         tissue-class
#<object>  The object being queried for its patient ID.
#Return  Upon successful completion, the patient ID is
#       returned. If an error occurs, then the error status
#       of -1 is returned.
b2_proc_generator b2_get_patient-id { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get res image  <imageID>
#Description  This command returns the resolution of the image in
#       a list. These represent the voxel size in each
#       dimension.
#<imageID>  An identifier for an image
#Return  Upon successful completion, image resolutions are
#       returned in a list. The length of the list corresponds
#       to the number of dimensions in the image.  If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_res_image { { volumeNode - required } } {
  set statusvalue [ catch { $volumeNode GetSpacing  } res ];
  if { $statusvalue } then {
    puts "b2_get_res_image Failed";
    return -1;
  }
  #Need to get the real dims (i.e. trailing dimensions of 1 don't count) so that only that many resolutions are reported.
  set tempdims [b2_get_dims_image ${volumeNode} ]
  set trunc_res [ lrange $res 0 [ expr [ llength $tempdims ] - 1 ] ];
  return ${trunc_res};
}

#Usage  b2 get res mask  <maskID>
#Description  This command returns the resolution of the mask in a list. These
#       represent the voxel size in each dimension.
#<maskID>  An identifier for a mask
#Return  Upon successful completion, mask resolutions are returned in a list. The
#       length of the list corresponds to the number of dimensions in the mask.
#       If an error occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_res_mask { { volumeNode - required } } {
  return [ b2_get_res_image ${volumeNode} ];
}

#Usage  b2 get res roi  <roiID>
#Description  This command returns the resolution of the ROI in a
#       list. These represent the voxel size in each dimension.
#<roiID>   An identifier for an roi
#Return  Upon successful completion, ROI resolutions are
#       returned in a list. The length of the list corresponds
#       to the number of dimensions in the ROI.  If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_res_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get res landmark <landmarkID>
#Description  This command returns the resolution of the landmark object in a
#       list. These represent the voxel size in each dimension.
#<landmarkID>  An identifier for a landmark object
#Return  Upon successful completion, landmark object resolutions are
#       returned in a list. The length of the list corresponds
#       to the number of dimensions in the landmark object.  If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_res_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get res talairach-parameters  <talairach-parametersID>
#Description  This command returns the resolution of the talairach-parameters
#       object in a list. These represent the voxel size in each dimension.
#       Note:  this is the same as 'b2 get reslice-res talairach-parameters'.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, talairach-parameters resolutions are
#       returned in a list. The length of the list corresponds
#       to the number of dimensions in the talairach-parameters.  If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_res_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get reslice-dims talairach-parameters  <talairach-parametersID>
#Description  This command returns the dimensions corresponding to
#       the Talairach parameters before the Talairach
#       transform is applied.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, the dimensions of the
#       space corresponding to the Talairach Parameters before
#       application of the Talairach transform are returned in
#       a list. If an error occurs, then the error status of
#       -1 is returned.
b2_proc_generator b2_get_reslice-dims_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get reslice-dims transform  <transformID>
#Description  This command returns the dimensions corresponding to
#       the Transform before the transform is applied.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, the dimensions of the
#       space corresponding to the Transform before
#       application are returned in a list. If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_reslice-dims_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get reslice-res talairach-parameters  <talairach-parametersID>
#Description  This command returns the resolutions corresponding to
#       the Talairach parameters before the Talairach
#       transform is applied.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, the resolutions of the
#       space corresponding to the Talairach Parameters before
#       application of the Talairach transform are returned in
#       a list. If an error occurs, then the error status of
#       -1 is returned.
b2_proc_generator b2_get_reslice-res_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get reslice-res transform  <transformID>
#Description  This command returns the resolution corresponding to
#       the Transform parameters before the transform is
#       applied.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, the resolution of the
#       space corresponding to the Transform before
#       application are returned in a list. If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_reslice-res_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get scan-id <object-type> <object>
#Description  This command returns the scan ID of the specified
#       object.
#<object-type>  The type of object being queried. Valid object types
#       are:   image | roi | mask | surface | landmark |
#        transform | talairach-parameters |
#        talairach-box | histogram | palette | table |
#        tissue-class
#<object>  The object being queried for its scan ID.
#Return  Upon successful completion, the scan ID is returned.
#       If an error occurs, then the error status of -1 is
#       returned.
b2_proc_generator b2_get_scan-id { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get standard-dims talairach-parameters  <talairach-parametersID>
#Description  This command returns the dimensions corresponding to
#       the Talairach parameters after the Talairach transform
#       is applied.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, the dimensions of the
#       space corresponding to the Talairach Parameters after
#       application of the Talairach transform are returned
#       in a list. If an error occurs, then the error status
#       of -1 is returned.
b2_proc_generator b2_get_standard-dims_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get standard-dims transform  <transformID>
#Description  This command returns the dimensions corresponding to
#       the Transform after the transform is applied.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, the dimensions of the
#       space corresponding to the Transform after application
#       are returned in a list. If an error occurs, then the
#       error status of -1 is returned.
b2_proc_generator b2_get_standard-dims_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get standard-res talairach-parameters  <talairach-parametersID>
#Description  This command returns the resolutions corresponding to
#       the Talairach parameters after the Talairach transform
#       is applied.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, the dimensions of the
#       space corresponding to the Talairach Parameters after
#       application of the Talairach transform are returned in
#       a list. If an error occurs, then the error status of
#       -1 is returned.
b2_proc_generator b2_get_standard-res_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get standard-res transform  <transformID>
#Description  This command returns the resolution corresponding to
#       the Transform parameters after the transform is
#       applied.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, the resolution of the
#       space corresponding to the Transform after application
#       are returned in a list. If an error occurs, then the
#       error status of -1 is returned.
b2_proc_generator b2_get_standard-res_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get table data  <tableID>
#Description  This commands returns the data contained within a
#       table in a lists of lists. The number of lists
#       corresponds to the number of rows in the table and
#       the number of elements in each sublist corresponds
#       to the number of columns.
#<tableID>  An identifier for a table
#Return  Upon successful completion, the table data is
#       returned in a list of lists.  If an error occurs,
#       then the error status of -1 is returned.
b2_proc_generator b2_get_table_data { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get talairach points  <talairach-parametersID>
#Description  This command returns the location of the Talairach
#       points (AP, PC, SLA, IRP) in a list of lists. Each
#       sublist contains the x, y, and z locations of a point.
#       The order of the resulting points are AP, PC, SLA,
#       and IRP.
#<talairach-parametersID>  An identifier for a talairach-parameter object
#Return  Upon successful completion, the point locations are
#       returned in a list of lists. If an error occurs, then
#       the error status of -1 is returned.
b2_proc_generator b2_get_talairach_points { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get threshold <object-type> <object>
#Description  This command returns the threshold associated with
#       the specified object.
#<object-type>  The type of object being queried. Valid types are:
#         mask.
#<object>  The object being queried.
#Return  Upon successful completion, the threshold for the
#       specified object is returned.  If an error occurs,
#       then the error status of -1 is returned.
b2_proc_generator b2_get_threshold { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get tissue-class kappa  <tissue-classID>
#Description  This command returns the kappa values in the tissue
#       class model. These report the ability of the tissue
#       class model to appropriately classify the tissue plugs
#       used to generate the model.
#<tissue-classID>  An identifier for a tissue-class object
#Return  Upon successful completion, the kappa values are
#       returned in a list of lists.  Each sublist contains
#       the tissue type and kappa value. If an error occurs,
#       then the error status of -1 is returned.
b2_proc_generator b2_get_tissue-class_kappa { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get transform type  <transform>
#Description  This command returns the type of transform that the
#       transform object represents.  Valid types are:
#         Affine, Interleave, Bookstein, Talairach.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, type of transform is
#       returned as a string.  If an error occurs, then the
#       error status of -1 is returned.
b2_proc_generator b2_get_transform_type { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get transform affine-matrix  <transform>
#Description  This command returns the affine matrix that the
#       transform object represents.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, a list of 16 floats is
#       returned as a list.  If an error occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_transform_affine-matrix { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 get transform composite-affine-matrix  <transform>
#Description  This command returns the effective affine matrix that the
#       transform object represents, including its chain of previous affine transforms.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, a list of 16 floats is
#       returned as a list.  If an error occurs, then the error status of -1 is returned.
b2_proc_generator b2_get_transform_composite-affine-matrix { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set debug-level <debug-level>
#Description  This command sets and/or returns the debug printing
#       level used be BRAINS2.  If the debug-level is
#       specified the debug level is changed, otherwise the
#       current debug-level is simply returned.
#<debug-level>  The debug printing level used by the BRAINS2. This
#       can be one of:
#         silent | progress | tracing | critical | consistent
#Returns    : Returns the debug level of the program. If an error
#       occurs then -1 is returned
b2_proc_generator b2_set_debug-level { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set filename <object-type> <object> <filename>
#Description  This command sets the file name for the specified
#       object.
#<object-type>  The object type that is to have its filename changed.
#       Types: image | roi | mask | surface | landmark |
#         talairach-parameters | talairach-box | histogram |
#         transform | palette | table
#<object>  The object to have its name changed.
#<filename>  The filename for the specified object.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_filename { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set image palette  <image> <palette> <opacity>>
#Description  Sets the palette for the specified image via
#       the command line. If no palette is specified
#       then the current image palette is returned.
#<imageID>  An identifier for an image
#<palette>  The palette to be used by the specified image.
#<opacity>  The opacity for the specified image.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_set_image_palette { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set image res  <image> <xRes ...>
#Description  Sets the resolutions for the specified image via
#       the command line. Up to 4 coordinates allowed.
#<imageID>  An identifier for an image
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_set_image_res { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set interpolation <mode>
#Description  This command sets the interpolation mode for BRAINS2.
#       If then mode is not specified then the current
#       interpolation mode is returned.
#<mode>  The interpolation mode for BRAINS2. Valid modes are
#           trilinear | nearest-neighbor
#Returns    : The interpolation mode of BRAINS2. If an error occurs
#       then -1 is returned.
b2_proc_generator b2_set_interpolation { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set numeric acpc <acx> <acy> <acz> <pcx> <pcy> <pcz>
#Description  This commands sets the location of the AC and PC
#       by numeric arguments in the resampleMode.
#<acx>  The x location of the AC point.
#<acy>  The y location of the AC point.
#<acz>  The z location of the AC point.
#<pcx>  The x location of the PC point.
#<pcy>  The y location of the PC point.
#<pcz>  The z location of the PC point.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_numeric_acpc { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set location <x> <y> <z>
#Description  This commands sets the location of the cross-hairs in
#       the image viewers.
#<x>  The x location of the cross-hairs.
#<y>  The y location of the cross-hairs.
#<z>  The z location of the cross-hairs.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_location { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set name <object-type> <object> <name>
#Description  This commands changes the name of the specified
#       object.
#<object-type>  The object type in which the name is being modified.
#       Valid types are:
#          image | roi | mask | surface | transform
#<object>  The object having its name changed.
#<name>  The new name for the specified object.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_name { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set pixmap-directory  <pathname>
#Description  This command specifies the location for BRAINS2 to
#       look for the default pixmaps used for icons.
#<pathname>  Pathname for the directory containing the default
#       pixmaps.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_pixmap-directory { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set standard-size <voxel-res>
#Description  This command specifies the standard pixel size to
#       govern resampling when picking AC-PC points.
#<voxel-res>  Resolution of a voxel in the resampled image.
#Returns    : Returns the standard voxel size for image resampling.
#       If an error occurs then -1 is returned.
b2_proc_generator b2_set_standard-size { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set standard-size <xsize><ysize><xzize>
#Description  This command specifies the standard dimension sizes to
#       govern resampling when picking AC-PC points.
#<xsize>  Size of the resampled image in the x direction
#<ysize>  Size of the resampled image in the y direction
#<zsize>  Size of the resampled image in the z direction
#Returns    : Returns the standard dimensions for image resampling.
#       If an error occurs then -1 is returned.
b2_proc_generator b2_set_standard-dimensions { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set threshold <object-type> <index> <threshold>
#Description  This command sets the object threshold value. This is
#       used since the threshold value does not follow through
#       mask boolean operations where threshold values may
#       differ. This allows representing this information in
#       the resulting mask.
#<object-type>  The object type to set the threshold value. Valid
#       types are: mask
#<object>  The object having its threshold value set.
#<threshold>  The threshold value to be set for the object.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_threshold { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set transform <transform> <object-type> <object>
#Description  Sets the transform associated with the specified
#       object. Currently this allows for interactive
#       resamplign of images. If the transform is specified
#       as -1 then the current transform applied to the image
#       is removed.
#<transform>  Transform to apply to the image. -1 removes the
#       current transform applied to the image.
#<object-type>  Object type the transform is being applied to. Valid
#       object-types are:  image
#<object>  The object having the transform applied to it.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_transform { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set talairach-parameters <talairach-parameters> <object-type> <object>
#Description  Sets the talairach parameters associated with an
#       object. This allows the object to be referenced in
#       terms of Talairach coordinates.
#<talairach-parameters>  Talairach parameters used to define the Talairach
#       coordinate system.
#<object-type>  Object type in which the Talairach parameters are
#       associated with.  Valid object-types are:  image
#<object>  Object to have Talairach parameters associated with.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 set time-graph overlay <table>
#Description  Sets an overlap plot for the time graph dialog box.
#       Typically this will represent the estimated
#       hemodynamic response.
#<table>  Table to be used as an overlay plot for the time
#       graph.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_set_time-graph_overlay { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 window geometry  <window-name> <geometry= geometry-field>
#Description  Command to get and/or set the geometry and location
#       of the specified window.  If the geometry-field is
#       given, then the window will be resized.  If the
#       geometry field is not given then the current geometry
#       is returned to the user.
#<window-name>  An identifier specifying the window to change
#       geometry on.   Current valid values are
#         axi | cor | sag | surf
#<geometry= >  The string identifying the new location of the
#         window.  Eg. 'b2 window geometry axi
#         geometry= 300x300+290+45' will create a window
#         that is 300x300 pixels with the lower left
#         corner located at screen location (290,45).
#Return  A string containing the current window geometry.
#       Otherwise an error status of -1 is returned.
b2_proc_generator b2_window_geometry { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 inscribe-curvature-planes <GTSurface> <float-list> <mask-list>
#Description  (for the Iowa FreeSurfer atlas reference planes.)
#       Inscribe Jae Kim's reference planes into the FreeSurfer curvature tables.
#
#<GTSurface>  A FreeSurfer-derived GTSurface object to copy.
#<float-list>  One zPlane coordinate at which to inscribe a reference plane.
#       We only apply the plane within the corresponding mask.
#<mask-list>  One mask per zPlane coordinate in the accompanying list.
#       We only apply the plane within the corresponding mask.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_inscribe-curvature-planes { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 overwrite transform affine-matrix  <transform> <list ... 16 floats>
#Description  This command modifies the head affine matrix that the
#       transform object represents.
#<transformID>  An identifier for a transform
#Return  Upon successful completion, returns 0.
#       If an error occurs, then the error status of -1 is returned.
b2_proc_generator b2_overwrite_transform_affine-matrix { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 autocorrelation-lambda-resels <diff-image-list> <talairach-parameters-list> <mean-diff-image> <cardinality-image> <pooled-Sbar-squared> <maximum-T> <minimum-T> <data-type= type>
#Description  This command takes a series of difference PET images,
#       and will generate the autocorrelation Lambda matrix, Resel size,
#       and average TalairachParameters.
#<diff-image-list>  A list of PET difference images used for computing voxel-wise statistics.
#       They must all be the same dims. The length of the list must be
#       greater than 1 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<talairach-parameters-list>  A list of talairach parameters objects corresponding to the PET images.
#       They must all be the same dims. The length of the list must be
#       greater than 1 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<mean-diff-image>  A mean image produced by 'b2 stat images'.
#       The images in the sample must all have been differences of
#       PET images co-registered to Montreal space.
#<cardinality-image>  A number of supplied differences image that can be
#       produced by 'b2 sum masks'.
#<pooled-Sbar-squared>  A value for PooledSbarSquared
#       produced by 'b2 pooled-stddev-conversion'.
#<maximum-T>  A value for the corrected T image maximum
#       produced by 'b2 measure image mask'.
#<minimum-T>  A value for the corrected T image minimum
#       produced by 'b2 measure image mask'.
#<data-type= >  Specifies the storage type for the resulting
#       maximum image. Valid types are:
#          signed-8bit
#          unsigned-8bit
#          signed-16bit
#          unsigned-16bit
#          signed-32bit
#          unsigned-32bit
#          signed-64bit
#          unsigned-64bit
#          float-single
#          float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_autocorrelation-lambda-resels { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 average image slices <image>  <#-slices>
#Description  This command will average together #-slices
#       from an image and generate a new image with a
#       slice thickness #-slices times that of the
#       original image.
#<imageID>  An identifier for an image
#<#-slices>  Number of slices to average together.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_average_image-slices { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 average landmarks <landmark-1> <landmark-2> ...
#Description  Generate a new landmark set that is the
#       average of the specified landmark set locations
#       landmark name by landmark name.
#<landmark-i>  The landmark sets that are to be averaged
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_average_landmarks { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 clip image <imageID>  <maskID> <lower-bound= Float> <upper-bound= Float> <fill-value= Float>
#Description  This command will generate a new image in
#        that only the portion of the image within the
#        mask is kept. All other portions of the image
#        are set to 0. The resulting image has the same
#        dimensions and resolutions as the original; and
#        if a lower bound and or an upper bound is provided,
#        the remaining image within the mask will not go
#        beyond the permitted bounds.
#<imageID>  An identifier for an image
#<maskID>  An identifier for a mask
#<upper-bound= >  provides an enforceable upper bound.
#<lower-bound= >  provides an enforceable lower bound.
#        The bounds will replace any image values that exceed them.
#<fill-value= >  floating point number  |   NaN  |   BIGNEG.
#        Provides the value to fill the background with (defaults to 0.0).
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_clip_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 clip roi <roi> <mask>
#Description  Clip the ROI to the mask. All strands not intersecting the mask are removed.
#      If the invert option is turned on then all strands that intersect the mask are removed
#<roiID>   An identifier for an roi
#<maskID>  An identifier for a mask
#<invert= on | off >  Invert the logic and either keep or remove the strands that intersect the mask.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_clip_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 compute effect-size image <avg-Image-1> <std-Image-1> <avg-Image-2>  <std-Image-2> <n1= number in group1> <n2= number in group2> <study= study-name> <path= pathname>
#Description
#<avg-Image-1>
#<std-Image-1>
#<avg-Image-2>
#<std-Image-2>
#<n1= number in group1>
#<n2= number in group2>
#<study= study-name>
#<path= pathname>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_compute_effect-size_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 divide images <imageList> <data-type= type>
#Description  This command computes the division of images as
#       image1/.../imageN, and returns the result.
#<imageList>  A list of images used for the division. The
#       length of the list must be greater than 1 and
#       less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting
#       maximum image. Valid types are:
#          signed-8bit
#          unsigned-8bit
#          signed-16bit
#          unsigned-16bit
#          signed-32bit
#          unsigned-32bit
#          signed-64bit
#          unsigned-64bit
#          float-single
#          float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_divide_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 equal images <imageList> <data-type= type>
#Description  This command returns an image equal to their
#       common value wherever all the images in the given list are
#       mutually equal, and equal to zero otherwise,  This can be used
#       to partition space into pieces equal to the maximum of some
#       given images. (It came up for maximum likelihood atlas building
#       with probability clouds.) This is done on a voxel by voxel basis.
#<imageList>  A list of images used for the equal images
#       operation. The length of the list must be
#       greater than 1 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting
#       maximum image. Valid types are:
#          signed-8bit
#          unsigned-8bit
#          signed-16bit
#          unsigned-16bit
#          signed-32bit
#          unsigned-32bit
#          signed-64bit
#          unsigned-64bit
#          float-single
#          float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_equal_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 extrude-acquisition-plane-clipping image <imageID> <data-type= type>
#Description  This command generates a resampled image for which the legacy
#       'PET chimney' is for some reason called for.  It is computed in terms of
#       Float-single voxels, and stored in the given datatype.
#       CAVEAT:  You MUST give an image that has a transform set.  A typical
#       transform chain would be the montreal boxing transform composed with
#       the PET-to-MR-ACPC registration transform.
#       This method is part of a piecewise, transparent replacement script for
#       'b2 resample worsley-PET ... brains-interp= True'
#<imageID>  The filtered PET image WITH a transform chain set.
#<data-type= >  Specifies the storage type for the resulting image.
#       The default is Float-single. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_extrude-acquisition-plane-clipping_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 find blobs <image> <talairach-parameters>  <threshold> <study= study-name> <task= name> <path= pathname> <mask= mask>
#Description  This command generates a blobs object summarizing
#        areas of significant activation in functional imaging study.
#        These contain the location of the center of mass and
#        the voxel with the largest absolute value.
#     Note:  saving the blobs object to a file is the only way
#        to produce a human-readable text list of the blobs in either
#        of two possible formats, according to filter choice.
#        See 'b2 save blobs'.
#<image>  This is a Worsley-style T image.
#<talairach-parameters>  Talairach parameters for the T image.
#<threshold>  A blob-defining T threshold, e.g., 3.61
#<study= study-name>
#<task= name>
#<path= pathname>
#<mask= mask>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_find_blobs { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 fit image interleaved <image>  <#-interleaves> <skip-anterior= num> <skip-posterior= num>
#Description  This command will coregister 2D slices from a
#       multi pass (acquistion) sequence. It uses a
#       correlation approach to find the optimal
#       translations between slices. This is rounded
#       to avoid filtering of the image.
#<imageID>  An identifier for an image
#<#-interleaves>  Number of pass (acquisitions) in the image.
#<skip-anterior= num>  Number of anterior slices to exclude from the
#       analysis.
#<skip-posterior= num>  Number of posterior slices to exclude from the
#       analysis.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_fit_image_interleaved { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hanning-filter2D-pet <image> <filter-size= float> <historical-mode= <1|0>>
#Description  Generates a worsley Hanning-filtered image using
#       the 2D Hanning filter in the z-plane (PET acquisition).
#       This is used for subsequent group imaging studies. This
#       command works with PET images only.
#<imageID>  An identifier for an image
#<filter-size= float>  The size of the Hanning filter (FWHM) in mm.
#<historical-mode= <0|1> >  Defines how to run the hanning filter.  By default
#       1 is assumed and indicates that the filter will be compatible with the
#       original worsley analysis, and includes a possible shifting of the images.
#       If the 0 flag is given, then a centered filter with no spacial movement will
#       be applied.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_hanning-filter2D-pet { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 histogram-equalize image <imageID> <data-type= type>
#Description  This command generates a histogram-equalized image.
#       It is computed in terms of Unsigned-16Bit voxels,
#        and stored in the given datatype.
#<imageID>  The (montreal-transformed) image to equalize the levels of.
#<data-type= >  Specifies the storage type for the resulting image.
#       The default is unsigned-8bit. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_histogram-equalize_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 image min <imageID>
#Description  This command returns the minimum voxel value in the
#       specified image.
#<imageID>  An identifier for an image
#Return  Upon successful completion, the minimum voxel values
#       for erach channel are returned in a list. The
#       length of the list specifies the number of channels
#       in the image. If an error occurs, then the error
#       status of -1 is returned.
b2_proc_generator b2_image_min { { volumeNode - required } } {
#  vtkImageData::GetScalarRange
#   Place body of command here.
  set statusvalue [catch { [ $volumeNode GetImageData ] GetScalarRange } range ];
  if { $statusvalue } then {
    puts "b2_image_min Failed";
    return -1;
  }
  return [lindex ${range} 0 ];
}

#Usage  b2 image max <imageID>
#Description  This command returns the minimum voxel value in the
#       specified image.
#<imageID>  An identifier for an image
#Return  Upon successful completion, the maximum voxel values
#       for erach channel are returned in a list. The
#       length of the list specifies the number of channels
#       in the image. If an error occurs, then the error
#       status of -1 is returned.
b2_proc_generator b2_image_max { { volumeNode - required }  } {
#   Place body of command here.
  set statusvalue [catch { [ $volumeNode GetImageData ] GetScalarRange } range ];
  if { $statusvalue } then {
    puts "b2_image_max Failed";
    return -1;
  }
  return [lindex ${range} 1 ];
}

#Usage  b2 intensity-rescale image <imageID> <old-lower-bound> <old-upper-bound> <new-lower-bound> <new-upper-bound> <no-clipping= true|false> <data-type= type>
#Description  This command will generate a new image in
#        that the dynamic range is rescaled so as to equate the
#        old bounding interval with the new bounding interval.
#        Note that the old interval need not correspond to the
#        actual range of the image.  Overruns will continue to overrun,
#        albeit on the new scale.
#<imageID>  An identifier for an image
#<old-lower-bound>
#<old-upper-bound>
#<new-lower-bound>
#<new-upper-bound>
#<no-clipping= >  Flag to explicitly turn off clipping.
#<data-type= >  Specifies the storage type for the resulting maximum
#       image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_intensity-rescale_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 max images <imageList> <data-type= type>
#Description  This command generates an image containing the maximum
#       value of each voxel across all of the images.
#<imageList>  A list of images used for the max images operation.
#       The length of the list must be greater than 1 and less
#       than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting maximum
#       image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_max_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 min images <imageList> <data-type= type>
#Description  This command generates an image containing the minimum
#       value of each voxel across all of the images.
#<imageList>  A list of images used for the min images operation.
#       The length of the list must be greater than 1 and less
#       than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting minimum
#       image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_min_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 multiply images <imageList> <scale= constant> <data-type= type>
#Description  This command generates an image the product of each
#       voxel across all of the images.  The result is
#       constant * image1 * ... * imageN
#<imageList>  A list of images used for the multiply images
#       operation. The length of the list must be greater
#       than 1 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<scale= >  Specifies a scaling factor to be included in the
#       resulting multiplication.
#<data-type= >  Specifies the storage type for the resulting
#       multiplication image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_multiply_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 pooled-stddev-conversion <variance-diff-image> <cardinality-image> <requests= keywordList> <data-type= type>
#Description  This command generates the PooledSbarSquared for use by
#       'b2 autocorrelation-lambda-resels', and at your option, generates an
#       image to multiply a pointwise-T by to get the Worsley/Montreal T image.
#<variance-diff-image>  A variance image produced by 'b2 stat images'.
#       The images in the sample must all have been differences of
#       PET images co-registered to Montreal space.
#<cardinality-image>  A number of supplied differences image that can be
#       produced by 'b2 sum masks'.
#<requests= keywordList>  A list chosen from the following:
#          PooledStandardDeviationFactor
#          PooledSbarSquared
#   Note!  A simple and correct way to compute a keywordList is
#     <list PooledSbarSquared PooledStandardDeviationFactor>
#<data-type= >  Specifies the storage type for the resulting
#       maximum image. Valid types are:
#          signed-8bit
#          unsigned-8bit
#          signed-16bit
#          unsigned-16bit
#          signed-32bit
#          unsigned-32bit
#          signed-64bit
#          unsigned-64bit
#          float-single
#          float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_pooled-stddev-conversion { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 stat images <imageList> <supplied-masks= maskList>  <requests= keywordList> <data-type= type>
#Description  This command generates pointwise statistics,
#       possibly clipped to optional corresponding masks,
#       and returns a whole image for each request that you name.
#       This is done on a voxel by voxel basis.
#<imageList>  A list of images used for computing voxel-wise statistics.
#       They must all be the same dims and res. The length of the list must be
#       greater than 1 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<supplied-masks= maskList>  A list of masks used for computing voxel-wise statistics.
#       They must all be the same dims and res. The length of the list must be
#       the same as the length of the imageList.
#   Note!  A simple and correct way to compute a maskList is
#     <list $maskVar1 $maskVar2 ...>
#<requests= keywordList>  A list chosen from the following:
#          Minimum
#          Maximum
#          Median
#          Range
#          Cardinality
#          Mean
#          Variance
#          StandardDeviation
#          PairedDifferenceT
#          PairedDifferenceTProbability
#          Skewness
#          Kurtosis
#   Note!  A simple and correct way to compute a keywordList is
#     <list Mean Median ...>
#<data-type= >  Specifies the storage type for the resulting
#       maximum image. Valid types are:
#          signed-8bit
#          unsigned-8bit
#          signed-16bit
#          unsigned-16bit
#          signed-32bit
#          unsigned-32bit
#          signed-64bit
#          unsigned-64bit
#          float-single
#          float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_stat_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 region grow <imageID> <min= threshold> <max= threshold> <seed= location>
#Description  This command will perform region growing starting at
#       the specified location. Based on direction the region
#       growing will be limited above or below the threshold
#       of interrest.
#<imageID>  An identifier for an image
#<min= threshold>  Minimum value used for region inclusion
#<max= threshold>  Maximum value used for region inclusion
#<seed= location-list>  The x, y and z location of the seed used
#       for region growing.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_region_grow { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkBilateral <image> <spatialSigma> <rangeSigma>  <data-type= ...>
#Description  This command will run the itk edge-preserving filter named Bilateral
#       that is a non-iterative approximation to GradientAnisotropicDiffusion.
#       BEWARE:  it takes forever to run, compared to GradientAnisotropicDiffusion!
#
#<imageID>  An identifier for an image
#<spatialSigma>  Represents feature size.
#<rangeSigma>  Represents sensitivity to differences as edges.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkBilateral { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkConnectedThreshold <imageID> <min= threshold> <max= threshold> <seed= location>
#Description  This command will perform region growing starting at
#       the specified location. Based on direction the region
#       growing will be limited above or below the threshold
#       of interrest.
#<imageID>  An identifier for an image
#<min= threshold>  Minimum value used for region inclusion
#<max= threshold>  Maximum value used for region inclusion
#<seed= location-list>  The x, y and z location of the seed used
#       for region growing.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkConnectedThreshold { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkDanielssonDistanceMap <image>  <data-type= ...>
#Description  This command will run the itk filter named DanielssonDistanceMap
#       on a binary image.  To make one, sum a mask.
#
#<imageID>  An identifier for an image
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkDanielssonDistanceMap { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkDiscreteGaussian <image> <gaussianVariance> <maxKernelWidth>  <data-type= ...>
#Description  This command will perform gaussian smoothing.
#
#<imageID>  An identifier for an image
#<gaussianVariance>  Parameter governing filter size
#<maxKernelWidth>  Maximum dimension of smoothing kernel.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkDiscreteGaussian { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkFlipImage <image> <flipX> <flipY> <flipZ>  <data-type= ...>
#Description  This command will run the itk FlipImageFilter
#       so users can reverse the order of slices in X, Y, or Z.
#
#<imageID>  An identifier for an image
#<flipX>  1 = flip in X, 0 = don't.
#<flipY>  1 = flip in Y, 0 = don't.
#<flipZ>  1 = flip in Z, 0 = don't.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkFlipImage { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkGeodesicActiveContour <level-set-image> <speed-edge-image> <propagation-scaling-factor> <curvature-scaling-factor> <advection-scaling-factor>  <data-type= ...>
#Description  This command will run the itk segmentation filter
#       named GeodesicActiveContour.
#
#<imageID>  An identifier for an image
#<propagation-scaling-factor>  um,
#<curvature-scaling-factor>  um,
#<advection-scaling-factor>  um, see itk manual.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkGeodesicActiveContour { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkGradientAnisotropicDiffusion <image>  <iterations= ...> <conductance= ...>  <data-type= ...>
#Description  This command will perform gradient anisotropic
#       diffusion smoothing.
#<imageID>  An identifier for an image
#<iterations= n>  ipl MR6 default is 4.
#<conductance= f>  ipl MR6 default is 0.7
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkGradientAnisotropicDiffusion { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkGradientMagnitude <image>  <data-type= ...>
#Description  This command will run the itk edge filter named GradientMagnitude.
#
#<imageID>  An identifier for an image
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkGradientMagnitude { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkLargestRegionFilledMask <imageID> <LowerThreshold><UpperThreshold= threshold> <ClosingSize= a-small-integer>
#Description  This command will find the largest
#       connected region within the given intensity bounds,
#       fill it, and return the result as a mask.
#<imageID>  An identifier for an image
#<LowerThreshold>  Minimum value used for region inclusion
#<UpperThreshold= threshold>  Maximum value used for region inclusion
#<ClosingSize= a-small-radius>  The largest radius filled in by
#        dilating and eroding.  Default is 7.0 millimeters.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkLargestRegionFilledMask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkMedian <image> <radius>  <data-type= ...>
#Description  This command will run the itk BasicFilter named itkMedianImageFilter
#       that finds neighborhood medians out to a cubic radius in all dimensions.
#
#<imageID>  An identifier for an image
#<radius>  If you have no idea, try 1 or 2.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkMedian { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkPermuteAxes <image> <mapX> <mapY> <mapZ>  <data-type= ...>
#Description  This command will run the itk PermuyteAxesImageFilter
#       so users can reverse the order of the X, Y, or Z coordinates.
#
#<imageID>  An identifier for an image
#<mapX>  0, 1, or 2:  what to map X (0) to.
#<mapY>  0, 1, or 2:  what to map Y (1) to.
#<mapZ>  0, 1, or 2:  what to map Z (2) to.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkPermuteAxes { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkRecursiveGaussian <image> <gaussianSigma>  <data-type= ...>
#Description  This command will perform gaussian smoothing.
#
#<imageID>  An identifier for an image
#<gaussianSigma>  Parameter governing filter size in mm.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkRecursiveGaussian { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 itkSigmoid <image> <alpha> <beta>  <max= ...> <min= ...> <data-type= ...>
#Description  This command will run the itk edge filter named Sigmoid
#       that maps a dynamic range of interest onto <0, 1>.
#
#<imageID>  An identifier for an image
#<max>  Default is 1.0;  a common value is 255.0.
#<min>  Default is 0.0.
#<alpha>  e.g., stddev.  Itk recommends:  (min - avg)/6.0
#<beta>  e.g., mean.  Itk recommends:  (min+avg)/2, see itk manual.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_itkSigmoid { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 resample image  <imageID> <plane= plane> <min= min-Slice> <max= max-Slice> <time= time-index> <storage-type= type> <interp= interpolation> <channel= channel> <BackgroundFillValue= type>
#Description  This command will generate a new image of the
#        specified size, data type and interpolation method.
#        If a transform is applied to the image this will be
#        used to generate the resampled image.
#        Differs from 'b2 create bounded-image' in that you can
#        only crop in one of x, y, or z at a time, and no attempt
#        is made to maintain registration information about the
#        sub-image in the form of a transform, as with 'b2 create
#        bounded-image'.
#<imageID>  An identifier for an image
#<plane= plane>  The plane in which to crop the resulting image between
#        min-Slice and max-Slice. The default is CORONAL.
#<min= min-Slice>  The starting slice for the new image in
#        the specified plane. The default is 0.
#<max= max-Slice>  The ending slice for the new image in the
#        specified plane. The default is the image size.
#<time= time-index>  The time index to be used for the
#        resulting plane. The default is 0.
#<storage-type= type>  The storage type for the resulting
#        image. The default is unsigned 8 bit.
#<channel= channel>  The channel of the image to resample.
#        The default is all channels.
#<interp= interpolation>  The interpolation method used to generate the
#        resulting image. Valid types are:
#            trilinear | nearest
#<channel= channel>  The channel of the image to resample.
#        The default is all channels.
#<BackgroundFillValue= channel>  floating point number  |   NaN  |   BIGNEG.
#        The default is zero.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_resample_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 resample mask <forward | inverse> <mask>  <transform>  <silent-flag= True|False >
#Description  Transforms convert a display position to a data position.
#<forward | inverse>  Using 'forward' will work like 'b2 set transform'.
#<mask>
#<transform>
#<silent-flag= True|False > If True, then suppress warnings about the mask voxels that are being resampled                                     out of bounds.  The default is False.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_resample_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 resample landmark <forward | inverse> <landmark> <transform>
#Description  Generate a new landmark set that is the
#       landmarks with the specified trasnsform applied
#<forward | inverse>  Using 'forward' will work like 'b2 set transform' for images.
#<landmark>  The landmark sets that is being resampled
#<transform>  The transform to be applied to the landmark points
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_resample_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 resample GTSurface <forward | inverse> <GTSurface> <transform>
#Description  Generate a new GTSurface that is the
#       given GTSurface with the specified trasnsform applied
#<forward | inverse>  Using 'forward' will work like 'b2 set transform' for images.
#<GTSurface>  The GTSurface that is being resampled
#<transform>  The transform to be applied to the GTSurface vertex points
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_resample_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 resample ROI <forward | inverse> <ROI> <transform>
#Description  Generate a new ROI that is the
#       ROI with the specified trasnsform applied
#<forward | inverse>  Using 'forward' will work like 'b2 set transform' for images.
#<ROI>  The ROI that is being resampled
#<transform>  The transform to be applied to the ROI points
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_resample_ROI { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 resample talairach-parameters <forward | inverse> <talairach-parameters> <transform>
#Description  Generate a new talairach-parameters object that bounds the
#       same box with the specified trasnsform applied.  This will be incorrect if
#       the specified transform has a rotation component, due to the flaw that twisting
#       the SLA and IRP points will drift in space clipping one side of the enclosed
#       signal and missing the other side.
#<forward | inverse>  Using 'forward' will work like 'b2 set transform' for images.
#<talairach-parameters>  The talairach-parameters object that is being resampled
#<transform>  The transform to be applied to the talairach-parameters object
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_resample_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 subtract from constant <constant> <imageList> <data-type= type>
#Description  This command generates an image the difference of each
#       voxel across all of the images.  The result is
#       constant - image1 - ... - imageN
#<imageList>  A list of images used for the subtraction from
#       constant operation. The length of the list must be
#       greater than 0 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting
#       subtraction image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_subtract_from_constant { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 subtract images <imageList> <data-type= type>
#Description  This command generates an image the difference of each
#       voxel across all of the images.  The result is
#       image1 - ... - imageN
#<imageList>  A list of images used for the subtraction images
#       operation. The length of the list must be greater than
#       1 and less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting
#       subtraction image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_subtract_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 standardize-neighborhood image <image> <mask> <neighborhood-size>
#Description  Filters the image based on only the voxels in the
#       mask using a boxcar filter of the given size.
#       Uses the Heckel differential boxcar optimization.
#<image>  Resampled T1, T2, or PD.
#<mask>  Uniform tissue mask based on thresholding a tissue-
#       class image.
#<neighborhood-size>  3 .. min(image dims) - 1
#       Size of boxcar filter in voxels.  For the human brain
#       this should work out to 20mm.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_standardize-neighborhood_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 sum images <imageList> <data-type= type>
#Description  This command generates an image the sum of each voxel
#       across all of the images.  The result is
#       image1 + ... + imageN
#<imageList>  A list of images used for the sum images operation.
#       The length of the list must be greater than 0 and
#       less than or equal to 256
#   Note!  A simple and correct way to compute an imageList is
#     <list $imageVar1 $imageVar2 ...>
#<data-type= >  Specifies the storage type for the resulting summation
#       image. Valid types are:
#           signed-8bit
#           unsigned-8bit
#           signed-16bit
#           unsigned-16bit
#           signed-32bit
#           unsigned-32bit
#           signed-64bit
#           unsigned-64bit
#           float-single
#           float-double
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_sum_images { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 sum masks <mask-list>
#Description
#<mask-list>  <<mask-1> <mask-2> ... <mask-N>>
#        where N < 256
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_sum_masks { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 threshold image <imageID>  <lowerThreshold> <upperThreshold= Value > <absolute-value= True | False> <UseOpenInterval= True | False>
#Description  This command will generate a mask that defines the
#       portion of the image in the interval between the lower and upper thresholds.
#       By default, the threshold values are inclusive (i.e. lower <= image <= upper ),
#       The absolute-value= flag signals
#       if the absolute value of the image should be taken
#       before comparing the voxel value to the threshold.
#<imageID>  An identifier for an image
#<lowerThreshold>  The lower threshold used to generate the mask.
#<upperThreshold= >  The upper threshold used to generate the mask. It is required
#       that this value is greater than the lower threshold. (+Infinity if not specified)
#<absolute-value= >  Flag to signal if the absolute value of the voxel
#       value should be taken before comparing to the
#       threshold value. The valid flags are: True | False.
#<UseOpenInterval= >   Flag to signal if the threshold interval is inclusive of the
#       end points. For (default) false then it is closed interval (i.e. lower <= image <= upper ),
#       true is open interval (i.e. lower < image < upper).
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_threshold_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 threshold histogram pair <percent> <grey-hist> <csf-hist>
#Description  Command to compute the mean value of each histogram
#       and return the value that divides the mean of the
#       histograms by the specified distance given as a
#       percentage.
#<percent>  How far to erode the brain tissue
#         >> 0.0 -- include all the csf
#         >> 1.0 -- include only pure tissue (gm and wm)
#<grey-hist>  Histogram based on a sample of gray matter measured
#       in a T1 image.
#<csf-hist>  Histogram based on a sample of CSF measured in a T1
#       image.
#Return  Returns a number defining the T1 image threshold
#       to use to 'wash off' partial volume to the given
#       'percent' level.  Otherwise an error status of -1
#       is returned.
b2_proc_generator b2_threshold_histogram_pair { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 trim-effect image <image>  <sigmoid | square> <effect-size= float> <mask= mask>
#Description
#<image>
#<sigmoid | square>
#<effect-size= float>
#<mask= mask>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_trim-effect_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 and masks <mask-1> <mask-2> ...
#Description  Intersect two or more spatial masks to
#       form the resulting mask.
#<mask-i>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_and_masks { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 clean mask  <mask> <size-thresh= 2D-size-threshold <x-loc= point-coord  y-loc= point-coord  z-loc= point-coord>>
#Description  Generates a mask that is the 3D connected
#       region from the original mask. The location
#       used to start the flood fill algorithm is the
#       middle of the image if not specified by the
#       user.
#<maskID>  An identifier for a mask
#<size-thresh= ..>  2D size threshold
#<x-loc= point-coord>  The x location to start the flood fill.
#<y-loc= point-coord>  The y location to start the flood fill.
#<z-loc= point-coord>  The z location to start the flood fill.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_clean_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 dilate mask <mask>  <distance>
#Description  Adds distance voxels from the edge of the
#       mask.
#<maskID>  An identifier for a mask
#<distance>  The number of voxels to add to the edge of
#       the mask
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_dilate_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 erode mask <mask>  <distance>
#Description  Removes distance voxels from the edge of the
#       mask.
#<maskID>  An identifier for a mask
#<distance>  The number of voxels to remove from the edge of
#       the mask
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_erode_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 erode surface mask <mask> <image>  <threshold>
#Description  <GREG>
#<maskID>  An identifier for a mask
#<imageID>  An identifier for an image
#<threshold>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_erode_surface_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 fill brainseg mask  <mask>
#Description  Fills in internal holes in a mask. This
#       is done by converting from a mask to an ROI
#       and then back to a mask in the coronal
#       plane.
#<maskID>  An identifier for a mask
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_fill_brainseg_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 fill internal mask  <mask>
#Description
# Fills in holes in a mask. This is based on old BRAINS code and used for tissue classification measurements
#<maskID>  An identifier for a mask
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_fill_internal_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 not mask  <mask>
#Description  Generates the inverse of the mask.
#<maskID>  An identifier for a mask
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_not_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 or masks <mask-1> <mask-2> ...
#Description  Union two or more spatial masks.
#<mask-i>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_or_masks { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 split mask <mask> <plane> <slice>  <direction>
#Description  Splits a mask in the specified plane. Direction
#       is a keyword for the side of the mask that is to
#       be returned.
#<maskID>  An identifier for a mask
#<plane>  The plane in that to split the mask. Valid
#       planes are: axial | coronal | sagittal | x | y | z
#<slice>  The slice position to split the mask.
#<direction>  The direction keyword for the side of the mask
#       to return. Valid directions are: + | - | positive |
#       negative.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_split_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 xor masks <mask-1>  <mask-2>
#Description  Difference (exclusive or) of two spatial masks.
#<mask-1>
#<mask-2>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_xor_masks { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 finish image-table <Count-table>  <Sum-table>
#Description  Divides the sum table by the count table to obtain
#       the mean table.
#       See Also: 'b2 create image-table'.
#       See Also: 'b2 generate table pseudo-image'.
#<Count-table>
#<Sum-table>
#Return  Upon successful completion, returns the mean table.  If an
#       error occurs, then the error status of -1 is returned.
b2_proc_generator b2_finish_image-table { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 generate table pseudo-image <T1-image> <T2-image>  <Value-table>
#Description  Apply the expected value table to the first and
#       second images to reconstruct the missing third image.
#       See Also: 'b2 create image-table'.
#       See Also: 'b2 finish image-table'.
#<T1-image>
#<T2-image>
#<Value-table>
#Return  Upon successful completion, returns the reconstructed
#       PD image.  If an error occurs, then the error status of
#       -1 is returned.
b2_proc_generator b2_generate_table_pseudo-image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 generate class-plugs <image list> <class list> <mean-table>  <plug-region> <r= random-seed> <n= num-plugs> <nm= normal min-plugs> <sm= scarce min-plugs> <c= coverage> <p= permissiveness> <mo= trim mean outlier effect> <vo= trim variance outlier effect> <ps= plugSize-in-mm> <dx= num-x-chunks> <dy= num-y-chunks> <dz= num-z-chunks> <pb= pick blood from mask or rule code> <bi= blood-picking-image> <CLASS= class-minPlugs>
#Description  Tissue type training classes are generated stereo-
#       taxically based on random plugs of size 'ps=' within
#       mask 'plug-region' selected for unusually low variance
#       based on the variances in 'mean-table' and partitioned
#       by kmeans starting at the means in 'mean-table', one
#       for each class in 'class list'.
#       >> See Also:  the script brains2TissueClassify that is
#        called by the Workup:Run Segment:Brains2... wizzard.
#<image list>  A list of numImages imageIDs.
#<class list>  A list of numClasses names of classes, excluding blood.
#       >> 'image list' and 'class list' establish the
#        standard order within the method.
#<mean-table>  A list of numImages lists of numClasses expected image
#       means, plus an optional expected image plug variance.
#<plug-region>  A 'brain box' mask,  or a prior run's segment mask.
#<r= random-seed>  User-controlled for reproducibility.
#<n= num-plugs>  Normally 4000; how many plugs of acceptable variance
#       to try to pick each time you relax the plug variance
#       criterion by 'permissiveness'.
#<nm= normal min-plugs>  General case of number of training class plugs to
#       require. Defaults to 'num-plugs'.
#<sm= scarce min-plugs>  Number of training class plugs to require in classes
#       like 'csf'.
#<c= coverage>  Normally 0.85, fraction of spatial distribution extent
#       within 'plug-region' to require in each training class
#       for each of 3 dimensions (xyz).
#<p= permissiveness>  Normally 0.10, The size of the steps in the plug
#       variance criterion as you harvest another 'num-plugs'
#       training class exemplars.
#<mo= trim mean outlier effect>  Number of standard deviations in the
#           plug means to include.
#<vo= trim variance outlier effect>  Number of standard deviations in the
#           plug variances to include.
#<ps= plugSize-in-mm>  Normally 2.0, this lets you pick smaller plugs if you
#       need to.
#<dx= num-x-chunks>  'Chunks' (in x, y or z) is for coping with extreme
#<dy= num-y-chunks>  image ramping.
#<dz= num-z-chunks>
#<pb= pick blood code>  (>= 0)  vb mask,
#         (-1)  bottom 250 pixels of t1, or
#         (-2)  top 250 pixels of t1, or
#         (-3)  outside the head.
#<bi= blood-picking-image>  <bi> is an index in the above-provided 'image
#        list'.  Count from zero.
#<CLASS= class-minPlugs>  Class-specific number of training class plugs to
#        require.
#Return  Upon successful completion a list of plug-based
#       training class masks is returned including the blood
#       mask;  otherwise an error status of -1 is returned.
b2_proc_generator b2_generate_class-plugs { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 generate class-model <image-name list> <image list> <class-name list> <mask list>  <brainbox mask> <hist-eq= 0|1> <tg= float> <ts= float> <x= 0|1> <y= 0|1> <z= 0|1> <xx= 0|1> <xy= 0|1> <xz= 0|1> <yy= 0|1> <yz= 0|1> <zz= 0|1>
#Description  Tissue type training classes are applied to images
#       to produce a discriminant analysis model with
#       selected spatial modeling of class means and pairwise
#       discriminant partitioning of individual voxels by
#       tissue type.
#<image-name list>  The names t1, t2, and pd should be used if applicable.
#<image list>  A list of image-IDs corresponding to the image name
#       list.
#<class-name list>  A list of class name words.  Single words only (up to
#       whitespace).  The names gm, wm, csf and blood should
#       be used to obtain correct and normal behavior.
#<plug mask list>  A list of plug-mask-IDs corresponding to the class
#       name list.  See Also:  'b2 generate class-plugs'.
#<brainbox mask>  Defines the region within which to do histogram
#       equalization on the pairwise discriminant functions.
#<hist-eq= 0|1>  Normally 1, this lets you look at a 'mahalanobis'
#       classification by not equalizing the discriminant
#       function histograms.
#<tg= float>  Trim Gross trims the outliers from the training class
#       masks in terms of the numbers of stddevs beyond the
#       training class image intensity mean to include.
#<ts= float>  Trim Spatial is like Trim Gross only the mean used
#       is the one spatially fitted to the training classes.
#<x= 0|1>
#<y= 0|1>  ... These are the parameters that vary when making
#<z= 0|1>  a least-squares model of the training class
#<xx= 0|1>  means throughout the image to control for
#<xy= 0|1>  ramps in the MR images.
#<xz= 0|1>  ... The spatial model is used in place of the class
#<yy= 0|1>  mean in finding the pairwise mahalanobis distances
#<yz= 0|1>  on which to base within-voxel partial volume
#<zz= 0|1>  estimates.
#Return  Upon successful completion a tissue classifier
#       model object is returned for use with any images
#       fully equivalent to the given images, according to
#       image name;  otherwise an error status of -1 is
#       returned.
b2_proc_generator b2_generate_class-model { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 check roi  <roi>
#Description  Checks the ROI and determines if all ROI
#       strands are closed. All ROI strands that
#       are not closed are returned in a list.
#<roiID>   An identifier for an roi
#Returns:     : A list of ROI strands that are not closed.
#       if all strands are closed then the string
#       All <roiname> ROIs are closed.
b2_proc_generator b2_check_ROI { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 close roi  <roi>
#Description  Closes all unclosed strands of an ROI. The resulting
#       closed ROI is returned to the user. Closing the ROI
#       simply makes the first point in the strand the last
#       point as well.
#<roiID>   An identifier for an roi
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_close_ROI { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 extrude roi <roi> <x-length> <y-length>  <z-length>
#Description  extrudes a new mask within the plane of the given roi,
#       even though you have to give the vector in x, y, and z.
#<roiID>   An identifier for an roi
#<x-length>
#<y-length>  (floating pt) length of extrusion in pixels.
#<z-length>
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_extrude_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 thin roi  <roi> <min= min-slice> <max= max-slice>
#Description  This command reduces the number of points in an ROI
#       by removing every other point in the ROI.
#<roiID>   An identifier for an roi
#<min= min-slice>  The starting slice in that points should be removed.
#<max= max-slice>  The ending slice in that points should be removed.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_thin_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 apply class-model <image-name list> <image list> <tissue-model> <bg= bg-mask> <bw= bw-mask> <ex_t1= mask> <cth= thresh>
#Description  This is the normal way to use a tissue classifier
#       model.  This method assumes a wm-gm-csf linear
#       progression is the objective and produces a single
#       image on a linear scale.
#<image-name list>  A list of image names provided for in the 'tissue-
#       model' object, establishing ordered correspondence
#       with the images in the image list.
#<image list>  A list of image-IDs corresponding to the image name
#       list.
#<tissue-model>  The tissue classifier model object returned by
#       'b2 generate class-model'.
#<bg= bg-mask>  ...Masks to compute the threshold from that defines
#<bw= bw-mask>  the gray-white border in the basal ganglia.
#<ex_t1= mask>  Mask of portions of the t1 image to exclude.
#<cth= thresh>  Usually 197,  this is the classifier threshold
#       for gray matter in the basal ganglia.  This 'cth'
#       is stored in the file system next to the classified
#       image for use by 'b2 convert class-image to discrete'.
#Return  Upon successful completion a tissue classified
#       partial volume image is returned,  for use with
#       'b2 measure class-volume';  otherwise an error status
#       of -1 is returned.
b2_proc_generator b2_apply_class-model { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 apply full-class-model <image-name list> <image list>  <tissue-model>
#Description  This is the abnormal but fully general way to use
#       a tissue classifier model.  This method assumes
#       nothing about the classification scheme and produces
#       one partial volume image per class in the model.
#<image-name list>  A list of images names provided for in the 'tissue-
#       model' object, typically UPPERCASE, establishing ordered correspondence
#       with the images in the image list.
#<image list>  A list of image-IDs corresponding to the image name
#       list.
#<tissue-model>  The tissue classifier model object returned by
#       'b2 generate class-model'.
#Return  Upon successful completion a list of partial-volume
#       images is returned,  one per tissue class in the
#       model;  otherwise an error status of -1 is returned.
b2_proc_generator b2_apply_full-class-model { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 choose-freesurfer-atlas <string>
#Description  Moves the big switch on the FreeSurfer name table
#      to choose between '2005' (the new default), '2005aseg', and '2002'.
#      There is also 'iowa_gross_2005' if you know what you're doing.
#      Returns the number of labels defined in the table.
b2_proc_generator b2_choose-freesurfer-atlas { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 compose transform <head-transform-to-modify>  <new-previous-transform>
#Description  Concatenates two transforms. The previous transform specified
#       is applied before the head transform is applied.
#<head-transform-to-modify>  The transform that is applied after the previous transform
#         is applied
#<new-previous-transform>  The transform that is applied before the head transform
#        is applied. This may be -1 to clear previous transform
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_compose_transforms { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 echo <string> <more-strings> ...
#Description  Mimics the TCL puts command but allows for multiple
#       arguments and does not terminate the line.
#<string-i>  The string or strings to output.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_echo { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 roi voxel intersect <roi>
#Description  Create a mask defined by the intersection of the ROI with image voxels it was defined on.
#<roiID>   An identifier for an roi
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_roi_voxel_intersect { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 student-T <number-list-1> <number-list-2>
#Description  Command to take two lists of numbers and generate
#       the mean of each list as well as the T and P values
#       for the lists.
#<number-list-1>  A TCL formatted list of numbers, i.e. < 0 1 2.0 3 >
#<number-list-2>  A TCL formatted list of numbers, i.e. < 4 6.0 7.1 -4 >
#Return  Returns a list of lists containing average of first
#       list(avg1), average of second list(avg2), T value (T),
#       and p value (p).
b2_proc_generator b2_student-T { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 transform point <transform> <sign (1 or -1)> <floating-point-X> <floating-point-Y>  <floating-point-Z>
#Description  Converts a point between display space and data space.
#<transform>  A standard image transform mapping display position
#       to data position.
#<sign>  (1 or -1) Whether to invert the transform.
#         >> Specifying -1 means use the transform to
#         >> find the data sample position (normal case).
#         >> Specifying 1 means invert the transform.
#        Works numerically on any transform.
#<floating-point-X>  ... x, y, and z coordinates of the point in space to
#<floating-point-Y>  transform.
#<floating-point-Z>
#Return  Returns a list of x, y, and z coordinates of the
#       transformed point.  Otherwise an error status of -1
#       is returned.
b2_proc_generator b2_transform_point { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 help <command>
#Description  Provides online help.  If the optional <command> is
#       not given, a list of valid commands is returned.
#       When <command> is specified, then help on that command
#       is given.
#<command>  The command that help is requested for
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_help { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 apropos <string>
#Description  This command returns the list of all commands that
#       contain the specified string in the command name.
#<string>  The string being searched for in the BRAINS2 commands.
#Return  Returns a list of commands that match the specified
#       string.
b2_proc_generator b2_apropos { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 dirty-list <object-type>
#Description  This command returns a list of objects that have
#       been edited but not saved.
#       Currently this supports ROIs and Landmarks.
#<object-type>  Type of objects to be checked. Valid types are:
#         roi | landmarks
#Return  Returns a list of object names that have been edited
#       but not saved.
b2_proc_generator b2_dirty-list { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 exit <status> <delay>
#Description  This command exits BRAINS2 and sends the exit status
#       back to the shell.
#<status>  Exit status to send back to the shell.
#<delay>  Number of seconds to delay before issuing the exit call.
#Return  This command should not return, otherwise -1 in case of an error
b2_proc_generator b2_exit { {status -1 } {delay 0} } {
##DestroyBatchMRMLScene;
  puts "exit ${status} ${delay}"
  exit ${status};
}

#Usage  b2 file <file-type> <filename= filename>
#Description  Specifies an alternate file to which to write the
#       measurement data, as well as the standard data file.
#       If no filename is given, then the filename specified
#       is cleared and only the default file will be used.
#<file-type>  Must be one of:  saved-volume | saved-class-volume |
#         saved-surface-measures | saved-distance |
#         saved-image-stats
#<filename= filename>  The name of the alternate file to which to write the
#       measurement data.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_file { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 file-filter <mode> <type> <name> <check= check-filter> <read= read-filter> <status= status-filter> <write= write-filter>
#Description  This command allows for the addition or removal of
#       BRAINS2 filters that are used
#       by BRAINS2 to read and write files.
#<mode>  The mode of the command. Valid modes are: add | remove
#<type>  The type of filter. The valid types are:
#         image | mask | roi | surface | gts | transform |
#         talairach-parameters | talairach-mask | landmark |
#         histogram | talairach-box | palette | table |
#         tissue-class | blobs | GTSurface
#<name>  Name of the filter to be added or removed.
#<check= >  The command used by the filter to check file types.
#<read= >  The command used by the filter to read files.
#<status= >  The command used by the filter to check a files status.
#<write= >  The command used by the filter to write a file.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_file-filter { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 filter-list <object-type> <mode= mode>
#Description  This command returns a list of filters for the object
#       type of interest.  The mode option defines the type
#       of filter operations of interest.
#<object-type>  The filters for the specified object type. Valid
#       types are:  image | mask | roi | GTSurface | surface |
#         transform | talairach-parameters | taliarach-mask |
#         landmark | histogram | talairach-box | palette |
#         table | tissue-class
#<mode= >  Supported mode of the filter. Valid modes are:
#         load | save | all
#       The mode of all returns all filters.
#Return  Returns a list of filters for the specified object
b2_proc_generator b2_filter-list { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 HTMLhelp <command>
#Description  Provides online HTMLhelp.  If the optional <command> is
#       not given, a list of valid commands is returned.
#       When <command> is specified, then help on that command
#       is given.
#<command>  The command that help is requested for
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_HTMLhelp { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 object-index <object-type> <index>
#Description  This command returns an object ID based on the index
#       value provided. This does a conversion from TCL list
#       index to the actual object ID used by BRAINS2.
#<type>  The type of object being converted from TCL index to
#       BRAINS2 ID. Valid types are:
#         image | roi | mask | surface | transform |
#         talairach-parameters | talairach-box | histogram |
#         landmark | palette | table | tissue-class
#<index>  The TCL index being converted for the specified object
#       type.
#Return  Return the BRAINS2 ID for the specified object.  If an
#       error occurs, then the error status of -1 is returned.
b2_proc_generator b2_object-index { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 object-filename <object-type> <object>
#Description  Returns the filename of the specified object.
#<object-type>  The type of object for the filename query. Valid
#       types are:
#         image | roi | mask | surface | transform |
#         talairach-parameters | talairach-box | histogram |
#         landmark | palette | table | tissue-class
#Return  Return the filename of the specified object.  If an
#       error occurs, then the error status of -1 is returned.
b2_proc_generator b2_object-filename { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 object-list <object-type>
#Description  Returns a list of objects currently loaded into
#       BRAINS2 of the specified object type.
#<object-type>  The type of objects to return a list of.  Valid types
#       are:
#         image | roi | mask | surface | transform |
#         talairach-parameters | talairach-box | histogram |
#         landmark | palette | table | tissue-class
#Return  Return a list of objects of the specified type.  If an
#       error occurs, then the error status of -1 is returned.
b2_proc_generator b2_object-list { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 object-name <object-type> <object>
#<object-type>  The object-type being queried for its name. Valid
#       types are:
#         image | roi | mask | surface | landmark |
#         transform | talairach-parameters | talairach-box |
#         histogram | palette | table | tissue-class
#<object>  The object ID being queried.
#Return  Returns the name of the specified object.  If an
#       error occurs, then the error status of -1 is returned.
b2_proc_generator b2_object-name { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 version
#Description  Returns the current version of BRAINS2 in a string
#Return  Returns the current version of BRAINS2.  If an error
#       occurs, then the error status of -1 is returned.
b2_proc_generator b2_version { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 configuration list  <type>
#Description  This command returns a list of ROI or landmark names
#       that have previously been defined with the
#       'b2 default' command
#<type>  The type of configuration list parameters to return.
#       Valid types are:
#         rois | landmarks | masks
#Return  Returns a list of previously defined objects using
#       the 'b2 default' command
b2_proc_generator b2_configuration-list { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 default mask <filepattern= maskName> <red= float> <green= float> <blue= float> <line-thickness= float> <opacity= float> <visible= on|off> <invert= true|false> <perimeter= true|false> <mode= add|remove|remove-all>.
#Description  This command adds or removes mask names and changes
#       display properties from the default values used by
#       BRAINS2.
#<filepattern= >  A shell wildcard pattern.  If the mask filename matches the shell wildcard
#          pattern, then this set of default values will be used.
#          NOTE: The wildcard pattern is passed through TCL, so
#          you will need to escape <> characters
#          i.e. use \<lr\>_mymask to match left and right versions of
#          mymask with the same default values
#<red= >  The default red value for the mask (0.0 ... 1.0)
#<green= >  The default green value for the mask (0.0 ... 1.0)
#<blue= >  The default blue value for the mask (0.0 ... 1.0)
#<line-thickness= >  The default line thickness for the mask (0.0 ... 10.0)
#<opacity= >  The default opacity value for the mask (0.0 ... 1.0)
#<visible= >  The mask is visible by default. Valid values
#       are: on | off. This is currently ignored
#<invert= >  The mask is inverted by default. Valid values
#       are: true | false
#<perimeter=  >  The mask is displayed as a perimeter by
#       default. Valid values are: true | false
#<mode=  >  The mode of the command. Valid modes are:
#         add | remove | remove-all
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_default_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 default roi <name= roiName> <red= float> <green= float> <blue= float> <line-thickness= float> <xHairSize= float> <visible= on|off> <yellow-x= true|false> <intersections= all|xhair|none> <mode= add|remove|remove-all>.
#Description  This command add or removes ROI names and change
#       display properties from the default values used by
#       BRAINS2.
#<name= >  The name of the ROI to add or delete.
#<red= >  The default red value for the ROI (0.0 ... 1.0)
#<green= >  The default green value for the ROI (0.0 ... 1.0)
#<blue= >  The default blue value for the ROI (0.0 ... 1.0)
#<line-thickness= >  The default line thickness for the ROI (0.0 ... 10.0)
#<xHairSize= >  The default crosshair size for the ROI (0.0 ... 10.0)
#<visible= >  The is the ROI is visible by default. Valid values
#       are: on | off. This is currently ignored
#<yellow-x= >  The is the ROI crosshairs are displayed as yellow by
#       default. Valid values are: true | false
#<intersections=  >  The is the crosshair mode for the ROIs. Valid values
#       are: all | xhair | none
#<mode=  >  The mode of the command. Valid modes are:
#       add | remove | remove-all
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_default_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 default landmark <group-name= name> <mode= add|remove|remove-all> <points= <point-name-1> ... <point-name-N>>
#Description  This command add or removes Landmark group names and
#       the associated points used by default for newly
#       created landmark groups.
#<group-name= >  The name of the Landmark group to add or delete.
#<mode=  >  Specifies the mode of the command. Valid modes are:
#         add | remove | remove-all
#<points= >  Specifies a list of points names to be defined by
#       default for the landmark group.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_default_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show every <object-type | viewer>
#Description  Displays all objects of the specified type in the
#       image viewers.
#object-type    : image | roi | mask | surface | landmark |
#       talairach-parameters | loop-matcher | viewer
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_every { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show everything
#Description  Displays all objects in the image viewers.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_everything { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show image <image>
#Description  This function will make the selected image show up in
#       the image viewers.
#<imageID>  An identifier for an image
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show landmark <landmarksetID>
#Description  This function will make the selected landmark set
#       show up in the image viewers as crosses.
#<landmarksetID>  An identifier for a landmark set
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show mask  <mask>
#Description  This function will make the selected mask show up in
#       the image viewers.
#<maskID>  An identifier for a mask
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show roi  <roi>
#Description  This function will make the selected roi show up in
#       the image viewers.
#<roiID>   An identifier for an roi
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show GTSurface  <GTSurface>
#Description  This function will make the selected GTSurface show up in
#       the image viewers.
#<GTSurfaceID>  An identifier for a GTSurface
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show talairach-parameters <talairach-parameters>
#Description  This function will make the selected talairach bounds
#       show up in the image viewers.
#<talairach-parameters>
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show time-graph
#Description  This function will make the time-graph dialog box
#       appear on the screen.  This utility displays signal
#       intensity for the time dimension in 4D images for
#       the voxel at the 3D cross-hairs.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_time-graph { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 show viewer <axi | sag | cor | surf | tracker> ...
#Description  Places the specified image viewers on the screen.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_show_viewer { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide every <object-type>
#Description  Turns off display of the specified object type in
#       the image viewers.
#<object-type>  image | roi | mask | surface| landmark |
#       talairach-parameters | loop-matcher | viewer
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_every { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide everything
#Description  Turns off display of every object type in the image
#       viewers, and then turns off the image viewers, too.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_everything { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide image  <image>
#Description  Turns off display of the specified image in
#       the image viewers.
#<imageID>  An identifier for an image
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_image { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide landmark <landmarksetID>
#Description  Turns off display of the specified lamdmark object in
#       the image viewers.
#<landmarksetID>  An identifier for a landmark set
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide mask  <mask>
#Description  Turns off display of the specified mask in
#       the image viewers.
#<maskID>  An identifier for a mask
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_mask { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide roi  <roi>
#Description  Turns off display of the specified ROI in
#       the image viewers.
#<roiID>   An identifier for an roi
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide GTSurface  <GTSurface>
#Description  Turns off display of the specified iso-surface in
#       the image viewers.
#<GTSurfaceID>  An identifier for a GTSurface
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_GTSurface { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide talairach-parameters <talairach-parameters>
#Description  Turns off display of the specified talairach
#       parameter bounds inthe image viewers.
#<talairach-parameters>
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide time-graph
#Description  This function will remove the time-graph dialog box
#       from the screen.  This utility displays signal
#       intensity for the time dimension in 4D images for
#       the voxel at the 3D cross-hairs.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_time-graph { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_hide_time-graph Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide tracker
#Description  (Deprecated.)  Removes the image value tracker from
#       the screen. 'b2 hide viewer tracker' is preferred.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_tracker { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_hide_tracker Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 hide viewer <axi | sag | cor | surf | tracker> ...
#Description  Removes the specified image viewers from the screen.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_hide_viewer { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_hide_viewer Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode cerebellum-parameters
#Description  Changes GUI mode to permit an active cerebellum-
#       parameters object to be selected for interactive
#       modification.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_cerebellum-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_cerebellum-parameters Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode hippocampus-parameters
#Description  Changes GUI mode to permit an active hippocampus-
#       parameters object to be selected for interactive
#       modification.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_hippocampus-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_hippocampus-parameters Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode landmark
#Description  Changes GUI mode to permit an active landmark object
#       to be selected for interactive modification.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_landmark Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode resample
#Description  Changes GUI mode to permit AC-PC points to be picked.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_resample { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_resample Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode talairach-parameters
#Description  Changes GUI mode to permit an active talairach-
#       parameters object to be selected for interactive
#       modification.
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_talairach-parameters Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode view <plane>
#Description  Changes GUI mode to view data.
#<plane>  <Window to be changed to view mode
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_view { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_view Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 mode roi <plane>
#Description  Changes GUI mode to permit an ROI object to be selected for
#       interactive modification.
#<plane>  <Window to be changed to roi mode
#Return    Upon successful completion, 0 is returned,
#        otherwise an error status of -1 is returned
b2_proc_generator b2_mode_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_mode_roi Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 active talairach-parameters  <talairach-parameters | -1>
#Description  PRE: This requires that the the program is in
#       talairach-parameters mode.
#       This command then selects a currently loaded set of
#       talairach-parameters, or a new set of talairach-
#       parameters to be modified.
#<talairach-parameters | -1>  If -1, then create a new set of talairach-
#       parameters and set active, otherwise set the specified
#       talairach-parameters to be active.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_active_talairach-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_active_talairach-parameters Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 active cerebellum-parameters  <cerebellum-parameters | -1>
#Description  PRE: This requires that the the program is in
#       cerebellum-parameters mode.
#       : This command then selects a currently loaded set of
#       cerebellum-parameters, or a new set of cerebellum-
#       parameters to be modified.
#<cerebellum-parameters | -1>  If -1, then create a new set of cerebellum-
#       parameters and set active, otherwise set the specified
#       cerebellum-parameters to be active.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_active_cerebellum-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_active_cerebellum-parameters Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 active hippocampus-parameters  <hippocampus-parameters | -1>
#Description  PRE: This requires that the the program is in
#       hippocampus-parameters mode.
#       : This command then selects a currently loaded set of
#       hippocampus-parameters, or a new set of hippocampus-
#       parameters to be modified.
#<hippocampus-parameters | -1>  If -1, then create a new set of hippocampus-
#       parameters and set active, otherwise set the specified
#       hippocampus-parameters to be active.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_active_hippocampus-parameters { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "b2_active_hippocampus-parameters Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 active landmark <landmarksetID>
#Description  PRE: This requires that the the program is in
#       landmark mode.
#       : Selects the loaded landmark id to be active for
#       modification.
#<landmarksetID>  An identifier for a landmark set
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_active_landmark { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}

#Usage  b2 active roi  <roi-name> <plane= plane>
#Description  PRE: This requires that the the program is in
#       tracing mode.
#       : Selects the roi and plane to make active for editing
#       of ROI's.
#<roiID>   An identifier for an roi
#<plane= plane>  The plane to make active for editing of the ROI.
#Return    Upon successful completion a newly created
#        object identifier is returned,  otherwise
#        an error status of -1 is returned.
b2_proc_generator b2_active_roi { { volumeNode - required }  {arugment2defaultvalue 1} {argument3defaultvalue 0} } {
#   Place body of command here.
  set statusvalue [ catch { $volumeNode FAILED_CASE  } res ];
  if { $statusvalue } then {
    puts "XXXX Failed";
    return -1;
  }
  return -1;
}
