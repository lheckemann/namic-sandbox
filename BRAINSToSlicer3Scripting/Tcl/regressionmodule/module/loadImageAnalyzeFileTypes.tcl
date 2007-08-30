# \author    Hans J. Johnson"
# \date        $Date: 2005-04-26 13:23:02 -0500 (Tue, 26 Apr 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImageAnalyzeFileTypes {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImageAnalyzeFileTypes {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImageAnalyzeFileTypes"
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
############################### Noncompliant images ###########################################
## Set known information about this test image

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192"
        set Resolutions "1.015625 1.015625 1.015625"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Noncompliant/10_ACPC/char_bigendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "Classified"
        set ImageMin 0.000000
        set ImageMax  250.000000
        set Dimensions "320 320 384"
        set Resolutions "0.500000 0.500000 0.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Noncompliant/10_ACPC/char_littleendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  1.000000
        set Dimensions "256 17 256"
        set Resolutions "0.937500 6.500000 0.937500"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Noncompliant/10_ACPC/float_bigendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  1.000000
        set Dimensions "320 320 384"
        set Resolutions "0.500000 0.500000 0.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Noncompliant/10_ACPC/float_littleendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  2379.000000
        set Dimensions "256 17 256"
        set Resolutions "0.937500 6.500000 0.937500"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Noncompliant/10_ACPC/signed16_bigendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  300.000000
        set Dimensions "256 256 124"
        set Resolutions "0.703125 0.703125 1.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Noncompliant/10_ACPC/signed16_littleendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

############################### Strict Analyze Image ###########################################
    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192"
        set Resolutions "1.015625 1.015625 1.015625"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Compliant/10_ACPC/char_bigendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "Classified"
        set ImageMin 0.000000
        set ImageMax  250.000000
        set Dimensions "320 320 384"
        set Resolutions "0.500000 0.500000 0.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Compliant/10_ACPC/char_littleendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  1.000000
        set Dimensions "256 17 256"
        set Resolutions "0.937500 6.500000 0.937500"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Compliant/10_ACPC/float_bigendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  1.000000
        set Dimensions "320 320 384"
        set Resolutions "0.500000 0.500000 0.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Compliant/10_ACPC/float_littleendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  2379.000000
        set Dimensions "256 17 256"
        set Resolutions "0.937500 6.500000 0.937500"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Compliant/10_ACPC/signed16_bigendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

    set ImageTypeName "Analyze"
        set ImageType "UNKNOWN"
        set ImageMin 0.000000
        set ImageMax  300.000000
        set Dimensions "256 256 124"
        set Resolutions "0.703125 0.703125 1.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/AnalyzeTypes/Compliant/10_ACPC/signed16_littleendian.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }



########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

