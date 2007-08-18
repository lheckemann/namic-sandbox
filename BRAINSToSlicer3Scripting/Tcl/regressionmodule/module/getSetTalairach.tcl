# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the command "b2 get|set transform" commands
# \fn        proc getSetTalairach {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Tests the command "b2 get|set talairach-parameters"
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getSetTalairach {pathToRegressionDir dateString} {


    set ModuleName "getSetTalairach"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 get|set Talairach commands"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Get/Set Talairach - Load Image test"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096]
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Get/Set Talairach - Load Talairach test"
    set TestTalBndID [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTalBndID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "(set) required arguement test (1)"
    set errorTest [b2_set_talairach-parameters]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required arguement test (2)"
    set errorTest [b2_set_talairach-parameters $TestTalBndID]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required arguement test (2)"
    set errorTest [b2_set_talairach-parameters $TestTalBndID image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(set) required invalid type test"
    set errorTest [b2_set_talairach-parameters $TestTalBndID mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Set Transform Image Test"
    set TestResult [b2_set_talairach-parameters $TestTalBndID image $TestImageID]
    ReportTestStatus $LogFile  [ expr {$TestResult != -1 } ] $ModuleName $SubTestDes


    ###### Get Image Transform
    set SubTestDes "(get) required arguement test (1)"
    set errorTest [b2_get_image_talairach-parameters]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(get) invalid arguement number test"
    set errorTest [b2_get_image_talairach-parameters $TestImageID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(get) invalid optional arguement test"
    set errorTest [b2_get_image_talairach-parameters $TestImageID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Get Image transform test"
    set TalBndId [b2_get_image_talairach-parameters $TestImageID]
    ReportTestStatus $LogFile  [ expr {$TalBndId == $TestTalBndID } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestTalBndID ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestTalBndID"

    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

