# \author        Greg Harris"
# \date
# \brief        b2_convert_talairach-mask_to_mask
#        b2_convert_mask_to_talairach-mask
#        ????b2_convert_talairach-box_to_mask
# \fn                proc convertTalairachMask {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
#
# To Do
#------------------------------------------------------------------------
# b2_convert_talairach-box_to_mask
#
# To Test the Test
# -----------------------------------------------------------------------


proc convertTalairachMask {pathToRegressionDir dateString} {
    set ModuleName "convertTalairachMask"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2_convert_talairach-mask_to_mask to talairach-mask"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#    close $LogFile
#    set LogFile stderr




# Run Tests

#     set img [b2_load_image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_T1.hdr"]
    set tal_par [b2_load_talairach-parameters "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd"]
    set tal_mask [b2_load_mask "${pathToRegressionDir}/SGI/MR/b2-parcellation/TEST/10_ACPC/tal_brain.mask"]

    set tal_dims [b2_get_dims_mask ${tal_mask}]
    set tal_res [b2_get_res_mask ${tal_mask}]


        set num_ret [b2_convert_talairach-mask_to_mask]
        set SubTestDes "required argument test: \[b2_convert_talairach-mask_to_mask\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2_convert_talairach-mask_to_mask $tal_mask $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2_convert_talairach-mask_to_mask $tal_mask $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "correct response test: \[b2_convert_talairach-mask_to_mask <tal_mask> <tal_par>\]"
    set acq_mask [b2_convert_talairach-mask_to_mask $tal_mask $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$acq_mask >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set acq_dims [b2_get_dims_mask ${acq_mask}]
    set acq_res [b2_get_res_mask ${acq_mask}]


        set num_ret [b2_convert_mask_to_talairach-mask]
        set SubTestDes "required argument test: \[b2_convert_mask_to_talairach-mask\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2_convert_mask_to_talairach-mask $acq_mask $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2_convert_mask_to_talairach-mask $acq_mask $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "correct response test: \[b2_convert_mask_to_talairach-mask <acq_mask> <tal_par>\]"
    set idem_tal_mask [b2_convert_mask_to_talairach-mask $acq_mask $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$idem_tal_mask >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set idem_tal_dims [b2_get_dims_mask ${idem_tal_mask}]
    set idem_tal_res [b2_get_res_mask ${idem_tal_mask}]

    set SubTestDes "idem tal mask test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${idem_tal_dims} ${tal_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "idem tal mask test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${idem_tal_res} ${tal_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}


        set SubTestDes "correct response test: \[b2_convert_talairach-mask_to_mask <idem_tal_mask> <tal_par>\]"
    set idem_acq_mask [b2_convert_talairach-mask_to_mask $idem_tal_mask $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$idem_acq_mask >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set idem_acq_dims [b2_get_dims_mask ${idem_acq_mask}]
    set idem_acq_res [b2_get_res_mask ${idem_acq_mask}]

    set SubTestDes "idem acq mask test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${idem_acq_dims} ${acq_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "idem acq mask test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${idem_acq_res} ${acq_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}


    if {($idem_tal_mask >=0) && ($idem_acq_mask >= 0)} {

        set meas_tbl_1 [b2_measure_volume_mask $tal_mask]
        puts ${meas_tbl_1}
        set meas_tbl_2 [b2_measure_volume_mask $acq_mask]
        puts ${meas_tbl_2}
        set meas_tbl_3 [b2_measure_volume_mask $idem_tal_mask]
        puts ${meas_tbl_3}
        set meas_tbl_4 [b2_measure_volume_mask $idem_acq_mask]
        puts ${meas_tbl_4}

                set tal_drift_mask [b2_xor_masks $tal_mask $idem_tal_mask]
                set acq_drift_mask [b2_xor_masks $acq_mask $idem_acq_mask]
        set meas_tbl_5 [b2_measure_volume_mask $tal_drift_mask]
        puts ${meas_tbl_5}
        set meas_tbl_6 [b2_measure_volume_mask $acq_drift_mask]
        puts ${meas_tbl_6}

        puts $LogFile " - - - - - -:"
        puts $LogFile "Measurements:    tal volume [lindex [lindex $meas_tbl_1 0] 1]   idem tal volume [lindex [lindex $meas_tbl_3 0] 1]"

        set tal_enlar [ expr {(([lindex [lindex $meas_tbl_3 0] 1] - [lindex [lindex $meas_tbl_1 0] 1]) / [lindex [lindex $meas_tbl_1 0] 1]) } ]
        puts $LogFile "                 tal-to-tal enlargement ${tal_enlar}"
        set SubTestDes "tal volume  close enough to  idem tal volume test"
        if {[ReportTestStatus $LogFile  [ expr { ${tal_enlar} < 0.005 } ] $ModuleName $SubTestDes] == 0} {
}

        set tal_drift [ expr {([lindex [lindex $meas_tbl_5 0] 1] / [lindex [lindex $meas_tbl_1 0] 1]) } ]
        puts $LogFile "                 tal-to-tal drift ${tal_drift}"
        set SubTestDes "tal volume drift  close enough to zero test"
        if {[ReportTestStatus $LogFile  [ expr { ${tal_drift} < 0.005 } ] $ModuleName $SubTestDes] == 0} {
}


        puts $LogFile " - - - - - -:"
        puts $LogFile "Measurements:    acq volume [lindex [lindex $meas_tbl_2 0] 1]   idem acq volume [lindex [lindex $meas_tbl_4 0] 1]"

        set acq_enlar [ expr {(([lindex [lindex $meas_tbl_4 0] 1] - [lindex [lindex $meas_tbl_2 0] 1]) / [lindex [lindex $meas_tbl_2 0] 1]) } ]
        puts $LogFile "                 acq-to-acq enlargement ${acq_enlar}"
        set SubTestDes "acq volume  close enough to  idem acq volume test"
        if {[ReportTestStatus $LogFile  [ expr { ${acq_enlar} < 0.005 } ] $ModuleName $SubTestDes] == 0} {
}

        set acq_drift [ expr {([lindex [lindex $meas_tbl_6 0] 1] / [lindex [lindex $meas_tbl_2 0] 1]) } ]
        puts $LogFile "                 acq-to-acq drift ${acq_drift}"
        set SubTestDes "acq volume drift  close enough to zero test"
        if {[ReportTestStatus $LogFile  [ expr { ${acq_drift} < 0.005 } ] $ModuleName $SubTestDes] == 0} {
}


# Clean up Files
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $tal_drift_mask ] != -1 } ] $ModuleName "Destroying mask $tal_drift_mask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $acq_drift_mask ] != -1 } ] $ModuleName "Destroying mask $acq_drift_mask"

# Free memory

        }
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $tal_par ] != -1 } ] $ModuleName "Destroying talairach-parameters $tal_par"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $tal_mask ] != -1 } ] $ModuleName "Destroying mask $tal_mask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $acq_mask ] != -1 } ] $ModuleName "Destroying mask $acq_mask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $idem_tal_mask ] != -1 } ] $ModuleName "Destroying mask $idem_tal_mask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $idem_acq_mask ] != -1 } ] $ModuleName "Destroying mask $idem_acq_mask"

        return [ StopModule  $LogFile $ModuleName ]
}

