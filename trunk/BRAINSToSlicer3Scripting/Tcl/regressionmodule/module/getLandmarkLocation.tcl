# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2_get_landmark_location command
# \fn        proc getLandmarkNames {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_landmark_location command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getLandmarkLocation {pathToRegressionDir dateString} {

    set ModuleName "getLandmarkLocation"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_get_landmark_location command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    set ExpectLocs { {127.000000 127.000000 104.000000} {127.000000 127.000000 78.000000} \
                    {150.000000 132.000000 64.000000} {102.000000 130.000000 65.000000} \
                    {156.000000 137.000000 129.000000} {95.000000 135.000000 133.000000} \
                    {145.000000 109.000000 108.000000} {185.000000 109.000000 108.000000} \
                    {159.000000 101.000000 123.000000} {108.000000 109.000000 110.000000} \
                    {71.000000 109.000000 110.000000} {88.000000 104.000000 122.000000} \
                    {128.000000 104.000000 52.000000} {145.000000 101.000000 38.000000} \
                    {113.000000 101.000000 37.000000} {128.000000 106.000000 38.000000} \
                    {128.000000 117.000000 64.000000} {128.000000 132.000000 50.000000} \
                    {128.000000 112.000000 42.000000} {154.000000 112.000000 57.000000} \
                    {106.000000 112.000000 60.000000} {160.000000 93.000000 54.000000} \
                    {160.000000 91.000000 67.000000} {160.000000 99.000000 25.000000} \
                    {160.000000 115.000000 49.000000} {158.000000 104.000000 65.000000} \
                    {175.000000 86.000000 51.000000} {182.000000 98.000000 51.000000} \
                    {98.000000 93.000000 52.000000} {98.000000 90.000000 65.000000} \
                    {98.000000 100.000000 23.000000} {98.000000 115.000000 47.000000} \
                    {102.000000 105.000000 66.000000} {84.000000 88.000000 48.000000} \
                    {78.000000 99.000000 46.000000} }

    set SubTestDes "Get Landmark location - Load Landmark test"
    set TestLandID [b2_load_landmark $pathToRegressionDir/SGI/MR/B2-Land/TEST/10_ACPC/user/lewarpCln.lnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_get_landmark_location]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2_get_landmark_location $TestLandID]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_get_landmark_names $TestLandID -1 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_get_landmark_location $TestLandID -1 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid landmark test"
    set errorTest [b2_get_landmark_location -1 -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid location test"
    set errorTest [b2_get_landmark_location -1 256]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Landmark location All test"
    set landLocs [b2_get_landmark_location $TestLandID -1]
    ReportTestStatus $LogFile  [ expr {[llength $landLocs] == 35 } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $ExpectLocs]} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $landLocs $i] $j] == [lindex [lindex $ExpectLocs $i] $j] } ] $ModuleName "Landmark $i values $j test"
        }
    }

    for {set i 0} {$i < [llength $ExpectLocs]} {incr i} {
        set SubTestDes "Landmark location Index($i) Length test"
        set landLocs [b2_get_landmark_location $TestLandID $i]
        ReportTestStatus $LogFile  [ expr {[llength $landLocs] == 1 } ] $ModuleName $SubTestDes
        set SubTestDes "Landmark location Index($i) location $j test"
        for {set j 0} {$j < 3} {incr j} {
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $landLocs 0] $j]== [lindex [lindex $ExpectLocs $i] $j] } ] $ModuleName "Landmark Index $i values $j test"
        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

