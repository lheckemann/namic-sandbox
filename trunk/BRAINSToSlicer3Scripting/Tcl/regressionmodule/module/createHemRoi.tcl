# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2_create_hem-roi"
# \fn        proc createHemRoi {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# create hemispheric roi.
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc createHemRoi {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR


    set ModuleName "createHemRoi"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_create_hem-roi command"
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
    set SubTestDes "Create Hem ROI Load Image test"
    set ImageId [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] ==  0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_create_hem-roi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_create_hem-roi $ImageId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_create_hem-roi $ImageId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2_create_hem-roi 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes




    ############################ BRAINS2 Create Hem ROI ########################################
    set SubTestDes "B2 CREATE HEM-ROI test (1)"
    set TestRoiId [b2_create_hem-roi $ImageId]
    ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes

    ############################### Load Previous ROI ###########################################
    set SubTestDes "Create Hem ROI Load ROI(1) test"
    set OrigRoiId [b2_load_roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/surf_hem.yroi]
    ReportTestStatus $LogFile  [ expr {$OrigRoiId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Create Hem ROI Convert Original ROI(1) test"
    set maskOrig [ b2_extrude_roi $OrigRoiId 1 0 0]
    ReportTestStatus $LogFile  [ expr {$maskOrig != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Create Hem ROI Convert New ROI(1) test"
    set maskNew  [ b2_extrude_roi $TestRoiId 1 0 0]
    ReportTestStatus $LogFile  [ expr {$maskNew != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Create Hem ROI(1) Difference mask test"
    set diffMask [b2_xor_masks $maskOrig $maskNew]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set volume [b2_measure_volume_mask $diffMask]
    set SubTestDes "Create Hem ROI Difference Volume ([lindex [lindex $volume 0] 1] CCs region of error) test"
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $OrigRoiId ] != -1 } ] $ModuleName "Destroying roi $OrigRoiId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $maskOrig ] != -1 } ] $ModuleName "Destroying mask $maskOrig"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $maskNew ] != -1 } ] $ModuleName "Destroying mask $maskNew"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"

    ############################ BRAINS2 Create Hem ROI ########################################
    set SubTestDes "B2 CREATE HEM-ROI test (2)"
    set TestRoiId [b2_create_hem-roi $ImageId x-loc= 64]
    ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes

    ############################### Load Previous ROI ###########################################
    set SubTestDes "Create Hem ROI Load ROI(2) test"
    set OrigRoiId [b2_load_roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/surf_hem_64.yroi]
    ReportTestStatus $LogFile  [ expr {$OrigRoiId != -1 } ] $ModuleName $SubTestDes]

    set SubTestDes "Create Hem ROI Convert Original ROI(2) test"
    set maskOrig [ b2_extrude_roi $OrigRoiId 1 0 0]
    ReportTestStatus $LogFile  [ expr {$maskOrig != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Create Hem ROI Convert New ROI(2) test"
    set maskNew  [ b2_extrude_roi $TestRoiId 1 0 0]
    ReportTestStatus $LogFile  [ expr {$maskNew != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Create Hem ROI(2) Difference mask test"
    set diffMask [b2_xor_masks $maskOrig $maskNew]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set volume [b2_measure_volume_mask $diffMask]
    set SubTestDes "Create Hem ROI Difference Volume ([lindex [lindex $volume 0] 1] CCs region of error) test (2)"
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $OrigRoiId ] != -1 } ] $ModuleName "Destroying roi $OrigRoiId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $maskOrig ] != -1 } ] $ModuleName "Destroying mask $maskOrig"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $maskNew ] != -1 } ] $ModuleName "Destroying mask $maskNew"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"

    ############################ BRAINS2 Create Hem ROI ########################################
    set SubTestDes "B2 CREATE HEM-ROI test (2)"
    set TestRoiId [b2_create_hem-roi $ImageId x-loc= 2048]
    ReportTestStatus $LogFile  [ expr {$TestRoiId == -1 } ] $ModuleName $SubTestDes


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImageId ] != -1 } ] $ModuleName "Destroying image $ImageId"


    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

