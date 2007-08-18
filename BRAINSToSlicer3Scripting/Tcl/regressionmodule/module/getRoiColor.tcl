# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2_get_roi_color command
# \fn        proc getRoiColor {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_roi_color command
#
# To Do
#------------------------------------------------------------------------
# Maybe test an ROI in the brains2rc file
#


proc getRoiColor {pathToRegressionDir dateString} {

    set ModuleName "getRoiColor"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_get_roi_color command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Get roi color load test"
    set TestRoiID [b2_load_roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/96tricia_rcaud.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required argument test"
    set errorTest [b2_get_roi_color]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2_get_roi_color $TestRoiID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2_get_roi_color $TestRoiID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid roi test"
    set errorTest [b2_get_roi_color -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Get roi color test"
    set errorTest [b2_get_roi_color $TestRoiID]
    ReportTestStatus $LogFile  [ expr {[llength $errorTest] == 3 } ] $ModuleName $SubTestDes


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

