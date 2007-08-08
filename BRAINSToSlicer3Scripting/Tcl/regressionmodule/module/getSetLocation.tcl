# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the brains2 commands "b2 get|set location"
# \fn        proc getSetLocation {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Tests the brains2 commands "b2 get|set location"
#
# To Do
#------------------------------------------------------------------------
# Add a visual snapshot of the viewers
#


proc getSetLocation {pathToRegressionDir dateString} {


    set ModuleName "getSetLocation"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 get|set location commands"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    # First Test for invalid arguements
    set SubTestDes "required arguement test (set location 1)"
    set errorTest [b2 set location]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (set location 2)"
    set errorTest [b2 set location 128]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (set location 3)"
    set errorTest [b2 set location 128 128]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test (set location)"
    set errorTest [b2 set location 128 128 128 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test (set location)"
    set errorTest [b2 set location 128 128 128 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    set SubTestDes "Set Location Test"
    set TestResult [b2 set location 128 129 130]
    ReportTestStatus $LogFile  [ expr {$TestResult != 0 } ] $ModuleName $SubTestDes


    set SubTestDes "Get Location Test"
    set TestResult [b2 get location]
    ReportTestStatus $LogFile  [ expr {[llength $TestResult] == 3 } ] $ModuleName $SubTestDes
    set SubTestDes "Get Location Test - X"
    ReportTestStatus $LogFile  [ expr {[lindex $TestResult 0] == 128 } ] $ModuleName $SubTestDes
    set SubTestDes "Get Location Test - Y"
    ReportTestStatus $LogFile  [ expr {[lindex $TestResult 1] == 129 } ] $ModuleName $SubTestDes
    set SubTestDes "Get Location Test - Z"
    ReportTestStatus $LogFile  [ expr {[lindex $TestResult 2] == 130 } ] $ModuleName $SubTestDes

    set SubTestDes "Get Location Test Invalid arguement test"
    set TestResult [b2 get location 1]
    ReportTestStatus $LogFile  [ expr {$TestResult == -1 } ] $ModuleName $SubTestDes


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

