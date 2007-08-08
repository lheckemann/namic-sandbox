# \author    Hans J. Johnson"
# \date        $Date: 2007-05-25 12:31:38 -0500 (Fri, 25 May 2007) $
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
    set ModuleDescription "Test the b2 load image command and loading various image file formats";
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
    set ImageTypeName "GE-DICOM-InterleaveFitImage gdcm";
    set ImageType "T2";
    set ImageMin "0.000000";
    set ImageMax "1151.000000000000000";
    set Dimensions "256 256 107";
#Reading from SpacingBetweenSlices now set Resolutions "0.625000000000000 0.625000000000000 1.799999952316284";
    set Resolutions "0.625000000000000 0.625000000000000 1.800003051757812";
    set PixelValueStats {
            {Mean 114.382122145036604} {Mean-Absolute-Deviation 179.541084048784143} {Variance 46300.339318235506653} {Standard-Deviation 215.175136384842347} {Skewness 1.725237372612797} {Kurtosis 1.145818781621366} {Minimum -128.000000000000000} {Maximum 1023.000000000000000} {Min-Loc-X 252} {Min-Loc-Y 252} {Min-Loc-Z 104} {Max-Loc-X 145} {Max-Loc-Y 216} {Max-Loc-Z 68} {Center-Of-Mass-X 126.922773446698045} {Center-Of-Mass-Y 110.043127158855455} {Center-Of-Mass-Z 54.698290884231611}
    };
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2 load image ${pathToRegressionDir}/SGI/MR/B2-InterleaveFit/TEST/T2_003/006839.003.002 filter= gdcm ];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }

###### Next 3 test use the same values
    set PixelValueStats {
            {Mean 39.978497782061176} {Mean-Absolute-Deviation 34.753186086645101} {Variance 1507.723318959205017} {Standard-Deviation 38.829413064830185} {Skewness 1.387568935429534} {Kurtosis -0.576683021905644} {Minimum 0.000000000000000} {Maximum 169.000000000000000} {Min-Loc-X 252} {Min-Loc-Y 253} {Min-Loc-Z 120} {Max-Loc-X 215} {Max-Loc-Y 192} {Max-Loc-Z 64} {Center-Of-Mass-X 127.444881356865835} {Center-Of-Mass-Y 112.590671675271437} {Center-Of-Mass-Z 62.043331187961307}
    };
    set ImageType "T1";
    set ImageMin "0.000000";
    set ImageMax "169.000000";
    set Dimensions "256 256 124";
    set Resolutions "0.664062023162842 0.664062023162842 1.500000";

    set ImageTypeName "GE-Dicom-gdcmIPLfilter";
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2 load image $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/T1_002/006872.002.078 filter= gdcm];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
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
            {Mean 46.342227518848752} {Mean-Absolute-Deviation 150.853612558061371} {Variance 33720.517987472194363} {Standard-Deviation 183.631473303113808} {Skewness 1.815634050184448} {Kurtosis 1.538281792636980} {Minimum -128.000000000000000} {Maximum 779.000000000000000} {Min-Loc-X 20} {Min-Loc-Y 254} {Min-Loc-Z 254} {Max-Loc-X 15} {Max-Loc-Y 192} {Max-Loc-Z 192} {Center-Of-Mass-X 11.236752937851257} {Center-Of-Mass-Y 70.628063390329999} {Center-Of-Mass-Z 111.278936497383768}
    };

    set ImageTypeName "GE-DICOM-DIGITAL-JACKET-gdcmIPLfilter";
    set SubTestDes "load $ImageTypeName $ImageType test";
    set TestImageID [b2 load image $pathToRegressionDir/SGI/MR/DICOM-DIGITAL-JACKET/TEST/T1_002/MR00010002.dcm filter= gdcm];
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
        ExpandedImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $PixelValueStats $LogFile $ModuleName $SubTestDes;
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID";
    }

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    return [ StopModule  $LogFile $ModuleName ];
}

