# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:59:30 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImageNIfTIT1 {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImageNIfTIT1 {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImageNIfTIT1"
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

############################### NIfTI T1 Image ###########################################
## Set known information about this test image
    set ImageTypeName "NIfTI"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192"
        set Resolutions "1.015625 1.015625 1.015625"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.nii.gz]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

############################### Strict NIfTI Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

