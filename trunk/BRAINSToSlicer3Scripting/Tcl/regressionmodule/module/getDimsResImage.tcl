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
    ## Set known information about image
    set ImageType "T1"
    set ImageMin 0.000000
    set ImageMax  4095.000000
    set Dimensions "256 256 124"
    set Resolutions "1.015625 1.015625 1.500000"
    set SubTestDes "Get Image Dims/Res load $ImageType test"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099]
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid argumenets
    set SubTestDes "(dims) required argumenet test"
    set errorTest [b2_get_dims_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) argumenet number test"
    set errorTest [b2_get_dims_image $TestImageID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) optional argumenet test"
    set errorTest [b2_get_dims_image $TestImageID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(dims) invalid image test"
    set errorTest [b2_get_dims_image -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    ########## Res Command
    set SubTestDes "(res) required argumenet test"
    set errorTest [b2_get_res_image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) argumenet number test"
    set errorTest [b2_get_res_image $TestImageID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) optional argumenet test"
    set errorTest [b2_get_res_image $TestImageID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "(res) invalid image test"
    set errorTest [b2_get_res_image -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    CoreImageTest "Image Dims/Res" $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"

    set returnvalues [ StopModule  $LogFile $ModuleName ];
    puts "============================== ${returnvalues}"
    return ${returnvalues};
}
