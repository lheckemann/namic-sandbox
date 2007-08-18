# \author  Greg Harris
# \date    $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief  This module tests the "b2 resample worsley-pet"
# \fn    proc resampleWorsleyPET {pathToRegressionDir dateString}
# \param  string pathToRegressionDir  - Path to the regresssion test directory
# \param  string dateString      - String to label output file
# \result  1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 resample worsley-pet command
#
# To Do
#------------------------------------------------------------------------
# This has not yet been fully tested
#


proc resampleWorsleyPET {pathToRegressionDir dateString} {
  global env


    set ModuleName "resampleWorsleyPET"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 resample worsley-pet command"
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


set results_dir "$env(ARCH)/$env(ABI)/$dateString"
set experiment_dir "TEST"
exec mkdir -p $results_dir/$experiment_dir
########################################
########################################
# Run Tests
########################################
########################################


    ############################### Load an Image ###########################################
    set SubTestDes "Resample Worsley PET - Load PET Flow Image test"
    set PetFlowImageId [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/pckja10cn01_416.imf data-type= float-single]
    if { [ ReportTestStatus $LogFile  [ expr {$PetFlowImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }

    set SubTestDes "Resample Worsley PET - Load PET Count Image test"
    set PetCountImageId [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/pckja10cn01_416.ima data-type= float-single]
    if { [ ReportTestStatus $LogFile  [ expr {$PetCountImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }

    set SubTestDes "Resample Worsley PET - Load Talairach Parameters test"
    set TalairachId [b2_load_talairach-parameters $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/10_ACPC/talairach_parameters]
    if { [ ReportTestStatus $LogFile  [ expr {$TalairachId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }

    set SubTestDes "Resample Worsley PET - Load PET Flow AIR Transform test"
    set PetFlowXfrmId [b2_load_transform $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/pckja10cn01_416.imf.air]
    if { [ ReportTestStatus $LogFile  [ expr {$PetFlowXfrmId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }

    set SubTestDes "Resample Worsley PET - Load PET Count AIR Transform test"
    set PetCountXfrmId [b2_load_transform $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/pckja10cn01_416.ima.air]
    if { [ ReportTestStatus $LogFile  [ expr {$PetCountXfrmId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }

    b2 compute wbbf $PetFlowImageId
    b2 compute wbbf $PetCountImageId


    b2_set_transform $PetFlowXfrmId image $PetFlowImageId
    b2_set_transform $PetCountXfrmId image $PetCountImageId

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 resample worsley-pet]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 resample worsley-pet $PetFlowImageId $TalairachId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 resample worsley-pet $PetFlowImageId $TalairachId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2 resample worsley-pet 65536 $TalairachId]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid Talairach Parameter test"
    set errorTest [b2 resample worsley-pet $PetFlowImageId 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test"
    set TestImageId [b2 resample worsley-pet $PetCountImageId $TalairachId brains-interp= True scale= True]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 5000]
    ReportTestStatus $LogFile  [ expr {$TestMaskId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
#    set expect 1183.043855
    set expect 1137.698242
    set fraction [ expr {abs([lindex [lindex $result 0] 1] - $expect) / $expect} ]
    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == $fraction"
    ReportTestStatus $LogFile  [ expr {$fraction < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Save Resampled Worsley Image (No-Label)"
    set saveResult [b2_save_image $results_dir/$experiment_dir/test_IMA.raw pic/raw $TestImageId data-type= signed-16bit plane= Axial]
    ReportTestStatus $LogFile  [ expr {$saveResult != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Save Resampled Worsley Image (PET-Label)"
    set saveResult [b2_save_image $results_dir/$experiment_dir/test_PET_IMA.raw pic/raw $TestImageId data-type= signed-16bit plane= Axial]
    ReportTestStatus $LogFile  [ expr {$saveResult != -1 } ] $ModuleName $SubTestDes


    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Load Resampled Worsley Image - (No-label)"
    set newWorsleyId [b2_load_image $results_dir/$experiment_dir/test_IMA.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Loaded (No-label) Mask Generation"
    set TestNewMaskId [b2_threshold_image $newWorsleyId 5000]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Difference Mask Generation"
    set diffMask [ b2_xor_masks $TestNewMaskId $TestMaskId]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Difference Mask Volume"
    set result [b2_measure_volume_mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $result 0] 1] == 0.000000} ] $ModuleName $SubTestDes

    b2_destroy_mask $diffMask
    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $newWorsleyId

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Load Resampled Worsley Image - (PET-Label)"
    set newWorsleyId [b2_load_image $results_dir/$experiment_dir/test_PET_IMA.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Loaded(labeled) Mask Generation"
    set TestNewMaskId [b2_threshold_image $newWorsleyId 500.0]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - labeled image Mask Volume"
    set result [b2_measure_volume_mask $TestNewMaskId]
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 1336.234851) / 1336.234851 < 0.0001} ] $ModuleName $SubTestDes

    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $newWorsleyId

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Load Old Worsley Image - (PET-Label)"
    set newWorsleyId [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/WORS_ANON00022_PET_HF_18_IMA_1.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Loaded Old Worsley(labeled) Mask Generation"
    set TestNewMaskId [b2_threshold_image $newWorsleyId 500.0]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - labeled image Mask Volume"
    set result [b2_measure_volume_mask $TestNewMaskId]
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 1374.127701) / 1374.127701 < 0.0001} ] $ModuleName $SubTestDes

    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $newWorsleyId

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Load Old Worsley Image"
    set oldWorsleyId [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/WORS_ANON00022_HF_18_IMA_1.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$oldWorsleyId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Old Mask Generation"
    set TestNewMaskId [b2_threshold_image $oldWorsleyId 5000]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Difference Old-Mask Generation"
    set diffMask [ b2_xor_masks $TestNewMaskId $TestMaskId]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD COUNT B2 RESAMPLE Worsley-PET test - Difference Old-Mask Volume"
    set result [b2_measure_volume_mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $result 0] 1] < 100.000000} ] $ModuleName $SubTestDes

    b2_destroy_mask $diffMask
    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $oldWorsleyId

    b2_destroy_mask $TestMaskId
    b2_destroy_image $TestImageId


    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Flow"
    set TestImageId [b2 resample worsley-pet $PetFlowImageId $TalairachId brains-interp= True scale= True filter-size= 18.33]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-MR test - Mask Generation - Flow"
    set TestMaskId [b2_threshold_image $TestImageId 5000]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
#    set expect 1454.959468
    set expect 1399.191435
    set fraction [ expr {abs([lindex [lindex $result 0] 1] - $expect) / $expect} ]
    set SubTestDes "OLD FLOW B2 RESAMPLE IMAGE test - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == $fraction"
    ReportTestStatus $LogFile  [ expr {$fraction < 0.0001} ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Save Resampled Worsley Image (No-Label)"
    set saveResult [b2_save_image $results_dir/$experiment_dir/test_IMF.raw pic/raw $TestImageId data-type= signed-16bit plane= Axial]
    ReportTestStatus $LogFile  [ expr {$saveResult != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Save Resampled Worsley Image (PET-Label)"
    set saveResult [b2_save_image $results_dir/$experiment_dir/test_PET_IMF.raw pic/raw $TestImageId data-type= signed-16bit plane= Axial]
    ReportTestStatus $LogFile  [ expr {$saveResult != -1 } ] $ModuleName $SubTestDes


    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Load Resampled Worsley Image - (No-label)"
    set newWorsleyId [b2_load_image $results_dir/$experiment_dir/test_IMF.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Loaded Mask Generation"
    set TestNewMaskId [b2_threshold_image $newWorsleyId 5000]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Difference Mask Generation"
    set diffMask [ b2_xor_masks $TestNewMaskId $TestMaskId]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Difference Mask Volume"
    set result [b2_measure_volume_mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $result 0] 1] == 0.000000} ] $ModuleName $SubTestDes

    b2_destroy_mask $diffMask
    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $newWorsleyId

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Load Resampled Worsley Image - (PET-Label)"
    set newWorsleyId [b2_load_image $results_dir/$experiment_dir/test_PET_IMF.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Loaded(labeled) Mask Generation"
    set TestNewMaskId [b2_threshold_image $newWorsleyId 15.0]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - labeled image Mask Volume"
    set result [b2_measure_volume_mask $TestNewMaskId]
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 1555.045322) / 1555.045322 < 0.0001} ] $ModuleName $SubTestDes

    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $newWorsleyId

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Load Old Worsley Image - (PET-Label)"
    set newWorsleyId [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/WORS_ANON00022_PET_HF_18_IMF_1.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Loaded Old Worsley(labeled) Mask Generation"
    set TestNewMaskId [b2_threshold_image $newWorsleyId 15.0]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - labeled image Mask Volume"
    set result [b2_measure_volume_mask $TestNewMaskId]
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - 1594.016532) / 1594.016532 < 0.0001} ] $ModuleName $SubTestDes

    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $newWorsleyId

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Load Old Worsley Image"
    set oldWorsleyId [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/WORS_ANON00022_HF_18_IMF_1.raw data-type= float-single]
    ReportTestStatus $LogFile  [ expr {$oldWorsleyId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Old Mask Generation"
    set TestNewMaskId [b2_threshold_image $oldWorsleyId 5000]
    ReportTestStatus $LogFile  [ expr {$TestNewMaskId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Difference Old-Mask Generation"
    set diffMask [ b2_xor_masks $TestNewMaskId $TestMaskId]
    ReportTestStatus $LogFile  [ expr {$diffMask != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "OLD FLOW B2 RESAMPLE Worsley-PET test - Difference Old-Mask Volume"
    set result [b2_measure_volume_mask $diffMask]
    ReportTestStatus $LogFile  [ expr {[lindex [lindex $result 0] 1] < 86.000000} ] $ModuleName $SubTestDes


    b2_destroy_mask $diffMask
    b2_destroy_mask $TestNewMaskId
    b2_destroy_image $oldWorsleyId

    b2_destroy_mask $TestMaskId
    b2_destroy_image $TestImageId


    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "NEW COUNT B2 RESAMPLE Worsley-PET test"
    set TestImageId [b2 resample worsley-pet $PetCountImageId $TalairachId brains-interp= True scale= False]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "NEW COUNT B2 RESAMPLE Worsley-MR test - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 500]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
#    set expect 1336.234851
    set expect 1285.017486
    set fraction [ expr {abs([lindex [lindex $result 0] 1] - $expect) / $expect} ]
    set SubTestDes "NEW COUNT B2 RESAMPLE IMAGE test - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == $fraction"
    ReportTestStatus $LogFile  [ expr {$fraction < 0.0001} ] $ModuleName $SubTestDes
    b2_destroy_mask $TestMaskId
    b2_destroy_image $TestImageId

    ############################ BRAINS2 Resample Worsley-MR ########################################
    set SubTestDes "NEW FLOW B2 RESAMPLE Worsley-PET test"
    set TestImageId [b2 resample worsley-pet $PetFlowImageId $TalairachId brains-interp= False scale= False filter-size= 5.0]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "NEW FLOW B2 RESAMPLE Worsley-MR test - Mask Generation"
    set TestMaskId [b2_threshold_image $TestImageId 35]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set result [b2_measure_volume_mask $TestMaskId]
#    set expect 977.951612
    set expect 940.467105
    set SubTestDes "NEW FLOW B2 RESAMPLE IMAGE test - Image Measurement -- Does [lindex [lindex $result 0] 1] nearly equal $expect ?  Fraction == $fraction"
    ReportTestStatus $LogFile  [ expr {$fraction < 0.0001} ] $ModuleName $SubTestDes
    b2_destroy_mask $TestMaskId
    b2_destroy_image $TestImageId



    b2_destroy_image $PetFlowImageId
    b2_destroy_image $PetCountImageId
    b2_destroy_transform $PetFlowXfrmId
    b2_destroy_transform $PetCountXfrmId
    b2_destroy_talairach-parameters $TalairachId

    StopModule $LogFile $ModuleName

    return $MODULE_SUCCESS
}
