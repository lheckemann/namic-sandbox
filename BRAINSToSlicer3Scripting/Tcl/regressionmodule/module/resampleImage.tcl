# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the "b2_resample_image"
# \fn        proc resampleImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_threshold_image command
#
# To Do
#------------------------------------------------------------------------
# None
#


proc resampleImage {pathToRegressionDir dateString} {


    set ModuleName "resampleImage"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_resample_image command"
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
    set ImageId [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.042]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Resample Image - Load Transform test"
    set TransformId [b2_load_transform $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_T1.xfrm]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2_resample_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2_resample_image $ImageId 100 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2_resample_image $ImageId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2_resample_image 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    b2_set_transform $TransformId image $ImageId

    ############################ BRAINS2 Resample Image ########################################
    set SubTestDes "B2 RESAMPLE IMAGE test"
    set TestImageId [b2_resample_image $ImageId plane= coronal]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes
    if { $TestImageId != -1 } {
    set SubTestDes "B2 RESAMPLE IMAGE test - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 50]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
    set expect 81.393052
    set SubTestDes "B2 RESAMPLE IMAGE test - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == [ expr {abs([lindex [lindex $result 0] 1] - $expect)/$expect} ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $expect)/$expect < 0.0001} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"
    }

    ############################ BRAINS2 Resample Image - NN ########################################
    set SubTestDes "B2 RESAMPLE IMAGE test - NN"
    set TestImageId [b2_resample_image $ImageId plane= coronal interp= nearest-neighbor]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes
    if { $TestImageId != -1 } {
    set result [b2_measure_image_mask $TestMaskId $TestImageId]
    set expect 82.489904
    set SubTestDes "B2 RESAMPLE IMAGE test - NN - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == [ expr {abs([lindex [lindex $result 0] 1]  - $expect)/$expect} ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1]  - $expect)/$expect < 0.001} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }

    ############################ BRAINS2 Resample Image - Options ########################################
    set SubTestDes "B2 RESAMPLE IMAGE test - Options"
    set TestImageId [b2_resample_image $ImageId min= 126 max= 128 plane= axial storage-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes
    if { $TestImageId != -1 } {
    set SubTestDes "B2 RESAMPLE IMAGE test - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 50]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
    set expect 73.328402
    set SubTestDes "B2 RESAMPLE IMAGE test - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == [ expr {abs([lindex [lindex $result 0] 1]  - $expect)/$expect} ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1]  - $expect)/$expect < 0.0001} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImageId ] != -1 } ] $ModuleName "Destroying image $ImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TransformId ] != -1 } ] $ModuleName "Destroying transform $TransformId"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

