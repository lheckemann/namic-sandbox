proc CoreSaveRoiTest {filterName TestRoiID SaveFileName LogFile ModuleName} {
    global MODULE_FAILURE


    set SubTestDes "$filterName Save Roi - Load saved Roi"
    set SaveRoiID [b2 load roi $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveRoiID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Roi - Convert Original Roi to mask"
    set TestMaskID [ b2 convert roi to mask $TestRoiID ]
    ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Roi - Convert Saved Roi to mask"
    set SaveMaskID [b2 convert roi to mask $SaveRoiID ]
    ReportTestStatus $LogFile  [ expr {$SaveMaskID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Roi - Difference saved BRAINS Roi"
    set diffMask [b2 xor masks $SaveMaskID $TestMaskID]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Roi - Difference volume saved BRAINS Roi"
    set volume [b2 measure volume mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $SaveMaskID ] != -1 } ] $ModuleName "Destroying mask $SaveMaskID"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $SaveRoiID ] != -1 } ] $ModuleName "Destroying roi $SaveRoiID"
}



# \author    Hans J. Johnson"
# \date        $Date: 2005-04-28 14:54:52 -0500 (Thu, 28 Apr 2005) $
# \brief    This module tests the saving of various roi file formats
# \fn        proc saveRoi {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS ROI
# Save a BRAINS2 ROI
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveRoi {pathToRegressionDir dateString} {

    set ModuleName "saveRoi"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save roi command and loading various ROI file formats"
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

    set SubTestDes "Save ROI - Load Roi test"
    set TestRoiID [b2 load roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/101tricia_rcaud.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC/user } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 save roi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi brains1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi brains1 $TestRoiID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi brains1 $TestRoiID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2 save roi /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/user/junk.zroi brains1 $TestRoiID]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS RLE Masks ###########################################
    set SubTestDes "Save BRAINS Roi test"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi brains1 $TestRoiID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveRoiTest "BRAINS" $TestRoiID ${OUTPUT_DIR}/TEST/10_ACPC/user/101junk.zroi $LogFile $ModuleName

        set SubTestDes "BRAINS invalid filename test"
        set errorTest [b2 save roi /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/user/101junk.zroi brains1 $TestRoiID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS invalid filter option test"
        set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/101junk.zroi brains1 $TestRoiID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }


    ############################### BRAINS RLE Masks ###########################################
    set SubTestDes "Save BRAINS2 Roi test"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi brains2 $TestRoiID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveRoiTest "BRAINS2" $TestRoiID ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi $LogFile $ModuleName

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2 save roi /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/user/junk.zroi brains2 $TestRoiID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter option test"
        set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi brains2 $TestRoiID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ############################### BRAINS RLE Masks ###########################################
    set SubTestDes "Save INVENTOR Roi test"
    set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.iv inventor $TestRoiID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        # Test Resulting Inventor File

        set SubTestDes "Inventor invalid filename test"
        set errorTest [b2 save roi /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/user/junk.iv inventor $TestRoiID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Inventor invalid filter option test"
        set errorTest [b2 save roi ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.zroi inventor $TestRoiID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

