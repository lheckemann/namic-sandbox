# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various Surface
#             file formats
# \fn        proc loadSurface {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS Surface File
# Load BRAINS2 Surface File
#
# To Do
#------------------------------------------------------------------------
# Add tests on data for checking loaded surface
#


proc loadGTSurface {pathToRegressionDir dateString} {


    set ModuleName "loadGTSurface"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 load surface command and loading various Surface file formats"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_load_GTSurface]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_GTSurface $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_l_dec35.surf junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_GTSurface $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_l_dec35.surf junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_GTSurface /invalid_directory_name/ANON013_l_dec35.surf]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


# Actually, ifdef __USE_OLD_SURFACES__
if {0==1} {
    ############################### BRAINS Surface ###########################################
    set SubTestDes "BRAINS Surface Load test"
    set TestSurfaceId [b2_load_GTSurface $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_l_dec35.surf]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceId != -1 } ] $ModuleName $SubTestDes] } {
        # Run Tests Here on the Data
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $TestSurfaceId ] != -1 } ] $ModuleName "Destroying GTSurface $TestSurfaceId"
    }

    ############################### BRAINS2 Surface ###########################################
    set SubTestDes "BRAINS2 Surface Load test"
    set TestSurfaceId [b2_load_GTSurface $pathToRegressionDir/SGI/MR/B2-Surf/TEST/05_ACPC/ANON0021_130_l_dec35.srf]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceId != -1 } ] $ModuleName $SubTestDes] } {
        # Run Tests Here on the Data
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $TestSurfaceId ] != -1 } ] $ModuleName "Destroying GTSurface $TestSurfaceId"
    }

    ############################### BRAINS2 Surface Filter ######################################
    set SubTestDes "BRAINS2 Surface with Filter Load test"
    set TestSurfaceId [b2_load_GTSurface $pathToRegressionDir/SGI/MR/B2-Surf/TEST/05_ACPC/ANON0021_130_l_dec35.srf filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceId != -1 } ] $ModuleName $SubTestDes] } {
        # Run Tests Here on the Data
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $TestSurfaceId ] != -1 } ] $ModuleName "Destroying GTSurface $TestSurfaceId"
    }
}
    ############################### BRAINS2 Surface #############################################
    set SubTestDes "GTS Surface with Filter Load test"
    set TestSurfaceId [b2_load_GTSurface $pathToRegressionDir/SGI/MR/Surface-Gen/TEST/10_ACPC/ANON0008_l_dec.gts]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceId != -1 } ] $ModuleName $SubTestDes] } {
        # Run Tests Here on the Data
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $TestSurfaceId ] != -1 } ] $ModuleName "Destroying GTSurface $TestSurfaceId"
    }

    ############################### BRAINS2 Surface Filter ######################################
    set SubTestDes "GTS Surface with Filter Load test"
    set TestSurfaceId [b2_load_GTSurface $pathToRegressionDir/SGI/MR/Surface-Gen/TEST/10_ACPC/ANON0008_l_dec.gts filter= GTS]
    if { [ ReportTestStatus $LogFile  [ expr {$TestSurfaceId != -1 } ] $ModuleName $SubTestDes] } {
        # Run Tests Here on the Data
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_GTSurface $TestSurfaceId ] != -1 } ] $ModuleName "Destroying GTSurface $TestSurfaceId"
    }

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

