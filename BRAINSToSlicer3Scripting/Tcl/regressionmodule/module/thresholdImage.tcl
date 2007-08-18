# \author    Greg Harris
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2_threshold_image"
# \fn        proc createHemRoi {pathToRegressionDir dateString}
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


proc thresholdImage {pathToRegressionDir dateString} {


    set ModuleName "thresholdImage"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_threshold_image command"
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
    set SubTestDes "Threshold Image - Load Image test"
    set ImageId [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_threshold_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2_threshold_image $ImageId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_threshold_image $ImageId 100 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_threshold_image $ImageId 100 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2_threshold_image 65536 100]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Threshold Image ########################################
    set SubTestDes "B2 THRESHOLD IMAGE test - >=50"
    set TestMaskId [b2_threshold_image $ImageId 50]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - >=50 - does [lindex [lindex $result 0] 1] approximate 2893.355570"
    ReportTestStatus $LogFile  [ expr {abs ([lindex [lindex $result 0] 1] - 2893.355570) / 2893.355570 < 0.0001} ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"

    set SubTestDes "B2 THRESHOLD IMAGE test - >50"
    set TestMaskId [b2_threshold_image $ImageId 50 UseOpenInterval= True]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - >50 - does [lindex [lindex $result 0] 1] approximate 2869.884887695"
    ReportTestStatus $LogFile  [ expr {abs ([lindex [lindex $result 0] 1] - 2869.884887695) / 2869.884887695 < 0.0001} ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"

    ############################ BRAINS2 Threshold Image ########################################
    set SubTestDes "B2 THRESHOLD IMAGE test - >=100"
    set TestMaskId [b2_threshold_image $ImageId 100]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - >=100 - does [lindex [lindex $result 0] 1] approximate 954.878785"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 954.878785) / 954.878785 < 0.0001 } ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"

    set SubTestDes "B2 THRESHOLD IMAGE test - (50, 100)"
    set TestMaskId [b2_threshold_image $ImageId 50 UpperThreshold= 100 UseOpenInterval= True]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - (50, 100) - does [lindex [lindex $result 0] 1] approximate 1915.00610256"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 1915.00610256) / 1915.00610256 < 0.0001 } ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ############################ BRAINS2 Threshold Image ########################################
    set SubTestDes "B2 THRESHOLD IMAGE test - 150"
    set TestMaskId [b2_threshold_image $ImageId 150]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - 150 - does [lindex [lindex $result 0] 1] approximate 360.973309"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 360.973309) / 360.973309 < 0.0001 } ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"

    set SubTestDes "B2 THRESHOLD IMAGE test - \[50, 150\]"
    set TestMaskId [b2_threshold_image $ImageId 50 UpperThreshold= 150]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - \[50, 150\] - does [lindex [lindex $result 0] 1] approximate 2538.07288551"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 2538.07288551) / 2538.07288551 < 0.0001 } ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ################## BRAINS2 Threshold Image Functional Flag ##################################
    set SubTestDes "B2 THRESHOLD IMAGE test - Functional Flag - Subtraction"
    set b2cmd [format "b2_subtract_from_constant 50.0 {%s} data-type= float-single" $ImageId ]
    set TestImageId [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 THRESHOLD IMAGE test - Functional Flag"
    set TestMaskId [b2_threshold_image $TestImageId 50 absolute-value= True]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
    set SubTestDes "B2 THRESHOLD IMAGE test - Functional Flag - Volume - does [lindex [lindex $result 0] 1] approximate 3634.967945"
    ReportTestStatus $LogFile  [ expr {abs ([lindex [lindex $result 0] 1] - 3634.967945) / 3634.967945 < 0.0001 } ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImageId ] != -1 } ] $ModuleName "Destroying image $ImageId"

    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

