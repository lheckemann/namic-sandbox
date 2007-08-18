# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the command "b2_set_standard-size"
# \fn        proc setStandardSize {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Tests the command "b2_set_standard-size"
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc setStandardSize {pathToRegressionDir dateString} {


    set ModuleName "setStandardSize"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_set_standard-size command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    # First Test for invalid arguements
    set errorTest [b2_set_standard-size]
    set SubTestDes "Does observed default $errorTest == 1.015625"
    ReportTestStatus $LogFile  [ expr {$errorTest == 1.015625 } ] $ModuleName $SubTestDes


    set SubTestDes "Set Standard Size Test"
    set TestResult [b2_set_standard-size 0.5]
    set SubTestDes "Does observed reset $TestResult == 0.500000"
    ReportTestStatus $LogFile  [ expr {$TestResult == 0.500000 } ] $ModuleName $SubTestDes

    set errorTest [b2_set_standard-size]
    set SubTestDes "Does observed reset default $errorTest == 0.500000"
    ReportTestStatus $LogFile  [ expr {$errorTest == 0.500000 } ] $ModuleName $SubTestDes



    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

