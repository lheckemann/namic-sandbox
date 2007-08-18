# \author    Greg Harris
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the brains2 side of generating and measuring a freeSurfer MR5 workup.
# \fn        proc freeSurfer {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
#
# To Do
#------------------------------------------------------------------------
#



proc freeSurfer {pathToRegressionDir dateString} {

    set ModuleName "freeSurfer"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the brains2 side of generating and measuring a freeSurfer MR5 workup"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    global OUTPUT_DIR
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    #step  4:  Make dimorphism-weighted unsharp-mask versions of the PD.



    set a2 [b2_load_image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_PD_orig.hdr]
    set mh [b2_itkConnectedThreshold $a2 min= 25 max= 255 seed= [list 137 128 125]]
    set pd [b2_standardize-neighborhood_image $a2 $mh 5 dimorph-weight= 0.8]


    set observedImageMeasures [b2 measure Image Mask $mh $pd]
    set knownImageMeasures {
    {Mean 124.812741733719903} {Mean-Absolute-Deviation 46.014777372110515} {Variance 3002.562490944826095} {Standard-Deviation 54.795642992347723} {Skewness 1.418036596968040} {Kurtosis -0.752911427390599} {Minimum 0.001452067983337} {Maximum 338.809173583984375}
    }
    CoreMeasuresEpsilonTest "standardize-neighborhood image" 0.001 $knownImageMeasures $observedImageMeasures $LogFile $ModuleName

    b2_destroy_image $a2
    b2_destroy_image $pd
    b2_destroy_mask $mh



    #step  7:  Find the cerebrum of the standard workup using the existing .gts files.
    #          Stores it in the FreeSurfer frame of reference.



    b2_hide_viewer surf
    set a0 [b2 load Image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_T1.hdr data-type= unsigned-8bit]
    set s0 [b2_load_GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_l_dec35.gts]
    b2_hide_GTSurface $s0
    set s1 [b2_load_GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_r_dec35.gts]
    b2_hide_GTSurface $s1
    set cr0 [b2_create_gts-rois $s0 cor name= l_gts_cor]
    set cr1 [b2_create_gts-rois $s1 cor name= r_gts_cor]
    set cm0 [b2 convert ROI to Mask $cr0]
    set cm1 [b2 convert ROI to Mask $cr1]
    set ar0 [b2_create_gts-rois $s0 axi name= l_gts_axi]
    set ar1 [b2_create_gts-rois $s1 axi name= r_gts_axi]
    set am0 [b2 convert ROI to Mask $ar0]
    set am1 [b2 convert ROI to Mask $ar1]
    set sr0 [b2_create_gts-rois $s0 sag name= l_gts_sag]
    set sr1 [b2_create_gts-rois $s1 sag name= r_gts_sag]
    set sm0 [b2 convert ROI to Mask $sr0]
    set sm1 [b2 convert ROI to Mask $sr1]
    set m0 [b2_and_masks $cm0 $am0 $sm0]
    set m1 [b2_and_masks $cm1 $am1 $sm1]
    set m2 [b2_dilate_mask $m0 1]
    set m3 [b2_dilate_mask $m1 1]
    set m6 [b2_not_mask $m2]
    set m7 [b2_not_mask $m3]
    set m4 [b2_and_masks $m2 $m7]
    set m5 [b2_and_masks $m3 $m6]
    set m2a [b2_dilate_mask $m4 1]
    set m3a [b2_dilate_mask $m5 1]
    set m6a [b2_not_mask $m2a]
    set m7a [b2_not_mask $m3a]
    set m8 [b2_and_masks $m2a $m7a]
    set m9 [b2_and_masks $m3a $m6a]
    set r2 [b2 load ROI ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/crbl_gross_cut.xroi]
    set m10 [b2 convert ROI to Mask $r2]
    set m11 [b2_dilate_mask $m10 2]
    set m12 [b2_not_mask $m11]
    set m13 [b2_and_masks $m8 $m12]
    set m14 [b2_and_masks $m9 $m12]
    set m15 [b2_or_masks $m13 $m14]

    set m15a [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_bilateral_cerebrum.mask]
    set m15b [b2_xor_masks $m15 $m15a]
    set results [b2_measure_volume_mask $m15b]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "bilateral_cerebrum.mask" 0.001 $knownResults $results $LogFile $ModuleName

    set m13a [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_cerebrum.mask]
    set m13b [b2_xor_masks $m13 $m13a]
    set results [b2_measure_volume_mask $m13b]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "left_cerebrum.mask" 0.001 $knownResults $results $LogFile $ModuleName

    set m14a [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_cerebrum.mask]
    set m14b [b2_xor_masks $m14 $m14a]
    set results [b2_measure_volume_mask $m14b]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "right_cerebrum.mask" 0.001 $knownResults $results $LogFile $ModuleName

    b2_destroy_image $a0
    b2_destroy_GTSurface $s0
    b2_destroy_GTSurface $s1
    b2_destroy_roi $cr0
    b2_destroy_roi $cr1
    b2 destroy Mask $cm0
    b2 destroy Mask $cm1
    b2_destroy_roi $ar0
    b2_destroy_roi $ar1
    b2 destroy Mask $am0
    b2 destroy Mask $am1
    b2_destroy_roi $sr0
    b2_destroy_roi $sr1
    b2 destroy Mask $sm0
    b2 destroy Mask $sm1
    b2 destroy Mask $m0
    b2 destroy Mask $m1
    b2 destroy Mask $m2
    b2 destroy Mask $m3
    b2 destroy Mask $m6
    b2 destroy Mask $m7
    b2 destroy Mask $m4
    b2 destroy Mask $m5
    b2 destroy Mask $m2a
    b2 destroy Mask $m3a
    b2 destroy Mask $m6a
    b2 destroy Mask $m7a
    b2 destroy Mask $m8
    b2 destroy Mask $m9
    b2_destroy_roi $r2
    b2 destroy Mask $m10
    b2 destroy Mask $m11
    b2 destroy Mask $m12
    b2 destroy Mask $m13
    b2 destroy Mask $m14
    b2 destroy Mask $m15
    b2 destroy Mask $m15a
    b2 destroy Mask $m15b
    b2 destroy Mask $m13a
    b2 destroy Mask $m13b
    b2 destroy Mask $m14a
    b2 destroy Mask $m14b



    #step  8:  Pick up the basal ganglia masks from standard workup and save the information
    #          in the FreeSurfer frame of reference.



    set mlc [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_caud_cut.mask]
    set mrc [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_caud_cut.mask]
    set mlp [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_put_cut.mask]
    set mrp [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_put_cut.mask]
    set mlg [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_globus_cut.mask]
    set mrg [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_globus_cut.mask]
    set mlt [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_thal_cut.mask]
    set mrt [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_thal_cut.mask]
    set mlbg [b2_or_masks $mlc $mlp $mlg $mlt]
    set mrbg [b2_or_masks $mrc $mrp $mrg $mrt]

    set mlbgy [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_basal_ganglia.mask]
    set mlbgz [b2_xor_masks $mlbg $mlbgy]
    set results [b2_measure_volume_mask $mlbgz]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "left_basal_ganglia.mask" 0.001 $knownResults $results $LogFile $ModuleName

    set mrbgy [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_basal_ganglia.mask]
    set mrbgz [b2_xor_masks $mrbg $mrbgy]
    set results [b2_measure_volume_mask $mrbgz]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "right_basal_ganglia.mask" 0.001 $knownResults $results $LogFile $ModuleName

    b2_destroy_mask $mlc
    b2_destroy_mask $mrc
    b2_destroy_mask $mlp
    b2_destroy_mask $mrp
    b2_destroy_mask $mlg
    b2_destroy_mask $mrg
    b2_destroy_mask $mlt
    b2_destroy_mask $mrt
    b2_destroy_mask $mlbg
    b2_destroy_mask $mrbg
    b2_destroy_mask $mlbgy
    b2_destroy_mask $mlbgz
    b2_destroy_mask $mrbgy
    b2_destroy_mask $mrbgz



    #step 15:  Generate and measure the brains2 gray matter ribbons for the FreeSurfer parcels.



    set family 0
    set classImg [b2 load Image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_stereo.hdr data-type= unsigned-8bit]
    set m0 [b2 load Mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_bilateral_cerebrum.mask]
    set m1 [b2 dilate Mask $m0 5]
    b2_destroy_mask $m0
    set g0 [b2_load_GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/FreeSurfer_Subjects/Parcellate_ANON002/surf/rh.orig filter= FreeSurfer]
    b2_color_GTSurface_labels $g0 $family
    set g1 [b2_load_GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/FreeSurfer_Subjects/Parcellate_ANON002/surf/lh.orig filter= FreeSurfer]
    b2_color_GTSurface_labels $g1 $family
    set im1 [b2_convert_GTSurface-set_to_code-image $m1 $family [list $g0 $g1]]
    b2_destroy_mask $m1
    b2 destroy gtsurface $g0
    b2 destroy gtsurface $g1

    b2_save_image ${OUTPUT_DIR}/Parcellate_ANON002_orig_bilateral_cortical_parcels.hdr strictAnalyze75 $im1 data-type= S16
    if {0==1} {

        # The very fact that there are just a few flecks of unreliability from run to run mostly on slice y=128 is tough to explain.
        #
        set im1y [b2_load_image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/Parcellate_ANON002_orig_bilateral_cortical_parcels.hdr data-type= S16]
        set im1z [b2_subtract_images [list $im1 $im1y]]
        b2_destroy_image $im1y
        ReportTestStatus $LogFile  [ expr {[b2_image_min $im1z] == 0.0 } ] $ModuleName "Min bilateral_cortical_parcels difference should be zero."
        ReportTestStatus $LogFile  [ expr {[b2_image_max $im1z] == 0.0 } ] $ModuleName "Max bilateral_cortical_parcels difference should be zero."
        b2_destroy_image $im1z
    }

    set pairs [b2_convert_code-image_to_mask-set $im1 standard]
    b2_destroy_image $im1

    set left_cerebrum [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_cerebrum.mask]
    set right_cerebrum [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_cerebrum.mask]
    foreach t {1 2 3 4 5} {
        set left_cerebrum_dilated [b2 dilate Mask $left_cerebrum 1]
        set right_cerebrum_dilated [b2 dilate Mask $right_cerebrum 1]
        set left_non_prev [b2_not_mask $left_cerebrum]
        set right_non_prev [b2_not_mask $right_cerebrum]
        b2_destroy_mask $left_cerebrum
        b2_destroy_mask $right_cerebrum
        set left_cerebrum [b2_and_masks $left_cerebrum_dilated $right_non_prev]
        set right_cerebrum [b2_and_masks $right_cerebrum_dilated $left_non_prev]
        b2_destroy_mask $left_cerebrum_dilated
        b2_destroy_mask $right_cerebrum_dilated
        b2_destroy_mask $left_non_prev
        b2_destroy_mask $right_non_prev
    }
    set left_non_prev [b2_not_mask $left_cerebrum]
    set right_non_prev [b2_not_mask $right_cerebrum]
    set left_hemi [b2_and_masks $left_cerebrum $right_non_prev]
    set right_hemi [b2_and_masks $right_cerebrum $left_non_prev]
    b2_destroy_mask $left_cerebrum
    b2_destroy_mask $right_cerebrum
    b2_destroy_mask $left_non_prev
    b2_destroy_mask $right_non_prev

    set l_basal_ganglia [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_basal_ganglia.mask]
    set r_basal_ganglia [b2_load_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_basal_ganglia.mask]
    set l_basal_ganglia_roi [b2 convert Mask to ROI Y $l_basal_ganglia skip= 1 minimum-overlap= 0.00]
    set r_basal_ganglia_roi [b2 convert Mask to ROI Y $r_basal_ganglia skip= 1 minimum-overlap= 0.00]
    set l_basal_ganglia_hull_roi [b2_create_roi-hull $l_basal_ganglia_roi 2D]
    set r_basal_ganglia_hull_roi [b2_create_roi-hull $r_basal_ganglia_roi 2D]
    set lbg [b2_convert_roi_to_mask $l_basal_ganglia_hull_roi]
    set rbg [b2_convert_roi_to_mask $r_basal_ganglia_hull_roi]
    b2_destroy_mask $l_basal_ganglia
    b2_destroy_mask $r_basal_ganglia
    b2_destroy_roi $l_basal_ganglia_roi
    b2_destroy_roi $r_basal_ganglia_roi
    b2_destroy_roi $l_basal_ganglia_hull_roi
    b2_destroy_roi $r_basal_ganglia_hull_roi

    set bg [b2_or_masks $lbg $rbg]
    set no_bg [b2_not_mask $bg]
    set left_hemisphere [b2_and_masks $left_hemi $no_bg]
    set right_hemisphere [b2_and_masks $right_hemi $no_bg]
    b2_destroy_mask $lbg
    b2_destroy_mask $rbg
    b2_destroy_mask $bg
    b2_destroy_mask $no_bg
    b2_destroy_mask $left_hemi
    b2_destroy_mask $right_hemi

    set FundalMeanCurvature 2
    set GyralMeanCurvature 8
    set TotalSumArea 12
    set FundalMeanDepth 20
    set FundalStdDevDepth 23
    set GyralMeanDepth 26
    set GyralStdDevDepth 29
    set TotalMeanDepth 32
    set TotalStdDevDepth 35
    set measurements [list]
    set left_130 [b2_load_GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_l_dec35.gts]
    set right_130 [b2_load_GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_r_dec35.gts]
    foreach pair $pairs {
      set name "[lindex $pair 0]"
      set bilateral_mask [lindex $pair 1]
      set gts_gm_mask [b2_convert_mask_to_GTSurface-gm-mask 5 $classImg $bilateral_mask ${left_130} ${right_130}]
      set left_gm_mask [b2_and_masks $gts_gm_mask $left_hemisphere]
      set right_gm_mask [b2_and_masks $gts_gm_mask $right_hemisphere]
    #  b2_save_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_gm_${name}.mask brains2 $left_gm_mask
    #  b2_save_mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_gm_${name}.mask brains2 $right_gm_mask
    #  lappend leftMasksInOrder $left_gm_mask
    #  lappend rightMasksInOrder $right_gm_mask
      lappend colorsInOrder [b2_get_mask_color [lindex $pair 1]]

      set volume [b2_measure_volume_mask $left_gm_mask]
      set result [b2_measure_GTSurface_mask $bilateral_mask ${left_130}]
      # This is bilateral_mask because for GTSurface measures, parcel code space is what counts.
      set report [list [list ParcelRegionName $name] [list Hemisphere Left] [list GrayMatterVolume [lindex [lindex $volume 0] 1]]]
      lappend report [lindex $result $TotalSumArea]
      lappend report [lindex $result $FundalMeanDepth]
      lappend report [lindex $result $GyralMeanDepth]
      lappend report [lindex $result $TotalMeanDepth ]
      lappend report [lindex $result $FundalMeanCurvature]
      lappend report [lindex $result $GyralMeanCurvature]
      lappend measurements [list $report]

      set volume [b2_measure_volume_mask $right_gm_mask]
      set result [b2_measure_GTSurface_mask $bilateral_mask ${right_130}]
      # This is bilateral_mask because for GTSurface measures, parcel code space is what counts.
      set report [list [list ParcelRegionName $name] [list Hemisphere Right] [list GrayMatterVolume [lindex [lindex $volume 0] 1]]]
      lappend report [lindex $result $TotalSumArea]
      lappend report [lindex $result $FundalMeanDepth]
      lappend report [lindex $result $GyralMeanDepth]
      lappend report [lindex $result $TotalMeanDepth ]
      lappend report [lindex $result $FundalMeanCurvature]
      lappend report [lindex $result $GyralMeanCurvature]
      lappend measurements [list $report]

#      b2_destroy_mask $bilateral_mask
      b2_destroy_mask $gts_gm_mask
      b2_destroy_mask $left_gm_mask
      b2_destroy_mask $right_gm_mask
    }
    foreach pair $pairs {
      set bilateral_mask [lindex $pair 1]
      b2_destroy_mask $bilateral_mask
    }

    b2_destroy_image $classImg
    b2 destroy gtsurface $left_130
    b2 destroy gtsurface $right_130
    b2_destroy_mask $left_hemisphere
    b2_destroy_mask $right_hemisphere

    set MeasuresTableFile ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/Parcellate_ANON002_MeasuresTable.tcl

    set flpt [open ${OUTPUT_DIR}/Parcellate_ANON002_MeasuresTable.tcl w]
    puts $flpt "set knownMeasurements {"
    foreach m $measurements { puts $flpt $m }
    puts $flpt "}"
    close $flpt
    source $MeasuresTableFile
    # defines $knownMeasurements in the sourced file.
    while {[llength $measurements] > 0} {
      set observed [lindex [lindex $measurements 0] 0]
      # extra list wrapper was there to make the {  } printing come out right.
      set expected [lindex $knownMeasurements 0]
      puts "\n--Comparing-- $observed"
      puts "---Against--- $expected"
      ReportTestStatus $LogFile  [ expr {[string equal "[lindex $expected 0]" "[lindex $observed 0]"] == 1} ] $ModuleName "Is name '[lindex $observed 0]' the same string as '[lindex $expected 0]'?"
      ReportTestStatus $LogFile  [ expr {[string equal "[lindex $expected 1]" "[lindex $observed 1]"] == 1} ] $ModuleName "Is hemisphere '[lindex $observed 1]' the same string as '[lindex $expected 1]'?"

      # Using epsilon of 0.02, not numerical machine precision:  
      # again, the method is a tiny bit unstable from run to run.
      CoreMeasuresEpsilonTest "[lindex [lindex $expected 1] 1] [lindex [lindex $expected 0] 1] Gray Matter Parcel" 0.02 [lreplace $expected 0 1] [lreplace $observed 0 1] $LogFile $ModuleName
      if {[llength $measurements] > 1} {
        set measurements [lreplace $measurements 0 0]
        set knownMeasurements [lreplace $knownMeasurements 0 0]
      } else {
        set measurements ""
        set knownMeasurements ""
      }
    }


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

