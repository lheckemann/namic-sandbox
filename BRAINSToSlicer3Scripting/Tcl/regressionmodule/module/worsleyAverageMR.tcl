# \author    Vincent A. Magnotta
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2 worsley average-mr"
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


proc worsleyAverageMR {pathToRegressionDir dateString} {


    set ModuleName "worsleyAverageMR"
    set ModuleAuthor "Vincent A. Magnotta"
    set ModuleDescription "Test the b2 worsley average-mr command"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]

########################################
########################################
# Run Tests
########################################
########################################


    ############################### Load an Image ###########################################
    set SubTestDes "Worsley Average MR - Load Image test (1)"
    set ImageId(1) [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId(1) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Resample Image - Load Talairach Parameters test (1)"
    set TalairachId(1) [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TalairachId(1) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Worsley Average MR - Load Image test (2)"
    set ImageId(2) [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/10_ACPC/ANON0007_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId(2) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Resample Image - Load Talairach Parameters test (2)"
    set TalairachId(2) [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TalairachId(2) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Worsley Average MR - Load Image test (3)"
    set ImageId(3) [b2_load_image $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId(3) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Resample Image - Load Talairach Parameters test (3)"
    set TalairachId(3) [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TalairachId(3) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Worsley Average MR - Load Image test (4)"
    set ImageId(4) [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/ANON0019_05_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId(4) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Resample Image - Load Talairach Parameters test (4)"
    set TalairachId(4) [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/Talairach.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TalairachId(4) != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    for {set i 1} {$i <= 4} {incr i} {
        set SubTestDes "Resample Worsley MR Image - $i"
        set worsMrImage($i) [b2 resample worsley-mr $ImageId($i) $TalairachId($i)]
        ReportTestStatus $LogFile  [ expr {$worsMrImage($i) != -1 } ] $ModuleName $SubTestDes
    }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 worsley average-mr]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set worsImages [format "{%s %s %s %s}" $worsMrImage(1) $worsMrImage(2) $worsMrImage(3) $worsMrImage(4)]
    set errorTest [b2 worsley average-mr $worsImages junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 resample worsley-mr $worsImages junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 RESAMPLE Worsley-MR test"
    set TestImageId [eval "b2 worsley average-mr $worsImages study= REGRESSION path= ${OUTPUT_DIR}"]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 RESAMPLE Worsley-MR test - Mask Generation"
    set TestMaskId [b2_threshold_image [lindex $TestImageId 0] 100]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
#   SGI: 1970.431105
    set SubTestDes "B2 RESAMPLE IMAGE test - Image Measurement -- is [lindex [lindex $result 0] 1] nearly equal to 1867.024723"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 1867.024723) / 1867.024723 < 0.0553856522231 * 1.01} ] $ModuleName $SubTestDes
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image [lindex $TestImageId 0] ] != -1 } ] $ModuleName "Destroying image [lindex $TestImageId 0]"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image [lindex $TestImageId 1] ] != -1 } ] $ModuleName "Destroying image [lindex $TestImageId 1]"


    for {set i 1} {$i <= 4} {incr i} {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImageId($i) ] != -1 } ] $ModuleName "Destroying image $ImageId($i)"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $worsMrImage($i) ] != -1 } ] $ModuleName "Destroying image $worsMrImage($i)"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TalairachId($i) ] != -1 } ] $ModuleName "Destroying talairach-parameters $TalairachId($i)"
    }

    return [ StopModule  $LogFile $ModuleName ]
}

