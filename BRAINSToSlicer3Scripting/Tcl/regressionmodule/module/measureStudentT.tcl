# \author       Hans J. Johnson"
# \date         3/19/2002
# \brief        b2 measure volume
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
# set pathToRegressionDir "../regressiontest"
# set dateString "DEBUG"
proc measureStudentT { pathToRegressionDir dateString } {
        set ModuleName "measureStudentT"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Testing of b2 student-T"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#       close $LogFile
#       set LogFile stderr

# Run Tests
#######################################################################################################################
#######################################################################################################################

    set output [ b2 student-T { 1.0 2.0 -100 54 } { 1 2 3 4 5 6 7 8 9 10 11 12 13 } ]
#puts $output
    set standards {
    {ave1 -10.750000} {ave2 7.000000} {T -1.069387} {p 0.301803}
    }
    CoreMeasuresEpsilonTest "'student' T-test method" 0.00000 $standards $output  $LogFile $ModuleName

# Free memory
    return [ StopModule  $LogFile $ModuleName ]
      return $MODULE_SUCCESS
}

