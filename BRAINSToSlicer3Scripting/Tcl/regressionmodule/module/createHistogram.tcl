# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2 create histogram"
# \fn        proc createHemRoi {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 create histogram command
#
# To Do
#------------------------------------------------------------------------
# Write a BRAINS2 command to return a histogram in a list
#


proc createHistogram {pathToRegressionDir dateString} {


    set ModuleName "createHistogram"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 create histogram command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    ############################### Load an Image ###########################################
    set SubTestDes "Create Histogram - Load Image test"
    set ImageId [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load a Mask ###########################################
    set SubTestDes "Create Histogram - Load Mask test"
    set MaskId [b2 load mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 create histogram]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 create histogram $ImageId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 create histogram $ImageId $MaskId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 create histogram $ImageId $MaskId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2 create histogram 65536 $MaskId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid mask test"
    set errorTest [b2 create histogram $ImageId 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Create Histogram ########################################
    set SubTestDes "B2 CREATE HISOGRAM test"
    set TestHistId [b2 create histogram $ImageId $MaskId]
    ReportTestStatus $LogFile  [ expr {$TestHistId != -1 } ] $ModuleName $SubTestDes

    # b2 get histogram data ????????



    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $MaskId ] != -1 } ] $ModuleName "Destroying mask $MaskId"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $ImageId ] != -1 } ] $ModuleName "Destroying image $ImageId"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy histogram $TestHistId ] != -1 } ] $ModuleName "Destroying histogram $TestHistId"




    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

