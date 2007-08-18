# \author    Greg Harris"
# \date
# \brief   
# \fn        proc b2alignlinearRegistration {pathToRegressionDir dateString {GT Test}}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString        - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


#source DateStamp.tcl
#source ModuleUtils.tcl
#source ReportTestStatus.tcl

proc b2alignlinearRegistration {pathToRegressionDir dateString {GT Test}} {

    set ModuleName "b2alignlinearRegistration"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of brains2_fit.csh/b2alignlinear combination"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr
    global OUTPUT_DIR


    # Run Tests

    set im1 [b2_load_image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/31_003/148313.003.002" data-type= signed-16bit]
    set SubTestDes "required image loaded test: ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/31_003/148313.003.002"
    if {[ReportTestStatus $LogFile  [ expr {$im1 != -1 } ] $ModuleName $SubTestDes] == 0} {
      return $MODULE_FAILURE
    }

    
    set commandText "exec [file dirname [info nameofexecutable]]/brains2_fit.csh harris MR5 ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_T1_strict.hdr ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/31_003/148313.003.002 ${OUTPUT_DIR}/b2alignlinear.test.air16 -m 6 -f ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/T2_ACPC.init -k 20 20 64 25 80 80 -p1 256 -p2 256 -t1 30 -t2 100 -s 81 1 3 -c 0.000010 -r 50 -h 5 -v -z"
    puts "exec shell command is: \n$commandText\n" 
    set SubTestDes "execute the command $commandText"
    if {[ReportTestStatus $LogFile  [catch { eval $commandText } typescript ] $ModuleName $SubTestDes] == 0} {
        puts "exec failed, here is its typescript: \n$typescript\n" 
        return $MODULE_FAILURE
    } else {
        puts "exec succeeded, here is its typescript: \n$typescript\n" 
    }


    set num_ret [b2_load_transform "${OUTPUT_DIR}/b2alignlinear.test.air16"]
    set SubTestDes "Did the file ${OUTPUT_DIR}/b2alignlinear.test.air16 load as a transform?"
    if {[ReportTestStatus $LogFile  [ expr {$num_ret != -1 } ] $ModuleName $SubTestDes] == 0} {
    }


    if {$num_ret != -1} {
    b2_set_transform ${num_ret} image ${im1}
        set im2 [b2_load_image "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/31_003/148313.003.002" data-type= signed-16bit]
        set xfm [b2_load_transform "${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/b2alignlinear.reference.air16"]
    b2_set_transform ${xfm} image ${im2}
    set im3 [b2_subtract_images [list ${im1} ${im2}]]
    set errMask [b2_threshold_image ${im3} 1 absolute-value= True ]
    set volTab [b2_measure_volume_mask ${errMask}]

    set errorVolume [ expr {[lindex [lindex ${volTab} 0] 1] } ];
    set SubTestDes "difference error volume:  $errorVolume  <= 0.05 "
    if {[ReportTestStatus $LogFile  [ expr { $errorVolume  <= 0.05 } ] $ModuleName $SubTestDes] == 0} {
        }

    # Free memory
     ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${im2} ] != -1 } ] $ModuleName "Destroying image ${im2}"
     ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform ${xfm} ] != -1 } ] $ModuleName "Destroying transform ${xfm}"
     ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${im3} ] != -1 } ] $ModuleName "Destroying image ${im3}"
     ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask ${errMask} ] != -1 } ] $ModuleName "Destroying mask ${errMask}"
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image ${im1} ] != -1 } ] $ModuleName "Destroying image ${im1}"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform ${num_ret} ] != -1 } ] $ModuleName "Destroying transform ${num_ret}"

    return [ StopModule  $LogFile $ModuleName ]
}

