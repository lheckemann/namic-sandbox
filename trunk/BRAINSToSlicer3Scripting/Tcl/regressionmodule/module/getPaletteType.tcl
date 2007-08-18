# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2_get_palette_type command
# \fn        proc getPaletteType {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 transform type command
#
# To Do
#------------------------------------------------------------------------
# Add RGB COlor Palette
#


proc getPaletteType {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "getPaletteType"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_get_palette_type command "
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Get palette type - Load palette test"
    set TestPalId [b2_load_palette $B2_TALAIRACH_DIR/../palette/pet.pal]
    if { [ ReportTestStatus $LogFile  [ expr {$TestPalId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "Required arguement test"
    set errorTest [b2_get_palette_type]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Invalid arguement number test"
    set errorTest [b2_get_palette_type $TestPalId junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Invalid optional arguement test"
    set errorTest [b2_get_palette_type $TestPalId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Invalid object test"
    set errorTest [b2_get_palette_type -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Palette Type test"
    set PalType [b2_get_palette_type $TestPalId ]
    ReportTestStatus $LogFile  [ expr {$PalType == "RGB" } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_palette $TestPalId ] != -1 } ] $ModuleName "Destroying palette $TestPalId"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

