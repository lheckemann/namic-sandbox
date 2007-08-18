# \author        Greg Harris"
# \date
# \brief        b2_create_boxed-image
# \fn                proc createBoxedImage {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# 'b2_create_boxed-image' -- b2 invalid arguments
# 'b2_create_boxed-image' -- run without error signal
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


proc createBoxedImage {pathToRegressionDir dateString} {

    set ModuleName "createBoxedImage"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2_create_boxed-image"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests

        set img1 [b2_create_bullet-image 20 0 256 256 192 1.015625 1.015625 1.015625 120 100 75 8 3 1]
        set img2 [b2_create_bullet-image 18 0 256 256 192 1.015625 1.015625 1.015625 126 108 73 1 2 9]
    set imgN [b2_sum_images [list $img1 $img2]]


        set num_ret [b2_create_boxed-image]
        set SubTestDes "required argument test: \[b2_create_boxed-image\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

        set SubTestDes "optional argument number test"
        set num_ret [b2_create_boxed-image $imgN junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

        set SubTestDes "unknown optional argument test"
        set num_ret [b2_create_boxed-image junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }


        set pair_ret [b2_create_boxed-image $imgN threshold= 1.0]
        set SubTestDes "correct response test: \[b2_create_boxed-image <some-phantom> threshold= 1.0\]"
        if {[ReportTestStatus $LogFile  [ expr {[llength $pair_ret] == 2} ] $ModuleName $SubTestDes] == 0} {
}

        if {[llength $pair_ret] == 2} {
        set img_ret [lindex $pair_ret 0]
        set trans_ret [lindex $pair_ret 1]
        b2_set_transform -1 image $img_ret


                set dim_ret [b2_get_dims_image $img_ret]
                set SubTestDes "num dims test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
                    }

                set SubTestDes "dim\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 10 } ] $ModuleName $SubTestDes] == 0} {
                    }

                set SubTestDes "dim\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 12 } ] $ModuleName $SubTestDes] == 0} {
                    }

                set SubTestDes "dim\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 11 } ] $ModuleName $SubTestDes] == 0} {
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
              if {[SingleMeasureEpsilonTest "boxed image min" 0.00001 $imageMin 0.000000 $LogFile $ModuleName] == 0} {
                    }

        set SubTestDes "boxed image max test"
        set imageMax [b2_image_max   $img_ret]
              if {[SingleMeasureEpsilonTest "boxed image max" 0.00001 $imageMax 20.000000 $LogFile $ModuleName] == 0} {
                    }


        b2_set_transform $trans_ret image $img_ret
        set thresh_mask [b2_threshold_image $img_ret 1.0]
        set meas_tbl [b2_measure_volume_mask $thresh_mask]

              if {[SingleMeasureEpsilonTest "threshold 1.0 mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  0.044000 $LogFile $ModuleName] == 0} {
                    }


#        set meas_tbl [b2_measure_image_mask $thresh_mask $img_ret]
#        set SubTestDes "boxed image measure mean test"
#        if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex $meas_tbl 0] 1] == 244.285714 } ] $ModuleName $SubTestDes] == 0} {
#                   }
#        set SubTestDes "boxed image measure standard deviation test"
#        if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex $meas_tbl 3] 1] == 12.521758 } ] $ModuleName $SubTestDes] == 0} {
#                   }


# Clean up Files

# Free memory
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${img_ret} ] != -1 } ] $ModuleName "Destroying image ${img_ret}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform ${trans_ret} ] != -1 } ] $ModuleName "Destroying transform ${trans_ret}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${thresh_mask} ] != -1 } ] $ModuleName "Destroying mask ${thresh_mask}"

        }
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${img1} ] != -1 } ] $ModuleName "Destroying image ${img1}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${img2} ] != -1 } ] $ModuleName "Destroying image ${img2}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${imgN} ] != -1 } ] $ModuleName "Destroying image ${imgN}"

        return [ StopModule  $LogFile $ModuleName ]
}

