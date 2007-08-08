# \author        Greg Harris"
# \date
# \brief        b2 resample mask
# \fn                proc resampleMask {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 resample mask tested with a bookstein warp.
#
# To Do
#------------------------------------------------------------------------
# Evaluate mask idempotence with a quick and dirty Kappa.
#
# To Test the Test
# -----------------------------------------------------------------------


proc resampleMask {pathToRegressionDir dateString} {
    set ModuleName "resampleMask"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 resample mask"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests


    set acq_brain_mask [b2 load mask "${pathToRegressionDir}/SGI/MR/B2-Bookstein/TEST/10_ACPC/cran_mask.segment"]
    set orig_dims [b2 get dims mask ${acq_brain_mask}]
    set orig_res [b2 get res mask ${acq_brain_mask}]

    set outOfRange [b2 create affine-transform "$orig_dims" "$orig_res" "$orig_dims" "$orig_res" dz= 96]
    set range_mask [b2 resample mask forward ${acq_brain_mask} ${outOfRange} silent-flag= true]
    set SubTestDes "correct response test: \[b2 resample mask forward <brain-mask> <shift-z>\]"
    if {[ReportTestStatus $LogFile  [ expr { $range_mask != -1} ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tbl [b2 measure volume mask ${range_mask}]
    set new_vol [lindex [lindex $meas_tbl 0] 1]
    set old_vol 976.175673961639404
puts $LogFile "- - - - - - - - - -:"
puts $LogFile "Measurements:          acq mask ${old_vol}   warp idem mask ${new_vol}"
    set SubTestDes "acq mask volume  close enough to  warp idem mask volume test"
    set control_ratio [expr (${new_vol} - ${old_vol}) / ${old_vol}]
puts $LogFile "Difference Fraction:   ${control_ratio}"
    if {[ReportTestStatus $LogFile  [ expr { ${control_ratio} < 0.1 } ] $ModuleName $SubTestDes] == 0} {
}


    set bookstein_warp [b2 load transform "${pathToRegressionDir}/SGI/MR/B2-Bookstein/TEST/10_ACPC/ANON001_bookstein.wrp"]


    set num_ret [b2 resample mask]
    set SubTestDes "required argument test: \[b2 resample mask\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2 resample mask inverse ${acq_brain_mask} ${bookstein_warp} junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 resample mask inverse ${acq_brain_mask} ${bookstein_warp} junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set b_ret [b2 resample mask inverse ${acq_brain_mask} ${bookstein_warp}]
    set SubTestDes "correct response test: \[b2 resample mask inverse <brain-mask> <warp>\]"
    if {[ReportTestStatus $LogFile  [ expr { $b_ret != -1} ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask ${acq_brain_mask}]
    set old_vol [lindex [lindex $meas_tbl 0] 1]

    set idem_mask [b2 resample mask forward ${b_ret} ${bookstein_warp}]
    set SubTestDes "correct response test: \[b2 resample mask forward <brain-mask'> <warp>\]"
    if {[ReportTestStatus $LogFile  [ expr { $idem_mask != -1} ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res mask ${idem_mask}]
    set meas_tab [b2 measure volume mask ${idem_mask}]
    set new_vol [lindex [lindex $meas_tab 0] 1]
    for {set i 0} { ${i} < 3 } {incr i} {
    set new_vol [expr $new_vol * ([lindex ${orig_res} ${i}] / [lindex ${mod_res} ${i}])]
    }

puts $LogFile "- - - - - - - - - -:"
puts $LogFile "Measurements:          acq mask ${old_vol}   warp idem mask ${new_vol}"
    set SubTestDes "acq mask volume  close enough to  warp idem mask volume test"
    set control_ratio [expr (${new_vol} - ${old_vol}) / ${old_vol}]
puts $LogFile "Difference Fraction:   ${control_ratio}"
    if {[ReportTestStatus $LogFile  [ expr { ${control_ratio} < 0.1 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims mask ${idem_mask}]
    set SubTestDes "resample mask test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {


    puts "Original:  ${orig_dims}  Idempotent:  ${mod_dims}"
    }

    set SubTestDes "resample mask test for same res list"
#    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
#

#    puts "Original:  ${orig_res}  Idempotent:  ${mod_res}"
#    }

    for {set i 0} { ${i} < 3 } {incr i} {
    set diff_lr [expr [lindex ${mod_res} ${i}] - [lindex ${orig_res} ${i}]]
    set diff_rl [expr [lindex ${orig_res} ${i}] - [lindex ${mod_res} ${i}]]
    if {[ReportTestStatus $LogFile  [ expr { ( ${diff_lr} <= 0.001 ) && ( ${diff_rl} <= 0.001 ) } ] $ModuleName $SubTestDes] == 0} {


        puts "Resolutions in coordinate ${i}:   Original:  [lindex ${orig_res} ${i}]  Idempotent:  [lindex ${mod_res} ${i}]"
    }
    }

#return $MODULE_FAILURE

    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${outOfRange} ] != -1 } ] $ModuleName "Destroying transform ${outOfRange}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${bookstein_warp} ] != -1 } ] $ModuleName "Destroying transform ${bookstein_warp}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${range_mask} ] != -1 } ] $ModuleName "Destroying mask ${range_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${idem_mask} ] != -1 } ] $ModuleName "Destroying mask ${idem_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${b_ret} ] != -1 } ] $ModuleName "Destroying mask ${b_ret}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${acq_brain_mask} ] != -1 } ] $ModuleName "Destroying mask ${acq_brain_mask}"

    return [ StopModule  $LogFile $ModuleName ]
}

