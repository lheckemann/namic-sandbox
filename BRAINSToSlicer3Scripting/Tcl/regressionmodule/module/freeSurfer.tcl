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



    set a2 [b2 load image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_PD_orig.hdr]
    set mh [b2 itkConnectedThreshold $a2 min= 25 max= 255 seed= [list 137 128 125]]
    set pd [b2 standardize-neighborhood image $a2 $mh 5 dimorph-weight= 0.8]


    set observedImageMeasures [b2 measure Image Mask $mh $pd]
    set knownImageMeasures {
    {Mean 124.812741733719903} {Mean-Absolute-Deviation 46.014777372110515} {Variance 3002.562490944826095} {Standard-Deviation 54.795642992347723} {Skewness 1.418036596968040} {Kurtosis -0.752911427390599} {Minimum 0.001452067983337} {Maximum 338.809173583984375}
    }
    CoreMeasuresEpsilonTest "standardize-neighborhood image" 0.001 $knownImageMeasures $observedImageMeasures $LogFile $ModuleName

    b2 destroy image $a2
    b2 destroy image $pd
    b2 destroy mask $mh



    #step  7:  Find the cerebrum of the standard workup using the existing .gts files.
    #          Stores it in the FreeSurfer frame of reference.



    b2 hide viewer surf
    set a0 [b2 load Image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_T1.hdr data-type= unsigned-8bit]
    set s0 [b2 load GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_l_dec35.gts]
    b2 hide GTSurface $s0
    set s1 [b2 load GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_r_dec35.gts]
    b2 hide GTSurface $s1
    set cr0 [b2 create gts-rois $s0 cor name= l_gts_cor]
    set cr1 [b2 create gts-rois $s1 cor name= r_gts_cor]
    set cm0 [b2 convert ROI to Mask $cr0]
    set cm1 [b2 convert ROI to Mask $cr1]
    set ar0 [b2 create gts-rois $s0 axi name= l_gts_axi]
    set ar1 [b2 create gts-rois $s1 axi name= r_gts_axi]
    set am0 [b2 convert ROI to Mask $ar0]
    set am1 [b2 convert ROI to Mask $ar1]
    set sr0 [b2 create gts-rois $s0 sag name= l_gts_sag]
    set sr1 [b2 create gts-rois $s1 sag name= r_gts_sag]
    set sm0 [b2 convert ROI to Mask $sr0]
    set sm1 [b2 convert ROI to Mask $sr1]
    set m0 [b2 and masks $cm0 $am0 $sm0]
    set m1 [b2 and masks $cm1 $am1 $sm1]
    set m2 [b2 dilate mask $m0 1]
    set m3 [b2 dilate mask $m1 1]
    set m6 [b2 not mask $m2]
    set m7 [b2 not mask $m3]
    set m4 [b2 and masks $m2 $m7]
    set m5 [b2 and masks $m3 $m6]
    set m2a [b2 dilate mask $m4 1]
    set m3a [b2 dilate mask $m5 1]
    set m6a [b2 not mask $m2a]
    set m7a [b2 not mask $m3a]
    set m8 [b2 and masks $m2a $m7a]
    set m9 [b2 and masks $m3a $m6a]
    set r2 [b2 load ROI ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/crbl_gross_cut.xroi]
    set m10 [b2 convert ROI to Mask $r2]
    set m11 [b2 dilate mask $m10 2]
    set m12 [b2 not mask $m11]
    set m13 [b2 and masks $m8 $m12]
    set m14 [b2 and masks $m9 $m12]
    set m15 [b2 or masks $m13 $m14]

    set m15a [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_bilateral_cerebrum.mask]
    set m15b [b2 xor masks $m15 $m15a]
    set results [b2 measure volume mask $m15b]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "bilateral_cerebrum.mask" 0.001 $knownResults $results $LogFile $ModuleName

    set m13a [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_cerebrum.mask]
    set m13b [b2 xor masks $m13 $m13a]
    set results [b2 measure volume mask $m13b]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "left_cerebrum.mask" 0.001 $knownResults $results $LogFile $ModuleName

    set m14a [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_cerebrum.mask]
    set m14b [b2 xor masks $m14 $m14a]
    set results [b2 measure volume mask $m14b]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "right_cerebrum.mask" 0.001 $knownResults $results $LogFile $ModuleName

    b2 destroy image $a0
    b2 destroy GTSurface $s0
    b2 destroy GTSurface $s1
    b2 destroy roi $cr0
    b2 destroy roi $cr1
    b2 destroy Mask $cm0
    b2 destroy Mask $cm1
    b2 destroy roi $ar0
    b2 destroy roi $ar1
    b2 destroy Mask $am0
    b2 destroy Mask $am1
    b2 destroy roi $sr0
    b2 destroy roi $sr1
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
    b2 destroy roi $r2
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



    set mlc [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_caud_cut.mask]
    set mrc [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_caud_cut.mask]
    set mlp [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_put_cut.mask]
    set mrp [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_put_cut.mask]
    set mlg [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_globus_cut.mask]
    set mrg [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_globus_cut.mask]
    set mlt [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_thal_cut.mask]
    set mrt [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_thal_cut.mask]
    set mlbg [b2 or masks $mlc $mlp $mlg $mlt]
    set mrbg [b2 or masks $mrc $mrp $mrg $mrt]

    set mlbgy [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_basal_ganglia.mask]
    set mlbgz [b2 xor masks $mlbg $mlbgy]
    set results [b2 measure volume mask $mlbgz]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "left_basal_ganglia.mask" 0.001 $knownResults $results $LogFile $ModuleName

    set mrbgy [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_basal_ganglia.mask]
    set mrbgz [b2 xor masks $mrbg $mrbgy]
    set results [b2 measure volume mask $mrbgz]
    puts "set knownResults {"
    puts "$results"
    puts "}"
    set knownResults {
    {CubicCentimeters 0.000000000000000}
    }
    CoreMeasuresEpsilonTest "right_basal_ganglia.mask" 0.001 $knownResults $results $LogFile $ModuleName

    b2 destroy mask $mlc
    b2 destroy mask $mrc
    b2 destroy mask $mlp
    b2 destroy mask $mrp
    b2 destroy mask $mlg
    b2 destroy mask $mrg
    b2 destroy mask $mlt
    b2 destroy mask $mrt
    b2 destroy mask $mlbg
    b2 destroy mask $mrbg
    b2 destroy mask $mlbgy
    b2 destroy mask $mlbgz
    b2 destroy mask $mrbgy
    b2 destroy mask $mrbgz



    #step 15:  Generate and measure the brains2 gray matter ribbons for the FreeSurfer parcels.



    set family 0
    set classImg [b2 load Image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_stereo.hdr data-type= unsigned-8bit]
    set m0 [b2 load Mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_bilateral_cerebrum.mask]
    set m1 [b2 dilate Mask $m0 5]
    b2 destroy mask $m0
    set g0 [b2 load GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/FreeSurfer_Subjects/Parcellate_ANON002/surf/rh.orig filter= FreeSurfer]
    b2 color GTSurface labels $g0 $family
    set g1 [b2 load GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/FreeSurfer_Subjects/Parcellate_ANON002/surf/lh.orig filter= FreeSurfer]
    b2 color GTSurface labels $g1 $family
    set im1 [b2 convert GTSurface-set to code-image $m1 $family [list $g0 $g1]]
    b2 destroy mask $m1
    b2 destroy gtsurface $g0
    b2 destroy gtsurface $g1

    b2 save image ${OUTPUT_DIR}/Parcellate_ANON002_orig_bilateral_cortical_parcels.hdr strictAnalyze75 $im1 data-type= S16
    if {0==1} {

        # The very fact that there are just a few flecks of unreliability from run to run mostly on slice y=128 is tough to explain.
        #
        set im1y [b2 load image ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/Parcellate_ANON002_orig_bilateral_cortical_parcels.hdr data-type= S16]
        set im1z [b2 subtract images [list $im1 $im1y]]
        b2 destroy image $im1y
        ReportTestStatus $LogFile  [ expr {[b2 image min $im1z] == 0.0 } ] $ModuleName "Min bilateral_cortical_parcels difference should be zero."
        ReportTestStatus $LogFile  [ expr {[b2 image max $im1z] == 0.0 } ] $ModuleName "Max bilateral_cortical_parcels difference should be zero."
        b2 destroy image $im1z
    }

    set pairs [b2 convert code-image to mask-set $im1 standard]
    b2 destroy image $im1

    set left_cerebrum [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_cerebrum.mask]
    set right_cerebrum [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_cerebrum.mask]
    foreach t {1 2 3 4 5} {
        set left_cerebrum_dilated [b2 dilate Mask $left_cerebrum 1]
        set right_cerebrum_dilated [b2 dilate Mask $right_cerebrum 1]
        set left_non_prev [b2 not mask $left_cerebrum]
        set right_non_prev [b2 not mask $right_cerebrum]
        b2 destroy mask $left_cerebrum
        b2 destroy mask $right_cerebrum
        set left_cerebrum [b2 and masks $left_cerebrum_dilated $right_non_prev]
        set right_cerebrum [b2 and masks $right_cerebrum_dilated $left_non_prev]
        b2 destroy mask $left_cerebrum_dilated
        b2 destroy mask $right_cerebrum_dilated
        b2 destroy mask $left_non_prev
        b2 destroy mask $right_non_prev
    }
    set left_non_prev [b2 not mask $left_cerebrum]
    set right_non_prev [b2 not mask $right_cerebrum]
    set left_hemi [b2 and masks $left_cerebrum $right_non_prev]
    set right_hemi [b2 and masks $right_cerebrum $left_non_prev]
    b2 destroy mask $left_cerebrum
    b2 destroy mask $right_cerebrum
    b2 destroy mask $left_non_prev
    b2 destroy mask $right_non_prev

    set l_basal_ganglia [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_left_basal_ganglia.mask]
    set r_basal_ganglia [b2 load mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_right_basal_ganglia.mask]
    set l_basal_ganglia_roi [b2 convert Mask to ROI Y $l_basal_ganglia skip= 1 minimum-overlap= 0.00]
    set r_basal_ganglia_roi [b2 convert Mask to ROI Y $r_basal_ganglia skip= 1 minimum-overlap= 0.00]
    set l_basal_ganglia_hull_roi [b2 create roi-hull $l_basal_ganglia_roi 2D]
    set r_basal_ganglia_hull_roi [b2 create roi-hull $r_basal_ganglia_roi 2D]
    set lbg [b2 convert roi to mask $l_basal_ganglia_hull_roi]
    set rbg [b2 convert roi to mask $r_basal_ganglia_hull_roi]
    b2 destroy mask $l_basal_ganglia
    b2 destroy mask $r_basal_ganglia
    b2 destroy roi $l_basal_ganglia_roi
    b2 destroy roi $r_basal_ganglia_roi
    b2 destroy roi $l_basal_ganglia_hull_roi
    b2 destroy roi $r_basal_ganglia_hull_roi

    set bg [b2 or masks $lbg $rbg]
    set no_bg [b2 not mask $bg]
    set left_hemisphere [b2 and masks $left_hemi $no_bg]
    set right_hemisphere [b2 and masks $right_hemi $no_bg]
    b2 destroy mask $lbg
    b2 destroy mask $rbg
    b2 destroy mask $bg
    b2 destroy mask $no_bg
    b2 destroy mask $left_hemi
    b2 destroy mask $right_hemi

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
    set left_130 [b2 load GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_l_dec35.gts]
    set right_130 [b2 load GTSurface ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/ANON002_130_r_dec35.gts]
    foreach pair $pairs {
      set name "[lindex $pair 0]"
      set bilateral_mask [lindex $pair 1]
      set gts_gm_mask [b2 convert mask to GTSurface-gm-mask 5 $classImg $bilateral_mask ${left_130} ${right_130}]
      set left_gm_mask [b2 and masks $gts_gm_mask $left_hemisphere]
      set right_gm_mask [b2 and masks $gts_gm_mask $right_hemisphere]
    #  b2 save mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/l_gm_${name}.mask brains2 $left_gm_mask
    #  b2 save mask ${pathToRegressionDir}/SGI/MR/FreeSurfer/TEST/10_ACPC/r_gm_${name}.mask brains2 $right_gm_mask
    #  lappend leftMasksInOrder $left_gm_mask
    #  lappend rightMasksInOrder $right_gm_mask
      lappend colorsInOrder [b2 get mask color [lindex $pair 1]]

      set volume [b2 measure volume mask $left_gm_mask]
      set result [b2 measure GTSurface mask $bilateral_mask ${left_130}]
      # This is bilateral_mask because for GTSurface measures, parcel code space is what counts.
      set report [list [list ParcelRegionName $name] [list Hemisphere Left] [list GrayMatterVolume [lindex [lindex $volume 0] 1]]]
      lappend report [lindex $result $TotalSumArea]
      lappend report [lindex $result $FundalMeanDepth]
      lappend report [lindex $result $GyralMeanDepth]
      lappend report [lindex $result $TotalMeanDepth ]
      lappend report [lindex $result $FundalMeanCurvature]
      lappend report [lindex $result $GyralMeanCurvature]
      lappend measurements [list $report]

      set volume [b2 measure volume mask $right_gm_mask]
      set result [b2 measure GTSurface mask $bilateral_mask ${right_130}]
      # This is bilateral_mask because for GTSurface measures, parcel code space is what counts.
      set report [list [list ParcelRegionName $name] [list Hemisphere Right] [list GrayMatterVolume [lindex [lindex $volume 0] 1]]]
      lappend report [lindex $result $TotalSumArea]
      lappend report [lindex $result $FundalMeanDepth]
      lappend report [lindex $result $GyralMeanDepth]
      lappend report [lindex $result $TotalMeanDepth ]
      lappend report [lindex $result $FundalMeanCurvature]
      lappend report [lindex $result $GyralMeanCurvature]
      lappend measurements [list $report]

#      b2 destroy mask $bilateral_mask
      b2 destroy mask $gts_gm_mask
      b2 destroy mask $left_gm_mask
      b2 destroy mask $right_gm_mask
    }
    foreach pair $pairs {
      set bilateral_mask [lindex $pair 1]
      b2 destroy mask $bilateral_mask
    }

    b2 destroy image $classImg
    b2 destroy gtsurface $left_130
    b2 destroy gtsurface $right_130
    b2 destroy mask $left_hemisphere
    b2 destroy mask $right_hemisphere

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

