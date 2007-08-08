#######################################################
# Test Example:
#source ReportTestStatus.tcl
#
#set errorFlag [catch {set f [open "TestOut" w]}];
#    if {$errorFlag != 0} {
#        »·    puts "ERROR: Failed to open module ($name) output
#            file.";
#            return $MODULE_FAILURE;
#    }
#
#set ModName "dummyModule";
#set SubTestDes "A dummy test";
#set value  100;
#ReportTestStatus $f  [ expr {$value == 10 } ] $ModName $SubTestDes;
#ReportTestStatus $f  [ expr {$value == 100 } ] $ModName $SubTestDes;
#ReportTestStatus $f  [ expr {$value == 10 } ] $ModName $SubTestDes;
#ReportTestStatus $f  [ expr {$value == 10 } ] $ModName $SubTestDes;
#close $f;


