# \author    Hans J. Johnson"
# \date        $Date: 2005-06-24 13:54:35 -0500 (Fri, 24 Jun 2005) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImage-PetFlow {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error

proc loadImage-PetFlow {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "loadImage-PetFlow"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Test the b2_load_image command and loading various image file formats"
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

############################### PETFLOW Images ###########################################
## Set known information about this test image
    set ImageTypeName "PET"
        set ImageType "FLOW"
        set ImageMin 0.996874988079071
        set ImageMax   124.000000
#       set Dimensions "128 128 15"
#       set Resolutions "2.000000 2.000000 6.500000"
#   This change allows to load an axial acquisition into our coronal-standard dimensional system.
        set Dimensions "128 15 128"
        set Resolutions "2.000000 6.500000 2.000000"
        set SubTestDes "load $ImageTypeName $ImageType test"
        set TestImageID [b2_load_image $pathToRegressionDir/SGI/pet_images/worsley-test/TEST/images/pckja10cn01_416.imf data-type= float-single]
        if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
            CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions $LogFile $ModuleName $SubTestDes
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
        }

##################### Test Loading Various Data Types Image ###########################################

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
     return [ StopModule  $LogFile $ModuleName ]
        return $MODULE_SUCCESS
}

