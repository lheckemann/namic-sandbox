# \author    Hans J. Johnson"
# \date        $Date: 2006-08-30 12:55:44 -0500 (Wed, 30 Aug 2006) $
# \brief    This module tests the "b2_clip_image"
# \fn        proc clipImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_clip_image command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc clipImage {pathToRegressionDir dateString} {


    set ModuleName "clipImage"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_clip_image command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    ############################### Load an Image ###########################################
    set SubTestDes "Clip Image - Load Image test"
    set ImageId [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load a Mask ###########################################
    set SubTestDes "Clip Image - Load Mask test"
    set MaskId [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2_clip_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (2)"
    set errorTest [b2_clip_image $ImageId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2_clip_image $ImageId $MaskId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2_clip_image $ImageId $MaskId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2_clip_image 65536 $MaskId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid mask test"
    set errorTest [b2_clip_image $ImageId 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Create Histogram ########################################
    set SubTestDes "B2 CLIP IMAGE test"
    set TestImageId [b2_clip_image $ImageId $MaskId]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CLIP IMAGE - Not mask test"
    set TestMaskId [b2_not_mask $MaskId]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CLIP IMAGE - Measure background test"
    set result [b2_measure_image_mask $TestMaskId $TestImageId]
    ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CLIP IMAGE - Check background value mean test"
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $result 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CLIP IMAGE - Check background value stddev test"
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $result 1] 1] == 0.000000 } ] $ModuleName $SubTestDes


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $MaskId ] != "-1" }  ] $ModuleName "Destroying mask $MaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImageId ] != "-1" } ] $ModuleName "Destroying image $ImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != "-1" } ] $ModuleName "Destroying image $TestImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != "-1" } ] $ModuleName "Destroying mask $TestMaskId"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

