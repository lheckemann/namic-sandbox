# \author        Greg Harris"
# \date
# \brief        Testing of 'b2_convert_mask-set_to_code-image' and 'b2_convert_code-image_to_mask-set'
# \fn                proc codeImageAndMasks {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 convert Talairach-Box to Mask tested simply.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc codeImageAndMasks {pathToRegressionDir dateString} {
    set ModuleName "codeImageAndMasks"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of 'b2_convert_mask-set_to_code-image' and 'b2_convert_code-image_to_mask-set'"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests


    set crbl_params [b2 load Talairach-Parameters "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/cerebellum_parameters"]
    set crbl_lndmrks [b2 load Landmark "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/Cerebellum.lnd"]
    set avg_lndmrks [b2 load Landmark "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/Cerebellum_space_average.lnd"]
    set avg_lndmrks_acq [b2_convert_talairach-landmark_to_landmark $avg_lndmrks $crbl_params]
    set unwarp [b2_convert_landmarks_to_bookstein-transform $avg_lndmrks_acq $crbl_lndmrks ]

    set l_ant_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask4"]
    set r_ant_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask0"]
    set l_corpus_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask5"]
    set r_corpus_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask1"]
    set l_infpost_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask6"]
    set r_infpost_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask2"]
    set l_suppost_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask7"]
    set r_suppost_warp [b2 load Mask "${pathToRegressionDir}/SGI/MR/B2-Crbl/TEST/10_ACPC/crbl_reg_warp_its1000.mask3"]

    b2_set_interpolation nearest_neighbor
    set atlasList [list $l_ant_warp $r_ant_warp $l_corpus_warp $r_corpus_warp $l_infpost_warp $r_infpost_warp $l_suppost_warp $r_suppost_warp]

    set codeImage [b2_convert_mask-set_to_code-image ${atlasList}]
    set SubTestDes "correct response test: \[b2_convert_mask-set_to_code-image <atlasList>\] "
    if {[ReportTestStatus $LogFile  [ expr { ${codeImage} >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    b2_set_transform $unwarp image $codeImage
    set nameList {l_ant r_ant l_corpus r_corpus l_infpost r_infpost l_suppost r_suppost}
    set maskList [b2_convert_code-image_to_mask-set $codeImage $nameList]
    set SubTestDes "correct response test: \[b2_convert_code-image_to_mask-set <transformed-code-image>\]"
    if {[ReportTestStatus $LogFile  [ expr { ${maskList} != -1} ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "correct length test: \[b2_convert_code-image_to_mask-set <transformed-code-image>\]"
    if {[ReportTestStatus $LogFile  [ expr { [llength ${maskList}] == [llength ${atlasList}]} ] $ModuleName $SubTestDes] == 0} {
}

    set union_form "b2_or_masks"
    foreach pair $maskList {
     set union_form "$union_form [lindex $pair 1]"
    }
    set union_mask [eval ${union_form}]


    set union_tbl [b2_measure_volume_mask ${union_mask}]
    set total [lindex [lindex ${union_tbl} 0] 1]
    set sum 0
    foreach pair ${maskList} {
    set part_tbl [b2_measure_volume_mask [lindex $pair 1]]
    set sum [expr $sum + [lindex [lindex ${part_tbl} 0] 1]]
    }
puts $LogFile "- - - - - - - - - -:"
puts $LogFile "Measurements:          union: $total     sum: $sum"
puts "Measurements:          union: $total     sum: $sum"
        set SubTestDes "Variation between the whole and the sum of the parts"
        set control_ratio [expr abs($total - $sum) / $total]
puts $LogFile "Difference Fraction:   ${control_ratio}"
puts "Difference Fraction:   ${control_ratio}"
        if {[ReportTestStatus $LogFile  [ expr { ${control_ratio} < 0.00001 } ] $ModuleName $SubTestDes] == 0} {
}


    set num_ret [b2_convert_mask-set_to_code-image]
    set SubTestDes "required argument test: \[b2_convert_mask-set_to_code-image\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2_convert_mask-set_to_code-image ${atlasList} junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2_convert_mask-set_to_code-image ${atlasList} junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set num_ret [b2_convert_code-image_to_mask-set]
    set SubTestDes "required argument test: \[b2_convert_code-image_to_mask-set\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set num_ret [b2_convert_code-image_to_mask-set ${codeImage}]
    set SubTestDes "insufficient argument test: \[b2_convert_code-image_to_mask-set <img>\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2_convert_code-image_to_mask-set $codeImage $nameList junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2_convert_code-image_to_mask-set $codeImage $nameList junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    foreach mask ${atlasList} {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${mask} ] != -1 } ] $ModuleName "Destroying mask ${mask}"
    }
    foreach pair ${maskList} {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask [lindex $pair 1] ] != -1 } ] $ModuleName "Destroying mask [lindex $pair 1]"
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${union_mask} ] != -1 } ] $ModuleName "Destroying mask ${union_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters ${crbl_params} ] != -1 } ] $ModuleName "Destroying talairach-parameters ${crbl_params}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark ${crbl_lndmrks} ] != -1 } ] $ModuleName "Destroying landmark ${crbl_lndmrks}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark ${avg_lndmrks} ] != -1 } ] $ModuleName "Destroying landmark ${avg_lndmrks}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark ${avg_lndmrks_acq} ] != -1 } ] $ModuleName "Destroying landmark ${avg_lndmrks_acq}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform ${unwarp} ] != -1 } ] $ModuleName "Destroying transform ${unwarp}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $codeImage ] != -1 } ] $ModuleName "Destroying image $codeImage"


    return [ StopModule  $LogFile $ModuleName ]
}

