# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the brains2 command "b2 set debug-level"
# \fn        proc setDebugLevel {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Tests the BRAINS2 command "b2 set debug-level"
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc setDebugLevel {pathToRegressionDir dateString} {


    set ModuleName "setDebugLevel"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 set debug-level command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    ############################### Set Debug Level ###########################################

    # First Test for invalid arguements
    set SubTestDes "required arguement test (debug-level)"
    set errorTest [b2 set debug-level]
    ReportTestStatus $LogFile  [ expr {$errorTest != "critical" } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test (debug-level)"
    set errorTest [b2 set debug-level critical junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test (debug-level)"
    set errorTest [b2 set debug-level critical junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    set SubTestDes "Set Debug Level"
    set TestResult [b2 set debug-level tracing]
    ReportTestStatus $LogFile  [ expr {$TestResult == "tracing" } ] $ModuleName $SubTestDes

    set TestResult [b2 set debug-level critical]
    ReportTestStatus $LogFile  [ expr {$TestResult == "critical" } ] $ModuleName $SubTestDes


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

