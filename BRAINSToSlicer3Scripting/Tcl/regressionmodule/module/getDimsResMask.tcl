# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2 get dims|res mask commands
# \fn        proc getDimsResMask {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 get dims|res commands
#
# To Do
#------------------------------------------------------------------------
# Requires the mask loader to have already been sourced to define CoreMaskTest
#


proc getDimsResMask {pathToRegressionDir dateString} {

    set ModuleName "getDimsResMask"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 get dims|res mask commands"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    ############################### BRAINS RLE Masks ###########################################
    ## Set known information about mask
    set NumDims 3
    set XDims 256
    set YDims 256
    set ZDims 192
    set NumRes 3
    set XRes 1.015625
    set YRes 1.015625
    set ZRes 1.015625
    set SubTestDes "Get DIMS/RES load Mask test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "(dims) required arguement test"
    set errorTest [b2_get_dims_mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) arguement number test"
    set errorTest [b2_get_dims_mask $TestMaskID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) optional arguement test"
    set errorTest [b2_get_dims_mask $TestMaskID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) invalid mask test"
    set errorTest [b2_get_dims_mask -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ########## Res Command
    set SubTestDes "(res) required arguement test"
    set errorTest [b2_get_res_mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) arguement number test"
    set errorTest [b2_get_res_mask $TestMaskID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) optional arguement test"
    set errorTest [b2_get_res_mask $TestMaskID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) invalid mask test"
    set errorTest [b2_get_res_mask -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    CoreMaskTest "get Dims/Res mask" $TestMaskID $NumDims $XDims $YDims $ZDims $NumRes $XRes $YRes $ZRes $LogFile $ModuleName $SubTestDes


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"

    return [ StopModule  $LogFile $ModuleName ]
}

