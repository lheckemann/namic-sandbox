# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the loading of various Talairach
#             box file formats
# \fn        proc loadTalairachBox {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS Talairach Box
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc loadTalairachBox {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "loadTalairachBox"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_load_talairach-box command and loading various Talairach-Box file formats"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_load_talairach-box]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_talairach-box $B2_TALAIRACH_DIR/nfrontal_box junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_talairach-box $B2_TALAIRACH_DIR/nfrontal_box junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_talairach-box /invalid_directory_name/nfrontal_box]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes




    ############################### BRAINS Talairach Box #############################################
    set SubTestDes "BRAINS2 Talairach Box Load test"
    set TestBoxId [b2_load_talairach-box $B2_TALAIRACH_DIR/nfrontal_box]
    if { [ ReportTestStatus $LogFile  [ expr {$TestBoxId != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-box $TestBoxId ] != -1 } ] $ModuleName "Destroying talairach-box $TestBoxId"
    }

    ############################### BRAINS Talairach Box with FIlter #################################
    set SubTestDes "BRAINS2 Talairach Box with Filter Load test"
    set TestBoxId [b2_load_talairach-box $B2_TALAIRACH_DIR/nfrontal_box filter= brains1]
    if { [ ReportTestStatus $LogFile  [ expr {$TestBoxId != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-box $TestBoxId ] != -1 } ] $ModuleName "Destroying talairach-box $TestBoxId"
    }

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

