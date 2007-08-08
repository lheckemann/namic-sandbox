# \author        Greg Harris
# \date
# \brief        Testing of runMutualRegistration
# \fn                proc mutualInformationRegistration {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 erode surface mask tested simply.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc compareScaledImages {testImage standardImage minY testSeries LogFile ModuleName} {
    set EveryVoxelMask [b2 threshold image $standardImage 0 absolute-value= true]
    if {$minY != 0} {
        set UpperVoxelMask [b2 split mask $EveryVoxelMask y $minY +]
        b2 destroy mask $EveryVoxelMask
    } else {
        set UpperVoxelMask $EveryVoxelMask
    }
    set standard_meas [b2 measure image mask $UpperVoxelMask $standardImage]
    set test_meas [b2 measure image mask $UpperVoxelMask $testImage]
    set standard_mean [lindex [lindex $standard_meas 0] 1]
    set test_mean [lindex [lindex $test_meas 0] 1]
    set standard_normal [b2 multiply images $standardImage scale= [expr 100.0 / $standard_mean]]
    set test_normal [b2 multiply images $testImage scale= [expr 100.0 / $test_mean]]
puts "Measures for ${testSeries} ${test_normal}"
    set normal_diff [b2 subtract images [list $test_normal $standard_normal]]
    set normal_meas [b2 measure image mask $UpperVoxelMask $normal_diff]
    set normal_mean [lindex [lindex $normal_meas 0] 1]
    set normal_stddev [lindex [lindex $normal_meas 3] 1]

    set SubTestDes "${testSeries} masked image difference mean close enough to zero test"
    if {[ReportTestStatus $LogFile  [ expr { abs(${normal_mean}) < 0.5 } ] $ModuleName $SubTestDes] == 0} {
}
    set SubTestDes "${testSeries} masked image difference stddev close enough to zero test"
    if {[ReportTestStatus $LogFile  [ expr { ${normal_stddev} < 0.5 } ] $ModuleName $SubTestDes] == 0} {
}

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${standard_normal} ] != -1 } ] $ModuleName "Destroying 'standard_normal' image ${standard_normal} in compareScaledImages"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${test_normal} ] != -1 } ] $ModuleName "Destroying 'test_normal' image ${test_normal} in compareScaledImages"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${normal_diff} ] != -1 } ] $ModuleName "Destroying 'normal_diff' image ${normal_diff} in compareScaledImages"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${UpperVoxelMask} ] != -1 } ] $ModuleName "Destroying 'UpperVoxelMask' mask ${UpperVoxelMask} in compareScaledImages"

}





proc mutualInformationRegistration {pathToRegressionDir dateString} {
    set ModuleName "mutualInformationRegistration"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of runMutualRegistration"
    global OUTPUT_DIR
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr




# Run Tests

# set acpcDir [getACPCdir [pwd]]
# set OUTPUT_DIR /scratch/harris/regressiontest/SGI/MR/B2-downsampled/TEST/20_ACPC/harris
    set acpcDir [getACPCdir "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC"]

    set inRadians [expr 3.14159 / 180]
    set twist [b2 create affine-transform {100 100 100} {2.0 2.0 2.0} {100 100 100} {2.0 2.0 2.0} ry= [expr 5 * $inRadians] rz= [expr 10 * $inRadians] rx= [expr -10 * $inRadians] dx= -7.5 dy= 5 dz= 2.1]
    set twist_inverse [b2 invert affine-transform $twist]
    set twist_inverse_filename "${OUTPUT_DIR}/twist_inverse_itself.xfrm"
    b2 save transform ${twist_inverse_filename} air ${twist_inverse}
    
    set regular [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= unsigned-8bit]
    b2 set transform $twist image $regular
    set twisted_filename "${OUTPUT_DIR}/ANON0006_20_T1_twisted.nii.gz"
    b2 save image ${twisted_filename} nifti $regular data-type= unsigned-8bit

    set regularMask_filename ${acpcDir}/ANON0006_brain_cut.mask
    set regularMask [b2 load mask ${regularMask_filename}]
    set twistedMask [b2 resample mask forward ${regularMask} ${twist}]
    set twistedMask_filename "${OUTPUT_DIR}/ANON0006_20_T1_twisted.mask"
    b2 save mask ${twistedMask_filename} brains2 ${twistedMask}
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${twistedMask} ] != -1 } ] $ModuleName "Destroying 'twistedMask' mask ${twistedMask}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${regularMask} ] != -1 } ] $ModuleName "Destroying 'regularMask' mask ${regularMask}"


    set BootstrapMultiplicity 0
    set WriteResliceImage 0
    set resultTransformFile ${OUTPUT_DIR}/ANON0006_20_T1_twisted.mutual.xfrm
    catch { exec rm $resultTransformFile } squabble

    runMutualRegistration ${twisted_filename} ${acpcDir}/ANON0006_20_T1.hdr . ${resultTransformFile} ${OUTPUT_DIR}/TEST_deleteThisReslice.hdr . "900" "0.5"  1000 0 0 $WriteResliceImage 1 $BootstrapMultiplicity ${OUTPUT_DIR} ${OUTPUT_DIR}/TEST_MutuRegistr.csh ACPC  "-s 50000 -mm ${regularMask_filename} -fm ${twistedMask_filename}"

    set SubTestDes "runMutualRegistration expected to produce the file ${resultTransformFile}"
    if {[ReportTestStatus $LogFile  [ expr { [file exists ${resultTransformFile}] } ] $ModuleName $SubTestDes]} {

        set twist_MI [b2 load transform ${resultTransformFile}]

        set standard [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= float-single]
        set reslice [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= float-single]
        b2 set transform $twist image $standard
        b2 set transform $twist_MI image $reslice

        set diff [b2 subtract images [list $standard $reslice]]
        set sumsq [RootSumSquared $diff 42]
        set guideline 10.0
        set SubTestDes "MI_Fitted T1 to twisted T1: Comparing SumSquaredError measurement, does new ($sumsq) fall within guideline ($guideline)"
        ReportTestStatus $LogFile  [ expr { $sumsq <=  $guideline } ] $ModuleName $SubTestDes

    
#return $MODULE_SUCCESS



        b2 set transform -1 image $reslice
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${diff} ] != -1 } ] $ModuleName "Destroying 'diff' image ${diff}"
        set diff [b2 subtract images [list $standard $reslice]]
        set sumsq [RootSumSquared $diff 42]
        set guideline 1000.0
        set SubTestDes "T1 versus twisted T1: Comparing SumSquaredError measurement, does new ($sumsq) fall outside guideline ($guideline)"
        ReportTestStatus $LogFile  [ expr { $sumsq >  $guideline } ] $ModuleName $SubTestDes


        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist_MI} ] != -1 } ] $ModuleName "Destroying 'twist_MI' transform ${twist_MI}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${regular} ] != -1 } ] $ModuleName "Destroying 'regular' image ${regular}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${diff} ] != -1 } ] $ModuleName "Destroying 'diff' image ${diff}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${reslice} ] != -1 } ] $ModuleName "Destroying 'reslice' image ${reslice}"
    }

if {1 == 1} {

    set regularAC {49.0  50.0  55.0}
    set twistedAC [b2 transform point ${twist} 1 49 50 55]   
    #  -xfm 3 -fo ${twistedAC} -mo ${regularAC} 

    set BootstrapMultiplicity 0
    set WriteResliceImage 0
    set resultTransformFile ${OUTPUT_DIR}/ANON0006_20_T1_twisted_r.mutual.xfrm
    catch { exec rm $resultTransformFile } squabble

    runMutualRegistration ${twisted_filename} ${acpcDir}/ANON0006_20_T1.hdr . ${resultTransformFile} ${OUTPUT_DIR}/TEST_deleteThisReslice.hdr . "900" "0.5"  1000 0 0 $WriteResliceImage 1 $BootstrapMultiplicity ${OUTPUT_DIR} ${OUTPUT_DIR}/TEST_MutuRegistr.csh ACPC  "-s 50000 -xfm 4"

    set SubTestDes "runMutualRegistration expected to produce the file ${resultTransformFile}"
    if {[ReportTestStatus $LogFile  [ expr { [file exists ${resultTransformFile}] } ] $ModuleName $SubTestDes]} {

        set twist_MI [b2 load transform ${resultTransformFile}]

        b2 set transform -1 image $standard
        set reslice [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= float-single]
        b2 set transform $twist image $standard
        b2 set transform $twist_MI image $reslice

        set diff [b2 subtract images [list $standard $reslice]]
        set sumsq [RootSumSquared $diff 42]
        set guideline 10.0
        set SubTestDes "MI_Fitted T1 to twisted T1: Comparing SumSquaredError measurement, does new ($sumsq) fall within guideline ($guideline)"
        ReportTestStatus $LogFile  [ expr { $sumsq <=  $guideline } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist_MI} ] != -1 } ] $ModuleName "Destroying 'twist_MI' transform ${twist_MI}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${reslice} ] != -1 } ] $ModuleName "Destroying 'reslice' image ${reslice}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${diff} ] != -1 } ] $ModuleName "Destroying 'diff' image ${diff}"

    }
}



#return $MODULE_SUCCESS


    

    set BootstrapMultiplicity 0
    set WriteResliceImage 1
    set ResliceImage ${OUTPUT_DIR}/Clipped-Reslice-final-mutual.nii.gz
                   # generated name from runMutualRegistration.
    catch { exec rm ${OUTPUT_DIR}/ANON0006_20_T1_detwisted_Reslice.hdr } squabble
    catch { exec rm ${OUTPUT_DIR}/ANON0006_20_T1_detwisted_Reslice.img.gz } squabble
    set resultDeTransformFile ${OUTPUT_DIR}/ANON0006_20_T1_detwisted.mutual.xfrm
    catch { exec rm $resultDeTransformFile } squabble


    runMutualRegistration ${acpcDir}/ANON0006_20_T1.hdr ${twisted_filename} . ${resultDeTransformFile}  ${OUTPUT_DIR}/TEST_deleteThisReslice.hdr YES "900" "0.5"  1000 0 0 $WriteResliceImage 1 $BootstrapMultiplicity ${OUTPUT_DIR} ${OUTPUT_DIR}/TEST_MutuRegistr.csh ACPC "-s 50000 -median 1" 


    set SubTestDes "runMutualRegistration expected to produce the file ${ResliceImage}"
    ReportTestStatus $LogFile  [ expr { [file exists ${ResliceImage}] } ] $ModuleName $SubTestDes

    set SubTestDes "runMutualRegistration expected to produce the file ${resultDeTransformFile}"
    if {[ReportTestStatus $LogFile  [ expr { [file exists ${resultDeTransformFile}] } ] $ModuleName $SubTestDes]} {
    
        set reslice [b2 load image ${twisted_filename} data-type= float-single]
        set detwist_MI [b2 load transform ${resultDeTransformFile}]
        b2 set transform -1 image $standard
        b2 set transform $detwist_MI image $reslice

#        compareScaledImages $reslice $standard 42 "Fit twisted T1 to T1" $LogFile $ModuleName

        set diff [b2 subtract images [list $standard $reslice]]
        set sumsq [RootSumSquared $diff 42]
        set guideline 525.0
        set SubTestDes "Fit twisted T1 to T1: Comparing SumSquaredError measurement, does new ($sumsq) fall within guideline ($guideline)"
        ReportTestStatus $LogFile  [ expr { $sumsq <=  $guideline } ] $ModuleName $SubTestDes


#return $MODULE_SUCCESS



        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist} ] != -1 } ] $ModuleName "Destroying 'twist' transform ${twist}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist_inverse} ] != -1 } ] $ModuleName "Destroying 'twist_inverse' transform ${twist_inverse}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${detwist_MI} ] != -1 } ] $ModuleName "Destroying 'detwist_MI' transform ${detwist_MI}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${reslice} ] != -1 } ] $ModuleName "Destroying 'reslice' image ${reslice}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${standard} ] != -1 } ] $ModuleName "Destroying 'standard' image ${standard}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${diff} ] != -1 } ] $ModuleName "Destroying 'diff' image ${diff}"
    }




# set acpcDir [getACPCdir [pwd]]
# set OUTPUT_DIR /scratch/harris/regressiontest/SGI/MR/B2-downsampled/TEST/20_ACPC/harris
    set acpcDir [getACPCdir "${pathToRegressionDir}/SGI/MR/B2-downsampled/TEST/20_ACPC"]

    set inRadians [expr 3.14159 / 180]
    set twist [b2 create affine-transform {100 100 100} {1.9 1.9 1.9} {100 100 100} {2.0 2.0 2.0} ry= [expr 5 * $inRadians] rz= [expr 10 * $inRadians] rx= [expr -10 * $inRadians] dx= -7.5 dy= 5 dz= 2.1]
    set twist_inverse [b2 invert affine-transform $twist]
    set twist_inverse_filename "${OUTPUT_DIR}/twist_inverse_itself.xfrm"
    b2 save transform ${twist_inverse_filename} air ${twist_inverse}
    
    set regular [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= unsigned-8bit]
    b2 set transform $twist image $regular
    set twisted_filename "${OUTPUT_DIR}/ANON0006_20_T1_twisted.nii"
    b2 save image ${twisted_filename} nifti $regular data-type= unsigned-8bit
    catch { exec nifti_tool -mod_hdr -overwrite -infiles ${twisted_filename} -mod_field pixdim {0.0 2.0 2.0 2.0 0.0 0.0 0.0 0.0} } squabble



    set BootstrapMultiplicity 0
    set WriteResliceImage 0
    set result1TransformFile ${OUTPUT_DIR}/ANON0006_20_T1_twisted.mutual_1.xfrm
    set result1sTransformFile ${OUTPUT_DIR}/ANON0006_20_T1_twisted.mutual_1stripped.xfrm
    catch { exec rm $result1TransformFile } squabble

    runMutualRegistration ${twisted_filename} ${acpcDir}/ANON0006_20_T1.hdr . ${result1TransformFile} ${OUTPUT_DIR}/TEST_deleteThisReslice.hdr . "900" "0.5"  1000 0 0 $WriteResliceImage 1 $BootstrapMultiplicity ${OUTPUT_DIR} ${OUTPUT_DIR}/TEST_MutuRegistr.csh ACPC "-s 50000  -min 0.00001 -xfm 1 -no ${result1sTransformFile}"

    set SubTestDes "runMutualRegistration expected to produce the file ${result1sTransformFile}"
    ReportTestStatus $LogFile  [ expr { [file exists ${result1sTransformFile}] } ] $ModuleName $SubTestDes

    set SubTestDes "runMutualRegistration expected to produce the file ${result1TransformFile}"
    if {[ReportTestStatus $LogFile  [ expr { [file exists ${result1TransformFile}] } ] $ModuleName $SubTestDes]} {

        set twist_MI_1 [b2 load transform ${result1TransformFile}]

        set standard [b2 load image ${twisted_filename} data-type= float-single]
        set reslice_1 [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= float-single]
        b2 set transform $twist_MI_1 image $reslice_1

        set diff [b2 subtract images [list $standard $reslice_1]]
        set sumsq [RootSumSquared $diff 42]
        set guideline 50.0
        set SubTestDes "MI_Fitted T1 to twisted T1: Comparing SumSquaredError measurement, does new ($sumsq) fall within guideline ($guideline)"
        ReportTestStatus $LogFile  [ expr { $sumsq <=  $guideline } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${standard} ] != -1 } ] $ModuleName "Destroying 'standard' image ${standard}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${diff} ] != -1 } ] $ModuleName "Destroying 'diff' image ${diff}"
    }
    
    

    set BootstrapMultiplicity 0
    set WriteResliceImage 0
    set result2TransformFile ${OUTPUT_DIR}/ANON0006_20_T1_twisted.mutual_2.xfrm
    set result2sTransformFile ${OUTPUT_DIR}/ANON0006_20_T1_twisted.mutual_2stripped.xfrm
    catch { exec rm $result2TransformFile } squabble

    runMutualRegistration ${twisted_filename} ${acpcDir}/ANON0006_20_T1.hdr ${result1sTransformFile} ${result2TransformFile} ${OUTPUT_DIR}/TEST_deleteThisReslice.hdr . "900" "0.5"  1000 0 0 $WriteResliceImage 1 $BootstrapMultiplicity ${OUTPUT_DIR} ${OUTPUT_DIR}/TEST_MutuRegistr.csh ACPC "-s 50000 -min 0.00001 -xfm 2 -no ${result2sTransformFile}"

    set SubTestDes "runMutualRegistration expected to produce the file ${result2sTransformFile}"
    ReportTestStatus $LogFile  [ expr { [file exists ${result2sTransformFile}] } ] $ModuleName $SubTestDes

    set SubTestDes "runMutualRegistration expected to produce the file ${result2TransformFile}"
    if {[ReportTestStatus $LogFile  [ expr { [file exists ${result2TransformFile}] } ] $ModuleName $SubTestDes]} {

        set twist_MI_2 [b2 load transform ${result2TransformFile}]

        set standard [b2 load image ${twisted_filename} data-type= float-single]
        set reslice_2 [b2 load image ${acpcDir}/ANON0006_20_T1.hdr data-type= float-single]
        b2 set transform $twist_MI_2 image $reslice_2

        set diff [b2 subtract images [list $standard $reslice_2]]
        set sumsq [RootSumSquared $diff 42]
        set guideline 60.0
        set SubTestDes "MI_Fitted T1 to twisted T1: Comparing SumSquaredError measurement, does new ($sumsq) fall within guideline ($guideline)"
        ReportTestStatus $LogFile  [ expr { $sumsq <=  $guideline } ] $ModuleName $SubTestDes



#return $MODULE_SUCCESS




        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist} ] != -1 } ] $ModuleName "Destroying 'twist' transform ${twist}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist_inverse} ] != -1 } ] $ModuleName "Destroying 'twist_inverse' transform ${twist_inverse}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist_MI_1} ] != -1 } ] $ModuleName "Destroying 'twist_MI_1' transform ${twist_MI_1}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${twist_MI_2} ] != -1 } ] $ModuleName "Destroying 'twist_MI_2' transform ${twist_MI_2}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${regular} ] != -1 } ] $ModuleName "Destroying 'regular' image ${regular}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${standard} ] != -1 } ] $ModuleName "Destroying 'standard' image ${standard}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${diff} ] != -1 } ] $ModuleName "Destroying 'diff' image ${diff}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${reslice_1} ] != -1 } ] $ModuleName "Destroying 'reslice_1' image ${reslice_1}"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${reslice_2} ] != -1 } ] $ModuleName "Destroying 'reslice_2' image ${reslice_2}"
    }


    return [ StopModule  $LogFile $ModuleName ]
}

