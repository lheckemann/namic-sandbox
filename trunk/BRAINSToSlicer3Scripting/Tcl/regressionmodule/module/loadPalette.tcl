# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the loading of various Palette
#             file formats
# \fn        proc loadPalette {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS2 Palette
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc loadPalette {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "loadPalette"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 load palette command and loading various Palette file formats"
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
    set errorTest [b2 load palette]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 load palette $B2_TALAIRACH_DIR/../palette/pet.pal junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 load palette $B2_TALAIRACH_DIR/../palette/pet.pal junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2 load palette /invalid_directory_name/pet.pal]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Palette ###########################################
    set SubTestDes "BRAINS2 Palette Load test"
    set pal [b2 load palette $B2_TALAIRACH_DIR/../palette/pet.pal]
    if { [ ReportTestStatus $LogFile  [ expr {$pal != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy palette $pal ] != -1 } ] $ModuleName "Destroying palette $pal"
    }

    ############################### BRAINS2 Palette With Filter ##############################
    set SubTestDes "BRAINS2 Palette Load with filter test"
    set pal [b2 load palette $B2_TALAIRACH_DIR/../palette/pet.pal filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$pal != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy palette $pal ] != -1 } ] $ModuleName "Destroying palette $pal"
    }

    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

