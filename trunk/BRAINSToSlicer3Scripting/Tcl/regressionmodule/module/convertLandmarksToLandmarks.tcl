# \author        Greg Harris"
# \date
# \brief        b2 convert talairach-landmark to landmark
#        b2 convert landmark to talairach-landmark
#        b2 convert location to talairach-mm
#        b2 convert talairach-mm to location
# \fn                proc convertLandmarksToLandmarks {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# 'b2 convert talairach-landmark to landmark' -- b2 invalid arguments
# 'b2 convert landmark to talairach-landmark' -- b2 invalid arguments
# 'b2 convert location to talairach-mm' -- b2 invalid arguments
# 'b2 convert talairach-mm to location' -- b2 invalid arguments
# 'b2 convert talairach-landmark to landmark' -- run without error signal
# 'b2 convert landmark to talairach-landmark' -- run without error signal
# 'b2 convert location to talairach-mm' -- run without error signal
# 'b2 convert talairach-mm to location' -- run without error signal
# b2 get landmark count
# b2 get landmark names
# b2 get landmark location
# idempotencies all around.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc convertLandmarksToLandmarks {pathToRegressionDir dateString} {
    set ModuleName "convertLandmarksToLandmarks"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 convert landmark to landmark"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#    close $LogFile
#    set LogFile stderr




# Run Tests

set ModuleName "convertLandmarksToLandmarks\[WholeBrain\]"

#     set img [b2 load image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_T1.hdr"]
    set tal_par [b2 load talairach-parameters "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd"]
    set tal_land [b2 load landmark "${pathToRegressionDir}/SGI/MR/b2-parcellation/TEST/10_ACPC/Average_left_morph1_grooves.lnd"]


        set num_ret [b2 convert talairach-landmark to landmark]
        set SubTestDes "required argument test: \[b2 convert talairach-landmark to landmark\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2 convert talairach-landmark to landmark $tal_land $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2 convert talairach-landmark to landmark $tal_land $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "correct response test: \[b2 convert talairach-landmark to landmark <tal_land> <tal_par>\]"
    set acq_land [b2 convert talairach-landmark to landmark $tal_land $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$acq_land >= 0 } ] $ModuleName $SubTestDes] == 0} {
}


        set num_ret [b2 convert landmark to talairach-landmark]
        set SubTestDes "required argument test: \[b2 convert landmark to talairach-landmark\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2 convert landmark to talairach-landmark $acq_land $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2 convert landmark to talairach-landmark $acq_land $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "correct response test: \[b2 convert landmark to talairach-landmark <acq_land> <tal_par>\]"
    set idem_tal_land [b2 convert landmark to talairach-landmark $acq_land $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$idem_tal_land >= 0 } ] $ModuleName $SubTestDes] == 0} {
}


        set SubTestDes "correct response test: \[b2 convert talairach-landmark to landmark <idem_tal_land> <tal_par>\]"
    set idem_acq_land [b2 convert talairach-landmark to landmark $idem_tal_land $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$idem_acq_land >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    if {($idem_tal_land >=0) && ($idem_acq_land >= 0)} {

        set ct1 [b2 get landmark count $tal_land]
        set ct2 [b2 get landmark count $acq_land]
        set ct3 [b2 get landmark count $idem_tal_land]
        set ct4 [b2 get landmark count $idem_acq_land]

        set SubTestDes "tal count == acq count test"
        if {[ReportTestStatus $LogFile  [ expr {$ct1 == $ct2 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "tal count == idem tal count test"
        if {[ReportTestStatus $LogFile  [ expr {$ct1 == $ct3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acq count == idem acq count test"
        if {[ReportTestStatus $LogFile  [ expr {$ct2 == $ct4 } ] $ModuleName $SubTestDes] == 0} {
}


        set nl1 [b2 get landmark names $tal_land]
        set nl2 [b2 get landmark names $acq_land]
        set nl3 [b2 get landmark names $idem_tal_land]
        set nl4 [b2 get landmark names $idem_acq_land]

        set SubTestDes "tal names == acq names test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $nl1 $nl2] == 0} ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "tal names == idem tal names test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $nl1 $nl3] == 0} ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acq names == idem acq names test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $nl2 $nl4] == 0} ] $ModuleName $SubTestDes] == 0} {
}

        set pl1 [b2 get landmark location $tal_land -1]
        set pl2 [b2 get landmark location $acq_land -1]
        set pl3 [b2 get landmark location $idem_tal_land -1]
        set pl4 [b2 get landmark location $idem_acq_land -1]


        set i [expr $ct1 / 2]
        set acq_q [lindex $pl2 $i]

        set num_ret [b2 convert location to talairach-mm]
        set SubTestDes "required argument test: \[b2 convert location to talairach-mm\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 convert location to talairach-mm $acq_q]
        set SubTestDes "required argument test: \[b2 convert location to talairach-mm <acq_q>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test location to talairach"
        set num_ret [b2 convert location to talairach-mm $acq_q $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test location to talairach"
        set num_ret [b2 convert location to talairach-mm $acq_q $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


        set num_ret [b2 convert talairach-mm to location]
        set SubTestDes "required argument test: \[b2 convert talairach-mm to location\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 convert talairach-mm to location $acq_q]
        set SubTestDes "required argument test: \[b2 convert talairach-mm to location <acq_q>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test talairach to location"
        set num_ret [b2 convert talairach-mm to location $acq_q $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test talairach to location"
        set num_ret [b2 convert talairach-mm to location $acq_q $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set tal_q [b2 convert location to talairach-mm $acq_q $tal_par]
        set SubTestDes "tal_q point location to talairach correct response"
        if {[ReportTestStatus $LogFile  [ expr {[llength $tal_q] == 3} ] $ModuleName $SubTestDes] == 0} {
}
        set idem_acq_q [b2 convert talairach-mm to location $tal_q $tal_par]
        set SubTestDes "idem_acq_q point talairach to location correct response"
        if {[ReportTestStatus $LogFile  [ expr {[llength $idem_acq_q] == 3} ] $ModuleName $SubTestDes] == 0} {
}
        set idem_tal_q [b2 convert location to talairach-mm $idem_acq_q $tal_par]
        set SubTestDes "idem_tal_q point location to talairach correct response"
        if {[ReportTestStatus $LogFile  [ expr {[llength $idem_tal_q] == 3} ] $ModuleName $SubTestDes] == 0} {
}

        set tal_max 0.0
        set acq_max 0.0
        set tal_mm_max 0.0
        set acq_mm_max 0.0
                set criterion 0.0001
        for {set i 0} {$i < $ct1} {incr i} {

            for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex [lindex $pl1 $i] $j] - [lindex [lindex $pl3 $i] $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl1($i, $j) = [lindex [lindex $pl1 $i] $j] but pl3($i, $j) = [lindex [lindex $pl3 $i] $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl1($i, $j) = [lindex [lindex $pl1 $i] $j] but pl3($i, $j) = [lindex [lindex $pl3 $i] $j]"
            }
            if {$n > $tal_max} {
                set tal_max $n
            }
            if {[expr - $n] > $tal_max} {
                set tal_max [expr - $n]
            }
            set n [expr [lindex [lindex $pl2 $i] $j] - [lindex [lindex $pl4 $i] $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex [lindex $pl2 $i] $j] but pl4($i, $j) = [lindex [lindex $pl4 $i] $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex [lindex $pl2 $i] $j] but pl4($i, $j) = [lindex [lindex $pl4 $i] $j]"
            }
            if {$n > $acq_max} {
                set acq_max $n
            }
            if {[expr - $n] > $acq_max} {
                set acq_max [expr - $n]
            }
            }

            set acq_q [lindex $pl2 $i]
            set tal_q [b2 convert location to talairach-mm $acq_q $tal_par]
            set idem_acq_q [b2 convert talairach-mm to location $tal_q $tal_par]
            set idem_tal_q [b2 convert location to talairach-mm $idem_acq_q $tal_par]

            for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex $acq_q $j] - [lindex $idem_acq_q $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex $acq_q $j] but pl2($i, $j)'' = [lindex $idem_acq_q $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex $acq_q $j] but pl2($i, $j)'' = [lindex $idem_acq_q $j]"
            }
            if {$n > $tal_max} {
                set tal_mm_max $n
            }
            if {[expr - $n] > $tal_max} {
                set tal_mm_max [expr - $n]
            }
            set n [expr [lindex $tal_q $j] - [lindex $idem_tal_q $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl2($i, $j)' = [lindex $acq_q $j] but pl2($i, $j)''' = [lindex $idem_acq_q $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl2($i, $j)' = [lindex $acq_q $j] but pl2($i, $j)''' = [lindex $idem_acq_q $j]"
            }
            if {$n > $acq_max} {
                set acq_mm_max $n
            }
            if {[expr - $n] > $acq_max} {
                set acq_mm_max [expr - $n]
            }
            }

        }

        puts "tal_max_err == ${tal_max}; acq_max_err == ${acq_max}"
        puts "tal_mm_max_err == ${tal_mm_max}; acq_mm_max_err == ${acq_mm_max}"

        set SubTestDes "talairach idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${tal_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acquisition idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${acq_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "talairach mm idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${tal_mm_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acquisition mm idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${acq_mm_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set acq_pc {128.000000 127.000000 82.000000}
        set acq_ac {128.000000 127.000000 108.000000}
        set test_acq_pc [b2 convert talairach-mm to location {0.000000 0.000000 -24.000000} $tal_par]
        set test_acq_ac [b2 convert talairach-mm to location {0.000000 0.000000 0.000000} $tal_par]
        puts "test_acq_pc is $test_acq_pc"
        puts "test_acq_ac is $test_acq_ac"

        for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex $acq_ac $j] - [lindex $test_acq_ac $j]]
            if {$n >= 0.0} {
            set SubTestDes "idem acq AC landmark \[$j\] agreement test (+)"
            if {[ReportTestStatus $LogFile  [ expr {$n < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
            if {[expr - $n] > 0.0} {
            set SubTestDes "idem acq AC landmark \[$j\] agreement test (-)"
            if {[ReportTestStatus $LogFile  [ expr {( - $n) < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
            set n [expr [lindex $acq_pc $j] - [lindex $test_acq_pc $j]]
            if {$n >= 0.0} {
            set SubTestDes "idem acq PC landmark \[$j\] agreement test (+)"
            if {[ReportTestStatus $LogFile  [ expr {$n < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
            if {[expr - $n] > 0.0} {
            set SubTestDes "idem acq PC landmark \[$j\] agreement test (-)"
            if {[ReportTestStatus $LogFile  [ expr {( - $n) < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
        }

        set sla_num [expr $ct1 - 3]
        set irp_num [expr $ct1 - 10]
        set sla_name [lindex $nl1 $sla_num]
        set irp_name [lindex $nl1 $irp_num]
        set SubTestDes "tal SLA landmark name test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $sla_name SLAcorner] == 0} ] $ModuleName $SubTestDes] == 0} {
}
        set SubTestDes "tal IRP landmark name test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $irp_name IRPcorner] == 0} ] $ModuleName $SubTestDes] == 0} {
}
        if {([string compare $sla_name SLAcorner] == 0) && ([string compare $irp_name IRPcorner] == 0)} {
            set acq_sla [lindex $pl2 $sla_num]
            set tal_sla [b2 convert location to talairach-mm $acq_sla $tal_par]
            set idem_acq_sla [b2 convert talairach-mm to location $tal_sla $tal_par]
            set idem_tal_sla [b2 convert location to talairach-mm $idem_acq_sla $tal_par]

            set acq_irp [lindex $pl2 $irp_num]
            set tal_irp [b2 convert location to talairach-mm $acq_irp $tal_par]
            set idem_acq_irp [b2 convert talairach-mm to location $tal_irp $tal_par]
            set idem_tal_irp [b2 convert location to talairach-mm $idem_acq_irp $tal_par]

            puts "acq_sla is $acq_sla"
            puts "acq_irp is $acq_irp"
            puts "tal_sla is $tal_sla"
            puts "tal_irp is $tal_irp"
            puts "idem_acq_sla is $idem_acq_sla"
            puts "idem_acq_irp is $idem_acq_irp"
            puts "idem_tal_sla is $idem_tal_sla"
            puts "idem_tal_irp is $idem_tal_irp"

                    set acqSign "-1.0 1.0 1.0"

            for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex $acq_sla $j] - [lindex $acq_irp $j]]
            set SubTestDes "acq SLA \[$j\] > acq IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}

            set n [expr ([lindex $tal_sla $j] - [lindex $tal_irp $j]) * [lindex $acqSign $j]]
            if {$j == 0} {
                set n [expr -1 * $n]
            }
            set SubTestDes "tal SLA \[$j\] > tal IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}

            set n [expr [lindex $idem_acq_sla $j] - [lindex $idem_acq_irp $j]]
            set SubTestDes "idem acq SLA \[$j\] > idem acq IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}

            set n [expr ([lindex $idem_tal_sla $j] - [lindex $idem_tal_irp $j]) * [lindex $acqSign $j]]
            if {$j == 0} {
                set n [expr -1 * $n]
            }
            set SubTestDes "idem tal SLA \[$j\] > idem tal IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}
            }

        }
# return $MODULE_FAILURE

# Clean up Files

# Free memory

        }
        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $tal_par ] != -1 } ] $ModuleName "Destroying talairach-parameters $tal_par"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $tal_land ] != -1 } ] $ModuleName "Destroying landmark $tal_land"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $acq_land ] != -1 } ] $ModuleName "Destroying landmark $acq_land"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $idem_tal_land ] != -1 } ] $ModuleName "Destroying landmark $idem_tal_land"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $idem_acq_land ] != -1 } ] $ModuleName "Destroying landmark $idem_acq_land"

# Run Cerebellar Tests

set ModuleName "convertLandmarksToLandmarks\[Cerebellum\]"
#     set img [b2 load image "${pathToRegressionDir}/SGI/MR/B2-Crbl/ANON015/10_ACPC/ANON015_stereo.hdr"]
    set tal_par [b2 load talairach-parameters "${pathToRegressionDir}/SGI/MR/B2-Crbl/ANON015/10_ACPC/Cerebellum.bnd"]
    set tal_land [b2 load landmark "${pathToRegressionDir}/SGI/MR/B2-Crbl/ANON015/10_ACPC/ronald/Crbl_warp.lnd"]

#set nl1 [b2 get landmark names $tal_land]
#puts "Name list: $nl1"

        set num_ret [b2 convert talairach-landmark to landmark]
        set SubTestDes "required argument test: \[b2 convert talairach-landmark to landmark\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2 convert talairach-landmark to landmark $tal_land $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2 convert talairach-landmark to landmark $tal_land $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "correct response test: \[b2 convert talairach-landmark to landmark <tal_land> <tal_par>\]"
    set acq_land [b2 convert talairach-landmark to landmark $tal_land $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$acq_land >= 0 } ] $ModuleName $SubTestDes] == 0} {
}


        set num_ret [b2 convert landmark to talairach-landmark]
        set SubTestDes "required argument test: \[b2 convert landmark to talairach-landmark\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test"
        set num_ret [b2 convert landmark to talairach-landmark $acq_land $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test"
        set num_ret [b2 convert landmark to talairach-landmark $acq_land $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "correct response test: \[b2 convert landmark to talairach-landmark <acq_land> <tal_par>\]"
    set idem_tal_land [b2 convert landmark to talairach-landmark $acq_land $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$idem_tal_land >= 0 } ] $ModuleName $SubTestDes] == 0} {
}


        set SubTestDes "correct response test: \[b2 convert talairach-landmark to landmark <idem_tal_land> <tal_par>\]"
    set idem_acq_land [b2 convert talairach-landmark to landmark $idem_tal_land $tal_par]
        if {[ReportTestStatus $LogFile  [ expr {$idem_acq_land >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    if {($idem_tal_land >=0) && ($idem_acq_land >= 0)} {

        set ct1 [b2 get landmark count $tal_land]
        set ct2 [b2 get landmark count $acq_land]
        set ct3 [b2 get landmark count $idem_tal_land]
        set ct4 [b2 get landmark count $idem_acq_land]

        set SubTestDes "tal count == acq count test"
        if {[ReportTestStatus $LogFile  [ expr {$ct1 == $ct2 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "tal count == idem tal count test"
        if {[ReportTestStatus $LogFile  [ expr {$ct1 == $ct3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acq count == idem acq count test"
        if {[ReportTestStatus $LogFile  [ expr {$ct2 == $ct4 } ] $ModuleName $SubTestDes] == 0} {
}


        set nl1 [b2 get landmark names $tal_land]
        set nl2 [b2 get landmark names $acq_land]
        set nl3 [b2 get landmark names $idem_tal_land]
        set nl4 [b2 get landmark names $idem_acq_land]

        set SubTestDes "tal names == acq names test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $nl1 $nl2] == 0} ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "tal names == idem tal names test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $nl1 $nl3] == 0} ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acq names == idem acq names test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $nl2 $nl4] == 0} ] $ModuleName $SubTestDes] == 0} {
}

        set pl1 [b2 get landmark location $tal_land -1]
        set pl2 [b2 get landmark location $acq_land -1]
        set pl3 [b2 get landmark location $idem_tal_land -1]
        set pl4 [b2 get landmark location $idem_acq_land -1]


        set i [expr $ct1 / 2]
        set acq_q [lindex $pl2 $i]

        set num_ret [b2 convert location to talairach-mm]
        set SubTestDes "required argument test: \[b2 convert location to talairach-mm\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 convert location to talairach-mm $acq_q]
        set SubTestDes "required argument test: \[b2 convert location to talairach-mm <acq_q>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test location to talairach"
        set num_ret [b2 convert location to talairach-mm $acq_q $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test location to talairach"
        set num_ret [b2 convert location to talairach-mm $acq_q $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}


        set num_ret [b2 convert talairach-mm to location]
        set SubTestDes "required argument test: \[b2 convert talairach-mm to location\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 convert talairach-mm to location $acq_q]
        set SubTestDes "required argument test: \[b2 convert talairach-mm to location <acq_q>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "optional argument number test talairach to location"
        set num_ret [b2 convert talairach-mm to location $acq_q $tal_par junk= ]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "unknown optional argument test talairach to location"
        set num_ret [b2 convert talairach-mm to location $acq_q $tal_par junk= test]
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set tal_q [b2 convert location to talairach-mm $acq_q $tal_par]
        set SubTestDes "tal_q point location to talairach correct response"
        if {[ReportTestStatus $LogFile  [ expr {[llength $tal_q] == 3} ] $ModuleName $SubTestDes] == 0} {
}
        set idem_acq_q [b2 convert talairach-mm to location $tal_q $tal_par]
        set SubTestDes "idem_acq_q point talairach to location correct response"
        if {[ReportTestStatus $LogFile  [ expr {[llength $idem_acq_q] == 3} ] $ModuleName $SubTestDes] == 0} {
}
        set idem_tal_q [b2 convert location to talairach-mm $idem_acq_q $tal_par]
        set SubTestDes "idem_tal_q point location to talairach correct response"
        if {[ReportTestStatus $LogFile  [ expr {[llength $idem_tal_q] == 3} ] $ModuleName $SubTestDes] == 0} {
}

        set tal_max 0.0
        set acq_max 0.0
        set tal_mm_max 0.0
        set acq_mm_max 0.0
                set criterion 0.0001
        for {set i 0} {$i < $ct1} {incr i} {

            for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex [lindex $pl1 $i] $j] - [lindex [lindex $pl3 $i] $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl1($i, $j) = [lindex [lindex $pl1 $i] $j] but pl3($i, $j) = [lindex [lindex $pl3 $i] $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl1($i, $j) = [lindex [lindex $pl1 $i] $j] but pl3($i, $j) = [lindex [lindex $pl3 $i] $j]"
            }
            if {$n > $tal_max} {
                set tal_max $n
            }
            if {[expr - $n] > $tal_max} {
                set tal_max [expr - $n]
            }
            set n [expr [lindex [lindex $pl2 $i] $j] - [lindex [lindex $pl4 $i] $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex [lindex $pl2 $i] $j] but pl4($i, $j) = [lindex [lindex $pl4 $i] $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex [lindex $pl2 $i] $j] but pl4($i, $j) = [lindex [lindex $pl4 $i] $j]"
            }
            if {$n > $acq_max} {
                set acq_max $n
            }
            if {[expr - $n] > $acq_max} {
                set acq_max [expr - $n]
            }
            }

            set acq_q [lindex $pl2 $i]
            set tal_q [b2 convert location to talairach-mm $acq_q $tal_par]
            set idem_acq_q [b2 convert talairach-mm to location $tal_q $tal_par]
            set idem_tal_q [b2 convert location to talairach-mm $idem_acq_q $tal_par]

            for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex $acq_q $j] - [lindex $idem_acq_q $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex $acq_q $j] but pl2($i, $j)'' = [lindex $idem_acq_q $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl2($i, $j) = [lindex $acq_q $j] but pl2($i, $j)'' = [lindex $idem_acq_q $j]"
            }
            if {$n > $tal_max} {
                set tal_mm_max $n
            }
            if {[expr - $n] > $tal_max} {
                set tal_mm_max [expr - $n]
            }
            set n [expr [lindex $tal_q $j] - [lindex $idem_tal_q $j]]
            if {$n > $criterion} {
                puts "TROUBLE:  pl2($i, $j)' = [lindex $acq_q $j] but pl2($i, $j)''' = [lindex $idem_acq_q $j]"
            }
            if {[expr - $n] > $criterion} {
                puts "TROUBLE:  pl2($i, $j)' = [lindex $acq_q $j] but pl2($i, $j)''' = [lindex $idem_acq_q $j]"
            }
            if {$n > $acq_max} {
                set acq_mm_max $n
            }
            if {[expr - $n] > $acq_max} {
                set acq_mm_max [expr - $n]
            }
            }

        }

        puts "tal_max_err == ${tal_max}; acq_max_err == ${acq_max}"
        puts "tal_mm_max_err == ${tal_mm_max}; acq_mm_max_err == ${acq_mm_max}"

        set SubTestDes "talairach idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${tal_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acquisition idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${acq_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "talairach mm idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${tal_mm_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "acquisition mm idempotency maximum error <= 0.0001"
        if {[ReportTestStatus $LogFile  [ expr {${acq_mm_max} <= 0.0001 } ] $ModuleName $SubTestDes] == 0} {
}

        set acq_pc {129.000000 109.000000 57.000000}
        set acq_ac {129.000000 109.000000 57.000000}
        set test_acq_pc [b2 convert talairach-mm to location {0.000000 0.000000 -24.000000} $tal_par]
        set test_acq_ac [b2 convert talairach-mm to location {0.000000 0.000000 0.000000} $tal_par]
        puts "test_acq_pc is $test_acq_pc"
        puts "test_acq_ac is $test_acq_ac"

        for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex $acq_ac $j] - [lindex $test_acq_ac $j]]
            if {$n >= 0.0} {
            set SubTestDes "idem acq AC landmark \[$j\] agreement test (+)"
            if {[ReportTestStatus $LogFile  [ expr {$n < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
            if {[expr - $n] > 0.0} {
            set SubTestDes "idem acq AC landmark \[$j\] agreement test (-)"
            if {[ReportTestStatus $LogFile  [ expr {( - $n) < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
            set n [expr [lindex $acq_pc $j] - [lindex $test_acq_pc $j]]
            if {$n >= 0.0} {
            set SubTestDes "idem acq PC landmark \[$j\] agreement test (+)"
            if {[ReportTestStatus $LogFile  [ expr {$n < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
            if {[expr - $n] > 0.0} {
            set SubTestDes "idem acq PC landmark \[$j\] agreement test (-)"
            if {[ReportTestStatus $LogFile  [ expr {( - $n) < 1.0} ] $ModuleName $SubTestDes] == 0} {
}
            }
        }
#Name list: PC SLAcorner SRAcorner SLPcorner SRPcorner ILAcorner IRAcorner ILPcorner IRPcorner l_horiz_corp l_horiz_corp_post r_horiz_corp r_horiz_corp_post mid_horiz mid_ant mid_sup mid_prim mid_prim_sup mid_prim_inf l_prim_ext l_sup_prim l_inf_prim r_prim_ext r_sup_prim r_inf_prim l_corp l_horiz_ant l_horiz_post l_sup l_horiz_ext l_ext r_corp r_horiz_ant r_horiz_post r_sup r_horiz_ext r_ext

        set sla_num 1
        set irp_num 8
        set sla_name [lindex $nl1 $sla_num]
        set irp_name [lindex $nl1 $irp_num]
        set SubTestDes "tal SLA landmark name test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $sla_name SLAcorner] == 0} ] $ModuleName $SubTestDes] == 0} {
}
        set SubTestDes "tal IRP landmark name test"
        if {[ReportTestStatus $LogFile  [ expr {[string compare $irp_name IRPcorner] == 0} ] $ModuleName $SubTestDes] == 0} {
}
        if {([string compare $sla_name SLAcorner] == 0) && ([string compare $irp_name IRPcorner] == 0)} {
            set acq_sla [lindex $pl2 $sla_num]
            set tal_sla [b2 convert location to talairach-mm $acq_sla $tal_par]
            set idem_acq_sla [b2 convert talairach-mm to location $tal_sla $tal_par]
            set idem_tal_sla [b2 convert location to talairach-mm $idem_acq_sla $tal_par]

            set acq_irp [lindex $pl2 $irp_num]
            set tal_irp [b2 convert location to talairach-mm $acq_irp $tal_par]
            set idem_acq_irp [b2 convert talairach-mm to location $tal_irp $tal_par]
            set idem_tal_irp [b2 convert location to talairach-mm $idem_acq_irp $tal_par]

            puts "acq_sla is $acq_sla"
            puts "acq_irp is $acq_irp"
            puts "tal_sla is $tal_sla"
            puts "tal_irp is $tal_irp"
            puts "idem_acq_sla is $idem_acq_sla"
            puts "idem_acq_irp is $idem_acq_irp"
            puts "idem_tal_sla is $idem_tal_sla"
            puts "idem_tal_irp is $idem_tal_irp"

                    set acqSign "-1.0 1.0 1.0"

            for {set j 0} {$j < 3} {incr j} {
            set n [expr [lindex $acq_sla $j] - [lindex $acq_irp $j]]
            set SubTestDes "acq SLA \[$j\] > acq IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}

            set n [expr ([lindex $tal_sla $j] - [lindex $tal_irp $j]) * [lindex $acqSign $j]]
            if {$j == 0} {
                set n [expr -1 * $n]
            }
            set SubTestDes "tal SLA \[$j\] > tal IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}

            set n [expr [lindex $idem_acq_sla $j] - [lindex $idem_acq_irp $j]]
            set SubTestDes "idem acq SLA \[$j\] > idem acq IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}

            set n [expr ([lindex $idem_tal_sla $j] - [lindex $idem_tal_irp $j]) * [lindex $acqSign $j]]
            if {$j == 0} {
                set n [expr -1 * $n]
            }
            set SubTestDes "idem tal SLA \[$j\] > idem tal IRP \[$j\] test"
            if {[ReportTestStatus $LogFile  [ expr {$n > 0.0} ] $ModuleName $SubTestDes] == 0} {
}
            }

        }
# return $MODULE_FAILURE

# Clean up Files

# Free memory

        }
        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $tal_par ] != -1 } ] $ModuleName "Destroying talairach-parameters $tal_par"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $tal_land ] != -1 } ] $ModuleName "Destroying landmark $tal_land"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $acq_land ] != -1 } ] $ModuleName "Destroying landmark $acq_land"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $idem_tal_land ] != -1 } ] $ModuleName "Destroying landmark $idem_tal_land"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $idem_acq_land ] != -1 } ] $ModuleName "Destroying landmark $idem_acq_land"

        return [ StopModule  $LogFile $ModuleName ]
}

