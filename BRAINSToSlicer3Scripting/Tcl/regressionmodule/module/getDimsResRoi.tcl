# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2 get dims|res roi commands
# \fn        proc getDimsResImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 get dims|res roi commands
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getDimsResRoi {pathToRegressionDir dateString} {

    set ModuleName "getDimsResRoi"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 get dims|res roi commands"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################




    ############################### Load an Image ###########################################
    ## Set known information about mask
    set SubTestDes "Get Image Dims/Res Load 10_ACPC ROI Test"
    set TestRoiID [b2_load_roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/101tricia_rcaud.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "(dims) required arguement test"
    set errorTest [b2_get_dims_roi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) arguement number test"
    set errorTest [b2_get_dims_roi $TestRoiID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) optional arguement test"
    set errorTest [b2_get_dims_roi $TestRoiID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) invalid roi test"
    set errorTest [b2_get_dims_roi -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ########## Res Command
    set SubTestDes "(res) required arguement test"
    set errorTest [b2_get_res_roi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) arguement number test"
    set errorTest [b2_get_res_roi $TestRoiID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) optional arguement test"
    set errorTest [b2_get_res_roi $TestRoiID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) invalid roi test"
    set errorTest [b2_get_res_roi -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set RoiDims [ b2_get_dims_roi $TestRoiID]
    set RoiRes [ b2_get_res_roi $TestRoiID]

    set ExpectDims {256 256 192}
    set ExpectRes {1.015625  1.015625 1.015625}
    for {set i 0} {$i < 3} {incr i} {
        set SubTestDes "10_ACPC Get DIMS ROI Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $RoiDims $i] == [lindex $ExpectDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "10_ACPC Get RES ROI Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $RoiRes $i] == [lindex $ExpectRes $i] } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"

    set SubTestDes "Get Image Dims/Res Load 05_ACPC ROI Test"
    set TestRoiID [b2_load_roi $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/brain.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set RoiDims [ b2_get_dims_roi $TestRoiID]
    set RoiRes [ b2_get_res_roi $TestRoiID]

    set ExpectDims {320 320 384}
    set ExpectRes {0.500000  0.500000 0.500000}
    for {set i 0} {$i < 3} {incr i} {
        set SubTestDes "05_ACPC Get DIMS ROI Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $RoiDims $i] == [lindex $ExpectDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "05_ACPC Get RES ROI Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $RoiRes $i] == [lindex $ExpectRes $i] } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"



    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

