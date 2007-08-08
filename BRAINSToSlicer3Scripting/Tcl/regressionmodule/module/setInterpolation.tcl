# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the brains2 command "b2 set interpolation"
# \fn        proc loadTalairachParameter {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Tests the brains2 command "b2 set interpolation"
#
# To Do
#------------------------------------------------------------------------
# Maybe snap images and save an image to verify that the GUI and kernel
# see this variable change
#


proc setInterpolation {pathToRegressionDir dateString} {


    set ModuleName "setInterpolation"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 set interpolation command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    # First Test for invalid arguements
    set errorTest [b2 set interpolation]

    set SubTestDes "return of Interpolation Mode default is list of length 1 test"
    ReportTestStatus $LogFile  [ expr {[llength $errorTest] == 1 } ] $ModuleName $SubTestDes
    set SubTestDes "return of Interpolation Mode default is 'trilinear' test"
    ReportTestStatus $LogFile  [ expr {[lindex $errorTest 0] == "trilinear" } ] $ModuleName $SubTestDes


    set SubTestDes "arguement number test (interpolation)"
    set errorTest [b2 set interpolation nearest junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test (interpolation)"
    set errorTest [b2 set interpolation nearest junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



#    set SubTestDes "Set Interpolation Nearest-Neighbor Test"
    set TestResult [b2 set interpolation nearest-neighbor]
    puts "'$TestResult'"

    set SubTestDes "change of Interpolation to Nearest-Neighbor is list of length 1 test"
    ReportTestStatus $LogFile  [ expr {[llength $TestResult] == 1 } ] $ModuleName $SubTestDes
    set SubTestDes "change  of Interpolation to Nearest-Neighbor is 'nearest-neighbor' test"
    ReportTestStatus $LogFile  [ expr {[lindex $TestResult 0] == "nearest-neighbor" } ] $ModuleName $SubTestDes

#    set SubTestDes "Set Interpolation Trilinear Test"
    set TestResult [b2 set interpolation trilinear]
    puts "'$TestResult'"

    set SubTestDes "change of Interpolation to Trilinear is list of length 1 test"
    ReportTestStatus $LogFile  [ expr {[llength $TestResult] == 1 } ] $ModuleName $SubTestDes
    set SubTestDes "change  of Interpolation to Trilinear is 'trilinear' test"
    ReportTestStatus $LogFile  [ expr {[lindex $TestResult 0] == "trilinear" } ] $ModuleName $SubTestDes

    # We could snap images with an applied transform to verify that the GUI sees the flags
    # We should also save out images to verify that the save image command sees the mode


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

