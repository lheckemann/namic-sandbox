# \author    Greg Harris"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2 resample worsley-mr"
# \fn        proc resampleImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 resample worsley-mr command
#
# To Do
#------------------------------------------------------------------------
# None
#


proc resampleWorsleyMR {pathToRegressionDir dateString} {


    set ModuleName "resampleWorsleyMR"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 resample worsley-mr command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################

    ############################### Load an Image ###########################################
    set SubTestDes "Resample Image - Load Image test"
    set ImageId [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Resample Image - Load Talairach Parameters test"
    set TalairachId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 resample worsley-mr]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 resample worsley-mr $ImageId $TalairachId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 resample worsley-mr $ImageId $TalairachId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2 resample worsley-mr 65536 $TalairachId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid Talairach Parameter test"
    set errorTest [b2 resample worsley-mr $ImageId 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 RESAMPLE Worsley-MR test"
    set TestImageId [b2 resample worsley-mr $ImageId $TalairachId]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 RESAMPLE Worsley-MR test - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 100]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set standard 1814.667518846690655
    set SubTestDes "B2 RESAMPLE IMAGE test - Image Measurement - does [lindex [lindex $result 0] 1] approximate $standard"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard < 0.00001} ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 RESAMPLE Worsley-MR test with options"
    set TestImageId [b2 resample worsley-mr $ImageId $TalairachId xdim= 128 ydim= 80 zdim= 128 hist-equal= on]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 RESAMPLE Worsley-MR test with options- Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 100]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set standard 1814.667518846690655
    set SubTestDes "B2 RESAMPLE IMAGE test - Image Measurement - does [lindex [lindex $result 0] 1] approximate $standard"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard < 0.00001} ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 RESAMPLE Worsley-MR test no histogram equalization"
    set TestImageId [b2 resample worsley-mr $ImageId $TalairachId xdim= 128 ydim= 128 zdim= 128 hist-equal= off]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 RESAMPLE Worsley-MR test no histogram equalization Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 60]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set standard 1498.442462087259628
    set SubTestDes "B2 RESAMPLE IMAGE test no histogram equalization- Image Measurement - does [lindex [lindex $result 0] 1] approximate $standard"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard < 0.00001} ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImageId ] != -1 } ] $ModuleName "Destroying image $ImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TalairachId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TalairachId"

    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

