# \author    Greg Harris"
# \date
# \brief    The command 'b2 fit image interleaved 0  3 skip-anterior= 8 skip-posterior= 7'
# \fn        proc fitInterleaved {pathToRegressionDir dateString {GT Test}}
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

proc fitInterleaved {pathToRegressionDir dateString {GT Test}} {

    set ModuleName "fitInterleaved"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 fit image interleaved"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr



    # Run Tests

    set im1 [b2 load image "${pathToRegressionDir}/SGI/MR/B2-InterleaveFit/TEST/T2_003/006839.003.002"]
    set SubTestDes "required image loaded test: ${pathToRegressionDir}/SGI/MR/B2-InterleaveFit/TEST/T2_003/006839.003.002"
    if {[ReportTestStatus $LogFile  [ expr {$im1 != -1 } ] $ModuleName $SubTestDes] == 0} {
      return $MODULE_FAILURE
    }
#b2 fit image interleaved 0  3 skip-anterior= 8 skip-posterior= 7

    set num_ret [b2 fit image interleaved]
    set SubTestDes "required argument test: \[b2 fit image interleaved\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set num_ret [b2 fit image interleaved ${im1}]
    set SubTestDes "required argument test: \[b2 fit image interleaved ${im1}\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set num_ret [b2 fit image interleaved ${im1} 3 5]
    set SubTestDes "required argument test: \[b2 fit image interleaved ${im1} 3 5\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }


    set SubTestDes "optional argument number test"
    set num_ret [b2 fit image interleaved ${im1} 3 junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 fit image interleaved ${im1} 3 junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
    }


    set num_ret [b2 fit image interleaved ${im1} 3 skip-anterior= 8 skip-posterior= 7]
    set SubTestDes "correct response test: \[b2 fit image interleaved ${im1} 3 skip-anterior= 8 skip-posterior= 7\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
    }

    if {$num_ret != -1} {
    b2 set transform ${num_ret} image ${im1}
        set im2 [b2 load image "${pathToRegressionDir}/SGI/MR/B2-InterleaveFit/TEST/T2_003/006839.003.002"]
        set xfm [b2 load transform "${pathToRegressionDir}/SGI/MR/B2-InterleaveFit/TEST/T2_003/IR_interleave.xfrm"]
    b2 set transform ${xfm} image ${im2}
    set im3 [b2 subtract images [list ${im1} ${im2}]]
    set errMask [b2 threshold image ${im3} 0.5 absolute-value= True]
    set volTab [b2 measure volume mask ${errMask}]

    set SubTestDes "difference volume  <= 0.01   test"
    if {[ReportTestStatus $LogFile  [ expr {[lindex [lindex ${volTab} 0] 1]  <= 0.01 } ] $ModuleName $SubTestDes] == 0} {
        }

    # Free memory
     ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${im2} ] != -1 } ] $ModuleName "Destroying image ${im2}"
     ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${xfm} ] != -1 } ] $ModuleName "Destroying transform ${xfm}"
     ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${im3} ] != -1 } ] $ModuleName "Destroying image ${im3}"
     ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${errMask} ] != -1 } ] $ModuleName "Destroying mask ${errMask}"
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${im1} ] != -1 } ] $ModuleName "Destroying image ${im1}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${num_ret} ] != -1 } ] $ModuleName "Destroying transform ${num_ret}"

    return [ StopModule  $LogFile $ModuleName ]
}

