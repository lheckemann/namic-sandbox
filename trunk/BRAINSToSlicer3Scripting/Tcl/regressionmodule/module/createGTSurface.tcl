# \author       Greg Harris
# \date         3/19/2002
# \brief        b2_create_GTSurface
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
proc createGTSurface { pathToRegressionDir dateString } {
        set ModuleName "createGTSurface"
        set ModuleAuthor "Greg Harris"
        set ModuleDescription "Testing of b2_create_GTSurface"
        global OUTPUT_DIR;
        global MODULE_SUCCESS;
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#       close $LogFile
#       set LogFile stderr

# Run Tests
############ load prerequisite files ###################
        set SubTestDes "Load requisite objects for surface creation - classImage"
        set classImage [b2_load_image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_segment.hdr ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$classImage != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load image ($pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_segment.hdr)."
        }
    set SubTestDes "Load requisite objects for surface creation - brainMask"
        set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$brainMask != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load mask ($pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask)."
        }

    set SubTestDes "Load requisite objects for surface creation - hemRoi"
        set hemRoi [b2_load_roi $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/surf_midline.yroi ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$hemRoi != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/surf_midline.yroi."
        }

    set SubTestDes "Load requisite objects for surface creation - cerebellumRoi"
        set cerebellumRoi [b2_load_roi $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/crbl.xroi ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$cerebellumRoi != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/crbl.xroi."
        }

    set SubTestDes "Load requisite objects for surface creation - brainstemRoi"
        set brainstemRoi [b2_load_roi $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/surf_pons.yroi ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$brainstemRoi != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/harris/surf_pons.yroi."
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

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $hemRoi ] != -1 } ] $ModuleName "Destroying roi $hemRoi"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $brainstemRoi ] != -1 } ] $ModuleName "Destroying roi $brainstemRoi"

        set SubTestDes "b2_erode_surface_mask brainMask classImage 131"
        set errodeBrainMask [b2_erode_surface_mask $brainMask $classImage 131]
        ReportTestStatus $LogFile  [ expr {$errodeBrainMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_or_masks hemMask2 hemMask2"
        set hemMask [b2_or_masks $hemMask1 $hemMask2]
        ReportTestStatus $LogFile  [ expr {$hemMask != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "b2_not_mask hemMask"
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


        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $hemMask ] != -1 } ] $ModuleName "Destroying mask $hemMask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $hemMask1 ] != -1 } ] $ModuleName "Destroying mask $hemMask1"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $hemMask2 ] != -1 } ] $ModuleName "Destroying mask $hemMask2"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $hemMaskInv ] != -1 } ] $ModuleName "Destroying mask $hemMaskInv"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $cerebellumMask ] != -1 } ] $ModuleName "Destroying mask $cerebellumMask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $cerebellumMaskInv ] != -1 } ] $ModuleName "Destroying mask $cerebellumMaskInv"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainstemMask ] != -1 } ] $ModuleName "Destroying mask $brainstemMask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainstemMaskInv ] != -1 } ] $ModuleName "Destroying mask $brainstemMaskInv"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $excludePartMask ] != -1 } ] $ModuleName "Destroying mask $excludePartMask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $excludeRoiMask ] != -1 } ] $ModuleName "Destroying mask $excludeRoiMask"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $errodeBrainMask ] != -1 } ] $ModuleName "Destroying mask $errodeBrainMask"

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
# Do Turk decimation on left hemisphere
        set decimationArgs [list GTS 0.174 0.35 0.5 0.5 0.0 ]
        set parameterArgs [list $classImage $surfaceRegionMask -1 1.0 130 190 ]
        set boundArgs [list 40 -1 0 -1 0 -1 ]

        set SubTestDes "Create Left Surface EdgePercent"
        set surfacel [ b2_create_GTSurface $parameterArgs $boundArgs $decimationArgs depth-layer-intensity-list= [list 190.0]]
        ReportTestStatus $LogFile  [ expr {$surfacel != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Write Left Surface EdgePercent"
        set errorFlag [b2_save_GTSurface ${OUTPUT_DIR}/l_GTS_test.gts GTS $surfacel ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$errorFlag != -1 } ] $ModuleName $SubTestDes ] } {
            puts "Error creating LEFT surface.\nFailed to save ${OUTPUT_DIR}/l_GTS_test.gts"
        }

# Do level-of-aspiration decimation on right hemisphere
          set decimationArgs  [list GTS 0.174 150000.0 0.5 0.5 0.0]
          set boundArgs [list 0 60 0 -1 0 -1 ]
          set SubTestDes "Create Right Surface EdgeCount"
          set surfacer [ b2_create_GTSurface $parameterArgs $boundArgs $decimationArgs depth-layer-intensity-list= [list 190.0]]
          ReportTestStatus $LogFile  [ expr {$surfacer != -1 } ] $ModuleName $SubTestDes

          set SubTestDes "Write Right Surface EdgeCount"
          set errorFlag [b2_save_GTSurface ${OUTPUT_DIR}/r_GTS_test.gts GTS $surfacer ]
          if { ! [ ReportTestStatus $LogFile  [ expr {$errorFlag != -1 } ] $ModuleName $SubTestDes ]} {
              puts "Error creating RIGHT surface.\nFailed to save ${OUTPUT_DIR}/r_GTS_test.gts"
          }
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
array set KnownValueArray [ join  [list \
"FundalSumArea [expr (54688.447212956663861 + 53869.284385875348000) / 2.0]" \
"FundalMeanCurvature [expr (-0.077512592094799 + -0.076017485247091) / 2.0]" \
"FundalStdDevCurvature [expr (0.057246349336915 + 0.054863940785857) / 2.0]" \
"GyralSumArea [expr (80777.800104747904697 + 81596.962931840244000) / 2.0]" \
"GyralMeanCurvature [expr (0.080082807878143 + 0.082666719602634) / 2.0]" \
"GyralStdDevCurvature [expr (0.053848387710639 + 0.051817301129899) / 2.0]" \
"TotalSumArea 135470.298780308512505" \
"TotalMeanCurvature [expr (0.017396351702770 + 0.019564620534755) / 2.0]" \
"TotalStdDevCurvature [expr (0.093450241209391 + 0.095428334798929) / 2.0]" \
"FundalMeanDepth [expr (1.967070477988834 + 1.996898442508269) / 2.0]" \
"FundalStdDevDepth [expr (0.838513734254005 + 0.797611299101869) / 2.0]" \
"GyralMeanDepth [expr (2.644887456094986 + 2.632235730864085) / 2.0]" \
"GyralStdDevDepth [expr (1.311018269482263 + 1.319651010079989) / 2.0]" \
"TotalMeanDepth [expr (2.375237030856931 + 2.375441608068102) / 2.0]" \
"TotalStdDevDepth [expr (1.184962732171057 + 1.184618232186840) / 2.0]"
] ]

array set CriterionArray [ join  [list \
"FundalSumArea [expr abs(54688.447212956663861 - 53869.284385875348000) / 53869.284385875348000]" \
"FundalMeanCurvature [expr abs(-0.077512592094799 - -0.076017485247091) / 0.076017485247091]" \
"FundalStdDevCurvature [expr abs(0.057246349336915 - 0.054863940785857) / 0.054863940785857]" \
"GyralSumArea [expr abs(80777.800104747904697 - 81596.962931840244000) / 80777.800104747904697 ]" \
"GyralMeanCurvature [expr abs(0.080082807878143 - 0.082666719602634) / 0.080082807878143]" \
"GyralStdDevCurvature [expr abs(0.053848387710639 - 0.051817301129899) / 0.051817301129899
]" \
"TotalSumArea [expr abs(0.000001) / 135470.298780308512505]" \
"TotalMeanCurvature [expr abs(0.017396351702770 - 0.019564620534755) / 0.017396351702770]" \
"TotalStdDevCurvature [expr abs(0.095428334798929 - 0.093450241209391) / 0.093450241209391]" \
"FundalMeanDepth [expr abs(1.967070477988834 - 1.996898442508269) / 1.996898442508269]" \
"FundalStdDevDepth [expr abs(0.838513734254005 - 0.797611299101869) / 0.797611299101869]" \
"GyralMeanDepth [expr abs(2.644887456094986 - 2.632235730864085) / 2.632235730864085]" \
"GyralStdDevDepth [expr abs(1.311018269482263 - 1.319651010079989) / 1.311018269482263]" \
"TotalMeanDepth [expr abs(2.375237030856931 - 2.375441608068102) / 2.375237030]" \
"TotalStdDevDepth [expr abs(1.184962732171057 - 1.184618232186840) / 1.184618232186840]"
] ]
#return $MODULE_FAILURE

set output [ b2_measure_GTSurface_mask $brainMask $surfacel $surfacer ]
puts $output
array set ValueArray [ join $output ]
foreach {key} [array names CriterionArray] {
          set SubTestDes "Comparing to surface measurements for $key, does $KnownValueArray($key)  nearly equal  $ValueArray($key)  "
          ReportTestStatus $LogFile  [ expr { abs($KnownValueArray($key) - $ValueArray($key)) /  abs($KnownValueArray($key)) < 1.69 * $CriterionArray($key) } ] $ModuleName $SubTestDes
}

# Clean up Files
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $surfacel ] != -1 } ] $ModuleName "Destroying GTSurface $surfacel"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $surfacer ] != -1 } ] $ModuleName "Destroying GTSurface $surfacer"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $classImage ] != -1 } ] $ModuleName "Destroying image $classImage"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainMask ] != -1 } ] $ModuleName "Destroying mask $brainMask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $cerebellumRoi ] != -1 } ] $ModuleName "Destroying roi $cerebellumRoi"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $surfaceRegionMask ] != -1 } ] $ModuleName "Destroying mask $surfaceRegionMask"


# Free memory

    return [ StopModule  $LogFile $ModuleName ]
      return $MODULE_SUCCESS
}

