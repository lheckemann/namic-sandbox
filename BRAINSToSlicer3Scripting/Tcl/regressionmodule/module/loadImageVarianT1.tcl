# \author    Hans J. Johnson"
# \date        $Date: 2005-04-26 13:23:02 -0500 (Tue, 26 Apr 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImageVarianT1 {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImageVarianT1 {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImageVarianT1"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Test the b2 load image command and loading various image file formats"
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

############################### Varian T1 Flash Images ###########################################
## Set known information about this test image
    set ImageTypeName "Varian FDF T1"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  0.078125
        set Dimensions "256 64 256 1"
        set Resolutions "0.859375000000000 2.999991893768311 0.859375000000000 9.000000000000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2 load image $pathToRegressionDir/SGI/MR/varian/TEST/mpflash_T1/image0016.fdf data-type= float-single filter-suffix= "-c on"]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }


##################### Test Loading Various Data Types Image ###########################################


############################### Strict Analyze Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

