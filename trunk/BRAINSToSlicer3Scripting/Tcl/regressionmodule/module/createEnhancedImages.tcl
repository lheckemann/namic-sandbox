# \author    Greg Harris
# \date
# \brief    The command to extract a subset of an image is b2_create_bounded-image.
# \fn        proc createEnhancedImages {pathToRegressionDir dateString {GT Test}}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString        - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl

proc createEnhancedImages {pathToRegressionDir dateString {GT Test}} {

    set ModuleName "createEnhancedImages"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of normEnhanceImage and pseudoEnhanceImage"
    global OUTPUT_DIR
    global MODULE_SUCCESS
    global MODULE_FAILURE
    global MODULE_STATUS
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr



    # Run Tests
if {0 == 0} {
    set genimages 1
    if {[normEnhanceImage "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T1.hdr" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_segment.hdr" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask" "${OUTPUT_DIR}/TEST_20_T1_leveled.nii.gz" "${OUTPUT_DIR}/TEST_20_T1_enhanced.nii.gz" 1.5 20 0] == 0} {
    set t1TstLev "${OUTPUT_DIR}/TEST_20_T1_leveled.nii.gz"
    set t1StdLev "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T1_leveled.hdr"
    set t1TstEnh "${OUTPUT_DIR}/TEST_20_T1_enhanced.nii.gz"
    set t1StdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T1_enhanced.hdr"
    } else {
    set genimages 0
    }
    set SubTestDes "test: \[normEnhanceImage t1\]"
    ReportTestStatus $LogFile  $genimages $ModuleName $SubTestDes

    set genimages 1
    if {[normEnhanceImage "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T2.hdr" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_segment.hdr" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T2_leveled.hdr" "${OUTPUT_DIR}/TEST_20_T2_enhanced.nii.gz" -1.5 20 0] == 0} {
    set t2TstEnh "${OUTPUT_DIR}/TEST_20_T2_enhanced.nii.gz"
    set t2StdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T2_enhanced.hdr"
    } else {
    set genimages 0
    }
    set SubTestDes "test: \[normEnhanceImage t2\]"
    ReportTestStatus $LogFile  $genimages $ModuleName $SubTestDes

    set genimages 1
    if {[normEnhanceImage "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_PD.hdr" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_segment.hdr" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_PD_leveled.hdr" "${OUTPUT_DIR}/TEST_20_PD_enhanced.nii.gz" -1.5 20 0] == 0} {
    set pdTstEnh "${OUTPUT_DIR}/TEST_20_PD_enhanced.nii.gz"
    set pdStdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_PD_enhanced.hdr"
    } else {
    set genimages 0
    }
    set SubTestDes "test: \[normEnhanceImage pd\]"
    ReportTestStatus $LogFile  $genimages $ModuleName $SubTestDes

    set genimages 1
    if {[pseudoEnhanceImage "${OUTPUT_DIR}/TEST_20_T1_enhanced.nii.gz" "${OUTPUT_DIR}/TEST_20_T2_enhanced.nii.gz" "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/pseudoPD.tbl" "${OUTPUT_DIR}/TEST_20_pd_pseudo_enhanced.nii.gz"] == 0} {
    set psTstEnh "${OUTPUT_DIR}/TEST_20_pd_pseudo_enhanced.nii.gz"
    set psStdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_pd_pseudo_enhanced.hdr"
    } else {
    set genimages 0
    }
    set SubTestDes "test: \[normEnhanceImage pd_pseudo\]"
    ReportTestStatus $LogFile  $genimages $ModuleName $SubTestDes


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_every image ] != -1 } ] $ModuleName "Destroying every image"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_every mask ] != -1 } ] $ModuleName "Destroying every mask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_every table ] != -1 } ] $ModuleName "Destroying every table"
}

    if { $MODULE_STATUS == $MODULE_SUCCESS } {
    set t1TstLev "${OUTPUT_DIR}/TEST_20_T1_leveled.nii.gz"
    set t1StdLev "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T1_leveled.hdr"

    set t1TstEnh "${OUTPUT_DIR}/TEST_20_T1_enhanced.nii.gz"
    set t1StdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T1_enhanced.hdr"

    set t2TstEnh "${OUTPUT_DIR}/TEST_20_T2_enhanced.nii.gz"
    set t2StdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T2_enhanced.hdr"

    set pdTstEnh "${OUTPUT_DIR}/TEST_20_PD_enhanced.nii.gz"
    set pdStdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_PD_enhanced.hdr"

    set psTstEnh "${OUTPUT_DIR}/TEST_20_pd_pseudo_enhanced.nii.gz"
    set psStdEnh "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_pd_pseudo_enhanced.hdr"

    set brainMaskFile "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask"
    set brainMask [b2_load_mask $brainMaskFile]

    set t1TstLevImg [b2_load_image $t1TstLev]
    set t1StdLevImg [b2_load_image $t1StdLev]
    set t1DiffLevImg [b2_subtract_images [list $t1TstLevImg $t1StdLevImg]]
    set dtb [b2_measure_image_mask $brainMask $t1DiffLevImg]

        SingleMeasureEpsilonTest "t1DiffLev mean" 1.0 [expr {abs([lindex [lindex $dtb 0] 1]) }] 0.0 $LogFile $ModuleName
        SingleMeasureEpsilonTest "t1DiffLev stddev" 1.0 [lindex [lindex $dtb 3] 1] 0.0 $LogFile $ModuleName

    set t1TstEnhImg [b2_load_image $t1TstEnh]
    set t1StdEnhImg [b2_load_image $t1StdEnh]
    set t1DiffEnhImg [b2_subtract_images [list $t1TstEnhImg $t1StdEnhImg]]
    set dtb [b2_measure_image_mask $brainMask $t1DiffEnhImg]

        SingleMeasureEpsilonTest "t1DiffEnh mean" 1.0 [expr {abs([lindex [lindex $dtb 0] 1]) }] 0.0 $LogFile $ModuleName
        SingleMeasureEpsilonTest "t1DiffEnh stddev" 1.0 [lindex [lindex $dtb 3] 1] 0.0 $LogFile $ModuleName

    set t2TstEnhImg [b2_load_image $t2TstEnh]
    set t2StdEnhImg [b2_load_image $t2StdEnh]
    set t2DiffEnhImg [b2_subtract_images [list $t2TstEnhImg $t2StdEnhImg]]
    set dtb [b2_measure_image_mask $brainMask $t2DiffEnhImg]

        SingleMeasureEpsilonTest "t2DiffEnh mean" 1.0 [expr {abs([lindex [lindex $dtb 0] 1]) }] 0.0 $LogFile $ModuleName
        SingleMeasureEpsilonTest "t2DiffEnh stddev" 1.0 [lindex [lindex $dtb 3] 1] 0.0 $LogFile $ModuleName

    set pdTstEnhImg [b2_load_image $pdTstEnh]
    set pdStdEnhImg [b2_load_image $pdStdEnh]
    set pdDiffEnhImg [b2_subtract_images [list $pdTstEnhImg $pdStdEnhImg]]
    set dtb [b2_measure_image_mask $brainMask $pdDiffEnhImg]

        SingleMeasureEpsilonTest "pdDiffEnh mean" 1.0 [expr {abs([lindex [lindex $dtb 0] 1]) }] 0.0 $LogFile $ModuleName
        SingleMeasureEpsilonTest "pdDiffEnh stddev" 1.0 [lindex [lindex $dtb 3] 1] 0.0 $LogFile $ModuleName

    set psTstEnhImg [b2_load_image $psTstEnh]
    set psStdEnhImg [b2_load_image $psStdEnh]
    set psDiffEnhImg [b2_subtract_images [list $psTstEnhImg $psStdEnhImg]]
    set dtb [b2_measure_image_mask $brainMask $psDiffEnhImg]

        SingleMeasureEpsilonTest "psDiffEnh mean" 1.0 [expr {abs([lindex [lindex $dtb 0] 1]) }] 0.0 $LogFile $ModuleName
        SingleMeasureEpsilonTest "psDiffEnh stddev" 1.0 [lindex [lindex $dtb 3] 1] 0.0 $LogFile $ModuleName

#puts "calling 'b2_create_image-table':"
        set pair [b2_create_image-table $t1StdEnhImg $t2StdEnhImg $psStdEnhImg]
#puts "pair == $pair"
        set SubTestDes "create image-table test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $pair] == 2 } ] $ModuleName $SubTestDes] == 0} {
              }

#puts "calling 'b2_finish_image-table':"
        set psTstTbl [b2_finish_image-table [lindex $pair 0] [lindex $pair 1]]

        set SubTestDes "finish image-table test"
        if {[ReportTestStatus $LogFile  [ expr {$psTstTbl >= 0 } ] $ModuleName $SubTestDes] == 0} {
              }


#b2_save_table "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/TEST_pseudoPD.tbl" brains2 $psTstTbl


#puts "calling 'b2_generate_table_pseudo-image':"
    set psLimEnhImg [b2_generate_table_pseudo-image $t1StdEnhImg $t2StdEnhImg $psTstTbl]
    set psDiffLimEnhImg [b2_subtract_images [list $psLimEnhImg $psStdEnhImg]]
    set dtb [b2_measure_image_mask $brainMask $psDiffLimEnhImg]

#b2_save_image "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC/FP_TEST_20_pd_pseudo_enhanced.hdr" strictAnalyze75 $psLimEnhImg


#    puts "Reconstructed PD (Std) $Tbl"
#    puts "Reconstructed PD (Lim) $Tab"

        SingleMeasureEpsilonTest "psDiffLimEnh mean" 1.0 [expr {abs([lindex [lindex $dtb 0] 1]) }] 0.0 $LogFile $ModuleName
        SingleMeasureEpsilonTest "psDiffLimEnh stddev" 1.0 [lindex [lindex $dtb 3] 1] 0.0 $LogFile $ModuleName

    set num_ret [b2_standardize-neighborhood_image]
    set SubTestDes "required argument test: \[b2_standardize-neighborhood_image\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_standardize-neighborhood_image $t1StdEnhImg $brainMask]
    set SubTestDes "required argument test: \[b2_standardize-neighborhood_image <t1> <m>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_standardize-neighborhood_image $t1StdEnhImg $brainMask 20 junk=]
    set SubTestDes "optional argument test: \[b2_standardize-neighborhood_image <t1> <m> junk=\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_standardize-neighborhood_image $t1StdEnhImg $brainMask 20 junk= hunk]
    set SubTestDes "optional argument test: \[b2_standardize-neighborhood_image <t1> <m> junk= hunk\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_trim-effect_image ]
    set SubTestDes "required argument test: \[b2_trim-effect_image\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_trim-effect_image $t1StdEnhImg sigmoid 1.5 ]
    set SubTestDes "required argument test: \[b2 trim-effect <t1> sigmoid 1.5\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_trim-effect_image $t1StdEnhImg sigmoid 1.5 $brainMask junk=]
    set SubTestDes "required argument test: \[b2 trim-effect <t1> sigmoid 1.5 <m> junk=\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }

    set num_ret [b2_trim-effect_image $t1StdEnhImg sigmoid 1.5 $brainMask junk= hunk]
    set SubTestDes "required argument test: \[b2 trim-effect <t1> sigmoid 1.5 <m> junk= hunk\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
          }


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t1TstLevImg ] != -1 } ] $ModuleName "Destroying image $t1TstLevImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t1StdLevImg ] != -1 } ] $ModuleName "Destroying image $t1StdLevImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t1DiffLevImg ] != -1 } ] $ModuleName "Destroying image $t1DiffLevImg"

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t1TstEnhImg ] != -1 } ] $ModuleName "Destroying image $t1TstEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t1StdEnhImg ] != -1 } ] $ModuleName "Destroying image $t1StdEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t1DiffEnhImg ] != -1 } ] $ModuleName "Destroying image $t1DiffEnhImg"

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t2TstEnhImg ] != -1 } ] $ModuleName "Destroying image $t2TstEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t2StdEnhImg ] != -1 } ] $ModuleName "Destroying image $t2StdEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $t2DiffEnhImg ] != -1 } ] $ModuleName "Destroying image $t2DiffEnhImg"

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $pdTstEnhImg ] != -1 } ] $ModuleName "Destroying image $pdTstEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $pdStdEnhImg ] != -1 } ] $ModuleName "Destroying image $pdStdEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $pdDiffEnhImg ] != -1 } ] $ModuleName "Destroying image $pdDiffEnhImg"

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $psTstEnhImg ] != -1 } ] $ModuleName "Destroying image $psTstEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $psStdEnhImg ] != -1 } ] $ModuleName "Destroying image $psStdEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $psDiffEnhImg ] != -1 } ] $ModuleName "Destroying image $psDiffEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $psLimEnhImg ] != -1 } ] $ModuleName "Destroying image $psLimEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $psDiffLimEnhImg ] != -1 } ] $ModuleName "Destroying image $psDiffLimEnhImg"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainMask ] != -1 } ] $ModuleName "Destroying mask $brainMask"

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_table $psTstTbl ] != -1 } ] $ModuleName "Destroying table $psTstTbl"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_table [lindex $pair 0] ] != -1 } ] $ModuleName "Destroying table [lindex $pair 0]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_table [lindex $pair 1] ] != -1 } ] $ModuleName "Destroying table [lindex $pair 1]"


return [ StopModule  $LogFile $ModuleName ]


    # Clean up Files
    set t1TstEnh "${OUTPUT_DIR}/TEST_20_T1_enhanced.nii.gz"
    set t2TstEnh "${OUTPUT_DIR}/TEST_20_T2_enhanced.nii.gz"
    set pdTstEnh "${OUTPUT_DIR}/TEST_20_PD_enhanced.nii.gz"
    set psTstEnh "${OUTPUT_DIR}/TEST_20_pd_pseudo_enhanced.nii.gz"
    if {[catch { exec rm $t1TstEnh } squabble] != 0 } {puts "exec failed: $squabble" }
    if {[catch { exec rm $t2TstEnh } squabble] != 0 } {puts "exec failed: $squabble" }
    if {[catch { exec rm $pdTstEnh } squabble] != 0 } {puts "exec failed: $squabble" }
    if {[catch { exec rm $psTstEnh } squabble] != 0 } {puts "exec failed: $squabble" }

    }

    set t1TstLev "${OUTPUT_DIR}/TEST_20_T1_leveled.nii.gz"
    if {[catch { exec rm $t1TstLev } squabble] != 0 } {puts "exec failed: $squabble" }


    return [ StopModule  $LogFile $ModuleName ]
}

