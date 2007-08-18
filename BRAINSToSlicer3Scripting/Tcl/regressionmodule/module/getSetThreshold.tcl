# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the command "b2 get|set threshold" commands
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
# Fix the operation pv_attachTalairach and Transform
#


proc getSetThreshold {pathToRegressionDir dateString} {


    set ModuleName "getSetThreshold"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 get|set threshold command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Get/Set Mask Threshold - Load Mask Test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "(set) required arguement test (1)"
    set errorTest [b2_set_threshold]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required arguement test (2)"
    set errorTest [b2_set_threshold mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required arguement test (3)"
    set errorTest [b2_set_threshold mask $TestMaskID]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required invalid type test"
    set errorTest [b2_set_threshold image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Set Threshold Mask Test"
    set TestResult [b2_set_threshold mask $TestMaskID 100]
    ReportTestStatus $LogFile  [ expr {$TestResult == 100.000000 } ] $ModuleName $SubTestDes

    ######### Get Threshold
    set SubTestDes "(get) required arguement test (1)"
    set errorTest [b2_get_threshold]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(get) required arguement test (2)"
    set errorTest [b2_get_threshold mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required invalid type test"
    set errorTest [b2_get_threshold image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Get Threshold Mask Test"
    set errorTest [b2_get_threshold mask $TestMaskID]
    ReportTestStatus $LogFile  [ expr {$errorTest == 100.000000 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"

    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

