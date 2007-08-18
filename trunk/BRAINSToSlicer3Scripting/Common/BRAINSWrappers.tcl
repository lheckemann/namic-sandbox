
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
       uplevel 1 [list error "missing argument $_i"]
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
      puts "init $_init val $_initv"
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
  puts "$name:\n[info body $name]"
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
## Simple Load image
b2_proc_generator b2_load_image { { fileName - required }  {centered 1} {labelimage 0} {NodeName EmptyStringValue} } {
#  puts [ GetBatchVolumesLogic ]
  if { "${NodeName}" == "EmptyStringValue" } then {
    set NodeName [file tail $fileName]_$::FileReaderIncrementUniqueIdCounter;

    incr ::FileReaderIncrementUniqueIdCounter;
  }
  puts "set volumeNode [ [ GetBatchVolumesLogic ] AddArchetypeVolume $fileName $centered ${labelimage} ${NodeName} ]"
  set volumeNode [ [ GetBatchVolumesLogic ] AddArchetypeVolume $fileName $centered ${labelimage} ${NodeName} ]
  return ${volumeNode}
}

b2_proc_generator b2_save_image { { ImageToSave - required } { outputFileName - required } } {
    set VolumeStorageNode [ vtkMRMLVolumeArchetypeStorageNode New ]
    $VolumeStorageNode SetFileName ${outputFileName}
    $VolumeStorageNode WriteData ${ImageToSave}
    $VolumeStorageNode Delete
}


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

b2_proc_generator b2_get_dims_image { { volumeNode - required } } {
  set statusvalue [catch { [ $volumeNode GetImageData ] GetDimensions } dims ];
  if { $statusvalue } then {
    puts "b2_get_dims_image failed";
    return -1;
  }
  return $dims;
}

b2_proc_generator b2_get_res_image { { volumeNode - required } } {
  set statusvalue [ catch { $volumeNode GetSpacing  } res ];
  if { $statusvalue } then {
    puts "b2_get_res_image failed";
    return -1;
  }
  return $res;
}

b2_proc_generator b2_destroy_image { { volumeNode - required } } {
  set statusvalue [ catch { $volumeNode Delete  } returnvalue ];
  if { $statusvalue } then {
    puts "b2_destroy_image failed";
    return -1;
  }
  puts "Removed ${volumeNode} with message $returnvalue"
  return 0;
}

proc b2_load_blobs args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_histogram args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_palette args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_table args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_talairach-box args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_tissue-class args { foreach arg $args { puts $arg }; return -1; };
proc b2_load_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_blobs args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_histogram args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_palette args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_table args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_talairach-box args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_tissue-class args { foreach arg $args { puts $arg }; return -1; };
proc b2_save_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_gts-rois args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_affine-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_bounded-image args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_boxed-image args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_bullet-image args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_event-table args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_hem-roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_histogram args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_roi-hull args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_table-mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_witelson-rois args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_viewer args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_image-table args { foreach arg $args { puts $arg }; return -1; };
proc b2_create_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_color_GTSurface_labels args { foreach arg $args { puts $arg }; return -1; };
proc b2_color_GTSurface_masks args { foreach arg $args { puts $arg }; return -1; };
proc b2_color_GTSurface_normals args { foreach arg $args { puts $arg }; return -1; };
proc b2_color_GTSurface_standard-depth args { foreach arg $args { puts $arg }; return -1; };
proc b2_color_GTSurface_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_GTSurface_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_GTSurface_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_bounds_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_class-volume_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_class-volume_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_image_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_image_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_landmark_distance args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_resel-size_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_volume_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_volume_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_measure_roi_perimeter args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_mask_to_GTSurface-gm-mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_mask-set_to_code-image args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_code-image_to_mask-set args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_class-image_to_discrete args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_landmark_to_talairach-landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_landmarks_to_bookstein-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_location_to_talairach-mm args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_mask_to_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_mask_to_talairach-mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_rgb-image_to_hsi args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_roi_to_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-mm_to_location args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-box_to_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-landmark_to_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-mask_to_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-parameters_to_acquisition-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-parameters_to_cerebellum-landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-parameters_to_cortical-landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-parameters_to_montreal-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_talairach-parameters_to_talairach-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_convert_GTSurface-set_to_code-image args { foreach arg $args { puts $arg }; return -1; };
proc b2_nudge_affine-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_invert_affine-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_decompose_affine-transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_everything args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_every args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_blob args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_histogram args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_palette args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_table args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_talairach-box args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_tissue-class args { foreach arg $args { puts $arg }; return -1; };
proc b2_destroy_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_image_value args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_location args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_mask_color args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_roi_color args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_palette_data args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_palette_type args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_res_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_dims_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_dims_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_dims_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_dims_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_dims_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_histogram_data args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_image_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_image_dataType args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_image_type args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_image_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_landmark_count args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_landmark_location args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_landmark_names args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_name args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_patient-id args { foreach arg $args { puts $arg }; return -1; };

proc b2_get_res_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_res_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_res_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_res_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_reslice-dims_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_reslice-dims_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_reslice-res_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_reslice-res_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_scan-id args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_standard-dims_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_standard-dims_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_standard-res_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_standard-res_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_table_data args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_talairach_points args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_threshold args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_tissue-class_kappa args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_transform_type args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_transform_affine-matrix args { foreach arg $args { puts $arg }; return -1; };
proc b2_get_transform_composite-affine-matrix args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_debug-level args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_filename args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_image_palette args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_image_res args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_interpolation args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_numeric_acpc args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_location args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_name args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_pixmap-directory args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_standard-size args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_standard-dimensions args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_threshold args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_transform args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_set_time-graph_overlay args { foreach arg $args { puts $arg }; return -1; };
proc b2_window_geometry args { foreach arg $args { puts $arg }; return -1; };
proc b2_inscribe-curvature-planes args { foreach arg $args { puts $arg }; return -1; };
proc b2_overwrite_transform_affine-matrix args { foreach arg $args { puts $arg }; return -1; };
proc b2_autocorrelation-lambda-resels args { foreach arg $args { puts $arg }; return -1; };
proc b2_average_image-slices args { foreach arg $args { puts $arg }; return -1; };
proc b2_average_landmarks args { foreach arg $args { puts $arg }; return -1; };
proc b2_clip_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_clip_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_compute_effect-size_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_divide_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_equal_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_extrude-acquisition-plane-clipping_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_find_blobs args { foreach arg $args { puts $arg }; return -1; };
proc b2_fit_image_interleaved args { foreach arg $args { puts $arg }; return -1; };
proc b2_hanning-filter2D-pet args { foreach arg $args { puts $arg }; return -1; };
proc b2_histogram-equalize_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_image_min args { foreach arg $args { puts $arg }; return -1; };
proc b2_image_max args { foreach arg $args { puts $arg }; return -1; };
proc b2_intensity-rescale_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_max_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_min_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_multiply_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_pooled-stddev-conversion args { foreach arg $args { puts $arg }; return -1; };
proc b2_stat_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_region_grow args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkBilateral args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkConnectedThreshold args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkDanielssonDistanceMap args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkDiscreteGaussian args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkFlipImage args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkGeodesicActiveContour args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkGradientAnisotropicDiffusion args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkGradientMagnitude args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkLargestRegionFilledMask args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkMedian args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkPermuteAxes args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkRecursiveGaussian args { foreach arg $args { puts $arg }; return -1; };
proc b2_itkSigmoid args { foreach arg $args { puts $arg }; return -1; };
proc b2_resample_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_resample_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_resample_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_resample_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_resample_ROI args { foreach arg $args { puts $arg }; return -1; };
proc b2_resample_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_subtract_from_constant args { foreach arg $args { puts $arg }; return -1; };
proc b2_subtract_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_standardize-neighborhood_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_sum_images args { foreach arg $args { puts $arg }; return -1; };
proc b2_sum_masks args { foreach arg $args { puts $arg }; return -1; };
proc b2_threshold_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_threshold_histogram_pair args { foreach arg $args { puts $arg }; return -1; };
proc b2_trim-effect_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_and_masks args { foreach arg $args { puts $arg }; return -1; };
proc b2_clean_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_dilate_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_erode_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_erode_surface_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_fill_brainseg_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_fill_internal_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_not_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_or_masks args { foreach arg $args { puts $arg }; return -1; };
proc b2_split_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_xor_masks args { foreach arg $args { puts $arg }; return -1; };
proc b2_finish_image-table args { foreach arg $args { puts $arg }; return -1; };
proc b2_generate_table_pseudo-image args { foreach arg $args { puts $arg }; return -1; };
proc b2_generate_class-plugs args { foreach arg $args { puts $arg }; return -1; };
proc b2_generate_class-model args { foreach arg $args { puts $arg }; return -1; };
proc b2_check_ROI args { foreach arg $args { puts $arg }; return -1; };
proc b2_close_ROI args { foreach arg $args { puts $arg }; return -1; };
proc b2_extrude_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_thin_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_apply_class-model args { foreach arg $args { puts $arg }; return -1; };
proc b2_apply_full-class-model args { foreach arg $args { puts $arg }; return -1; };
proc b2_choose-freesurfer-atlas args { foreach arg $args { puts $arg }; return -1; };
proc b2_compose_transforms args { foreach arg $args { puts $arg }; return -1; };
proc b2_echo args { foreach arg $args { puts $arg }; return -1; };
proc b2_roi_voxel_intersect args { foreach arg $args { puts $arg }; return -1; };
proc b2_student-T args { foreach arg $args { puts $arg }; return -1; };
proc b2_transform_point args { foreach arg $args { puts $arg }; return -1; };
proc b2_help args { foreach arg $args { puts $arg }; return -1; };
proc b2_apropos args { foreach arg $args { puts $arg }; return -1; };
proc b2_dirty-list args { foreach arg $args { puts $arg }; return -1; };
proc b2_exit args { foreach arg $args { puts $arg }; return -1; };
proc b2_file args { foreach arg $args { puts $arg }; return -1; };
proc b2_file-filter args { foreach arg $args { puts $arg }; return -1; };
proc b2_filter-list args { foreach arg $args { puts $arg }; return -1; };
proc b2_HTMLhelp args { foreach arg $args { puts $arg }; return -1; };
proc b2_object-index args { foreach arg $args { puts $arg }; return -1; };
proc b2_object-filename args { foreach arg $args { puts $arg }; return -1; };
proc b2_object-list args { foreach arg $args { puts $arg }; return -1; };
proc b2_object-name args { foreach arg $args { puts $arg }; return -1; };
proc b2_version args { foreach arg $args { puts $arg }; return -1; };
proc b2_configuration-list args { foreach arg $args { puts $arg }; return -1; };
proc b2_default_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_default_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_default_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_every args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_everything args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_time-graph args { foreach arg $args { puts $arg }; return -1; };
proc b2_show_viewer args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_every args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_everything args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_image args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_mask args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_GTSurface args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_time-graph args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_tracker args { foreach arg $args { puts $arg }; return -1; };
proc b2_hide_viewer args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_cerebellum-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_hippocampus-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_resample args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_view args { foreach arg $args { puts $arg }; return -1; };
proc b2_mode_roi args { foreach arg $args { puts $arg }; return -1; };
proc b2_active_talairach-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_active_cerebellum-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_active_hippocampus-parameters args { foreach arg $args { puts $arg }; return -1; };
proc b2_active_landmark args { foreach arg $args { puts $arg }; return -1; };
proc b2_active_roi args { foreach arg $args { puts $arg }; return -1; };

proc b2_exit {} {
  DestroyBatchMRMLScene;
}
