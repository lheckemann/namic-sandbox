# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the Talairach Parameters
# \fn        proc CoreSaveTalBnd {filterType OrigTalBndID NewTalBndID}
# \param    string filterType        - Filter used for saving
# \param    object OrigTalBndID        - Original Talairach Bounds
# \param     object NewTalBndID        - Saved Talairach Bounds
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

proc CoreSaveTalBnd {filterType OrigTalBndID NewTalBndID LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "$filterType Get Original Talairach Point Locations"
    set origPoints [b2 get talairach points $OrigTalBndID]
    ReportTestStatus $LogFile  [ expr {$origPoints != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterType Get New Talairach Point Locations"
    set newPoints [b2 get talairach points $NewTalBndID]
    ReportTestStatus $LogFile  [ expr {$newPoints != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterType Get Talairach Point Number"
    ReportTestStatus $LogFile  [ expr {[llength newPoints] == [llength origPoints] } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength origPoints]} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            set SubTestDes "$filterType Talairach Point Comparison Point $i Location $j"
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $newPoints $i] $j] == [lindex [lindex $origPoints $i] $j] } ] $ModuleName $SubTestDes
        }
    }
    puts "Get Dims"
    set origStandardDims [b2 get standard-dims talairach-parameters $OrigTalBndID]
    set newStandardDims [b2 get standard-dims talairach-parameters $NewTalBndID]
    set origResliceDims [b2 get reslice-dims talairach-parameters $OrigTalBndID]
    set newResliceDims [b2 get reslice-dims talairach-parameters $NewTalBndID]
    puts "Get Res"
    set origStandardRes [b2 get standard-res talairach-parameters $OrigTalBndID]
    set newStandardRes [b2 get standard-res talairach-parameters $NewTalBndID]
    set origResliceRes [b2 get reslice-res talairach-parameters $OrigTalBndID]
    set newResliceRes [b2 get reslice-res talairach-parameters $NewTalBndID]
    puts "Check Dims Res"
    for {set i 0} {$i < 3} {incr i} {
        set SubTestDes "$filterType Talairach Parameter Standard Dims Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newStandardDims $i] == [lindex $origStandardDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "$filterType Talairach Parameter Reslice Dims Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newResliceDims $i] == [lindex $origResliceDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "$filterType Talairach Parameter Standard Res Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newStandardRes $i] == [lindex $origStandardRes $i] } ] $ModuleName $SubTestDes
        set SubTestDes "$filterType Talairach Parameter Reslice Res Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newResliceRes $i] == [lindex $origResliceRes $i] } ] $ModuleName $SubTestDes
    }
    return $MODULE_SUCCESS
}

# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various Talairach Parameters file formats
# \fn        proc saveTalairachParameters {pathToRegressionDir dateString}
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



proc saveTalairachParameters {pathToRegressionDir dateString} {

    set ModuleName "saveTalairachParameters"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save Talairach-Parameters command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Talairach Parameters - Load Talairach Parameters test"
    set TestTalBndID [b2 load talairach-parameters $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTalBndID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 save talairach-parameters]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters brains1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters brains1 $TestTalBndID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters brains1 $TestTalBndID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS Talairach Parameters ###########################################
    set SubTestDes "Save BRAINS Talairach Parameter test"
    set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters brains1 $TestTalBndID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        set SubTestDes "Save Talairach Parameters - Load saved BRAINS Talairach Parameters"
        set SaveTalBndID [b2 load talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters]
        ReportTestStatus $LogFile  [ expr {$SaveTalBndID != -1 } ] $ModuleName $SubTestDes

        CoreSaveTalBnd "BRAINS" $TestTalBndID $SaveTalBndID $LogFile $ModuleName

        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $SaveTalBndID ] != -1 } ] $ModuleName "Destroying talairach-parameters $SaveTalBndID"

        set SubTestDes "BRAINS invalid filename test"
        set errorTest [b2 save talairach-parameters /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk_parameters brains1 $TestTalBndID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS invalid filter-suffix test"
        set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk_parameters brains1 $TestTalBndID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }


    ############################### BRAINS2 Talairach Parameters ###########################################
    set SubTestDes "Save BRAINS2 Talairach Parameter test"
    set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk.bnd brains2 $TestTalBndID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        set SubTestDes "Save Talairach Parameter - Load saved BRAINS2 Talairach Parameter"
        set SaveTalBndID [b2 load talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk.bnd]
        ReportTestStatus $LogFile  [ expr {$SaveTalBndID != -1 } ] $ModuleName $SubTestDes

        CoreSaveTalBnd "BRAINS2" $TestTalBndID $SaveTalBndID $LogFile $ModuleName

        ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $SaveTalBndID ] != -1 } ] $ModuleName "Destroying talairach-parameters $SaveTalBndID"

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2 save talairach-parameters /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.bnd brains2 $TestTalBndID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2 save talairach-parameters ${OUTPUT_DIR}/TEST/10_ACPC/junk.bnd brains2 $TestTalBndID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $TestTalBndID ] != -1 } ] $ModuleName "Destroying talairach-parameters $TestTalBndID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

