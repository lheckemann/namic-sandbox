# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the "b2_convert_mask_to_roi"
# \fn        proc convertMaskToRoi {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_convert_mask_to_roi command
#
# Note: Conversion of a Mask to an ROI back to a mask may produce
#         different measurements based on the plane that the ROI
#        is generated. Various holes in the mask be be filled
#        as seen in this test.
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc convertMaskToRoi {pathToRegressionDir dateString} {

    set ModuleName "convertMaskToRoi"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_convert_mask_to_roi command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]

#        close $LogFile
#        set LogFile stderr

########################################
########################################
# Run Tests
########################################
########################################


    ############################### Load a Mask ###########################################
    set SubTestDes "Convert Mask To ROI Load Mask test"
    set MaskId [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 convert MASK TO ROI]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 convert MASK TO ROI coronal]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 convert MASK TO ROI coronal $MaskId junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 convert MASK TO ROI coronal $MaskId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ########################## BRAINS2 Convert Mask To Coronal ROI ##############################
    set SubTestDes "B2 CONVERT MASK To ROI Coronal test"
    set TestRoiId [b2_convert_mask_to_roi coronal $MaskId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT MASK TO ROI Conversion to mask test"
        set TestMaskId [b2_convert_roi_to_mask $TestRoiId]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Difference mask test"
        set diffMask [ b2_xor_masks $MaskId $TestMaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes
        puts "$volume [llength $volume]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    }

    ########################## BRAINS2 Convert Mask To Sagittal ROI ##############################
    set SubTestDes "B2 CONVERT MASK TO ROI Sagittal test"
    set TestRoiId [b2_convert_mask_to_roi sagittal $MaskId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT MASK TO ROI Sagittal Conversion to mask test"
        set TestMaskId [b2_convert_roi_to_mask $TestRoiId]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Sagittal Difference mask test"
        set diffMask [ b2_xor_masks $MaskId $TestMaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Sagittal Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $volume 0] 1] - 0.826565) / 0.826565 < 0.0001 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    }

    ########################## BRAINS2 Convert Mask To Axial ROI ##############################
    set SubTestDes "B2 CONVERT MASK TO ROI Axial test"
    set TestRoiId [b2_convert_mask_to_roi axial $MaskId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT MASK TO ROI Axial Conversion to mask test"
        set TestMaskId [b2_convert_roi_to_mask $TestRoiId]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Axial Difference mask test"
        set diffMask [ b2_xor_masks $MaskId $TestMaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Axial Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $volume 0] 1] - 4.812726) / 4.812726 < 0.0001 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    }


    ########################## BRAINS2 Convert Mask To ROI Range Check ##############################
    set SubTestDes "B2 CONVERT MASK TO ROI Coronal Slice Range Check test"
    set TestRoiId [b2_convert_mask_to_roi coronal $MaskId min= 95 max= 97]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Range Check mask generation test(1)"
        set RoiMaskId [b2_convert_roi_to_mask $TestRoiId min= 95 max= 97]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Range Check mask generation test(2)"
        set TestMaskId [b2_convert_roi_to_mask $TestRoiId min= 95 max= 97]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Range Check Difference mask test"
        set diffMask [ b2_xor_masks $RoiMaskId $TestMaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Range Check Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $RoiMaskId ] != -1 } ] $ModuleName "Destroying mask $RoiMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
    }

    ########################## BRAINS2 Convert Mask To ROI Slice Skip ##############################
    set SubTestDes "B2 CONVERT MASK TO ROI Coronal Slice Skip Check test"
    set TestRoiId [b2_convert_mask_to_roi coronal $MaskId skip= 2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Slice skip mask generation test"
        set TestMaskId [b2_convert_roi_to_mask $TestRoiId]
        ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Slice Skip Difference mask test"
        set diffMask [ b2_xor_masks $TestMaskId $MaskId]
        ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "B2 CONVERT MASK TO ROI Coronal Slice Skip Difference mask volume test"
        set volume [b2_measure_volume_mask $diffMask]
        ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $volume 0] 1] - 11.657818) / 11.657818  < 0.0001 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ########################## BRAINS2 Convert Class Image to Discrete ##############################
    set SubTestDes "B2 CONVERT MASK TO ROI Coronal Slice Skip Check test"
    set TestRoiId [b2_convert_mask_to_roi coronal $MaskId skip= 2 name= test.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set name [ b2_get_name roi $TestRoiId]
        ReportTestStatus $LogFile  [ expr {$TestRoiId != "test.zroi" } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $MaskId ] != -1 } ] $ModuleName "Destroying mask $MaskId"


    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

