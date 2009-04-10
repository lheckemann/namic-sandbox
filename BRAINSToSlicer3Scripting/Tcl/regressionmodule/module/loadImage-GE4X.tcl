proc loadImage-GE4X {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImage-GE4X"
        set ModuleAuthor "Vincent A. Magnotta"
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

############################### GE 4x Images ###########################################
## Set known information about this test image
        set ImageTypeName "GE-4x"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax 200.000000
        set Dimensions "256 256 124"
        set Resolutions "1.015625 1.015625 1.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096]
#only continue upon success
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Loaded image, now testing"
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

##################### Test Loading Various Data Types Image ###########################################
    set SubTestDes "load GE 4x US 16 bit T1 test"
        set ge4xU16bitT1 [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096 data-type= unsigned-16bit]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge4xU16bitT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 4x US 16 bit T1 image min"
                set imageMin [b2_image_min $ge4xU16bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 4x US 16 bit T1 image max"
                set imageMax [b2_image_max $ge4xU16bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 200.000000}   ] $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ge4xU16bitT1 ] != -1 } ] $ModuleName "Destroying image $ge4xU16bitT1"
        }

    set SubTestDes "load GE 4x S 16 bit T1 test"
        set ge4xS16bitT1 [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096 data-type= signed-16bit]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge4xS16bitT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 4x S 16 bit T1 image min"
                set imageMin [b2_image_min $ge4xS16bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 4x S 16 bit T1 image max"
                set imageMax [b2_image_max $ge4xS16bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 200.000000}   ] $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ge4xS16bitT1 ] != -1 } ] $ModuleName "Destroying image $ge4xS16bitT1"
        }


########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

