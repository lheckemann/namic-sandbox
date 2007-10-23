proc coreSaveTransformTest {filterName SaveFileName TestXfrmId TestMaskId TestImageId Mean Std LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "$filterName Save Transform - Load Transform test"
    set SaveXfrmID [b2_load_transform $SaveFileName]
    if { [ ReportTestStatus $LogFile  [ expr {$SaveXfrmID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }
    b2_set_transform $SaveXfrmID image $TestImageId

    set result [b2_measure_image_mask $TestMaskId $TestImageId]
    puts $result
    set SubTestDes "$filterName saved transform Mean test -- Does [lindex [lindex $result 0] 1] nearly equal $Mean by fraction [ expr abs([lindex [lindex $result 0] 1] - $Mean) / $Mean ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $Mean) / $Mean <  0.00025} ] $ModuleName $SubTestDes

    set SubTestDes "$filterName saved transform Std test -- Does [lindex [lindex $result 3] 1] nearly equal $Std by fraction [ expr abs([lindex [lindex $result 3] 1] - $Std) / $Std ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 3] 1] - $Std) / $Std <  0.00025} ] $ModuleName $SubTestDes

    set origStandardDims [b2_get_standard-dims_transform $TestXfrmId]
    set newStandardDims [b2_get_standard-dims_transform $SaveXfrmID]
    set origResliceDims [b2_get_reslice-dims_transform $TestXfrmId]
    set newResliceDims [b2_get_reslice-dims_transform $SaveXfrmID]

    set origStandardRes [b2_get_standard-res_transform $TestXfrmId]
    set newStandardRes [b2_get_standard-res_transform $SaveXfrmID]
    set origResliceRes [b2_get_reslice-res_transform $TestXfrmId]
    set newResliceRes [b2_get_reslice-res_transform $SaveXfrmID]

    for {set i 0} {$i < 3} {incr i} {
        set SubTestDes "$filterName Transform Standard Dims Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newStandardDims $i] == [lindex $origStandardDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "$filterName Transform Reslice Dims Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newResliceDims $i] == [lindex $origResliceDims $i] } ] $ModuleName $SubTestDes
        set SubTestDes "$filterName Transform Standard Res Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newStandardRes $i] == [lindex $origStandardRes $i] } ] $ModuleName $SubTestDes
        set SubTestDes "$filterName Transform Reslice Res Index $i"
        ReportTestStatus $LogFile  [ expr {[lindex $newResliceRes $i] == [lindex $origResliceRes $i] } ] $ModuleName $SubTestDes
    }

    set SubTestDes "$filterName Original Transform Type Test"
    set origType [ b2_get_transform_type $TestXfrmId]
    ReportTestStatus $LogFile  [ expr {$origType != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Saved Transform Type Test"
    set newType [ b2_get_transform_type $SaveXfrmID]
    ReportTestStatus $LogFile  [ expr {$newType != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "$filterName Transform Type Test"
    ReportTestStatus $LogFile  [ expr {$origType == $newType } ] $ModuleName $SubTestDes

    b2_set_transform  -1 image $TestImageId
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $SaveXfrmID ] != -1 } ] $ModuleName "Destroying transform $SaveXfrmID"

    return $MODULE_SUCCESS

}


# \author    Hans J. Johnson"
# \date        $Date: 2007-05-25 12:31:38 -0500 (Fri, 25 May 2007) $
# \brief    This module tests the saving of various transform file formats
# \fn        proc saveTransform {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS2 Transform
# Save an AIR Transform
# Save an AIR Intialization Transform
#
# To Do
#------------------------------------------------------------------------
# Add Bookstein, and Interleave fit transforms to tests
#



proc saveTransform {pathToRegressionDir dateString} {

    set ModuleName "saveTransform"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_save_transform command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#    close $LogFile
#    set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Transform - Load Transform test"
    set TestXfrmID [b2_load_transform $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_T1.xfrm]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Transform - Load Image test"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.002]
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Transform - Load Mask test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }


    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2_save_transform]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (2)"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (3)"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm brains2]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm brains2 $TestXfrmID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm brains2 $TestXfrmID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Transform ###########################################
    set SubTestDes "Save BRAINS2 Transform test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm  brains2 $TestXfrmID]

#set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm  brains2 [b2_load_transform $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_parameters]]

    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        coreSaveTransformTest "BRAINS2-TransformTest" ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm $TestXfrmID $TestMaskID $TestImageID 57.851422941039189 18.961051162661676 $LogFile $ModuleName

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2_save_transform /invalid_dir_name/TEST/10_ACPC/junk.xfrm brains2 $TestXfrmID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.xfrm brains2 $TestXfrmID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }



    ############################### AIR Transform ###########################################
    set SubTestDes "Save AIR Transform test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.air air $TestXfrmID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        coreSaveTransformTest "BRAINS2-SaveAIR" ${OUTPUT_DIR}/TEST/10_ACPC/junk.air $TestXfrmID $TestMaskID $TestImageID 57.851422941039189 18.961051162661676 $LogFile $ModuleName

        set SubTestDes "AIR invalid filename test"
        set errorTest [b2_save_transform /invalid_dir_name/TEST/10_ACPC/junk.air air $TestXfrmID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "AIR invalid filter-suffix test"
        set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.air air $TestXfrmID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }



    ############################### AIR-Init Transform ###########################################
    set SubTestDes "Save AIR-INIT Transform test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.init  initAir $TestXfrmID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
# Do we want to test the resulting transform???????

        set SubTestDes "AIR-INIT invalid filename test";
        set errorTest [b2_save_transform /invalid_dir_name/TEST/10_ACPC/junk.init initAir $TestXfrmID];
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes;

        set SubTestDes "AIR-INIT invalid filter-suffix test";
        set errorTest [b2_save_transform ${OUTPUT_DIR}/TEST/10_ACPC/junk.init initAir $TestXfrmID filter-suffix= -invalid];
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes;
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmID ] != -1 } ] $ModuleName "Destroying transform $TestXfrmID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"

    ############################ Interleave Transform ############################################
    set SubTestDes "Save Interleave Transform - Load Transform test"
    set TestXfrmID [b2_load_transform $pathToRegressionDir/SGI/MR/B2-InterleaveFit/TEST/T2_003/T2_interleave.xfrm]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Interleave Transform - Load Image test"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/B2-InterleaveFit/TEST/T2_003/006839.003.009]
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Interleave Transform - Load Mask test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/B2-InterleaveFit/TEST/T2_003/ANON0018_brain_trim.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Interleave Transform test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/junk.itl  brains2 $TestXfrmID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        coreSaveTransformTest "BRAINS2-Interleave" ${OUTPUT_DIR}/junk.itl $TestXfrmID $TestMaskID $TestImageID 308.476045267094889 165.190663862634523 $LogFile $ModuleName
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmID ] != -1 } ] $ModuleName "Destroying transform $TestXfrmID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"
    ############################ Bookstein Transform ############################################
    set SubTestDes "Save Bookstein Transform - Load Transform test"
    set TestXfrmID [b2_load_transform $pathToRegressionDir/SGI/MR/B2-Classify/TEST/10_ACPC/bookstein.wrp]
    if { [ ReportTestStatus $LogFile  [ expr {$TestXfrmID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Bookstein Transform - Load Image test"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/B2-Classify/TEST/10_ACPC/ANON0006_10_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Save Bookstein Transform - Load Mask test"
    set TestMaskID [b2_load_mask $pathToRegressionDir/SGI/MR/B2-Classify/TEST/10_ACPC/ANON0006_brain_cut.mask]
    if { [ ReportTestStatus $LogFile  [ expr {$TestMaskID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }
    set SubTestDes "Save BOOKSTEIN Transform test"
    set errorTest [b2_save_transform ${OUTPUT_DIR}/junk.wrp  brains2 $TestXfrmID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        coreSaveTransformTest "BRAINS2-Bookstein" ${OUTPUT_DIR}/junk.wrp $TestXfrmID $TestMaskID $TestImageID 51.767158565166049 47.912033275490387 $LogFile $ModuleName
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform $TestXfrmID ] != -1 } ] $ModuleName "Destroying transform $TestXfrmID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID"

    return [ StopModule  $LogFile $ModuleName ]
}

