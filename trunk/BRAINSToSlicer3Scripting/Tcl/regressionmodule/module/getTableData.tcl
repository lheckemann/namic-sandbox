# \author    Greg Harris"
# \date        $Date: 2007-06-14 17:10:49 +0200 (Thu, 14 Jun 2007) $
# \brief    This module tests the b2_get_table_data command
# \fn        proc getTableData {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_table_data command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getTableData {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR


    set ModuleName "getTableData"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_get_table_data command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#    close $LogFile
#    set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################


# Refactored for the new standard place for the regressionmodule.
        set harpo [file dirname [info script]]
        if {[string equal $harpo "."]} {
            set groucho module/data/table.dat
        } else {
            set SubTestDes "find 'brainsN' in $harpo test"
            if { [ ReportTestStatus $LogFile  [ expr { [regexp -nocase -indices {brains[0-9a-z]*} $harpo startend] == 1 } ] $ModuleName $SubTestDes] == 0} {
                set startend "0 -1"
            }
        set chico [string range $harpo 0 [lindex $startend 1]]/src/regressionmodule/module/data
        puts "finding in ${chico}"
            set SubTestDes "find table.dat test"
            if { [ ReportTestStatus $LogFile  [ expr { [catch { exec find "${chico}" -name table.dat } squabble] == 0 } ] $ModuleName $SubTestDes] == 0} {
                puts "exec find failed: $squabble" 
                return $MODULE_FAILURE
            } else {
                puts "exec find succeeded: $squabble" 
                set groucho [lindex $squabble 0]
            }
        }

    set SubTestDes "read in table.dat at $groucho test"
    if { [ ReportTestStatus $LogFile  [ expr { [catch { set ExpectData [exec cat $groucho] } squabble] == 0 } ] $ModuleName $SubTestDes] == 0} {
        puts "exec failed: $squabble" 
        return $MODULE_FAILURE
    }


    set SubTestDes "Get table data - Load Table test"
    set TestTableId [b2_load_table $B2_TALAIRACH_DIR/../tables/pseudoPD.tbl]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTableId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_get_table_data]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2_get_table_data $TestTableId -1 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_get_table_data $TestTableId -1 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Table Data test"
    set tableData [b2_get_table_data $TestTableId]
    ReportTestStatus $LogFile  [ expr {[llength $tableData] == 256 } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $ExpectData]} {incr i} {
        for {set j 0} {$j < [llength [lindex $ExpectData 0]]} {incr j} {
        #ReportTestStatus $LogFile  [ expr {[lindex [lindex $tableData $i] $j] == [lindex [lindex $ExpectData $i] $j] } ] $ModuleName "Table ($i,$j) value test"
                SingleMeasureEpsilonTest "Table ($i,$j) value test" 0.00000001 [lindex [lindex $tableData $i] $j]  [lindex [lindex $ExpectData $i] $j] $LogFile $ModuleName

        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_table $TestTableId ] != -1 } ] $ModuleName "Destroying table $TestTableId"

    return [ StopModule  $LogFile $ModuleName ]
}

