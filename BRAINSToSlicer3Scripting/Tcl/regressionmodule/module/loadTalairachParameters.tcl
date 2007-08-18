# \author    Hans J. Johnson"
# \date     $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests common components of loading any Talairach Parameters file format, i.e., if it
#           is a valid Talairach Parameter, then these items must be testable:
# \fn        proc CoreTalParTest {}
# \params  They should be self expanatory.
# \result    1 in case testing is complete or 0 in case of a fatal error
proc CoreTalParTest { TalTypeName TestTalID AC PC IRP SLA LogFile ModuleName SubTestDes} {
    global MODULE_SUCCESS
    global MODULE_FAILURE

        set SubTestDes "$TalTypeName Talairach Parameters Location test"
        set locations [b2_get_talairach_points $TestTalID]
        set lentype [llength  $locations]
        ReportTestStatus $LogFile  [ expr { [llength $locations] == 4 } ] $ModuleName $SubTestDes

        set curAC [lindex $locations 0]
        set curPC [lindex $locations 1]
        set curSLA [lindex $locations 2]
        set curIRP [lindex $locations 3]

        # Check AC
        for {set curpos 0} { $curpos < 3 } {incr curpos} {
            set SubTestDes "$TalTypeName AC Location ($curpos)"
            ReportTestStatus $LogFile  [ expr  {[lindex $curAC $curpos ] == [lindex $AC $curpos]}   ] $ModuleName $SubTestDes
        }

        # Check PC
        for {set curpos 0} { $curpos < 3 } {incr curpos} {
            set SubTestDes "$TalTypeName PC Location ($curpos)"
            ReportTestStatus $LogFile  [ expr  {[lindex $curPC $curpos ] == [lindex $PC $curpos]}   ] $ModuleName $SubTestDes
        }

        # Check IRP
        for {set curpos 0} { $curpos < 3 } {incr curpos} {
            set SubTestDes "$TalTypeName IRP Location ($curpos)"
            ReportTestStatus $LogFile  [ expr  {[lindex $curIRP $curpos ] == [lindex $IRP $curpos]}   ] $ModuleName $SubTestDes
        }

        # Check SLA
        for {set curpos 0} { $curpos < 3 } {incr curpos} {
            set SubTestDes "$TalTypeName SLA Location ($curpos)"
            ReportTestStatus $LogFile  [ expr  {[lindex $curSLA $curpos ] == [lindex $SLA $curpos]}   ] $ModuleName $SubTestDes
        }

        return $MODULE_SUCCESS
}



# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the loading of various Talairach
#             parameter file formats
# \fn        proc loadTalairachParameter {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS Talairach Parameters - Very old
# Load a BRAINS Talairach Parameters - More Recent via script
# Load BRAINS Talairach Parameters which need adjustment
# Load BRAINS2 MR5 Talairach Parameters
# Load BRAINS2 MR6 Talairach Parameters
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc loadTalairachParameters {pathToRegressionDir dateString} {


    set ModuleName "loadTalairachParameters"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_load_talairach-parameters command and loading various Talairach-Parameter file formats"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_load_talairach-parameters]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_talairach-parameters /invalid_directory_name/talairach_parameters]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes




    ############################### BRAINS Talairach Parameters ###########################################
    set SubTestDes "BRAINS Talairach-Parameters Load test"
    set TalTypeName "BRAINS"
    set AC {127.00000 127.00000 108.00000}
    set PC {127.00000 127.00000 84.00000}
    set IRP {55.00000 83.00000 12.00000}
    set SLA {199.00000 206.00000 172.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS Picked By Hand Talairach Parameters ############################
    set SubTestDes "BRAINS Picked by hand Talairach-Parameters Load test"
    set TalTypeName "BRAINS-By-Hand"
    set AC {127.00000 127.00000 109.00000}
    set PC {127.00000 127.00000 83.00000}
    set IRP {63.00000 87.00000 10.00000}
    set SLA {193.00000 196.00000 174.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS Adjust Talairach Parameters ############################
    set SubTestDes "BRAINS Adjust Talairach Parameters Load test"
    set TalTypeName "BRAINS-Adjust"
    set AC {127.00000 127.00000 104.00000}
    set PC {127.00000 127.00000 78.00000}
    set IRP {60.00000 79.00000 6.00000}
    set SLA {198.00000 206.00000 174.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/B1-Adj-TalPar/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS2 MR5 Talairach Parameters ############################
    set SubTestDes "BRAINS2 MR5 Talairach Parameters Load test"
    set TalTypeName "BRAINS2-MR5"
    set AC {127.00000 127.00000 108.00000}
    set PC {127.00000 127.00000 82.00000}
    set IRP {65.00000 87.00000 3.00000}
    set SLA {190.00000 201.00000 179.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS Bogus Left Talairach Parameters ###########################################
    set SubTestDes "BRAINS Bogus Left Talairach-Parameters Load test"
    set TalTypeName "BRAINS-Bogus-Left"
    set AC {127.00000 127.00000 108.00000}
    set PC {127.00000 127.00000 84.00000}
    set IRP {55.00000 83.00000 12.00000}
    set SLA {248.00000 206.00000 172.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/bogus_left_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS Bogus Right Talairach Parameters ###########################################
    set SubTestDes "BRAINS Bogus Right Talairach-Parameters Load test"
    set TalTypeName "BRAINS-Bogus-Right"
    set AC {127.00000 127.00000 108.00000}
    set PC {127.00000 127.00000 84.00000}
    set IRP {13.00000 83.00000 12.00000}
    set SLA {199.00000 206.00000 172.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/bogus_right_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS2 MR6 Talairach Parameters ############################
    set SubTestDes "BRAINS2 MR6 Talairach Parameters Load test"
    set TalTypeName "BRAINS2-MR6"
    set AC {159.00000 159.00000 218.00000}
    set PC {159.00000 159.00000 164.00000}
    set IRP {24.00000 76.00000 9.00000}
    set SLA {293.00000 307.00000 363.00000}
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/Talairach.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        CoreTalParTest $TalTypeName $TestParamId $AC $PC $IRP $SLA $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }

    ############################### BRAINS2 MR5 Talairach Parameters with FIlter ############################
    set SubTestDes "BRAINS2 MR5 Talairach Parameters Load test"
    set TestParamId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestParamId != -1 } ] $ModuleName $SubTestDes] } {
        # set talPoints [b2_get_talairach_points $b1TalPar]
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestParamId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestParamId"
    }


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

