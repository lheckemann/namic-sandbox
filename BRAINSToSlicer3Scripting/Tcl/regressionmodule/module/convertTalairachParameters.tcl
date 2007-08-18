# \author    Hans J. Johnson"
# \date     $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests common components of loading any Talairach Parameters file format, i.e., if it
#           is a valid Talairach Parameter, then these items must be testable:
# \fn        proc CoreTalParTest {}
# \params  They should be self expanatory.
# \result    1 in case testing is complete or 0 in case of a fatal error
proc CoreTalParConvTest { LocationName ExpectedLoc TestLoc LogFile ModuleName SubTestDes} {
    global MODULE_SUCCESS
    global MODULE_FAILURE

        # Point Location
        for {set curpos 0} { $curpos < 3 } {incr curpos} {
            ReportTestStatus $LogFile  [ expr  {[lindex $ExpectedLoc $curpos ] == [lindex $TestLoc $curpos]}   ] $ModuleName "$SubTestDes $LocationName Location ($curpos)"
        }

        return $MODULE_SUCCESS
}



# set talPoints [b2_get_talairach_points $b1TalPar]


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
# Test resulting Talairach Transform???????
#


proc convertTalairachParameters {pathToRegressionDir dateString} {


    set ModuleName "convertTalairachParameters"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_convert_talairach-parameters_to_cerebellum-landmark | cortical-landmark | talairach-transform commands"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    set SubTestDes "Convert Talairach-Parameters to ... Load test (1)"
    set TestCrblBndId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/Cerebellum.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestCrblBndId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Convert Talairach-Parameters to ... Load test (2)"
    set TestTalBndId [b2_load_talairach-parameters $pathToRegressionDir/SGI/MR/DICOM-GE-B2/TEST/05_ACPC/Talairach.bnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTalBndId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }



    # First Test for invalid arguments

    ######### Cerebellum Landmarks
    set SubTestDes "required argument test (cerebellum-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cerebellum-landmark]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test (cerebellum-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cerebellum-landmark $TestTalBndId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test (cerebellum-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cerebellum-landmark $TestTalBndId  junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid Talairach-Parameter test (cerebellum-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cerebellum-landmark -100]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ######### Cortical Landmarks
    set SubTestDes "required argument test (cortical-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cortical-landmark]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test (cortical-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cortical-landmark $TestTalBndId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test (cortical-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cortical-landmark $TestTalBndId  junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid Talairach-Parameter test (cortical-landmark)"
    set errorTest [b2_convert_talairach-parameters_to_cortical-landmark -100]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ######### Talairach Transform
    set SubTestDes "required argument test (talairach-transform)"
    set errorTest [b2_convert_talairach-parameters_to_talairach-transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test (talairach-transform)"
    set errorTest [b2_convert_talairach-parameters_to_talairach-transform $TestTalBndId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test (talairach-transform)"
    set errorTest [b2_convert_talairach-parameters_to_talairach-transform $TestTalBndId  junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid Talairach-Parameter test (talairach-transform)"
    set errorTest [b2_convert_talairach-parameters_to_talairach-transform -100]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes




    ############################### Convert Talairach Parameters To Cerebellum Landmarks ###########################################
    set SubTestDes "Convert Talairach-Parameters To Cerebellum Landmarks test"
    set convertType "Cerebellum Landmarks"
    set V4 {161.000000 119.000000 114.000000}
    set IRP {67.000000 55.000000 28.000000}
    set SLA {259.000000 180.000000 193.000000}
    set TestLandId [b2_convert_talairach-parameters_to_cerebellum-landmark $TestCrblBndId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "Convert Talairach-Parameters To Cerebellum Landmarks location test"
        set locations [b2_get_landmark_location $TestLandId -1]
                puts "Cerebellum location name list: [b2_get_landmark_names $TestLandId]"
        ReportTestStatus $LogFile  [ expr {$locations != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cerebellum Landmarks location number test -- Does [llength  $locations] == 37 ?"
        ReportTestStatus $LogFile  [ expr {[llength  $locations] == 37 } ] $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cerebellum Landmarks 4V test"
        CoreTalParConvTest "4V" $V4 [lindex  $locations 0] $LogFile $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cerebellum Landmarks SLA test"
        CoreTalParConvTest "SLA" $SLA [lindex  $locations 1] $LogFile $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cerebellum Landmarks IRP test"
        CoreTalParConvTest "IRP" $IRP [lindex  $locations 8] $LogFile $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandId ] != -1 } ] $ModuleName "Destroying landmark $TestLandId"
    }

    ############################### Convert Talairach Parameters To Cortical Landmarks ###########################################
    set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks test"
    set convertType "Cortical Landmarks"
    set AC {159.000000 159.000000 218.000000}
    set PC {159.000000 159.000000 164.000000}
    set IRP {24.000000 76.000000 9.000000}
    set SLA {293.000000 307.000000 363.000000}
    set TestLandId [b2_convert_talairach-parameters_to_cortical-landmark $TestTalBndId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandId != -1 } ] $ModuleName $SubTestDes] } {
        set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks location test"
        set locations [b2_get_landmark_location $TestLandId -1]
                puts "Cerebral location name list: [b2_get_landmark_names $TestLandId]"
        ReportTestStatus $LogFile  [ expr {$locations != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks location number test -- Does [llength  $locations] == 28 ?"
        ReportTestStatus $LogFile  [ expr {[llength  $locations] == 28 } ] $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks AC test"
        CoreTalParConvTest "AC" $AC [lindex  $locations 0] $LogFile $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks PC test"
        CoreTalParConvTest "PC" $PC [lindex  $locations 1] $LogFile $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks SLA test"
        CoreTalParConvTest "SLA" $SLA [lindex  $locations 2] $LogFile $ModuleName $SubTestDes

        set SubTestDes "Convert Talairach-Parameters To Cortical Landmarks IRP test"
        CoreTalParConvTest "IRP" $IRP [lindex  $locations 9] $LogFile $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandId ] != -1 } ] $ModuleName "Destroying landmark $TestLandId"
    }



    ############################### Convert Talairach Parameters To Talairach Transform ###########################################
    set SubTestDes "Convert Talairach-Parameters To Talairach Transform test"
    set convertType "Talairach Transform"
    set TestXfrmId [b2_convert_talairach-parameters_to_talairach-transform $TestTalBndId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] } {
        # Do We have a way to test the result??????
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestCrblBndId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestCrblBndId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_talairach-parameters $TestTalBndId ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestTalBndId"


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

