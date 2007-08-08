# \author    Hans J. Johnson"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the saving of various Tissue Class file formats
# \fn        proc saveTissueClass {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS2 Tissue-Class Model
#
# To Do
#------------------------------------------------------------------------
# Test for comparison of resulting file
#



proc saveTissueClass {pathToRegressionDir dateString} {

    set ModuleName "saveTissueClass"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save tissue-class command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Tissue-Class - Load Tissue-Class test"
    set TestTissueID [b2 load tissue-class $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/STD_20_Tissue_Class.mdl]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTissueID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/20_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguments
    set SubTestDes "required argument test (1)"
    set errorTest [b2 save tissue-class]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (2)"
    set errorTest [b2 save tissue-class ${OUTPUT_DIR}/TEST/20_ACPC/ANON0006_20_junk.mdl]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required argument test (3)"
    set errorTest [b2 save tissue-class ${OUTPUT_DIR}/TEST/20_ACPC/ANON0006_20_junk.mdl brains2]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2 save tissue-class ${OUTPUT_DIR}/TEST/20_ACPC/ANON0006_20_junk.mdl brains2 $TestTissueID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2 save tissue-class ${OUTPUT_DIR}/TEST/20_ACPC/ANON0006_20_junk.mdl brains2 $TestTissueID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Tissue Class ###########################################
    set SubTestDes "Save BRAINS2 Tissue-Class test"
    set errorTest [b2 save tissue-class ${OUTPUT_DIR}/ANON0006_20_Tissue_Class.mdl brains2 $TestTissueID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
        set SubTestDes "Save BRAINS2 Tissue-Class - Load Model test"
        set SaveTissueID [b2 load tissue-class ${OUTPUT_DIR}/ANON0006_20_Tissue_Class.mdl]
        ReportTestStatus $LogFile  [ expr {$SaveTissueID != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Save BRAINS2 Tissue-Class - Load T1 test"
        set T1ImageID [b2 load image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T1.hdr]
        ReportTestStatus $LogFile  [ expr {$T1ImageID != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Save BRAINS2 Tissue-Class - Load T2 test"
        set T2ImageID [b2 load image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_T2.hdr]
        ReportTestStatus $LogFile  [ expr {$T2ImageID != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Save BRAINS2 Tissue-Class - Load PD test"
        set PDImageID [b2 load image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_20_PD.hdr]
        ReportTestStatus $LogFile  [ expr {$PDImageID != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Save BRAINS2 Tissue-Class - Load Class Image test"
        set ClassImageID [b2 load image $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/STD_20_segment.hdr]
        ReportTestStatus $LogFile  [ expr {$ClassImageID != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Save BRAINS2 Tissue-Class - Load Brain Mask test"
        set brainMaskID [b2 load mask $pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC/ANON0006_brain_cut.mask]
        ReportTestStatus $LogFile  [ expr {$brainMaskID != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Save BRAINS2 Tissue-Class - Classify test"
        set newImageId [b2 apply class-model {T1 T2 PD} [list $T1ImageID $T2ImageID $PDImageID] $SaveTissueID cth= 197]
        ReportTestStatus $LogFile  [ expr {$newImageId != -1 } ] $ModuleName $SubTestDes

        set expectClassVolume [ b2 measure class-volume mask $brainMaskID $ClassImageID]
        set newClassVolume [ b2 measure class-volume mask $brainMaskID $newImageId]

        for {set i 0} {$i < 5} {incr i} {
            set SubTestDes "Save BRAINS2 Tissue-Class - Measurement Difference [lindex $expectClassVolume $i] == [lindex $newClassVolume $i] test"
            ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $expectClassVolume $i] 1] - [lindex [lindex $newClassVolume $i] 1]) < 1.0 } ] $ModuleName $SubTestDes
        }

        set errorTest [b2 save tissue-class /invalid_directory_nametmp/SGI/MR/B2-downsampled/TEST/20_ACPC/junk.mdl brains2 $TestTissueID]
        set SubTestDes "BRAINS2 invalid filename test; return: '$errorTest'"
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set errorTest [b2 save tissue-class ${OUTPUT_DIR}/ANON0006_20_Tissue_Class.mdl brains2 $TestTissueID filter-suffix= -invalid]
        set SubTestDes "BRAINS2 invalid filter-suffix test; return: '$errorTest'"
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $T1ImageID ] != -1 } ] $ModuleName "Destroying image $T1ImageID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $T2ImageID ] != -1 } ] $ModuleName "Destroying image $T2ImageID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $PDImageID ] != -1 } ] $ModuleName "Destroying image $PDImageID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $ClassImageID ] != -1 } ] $ModuleName "Destroying image $ClassImageID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $brainMaskID ] != -1 } ] $ModuleName "Destroying mask $brainMaskID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy tissue-class $SaveTissueID ] != -1 } ] $ModuleName "Destroying tissue-class $SaveTissueID"
        ReportTestStatus $LogFile  [ expr { [ b2 destroy image $newImageId ] != -1 } ] $ModuleName "Destroying image $newImageId"

    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy tissue-class $TestTissueID ] != -1 } ] $ModuleName "Destroying tissue-class $TestTissueID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

