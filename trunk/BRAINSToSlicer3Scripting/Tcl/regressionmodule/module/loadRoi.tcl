# \author  Hans J. Johnson"
# \date    $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief  This module tests the loading of various roi file formats
# \fn    proc loadRoi {pathToRegressionDir dateString}
# \param  string pathToRegressionDir  - Path to the regresssion test directory
# \param  string dateString      - String to label output file
# \result  1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS ROI
# Load a BRAINS2 ROI
#
# To Do
#------------------------------------------------------------------------
# Nothing
#

proc loadRoi {pathToRegressionDir dateString} {

    set ModuleName "loadRoi"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 load roi command and loading various ROI file formats"
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
    set errorTest [b2 load roi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 load roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/101tricia_rcaud.zroi junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 load roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/101tricia_rcaud.zroi junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2 load roi /invalid_directory_name/101tricia_rcaud.zroi]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################### BRAINS ROIs ###########################################
    set SubTestDes "Load BRAINS ROI test"
    set TestRoiID [b2 load roi $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/101tricia_rcaud.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ]} {

      set SubTestDes "Load BRAINS ROI test - Load Mask Test"
      set TestMaskId [b2 load mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/ANON013_tricia_rcaud.mask]
      ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

      set SubTestDes "Load BRAINS ROI test - Convert ROI To Mask Test"
      set NewMaskId [ b2 convert roi to mask $TestRoiID]
      ReportTestStatus $LogFile  [ expr {$NewMaskId != -1 } ] $ModuleName $SubTestDes

      set SubTestDes "Load BRAINS ROI test - Difference Mask Test"
      set diffMask [ b2 xor masks $TestMaskId $NewMaskId]
      ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

      set SubTestDes "Load BRAINS ROI test - Difference Volume Test"
      set volume [ b2 measure volume mask $diffMask]
      ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

      ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"
      ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
      ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $NewMaskId ] != -1 } ] $ModuleName "Destroying mask $NewMaskId"
      ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"

    }


    ############################### BRAINS2 ROIs ###########################################
    set SubTestDes "Load BRAINS2 ROI test"
    set TestRoiID [b2 load roi $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/user/brain.zroi]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ]} {

      set SubTestDes "Load BRAINS2 ROI test - Load Mask Test"
      set TestMaskId [b2 load mask $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_brain_trim.mask]
      ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

      set SubTestDes "Load BRAINS2 ROI test - Convert ROI To Mask Test"
      set NewMaskId [ b2 convert roi to mask $TestRoiID]
      ReportTestStatus $LogFile  [ expr {$NewMaskId != -1 } ] $ModuleName $SubTestDes

      set SubTestDes "Load BRAINS2 ROI test - Difference Mask Test"
      set diffMask [ b2 xor masks $TestMaskId $NewMaskId]
      ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

      set SubTestDes "Load BRAINS2 ROI test - Difference Volume Test"
      set volume [ b2 measure volume mask $diffMask]
      ReportTestStatus $LogFile  [ expr {[lindex [lindex $volume 0] 1] == 0.000000 } ] $ModuleName $SubTestDes

      ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"
      ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
      ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $NewMaskId ] != -1 } ] $ModuleName "Destroying mask $NewMaskId"
      ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"

    }

    set SubTestDes "Load BRAINS2 ROI With filter test"
    set TestRoiID [b2 load roi $pathToRegressionDir/SGI/MR/5x-B2/TEST/10_ACPC/user/brain.zroi filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiID != -1 } ] $ModuleName $SubTestDes ]} {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $TestRoiID ] != -1 } ] $ModuleName "Destroying roi $TestRoiID"
    }

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

