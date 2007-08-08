# \author        Greg Harris"
# \date
# \brief        b2 erode surface mask
# \fn                proc erodeSurfaceMask {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 erode surface mask tested simply.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc erodeSurfaceMask {pathToRegressionDir dateString} {
    set ModuleName "erodeSurfaceMask"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 erode surface mask"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests


    set class_image [b2 load image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_segment.hdr"]
    set brain_mask [b2 load mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/cran_mask.default"]
    set orig_dims [b2 get dims mask ${brain_mask}]
    set orig_res [b2 get res mask ${brain_mask}]

#    set errode_brain_mask [b2 erode surface mask ${brain_mask} ${class_image} 131]


    set num_ret [b2 erode surface mask]
    set SubTestDes "required argument test: \[b2 erode surface mask\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "insufficient argument number test"
    set num_ret [b2 erode surface mask ${brain_mask} ${class_image}  ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2 erode surface mask ${brain_mask} ${class_image} 131 junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 erode surface mask ${brain_mask} ${class_image} 131  junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set erode_brain_mask [b2 erode surface mask ${brain_mask} ${class_image} 131 ]
    set SubTestDes "correct response test: \[b2 erode surface mask <brain-mask> <class-image> 131\]"
    if {[ReportTestStatus $LogFile  [ expr { ${erode_brain_mask} >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask ${brain_mask}]
    set old_vol [lindex [lindex $meas_tbl 0] 1]

    set mod_res [b2 get res mask ${erode_brain_mask}]
    set meas_tab [b2 measure volume mask ${erode_brain_mask}]
    set new_vol [lindex [lindex $meas_tab 0] 1]
    for {set i 0} { ${i} < 3 } {incr i} {
    set new_vol [expr $new_vol * ([lindex ${orig_res} ${i}] / [lindex ${mod_res} ${i}])]
    }

puts $LogFile "- - - - - - - - - -:"
puts $LogFile "Measurements:          brain mask ${old_vol}   erode surface mask ${new_vol}"
    set SubTestDes "erode surface mask volume  close enough to  brain mask volume test"
    set control_ratio [expr (${old_vol} - ${new_vol}) / ${old_vol}]
puts $LogFile "Difference Fraction:   ${control_ratio}"
    if {[ReportTestStatus $LogFile  [ expr { ${control_ratio} < 0.05 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims mask ${erode_brain_mask}]
    set SubTestDes "erode surface mask test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {


    puts "Original:  ${orig_dims}  Idempotent:  ${mod_dims}"
    }

    set SubTestDes "erode surface mask test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {


    puts "Original:  ${orig_res}  Idempotent:  ${mod_res}"
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${erode_brain_mask} ] != -1 } ] $ModuleName "Destroying mask ${erode_brain_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${brain_mask} ] != -1 } ] $ModuleName "Destroying mask ${brain_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${class_image} ] != -1 } ] $ModuleName "Destroying image ${class_image}"

    return [ StopModule  $LogFile $ModuleName ]
}

