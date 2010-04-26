# \author    Greg Harris"
# \date
# \brief    The command to extract a subset of an image is b2_create_bounded-image.
# \fn        proc createBoundedImage {pathToRegressionDir dateString {GT Test}}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString        - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# 'b2_create_bounded-image' -- b2 invalid arguments
# 'b2_create_bounded-image' -- run without error signal
# resulting image's parameters.
# transform checking by image subtraction.
# threshold and mask volumes.
#
# To Do
#------------------------------------------------------------------------
#
# To Establish the Test's standard
# -----------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl

proc createBoundedImage {pathToRegressionDir dateString {GT Test}} {

    set ModuleName "createBoundedImage"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2_create_bounded-image"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr



    # Run Tests

    set im1 [b2_load_image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_T1.hdr"]

    set num_ret [b2_create_bounded-image]
    set SubTestDes "required argument test: \[b2_create_bounded-image\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2_create_bounded-image ${im1}]
    set SubTestDes "required argument test: \[b2_create_bounded-image <image>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2_create_bounded-image ${im1} 106 147 115 149 73]
    set SubTestDes "required argument test: \[b2_create_bounded-image <image> 106 147 115 149 73\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2_create_bounded-image ${im1} 106 147 115 149 73 107 5]
    set SubTestDes "required argument test: \[b2_create_bounded-image <image> 106 147 115 149 73 107 5\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set SubTestDes "optional argument number test"
    set num_ret [b2_create_bounded-image ${im1} 106 147 115 149 73 107 junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2_create_bounded-image ${im1} 106 147 115 149 73 107 junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set pair_ret [b2_create_bounded-image ${im1} 106 147 115 149 73 107]
    set SubTestDes "correct response test: \[b2_create_bounded-image <image ANON0009_10_T1> 106 147 115 149 73 107\]"
    if {[ReportTestStatus $LogFile  [ expr {[llength $pair_ret] == 2} ] $ModuleName $SubTestDes] == 0} {
}

    if {[llength $pair_ret] == 2} {
    b2_set_transform -1 image [lindex ${pair_ret} 0]
    set dim_ret [b2_get_dims_image [lindex ${pair_ret} 0]]
    set SubTestDes "num dims test"
    if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "dim\[0\] test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 42 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "dim\[1\] test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 35 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "dim\[2\] test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 35 } ] $ModuleName $SubTestDes] == 0} {
}

    set orig_res [b2_get_res_image ${im1}]
    set test_res [b2_get_res_image [lindex ${pair_ret} 0]]

    set SubTestDes "num res test"
    if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "res\[0\] test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex $orig_res 0] == [lindex $test_res 0] } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "res\[1\] test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex $orig_res 1] == [lindex $test_res 1] } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "res\[2\] test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex $orig_res 2] == [lindex $test_res 2] } ] $ModuleName $SubTestDes] == 0} {
}
        exec mkdir -p "${OUTPUT_DIR}"
    set standardTransformFilename "${OUTPUT_DIR}/${ModuleName}_${GT}.xfrm"

        set check [b2_save_transform $standardTransformFilename brains2 [lindex ${pair_ret} 1]]
            set SubTestDes "save transform test: \[b2_save_transform <filename> brains2 <transform>\]"
            if {[ReportTestStatus $LogFile  [ expr {$check != -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set check [b2_save_image "${OUTPUT_DIR}/${ModuleName}_${GT}.hdr" strictAnalyze75 [lindex ${pair_ret} 0]]
            set SubTestDes "save image test: \[b2_save_image <filename> strictAnalyze75 <image>\]"
            if {[ReportTestStatus $LogFile  [ expr {$check != -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set dup_img [b2_load_image "${OUTPUT_DIR}/${ModuleName}_${GT}.hdr"]
            set SubTestDes "load image test: \[b2_load_image <filename>\]"
            if {[ReportTestStatus $LogFile  [ expr {$dup_img != -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set std_trans [b2_load_transform $standardTransformFilename]
            set SubTestDes "load transform test: \[b2_load_transform <filename>\]"
            if {[ReportTestStatus $LogFile  [ expr {$std_trans  != -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "bounded image min test"
        set imageMin [b2_image_min  $dup_img]
        if {[ReportTestStatus $LogFile  [ expr {$imageMin == 9.000000 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "bounded image max test"
        set imageMax [b2_image_max   $dup_img]
        if {[ReportTestStatus $LogFile  [ expr {$imageMax == 124.000000 } ] $ModuleName $SubTestDes] == 0} {
}


        b2_set_transform [lindex ${pair_ret} 1] image [lindex ${pair_ret} 0]
        b2_set_transform $std_trans image $dup_img

        set diff_img [b2_subtract_images [list [lindex ${pair_ret} 0] $dup_img]]

        set SubTestDes "diff image min zero test"
        set imageMin [b2_image_min $diff_img]
        if {[ReportTestStatus $LogFile  [ expr {$imageMin == 0.000000 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "diff image max zero test"
        set imageMax [b2_image_max $diff_img]
        if {[ReportTestStatus $LogFile  [ expr {$imageMax == 0.000000 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "threshold 94.0 mask volume close to 25.390954 test"
        b2_set_transform -1 image $dup_img
        set thresh_mask [b2_threshold_image $dup_img 94.0]
        set meas_tbl [b2_measure_volume_mask $thresh_mask]
            puts $meas_tbl
        if {[ReportTestStatus $LogFile  [ expr {abs( [lindex [lindex $meas_tbl 0] 1] - 25.390954) < 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        # Clean up Files
        if {[catch { exec rm "${OUTPUT_DIR}/${ModuleName}_${GT}.hdr" } squabble] != 0 } {puts "exec failed: $squabble" }
        if {[catch { exec rm "${OUTPUT_DIR}/${ModuleName}_${GT}.img" } squabble] != 0 } {puts "exec failed: $squabble" }

        # Free memory
         ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${dup_img} ] != -1 } ] $ModuleName "Destroying image ${dup_img}"
         ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform ${std_trans} ] != -1 } ] $ModuleName "Destroying transform ${std_trans}"
         ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${diff_img} ] != -1 } ] $ModuleName "Destroying image ${diff_img}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${thresh_mask} ] != -1 } ] $ModuleName "Destroying mask ${thresh_mask}"

    # Free memory
     ReportTestStatus $LogFile  [ expr { [ b2_destroy_image [lindex ${pair_ret} 0] ] != -1 } ] $ModuleName "Destroying image [lindex ${pair_ret} 0]"
     ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform [lindex ${pair_ret} 1] ] != -1 } ] $ModuleName "Destroying transform [lindex ${pair_ret} 1]"
    }
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${im1} ] != -1 } ] $ModuleName "Destroying image ${im1}"

    return [ StopModule  $LogFile $ModuleName ]
}
