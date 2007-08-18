# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the "b2 table mask"
# \fn        proc createTableMask {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_create_table-mask command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc createTableMask {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR


    set ModuleName "createTableMask"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_create_table-mask"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################

    set resultData {{1.000000 0.000000 0.000000} {0.000000 1.000000 0.000000}}
    set maskData {{1.000000} {1.000000} }

    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2_create_table-mask]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test (2)"
    set errorTest [b2_create_table-mask 2 ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test (3)"
    set errorTest [b2_create_table-mask 2 3]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2_create_table-mask 2 3 {{0 0} {1 1}} junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid otional arguement test"
    set errorTest [b2_create_table-mask 2 3 {{0 0} {1 1}} junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################ BRAINS2 Create Roi Coronal ########################################
    set SubTestDes "B2 CREATE TABLE-MASK  test"
    set TestTableId [b2_create_table-mask 2 3 {{0 0} {1 1}} ]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTableId != -1 } ] $ModuleName $SubTestDes] } {
        set tableData [b2_get_table_data $TestTableId]
        for {set i 0} {$i < [llength $tableData]} {incr i} {
           for {set j 0} {$j < [llength [lindex $tableData $i]]} {incr j} {
               set SubTestDes "B2 CREATE TABLE-MASK  Table verification ($i $j)"
               ReportTestStatus $LogFile  [ expr {[lindex [lindex $tableData $i] $j] == [lindex [lindex $resultData $i] $j] } ] $ModuleName $SubTestDes
           }
        }
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_table $TestTableId ] != -1 } ] $ModuleName "Destroying table $TestTableId"

    ############################ BRAINS2 Create Roi Coronal ########################################
    set SubTestDes "B2 CREATE EVENT-TABLE  test"
    set TestTableId [b2_create_table-mask 10 1 {{0 0} {1 0} {9 0}} ]
    set MaskTableId [b2_create_table-mask 10 1 {{0 0} {9 0}} ]

    # First Test for invalid arguements
    set SubTestDes "B2 CREATE EVENT-TABLE required arguement test (1)"
    set errorTest [b2_create_event-table]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CREATE EVENT-TABLE arguement number test (2)"
    set errorTest [b2_create_event-table $TestTableId ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CREATE EVENT-TABLE optional arguement test"
    set errorTest [b2_create_event-table $TestTableId $MaskTableId junk=]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CREATE EVENT-TABLE invalid optional arguement test"
    set errorTest [b2_create_event-table $TestTableId $MaskTableId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set tableData [b2_get_table_data $TestTableId]
    puts $tableData

    set tableData [b2_get_table_data $MaskTableId]
    puts $tableData

    set SubTestDes "B2 CREATE EVENT-TABLE test"
    set eventTableId [b2_create_event-table $TestTableId $MaskTableId ]
    if { [ ReportTestStatus $LogFile  [ expr {$eventTableId != -1 } ] $ModuleName $SubTestDes] } {
        set tableData [b2_get_table_data $eventTableId]
        puts $tableData
        for {set i 0} {$i < [llength $tableData]} {incr i} {
           for {set j 0} {$j < [llength [lindex $tableData $i]]} {incr j} {
               set SubTestDes "B2 CREATE EVENT-TABLE  Table verification ($i $j)"
               ReportTestStatus $LogFile  [ expr {[lindex [lindex $tableData $i] $j] == [lindex [lindex $maskData $i] $j] } ] $ModuleName $SubTestDes
           }
        }
    }

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_table $TestTableId ] != -1 } ] $ModuleName "Destroying table $TestTableId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_table $MaskTableId ] != -1 } ] $ModuleName "Destroying table $MaskTableId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_table $eventTableId   ] != -1 } ] $ModuleName "Destroying table $eventTableId  "


    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

