# \author    Hans J. Johnson"
# \date        $Date: 2005-04-28 14:56:19 -0500 (Thu, 28 Apr 2005) $
# \brief    This module tests the bad parameters for b2_load_image so that it is not done in all other load image files.
# \fn        proc loadImage-BadParameters {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error

proc loadImage-BadParameters {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
    set ModuleName "loadImage-BadParameters" ;
    set ModuleAuthor "Hans J. Johnson" ;
    set ModuleDescription "Test the b2_load_image command for bad parameters." ;
    global MODULE_SUCCESS ;
    global MODULE_FAILURE ;
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString] ;

########################################
########################################
# Run Tests
########################################
########################################
# First Test for invalid arguments
    set SubTestDes "required argument test" ;
    set ErrorReturnOK [b2_load_image] ;
    ReportTestStatus $LogFile  [ expr {$ErrorReturnOK == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "argument number test" ;
    set ErrorReturnOK [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096 junk= ] ;
    ReportTestStatus $LogFile  [ expr {$ErrorReturnOK == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "optional argument test" ;
    set ErrorReturnOK [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096 junk= test] ;
    ReportTestStatus $LogFile  [ expr {$ErrorReturnOK == -1 } ] $ModuleName $SubTestDes ;

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    return [ StopModule  $LogFile $ModuleName ] ;
}

