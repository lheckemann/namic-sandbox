# \author        Greg Harris"/
# \date
# \brief        b2 thin roi
# \fn                proc thinROI {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 thin roi tested in x, y, and z separately.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc thinROI {pathToRegressionDir dateString} {
    set ModuleName "thinROI"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 thin roi"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]

# Run Tests

    set tal_brain_mask [b2 load mask "${pathToRegressionDir}/SGI/MR/b2-parcellation/TEST/10_ACPC/tal_brain.mask"]
    set tal_brain_zroi [b2 convert mask to roi Coronal ${tal_brain_mask} name= ROICoronal]

    set num_ret [b2 thin roi]
    set SubTestDes "required argument test: \[b2 thin roi\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2 thin roi ${tal_brain_zroi} junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 thin roi ${tal_brain_zroi} junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set orig_dims [b2 get dims mask ${tal_brain_mask}]
    set orig_res [b2 get res mask ${tal_brain_mask}]


    set z_ret [b2 thin roi ${tal_brain_zroi}]
    set SubTestDes "correct response test: \[b2 thin roi <brain-zroi>\]"
    if {[ReportTestStatus $LogFile  [ expr { $z_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims roi ${z_ret}]
    set SubTestDes "thinned roi test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res roi ${z_ret}]
    set SubTestDes "thinned roi test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set tmp [b2 convert roi to mask $tal_brain_zroi name= MaskCoronal]
    set meas_tbl [b2 measure volume mask $tmp]
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tmp ] != -1 } ] $ModuleName "Destroying mask $tmp"
    set tmp [b2 convert roi to mask $z_ret name= CoronalRet]
    set meas_tab [b2 measure volume mask $tmp]
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tmp ] != -1 } ] $ModuleName "Destroying mask $tmp"
puts $LogFile " - - - - - -:"
puts $LogFile "Measurements:    z vol [lindex [lindex $meas_tbl 0] 1]   thin z vol [lindex [lindex $meas_tab 0] 1]"
    set SubTestDes "thin z roi volume  close enough to  z roi volume test"
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tab 0] 1] - [lindex [lindex $meas_tbl 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}


    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $z_ret ] != -1 } ] $ModuleName "Destroying roi $z_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $tal_brain_zroi ] != -1 } ] $ModuleName "Destroying roi $tal_brain_zroi"
    set tal_brain_yroi [b2 convert mask to roi Axial ${tal_brain_mask} name= ROIAxial]
    set y_ret [b2 thin roi ${tal_brain_yroi}]
    set SubTestDes "correct response test: \[b2 thin roi <brain-yroi>\]"
    if {[ReportTestStatus $LogFile  [ expr { $y_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims roi ${y_ret}]
    set SubTestDes "thinned roi test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res roi ${y_ret}]
    set SubTestDes "thinned roi test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set tmp [b2 convert roi to mask $tal_brain_yroi name= MaskAxial]
    set meas_tbl [b2 measure volume mask $tmp]
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tmp ] != -1 } ] $ModuleName "Destroying mask $tmp"
    set tmp [b2 convert roi to mask $y_ret name= AxialRet]
    set meas_tab [b2 measure volume mask $tmp]
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tmp ] != -1 } ] $ModuleName "Destroying mask $tmp"
puts $LogFile " - - - - - -:"
puts $LogFile "Measurements:    y vol [lindex [lindex $meas_tbl 0] 1]   thin y vol [lindex [lindex $meas_tab 0] 1]"
    set SubTestDes "thin y roi volume  close enough to  y roi volume test"
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tab 0] 1] - [lindex [lindex $meas_tbl 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}


    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $y_ret ] != -1 } ] $ModuleName "Destroying roi $y_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $tal_brain_yroi ] != -1 } ] $ModuleName "Destroying roi $tal_brain_yroi"
    set tal_brain_xroi [b2 convert mask to roi Sagittal ${tal_brain_mask} name= ROISagittal]
    set x_ret [b2 thin roi ${tal_brain_xroi}]
    set SubTestDes "correct response test: \[b2 thin roi <brain-xroi>\]"
    if {[ReportTestStatus $LogFile  [ expr { $x_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims roi ${x_ret}]
    set SubTestDes "thinned roi test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res roi ${x_ret}]
    set SubTestDes "thinned roi test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set tmp [b2 convert roi to mask $tal_brain_xroi name= MaskSagittal]
    set meas_tbl [b2 measure volume mask $tmp]
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tmp ] != -1 } ] $ModuleName "Destroying mask $tmp"
    set tmp [b2 convert roi to mask $x_ret name= SagittalRet]
    set meas_tab [b2 measure volume mask $tmp]
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tmp ] != -1 } ] $ModuleName "Destroying mask $tmp"
puts $LogFile " - - - - - -:"
puts $LogFile "Measurements:    x vol [lindex [lindex $meas_tbl 0] 1]   thin x vol [lindex [lindex $meas_tab 0] 1]"
    set SubTestDes "thin x roi volume  close enough to  x roi volume test"
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}
    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tab 0] 1] - [lindex [lindex $meas_tbl 0] 1]) < 1.0 } ] $ModuleName $SubTestDes] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $x_ret ] != -1 } ] $ModuleName "Destroying roi $x_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $tal_brain_xroi ] != -1 } ] $ModuleName "Destroying roi $tal_brain_xroi"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tal_brain_mask ] != -1 } ] $ModuleName "Destroying mask $tal_brain_mask"

    return [ StopModule  $LogFile $ModuleName ]
}

