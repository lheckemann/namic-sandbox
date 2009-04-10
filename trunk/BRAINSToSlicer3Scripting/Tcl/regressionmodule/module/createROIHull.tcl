# \author        Greg Harris"
# \date
# \brief        b2_create_roi-hull
# \fn                proc createROIHull {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# 'b2_create_roi-hull' -- b2 invalid arguments
# 'b2_create_roi-hull' -- run without error signal
# construction of basal ganglia hulls and grand unified hull
# expected measures
#
# To Do
#------------------------------------------------------------------------
# hull roi dims and res
#
# To Test the Test
# -----------------------------------------------------------------------


#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl


proc createROIHull {pathToRegressionDir dateString} {

    set ModuleName "createROIHull"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2_create_roi-hull"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests

        set mask1 [b2_load_mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/masks/l_caudate_cut.mask"]
        set mask2 [b2_load_mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/masks/l_putamen_cut.mask"]
        set mask3 [b2_load_mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/masks/l_thal_gross_cut.mask"]
        set mask4 [b2_load_mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/masks/l_globus_cut.mask"]
    set core_mask [b2_or_masks $mask1 $mask2 $mask3 $mask4]
    set roi_Y [b2_convert_mask_to_roi y $core_mask name= basal_Y]
    set roi_Z [b2_convert_mask_to_roi z $core_mask name= basal_Z]
    set roi_X [b2_convert_mask_to_roi x $core_mask name= basal_X]


        set num_ret [b2_create_roi-hull]
        set SubTestDes "required argument test: \[b2_create_roi-hull\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2_create_roi-hull $roi_Y 2D junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2_create_roi-hull $roi_Y 2D junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


        set hull_Y [b2_create_roi-hull $roi_Y 2D]
        set SubTestDes "correct response test: \[b2_create_roi-hull <roi-in-Y> 2D\]"
        if {[ReportTestStatus $LogFile  [ expr {$hull_Y >= 0} ] $ModuleName $SubTestDes] == 0} {
}

        set hull_Z [b2_create_roi-hull $roi_Z 2D]
        set SubTestDes "correct response test: \[b2_create_roi-hull <roi-in-Z> 2D\]"
        if {[ReportTestStatus $LogFile  [ expr {$hull_Z >= 0} ] $ModuleName $SubTestDes] == 0} {
}

        set hull_X [b2_create_roi-hull $roi_X 2D]
        set SubTestDes "correct response test: \[b2_create_roi-hull <roi-in-X> 2D\]"
        if {[ReportTestStatus $LogFile  [ expr {$hull_X >= 0} ] $ModuleName $SubTestDes] == 0} {
}

        if {($hull_Y >= 0) && ($hull_Z >= 0) && ($hull_X >= 0)} {

        set mask_Y [b2_convert_roi_to_mask $hull_Y]
        set mask_Z [b2_convert_roi_to_mask $hull_Z]
        set mask_X [b2_convert_roi_to_mask $hull_X]

                set dim_ret [b2_get_dims_mask $mask_Y]
                set SubTestDes "hull Y num dims test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Y dim\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Y dim\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Y dim\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 192 } ] $ModuleName $SubTestDes] == 0} {
}

                set test_res [b2_get_res_mask $mask_Y]

                set SubTestDes "hull Y num res test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Y res\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Y res\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Y res\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set dim_ret [b2_get_dims_mask $mask_Z]

                set SubTestDes "hull Z num dims test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Z dim\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Z dim\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Z dim\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 192 } ] $ModuleName $SubTestDes] == 0} {
}

                set test_res [b2_get_res_mask $mask_Z]

                set SubTestDes "hull Z num res test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Z res\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Z res\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull Z res\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set dim_ret [b2_get_dims_mask $mask_X]

                set SubTestDes "hull X num dims test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull X dim\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull X dim\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == 256 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull X dim\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == 192 } ] $ModuleName $SubTestDes] == 0} {
}

                set test_res [b2_get_res_mask $mask_X]

                set SubTestDes "hull X num res test"
                if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull X res\[0\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull X res\[1\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}

                set SubTestDes "hull X res\[2\] test"
                if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == 1.015625 } ] $ModuleName $SubTestDes] == 0} {
}


        set grand_hull [b2_or_masks $mask_Y $mask_Z $mask_X]
        set infra_hull [b2_and_masks $mask_Y $mask_Z $mask_X]
        set meas_tbl [b2_measure_volume_mask $grand_hull]
        set grand_volume [lindex [lindex $meas_tbl 0] 1]
        set meas_tbl [b2_measure_volume_mask $infra_hull]
        set infra_volume [lindex [lindex $meas_tbl 0] 1]

              if {[SingleMeasureEpsilonTest "outer union of hulls volume" 0.00001 $grand_volume 25.553333282470703 $LogFile $ModuleName] == 0} {
}

              if {[SingleMeasureEpsilonTest "inner intersection of hulls volume" 0.00001 $infra_volume 20.937558174133301 $LogFile $ModuleName] == 0} {
}

                set SubTestDes "inner volume < outer volume test"
                if {[ReportTestStatus $LogFile  [ expr {$infra_volume < $grand_volume } ] $ModuleName $SubTestDes] == 0} {
}

# Clean up Files

# Free memory
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi ${hull_Y} ] != -1 } ] $ModuleName "Destroying roi ${hull_Y}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi ${hull_Z} ] != -1 } ] $ModuleName "Destroying roi ${hull_Z}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi ${hull_X} ] != -1 } ] $ModuleName "Destroying roi ${hull_X}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${mask_Y} ] != -1 } ] $ModuleName "Destroying mask ${mask_Y}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${mask_Z} ] != -1 } ] $ModuleName "Destroying mask ${mask_Z}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${mask_X} ] != -1 } ] $ModuleName "Destroying mask ${mask_X}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${grand_hull} ] != -1 } ] $ModuleName "Destroying mask ${grand_hull}"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${infra_hull} ] != -1 } ] $ModuleName "Destroying mask ${infra_hull}"

        }
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $mask1 ] != -1 } ] $ModuleName "Destroying mask $mask1"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $mask2 ] != -1 } ] $ModuleName "Destroying mask $mask2"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $mask3 ] != -1 } ] $ModuleName "Destroying mask $mask3"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $mask4 ] != -1 } ] $ModuleName "Destroying mask $mask4"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $core_mask ] != -1 } ] $ModuleName "Destroying mask $core_mask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $roi_Y ] != -1 } ] $ModuleName "Destroying roi $roi_Y"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $roi_Z ] != -1 } ] $ModuleName "Destroying roi $roi_Z"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $roi_X ] != -1 } ] $ModuleName "Destroying roi $roi_X"

        return [ StopModule  $LogFile $ModuleName ]
}

