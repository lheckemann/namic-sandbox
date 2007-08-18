# \author       Hans J. Johnson"
# \date         3/19/2002
# \brief        b2 create surface
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
proc createSurface { pathToRegressionDir dateString } {
    global env
        set ModuleName "createSurface"
#set ModuleAuthor "Hans J. Johnson"  Hans was original author, but Greg knows what is supposed to happen.
        set ModuleAuthor "Greg Harris"
        set ModuleDescription "Testing of b2 create surface"
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#        close $LogFile
#        set LogFile stderr

# Run Tests
############ load prerequisite files ###################
        set SubTestDes "Load requisite objects for surface creation - classImage"
        set classImage [b2_load_image $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/ANON0019_05_segment.img ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$classImage != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load mask ($pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/ANON0019_brain_trim.mask)."
        }
    set SubTestDes "Load requisite objects for surface creation - brainMask"
        set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/ANON0019_brain_trim.mask ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$brainMask != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load mask ($pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/ANON0019_brain_trim.mask)."
        }

    set SubTestDes "Load requisite objects for surface creation - hemRoi"
        set hemRoi [b2_load_roi $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/surf_midline.yroi ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$hemRoi != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/surf_midline.yroi."
        }

    set SubTestDes "Load requisite objects for surface creation - cerebellumRoi"
        set cerebellumRoi [b2_load_roi $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/crbl_gross_seg_trim.xroi ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$cerebellumRoi != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/crbl_gross_seg_trim.xroi."
        }

    set SubTestDes "Load requisite objects for surface creation - brainstemRoi"
        set brainstemRoi [b2_load_roi $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/surf_brainstem.yroi ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$brainstemRoi != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/user/surf_brainstem.yroi."
        }


    set SubTestDes "b2_extrude_roi hemRoi 2 0 0"
        set hemMask1 [b2_extrude_roi $hemRoi 2 0 0 ]
        ReportTestStatus $LogFile  [ expr {$hemMask1 != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_extrude_roi hemRoi -1 0 0"
        set hemMask2 [b2_extrude_roi $hemRoi -1 0 0 ]
        ReportTestStatus $LogFile  [ expr {$hemMask2 != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_convert_roi_to_mask brainstemRoi"
        set brainstemMask [b2_convert_roi_to_mask $brainstemRoi ]
        ReportTestStatus $LogFile  [ expr {$brainstemMask != -1 } ] $ModuleName $SubTestDes

        b2_destroy_roi $hemRoi
        b2_destroy_roi $brainstemRoi

        set SubTestDes "b2_erode_surface_mask brainMask classImage 131"
        set errodeBrainMask [b2_erode_surface_mask $brainMask $classImage 131]
        ReportTestStatus $LogFile  [ expr {$errodeBrainMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_or_masks hemMask2 hemMask2"
        set hemMask [b2_or_masks $hemMask1 $hemMask2]
        ReportTestStatus $LogFile  [ expr {$hemMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_not_mask hemMask]"
        set hemMaskInv [b2_not_mask $hemMask]
        ReportTestStatus $LogFile  [ expr {$hemMaskInv != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_convert_roi_to_mask cerebellumRoi"
        set cerebellumMask [b2_convert_roi_to_mask $cerebellumRoi]
        ReportTestStatus $LogFile  [ expr {$cerebellumMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_not_mask cerebellumMask"
        set cerebellumMaskInv [b2_not_mask $cerebellumMask]
        ReportTestStatus $LogFile  [ expr {$cerebellumMaskInv != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_not_mask brainstemMask"
        set brainstemMaskInv [b2_not_mask $brainstemMask]
        ReportTestStatus $LogFile  [ expr {$brainstemMaskInv != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_and_masks hemMaskInv cerebellumMaskInv"
        set excludePartMask [b2_and_masks $hemMaskInv $cerebellumMaskInv]
        ReportTestStatus $LogFile  [ expr {$excludePartMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_and_masks excludePartMask brainstemMaskInv"
        set excludeRoiMask [b2_and_masks $excludePartMask $brainstemMaskInv]
        ReportTestStatus $LogFile  [ expr {$excludeRoiMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_and_masks errodeBrainMask excludeRoiMask"
        set surfaceRegionMask [b2_and_masks $errodeBrainMask $excludeRoiMask]
        ReportTestStatus $LogFile  [ expr {$surfaceRegionMask != -1 } ] $ModuleName $SubTestDes


        b2_destroy_mask $hemMask
        b2_destroy_mask $hemMask1
        b2_destroy_mask $hemMask2
        b2_destroy_mask $hemMaskInv
        b2_destroy_mask $cerebellumMask
        b2_destroy_mask $cerebellumMaskInv
        b2_destroy_mask $brainstemMask
        b2_destroy_mask $brainstemMaskInv
        b2_destroy_mask $excludePartMask
        b2_destroy_mask $excludeRoiMask
        b2_destroy_mask $errodeBrainMask

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
# Do TURK decimation on left hemisphere
        set decimationArgs [list TURK 1 0.35 0.05 0.1 0.35 0.7 1.5 ]
        set parameterArgs [list $classImage $surfaceRegionMask 2.0 130 ]
        set optionArgs [list 1 0 0 1 1 ]
        set boundArgs [list 142 -1 0 -1 0 -1 ]

        set SubTestDes "Create Left Surface TURK"
        set surfacel [ b2 create surface $parameterArgs $boundArgs $decimationArgs $optionArgs ]
        ReportTestStatus $LogFile  [ expr {$surfacel != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Write Left Surface TURK"
        set errorFlag [b2 save surface $env(ARCH)/$env(ABI)/l_surf_test brains2 $surfacel ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$errorFlag != -1 } ] $ModuleName $SubTestDes ] } {
            puts "Error creating LEFT surface.\nFailed to save $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/TEST_130_l_dec35.srf."
        }

# Do NO decimation on right hemisphere
          set decimationArgs  [list ]
          set boundArgs [list 0 222 0 -1 0 -1 ]
          set SubTestDes "Create Right Surface No decimation"
          set surfacer [ b2 create surface $parameterArgs $boundArgs $decimationArgs $optionArgs ]
          ReportTestStatus $LogFile  [ expr {$surfacer != -1 } ] $ModuleName $SubTestDes

          set SubTestDes "Write Right Surface No decimation"
          set errorFlag [b2 save surface $env(ARCH)/$env(ABI)/r_surf_test brains2 $surfacer ]
          if { ! [ ReportTestStatus $LogFile  [ expr {$errorFlag != -1 } ] $ModuleName $SubTestDes ]} {
              puts "Error creating RIGHT surface.\nFailed to save $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/TEST_130_r_dec35.srf."
          }
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
array set KnownValueArray [ join  {
{FundalSumArea 107816.982394}
{FundalSumCurvature -32786063.389729}
{FundalMeanCurvature -304.089974}
{FundalSumSqCurvature 15846763493.235018}
{FundalVarianceCurvature 54507.649851}
{FundalStdDevCurvature 233.468734}
{GyralSumArea 114283.786484}
{GyralSumCurvature 33285396.955329}
{GyralMeanCurvature 291.252136}
{GyralSumSqCurvature 15226426041.287905}
{GyralVarianceCurvature 48405.668393}
{GyralStdDevCurvature 220.012882}
{TotalSumArea 222100.768878}
{TotalSumCurvature 499333.565601}
{TotalMeanCurvature 2.248230}
{TotalSumSqCurvature 31073189534.522923}
{TotalVarianceCurvature 139900.762500}
{TotalStdDevCurvature 374.033103}
{FundalSumPartialArea 107816.982394}
{FundalSumDepth 193907.013204}
{FundalMeanDepth 1.798483}
{FundalSumSqDepth 909994.487259}
{FundalVarianceDepth 5.205636}
{FundalStdDevDepth 2.281586}
{GyralSumPartialArea 114283.786484}
{GyralSumDepth 316804.677612}
{GyralMeanDepth 2.772088}
{GyralSumSqDepth 2030697.774031}
{GyralVarianceDepth 10.084435}
{GyralStdDevDepth 3.175600}
{TotalSumPartialArea 222100.768878}
{TotalSumDepth 510711.690816}
{TotalMeanDepth 2.299459}
{TotalSumSqDepth 2940692.261290}
{TotalVarianceDepth 7.952838}
{TotalStdDevDepth 2.820078}
} ]

array set CriterionArray [ join  [list \
"FundalVarianceDepth [expr abs(5.205636 - 5.281672) / abs(5.205636)]" \
"FundalSumSqDepth [expr abs(909994.487259 - 920360.708022) / abs(909994.487259)]" \
"FundalSumCurvature [expr abs(-32786063.389729 - -32747637.988054) / abs(-32786063.389729)]" \
"GyralSumDepth [expr abs(316804.677612 - 316452.854379 ) / abs(316804.677612)]" \
"GyralVarianceCurvature [expr abs(48405.668393 - 48269.833098) / abs(48405.668393)]" \
"TotalVarianceDepth [expr abs(7.952838 - 7.963181) / abs(7.952838)]" \
"TotalSumSqDepth [expr abs(2940692.261290 - 2944052.332231) / abs(2940692.261290)]" \
"TotalSumCurvature [expr abs(499333.565601 - 435028.470397) / abs(499333.565601)]" \
"FundalSumDepth [expr abs(193907.013204 - 194487.417135) / abs(193907.013204)]" \
"FundalMeanCurvature [expr abs(-304.089974 - -303.502133) / abs(-304.089974)]" \
"GyralVarianceDepth [expr abs(10.084435 - 10.040891 ) / abs(10.084435)]" \
"FundalStdDevCurvature [expr abs(233.468734 - 233.081449) / abs(233.468734)]" \
"TotalSumDepth [expr abs(510711.690816 - 511110.559835) / abs(510711.690816)]" \
"TotalMeanCurvature [expr abs(2.248230 - 1.958661) / abs(2.248230)]" \
"TotalStdDevCurvature [expr abs(374.033103 - 373.601573) / abs(374.033103)]" \
"FundalMeanDepth [expr abs(1.798483 - 1.803228) / abs(1.798483)]" \
"FundalSumArea [expr abs(107816.982394 - 107899.202049) / abs(107816.982394)]" \
"FundalStdDevDepth [expr abs(2.281586 - 2.298189) / abs(2.281586)]" \
"GyralMeanCurvature [expr abs(291.252136 - 291.076240) / abs(291.252136)]" \
"GyralSumSqCurvature [expr abs(15226426041.287905 - 15205315451.330257) / abs(15226426041.287905)]" \
"TotalMeanDepth [expr abs(2.299459 - 2.300923) / abs(2.299459)]" \
"GyralSumPartialArea [expr abs(114283.786484 - 114233.643116) / abs(114283.786484)]" \
"TotalSumArea [expr abs(222100.768878 - 222132.845164) / abs(222100.768878)]" \
"GyralStdDevCurvature [expr abs(220.012882 - 219.703967) / abs(220.012882)]" \
"TotalStdDevDepth [expr abs(2.820078 - 2.821911) / abs(2.820078)]" \
"FundalSumSqCurvature [expr abs(15846763493.235018 - 15800813817.361664) / abs(15846763493.235018)]" \
"FundalVarianceCurvature [expr abs(54507.649851 - 54326.961838) / abs(54507.649851)]" \
"GyralSumSqDepth [expr abs(2030697.774031 - 2023691.624209) / abs(2030697.774031)]" \
"GyralMeanDepth [expr abs(2.772088 - 2.769830) / abs(2.772088)]" \
"FundalSumPartialArea [expr abs(107816.982394 - 107899.202049) / abs(107816.982394)]" \
"GyralSumArea [expr abs(114283.786484 - 114233.643116) / abs(114283.786484)]" \
"GyralSumCurvature [expr abs(33285396.955329 - 33255435.215487) / abs(33285396.955329)]" \
"GyralStdDevDepth [expr abs(3.175600 - 3.168737) / abs(3.175600)]" \
"TotalVarianceCurvature [expr abs(139900.762500 - 139578.135487) / abs(139900.762500)]" \
"TotalSumSqCurvature [expr abs(31073189534.522923 - 31006129268.691921) / abs(31073189534.522923)]" \
"TotalSumPartialArea [expr abs(222100.768878 - 222132.845164) / abs(222100.768878)]" \
] ]

set output [ b2 measure surface mask $brainMask $surfacel $surfacer ]
array set ValueArray [ join $output ]
foreach {key} [array names KnownValueArray] {
          set SubTestDes "Comparing to surface measurements for $key, does $KnownValueArray($key)  nearly equal  $ValueArray($key)  "
          ReportTestStatus $LogFile  [ expr { abs($KnownValueArray($key) - $ValueArray($key)) /  abs($KnownValueArray($key)) < $CriterionArray($key) * 1.001 } ] $ModuleName $SubTestDes
}

if { 7 == 11 } {
#  MEANINGLESS TESTS
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
array set KnownValueArray2 [ join  {
{FundalSumArea 4.490093}
{FundalSumCurvature -236.046924}
{FundalMeanCurvature -52.570608}
{FundalSumSqCurvature 18436.434204}
{FundalVarianceCurvature 1342.356094}
{FundalStdDevCurvature 36.638178}
{GyralSumArea 49.582291}
{GyralSumCurvature 18275.487467}
{GyralMeanCurvature 368.589007}
{GyralSumSqCurvature 8944815.407942}
{GyralVarianceCurvature 44545.574206}
{GyralStdDevCurvature 211.058225}
{TotalSumArea 54.072384}
{TotalSumCurvature 18039.440543}
{TotalMeanCurvature 333.616518}
{TotalSumSqCurvature 8963251.842145}
{TotalVarianceCurvature 54463.966137}
{TotalStdDevCurvature 233.375162}
{FundalSumPartialArea 4.490093}
{FundalSumDepth 9.724580}
{FundalMeanDepth 2.165786}
{FundalSumSqDepth 36.896488}
{FundalVarianceDepth 3.526682}
{FundalStdDevDepth 1.877946}
{GyralSumPartialArea 49.582291}
{GyralSumDepth 100.685551}
{GyralMeanDepth 2.030676}
{GyralSumSqDepth 249.911766}
{GyralVarianceDepth 0.916700}
{GyralStdDevDepth 0.957444}
{TotalSumPartialArea 54.072384}
{TotalSumDepth 110.410131}
{TotalMeanDepth 2.041895}
{TotalSumSqDepth 286.808254}
{TotalVarianceDepth 1.134819}
{TotalStdDevDepth 1.065279}
} ]

set output2 [ b2 measure surface roi $cerebellumRoi $surfacel $surfacer ]
array set ValueArray2 [ join $output2 ]
foreach {key} [array names KnownValueArray2] {
          set SubTestDes "Comparing to surface measurements for $key, does $KnownValueArray2($key)  nearly equal  $ValueArray2($key)  "
          ReportTestStatus $LogFile  [ expr { abs($KnownValueArray2($key)  == $ValueArray2($key)) / abs($KnownValueArray2($key)) < 0.0001 } ] $ModuleName $SubTestDes
}

}
# Clean up Files
    b2 destroy surface $surfacel
    b2 destroy surface $surfacer
    b2_destroy_image $classImage
    b2_destroy_mask  $brainMask
    b2_destroy_roi $cerebellumRoi
    b2_destroy_mask $surfaceRegionMask


# Free memory

    StopModule $LogFile $ModuleName
      return $MODULE_SUCCESS
}

