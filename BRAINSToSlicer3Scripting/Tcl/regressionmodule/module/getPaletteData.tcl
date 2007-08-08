# \author    Hans J. Johnson"
# \date        $Date: 2007-06-14 17:10:49 +0200 (Thu, 14 Jun 2007) $
# \brief    This module tests the b2 get palette data command
# \fn        proc getPaletteData {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 get palette data command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getPaletteData {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "getPaletteData"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2 get palette data command"
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
            set groucho module/data/palette.dat
        } else {
            set SubTestDes "find 'brainsN' in $harpo test"
            if { [ ReportTestStatus $LogFile  [ expr { [regexp -nocase -indices {brains[0-9a-z]*} $harpo startend] == 1 } ] $ModuleName $SubTestDes] == 0} {
                set startend "0 -1"
            }
        set chico [string range $harpo 0 [lindex $startend 1]]/src/regressionmodule/module/data
        puts "finding in ${chico}"
            set SubTestDes "find palette.dat test"
            if { [ ReportTestStatus $LogFile  [ expr { [catch { exec find "${chico}" -name palette.dat } squabble] == 0 } ] $ModuleName $SubTestDes] == 0} {
                puts "exec find failed: $squabble" 
                return $MODULE_FAILURE
            } else {
                puts "exec find succeeded: $squabble" 
                set groucho [lindex $squabble 0]
            }
        }

    set SubTestDes "read in palette.dat at $groucho test"
    if { [ ReportTestStatus $LogFile  [ expr { [catch { set ExpectData [exec cat $groucho] } squabble] == 0 } ] $ModuleName $SubTestDes] == 0} {
        puts "exec failed: $squabble" 
        return $MODULE_FAILURE
    }

    set SubTestDes "Get palette data - Load Palette test"
    set TestPalID [b2 load palette $B2_TALAIRACH_DIR/../palette/pet.pal]
    if { [ ReportTestStatus $LogFile  [ expr {$TestPalID != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 get palette data]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 get palette data $TestPalID -1 junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 get palette data $TestPalID -1 junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    set SubTestDes "Palette Data test"
    set histData [b2 get palette data $TestPalID]
    ReportTestStatus $LogFile  [ expr {[llength $histData] == 256 } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $ExpectData]} {incr i} {
        for {set j 0} {$j < [llength [lindex $ExpectData 0]]} {incr j} {
            SingleMeasureEpsilonTest "Palette index $i value $j test:" 0.0001 [lindex [lindex $histData $i] $j] [lindex [lindex $ExpectData $i] $j] $LogFile $ModuleName
        }
    }


    ReportTestStatus $LogFile  [ expr { [ b2 destroy palette $TestPalID ] != -1 } ] $ModuleName "Destroying palette $TestPalID"

    return [ StopModule  $LogFile $ModuleName ]
}

