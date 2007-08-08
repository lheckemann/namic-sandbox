proc CoreSaveGTSurfaceTest {filterName SaveFileName TestMaskID LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set SubTestDes "$filterName Save GTSurface - Load saved GTSurface"
    set SaveSurfID [b2 load GTSurface $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveSurfID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save GTSurface - Measure GTSurface Mask Test"
    set result [b2 measure GTSurface mask $TestMaskID $SaveSurfID]
    ReportTestStatus $LogFile  [ expr {$result !=  -1} ] $ModuleName $SubTestDes
#puts $result
    set SubTestDes "$filterName Save GTSurface - Fundal Area Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 49046.568095) / 49046.568095 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save GTSurface - Fundal Mean Curv Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 2] 1] - -0.090393) / -0.090393 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save GTSurface - Gyral Area Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 6] 1] - 58938.336150) / 58938.336150 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save GTSurface - Gyral Mean Curv Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 8] 1] - 0.073822) / 0.073822 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save GTSurface - Fundal Mean Depth Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 20] 1] - 1.762441) / 1.762441 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save GTSurface - Gyral Mean Depth Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 26] 1] - 2.317714) / 2.317714 < 0.0001} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy GTSurface $SaveSurfID ] != -1 } ] $ModuleName "Destroying GTSurface $SaveSurfID"

}


# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the saving of various GTSurface file formats
# \fn        proc saveGTSurface {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS2 GTSurface
# Save an Inventor GTSurface
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveGTSurface {pathToRegressionDir dateString} {

    set ModuleName "saveGTSurface"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 save GTSurface command and saving various file formats"
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

    set SubTestDes "Save GTSurface - Load GTSurface test"
    set TestSurfaceID [b2 load GTSurface $pathToRegressionDir/SGI/MR/Surface-Gen/TEST/10_ACPC/ANON0008_l_dec.gts]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save GTSurface - Load Mask test"
    set TestMaskID [b2 load mask $pathToRegressionDir/SGI/MR/Surface-Gen/TEST/10_ACPC/ANON0008_brain_trim.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2 save GTSurface]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (2)"
    set errorTest [b2 save GTSurface ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (3)"
    set errorTest [b2 save GTSurface ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts GTS ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2 save GTSurface ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts GTS $TestSurfaceID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2 save GTSurface ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts GTS $TestSurfaceID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 GTSurface ###########################################
    set SubTestDes "Save BRAINS2 GTSurface test"
    set errorTest [b2 save GTSurface ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts GTS $TestSurfaceID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveGTSurfaceTest "BRAINS2" ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts $TestMaskID $LogFile $ModuleName

        set errorTest [b2 save GTSurface /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.gts GTS $TestSurfaceID]
        set SubTestDes "BRAINS2 invalid filename test -- return was $errorTest"
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set errorTest [b2 save GTSurface ${OUTPUT_DIR}/TEST/10_ACPC/junk.gts GTS $TestSurfaceID filter-suffix= -invalid]
        set SubTestDes "BRAINS2 invalid filter-suffix test -- return was $errorTest"
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy GTSurface $TestSurfaceID ] != -1 } ] $ModuleName "Destroying GTSurface $TestSurfaceID"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

