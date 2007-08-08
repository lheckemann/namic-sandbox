proc CoreSaveTableTest {filterName TestTableID SaveFileName LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

    set SubTestDes "Save Table - Load saved $filterName Table"
    set SaveTableID [b2 load table $SaveFileName]
    ReportTestStatus $LogFile  [ expr {$SaveTableID != -1 } ] $ModuleName $SubTestDes

    set origTableData [b2 get table data $TestTableID ]
    set newTableData [b2 get table data $SaveTableID]

    set SubTestDes "Save Table - $filterName Row Check Table"
    ReportTestStatus $LogFile  [ expr {[llength $origTableData] == [llength $newTableData] } ] $ModuleName $SubTestDes
    set SubTestDes "Save Table - $filterName Column Check Table"
    ReportTestStatus $LogFile  [ expr {[llength [lindex $origTableData 0]] == [llength [lindex $newTableData 0]] } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $origTableData]} {incr i} {
        for {set j 0} {$j < [llength [lindex $origTableData 0]]} {incr j} {
            set SubTestDes "$filterName Table Comparison Location ($i,$j)"
            ReportTestStatus $LogFile  [ expr {[lindex [lindex $origTableData $i] $j] == [lindex [lindex $newTableData $i] $j] } ] $ModuleName $SubTestDes
        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy table $SaveTableID ] != -1 } ] $ModuleName "Destroying table $SaveTableID"
}



# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the saving of various table file formats
# \fn        proc saveTable {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Save a BRAINS2 Table
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc saveTable {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR


    set ModuleName "saveTable"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 save table command and saving various file formats"
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

    set SubTestDes "Save Table - Load Table test"
    set TestTableID [b2 load table $B2_TALAIRACH_DIR/../tables/pseudoPD.tbl]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTableID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 save table]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (2)"
    set errorTest [b2 save table ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "required arguement test (3)"
    set errorTest [b2 save table ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl brains]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 save table ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl brains $TestTableID junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 save table ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl brains $TestTableID junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Palette ###########################################
    set SubTestDes "Save BRAINS2 Table test"
    set errorTest [b2 save table ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl brains2 $TestTableID]
    if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {

        CoreSaveTableTest "BRAINS2" $TestTableID ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl $LogFile $ModuleName

        set SubTestDes "BRAINS2 invalid filename test"
        set errorTest [b2 save table /invalid_directory_nametmp/SGI/MR/4x-B1/TEST/10_ACPC/junk.tbl brains2 $TestTableID]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

        set SubTestDes "BRAINS2 invalid filter-suffix test"
        set errorTest [b2 save table ${OUTPUT_DIR}/TEST/10_ACPC/junk.tbl brains2 $TestTableID filter-suffix= -invalid]
        ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes
    }



    ReportTestStatus $LogFile  [ expr { [ b2 destroy table $TestTableID ] != -1 } ] $ModuleName "Destroying table $TestTableID"

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

