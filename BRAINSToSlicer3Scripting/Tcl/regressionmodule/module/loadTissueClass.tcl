# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the loading of various Tissue Class model
#             file formats
# \fn        proc loadTissueClass {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS2 Tissue Class model
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc loadTissueClass {pathToRegressionDir dateString} {


    set ModuleName "loadTissueClass"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_load_tissue-class command and loading various Tissue Class file formats"
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
    set errorTest [b2_load_tissue-class]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_load_tissue-class $pathToRegressionDir/SGI/MR/B2-Class-Mdl/TEST/05_ACPC/ANON0017_05_Tissue_Class.mdl junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_load_tissue-class $pathToRegressionDir/SGI/MR/B2-Class-Mdl/TEST/05_ACPC/ANON0017_05_Tissue_Class.mdl junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2_load_tissue-class /invalid_directory_name/ANON0017_05_Tissue_Class.mdl]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS2 Tissue Class ###########################################
    set SubTestDes "BRAINS2 Tissue Class Load test"
    set TestClassId [b2_load_tissue-class $pathToRegressionDir/SGI/MR/B2-Class-Mdl/TEST/05_ACPC/ANON0017_05_Tissue_Class.mdl]
    if { [ ReportTestStatus $LogFile  [ expr {$TestClassId != -1 } ] $ModuleName $SubTestDes] } {
        set kappa [b2_get_tissue-class_kappa $TestClassId]
#puts "kappa value is $kappa"
        set SubTestDes "BRAINS2 Tissue Class Kappa test"
        ReportTestStatus $LogFile  [ expr {[llength $kappa] == 4 } ] $ModuleName $SubTestDes
        set SubTestDes "BRAINS2 Tissue Class GM Kappa test"
        ReportTestStatus $LogFile  [ expr {[lindex [lindex $kappa 0] 1] == 0.755129992961884 } ] $ModuleName $SubTestDes
        set SubTestDes "BRAINS2 Tissue Class WM Kappa test"
        ReportTestStatus $LogFile  [ expr {[lindex [lindex $kappa 1] 1] == 0.757210016250610 } ] $ModuleName $SubTestDes
        set SubTestDes "BRAINS2 Tissue Class CSF Kappa test"
        ReportTestStatus $LogFile  [ expr {[lindex [lindex $kappa 2] 1] == 0.999960005283356 } ] $ModuleName $SubTestDes
        set SubTestDes "BRAINS2 Tissue Class VB Kappa test"
        ReportTestStatus $LogFile  [ expr {[lindex [lindex $kappa 3] 1] == 0.591979980468750 } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_tissue-class $TestClassId ] != -1 } ] $ModuleName "Destroying tissue-class $TestClassId"
    }


    return [ StopModule  $LogFile $ModuleName ]


    return $MODULE_SUCCESS
}

