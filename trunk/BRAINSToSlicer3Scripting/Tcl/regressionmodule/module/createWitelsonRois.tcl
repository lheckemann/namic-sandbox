# \author    Greg Harris"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the "b2 create witelson-rois"
# \fn        proc createRoi {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 create witelson-roi command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc createWitelsonRois {pathToRegressionDir dateString} {


    set ModuleName "createWitelsonRois"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 create witelson-roi command"
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


    ############################### Load the ROI ###########################################
    set SubTestDes "Create Witelson Roi - Load Roi test"
    set RoiId [b2 load roi $pathToRegressionDir/SGI/MR/B2-Witelson/TEST/10_ACPC/user/corpus.xroi]
    if { [ ReportTestStatus $LogFile  [ expr {$RoiId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load the Landmark ###########################################
    set SubTestDes "Create Witelson Roi - Load Landmark test"
    set LandId [b2 load landmark $pathToRegressionDir/SGI/MR/B2-Witelson/TEST/10_ACPC/user/w-point.lnd]
    if { [ ReportTestStatus $LogFile  [ expr {$LandId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 create witelson-rois]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 create witelson-rois $RoiId ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 create witelson-rois $RoiId $LandId junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 create witelson-rois $RoiId $LandId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Create Witelson-Rois ########################################
    set SubTestDes "B2 CREATE WITELSON-ROIS test"
    set TestRoiId [b2 create witelson-rois $RoiId $LandId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestRoiId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }
puts $TestRoiId
    set SubTestDes "B2 CREATE WITELSON-ROIS N Segment test"
    if { [ ReportTestStatus $LogFile  [ expr {[llength  $TestRoiId] == 7 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################ Test the Generated Segments ########################################

    for {set curIndex 0} { $curIndex < 7 } {incr curIndex} {
        set SubTestDes "CREATE WITELSON-ROIS Load Seg $curIndex test"
        set RoiSegId [b2 load roi $pathToRegressionDir/SGI/MR/B2-Witelson/TEST/10_ACPC/user/witelson_[expr $curIndex + 1].xroi]
        if { [ ReportTestStatus $LogFile  [ expr {$RoiId != -1 } ] $ModuleName $SubTestDes] == 0} {
            return $MODULE_FAILURE
        }
        set SubTestDes "CREATE WITELSON-ROIS Create Original Mask $curIndex test"
        set origSegMask [ b2 convert roi to mask $RoiSegId]
        ReportTestStatus $LogFile  [ expr {$origSegMask != -1 } ] $ModuleName $SubTestDes
        set SubTestDes "CREATE WITELSON-ROIS Create New Mask $curIndex test"
        set newSegMask [ b2 convert roi to mask [lindex $TestRoiId $curIndex]]
        if {[ReportTestStatus $LogFile  [ expr {$newSegMask != -1 } ] $ModuleName $SubTestDes] != 0} {
            set SubTestDes "CREATE WITELSON-ROIS Create Difference Mask $curIndex test"
            set diffMask [ b2 xor masks $origSegMask $newSegMask]
            ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes
            set SubTestDes "CREATE WITELSON-ROIS Create Difference Volume $curIndex test"
            set volume [ b2 measure volume mask $diffMask]
              SingleMeasureEpsilonTest $SubTestDes 0.00000 [lindex [lindex $volume 0] 1] 0.000000000000000 $LogFile $ModuleName
        }
        ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $origSegMask ] != -1 } ] $ModuleName "Destroying mask $origSegMask"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $newSegMask ] != -1 } ] $ModuleName "Destroying mask $newSegMask"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $diffMask ] != -1 } ] $ModuleName "Destroying mask $diffMask"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $RoiSegId ] != -1 } ] $ModuleName "Destroying roi $RoiSegId"
    }

    for {set curIndex 0} { $curIndex < 7 } {incr curIndex} {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy roi [lindex $TestRoiId $curIndex] ] != -1 } ] $ModuleName "Destroying roi [lindex $TestRoiId $curIndex]"
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $RoiId ] != -1 } ] $ModuleName "Destroying roi $RoiId"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy landmark $LandId ] != -1 } ] $ModuleName "Destroying landmark $LandId"



    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

