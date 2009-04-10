proc CoreSaveMaskTest {filterName TestMaskID SaveFileName LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "$filterName Save Mask - Load saved Mask"
    set SaveMaskID [b2_load_mask $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveMaskID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Mask - Difference saved Mask"
    set diffMask [b2_xor_masks $SaveMaskID $TestMaskID]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Mask - Difference volume saved Mask"
    set volume [b2_measure_volume_mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $SaveMaskID ] != -1 } ] $ModuleName "Destroying mask $SaveMaskID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
}


# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various mask file formats
# \fn        proc saveMask {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a Run Length Endoded Mask
# Save a BRAINS2 mask (Octree)
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveMask {pathToRegressionDir dateString} {

    set ModuleName "saveMask"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_save_mask command and loading various Mask file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set MaskTypeName "BRAINS-RLE-MASK"
    set NumDims 3
    set XDims 256
    set YDims 256
    set ZDims 192
    set NumRes 3
    set XRes 1.015625
    set YRes 1.015625
    set ZRes 1.015625
    set SubTestDes "load $MaskTypeName test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] ==0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_save_mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk cran]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk brains1 $TestMaskID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk brains1 $TestMaskID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS RLE Masks ###########################################
    set SubTestDes "Save BRAINS Mask test"
    set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk brains1 $TestMaskID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveMaskTest "BRAINS" $TestMaskID ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk $LogFile $ModuleName

        set SubTestDes "BRAINS invalid filename test"
        set errorTest [b2_save_mask /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.junk brains1 $TestMaskID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS invalid filter-suffix test"
        set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/cran_mask.junk brains1 $TestMaskID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }


    ############################### BRAINS2 Octree Masks ###########################################
    set SubTestDes "Save BRAINS2 Mask test"
    set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/junk.mask brains2 $TestMaskID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveMaskTest "BRAINS2" $TestMaskID ${OUTPUT_DIR}/TEST/10_ACPC/junk.mask $LogFile $ModuleName

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2_save_mask /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.mask brains2 $TestMaskID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2_save_mask ${OUTPUT_DIR}/TEST/10_ACPC/junk.mask brains2 $TestMaskID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

