# \author    Vincent A. Magnotta
# \date        $Date: 2006-08-11 13:54:53 -0500 (Fri, 11 Aug 2006) $
# \brief    This module tests the "b2 normalize pet"
# \fn        proc resampleImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 normalize pet command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc normalizePet {pathToRegressionDir dateString} {


    set ModuleName "normalizePet"
    set ModuleAuthor "Vincent A. Magnotta"
    set ModuleDescription "Test the b2 normalize pet command"
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


    ############################### Load an Image ###########################################
    set SubTestDes "Normalize PET - Load PET Flow Image test"
    set PetFlowId [b2_load_image $pathToRegressionDir/SGI/pet_images/ge-4096/TEST/pcanonimiz__410.imf data-type= float-single]
    if { [ ReportTestStatus $LogFile  [ expr {$PetFlowId != -1 } ] $ModuleName $SubTestDes] == 0} {
          #return $MODULE_FAILURE
    }

    set SubTestDes "Normalize PET - Load PET Count Image test"
    set PetCountId [b2_load_image $pathToRegressionDir/SGI/pet_images/ge-4096/TEST/pcanonimiz__410.ima data-type= float-single]
    if { [ ReportTestStatus $LogFile  [ expr {$PetCountId != -1 } ] $ModuleName $SubTestDes] == 0} {
          #return $MODULE_FAILURE
    }

    set SubTestDes "Normalize PET - Load PET Flow Image 8bit test"
    set PetFlow8bitId [b2_load_image $pathToRegressionDir/SGI/pet_images/ge-4096/TEST/pcanonimiz__410.imf]
    if { [ ReportTestStatus $LogFile  [ expr {$PetFlow8bitId != -1 } ] $ModuleName $SubTestDes] == 0} {
          #return $MODULE_FAILURE
    }

    b2 compute wbbf $PetFlowId
    b2 compute wbbf $PetCountId
    b2 compute wbbf $PetFlow8bitId


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 normalize pet]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 normalize pet $PetFlowId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 normalize pet $PetFlowId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2 normalize pet 65536 ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 NORMALIZE PET test Flow"
    set TestImageId [b2 normalize pet $PetFlowId]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 NORMALIZE PET test - Flow - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 0.5]
    if {[ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes] != 0} {

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
puts "$result"
    set standards {
    {Mean 1.051735} {Mean-Absolute-Deviation 0.255599} {Variance 0.095954} {Standard-Deviation 0.309765} {Skewness 1.478819} {Kurtosis -0.463108} {Minimum 0.516527} {Maximum 2.496835}
    }
          CoreMeasuresEpsilonTest "NORMALIZE PET - Flow - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName
#    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 NORMALIZE PET test Count no Mean clipping"
    set TestImageId [b2 normalize pet $PetCountId clip-mean= False]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 NORMALIZE PET test - Count no Mean clipping - Mask Generation"
#    set TestMaskId [b2_threshold_image $TestImageId 0.5]
    if {[ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes] != 0} {

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
puts "$result"
    set standards {
    {Mean 1.072794} {Mean-Absolute-Deviation 0.219890} {Variance 0.069927} {Standard-Deviation 0.264437} {Skewness 1.432511} {Kurtosis -0.705176} {Minimum 0.571381} {Maximum 2.065100}
    }
          CoreMeasuresEpsilonTest "NORMALIZE PET - Count no Mean clipping - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName
#    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 Resample Worsley-MR ########################################
    set result [b2_measure_image_mask $TestMaskId $PetFlowId]
puts "$result"
    set standards {
    {Mean 61.077930} {Mean-Absolute-Deviation 14.843539} {Variance 323.608349} {Standard-Deviation 17.989118} {Skewness 1.478819} {Kurtosis -0.463108} {Minimum 29.996531} {Maximum 145.000000}
    }
          CoreMeasuresEpsilonTest "LOAD PET - Flow - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName

    set result [b2_measure_image_mask $TestMaskId $PetCountId]
puts "$result"
    set standards {
    {Mean 1686.004941} {Mean-Absolute-Deviation 345.579130} {Variance 172715.055969} {Standard-Deviation 415.590009} {Skewness 1.432511} {Kurtosis -0.705176} {Minimum 897.982910} {Maximum 3245.513184}
    }
          CoreMeasuresEpsilonTest "LOAD PET - Count - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName

    set result [b2_measure_image_mask $TestMaskId $PetFlow8bitId]
puts "$result"
    set standards {
    {Mean 61.074168} {Mean-Absolute-Deviation 14.853704} {Variance 323.786834} {Standard-Deviation 17.994078} {Skewness 1.479136} {Kurtosis -0.462093} {Minimum 29.796950} {Maximum 145.000000}
    }
          CoreMeasuresEpsilonTest "LOAD PET - 8 bit With Clip Mean - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName


    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 NORMALIZE PET test 8 bit With Clip Mean "
    set TestImageId [b2 normalize pet $PetFlow8bitId clip-mean= True]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 NORMALIZE PET test - 8 bit With Clip Mean - Mask Generation"
#    set TestMaskId [b2_threshold_image $TestImageId 0.5]
    if {[ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes] != 0} {

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
puts "$result"
    set standards {
    {Mean 1.051611} {Mean-Absolute-Deviation 0.255760} {Variance 0.095996} {Standard-Deviation 0.309833} {Skewness 1.479136} {Kurtosis -0.462093} {Minimum 0.513061} {Maximum 2.496695}
    }
          CoreMeasuresEpsilonTest "NORMALIZE PET - 8 bit With Clip Mean - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName
#    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "B2 NORMALIZE PET test - 8 bit With No Clip Mean "
    set TestImageId [b2 normalize pet $PetFlow8bitId clip-mean= False]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 NORMALIZE PET test - 8 bit With No Clip Mean Mask Generation"
#    set TestMaskId [b2_threshold_image $TestImageId 0.5]
    if {[ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes] != 0} {

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
puts "$result"
    set standards {
    {Mean 1.051611} {Mean-Absolute-Deviation 0.255760} {Variance 0.095996} {Standard-Deviation 0.309833} {Skewness 1.479136} {Kurtosis -0.462093} {Minimum 0.513061} {Maximum 2.496695}
    }
          CoreMeasuresEpsilonTest "NORMALIZE PET - 8 bit With No Clip Mean - measure threshold image" 0.0001 $standards $result  $LogFile $ModuleName
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $PetFlow8bitId ] != -1 } ] $ModuleName "Destroying image $PetFlow8bitId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $PetFlowId ] != -1 } ] $ModuleName "Destroying image $PetFlowId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $PetCountId ] != -1 } ] $ModuleName "Destroying image $PetCountId"

    return [ StopModule  $LogFile $ModuleName ]
    return $MODULE_SUCCESS
}

