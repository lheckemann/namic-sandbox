# \author    Hans J. Johnson"
# \date        $Date: 2006-08-11 13:54:53 -0500 (Fri, 11 Aug 2006) $
# \brief    This module tests the loading of various Transform
#             file formats
# \fn        proc loadTransform {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS Resample Transform File
# Load BRAINS2 Transform File
# Load an AIR Transform File, MR5 and MR6
# Load an surface fit transform
# Load a Bookstein Transform
#
# To Do
#------------------------------------------------------------------------
# Add tests to verify loaded transforms


proc loadTransform {pathToRegressionDir dateString} {

    set ModuleName "loadTransform"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_load_transform command and loading various Transform file formats"
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


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_load_transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_transform $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009.linear.air16 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_transform $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009.linear.air16 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_transform /invalid_directory_name/ANON0009.linear.air16]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes




    ############################### BRAINS Resample Transform ###########################################
    set SubTestDes "BRAINS Resample Transform Load test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # set image [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.044]
        # set xfrmImage [ b2_set_transform $b1Xfrm image $image]
        # Snap a Picture Here
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
        # ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $image ] != -1 } ] $ModuleName "Destroying image $image"
    }


    ############################### AIR Linear Transform ###########################################
    set SubTestDes "AIR Transform Load test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009.linear.air16]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # set image [b2_load_image $pathToRegressionDir/SGI/MR/5x-B2/TEST/31_003/148313.003.056]
        # set xfrmImage [ b2_set_transform $b1Xfrm image $image]
        # Snap a Picture Here
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
        # ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $image ] != -1 } ] $ModuleName "Destroying image $image"
    }


    ############################### BRAINS2 Transform ###########################################
    set SubTestDes "BRAINS2 Transform Load test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/T1_ACPC.xfrm]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # set image [b2_load_image $pathToRegressionDir/SGI/MR/5x-B2/TEST/15_002/148313.003.056]
        # set xfrmImage [ b2_set_transform $b1Xfrm image $image]
        # Snap a Picture Here
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
        # ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $image ] != -1 } ] $ModuleName "Destroying image $image"
    }

    ############################### BRAINS2 Transform With FIlter ################################
    set SubTestDes "BRAINS2 Transform with filter Load test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/T1_ACPC.xfrm filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # set image [b2_load_image $pathToRegressionDir/SGI/MR/5x-B2/TEST/15_002/148313.003.056]
        # set xfrmImage [ b2_set_transform $b1Xfrm image $image]
        # Snap a Picture Here
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
        # ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $image ] != -1 } ] $ModuleName "Destroying image $image"
    }

    ############################### SURFACE FIT Transform ###########################################
    set SubTestDes "Surface Fit Transform Load test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/pet_images/ge-4096/TEST/pcanonimiz__457.imf.par]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # set image [b2_load_image $pathToRegressionDir/SGI/MR/5x-B2/TEST/15_002/148313.003.056]
        # set xfrmImage [ b2_set_transform $b1Xfrm image $image]
        # Snap a Picture Here
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
        # ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $image ] != -1 } ] $ModuleName "Destroying image $image"
    }

    ############################### BRAINS2 Bookstein Transform  ################################
    set SubTestDes "BRAINS2 Bookstein Transform test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/MR/B2-Bookstein/TEST/10_ACPC/ANON001_bookstein.wrp ]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # set image [b2_load_image $pathToRegressionDir/SGI/MR/5x-B2/TEST/15_002/148313.003.056]
        # set xfrmImage [ b2_set_transform $b1Xfrm image $image]
        # Snap a Picture Here
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
        # ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $image ] != -1 } ] $ModuleName "Destroying image $image"
    }


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

