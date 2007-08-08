# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2 convert rgb-image to hsi"
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 convert class-image to discrete command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc convertRGBImageToHSI {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "convertRGBImageToHSI"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 convert rgb-image to hsi command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


## Set known information about this test image
        set ImageTypeName "Analyze Trimodal"
        set ImageType "T1 T2 PD"
        set ImageMin "0.000000 0.000000 0.000000"
        set ImageMax "255.000000 255.000000 255.000000"
        set Dimensions "256 256 192 1"
        set Resolutions "1.015625 1.015625 1.015625 0.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set b2cmd [format "b2 load image {%s %s %s}" \
              $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr \
                    $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr \
                    $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
        set TestImageID [eval $b2cmd]
#only continue upon success
        ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "BRAINS2 Palette Load test"
    set pal [b2 load palette $B2_TALAIRACH_DIR/../palette/pet.pal]
    ReportTestStatus $LogFile  [ expr {$pal != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "b2 convert rgb-image to hsi $TestImageID 1"
    set new1 [ b2 convert rgb-image to hsi $TestImageID 1 ]
    ReportTestStatus $LogFile  [ expr {$new1 != -1 } ] $ModuleName $SubTestDes

    set SubTestDes " b2 convert rgb-image to hsi $TestImageID -1"
    set new2 [ b2 convert rgb-image to hsi $TestImageID -1 ]
    ReportTestStatus $LogFile  [ expr {$new2 != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "b2 convert rgb-image to hsi $TestImageID 1 palette= $pal"
    set new3 [ b2 convert rgb-image to hsi $TestImageID 1 palette= $pal ]
    ReportTestStatus $LogFile  [ expr {$new3 != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "b2 convert rgb-image to hsi $TestImageID -1 palette= $pal"
    set new4 [ b2 convert rgb-image to hsi $TestImageID -1 palette= $pal ]
    ReportTestStatus $LogFile  [ expr {$new4 != -1 } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $new1 ] != -1 } ] $ModuleName "Destroying image $new1"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $new2 ] != -1 } ] $ModuleName "Destroying image $new2"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $new3 ] != -1 } ] $ModuleName "Destroying image $new3"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $new4 ] != -1 } ] $ModuleName "Destroying image $new4"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy palette $pal ] != -1 } ] $ModuleName "Destroying palette $pal"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

