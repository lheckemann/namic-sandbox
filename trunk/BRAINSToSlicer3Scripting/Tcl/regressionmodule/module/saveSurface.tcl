proc CoreSaveSurfaceTest {filterName SaveFileName TestMaskID LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set SubTestDes "$filterName Save Surface - Load saved Surface"
    set SaveSurfID [b2 load surface $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveSurfID != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Measure Surface Mask Test"
    set result [b2 measure surface mask $TestMaskID $SaveSurfID]
    ReportTestStatus $LogFile  [ expr {$result !=  -1} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Fundal Area Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 50222.117935) / 50222.117935 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Fundal Mean Curv Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 2] 1] - -297.468446) / 297.468446 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Gyral Area Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 6] 1] - 53029.336433) / 53029.336433 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Gyral Mean Curv Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 8] 1] - 293.603240) / 293.603240 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Fundal Mean Depth Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 20] 1] - 1.932073) / 1.932073 < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Save Surface - Gyral Mean Depth Test"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 26] 1] - 2.923401) / 2.923401 < 0.0001} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy surface $SaveSurfID ] != -1 } ] $ModuleName "Destroying surface $SaveSurfID"

}


# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the saving of various Surface file formats
# \fn        proc saveSurface {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS2 Surface
# Save an Inventor Surface
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveSurface {pathToRegressionDir dateString} {

    set ModuleName "saveSurface"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 save surface command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Surface - Load Surface test"
    set TestSurfaceID [b2 load surface $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_l_dec35.surf]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Surface - Load Mask test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2 save surface]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (2)"
    set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (3)"
    set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf brains2]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf brains2 $TestSurfaceID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf brains2 $TestSurfaceID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Surface ###########################################
    set SubTestDes "Save BRAINS2 Surface test"
    set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf brains2 $TestSurfaceID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveSurfaceTest "BRAINS2" ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf $TestMaskID $LogFile $ModuleName

        set errorTest [b2 save surface /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.srf brains2 $TestSurfaceID]
        set SubTestDes "BRAINS2 invalid filename test -- return was $errorTest"
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.srf brains2 $TestSurfaceID filter-suffix= -invalid]
        set SubTestDes "BRAINS2 invalid filter-suffix test -- return was $errorTest"
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }



    ############################### Inventor Surface ###########################################
    set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.iv  inventor $TestSurfaceID]
    set SubTestDes "Save INVENTOR Surface test -- return was $errorTest"
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        # Do we want to test the resulting Surface???????

        set SubTestDes "INVENTOR invalid filename test -- return was $errorTest"
        set errorTest [b2 save surface /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.iv inventor $TestSurfaceID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "INVENTOR invalid filter-suffix test -- return was $errorTest"
        set errorTest [b2 save surface ${OUTPUT_DIR}/TEST/10_ACPC/junk.iv inventor $TestSurfaceID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy surface $TestSurfaceID ] != -1 } ] $ModuleName "Destroying surface $TestSurfaceID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

