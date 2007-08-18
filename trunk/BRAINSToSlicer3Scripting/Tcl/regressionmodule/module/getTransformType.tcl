# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the b2_get_transform_type command
# \fn        proc getTransformType {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 transform type command
#
# To Do
#------------------------------------------------------------------------
# Add bookstein and interleave fit transforms
#


proc getTransformType {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "getTransformType"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_get_transform_type command "
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Get transform type - Load transform test"
    set TestXfrmId [b2_load_transform $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }



    # First Test for invalid arguements
    set SubTestDes "Required arguement test"
    set errorTest [b2_get_transform_type]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Invalid arguement number test"
    set errorTest [b2_get_transform_type $TestXfrmId junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Invalid optional arguement test"
    set errorTest [b2_get_transform_type $TestXfrmId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Invalid object test"
    set errorTest [b2_get_transform_type -1]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Transform Type test"
    set XfrmType [b2_get_transform_type $TestXfrmId ]
    ReportTestStatus $LogFile  [ expr {$XfrmType == "Affine" } ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmId ] != -1 } ] $ModuleName "Destroying transform $TestXfrmId"


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

