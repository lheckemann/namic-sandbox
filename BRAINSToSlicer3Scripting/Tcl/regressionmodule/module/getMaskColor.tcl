# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2_get_mask_color command
# \fn        proc getMaskColor {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_mask_color command
#
# To Do
#------------------------------------------------------------------------
# Maybe extend if we allow mask colors to be specified in the
# brains2rc file
#


proc getMaskColor {pathToRegressionDir dateString} {

    set ModuleName "getMaskColor"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_get_mask_color command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Get mask color load test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_get_mask_color]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_get_mask_color $TestMaskID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_get_mask_color $TestMaskID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid mask test"
    set errorTest [b2_get_mask_color -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Get mask color test"
    set errorTest [b2_get_mask_color $TestMaskID]
    ReportTestStatus $LogFile  [ expr {[llength $errorTest] == 4 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

