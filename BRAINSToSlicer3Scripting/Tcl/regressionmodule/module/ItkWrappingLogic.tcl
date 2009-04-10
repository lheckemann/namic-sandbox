# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the wrapping of Itk image filter methods for Tcl users.
# \fn        proc ItkWrappingLogic {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error

proc ItkWrappingLogic {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
    set ModuleName "ItkWrappingLogic";
    set ModuleAuthor "Hans J. Johnson";
    set ModuleDescription "Test the ITK wrapping by comparing against equivalent internal calls to RecursiveGaussian on an image.";
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE;
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString];

########################################
########################################
# Run Tests
########################################
########################################

############################### based on IPLtoITKcmd/TEST_CASE.txt ###########################################
## A test of the two ways to access the same Recursive Gaussian ITK Image Filter.

    set TestFileName "${pathToRegressionDir}/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_stereo.hdr";

    set gaussianFilterX [itkRecursiveGaussianImageFilterF3F3_New];
    set gaussianFilterY [itkRecursiveGaussianImageFilterF3F3_New];
    set gaussianFilterZ [itkRecursiveGaussianImageFilterF3F3_New];
    ${gaussianFilterX} SetDirection 0;
    ${gaussianFilterY} SetDirection 1;
    ${gaussianFilterZ} SetDirection 2;
    ${gaussianFilterX} SetSigma 6.0;
    ${gaussianFilterY} SetSigma 6.0;
    ${gaussianFilterZ} SetSigma 6.0;
    ${gaussianFilterX} SetNormalizeAcrossScale  0;
    ${gaussianFilterY} SetNormalizeAcrossScale  0;
    ${gaussianFilterZ} SetNormalizeAcrossScale  0;

    set b2Image [b2 load Image ${TestFileName} data-type= "float-single"];
    set MyFirstIPLImage [ connectIPLtoITKF3 $b2Image ];

#Still needs ${gaussianFilterX} ReleaseOutput to clear memory and shorten pipeline.
    ${gaussianFilterX} SetInput ${MyFirstIPLImage};
    ${gaussianFilterX} Update;
#Still needs ${gaussianFilterX} ReleaseOutput to clear memory and shorten pipeline.
    ${gaussianFilterY} SetInput [ ${gaussianFilterX} GetOutput ];
    ${gaussianFilterY} Update;
#Still needs ${gaussianFilterX} ReleaseOutput to clear memory and shorten pipeline.
    ${gaussianFilterZ} SetInput [ ${gaussianFilterY} GetOutput ];
    ${gaussianFilterZ} Update;
#Still needs ${gaussianFilterX} ReleaseOutput to clear memory and shorten pipeline.
#Still needs ${gaussianFilterX} ReleaseOutput to clear memory and shorten pipeline.


#Common to both outputs
    set ImageMin -0.231578126549721 ;
    set ImageMax 219.019989013671875 ;

    set itkRGaussian [ connectITKtoIPLF3 [ ${gaussianFilterZ} GetOutput ] ];
    set ImageTypeName "ITKWrapped-RecursiveGaussian-6.0" ;
    set ImageType "ITK-Image" ;
    set Dimensions "256 256 192" ;
    set Resolutions "1.015625 1.015625 1.015625" ;
    set SubTestDes "take data (result: $itkRGaussian) from the ITK pipeline test" ;
    if { [ ReportTestStatus $LogFile  [ expr {[string equal [string index $itkRGaussian 0] "i"] } ] $ModuleName $SubTestDes ]} {
        CoreImageTest $ImageTypeName $itkRGaussian $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes;
    }


    set b2RGaussian [b2_itkRecursiveGaussian $b2Image 6.0 data-type= float-single];
    set ImageTypeName "b2-itkRecursiveGaussian-6.0";
    set ImageType "Classified";
    set Dimensions "256 256 192" ;
    set Resolutions "1.015625 1.015625 1.015625" ;
    set SubTestDes "take data (result: $b2RGaussian) from the brains2 kernel test";
    if { [ ReportTestStatus $LogFile  [ expr {[string equal [string index $b2RGaussian 0] "i"] } ] $ModuleName $SubTestDes ]} {
        CoreImageTest $ImageTypeName $b2RGaussian $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes;
    }

    set SubTestDes "Both RecursiveGaussian images should be available for diff image test";
    if { [ ReportTestStatus $LogFile  [ expr { ([string equal [string index $itkRGaussian 0] "i"]) && ([string equal [string index $b2RGaussian 0] "i"]) } ] $ModuleName $SubTestDes ]} {
        set subImage [b2_subtract_images [list $b2RGaussian $itkRGaussian ] ];

        set leastError [b2_image_min $subImage];
        set greatestError [b2_image_max $subImage];
        set playEnvelopeError [expr $greatestError - $leastError];
        set guideline 0.0001;
        set SubTestDes "Recursive Gaussian Both Ways: Comparing the maximum range of errors, does new ($playEnvelopeError) fall within guideline ($guideline)";
        ReportTestStatus $LogFile  [ expr { $playEnvelopeError <=  $guideline } ] $ModuleName $SubTestDes;

        set rmsError [RootSumSquared $subImage];
        set guideline 0.00001;
        set SubTestDes "Recursive Gaussian Both Ways: Comparing the integral of squared errors over the difference image, does new ($rmsError) fall within guideline ($guideline)";
        ReportTestStatus $LogFile  [ expr { $rmsError <=  $guideline } ] $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $subImage ] != -1 } ] $ModuleName "Destroying image $subImage";
    }
    foreach TestImageID [list $b2Image $b2RGaussian $itkRGaussian ] {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }

##################### Test Loading Various Data Types Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    return [ StopModule  $LogFile $ModuleName ];
}

