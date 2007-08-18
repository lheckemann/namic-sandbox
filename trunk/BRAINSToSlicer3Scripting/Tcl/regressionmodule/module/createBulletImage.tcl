# \author        Greg Harris"
# \date
# \brief        b2_create_bullet-image
# \fn                proc createBulletImage {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# 'b2_create_bullet-image' -- b2 invalid arguments
# 'b2_create_bullet-image' -- run without error signal
# resulting image's parameters
# threshold and mask: parameters, volumes
# boxed bullet: parameters, volumes
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl


proc createBulletImage {pathToRegressionDir dateString} {

    set ModuleName "createBulletImage"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2_create_bullet-image"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests

        set num_ret [b2_create_bullet-image]
        set SubTestDes "required argument test: \[b2_create_bullet-image\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2_create_bullet-image 200 0 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0]
        set SubTestDes "required argument test: \[b2_create_bullet-image 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2_create_bullet-image 200 0 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0 4.0 5]
        set SubTestDes "required argument test: \[b2_create_bullet-image 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0 4.0 5\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


        set SubTestDes "optional argument number test"
        set num_ret [b2_create_bullet-image 200 0 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0 4.0 junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2_create_bullet-image 200 0 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0 4.0 junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


        set img_ret [b2_create_bullet-image 200 0 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0 4.0]
        set SubTestDes "correct response test: \[b2_create_bullet-image 256 256 192 1.015625 1.015625 1.015625 120 100 75 2.0 1.0 4.0\]"
        if {[ReportTestStatus $LogFile  [ expr {$img_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
}

        if {$img_ret >= 0} {
                set dim_ret [b2_get_dims_image $img_ret]
                set SubTestDes "num dims test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "dim\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "dim\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "dim\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 192 } ] $ModuleName $SubTestDes] == 0} {
}

                set test_res [b2_get_res_image $img_ret]

                set SubTestDes "num res test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "res\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "res\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "res\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}


        set imageMin [b2_image_min  $img_ret]
              if {[SingleMeasureEpsilonTest "bullet image min" 0.00001 $imageMin 0.000000 $LogFile $ModuleName] == 0} {
}

        set imageMax [b2_image_max   $img_ret]
              if {[SingleMeasureEpsilonTest "bullet image max" 0.00001 $imageMax 200.000000 $LogFile $ModuleName] == 0} {
}


        set thresh_mask [b2_threshold_image $img_ret 1.0]
        set meas_tbl [b2_measure_volume_mask $thresh_mask]


                set dim_ret [b2_get_dims_mask $thresh_mask]
                set SubTestDes "thresh mask num dims test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "thresh mask dim\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "thresh mask dim\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "thresh mask dim\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 192 } ] $ModuleName $SubTestDes] == 0} {
}

                set test_res [b2_get_res_mask $thresh_mask]

                set SubTestDes "thresh mask num res test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "thresh mask res\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "thresh mask res\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "thresh mask res\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}


                set SubTestDes "threshold 1.0 mask volume == 0.008381 test"
              if {[SingleMeasureEpsilonTest "threshold 1.0 mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1] 0.008380889892578  $LogFile $ModuleName] == 0} {
}


        set meas_tbl [b2_measure_image_mask $thresh_mask $img_ret]
#puts $meas_tbl
              set standards {
{Mean 200.000000000000000} {Mean-Absolute-Deviation 0.000000000000000} {Variance 0.000000000000000} {Standard-Deviation 0.000000000000000} {Skewness 0.000000000000000} {Kurtosis 0.000000000000000} {Minimum 200.000000000000000} {Maximum 200.000000000000000}
        }
              if {[CoreMeasuresEpsilonTest "bullet image" 0.00001 $standards $meas_tbl  $LogFile $ModuleName] == 0} {
}


        set pair_ret [b2_create_boxed-image $img_ret threshold= 1.0]
        b2_set_transform -1 image [lindex ${pair_ret} 0]


        set dim_ret [b2_get_dims_image [lindex ${pair_ret} 0]]

        set SubTestDes "boxed bullet num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "boxed bullet dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 4 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "boxed bullet dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "boxed bullet dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 6 } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2_get_res_image [lindex ${pair_ret} 0]]

                set SubTestDes "boxed bullet num res test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "boxed bullet res\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "boxed bullet res\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "boxed bullet res\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}


# Clean up Files

# Free memory
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image [lindex ${pair_ret} 0] ] != -1 } ] $ModuleName "Destroying image [lindex ${pair_ret} 0]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform [lindex ${pair_ret} 1] ] != -1 } ] $ModuleName "Destroying transform [lindex ${pair_ret} 1]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${img_ret} ] != -1 } ] $ModuleName "Destroying image ${img_ret}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${thresh_mask} ] != -1 } ] $ModuleName "Destroying mask ${thresh_mask}"

        }

        return [ StopModule  $LogFile $ModuleName ]
}

