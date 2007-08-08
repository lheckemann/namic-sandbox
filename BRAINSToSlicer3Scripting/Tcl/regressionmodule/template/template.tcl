# \author    Vincent A. Magnotta
# \date
# \brief    Tests which are run by this module
# \fn        proc template {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc template {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
    set ModuleName "template"
    set ModuleAuthor "Vincent A. Magnotta"
    set ModuleDescription "Testing of ......."
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription ]

########################################
########################################
# Run Tests
########################################
########################################

########################################
########################################
# Clean up files
########################################
########################################



########################################
########################################
# Free memory - All objects must be destroyed
########################################
########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    StopModule $LogFile $ModuleName
    return 1
}

