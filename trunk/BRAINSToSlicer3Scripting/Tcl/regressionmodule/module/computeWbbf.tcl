# \author    Vincent A. Magnotta
# \date        $Date: 2006-08-11 13:54:53 -0500 (Fri, 11 Aug 2006) $
# \brief    This module tests the "b2 compute wbbf"
# \fn        proc computeWbbf {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 compute wbbf command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc computeWbbf {pathToRegressionDir dateString} {


    set ModuleName "computeWbbf"
    set ModuleAuthor "Vincent A. Magnotta"
    set ModuleDescription "Test the b2 compute wbbf command"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################



    ############################### Load an Image ###########################################
    set SubTestDes "Compute WBBF - Load Image (IMF) test"
    set ImfImageId [b2_load_image $pathToRegressionDir/SGI/pet_images/ge-4096/TEST/pcanonimiz__426.imf data-type= float-single]
    if { [ ReportTestStatus $LogFile  [ expr {$ImfImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }

    set SubTestDes "Compute WBBF - Load Image (IMA) test"
    set ImaImageId [b2_load_image $pathToRegressionDir/SGI/pet_images/ge-4096/TEST/pcanonimiz__426.ima data-type= float-single]
    if { [ ReportTestStatus $LogFile  [ expr {$ImaImageId != -1 } ] $ModuleName $SubTestDes] == 0} {
#return $MODULE_FAILURE
    }


    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 compute wbbf]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set errorTest [b2 compute wbbf $ImfImageId junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set errorTest [b2 compute wbbf $ImfImageId junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2 compute wbbf 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################ BRAINS2 Create Histogram ########################################
    set SubTestDes "B2 CLIP IMAGE - IMF test"
    set result [b2 compute wbbf $ImfImageId]
    ReportTestStatus $LogFile  [ expr {[lindex $result 1] != 70.543617 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 CLIP IMAGE - IMA test"
    set result [b2 compute wbbf $ImaImageId]
    ReportTestStatus $LogFile  [ expr {[lindex $result 1] != 1368.494141 } ] $ModuleName $SubTestDes



    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImfImageId ] != -1 } ] $ModuleName "Destroying image $ImfImageId"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $ImaImageId ] != -1 } ] $ModuleName "Destroying image $ImaImageId"




    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

