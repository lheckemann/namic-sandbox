# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module compares two landmark sets
# \fn        proc CoreSaveLand {filterType OrigTalBndID NewTalBndID}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#

proc CoreSaveLand {filterType OrigLandID NewLandID LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "$filterType Get Original Landmark Locations"
    set origPoints [b2 get landmark location $OrigLandID -1]
    ReportTestStatus $LogFile  [ expr {$origPoints != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterType Get New Landmark Locations"
    set newPoints [b2 get landmark location $NewLandID -1]
    ReportTestStatus $LogFile  [ expr {$newPoints != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterType Get Landmark locations Number"
    ReportTestStatus $LogFile  [ expr {[llength newPoints] == [llength origPoints] } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength origPoints]} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            set SubTestDes "$filterType Landmark location Comparison Point $i Location $j"
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $newPoints $i] $j] == [lindex [lindex $origPoints $i] $j] } ] $ModuleName $SubTestDes
        }
    }

    return
}

# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various roi file formats
# \fn        proc saveLandmark {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS Talairach Parameters
# Save a BRAINS2 Talairach Parameters
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveLandmark {pathToRegressionDir dateString} {

    set ModuleName "saveLandmark"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save landmark command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Landmark - Load Landmark test"
    set TestLandID [b2 load landmark $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/landmark_locations]
    if { [ ReportTestStatus $LogFile  [ expr {$TestLandID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC/user } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 save landmark]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations brains1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations brains1 $TestLandID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations brains1 $TestLandID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS Landmarks ###########################################
    set SubTestDes "Save BRAINS Talairach Parameter test"
    set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations brains1 $TestLandID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        set SubTestDes "Save Landmark - Load saved BRAINS Landmark"
        set SaveLandID [b2 load landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations]
        ReportTestStatus $LogFile  [ expr {$SaveLandID != -1 } ] $ModuleName $SubTestDes

        CoreSaveLand "BRAINS" $TestLandID $SaveLandID $LogFile $ModuleName

        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $SaveLandID ] != -1 } ] $ModuleName "Destroying landmark $SaveLandID"

        set SubTestDes "BRAINS invalid filename test"
        set errorTest [b2 save landmark /invalid_directory_name/SGI/MR/4x-B1/TEST/10_ACPC/landmark_locations brains1 $TestLandID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS invalid filter-suffix test"
        set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/landmark_locations brains1 $TestLandID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }


    ############################### BRAINS2 Landmarks ###########################################
    set SubTestDes "Save BRAINS2 Landmark test"
    set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.lnd  brains2 $TestLandID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        set SubTestDes "Save Landmark - Load saved BRAINS Landmark"
        set SaveLandID [b2 load landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.lnd]
        ReportTestStatus $LogFile  [ expr {$SaveLandID != -1 } ] $ModuleName $SubTestDes

        CoreSaveLand "BRAINS2" $TestLandID $SaveLandID $LogFile $ModuleName

        ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $SaveLandID ] != -1 } ] $ModuleName "Destroying landmark $SaveLandID"

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2 save landmark /invalid_directory_name/SGI/MR/4x-B1/TEST/10_ACPC/junk/junk.lnd brains2 $TestLandID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2 save landmark ${OUTPUT_DIR}/TEST/10_ACPC/user/junk.lnd brains2 $TestLandID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $TestLandID ] != -1 } ] $ModuleName "Destroying landmark $TestLandID"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

