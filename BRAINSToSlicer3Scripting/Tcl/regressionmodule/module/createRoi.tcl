# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2 create roi"
# \fn        proc createRoi {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 create roi command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#

proc createRoi {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR
    set ModuleName "createRoi"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 create roi command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
########################################
########################################
# Run Tests
########################################
########################################

    ############################### Load an Image ###########################################
    set SubTestDes "Create Roi - Load Image test"
    set ImageId [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 create roi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 create roi test junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 create roi test junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################ BRAINS2 Create Roi Coronal ########################################
    set SubTestDes "B2 CREATE ROI Coronal test"
    set TestRoiId [b2 create roi test plane= coronal]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set name [b2 get name roi $TestRoiId]
        set SubTestDes "b2 create roi Coronal Name Test: does \"$name\" = \"test.zroi\""
        ReportTestStatus $LogFile  [ expr {$name == "test.zroi" } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ############################ BRAINS2 Create Roi Axial ########################################
    set SubTestDes "B2 CREATE ROI Axial test"
    set TestRoiId [b2 create roi test plane= axial]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set name [b2 get name roi $TestRoiId]
        set SubTestDes "b2 create roi Axial Name Test: does \"$name\" = \"test.yroi\""
        ReportTestStatus $LogFile  [ expr {$name == "test.yroi" } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ############################ BRAINS2 Create Roi Sagittal ########################################
    set SubTestDes "B2 CREATE ROI Sagittal test"
    set TestRoiId [b2 create roi test plane= sagittal]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set name [b2 get name roi $TestRoiId]
        set SubTestDes "b2 create roi Sagittal Name Test: does \"$name\" = \"test.xroi\""
        ReportTestStatus $LogFile  [ expr {$name == "test.xroi" } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ############################ BRAINS2 Create Roi Surface ########################################
    set SubTestDes "B2 CREATE ROI Surface test"
    set TestRoiId [b2 create roi test plane= surface]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set name [b2 get name roi $TestRoiId]
        set SubTestDes "b2 create roi Surface Name Test: does \"$name\" = \"test.sroi\""
        ReportTestStatus $LogFile  [ expr {$name == "test.sroi" } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ############################ BRAINS2 Create Roi Coronal-Name ########################################
    set SubTestDes "B2 CREATE ROI Coronal-Name test"
    set TestRoiId [b2 create roi test.zroi ]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] } {
        set name [b2 get name roi $TestRoiId]
        set SubTestDes "b2 create roi Coronal-Name Test: does \"$name\" = \"test.zroi\""
        ReportTestStatus $LogFile  [ expr {$name == "test.zroi" } ] $ModuleName $SubTestDes
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiId ] != -1 } ] $ModuleName "Destroying roi $TestRoiId"
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $ImageId ] != -1 } ] $ModuleName "Destroying image $ImageId"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

