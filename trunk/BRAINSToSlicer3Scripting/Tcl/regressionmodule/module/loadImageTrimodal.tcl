# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImageTrimodal {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImageTrimodal {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImageTrimodal"
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

############################### Trimodal Image ###########################################
## Set known information about this test image
        set ImageTypeName "Analyze Trimodal"
        set ImageType "T1 T2 PD"
        set ImageMin "0.000000 0.000000 0.000000"
        set ImageMax "255.000000 255.000000 255.000000"
        set Dimensions "256 256 192"
        set Resolutions "1.015625 1.015625 1.015625"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set b2cmd [format "b2_load_image {%s %s %s}" \
              $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr \
                    $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr \
                    $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
        set TestImageID [eval $b2cmd]
#only continue upon success
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
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

