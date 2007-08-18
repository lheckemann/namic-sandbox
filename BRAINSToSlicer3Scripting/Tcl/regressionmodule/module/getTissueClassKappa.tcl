# \author    Greg Harris"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the b2_get_tissue-class_kappa command
# \fn        proc getTissueClassKappa {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_tissue-class_kappa
#
# To Do
#------------------------------------------------------------------------
# Fix free memory bug upon destroy of object
#


proc getTissueClassKappa {pathToRegressionDir dateString} {

    set ModuleName "getTissueClassKappa"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_get_tissue-class_kappa command"
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

    set SubTestDes "Get tissue-class kappa load test"
    set TestClassId [b2_load_tissue-class $pathToRegressionDir/SGI/MR/B2-Class-Mdl/TEST/05_ACPC/ANON0017_05_Tissue_Class.mdl]
    if { [ ReportTestStatus $LogFile  [ expr {$TestClassId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2_get_tissue-class_kappa]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_get_tissue-class_kappa $TestClassId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_get_tissue-class_kappa $TestClassId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid tissue-class model test"
    set errorTest [b2_get_tissue-class_kappa -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "BRAINS2 Tissue Class Kappa test"
set known_kappa {
{gm_kappa 0.755129992961884} {wm_kappa 0.757210016250610} {csf_kappa 0.999960005283356} {vb_kappa 0.591979980468750}
}

    set kappa [b2_get_tissue-class_kappa $TestClassId]
puts $kappa
CoreMeasuresEpsilonTest "BRAINS2 Tissue Class Kappa" 0.0001 $known_kappa $kappa $LogFile $ModuleName

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_tissue-class $TestClassId ] != -1 } ] $ModuleName "Destroying tissue-class $TestClassId"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

