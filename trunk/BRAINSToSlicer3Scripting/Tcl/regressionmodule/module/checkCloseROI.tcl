# \author    Greg Harris"
# \date
# \brief    b2 check roi
#        b2 close roi
# \fn        proc checkCloseROI {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString        - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl

proc checkCloseROI {pathToRegressionDir dateString} {

    set ModuleName "checkCloseROI"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 close ROI and b2 check ROI"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




    # Run Tests

    set traceZ [b2 load roi "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/user/unclosed_A.zroi"]
    set traceY [b2 load roi "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/user/unclosed_B.yroi"]
    set traceX [b2 load roi "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/user/unclosed_C.xroi"]

    set num_ret [b2 check roi]
    set SubTestDes "required argument test: \[b2 check roi\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2 check roi ${traceZ} ${traceX}]
    set SubTestDes "unknown optional argument test: \[b2 check roi <traceA> <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set orig_dims [b2 get dims roi ${traceZ}]
    set orig_res [b2 get res roi ${traceZ}]

    set list_ret [b2 check roi ${traceZ}]
puts "traceZ checked as '${list_ret}'"
    set SubTestDes "legal zroi test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {$list_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'Unclosed' test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 0] Unclosed] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'Coronal' test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 5] Coronal] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return slice 96 test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr { [lindex $list_ret 7]  == 96 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2 close roi]
    set SubTestDes "required argument test: \[b2 close roi\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2 close roi ${traceZ} ${traceX}]
    set SubTestDes "unknown optional argument test: \[b2 close roi <traceA> <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set traceA [b2 close roi ${traceZ}]
    set SubTestDes "legal zroi test: \[b2 close roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {$traceA != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set list_ret [b2 check roi ${traceA}]
puts "traceA checked as '${list_ret}'"
    set SubTestDes "legal zroi test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {$list_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'All' test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 0] All] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'ROIs are closed' test: \[b2 check roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string first "ROIs are closed" $list_ret] != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2 close roi ${traceZ}]
    set SubTestDes "repeat close test: \[b2 close roi <traceA>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims roi ${traceA}]
    set SubTestDes "converted roi test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res roi ${traceA}]
    set SubTestDes "converted roi test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}


    set orig_dims [b2 get dims roi ${traceY}]
    set orig_res [b2 get res roi ${traceY}]

    set list_ret [b2 check roi ${traceY}]
puts "traceY checked as '${list_ret}'"
    set SubTestDes "legal yroi test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {$list_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'Unclosed' test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 0] Unclosed] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'Axial' test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 5] Axial] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return slice 128 test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr { [lindex $list_ret 7]  == 128 } ] $ModuleName $SubTestDes] == 0} {
}

    set traceB [b2 close roi ${traceY}]
    set SubTestDes "legal yroi test: \[b2 close roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {$traceB != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set list_ret [b2 check roi ${traceB}]
puts "traceB checked as '${list_ret}'"
    set SubTestDes "legal yroi test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {$list_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'All' test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 0] All] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'ROIs are closed' test: \[b2 check roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string first "ROIs are closed" $list_ret] != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2 close roi ${traceY}]
    set SubTestDes "repeat close test: \[b2 close roi <traceB>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims roi ${traceB}]
    set SubTestDes "converted roi test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res roi ${traceB}]
    set SubTestDes "converted roi test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}


    set orig_dims [b2 get dims roi ${traceX}]
    set orig_res [b2 get res roi ${traceX}]

    set list_ret [b2 check roi ${traceX}]
puts "traceX checked as '${list_ret}'"
    set SubTestDes "legal xroi test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {$list_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'Unclosed' test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 0] Unclosed] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'Sagittal' test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 5] Sagittal] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return slice 127 test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr { [lindex $list_ret 7]  == 127 } ] $ModuleName $SubTestDes] == 0} {
}

    set traceC [b2 close roi ${traceX}]
    set SubTestDes "legal xroi test: \[b2 close roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {$traceC != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set list_ret [b2 check roi ${traceC}]
puts "traceC checked as '${list_ret}'"
    set SubTestDes "legal xroi test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {$list_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'All' test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string compare [lindex $list_ret 0] All] == 0 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct return 'ROIs are closed' test: \[b2 check roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {[string first "ROIs are closed" $list_ret] != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2 close roi ${traceX}]
    set SubTestDes "repeat close test: \[b2 close roi <traceC>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_dims [b2 get dims roi ${traceC}]
    set SubTestDes "converted roi test for same dims list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_dims} ${orig_dims}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set mod_res [b2 get res roi ${traceC}]
    set SubTestDes "converted roi test for same res list"
    if {[ReportTestStatus $LogFile  [ expr {[string compare ${mod_res} ${orig_res}] == 0 } ] $ModuleName $SubTestDes] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi ${traceA} ] != -1 } ] $ModuleName "Destroying roi ${traceA}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi ${traceB} ] != -1 } ] $ModuleName "Destroying roi ${traceB}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi ${traceC} ] != -1 } ] $ModuleName "Destroying roi ${traceC}"

    return [ StopModule  $LogFile $ModuleName ]
}

