# \author    Hans J. Johnson"
# \date        $Date: 2005-11-07 16:44:43 -0600 (Mon, 07 Nov 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImageAnalyzeT1Strict {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImageNIfTIT1Strict {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImageNIfTIT1Strict"
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

############################### NIfTI T1 Image - Strict Format ###########################################
## Set known information about this test image
    set ImageTypeName "NIfTI-Strict"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192 1"
        set Resolutions "1.015625 1.015625 1.015625 1.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [ b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/strict_analyze_T1.nii.gz ]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
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

