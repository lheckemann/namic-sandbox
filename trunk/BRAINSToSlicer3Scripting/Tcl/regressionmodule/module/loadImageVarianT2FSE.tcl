# \author    Hans J. Johnson"
# \date        $Date: 2005-04-26 13:23:02 -0500 (Tue, 26 Apr 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImageVarianT2FSE {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImageVarianT2FSE {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImageVarianT2FSE"
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

############################### Varian T2 FSE Images ###########################################
## Set known information about this test image
    set ImageTypeName "Varian FDF T2"
        set ImageType "PD"
        set ImageMin 0.000000
        set ImageMax 0.052734
        set Dimensions "512 24 512 1"
        set Resolutions "0.390625 5.000000 0.390625 10000.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/varian/TEST/fse_T2/image0009.fdf data-type= float-single filter-suffix= "-c on"]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

##################### Test Loading Various Data Types Image ###########################################

    set SubTestDes "load Varian 8 bit T2 test"
        set varian8bitT2 [b2_load_image $pathToRegressionDir/SGI/MR/varian/TEST/fse_T2/image0009.fdf filter-suffix= "-c on"]
        if { [ReportTestStatus $LogFile  [ expr  {$varian8bitT2 != -1}   ] $ModuleName $SubTestDes ]} {

            set SubTestDes "Varian 8 bit T2 image min"
                set imageMin [b2_image_min $varian8bitT2]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "Varian 8 bit T2 image max"
                set imageMax [b2_image_max $varian8bitT2]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 0.052734}   ] $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $varian8bitT2 ] != -1 } ] $ModuleName "Destroying image $varian8bitT2"
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

