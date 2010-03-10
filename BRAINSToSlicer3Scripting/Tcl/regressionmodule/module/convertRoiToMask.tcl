# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2_convert_roi_to_mask"
# \fn        proc convertRoiToMask {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_convert_mask_to_roi command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc convertRoiToMask {pathToRegressionDir dateString} {

    set ModuleName "convertRoiToMask"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_convert_roi_to_mask command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
##  It does not matter what the roi's names are, just as long as they are unique.
    set B2_FORCE_QUIET_DEFAULTS 1


########################################
########################################
# Run Tests
########################################
########################################


    ############################### Load a ROI ###########################################
    set SubTestDes "Convert ROI to MASK Load ROI test"
    set RoiId [b2_load_roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/96tricia_rcaud.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$RoiId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load a Mask ###########################################
    set SubTestDes "Convert ROI to MASK Load Mask test"
    set MaskId [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/ANON013_tricia_rcaud.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_convert_roi_to_mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_convert_roi_to_mask $RoiId junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_convert_roi_to_mask $RoiId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ########################## BRAINS2 Convert Roi To Mask ##############################
    set SubTestDes "B2 CONVERT ROI to Mask test"
    set TestMaskId [b2_convert_roi_to_mask $RoiId name= Mask1]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT ROI TO MASK Difference mask test"
        set diffMask [ b2_xor_masks $MaskId $TestMaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT ROI TO MASK Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {$volume != 0.000000 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    }


    ########################## BRAINS2 Convert Roi To Mask Range Check ##############################
    set SubTestDes "B2 CONVERT ROI TO MASK Slice Range Check test"
    set TestMaskId [b2_convert_roi_to_mask $RoiId min= 95 max= 97 name= Mask2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT ROI TO MASK Slice Range Check roi generation test"
        set TestRoiId [b2_convert_mask_to_roi coronal $MaskId min= 95 max= 97 name= Mask3]
        ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT ROI TO MASK Slice Range Check mask generation test"
        set RoiMaskId [b2_convert_roi_to_mask $TestRoiId name= Mask4]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT ROI TO MASK Slice Range Check Difference mask test"
        set diffMask [ b2_xor_masks $RoiMaskId $TestMaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT ROI TO MASK Slice Range Check Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {$volume != 0.000000 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $RoiMaskId ] != -1 } ] $ModuleName "Destroying mask $RoiMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    }


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $MaskId ] != -1 } ] $ModuleName "Destroying mask $MaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $RoiId ] != -1 } ] $ModuleName "Destroying roi $RoiId"


    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}
