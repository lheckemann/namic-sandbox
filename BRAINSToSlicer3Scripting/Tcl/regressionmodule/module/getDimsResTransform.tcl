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


proc getDimsResTransform {pathToRegressionDir dateString} {

    set ModuleName "getDimsResTransform"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 get standard|reslice dims|res transform commands"
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
    set SubTestDes "Get Transform Dims/Res Load Transform Test"
    set TestXfrmID [b2_load_transform $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    ########## Standard Dims
    set SubTestDes "(standard-dims) required arguement test"
    set errorTest [b2_get_standard-dims_transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(standard-dims) arguement number test"
    set errorTest [b2_get_standard-dims_transform $TestXfrmID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(standard-dims) optional arguement test"
    set errorTest [b2_get_standard-dims_transform $TestXfrmID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(standard-dims) invalid roi test"
    set errorTest [b2_get_standard-dims_transform -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ########## Standard Res
    set SubTestDes "(standard-res) required arguement test"
    set errorTest [b2_get_standard-res_transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(standard-res) arguement number test"
    set errorTest [b2_get_standard-res_transform $TestXfrmID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(standard-res) optional arguement test"
    set errorTest [b2_get_standard-res_transform $TestXfrmID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(standard-res) invalid roi test"
    set errorTest [b2_get_standard-res_transform -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes




    ############# Reslice Dims
    set SubTestDes "(reslice-dims) required arguement test"
    set errorTest [b2_get_reslice-dims_transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(reslice-dims) arguement number test"
    set errorTest [b2_get_reslice-dims_transform $TestXfrmID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(reslice-dims) optional arguement test"
    set errorTest [b2_get_reslice-dims_transform $TestXfrmID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(reslice-dims) invalid roi test"
    set errorTest [b2_get_reslice-dims_transform -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ########## Reslice Res
    set SubTestDes "(reslice-res) required arguement test"
    set errorTest [b2_get_reslice-res_transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(reslice-res) arguement number test"
    set errorTest [b2_get_standard-res_transform $TestXfrmID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(reslice-res) optional arguement test"
    set errorTest [b2_get_reslice-res_transform $TestXfrmID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(reslice-res) invalid roi test"
    set errorTest [b2_get_reslice-res_transform -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set StandardDims [ b2_get_standard-dims_transform $TestXfrmID]
    set StandardRes [ b2_get_standard-res_transform $TestXfrmID]
    set ResliceDims [ b2_get_reslice-dims_transform $TestXfrmID]
    set ResliceRes [ b2_get_reslice-res_transform $TestXfrmID]

    set ExpectStandardDims {256 256 192}
    set ExpectStandardRes {1.015625  1.015625 1.015625}
    set ExpectResliceDims {256 256 124}
    set ExpectResliceRes {1.015625  1.015625 1.500000}
    for {set i 0} {$i < 3} {incr i} {
        set SubTestDes "Get STANDARD-DIMS Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $StandardDims $i] == [lindex $ExpectStandardDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "Get STANDARD-RES Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $StandardRes $i] == [lindex $ExpectStandardRes $i] } ] $ModuleName $SubTestDes
        set SubTestDes "Get RESLICE-DIMS Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $ResliceDims $i] == [lindex $ExpectResliceDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "Get RESLICE-RES Index ($i)"
        ReportTestStatus $LogFile  [ expr {[lindex $ResliceRes $i] == [lindex $ExpectResliceRes $i] } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmID ] != -1 } ] $ModuleName "Destroying transform $TestXfrmID"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

