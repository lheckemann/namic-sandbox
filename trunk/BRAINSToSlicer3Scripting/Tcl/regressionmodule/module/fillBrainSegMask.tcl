# \author    Greg Harris
# \date        $Date: 2007-01-27 14:44:03 -0600 (Sat, 27 Jan 2007) $
# \brief    This module tests the "b2 fill brainseg mask"
# \fn        proc fillBrainSegMask {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 fill brainseg mask command
#
# To Do
#------------------------------------------------------------------------
# None
#


proc fillBrainSegMask {pathToRegressionDir dateString} {


    set ModuleName "fillBrainSegMask"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 fill brainseg mask command"
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


    ############################### Load a Mask ###########################################
    set SubTestDes "Fill Brain seg mask - Load Image test"
#    set ImageId [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_stereo.hdr]
    set ImageId [b2 load image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_segment.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$ImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Fill Brain seg mask - Load Mask test"
#    set MaskId [b2 load mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    set MaskId [b2 load mask $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2 fill brainseg mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2 fill brainseg mask $MaskId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2 fill brainseg mask $MaskId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid mask test"
    set errorTest [b2 fill brainseg mask 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Threshold Image ########################################
    set SubTestDes "B2 FILL BRAINSEG Mask test - Threshold Image"
    set ThreshMaskId [b2 threshold image $ImageId 70]
    ReportTestStatus $LogFile  [ expr {$ThreshMaskId != -1 } ] $ModuleName $SubTestDes

    ############################ BRAINS2 XOR Masks ########################################
    set SubTestDes "B2 FILL BRAINSEG Mask test - AND Masks"
    set AndMaskId [b2 and masks $ThreshMaskId $MaskId]
    ReportTestStatus $LogFile  [ expr {$AndMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2 measure volume mask $AndMaskId]
    set standard {{CubicCentimeters 1095.904000}}
    set SubTestDes "B2 AND Masks test"
    CoreMeasuresEpsilonTest $SubTestDes 0.0001 $standard $result $LogFile $ModuleName

    set SubTestDes "B2 FILL BRAINSEG Mask test"
    set TestMaskId [b2 fill brainseg mask $AndMaskId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes ] } {

    set result [b2 measure volume mask $TestMaskId]
    set standard {{CubicCentimeters 1151.048000}}
    set SubTestDes "B2 FILL BRAINSEG Mask size test"
    CoreMeasuresEpsilonTest $SubTestDes 0.0001 $standard $result $LogFile $ModuleName
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }
    ############################ BRAINS2 Fill Internal Masks ########################################
    set SubTestDes "B2 FILL INTERNAL Mask test"
    set TestMaskId1 [b2 fill internal mask $AndMaskId]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskId1 != -1 } ] $ModuleName $SubTestDes ] } {

    set result [b2 measure volume mask $TestMaskId1]
    set standard {{CubicCentimeters 1161.352}}

    set SubTestDes "B2 FILL INTERNAL Mask"
    CoreMeasuresEpsilonTest $SubTestDes 0.001 $standard $result $LogFile $ModuleName
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $TestMaskId1 ] != -1 } ] $ModuleName "Destroying mask $TestMaskId1"
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $AndMaskId ] != -1 } ] $ModuleName "Destroying mask $AndMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $ThreshMaskId ] != -1 } ] $ModuleName "Destroying mask $ThreshMaskId"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $MaskId  ] != -1 } ] $ModuleName "Destroying mask $MaskId "

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $ImageId  ] != -1 } ] $ModuleName "Destroying image $ImageId "

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

