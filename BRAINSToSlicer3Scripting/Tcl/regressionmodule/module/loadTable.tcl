# \author    Hans J. Johnson"
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the loading of various Table
#             file formats
# \fn        proc loadTable {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Load a BRAINS2 Table
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc loadTable {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR


    set ModuleName "loadTable"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 load table command and loading various Table file formats"
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


    # First Test for invalid arguements
    set SubTestDes "required arguement test"
    set errorTest [b2 load table]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 load table $B2_TALAIRACH_DIR/../tables/pseudoPD.tbl junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 load table $B2_TALAIRACH_DIR/../tables/pseudoPD.tbl junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid file test"
    set errorTest [b2 load table /invalid_directory_name/pseudoPD.tbl]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    ############################### BRAINS2 Table ###########################################
    set SubTestDes "BRAINS2 Table Load test"
    set TestTableId [b2 load table $B2_TALAIRACH_DIR/../tables/pseudoPD.tbl]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTableId != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy table $TestTableId ] != -1 } ] $ModuleName "Destroying table $TestTableId"
    }

    ############################ BRAINS2 Table Filter ########################################
    set SubTestDes "BRAINS2 Table Load test"
    set TestTableId [b2 load table $B2_TALAIRACH_DIR/../tables/pseudoPD.tbl filter= brains2]
    if { [ ReportTestStatus $LogFile  [ expr {$TestTableId != -1 } ] $ModuleName $SubTestDes] } {
        ReportTestStatus $LogFile  [ expr { [ b2 destroy table $TestTableId ] != -1 } ] $ModuleName "Destroying table $TestTableId"
    }


    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

