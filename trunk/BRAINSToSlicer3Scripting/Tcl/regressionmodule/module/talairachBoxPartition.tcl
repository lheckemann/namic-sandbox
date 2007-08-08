# \author        Greg Harris"
# \date
# \brief        b2 convert Talairach-Box to Mask
# \fn                proc talairachBoxPartition {pathToRegressionDir dateString}
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


proc talairachBoxPartition {pathToRegressionDir dateString} {
    set ModuleName "talairachBoxPartition"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 convert Talairach-Box to Mask"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests


    set class_image [b2 load image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_segment.hdr"]
    set brain_mask [b2 load mask "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_brain_trim.mask"]
    set tal_par [b2 load talairach-parameters "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd"]
    set box_file_list [glob "${pathToRegressionDir}/tal_boxes/*_box"]
    set box_masks_form "list"
    set tissue_masks_form "list"
    foreach box_name ${box_file_list} {
        set box_num [b2 load talairach-box ${box_name}]
        set box_mask [b2 convert Talairach-Box to Mask ${box_num} ${tal_par}]
        if { ${box_mask} != -1 } {
            set box_masks_form "${box_masks_form} ${box_mask}"
            set tissue_mask [b2 and masks ${box_mask} ${brain_mask}]
            if { ${tissue_mask} != -1 } {
                set tissue_masks_form "${tissue_masks_form} ${tissue_mask}"
            }
        }
        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-box $box_num ] != -1 } ] $ModuleName "Destroying talairach-box $box_num"
    }
    set box_num_brainstem [b2 load talairach-box ${pathToRegressionDir}/tal_boxes/brainstem_box ]
    set box_masks [eval ${box_masks_form}]
    set tissue_masks [eval ${tissue_masks_form}]

    set SubTestDes "loaded and converted the right number of box masks"
    if {[ReportTestStatus $LogFile  [ expr {[llength ${box_masks}] == [llength ${box_file_list}] } ] $ModuleName $SubTestDes] == 0} {


        puts "Box Files:  ${box_file_list}  Masks:  ${box_masks}"
    }

    set SubTestDes "clipped the right number of box-brain tissue masks"
    if {[ReportTestStatus $LogFile  [ expr {[llength ${box_masks}] == [llength ${tissue_masks}] } ] $ModuleName $SubTestDes] == 0} {


        puts "Original:  ${box_masks}  Clipped:  ${tissue_masks}"
    }

    set union_form "b2 or masks ${tissue_masks}"
    set union_tissue_mask [eval ${union_form}]

    set union_tbl [b2 measure class-volume mask ${union_tissue_mask} ${class_image}]
    for {set i 0} { ${i} < [llength ${union_tbl}] } {incr i} {
        set total(${i}) [lindex [lindex ${union_tbl} ${i}] 1]
        set sum(${i}) 0
    }
    foreach tissue_mask ${tissue_masks} {
        set part_tbl [b2 measure class-volume mask ${tissue_mask} ${class_image}]
        for {set i 0} {${i} < [llength ${part_tbl}]} {incr i} {
            set sum(${i}) [expr $sum(${i}) + [lindex [lindex ${part_tbl} ${i}] 1]]
        }
    }
    for {set i 0} { ${i} < [llength ${union_tbl}] } {incr i} {
puts $LogFile "- - - - - - - - - -:"
puts "- - - - - - - - - -:"
puts $LogFile "Measurements:          class ${i},     union: $total(${i})     sum: $sum(${i})"
puts "Measurements:          class ${i},     union: $total(${i})     sum: $sum(${i})"
        set SubTestDes "Variation between the whole and the sum of the parts, class ${i}"
        set control_ratio [expr abs($total(${i}) - $sum(${i})) / $total(${i})]
puts $LogFile "Difference Fraction:   ${control_ratio}"
puts "Difference Fraction:   ${control_ratio}"
        if {[ReportTestStatus $LogFile  [ expr { ${control_ratio} < 0.00001 } ] $ModuleName $SubTestDes] == 0} {
}
    }

    set num_ret [b2 convert Talairach-Box to Mask]
    set SubTestDes "required argument test: \[b2 convert Talairach-Box to Mask\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "insufficient argument number test"
    set num_ret [b2 convert Talairach-Box to Mask ${box_num_brainstem}  ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "optional argument number test"
    set num_ret [b2 convert Talairach-Box to Mask ${box_num_brainstem} ${tal_par} junk= ]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "unknown optional argument test"
    set num_ret [b2 convert Talairach-Box to Mask ${box_num_brainstem} ${tal_par} junk= test]
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


    set num_ret [b2 convert Talairach-Box to Mask ${box_num_brainstem} ${tal_par} ]
    set SubTestDes "correct response test: \[b2 convert Talairach-Box to Mask <last-box> <talairach-paremeters>\]"
    if {[ReportTestStatus $LogFile  [ expr { ${num_ret} >= 0} ] $ModuleName $SubTestDes] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${num_ret} ] != -1 } ] $ModuleName "Destroying mask ${num_ret}"
    foreach box_mask $box_masks {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${box_mask} ] != -1 } ] $ModuleName "Destroying mask ${box_mask}"
    }
    foreach tissue_mask $tissue_masks {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${tissue_mask} ] != -1 } ] $ModuleName "Destroying mask ${tissue_mask}"
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${union_tissue_mask} ] != -1 } ] $ModuleName "Destroying mask ${union_tissue_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters ${tal_par} ] != -1 } ] $ModuleName "Destroying talairach-parameters ${tal_par}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${brain_mask} ] != -1 } ] $ModuleName "Destroying mask ${brain_mask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${class_image} ] != -1 } ] $ModuleName "Destroying image ${class_image}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-box ${box_num_brainstem} ] != -1 } ] $ModuleName "Destroying talairach-box ${box_num_brainstem}"

    return [ StopModule  $LogFile $ModuleName ]
}

