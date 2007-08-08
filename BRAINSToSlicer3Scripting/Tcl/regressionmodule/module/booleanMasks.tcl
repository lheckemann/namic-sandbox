# \author        Greg Harris"
# \date
# \brief        b2 and masks
# \brief        b2 or masks
# \brief        b2 xor masks
# \brief        b2 not mask
# \brief        b2 split mask
# \brief        b2 dilate mask
# \brief        b2 erode mask
# \brief        b2 sum masks
# \fn                proc booleanMasks {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
#
# To Do
#------------------------------------------------------------------------
# 'b2 <cmds>' -- b2 invalid arguments
# 'b2 <cmds>' -- run without error signal
# <results>'s parameters
# <results>'s volume
#
# To Test the Test
# -----------------------------------------------------------------------

#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl


proc booleanMasks {pathToRegressionDir dateString} {

    set ModuleName "booleanMasks"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 boolean mask and friends."
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests

# anisotropic dims.

    set xDims 64
    set yDims 96
    set zDims 128

#        set xRes 3.5
#        set yRes 1.5
#        set zRes 0.5

#        set xRes 0.7
#        set yRes 0.5
#        set zRes 0.3

    set xRes 0.5
    set yRes 0.5
    set zRes 0.5

        set img1 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 33 49 65 20 10 10]
        set img2 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 33 49 65 10 30 10]
        set img3 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 33 69 65 10 10 50]

    set imgA [b2 sum images [list $img1 $img2 $img3]]
    set maskA [b2 threshold image $imgA 1.0]
    set maskS [b2 threshold image $imgA 21.0]

        set img4 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 33 69 105 20 10 10]
        set img5 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 43 49 105 10 30 10]
        set img6 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 43 49 65 10 10 50]

    set imgB [b2 sum images [list $img4 $img5 $img6]]
    set maskB [b2 threshold image $imgB 1.0]
    set maskT [b2 threshold image $imgB 21.0]

# b2 and masks
        set num_ret [b2 and masks]
        set SubTestDes "required argument test: \[b2 and masks\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 and masks $maskS]
        set SubTestDes "single argument test: \[b2 and masks <mask-S>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 or masks
        set num_ret [b2 or masks]
        set SubTestDes "required argument test: \[b2 or masks\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 or masks $maskS]
        set SubTestDes "single argument test: \[b2 or masks <mask-S>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 xor masks
        set num_ret [b2 xor masks]
        set SubTestDes "required argument test: \[b2 xor masks\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 xor masks $maskS]
        set SubTestDes "single argument test: \[b2 xor masks <mask-S>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 not mask
        set num_ret [b2 not mask]
        set SubTestDes "required argument test: \[b2 not mask\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 split mask
        set num_ret [b2 split mask]
        set SubTestDes "missing argument test: \[b2 split mask\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 split mask $maskA z 90 foo]
        set SubTestDes "missing argument test: \[b2 split mask <mask-A> z 90 foo\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 split mask $maskA w 90 +]
        set SubTestDes "missing argument test: \[b2 split mask <mask-A> w 90 +\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 split mask $maskA z +]
        set SubTestDes "missing argument test: \[b2 split mask <mask-A> z +\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 dilate mask
        set num_ret [b2 dilate mask]
        set SubTestDes "missing argument test: \[b2 dilate mask\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 dilate mask $maskA]
        set SubTestDes "single argument test: \[b2 dilate mask <mask-A>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 erode mask
        set num_ret [b2 erode mask]
        set SubTestDes "missing argument test: \[b2 erode mask\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 erode mask $maskA]
        set SubTestDes "single argument test: \[b2 erode mask <mask-A>\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

# b2 sum masks
        set num_ret [b2 sum masks]
        set SubTestDes "missing argument test: \[b2 sum masks\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

        set num_ret [b2 sum masks [list]]
        set SubTestDes "empty argument list test: \[b2 sum masks {}\]"
        if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set emptyMask [b2 and masks $maskS $maskT]
        set SubTestDes "empty and test: \[b2 and masks <mask-S> <mask-T>\]"
        if {[ReportTestStatus $LogFile  [ expr {$emptyMask >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $emptyMask]
        set SubTestDes "emptyMask num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "emptyMask dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "emptyMask dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "emptyMask dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $emptyMask]

        set SubTestDes "emptyMask num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "emptyMask res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "emptyMask res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "emptyMask res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $emptyMask]
              if {[SingleMeasureEpsilonTest "empty mask volume" 0.00000 [lindex [lindex $meas_tbl 0] 1]  0.000000 $LogFile $ModuleName] == 0} {
}

    set universeMask [b2 not mask $emptyMask]
        set SubTestDes "not empty and test: \[b2 not mask <Empty-mask>\]"
        if {[ReportTestStatus $LogFile  [ expr {$universeMask >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $universeMask]
        set SubTestDes "universeMask num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "universeMask dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "universeMask dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "universeMask dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $universeMask]

        set SubTestDes "universeMask num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "universeMask res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "universeMask res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "universeMask res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set uniVol [expr ($xDims * $yDims * $zDims * $xRes * $yRes * $zRes / 1000.0)]
    set meas_tbl [b2 measure volume mask $universeMask]
              if {[SingleMeasureEpsilonTest "universe mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  $uniVol $LogFile $ModuleName] == 0} {
}

        set img7 [b2 create bullet-image 20 0 $xDims $yDims $zDims $xRes $yRes $zRes 0 0 0 $xDims $yDims $zDims]
    set maskU [b2 threshold image $img7 1.0]
    set meas_tab [b2 measure volume mask $maskU]
              if {[SingleMeasureEpsilonTest "threshold mask for largest bullet volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  $uniVol $LogFile $ModuleName] == 0} {
}

              if {[SingleMeasureEpsilonTest "universe mask volume == threshold mask volume for largest bullet" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}

    set img8 [b2 sum masks [list $universeMask $maskU]]

    set mask8 [b2 or masks $universeMask $maskU]
    set meas_img [b2 measure image mask $mask8 $img8]
          set standards {
{Mean 2.000000000000000} {Standard-Deviation 0.000000000000000} {Skewness 0.000000000000000} {Kurtosis 0.000000000000000}
    }
              if {[CoreMeasuresEpsilonTest "overlap image measure mean test" 0.00001 $standards $meas_img  $LogFile $ModuleName] == 0} {
}

    set imgR [b2 sum images [list $imgA $imgB]]
    set maskR [b2 threshold image $imgR 1.0]
    set maskQ [b2 or masks $maskA $maskB]
        set SubTestDes "ring or test: \[b2 or masks <mask-A> <mask-B>\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskQ >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $maskQ]
        set SubTestDes "<mask-or> num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-or> dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-or> dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-or> dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $maskQ]

        set SubTestDes "<mask-or> num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-or> res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-or> res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-or> res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "<mask-or> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_union [b2 measure volume mask $maskR]
        set SubTestDes "ring image threshold mask volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_union != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "<mask-or> volume == ring image threshold mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_union 0] 1] $LogFile $ModuleName] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $imgR ] != -1 } ] $ModuleName "Destroying image $imgR"

    set imgP [b2 sum masks [list $maskA $maskB]]
        set SubTestDes "ring and by sum test: \[b2 sum masks {<mask-A> <mask-B>}\]"
        if {[ReportTestStatus $LogFile  [ expr {$imgP >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims image $imgP]
        set SubTestDes "<and-by-sum> num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<and-by-sum> dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<and-by-sum> dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<and-by-sum> dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res image $imgP]

        set SubTestDes "<and-by-sum> num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<and-by-sum> res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<and-by-sum> res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<and-by-sum> res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set maskP [b2 threshold image $imgP 2.0]
    set imgR [b2 multiply images [list $imgA $imgB]]
    set maskR [b2 threshold image $imgR 1.0]
    set maskQ [b2 and masks $maskA $maskB]
        set SubTestDes "ring and test: \[b2 and masks <mask-A> <mask-B>\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskQ >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $maskQ]
        set SubTestDes "<mask-and> num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-and> dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-and> dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-and> dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $maskQ]

        set SubTestDes "<mask-and> num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-and> res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-and> res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-and> res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "<mask-and> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_inter [b2 measure volume mask $maskR]
        set SubTestDes "ring image threshold mask volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_inter != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "<mask-and> volume == ring image threshold mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_inter 0] 1] $LogFile $ModuleName] == 0} {
}

    set meas_inter [b2 measure volume mask $maskP]
        set SubTestDes "ring sum mask intersection volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_inter != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "<mask-and> volume == ring sum mask intersection volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_inter 0] 1] $LogFile $ModuleName] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $imgR ] != -1 } ] $ModuleName "Destroying image $imgR"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskP ] != -1 } ] $ModuleName "Destroying mask $maskP"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $imgP ] != -1 } ] $ModuleName "Destroying image $imgP"

    set maskQ [b2 xor masks $maskA $maskB]
        set SubTestDes "ring xor test: \[b2 xor masks <mask-A> <mask-B>\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskQ >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $maskQ]
        set SubTestDes "<mask-xor> num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-xor> dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-xor> dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-xor> dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $maskQ]

        set SubTestDes "<mask-xor> num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-xor> res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-xor> res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "<mask-xor> res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "<mask-xor> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_diff [expr ([lindex [lindex $meas_union 0] 1] - [lindex [lindex $meas_inter 0] 1])]
              if {[SingleMeasureEpsilonTest "<mask-xor> volume == ring image check volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  $meas_diff $LogFile $ModuleName] == 0} {
}
    set meas_diff [expr ([lindex [lindex $meas_union 0] 1] - [lindex [lindex $meas_inter 0] 1])]

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

        set maskX [b2 split mask $maskA z 90 +]
        set SubTestDes "split mask test: \[b2 split mask <mask-A> z 90 +\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskX >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $maskX]
        set SubTestDes "split mask num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "split mask dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "split mask dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "split mask dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $maskX]

        set SubTestDes "split mask num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "split mask res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "split mask res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "split mask res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

        set maskY [b2 split mask $maskA z 90 -]
        set SubTestDes "split mask test: \[b2 split mask <mask-A> z 90 -\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskY >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set maskQ [b2 and masks $maskX $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "split and intersect volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "empty mask volume" 0.00000 [lindex [lindex $meas_tbl 0] 1]  0.000000 $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

    set maskQ [b2 or masks $maskX $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "split and union volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tab [b2 measure volume mask $maskA]
        set SubTestDes "<mask-A> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}


              if {[SingleMeasureEpsilonTest "split and union volume:  sum of parts volume == whole volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

        set maskZ [b2 dilate mask $maskX 1]
        set SubTestDes "dilate mask test: \[b2 dilate mask <mask-X> 1\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskZ >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $maskZ]
        set SubTestDes "dilate mask num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "dilate mask dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "dilate mask dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "dilate mask dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $maskZ]

        set SubTestDes "dilate mask num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "dilate mask res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "dilate mask res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "dilate mask res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set maskQ [b2 and masks $maskZ $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "dilate and intersect plane volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "dilate and intersect mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  0.012500 $LogFile $ModuleName] == 0} {
}

    set maskR [b2 erode mask $maskQ 1]
    set meas_tbl [b2 measure volume mask $maskR]
        set SubTestDes "erode leaf volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "erode leaf mask volume" 0.00000 [lindex [lindex $meas_tbl 0] 1]  0.000000 $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"

        set maskW [b2 erode mask $maskZ 1]
        set SubTestDes "erode mask test: \[b2 erode mask <mask-Z> 1\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskW >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set dim_ret [b2 get dims mask $maskW]
        set SubTestDes "erode mask num dims test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $dim_ret] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "erode mask dim\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 0] == $xDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "erode mask dim\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 1] == $yDims } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "erode mask dim\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $dim_ret 2] == $zDims } ] $ModuleName $SubTestDes] == 0} {
}

        set test_res [b2 get res mask $maskW]

        set SubTestDes "erode mask num res test"
        if {[ReportTestStatus $LogFile  [ expr {[llength $test_res] == 3 } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "erode mask res\[0\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 0] == $xRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "erode mask res\[1\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 1] == $yRes } ] $ModuleName $SubTestDes] == 0} {
}

        set SubTestDes "erode mask res\[2\] test"
        if {[ReportTestStatus $LogFile  [ expr {[lindex $test_res 2] == $zRes } ] $ModuleName $SubTestDes] == 0} {
}

    set meas_tbl [b2 measure volume mask $maskW]
        set SubTestDes "dilate and erode volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tab [b2 measure volume mask $maskX]
        set SubTestDes "<mask-X> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}


              if {[SingleMeasureEpsilonTest "erode mask:  sum of parts volume == whole volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskW  ] != -1 } ] $ModuleName "Destroying mask $maskW "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskX  ] != -1 } ] $ModuleName "Destroying mask $maskX "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskY  ] != -1 } ] $ModuleName "Destroying mask $maskY "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskZ      ] != -1 } ] $ModuleName "Destroying mask $maskZ     "

        set maskX [b2 split mask $maskA y 65 +]
        set SubTestDes "split mask test: \[b2 split mask <mask-A> y 65 +\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskX >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set maskY [b2 split mask $maskA y 65 -]
        set SubTestDes "split mask test: \[b2 split mask <mask-A> y 65 -\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskY >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set maskQ [b2 and masks $maskX $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "split and intersect volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "empty mask volume" 0.00000 [lindex [lindex $meas_tbl 0] 1]  0.000000  $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

    set maskQ [b2 or masks $maskX $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "split and union volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tab [b2 measure volume mask $maskA]
        set SubTestDes "<mask-A> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "split mask wrt y:  sum of parts volume == whole volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

        set maskZ [b2 dilate mask $maskX 1]
        set SubTestDes "dilate mask test: \[b2 dilate mask <mask-X> 1\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskZ >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set maskQ [b2 and masks $maskZ $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "dilate and intersect plane volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "dilate and intersect mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  0.012500 $LogFile $ModuleName] == 0} {
}

    set maskR [b2 erode mask $maskQ 1]
    set meas_tbl [b2 measure volume mask $maskR]
        set SubTestDes "erode leaf volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "erode leaf mask volume" 0.00000 [lindex [lindex $meas_tbl 0] 1]  0.000000 $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskX  ] != -1 } ] $ModuleName "Destroying mask $maskX "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskY  ] != -1 } ] $ModuleName "Destroying mask $maskY "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskZ      ] != -1 } ] $ModuleName "Destroying mask $maskZ     "

        set maskX [b2 split mask $maskA x 48 +]
        set SubTestDes "split mask test: \[b2 split mask <mask-A> x 48 +\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskX >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

        set maskY [b2 split mask $maskA x 48 -]
        set SubTestDes "split mask test: \[b2 split mask <mask-A> x 48 -\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskY >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set maskQ [b2 and masks $maskX $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "split and intersect volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "empty mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  0.000000 $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

    set maskQ [b2 or masks $maskX $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "split and union volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tab [b2 measure volume mask $maskA]
        set SubTestDes "<mask-A> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}


              if {[SingleMeasureEpsilonTest "split mask wrt x:  sum of parts volume == whole volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"

        set maskZ [b2 dilate mask $maskX 1]
        set SubTestDes "dilate mask test: \[b2 dilate mask <mask-X> 1\]"
        if {[ReportTestStatus $LogFile  [ expr {$maskZ >= 0 } ] $ModuleName $SubTestDes] == 0} {
}

    set maskQ [b2 and masks $maskZ $maskY]
    set meas_tbl [b2 measure volume mask $maskQ]
        set SubTestDes "dilate and intersect plane volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "dilate and intersect mask volume" 0.00001 [lindex [lindex $meas_tbl 0] 1]  0.012500 $LogFile $ModuleName] == 0} {
}

    set maskR [b2 erode mask $maskQ 1]
    set meas_tbl [b2 measure volume mask $maskR]
        set SubTestDes "erode leaf volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}

              if {[SingleMeasureEpsilonTest "erode leaf mask volume" 0.00000 [lindex [lindex $meas_tbl 0] 1]  0.000000 $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskX  ] != -1 } ] $ModuleName "Destroying mask $maskX "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskY  ] != -1 } ] $ModuleName "Destroying mask $maskY "
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskZ      ] != -1 } ] $ModuleName "Destroying mask $maskZ     "



    set maskQ [b2 dilate mask $maskB 2]
    set maskR [b2 not mask $maskQ]
    set maskP [b2 dilate mask $maskR 2]
    set maskG [b2 not mask $maskP]

    set meas_tbl [b2 measure volume mask $maskG]
        set SubTestDes "dilate-not-dilate-not test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tab [b2 measure volume mask $maskB]
        set SubTestDes "<mask-B> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}


              if {[SingleMeasureEpsilonTest "dilate and invert:  result volume == original volume" 0.01 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskP ] != -1 } ] $ModuleName "Destroying mask $maskP"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskG ] != -1 } ] $ModuleName "Destroying mask $maskG"



    set maskR [b2 not mask $maskQ]
    set maskP [b2 erode mask $maskR 2]
    set maskG [b2 not mask $maskP]
    set maskH [b2 erode mask $maskG 2]

    set meas_tbl [b2 measure volume mask $maskH]
        set SubTestDes "not-erode-not-erode test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}
    set meas_tab [b2 measure volume mask $maskQ]
        set SubTestDes "<mask-Q> volume test"
        if {[ReportTestStatus $LogFile  [ expr {$meas_tbl != -1 } ] $ModuleName $SubTestDes] == 0} {
}


              if {[SingleMeasureEpsilonTest "invert and erode:  result volume == original volume" 0.01 [lindex [lindex $meas_tbl 0] 1]  [lindex [lindex $meas_tab 0] 1] $LogFile $ModuleName] == 0} {
}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskH ] != -1 } ] $ModuleName "Destroying mask $maskH"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskR ] != -1 } ] $ModuleName "Destroying mask $maskR"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskP ] != -1 } ] $ModuleName "Destroying mask $maskP"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskG ] != -1 } ] $ModuleName "Destroying mask $maskG"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskQ ] != -1 } ] $ModuleName "Destroying mask $maskQ"




# Clean up Files


# Free memory

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $mask8 ] != -1 } ] $ModuleName "Destroying mask $mask8"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img8 ] != -1 } ] $ModuleName "Destroying image $img8"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskU ] != -1 } ] $ModuleName "Destroying mask $maskU"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img7 ] != -1 } ] $ModuleName "Destroying image $img7"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $universeMask ] != -1 } ] $ModuleName "Destroying mask $universeMask"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $emptyMask ] != -1 } ] $ModuleName "Destroying mask $emptyMask"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img1 ] != -1 } ] $ModuleName "Destroying image $img1"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img2 ] != -1 } ] $ModuleName "Destroying image $img2"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img3 ] != -1 } ] $ModuleName "Destroying image $img3"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $imgA ] != -1 } ] $ModuleName "Destroying image $imgA"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskA ] != -1 } ] $ModuleName "Destroying mask $maskA"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskS ] != -1 } ] $ModuleName "Destroying mask $maskS"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img4 ] != -1 } ] $ModuleName "Destroying image $img4"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img5 ] != -1 } ] $ModuleName "Destroying image $img5"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $img6 ] != -1 } ] $ModuleName "Destroying image $img6"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $imgB ] != -1 } ] $ModuleName "Destroying image $imgB"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskB ] != -1 } ] $ModuleName "Destroying mask $maskB"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $maskT ] != -1 } ] $ModuleName "Destroying mask $maskT"

        return [ StopModule  $LogFile $ModuleName ]
}

