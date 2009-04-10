
This file is outdated.  It has been broken apart into sub modules, and should not be used.


# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImage {pathToRegressionDir dateString} {
    global env
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImage"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Test the b2_load_image command and loading various image file formats"
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#    close $LogFile
#    set LogFile stderr

########################################
########################################
# Run Tests
########################################
########################################
# First Test for invalid arguments
        set SubTestDes "required argument test"
        set ErrorReturnOK [b2_load_image]
        ReportTestStatus $LogFile  [ expr {$ErrorReturnOK == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "argument number test"
        set ErrorReturnOK [b2_load_image SGI/MR/4x-B1/TEST/15_002/19771.002.096 junk= ]
        ReportTestStatus $LogFile  [ expr {$ErrorReturnOK == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "optional argument test"
        set ErrorReturnOK [b2_load_image SGI/MR/4x-B1/TEST/15_002/19771.002.096 junk= test]
        ReportTestStatus $LogFile  [ expr {$ErrorReturnOK == -1 } ] $ModuleName $SubTestDes


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
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }


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
                b2_destroy_image $TestImageID
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
                b2_destroy_image $TestImageID
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
                b2_destroy_image $TestImageID
        }


############################### GE ADW Images ###########################################
## Set known information about this test image
    set ImageTypeName "GE-Adw"
        set ImageType "INVERSION-RECOVERY"
        set ImageMin 0.000000
        set ImageMax  1069.000000
        set Dimensions "256 256 120"
        set Resolutions "0.625000 0.625000 1.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/GE-Adw/TEST/Series2/I.074]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }

############################### GE DICOM Images ###########################################
## Set known information about this test image
    set ImageTypeName "GE-Dicom"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  169.000000
        set Dimensions "256 256 124"
        set Resolutions "0.664062 0.664062 1.500000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/T1_002/006872.002.078]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }

############################### Digital Jacket DICOM Images ###########################################
# These images currently do not load
#######################################################################################################


############################### BRAINS Resampled T1 Images ###########################################
## Set known information about this test image
    set ImageTypeName "Brains Pic"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192"
        set Resolutions "1.015625 1.015625 1.015625"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/MR-PIC/TEST/10_ACPC/ACPC_ANON011_127.pic]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }


############################### Varian T1 Flash Images ###########################################
## Set known information about this test image
    set ImageTypeName "Varian FDF T1"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  0.078125
        set Dimensions "256 64 256 1"
        set Resolutions "0.859375 2.999992 0.859375 9.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/varian/TEST/mpflash_T1/image0016.fdf data-type= float-single filter-suffix= "-c on"]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }


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
                b2_destroy_image $TestImageID
        }
        
############################### Varian EPI fMR Image Set ###########################################
## Set known information about this test image
    set ImageTypeName "Varian FDF EPI"
        set ImageType "T2"
        set ImageMin 0.000000
        set ImageMax 0.025391
        set Dimensions "64 24 64 120"
        set Resolutions "3.437500 5.000000 3.437500 2000.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/varian/TEST/epi_fMR/image0009.fdf data-type= float-single filter-suffix= "-c on"]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }


############################### Analyze Tissue Classified Image ###########################################
## Set known information about this test image
    set ImageTypeName "Analyze Tissue class"
        set ImageType "Classified"
        set ImageMin 0.000000
        set ImageMax  250.000000
        set Dimensions "256 256 192 1"
        set Resolutions "1.016000 1.016000 1.016000 1.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_stereo.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }

############################### Analyze T1 Image ###########################################
## Set known information about this test image
    set ImageTypeName "Analyze"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192 1"
        set Resolutions "1.016000 1.016000 1.016000 1.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }

############################### Analyze T1 Image - Strict Format ###########################################
## Set known information about this test image
    set ImageTypeName "Analyze-Strict"
        set ImageType "T1"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192 1"
        set Resolutions "1.016000 1.016000 1.016000 1.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/strict_analyze_T1.hdr filter-suffix= "-s on"]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }
        
        
############################### Analyze T2 Image ###########################################
## Set known information about this test image
    set ImageTypeName "Analyze"
        set ImageType "T2"
        set ImageMin 0.000000
        set ImageMax  255.000000
        set Dimensions "256 256 192 1"
        set Resolutions "1.016000 1.016000 1.016000 1.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }

############################### Trimodal Image ###########################################
## Set known information about this test image
        set ImageTypeName "Analyze Trimodal"
        set ImageType "T1 T2 PD"
        set ImageMin "0.000000 0.000000 0.000000" 
        set ImageMax "255.000000 255.000000 255.000000"
        set Dimensions "256 256 192 1"
        set Resolutions "1.016000 1.016000 1.016000 1.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set b2cmd [format "b2_load_image {%s %s %s}" \
              $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr \
                    $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr \
                    $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
        set TestImageID [eval $b2cmd]
#only continue upon success
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                b2_destroy_image $TestImageID
        }
        
        
        
##################### Test Loading Various Data Types Image ###########################################
##################### Test Loading Various Data Types Image ###########################################
##################### Test Loading Various Data Types Image ###########################################
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
                b2_destroy_image $varian8bitT2
        }

    set SubTestDes "load GE 4x US 16 bit T1 test"
        set ge4xU16bitT1 [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096 data-type= unsigned-16bit]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge4xU16bitT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 4x US 16 bit T1 image min"
                set imageMin [b2_image_min $ge4xU16bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 4x US 16 bit T1 image max"
                set imageMax [b2_image_max $ge4xU16bitT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 200.000000}   ] $ModuleName $SubTestDes
                b2_destroy_image $ge4xU16bitT1
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
                b2_destroy_image $ge4xS16bitT1
        }

    set SubTestDes "load GE 5x Float T1 test"
        set ge5xFloatT1 [b2_load_image $pathToRegressionDir/SGI/MR/5x-B1/TEST/15_002/113766.002.099 data-type= float-single]
        if { [ ReportTestStatus $LogFile  [ expr  {$ge5xFloatT1 != -1}   ] $ModuleName $SubTestDes ] } {

            set SubTestDes "GE 5x Float T1 image min"
                set imageMin [b2_image_min $ge5xFloatT1]
                ReportTestStatus $LogFile  [ expr  {$imageMin == 0.000000}   ] $ModuleName $SubTestDes

                set SubTestDes "GE 5x Float T1 image max"
                set imageMax [b2_image_max $ge5xFloatT1]
                ReportTestStatus $LogFile  [ expr  {$imageMax == 2337.000000}   ] $ModuleName $SubTestDes
                b2_destroy_image $ge5xFloatT1
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
                b2_destroy_image $ge5xU32bitT1
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
                b2_destroy_image $ge5xS32bitT1
        }


############################### Strict Analyze Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     StopModule $LogFile $ModuleName
        return $MODULE_SUCCESS
}

