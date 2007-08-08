# \author        Greg Harris"
# \date
# \brief        b2 clean mask
# \fn                proc cleanMask {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 clean mask tested in x, y, and z separately.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc cleanMask {pathToRegressionDir dateString} {
    set ModuleName "cleanMask"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 clean mask"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


# Run Tests


    set tal_brain_mask [b2 load mask "${pathToRegressionDir}/SGI/MR/b2-parcellation/TEST/10_ACPC/tal_brain.mask"]
    set orig_dims [b2 get dims mask ${tal_brain_mask}]
    set orig_res [b2 get res mask ${tal_brain_mask}]

    set m_fore [b2 split mask ${tal_brain_mask} z 83 +]
    set m_aft [b2 split mask ${tal_brain_mask} z 82 -]
    set z_split_mask [b2 or masks ${m_fore} ${m_aft}]

    set m_up [b2 split mask ${tal_brain_mask} y 74 +]
    set m_dn [b2 split mask ${tal_brain_mask} y 73 -]
    set y_split_mask [b2 or masks ${m_up} ${m_dn}]


    set num_ret [b2 clean mask]
    set SubTestDes "required argument test: \[b2 clean mask\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2 clean mask ${z_split_mask} junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 clean mask ${z_split_mask} junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set z_ret [b2 clean mask ${z_split_mask}]
    set SubTestDes "correct response test: \[b2 clean mask <brain-mask-split-z>\]"
    if {[ReportTestStatus $LogFile  [ expr { $z_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $m_fore]
    set meas_tab [b2 measure volume mask $z_ret]
puts $LogFile " - - - - - -:"
puts $LogFile "Measurements:    z fore [lindex [lindex $meas_tbl 0] 1]   clean z split [lindex [lindex $meas_tab 0] 1]"
    set SubTestDes "z fore volume  close enough to  clean z split volume test"
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims mask ${z_ret}]
    set SubTestDes "clean mask test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res mask ${z_ret}]
    set SubTestDes "clean mask test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}


    set y_ret [b2 clean mask ${y_split_mask} size-thresh= 1 x-loc= 106 y-loc= 63 z-loc= 60]
    set SubTestDes "correct response test: \[b2 clean mask <brain-mask-split-y>\]"
    if {[ReportTestStatus $LogFile  [ expr { $y_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $m_dn]
    set meas_tab [b2 measure volume mask $y_ret]
puts $LogFile " - - - - - -:"
puts $LogFile "Measurements:    y down [lindex [lindex $meas_tbl 0] 1]   clean y split [lindex [lindex $meas_tab 0] 1]"
    set SubTestDes "y down volume  close enough to  clean y split volume test"
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}


    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $y_ret ] != -1 } ] $ModuleName "Destroying mask $y_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $y_split_mask ] != -1 } ] $ModuleName "Destroying mask $y_split_mask"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m_up ] != -1 } ] $ModuleName "Destroying mask $m_up"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m_dn ] != -1 } ] $ModuleName "Destroying mask $m_dn"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $z_ret ] != -1 } ] $ModuleName "Destroying mask $z_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $z_split_mask ] != -1 } ] $ModuleName "Destroying mask $z_split_mask"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m_fore ] != -1 } ] $ModuleName "Destroying mask $m_fore"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m_aft ] != -1 } ] $ModuleName "Destroying mask $m_aft"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tal_brain_mask ] != -1 } ] $ModuleName "Destroying mask $tal_brain_mask"

    return [ StopModule  $LogFile $ModuleName ]
}

