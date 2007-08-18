# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the brains2 command "b2_set_pixmap-directory"
# \fn        proc setPixmapDirectory {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Tests the brains2 command "b2_set_pixmap-directory"
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc setPixmapDirectory {pathToRegressionDir dateString} {
    set ModuleName "setPixmapDirectory";
    set ModuleAuthor "Greg Harris";
    set ModuleDescription "Test the b2_set_pixmap-directory command";

    global OUTPUT_DIR;
    global B2_BATCH_MODE;
    global MODULE_SUCCESS;
    global MODULE_FAILURE;
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString];


########################################
########################################
# Run Tests
########################################
########################################

    if {$B2_BATCH_MODE == 0} {

# First Test for invalid arguements
        set SubTestDes "required arguement test (pixmap directory)";
        set errorTest [b2_set_pixmap-directory];
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes;

        set SubTestDes "Set Pixmap Directory Test";
        set TestResult [b2_set_pixmap-directory $OUTPUT_DIR ];
        ReportTestStatus $LogFile  [ expr {$TestResult == 1 } ] $ModuleName $SubTestDes;
    }
    return [ StopModule  $LogFile $ModuleName ];
}

