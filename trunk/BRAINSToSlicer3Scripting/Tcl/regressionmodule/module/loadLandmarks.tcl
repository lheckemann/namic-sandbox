# \author    Hans J. Johnson"
# \date     $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests common components of loading any landmark file format, i.e., if it
#           is a valid landmark, then these items must be testable:
# \fn        proc CoreImageTest {}
# \params  They should be self expanatory.
# \result    1 in case testing is complete or 0 in case of a fatal error
proc CoreLandTest { LandTypeName TestLandID Index Position LogFile ModuleName SubTestDes} {
    global MODULE_SUCCESS
    global MODULE_FAILURE

        set SubTestDes "$LandTypeName Location test ($Index)"
        set location [b2_get_landmark_location $TestLandID $Index]
        ReportTestStatus $LogFile  [ expr { [llength [lindex $location 0]] == 3 } ] $ModuleName $SubTestDes
        for {set curpos 0} { $curpos < 3 } {incr curpos} {
            set SubTestDes "$LandTypeName Index ($Index) Location ($curpos)"
            ReportTestStatus $LogFile  [ expr  {[lindex [lindex $location 0] $curpos ] == [lindex $Position $curpos]}   ] $ModuleName $SubTestDes
        }

        return $MODULE_SUCCESS
}




# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the loading of various Landmark
#             file formats
# \fn        proc loadLandmark {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS Landmark File
# Load a BRAINS2 Landmark File
# Load a GEC Landmark File
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc loadLandmarks {pathToRegressionDir dateString} {

    set ModuleName "loadLandmarks"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 load Landmark command and loading various Landmark file formats"
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
    set errorTest [b2_load_landmark]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_landmark $pathToRegressionDir/4x-B1/TEST/10_ACPC/user/landmark_locations junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_landmark $pathToRegressionDir/4x-B1/TEST/10_ACPC/user/landmark_locations junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_landmark /invalid_directory_name/landmark_locations]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS Landmarks ###########################################
    set SubTestDes "BRAINS Landmark Load test"
    set LandTypeName "BRAINS"
    set TestLandID [b2_load_landmark $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/landmark_locations]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes] } {
        set LocationValues {170.00000 120.00000 83.00000}
        set LocationIndex 0
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        set LocationValues {161.00000 82.00000 101.00000}
        set LocationIndex 2
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        set LocationValues {127.00000 138.00000 74.00000}
        set LocationIndex 5
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"
    }


    ############################### BRAINS2 Landmarks ###########################################
    set SubTestDes "BRAINS2 Landmark Load test"
    set LandTypeName "BRAINS2"
    set TestLandID [b2_load_landmark $pathToRegressionDir/SGI/MR/B2-Land/TEST/10_ACPC/user/lewarpCln.lnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes] } {
        set LocationValues {95.000000 135.000000 133.000000}
        set LocationIndex 5
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        set LocationValues {128.00000 104.00000 52.00000}
        set LocationIndex 12
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        set LocationValues {78.000000 99.000000 46.000000}
        set LocationIndex 34
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"
    }



    ############################### GEC Landmarks ###########################################
    set SubTestDes "GEC Landmark Load test"
    set TestLandID [b2_load_landmark $pathToRegressionDir/SGI/MR/B2-Land/TEST/10_ACPC/user/lewarpCln.gec]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes] } {
        set LocationValues {95.000000 135.000000 133.000000}
        set LocationIndex 5
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        set LocationValues {128.00000 104.00000 52.00000}
        set LocationIndex 12
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        set LocationValues {78.000000 99.000000 46.000000}
        set LocationIndex 34
        CoreLandTest $LandTypeName $TestLandID $LocationIndex $LocationValues $LogFile $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"
    }

    ######################### Test with specifying the filter to use for loading ###############
    set SubTestDes "BRAINS2 Landmark Load with filter test"
    set TestLandID [b2_load_landmark $pathToRegressionDir/SGI/MR/B2-Land/TEST/10_ACPC/user/lewarpCln.lnd filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"
    }


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

