# \author    Hans J. Johnson"
# \date        $Date: 2005-04-26 13:23:02 -0500 (Tue, 26 Apr 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error

proc loadImage-GE5X {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImage-GE5X"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Test the b2_load_image command and loading various image file formats"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#    close $LogFile
#    set LogFile stderr

########################################
########################################
# Run Tests
########################################
########################################

############################### GE 5x Images ###########################################
## Set known information about this test image
    set ImageTypeName "GE-5x"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  2337.000000
        set Dimensions "256 256 124"
        set Resolutions "1.015625 1.015625 1.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099]
#only continue upon success
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

## Set known information about this test image
    set ImageTypeName "GE-5x"
        set ImageType "T2"
        set ImageMin 0.000000
        set ImageMax  824.000000
        set Dimensions "256 256 59"
        set Resolutions "1.015625 1.015625 3.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/31_003/113766.003.070]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

## Set known information about this test image
    set ImageTypeName "GE-5x"
        set ImageType "PD"
        set ImageMin 0.000000
        set ImageMax  1183.000000
        set Dimensions "256 256 59"
        set Resolutions "1.015625 1.015625 3.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/31_003/113766.003.071]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }


##################### Test Loading Various Data Types Image ###########################################

    set SubTestDes "load GE 5x Float T1 test"
        set ge5xFloatT1 [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099 data-type= float-single]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge5xFloatT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 5x Float T1 image min"
                set imageMin [b2_image_min $ge5xFloatT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 5x Float T1 image max"
                set imageMax [b2_image_max $ge5xFloatT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 2337.000000}   ] $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ge5xFloatT1 ] != -1 } ] $ModuleName "Destroying image $ge5xFloatT1"
        }

    set SubTestDes "load GE 5x US 32 T1 test"
        set ge5xU32bitT1 [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099 data-type= unsigned-32bit]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge5xU32bitT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 5x US 32 T1 image min"
                set imageMin [b2_image_min $ge5xU32bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 5x US 32 T1 image max"
                set imageMax [b2_image_max $ge5xU32bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 2337.000000}   ] $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ge5xU32bitT1 ] != -1 } ] $ModuleName "Destroying image $ge5xU32bitT1"
        }

    set SubTestDes "load GE 5x S 32 T1 test"
        set ge5xS32bitT1 [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099 data-type= signed-32bit]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge5xS32bitT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 5x S 32 T1 image min"
                set imageMin [b2_image_min $ge5xS32bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 5x S 32 T1 image max"
                set imageMax [b2_image_max $ge5xS32bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 2337.000000}   ] $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ge5xS32bitT1 ] != -1 } ] $ModuleName "Destroying image $ge5xS32bitT1"
        }

############################### Strict Analyze Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

