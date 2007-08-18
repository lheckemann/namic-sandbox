# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2_get_landmark_names command
# \fn        proc getLandmarkNames {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_landmark_names command
#
# To Do
#------------------------------------------------------------------------
# Possibly change this function to be similar to get landmark locations
#


proc getLandmarkNames {pathToRegressionDir dateString} {

    set ModuleName "getLandmarkNames"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_get_landmark_names command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    set ExpectNames {AC PC l_post_hippo r_post_hippo l_ant_ins r_ant_ins l_med_TP l_lat_TP \
                    l_ant_TP r_med_TP r_lat_TP r_ant_TP 4V l_crbl_horiz_corpus \
                    r_crbl_horiz_corpus mid_crbl_horiz mid_ant mid_sup mid_crbl_prim l_crbl_prim \
                    r_crbl_prim l_corpus l_crbl_horiz_ant l_crbl_horiz l_sup l_prim_extreme \
                    l_horiz_extreme l_extreme r_corpus r_crbl_horiz_ant r_crbl_horiz \
                    r_sup r_prim_extreme r_horiz_extreme r_extreme}

    set SubTestDes "Get Landmark names Load landmark test"
    set TestLandID [b2_load_landmark $pathToRegressionDir/SGI/MR/B2-Land/TEST/10_ACPC/user/lewarpCln.lnd]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_get_landmark_names]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_get_landmark_names $TestLandID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_get_landmark_names $TestLandID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid landmark test"
    set errorTest [b2_get_landmark_names -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Get Landmark names test"
    set landNames [b2_get_landmark_names $TestLandID]
    ReportTestStatus $LogFile  [ expr {[llength $landNames] == 35 } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $ExpectNames]} {incr i} {
        set SubTestDes "Get Landmark All names [lindex $ExpectNames $i] test"
        ReportTestStatus $LogFile  [ expr {[lindex $landNames $i] == [lindex $ExpectNames $i] } ] $ModuleName $SubTestDes
    }

##### Do we want to support index based names
#    for {set i 0} {$i < [llength $ExpectNames]} {incr i} {
#        set SubTestDes "Get Landmark Index name [lindex $ExpectNames $i] test"
#        set landName [b2_get_landmark_names $TestLandID $i]
#        ReportTestStatus $LogFile  [ expr {$landName == [lindex $ExpectNames $i] } ] $ModuleName $SubTestDes
#    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

