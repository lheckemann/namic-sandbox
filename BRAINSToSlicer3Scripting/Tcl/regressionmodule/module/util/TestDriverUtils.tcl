#######  This file overwrite the functionality of StartModule and StopModule defined in ModuleUtils!
#######  Previously there were several other files that were also duplicated, but that caused an inconsistency
#######  between how regression tests were run.


# \author   John Todtz 
# \date    5/8/2002
# \brief  Provides a common procedure for starting a module
# \result  Places result into file
# \fn    proc StartModule {} 
# \param ModuleName        - The name of the module from which the test was run
# \param ModuleAuthor     - The developer that wrote the module
# \param ModuleDescription   - A description of the test that was run
# \return An open file for writing
proc StartModule { ModuleName ModuleAuthor ModuleDescription dateString } {
    global env

    set errorFlag [catch {set LogFile [open ${ModuleName}_$dateString w]}]
    if {$errorFlag != 0} {
        puts "SETUP_ERROR: Failed to open module ($ModuleName) output file."
        return $MODULE_FAILURE
    }

    puts $LogFile "Starting Regression Test Module - $ModuleName"
    puts $LogFile "Regression Test Author - $ModuleAuthor"
    puts $LogFile "Regression Purpose: $ModuleDescription"
    puts -nonewline  $LogFile "Starting Time "
    puts $LogFile [exec date]
    return $LogFile
}


# \author  Hans J. Johnson
# \date    3/14/2002
# \brief  Provides a common procedure for stopping a module
# \result  Places result into file
# \fn    proc StopModule {} 
# \param LogFile        - The open file to write in
# \param ModuleName     - The name of the module from which the test was run
# \return     - nothing
proc StopModule { LogFile ModuleName } {
    puts -nonewline  $LogFile "Ending Time "
    puts $LogFile [exec date]
    puts $LogFile "Regression Test Module - $ModuleName Completed"
    # Close the module log file
    close $LogFile
}

