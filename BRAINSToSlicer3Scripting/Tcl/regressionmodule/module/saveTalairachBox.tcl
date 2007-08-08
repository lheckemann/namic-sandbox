proc CoreSaveTalBoxTest {filterName TestTalBoxID SaveFileName TestTalParID LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "Save Talairach Box - Load saved $filterName Talairach Box"
    set SaveTalBoxID [b2 load talairach-box $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveTalBoxID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Save $filterName Talairach Box - Original Talairach Box to Mask"
    set origBoxMask [ b2 convert talairach-box to mask $TestTalBoxID $TestTalParID]
    ReportTestStatus $LogFile  [ expr {$origBoxMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Save $filterName Talairach Box - New Talairach Box to Mask"
    set newBoxMask [ b2 convert talairach-box to mask $SaveTalBoxID $TestTalParID]
    ReportTestStatus $LogFile  [ expr {$newBoxMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Save $filterName Talairach Box - Difference Mask"
    set diffMask [ b2 xor masks $origBoxMask $newBoxMask]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Save $filterName Talairach Box - Volume Difference"
    set volume [ b2 measure volume mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-box $SaveTalBoxID ] != -1 } ] $ModuleName "Destroying talairach-box $SaveTalBoxID"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $origBoxMask ] != -1 } ] $ModuleName "Destroying mask $origBoxMask"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $newBoxMask ] != -1 } ] $ModuleName "Destroying mask $newBoxMask"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"

    return $MODULE_SUCCESS

}

# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various Talairach Box file formats
# \fn        proc saveTalairachBox {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS Talairach Box
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveTalairachBox {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR


    set ModuleName "saveTalairachBox"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save talairach-box command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Talairach Box - Load Talairach Box test"
    set TestTalBoxID [b2 load talairach-box $B2_TALAIRACH_DIR/nfrontal_box]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTalBoxID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Talairach Box - Load Talairach Parameter test"
    set TestTalParID [b2 load talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTalParID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 save talairach-box]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 save talairach-box ${OUTPUT_DIR}/TEST/10_ACPC/junk_box]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2 save talairach-box ${OUTPUT_DIR}/TEST/10_ACPC/junk_box brains1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 save talairach-box ${OUTPUT_DIR}/TEST/10_ACPC/junk_box brains $TestTalBoxID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 save talairach-box ${OUTPUT_DIR}/TEST/10_ACPC/junk_box brains $TestTalBoxID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS Talairach Box ###########################################
    set SubTestDes "Save BRAINS Talairach Box test"
    set errorTest [b2 save talairach-box ${OUTPUT_DIR}/TEST/10_ACPC/junk_box brains1 $TestTalBoxID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveTalBoxTest "BRAINS" $TestTalBoxID ${OUTPUT_DIR}/TEST/10_ACPC/junk_box $TestTalParID $LogFile $ModuleName

        set SubTestDes "BRAINS invalid filename test"
        set errorTest [b2 save talairach-box /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk_box brains1 $TestTalBoxID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS invalid filter-suffix test"
        set errorTest [b2 save talairach-box ${OUTPUT_DIR}/TEST/10_ACPC/junk_box brains1 $TestTalBoxID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-box $TestTalBoxID ] != -1 } ] $ModuleName "Destroying talairach-box $TestTalBoxID"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $TestTalParID ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestTalParID"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

