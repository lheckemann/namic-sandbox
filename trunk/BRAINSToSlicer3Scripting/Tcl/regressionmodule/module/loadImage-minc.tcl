# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImage-minc {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error

proc loadImage-minc {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImage-minc"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Test the b2 load image command and loading minc image file format"
        global OUTPUT_DIR;
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

############################### PETFLOW Images ###########################################
## Set known information about this test image
    set ImageTypeName "minc"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000000000000
        set ImageMax 255.0
        set Dimensions "91 91 109"
        set Resolutions "2.000000 2.000000 2.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2 load image $pathToRegressionDir/spm2/canonical/avg305T1.mnc ]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
            ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set anisotropic [b2 load image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/ANON0006_T2_downsampled.hdr ]
    if {[file exists ${OUTPUT_DIR}/ANON0006_T2_downsampled.mnc ] == 1} {
        file delete ${OUTPUT_DIR}/ANON0006_T2_downsampled.mnc
    }
    b2 save image ${OUTPUT_DIR}/ANON0006_T2_downsampled.mnc minc $anisotropic
    b2 destroy image $anisotropic
    set ImageTypeName "minc-T2-anisotropic"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000000000000
        set ImageMax 255.0
        set Dimensions "100 100 66"
        set Resolutions "2.0 2.0 3.0"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2 load image ${OUTPUT_DIR}/ANON0006_T2_downsampled.mnc ]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
            ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }
        

##################### Test Loading Various Data Types Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

