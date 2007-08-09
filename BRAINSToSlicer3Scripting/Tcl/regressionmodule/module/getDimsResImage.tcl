# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2 get dims|res image commands
# \fn        proc getDimsResImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 get dims|res image commands
#
# To Do
#------------------------------------------------------------------------
# Requires the image loader to have already been sourced to define CoreImageTest
#

proc getDimsResImage {pathToRegressionDir dateString} {

    set ModuleName "getDimsResImage"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 get dims|res image commands"
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
    set ImageType "T1"
    set ImageMin 0.000000
    set ImageMax  2337.000000
    set Dimensions "256 256 124"
    set Resolutions "1.015625 1.015625 1.500000"
    set SubTestDes "Get Image Dims/Res load $ImageType test"
    set TestImageID [b2 load image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099]
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "(dims) required arguement test"
    set errorTest [b2_get_dims_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) arguement number test"
    set errorTest [b2_get_dims_image $TestImageID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) optional arguement test"
    set errorTest [b2_get_dims_image $TestImageID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) invalid mask test"
    set errorTest [b2_get_dims_image -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    ########## Res Command
    set SubTestDes "(res) required arguement test"
    set errorTest [b2_get_res_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) arguement number test"
    set errorTest [b2_get_res_image $TestImageID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) optional arguement test"
    set errorTest [b2_get_res_image $TestImageID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) invalid mask test"
    set errorTest [b2_get_res_image -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    CoreImageTest "Image Dims/Res" $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}
