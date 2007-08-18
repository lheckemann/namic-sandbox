# \author        Greg Harris"
# \date
# \brief        b2_average_landmarks
# \fn                proc averageLandmarks {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Is the average landmark point the average point?
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc averageLandmarks {pathToRegressionDir dateString} {
    set ModuleName "averageLandmarks"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2_average_landmarks"

    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests
    set num_ret [b2_average_landmarks]
    set SubTestDes "required argument test: \[b2_average_landmarks\]"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret == -1 } ] $ModuleName $SubTestDes] == 0} {
}

    set avg_cmd "b2_average_landmarks"
    set irp_x "expr (0.0"
    set irp_y "expr (0.0"
    set irp_z "expr (0.0"

    ############################### BRAINS Talairach Parameters ###########################################
    set SubTestDes "BRAINS Talairach-Parameters Load test"
        set TalTypeName "BRAINS"
        set AC {127.00000 127.00000 108.00000}
        set PC {127.00000 127.00000 84.00000}
        set IRP {55.00000 83.00000 12.00000}
        set SLA {199.00000 206.00000 172.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set SubTestDes "BRAINS Talairach-Parameters convert to landmarks test"
    set TestLand [b2_convert_talairach-parameters_to_cortical-landmark ${TestParamId}]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLand != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set avg_cmd "${avg_cmd} ${TestLand}"
    set locations [b2_get_talairach_points $TestParamId]
    set irp_x "${irp_x} + [lindex [lindex ${locations} 3] 0]"
    set irp_y "${irp_y} + [lindex [lindex ${locations} 3] 1]"
    set irp_z "${irp_z} + [lindex [lindex ${locations} 3] 2]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"

    ############################### BRAINS Picked By Hand Talairach Parameters ############################
    set SubTestDes "BRAINS Picked by hand Talairach-Parameters Load test"
    set TalTypeName "BRAINS-By-Hand"
        set AC {127.00000 127.00000 109.00000}
        set PC {127.00000 127.00000 83.00000}
        set IRP {63.00000 87.00000 10.00000}
        set SLA {193.00000 196.00000 174.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set SubTestDes "BRAINS Picked by hand Talairach-Parameters convert to landmarks test"
    set TestLand [b2_convert_talairach-parameters_to_cortical-landmark ${TestParamId}]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLand != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set avg_cmd "${avg_cmd} ${TestLand}"
    set locations [b2_get_talairach_points $TestParamId]
    set irp_x "${irp_x} + [lindex [lindex ${locations} 3] 0]"
    set irp_y "${irp_y} + [lindex [lindex ${locations} 3] 1]"
    set irp_z "${irp_z} + [lindex [lindex ${locations} 3] 2]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"

    ############################### BRAINS Adjust Talairach Parameters ############################
    set SubTestDes "BRAINS Adjust Talairach Parameters Load test"
    set TalTypeName "BRAINS-Adjust"
        set AC {127.00000 127.00000 109.00000}
        set PC {127.00000 127.00000 83.00000}
        set IRP {60.00000 79.00000 6.00000}
        set SLA {198.00000 206.00000 174.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/B1-Adj-TalPar/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set SubTestDes "BRAINS Adjust Talairach Parameters convert to landmarks test"
    set TestLand [b2_convert_talairach-parameters_to_cortical-landmark ${TestParamId}]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLand != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set avg_cmd "${avg_cmd} ${TestLand}"
    set locations [b2_get_talairach_points $TestParamId]
    set irp_x "${irp_x} + [lindex [lindex ${locations} 3] 0]"
    set irp_y "${irp_y} + [lindex [lindex ${locations} 3] 1]"
    set irp_z "${irp_z} + [lindex [lindex ${locations} 3] 2]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"

    ############################### BRAINS2 MR5 Talairach Parameters ############################
    set SubTestDes "BRAINS2 MR5 Talairach Parameters Load test"
    set TalTypeName "BRAINS2-MR5"
        set AC {127.00000 127.00000 108.00000}
        set PC {127.00000 127.00000 82.00000}
        set IRP {65.00000 87.00000 3.00000}
        set SLA {190.00000 201.00000 179.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set SubTestDes "BRAINS2 MR5 Talairach-Parameters convert to landmarks test"
    set TestLand [b2_convert_talairach-parameters_to_cortical-landmark ${TestParamId}]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLand != -1 } ] $ModuleName $SubTestDes] == 0 } {
}
    set avg_cmd "${avg_cmd} ${TestLand}"
    set locations [b2_get_talairach_points $TestParamId]
    set irp_x "${irp_x} + [lindex [lindex ${locations} 3] 0]"
    set irp_y "${irp_y} + [lindex [lindex ${locations} 3] 1]"
    set irp_z "${irp_z} + [lindex [lindex ${locations} 3] 2]"
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"


    set irp_x "${irp_x})/4.0"
    set irp_y "${irp_y})/4.0"
    set irp_z "${irp_z})/4.0"

    set std_x [eval $irp_x]
    set std_y [eval $irp_y]
    set std_z [eval $irp_z]

    set lnd [eval $avg_cmd]

    set lnd_locs [b2_get_landmark_location $lnd -1]
    set irp_loc [expr [llength $lnd_locs] - 1 - 9]
    set lnd_irp [lindex ${lnd_locs} ${irp_loc}]
    set lnd_x [lindex ${lnd_irp} 0]
    set lnd_y [lindex ${lnd_irp} 1]
    set lnd_z [lindex ${lnd_irp} 2]

    puts "std_x is ${std_x}"
    puts "std_y is ${std_y}"
    puts "std_z is ${std_z}"
    puts "lnd_x is ${lnd_x}"
    puts "lnd_y is ${lnd_y}"
    puts "lnd_z is ${lnd_z}"

    set SubTestDes "method avg equals known avg (in X) test"
    if {[ReportTestStatus $LogFile  [ expr {${lnd_x} == ${std_x} } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "method avg equals known avg (in Y) test"
    if {[ReportTestStatus $LogFile  [ expr {${lnd_y} == ${std_y} } ] $ModuleName $SubTestDes] == 0} {
}

    set SubTestDes "method avg equals known avg (in Z) test"
    if {[ReportTestStatus $LogFile  [ expr {${lnd_z} == ${std_z} } ] $ModuleName $SubTestDes] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $lnd ] != -1 } ] $ModuleName "Destroying landmark $lnd"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark [lindex ${avg_cmd} 3] ] != -1 } ] $ModuleName "Destroying landmark [lindex ${avg_cmd} 3]"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark [lindex ${avg_cmd} 4] ] != -1 } ] $ModuleName "Destroying landmark [lindex ${avg_cmd} 4]"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark [lindex ${avg_cmd} 5] ] != -1 } ] $ModuleName "Destroying landmark [lindex ${avg_cmd} 5]"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark [lindex ${avg_cmd} 6] ] != -1 } ] $ModuleName "Destroying landmark [lindex ${avg_cmd} 6]"

    return [ StopModule    $LogFile $ModuleName ]
}

