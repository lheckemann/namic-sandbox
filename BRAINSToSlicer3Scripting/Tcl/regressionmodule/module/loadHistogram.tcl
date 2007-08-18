# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various Histogram
#             file formats
# \fn        proc loadHistogram {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# File Types Tested
# -----------------------------------------------------------------------
# BRAINS
# BRAINS2
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc loadHistogram {pathToRegressionDir dateString} {

    set ModuleName "loadHistogram"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 load Histogram command and loading various Histogram file formats"
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
    set errorTest [b2_load_histogram]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/cran_histogram junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/cran_histogram junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_histogram /invalid_directory_name/cran_histogram]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS Histograms ###########################################
    set SubTestDes "BRAINS Histogram Load test"
    set hist [b2_load_histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/cran_histogram]
    if { [ ReportTestStatus $LogFile  [ expr {$hist != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_histogram $hist ] != -1 } ] $ModuleName "Destroying histogram $hist"
    }

    ############################### BRAINS2 Histograms ###########################################
    set SubTestDes "BRAINS2 Histogram Load test"
    set hist [b2_load_histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/ANON013_T1.hst]
    if { [ ReportTestStatus $LogFile  [ expr {$hist != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_histogram $hist ] != -1 } ] $ModuleName "Destroying histogram $hist"
    }

    ################### Test with specifying the filter to use for loading #######################
    set SubTestDes "BRAINS2 Histogram Load with filter test"
    set hist [b2_load_histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/ANON013_T1.hst filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$hist != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_histogram $hist ] != -1 } ] $ModuleName "Destroying histogram $hist"
    }

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

