proc CoreSaveHistogramTest {filterName TestHistID SaveFileName LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "$filterName Save Histogram - Load saved Histogram"
    set SaveHistID [b2 load histogram $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveHistID != -1 } ] $ModuleName $SubTestDes


    set origHistData [b2 get histogram data $TestHistID ]
    set newHistData [b2 get histogram data $SaveHistID]

    set SubTestDes "$filterName Save Histogram - Histogram Size Test"
    ReportTestStatus $LogFile  [ expr {[llength $origHistData] == [llength $newHistData] } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $origHistData]} {incr i} {
        for {set j 0} {$j < 3} {incr j} {
            set SubTestDes "$filterName Histogram Bin Comparison ($i,$j)"
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $origHistData $i] $j] == [lindex [lindex $newHistData $i] $j] } ] $ModuleName $SubTestDes
        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy histogram $SaveHistID ] != -1 } ] $ModuleName "Destroying histogram $SaveHistID"

    return $MODULE_SUCCESS
}


# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various histogram file formats
# \fn        proc saveHistogram {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS Histogram
# Save a BRAINS2 Histogram
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveHistogram {pathToRegressionDir dateString} {


    set ModuleName "saveHistogram"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save histogram command and saving various file formats"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set SubTestDes "Save Histogram - Load Histogram test"
    set TestHistID [b2 load histogram $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/cran_histogram]
    if { [ ReportTestStatus $LogFile  [ expr {$TestHistID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC/user } squabble] != 0 } {puts "exec failed: $squabble" }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 save histogram]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst brains2]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst briains2 $TestHistID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst briains2 $TestHistID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Histogram ###########################################
    set SubTestDes "Save BRAINS2 Histogram test"
    set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst brains2 $TestHistID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveHistogramTest "BRAINS2" $TestHistID ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst $LogFile $ModuleName

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2 save histogram /invalid_directory_nametmp/TEST/10_ACPC/junk.hst briains2 $TestHistID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk.hst briains2 $TestHistID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }




    ############################### BRAINS Histogram ###########################################
    set SubTestDes "Save BRAINS Histogram test"
    set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk_histogram brains1 $TestHistID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveHistogramTest "BRAINS" $TestHistID ${OUTPUT_DIR}/TEST/10_ACPC/junk_histogram $LogFile $ModuleName

        set SubTestDes "BRAINS invalid filename test"
        set errorTest [b2 save histogram /invalid_directory_name/TEST/10_ACPC/junk_histogram brains1 $TestHistID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS invalid filter-suffix test"
        set errorTest [b2 save histogram ${OUTPUT_DIR}/TEST/10_ACPC/junk_histogram brains1 $TestHistID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }

    ReportTestStatus $LogFile  [ expr { [ b2 destroy histogram $TestHistID ] != -1 } ] $ModuleName "Destroying histogram $TestHistID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

