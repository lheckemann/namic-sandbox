# \author        Greg Harris"
# \date
# \brief        b2 extrude roi
# \fn                proc extrudeROI {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 extrude roi tested in x, y, and z separately.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc extrudeROI {pathToRegressionDir dateString} {
    set ModuleName "extrudeROI"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 extrude roi"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


# Run Tests
#   set m1 [b2 load mask "${pathToRegressionDir}/SGI/MR/b2-parcellation/TEST/10_ACPC/tal_brain.mask"]
#    set img [b2 load image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_segment.hdr"]
    set m1 [b2 load mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_brain_trim.mask"]
    set orig_dims [b2 get dims mask ${m1}]
    set orig_res [b2 get res mask ${m1}]
#    set m2 [b2 split mask ${m1} y 79 -]
#    set m3 [b2 split mask ${m2} z 83 +]
#    set tal_brain_mask [b2 split mask ${m3} x 82 -]
#    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m1 ] != -1 } ] $ModuleName "Destroying mask $m1"
#    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m2 ] != -1 } ] $ModuleName "Destroying mask $m2"
#    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $m3 ] != -1 } ] $ModuleName "Destroying mask $m3"
    set tal_brain_mask $m1
    set tal_brain_zroi [b2 convert mask to roi Coronal ${tal_brain_mask}]
    set tal_brain_yroi [b2 convert mask to roi Axial ${tal_brain_mask}]
    set tal_brain_xroi [b2 convert mask to roi Sagittal ${tal_brain_mask}]

    set num_ret [b2 extrude roi]
    set SubTestDes "required argument test: \[b2 extrude roi\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set num_ret [b2 extrude roi ${tal_brain_zroi}]
    set SubTestDes "required argument test: \[b2 extrude roi <brain-roi>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set num_ret [b2 extrude roi ${tal_brain_zroi} 0.0 1.0]
    set SubTestDes "required argument test: \[b2 extrude roi <brain-roi> 0.0 1.0\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set num_ret [b2 extrude roi ${tal_brain_zroi} 0.0 1.0 0.0 0.0]
    set SubTestDes "required argument test: \[b2 extrude roi <brain-roi> 0.0 1.0 0.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set SubTestDes "optional argument number test"
    set num_ret [b2 extrude roi ${tal_brain_zroi} 0.0 1.0 0.0 junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 extrude roi ${tal_brain_zroi} 0.0 1.0 0.0 junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }


    set num_ret [b2 extrude roi ${tal_brain_zroi} 0.0 0.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-roi> 0.0 0.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $num_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }



    set yz_ret [b2 extrude roi ${tal_brain_zroi} 0.0 5.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-zroi> 0.0 2.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $yz_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set xz_ret [b2 extrude roi ${tal_brain_zroi} -5.0 0.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-zroi> -2.0 0.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $xz_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set zz_ret [b2 extrude roi ${tal_brain_zroi} 0.0 0.0 5.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-zroi> 0.0 0.0 2.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $zz_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }


    set yy_ret [b2 extrude roi ${tal_brain_yroi} 0.0 5.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-yroi> 0.0 2.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $yy_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set xy_ret [b2 extrude roi ${tal_brain_yroi} -5.0 0.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-yroi> -2.0 0.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $xy_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set zy_ret [b2 extrude roi ${tal_brain_yroi} 0.0 0.0 5.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-yroi> 0.0 0.0 2.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $zy_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }


    set yx_ret [b2 extrude roi ${tal_brain_xroi} 0.0 5.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-xroi> 0.0 2.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $yx_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set xx_ret [b2 extrude roi ${tal_brain_xroi} -5.0 0.0 0.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-xroi> -2.0 0.0 0.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $xx_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set zx_ret [b2 extrude roi ${tal_brain_xroi} 0.0 0.0 5.0]
    set SubTestDes "correct response test: \[b2 extrude roi <brain-xroi> 0.0 0.0 2.0\]"
    if {[ReportTestStatus $LogFile  [ expr { $zx_ret >= 0} ] $ModuleName $SubTestDes] == 0} {
    }

    set mod_dims [b2 get dims mask ${zx_ret}]
    set SubTestDes "extruded mask test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
    }

    set mod_res [b2 get res mask ${zx_ret}]
    set SubTestDes "extruded mask test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
    }

    set meas_tbl [b2 measure volume mask $num_ret]
    set SubTestDes "zero mask volume == 0.000000 test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex $meas_tbl 0] 1] == 0.000000 } ] $ModuleName $SubTestDes] == 0} {
    }

    set meas_tbl [b2 measure volume mask $xx_ret]
    set SubTestDes "xx mask volume == 0.000000 test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex $meas_tbl 0] 1] == 0.000000 } ] $ModuleName $SubTestDes] == 0} {
    }

    set meas_tbl [b2 measure volume mask $yy_ret]
    set SubTestDes "yy mask volume == 0.000000 test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex $meas_tbl 0] 1] == 0.000000 } ] $ModuleName $SubTestDes] == 0} {
    }

    set meas_tbl [b2 measure volume mask $zz_ret]
    set SubTestDes "zz mask volume == 0.000000 test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex $meas_tbl 0] 1] == 0.000000 } ] $ModuleName $SubTestDes] == 0} {
    }

    set meas_tbl [b2 measure volume mask $xy_ret]
puts "---->  xy mask measures $meas_tbl"
    set meas_tab [b2 measure volume mask $xz_ret]
puts "---->  xz mask measures $meas_tab"
#puts $LogFile " - - - - - -:"
#puts $LogFile "Measurements:    x wrt y vol [lindex [lindex $meas_tbl 0] 1]   x wrt z vol [lindex [lindex $meas_tab 0] 1]"
#    set SubTestDes "xy mask volume  close enough to  xz mask volume test"
#    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 0.2 } ] $ModuleName $SubTestDes] == 0} {
#    }
    set standard {{CubicCentimeters 185.645092010498047}}
    if {[CoreMeasuresEpsilonTest "xy mask volume" 0.00001 $standard $meas_tbl $LogFile $ModuleName] == 0} {
    }
    set standard {{CubicCentimeters 185.628330230712891}}
    if {[CoreMeasuresEpsilonTest "xz mask volume" 0.00001 $standard $meas_tab $LogFile $ModuleName] == 0} {
    }

    set meas_tbl [b2 measure volume mask $yx_ret]
puts "---->  yx mask measures $meas_tbl"
    set meas_tab [b2 measure volume mask $yz_ret]
puts "---->  yz mask measures $meas_tab"
#puts $LogFile " - - - - - -:"
#puts $LogFile "Measurements:    y wrt z vol [lindex [lindex $meas_tbl 0] 1]   y wrt x vol [lindex [lindex $meas_tab 0] 1]"
#    set SubTestDes "yx mask volume  close enough to  yz mask volume test"
#    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 0.2 } ] $ModuleName $SubTestDes] == 0} {
#   }
    set standard {{CubicCentimeters 178.085529327392578}}
    if {[CoreMeasuresEpsilonTest "yx mask volume" 0.00001 $standard $meas_tbl $LogFile $ModuleName] == 0} {
    }
    set standard {{CubicCentimeters 178.071910381317139}}
    if {[CoreMeasuresEpsilonTest "yz mask volume" 0.00001 $standard $meas_tab $LogFile $ModuleName] == 0} {
    }

    set meas_tbl [b2 measure volume mask $zx_ret]
puts "---->  zx mask measures $meas_tbl"
    set meas_tab [b2 measure volume mask $zy_ret]
puts "---->  zy mask measures $meas_tab"
#puts $LogFile " - - - - - -:"
#puts $LogFile "Measurements:    z wrt y vol [lindex [lindex $meas_tbl 0] 1]   z wrt x vol [lindex [lindex $meas_tab 0] 1]"
#    set SubTestDes "zx mask volume  close enough to  yx mask volume test"
#    if {[ReportTestStatus $LogFile  [ expr {([lindex [lindex $meas_tbl 0] 1] - [lindex [lindex $meas_tab 0] 1]) < 0.2 } ] $ModuleName $SubTestDes] == 0} {
#    }
    set standard {{CubicCentimeters 148.953556060791016}}
    if {[CoreMeasuresEpsilonTest "zx mask volume" 0.00001 $standard $meas_tbl $LogFile $ModuleName] == 0} {
    }
    set standard {{CubicCentimeters 149.049936294555664}}
    if {[CoreMeasuresEpsilonTest "yx mask volume" 0.00001 $standard $meas_tab $LogFile $ModuleName] == 0} {
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $xx_ret ] != -1 } ] $ModuleName "Destroying mask $xx_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $yx_ret ] != -1 } ] $ModuleName "Destroying mask $yx_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $zx_ret ] != -1 } ] $ModuleName "Destroying mask $zx_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $xy_ret ] != -1 } ] $ModuleName "Destroying mask $xy_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $yy_ret ] != -1 } ] $ModuleName "Destroying mask $yy_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $zy_ret ] != -1 } ] $ModuleName "Destroying mask $zy_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $xz_ret ] != -1 } ] $ModuleName "Destroying mask $xz_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $yz_ret ] != -1 } ] $ModuleName "Destroying mask $yz_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $zz_ret ] != -1 } ] $ModuleName "Destroying mask $zz_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $num_ret ] != -1 } ] $ModuleName "Destroying mask $num_ret"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $tal_brain_zroi ] != -1 } ] $ModuleName "Destroying roi $tal_brain_zroi"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $tal_brain_yroi ] != -1 } ] $ModuleName "Destroying roi $tal_brain_yroi"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $tal_brain_xroi ] != -1 } ] $ModuleName "Destroying roi $tal_brain_xroi"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $tal_brain_mask ] != -1 } ] $ModuleName "Destroying mask $tal_brain_mask"

    return [ StopModule  $LogFile $ModuleName ]
}

