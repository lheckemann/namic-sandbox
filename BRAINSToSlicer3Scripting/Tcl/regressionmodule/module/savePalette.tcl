proc CoreSavePaletteTest {filterName TestPaletteID SaveFileName LogFile ModuleName} {
    global MODULE_FAILURE

    set SubTestDes "$filterName Save Palette - Load saved Palette"
    set SavePaletteID [b2_load_palette $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SavePaletteID != -1 } ] $ModuleName $SubTestDes

    set origPaletteType [b2_get_palette_type $TestPaletteID ]
    set newPaletteType [b2_get_palette_type $SavePaletteID]

    set SubTestDes "$filterName Save Palette - Palette Type Test"
    ReportTestStatus $LogFile  [ expr {$origPaletteType == $newPaletteType } ] $ModuleName $SubTestDes


    set origPaletteData [b2_get_palette_data $TestPaletteID ]
    set newPaletteData [b2_get_palette_data $SavePaletteID]

    set SubTestDes "$filterName Save Palette - Palette Size Test"
    ReportTestStatus $LogFile  [ expr {[llength $origPaletteData] == [llength $newPaletteData] } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $origPaletteData]} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            set SubTestDes "$filterName Palette Comparison Location ($i,$j)"
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $origPaletteData $i] $j] == [lindex [lindex $newPaletteData $i] $j] } ] $ModuleName $SubTestDes
        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_palette $SavePaletteID ] != -1 } ] $ModuleName "Destroying palette $SavePaletteID"
}


# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various palette file formats
# \fn        proc saveTransform {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS2 Palette
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc savePalette {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "savePalette"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_save_palette command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Palette - Load Palette test"
    set TestPalID [b2_load_palette $B2_TALAIRACH_DIR/../palette/pet.pal]
    if { [ ReportTestStatus $LogFile  [ expr {$TestPalID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_save_palette]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2_save_palette ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2_save_palette ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal brains2]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_save_palette ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal brains2 $TestPalID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_save_palette ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal brains2 $TestPalID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Palette ###########################################
    set SubTestDes "Save BRAINS2 Palette test"
    set errorTest [b2_save_palette ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal  brains2 $TestPalID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSavePaletteTest "BRAINS2" $TestPalID ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal $LogFile $ModuleName

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2_save_palette /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.pal brains2 $TestPalID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2_save_palette ${OUTPUT_DIR}/TEST/10_ACPC/junk.pal brains2 $TestPalID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }




    ReportTestStatus $LogFile  [ expr { [ b2_destroy_palette $TestPalID ] != -1 } ] $ModuleName "Destroying palette $TestPalID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

