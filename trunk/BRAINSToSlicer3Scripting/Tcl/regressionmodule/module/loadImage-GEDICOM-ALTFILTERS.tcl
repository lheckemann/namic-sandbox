# \author    Hans J. Johnson"
# \date        $Date: 2005-04-26 13:23:02 -0500 (Tue, 26 Apr 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImage-GEDICOM {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc loadImage-GEDICOM {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
    set ModuleName "loadImage-GEDICOM";
    set ModuleAuthor "Hans J. Johnson";
    set ModuleDescription "Test the b2_load_image command and loading various image file formats";
    global MODULE_SUCCESS;
    global MODULE_FAILURE;
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString];
#    close $LogFile
#    set LogFile stderr

########################################
########################################
# Run Tests
########################################
########################################

############################### GE DICOM Images ###########################################
## Set known information about this test image
    set ImageTypeName "GE-DICOM-IternleaveFitImage xmedcon";
    set ImageType "T2";
    set ImageMin "0.000000";
    set ImageMax "1151.000000000000000";
    set Dimensions "256 256 107";
    set Resolutions "0.625000000000000 0.625000000000000 1.800003051757812";
    set PixelValueStats {
            {Mean 242.382122774631426} {Mean-Absolute-Deviation 179.541083326200862} {Variance 46300.339129506850441} {Standard-Deviation 215.175135946295740} {Skewness 1.725237378945965} {Kurtosis 1.145818817081402} {Minimum 0.000000000000000} {Maximum 1151.000000000000000} {Min-Loc-X 252} {Min-Loc-Y 252} {Min-Loc-Z 104} {Max-Loc-X 145} {Max-Loc-Y 216} {Max-Loc-Z 68}
    };
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2_load_image ${pathToRegressionDir}/SGI/MR/B2-InterleaveFit/TEST/T2_003/006839.003.002 filter= xmedcon ];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }

## Set known information about this test image
    set ImageType "T2";
    set ImageMin "0.000000";
    set ImageMax "1151.000000000000000";
    set Dimensions "256 256 107";
    set Resolutions "0.625000000000000 0.625000000000000 1.799999952316284";
    set PixelValueStats {
            {Mean 242.382122774631426} {Mean-Absolute-Deviation 179.541083326200862} {Variance 46300.339129506850441} {Standard-Deviation 215.175135946295740} {Skewness 1.725237378945965} {Kurtosis 1.145818817081402} {Minimum 0.000000000000000} {Maximum 1151.000000000000000} {Min-Loc-X 252} {Min-Loc-Y 252} {Min-Loc-Z 104} {Max-Loc-X 145} {Max-Loc-Y 216} {Max-Loc-Z 68}
    };

###### Next 3 test use the same values
    set PixelValueStats {
            {Mean 39.978497782061176} {Mean-Absolute-Deviation 34.753186086645101} {Variance 1507.723318959205017} {Standard-Deviation 38.829413064830185} {Skewness 1.387568935429534} {Kurtosis -0.576683021905644} {Minimum 0.000000000000000} {Maximum 169.000000000000000} {Min-Loc-X 252} {Min-Loc-Y 253} {Min-Loc-Z 120} {Max-Loc-X 215} {Max-Loc-Y 192} {Max-Loc-Z 64}
    };
    set ImageType "T1";
    set ImageMin "0.000000";
    set ImageMax "169.000000";
    set Dimensions "256 256 124";
    set Resolutions "0.664062023162842 0.664062023162842 1.500000";

    set ImageTypeName "GE-Dicom-xmedconIPLfilter";
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/T1_002/006872.002.078 filter= xmedcon];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }

    set ImageTypeName "GE-Dicom-dicomIPLfilter";
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/T1_002/006872.002.078 filter= dicom];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }
#
# do same sort of thing with the DICOM Digital Jacket file
### Next three sets of test use the same values
    set ImageType "T1";
    set ImageMin "0.000000";
    set ImageMax "907.000000";
    set Dimensions "22 256 256";
    set Resolutions "5.000000000000000 0.859375000000000 0.859375000000000";
    set PixelValueStats {
            {Mean 174.342228272437012} {Mean-Absolute-Deviation 150.853612357979785} {Variance 33720.517911218674271} {Standard-Deviation 183.631473095487308} {Skewness 1.815634050285513} {Kurtosis 1.538281792359691} {Minimum 0.000000000000000} {Maximum 907.000000000000000} {Min-Loc-X 20} {Min-Loc-Y 254} {Min-Loc-Z 254} {Max-Loc-X 15} {Max-Loc-Y 192} {Max-Loc-Z 192}
    };

    set ImageTypeName "GE-DICOM-DIGITAL-JACKET-xmedconIPLfilter";
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-DIGITAL-JACKET/TEST/T1_002/MR00010002.dcm filter= xmedcon];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }

##dicomIPLfitler can not read DIGITAL JACKET    set ImageTypeName "GE-DICOM-DIGITAL-JACKET-dicomIPLfilter";
##dicomIPLfitler can not read DIGITAL JACKET    set SubTestDes "load $ImageTypeName $ImageType test";
##dicomIPLfitler can not read DIGITAL JACKET    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-DIGITAL-JACKET/TEST/T1_002/MR00010002.dcm filter= dicom];
##dicomIPLfitler can not read DIGITAL JACKET    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
##dicomIPLfitler can not read DIGITAL JACKET        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
##dicomIPLfitler can not read DIGITAL JACKET        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
##dicomIPLfitler can not read DIGITAL JACKET    }

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    return [ StopModule  $LogFile $ModuleName ];
}

