# \author  Greg Harris"
# \date    $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief  This module tests the "b2_min_images"
# \fn    proc minImages {pathToRegressionDir dateString}
# \param  string pathToRegressionDir  - Path to the regresssion test directory
# \param  string dateString      - String to label output file
# \result  1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_min_images command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc minImages {pathToRegressionDir dateString} {


                set ModuleName "minImages"
                set ModuleAuthor "Greg Harris"
                set ModuleDescription "Test the b2_min_images command"
                global MODULE_SUCCESS
                global MODULE_FAILURE
                set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr


########################################
########################################
# Run Tests
########################################
########################################


############################### Load an Image ###########################################
                set SubTestDes "Min images - Load Image(1) test"
                set Image1Id [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
                if { [ ReportTestStatus $LogFile  [ expr {$Image1Id != -1 } ] $ModuleName $SubTestDes] == 0} {
                        return $MODULE_FAILURE
                }

############################### Load an Image ###########################################
        set SubTestDes "Min images - Load Image(2) test"
                set Image2Id [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr]
                if { [ ReportTestStatus $LogFile  [ expr {$Image2Id != -1 } ] $ModuleName $SubTestDes] == 0} {
                        return $MODULE_FAILURE
                }

############################### Load an Image ###########################################
        set SubTestDes "Min images - Load Image(3) test"
                set Image3Id [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
                if { [ ReportTestStatus $LogFile  [ expr {$Image3Id != -1 } ] $ModuleName $SubTestDes] == 0} {
                        return $MODULE_FAILURE
                }

############################### Load a Mask ###########################################
        set SubTestDes "Min images - Load Mask test"
                set MaskId [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
                if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
                        return $MODULE_FAILURE
                }



# First Test for invalid arguements
        set SubTestDes "required arguement test (1)"
                set errorTest [b2_sum_images]
                ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

                set SubTestDes "arguement number test"
                set b2cmd [format "b2_min_images {%s %s %s} junk=" $Image1Id $Image2Id $Image3Id]
                set errorTest [eval $b2cmd]
                ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

                set SubTestDes "optional arguement test"
                set b2cmd [format "b2_min_images {%s %s %s} junk= test" $Image1Id $Image2Id $Image3Id]
                set errorTest [eval $b2cmd]
                ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

                set SubTestDes "invalid image test"
                set errorTest [b2_min_images {65536}]
                ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



############################ BRAINS2 MIN IMAGE - 1 Image ########################################
                set SubTestDes "B2 MIN IMAGE test - 1 Image"
                set b2cmd [format "b2_min_images {%s}" $Image1Id ]
                set TestImageId [eval $b2cmd]
                ReportTestStatus $LogFile  [ expr {$TestImageId == -1 } ] $ModuleName $SubTestDes

#    set SubTestDes "B2 MIN IMAGE - Measure Image Test (1)"
#    set result [b2_measure_image_mask $MaskId $Image1Id]
#    ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes

#    set SubTestDes "B2 MIN IMAGE - Measure Image Test (1)"
#    set resultSum [b2_measure_image_mask $MaskId $TestImageId]
#    ReportTestStatus $LogFile  [ expr {$resultSum != -1 } ] $ModuleName $SubTestDes

# Compare Results
#    set SubTestDes "B2 MIN IMAGE(1) - Result comparison"
#    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - [lindex [lindex $resultSum 0] 1]) <  0.00001 } ] $ModuleName $SubTestDes
#    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

############################ BRAINS2 MIN IMAGE - 2 Images ########################################
                set SubTestDes "B2 MIN IMAGE test - 2 Image"
                set TestImageId [b2_min_images [list $Image1Id $Image2Id]]
                ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

                set SubTestDes "B2 MIN IMAGE - Measure Image Test (2)"
                set result [b2_measure_image_mask $MaskId $TestImageId]
                ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes

# Compare Results
                set standard 69.646440795597258
                set SubTestDes "B2 MIN IMAGE(2) - Result comparison -- Does [lindex [lindex $result 0] 1] nearly equal $standard by fraction [ expr abs([lindex [lindex $result 0] 1] - $standard) / $standard ]"
                ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard <  0.00025} ] $ModuleName $SubTestDes

                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

############################ BRAINS2 MIN IMAGE - 3 Images ########################################
                set SubTestDes "B2 MIN IMAGE test - 3 Image"
                set TestImageId [b2_min_images [list $Image1Id $Image2Id $Image3Id]]
                ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

                set SubTestDes "B2 MIN IMAGE - Measure Image Test (3)"
                set result [b2_measure_image_mask $MaskId $TestImageId]
                ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes
# Compare Results
                set standard 69.645830874448833
                set SubTestDes "B2 MIN IMAGE(3) - Result comparison -- Does [lindex [lindex $result 0] 1] nearly equal $standard by fraction [ expr abs([lindex [lindex $result 0] 1] - $standard) / $standard ]"
                ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard <  0.00025} ] $ModuleName $SubTestDes

                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

############################ BRAINS2 MIN IMAGE - Data Type ########################################
                set SubTestDes "B2 MIN IMAGE test - 4 Image"
                set TestImageId [b2_min_images [list $Image1Id $Image2Id $Image3Id] data-type= float-single]
                ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

                set SubTestDes "B2 MIN IMAGE - Measure Image Test (4)"
                set result [b2_measure_image_mask $MaskId $TestImageId]
                ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes
# Compare Results
                set standard 69.645830874448833
                set SubTestDes "B2 MIN  IMAGE(4) - Result comparison -- Does [lindex [lindex $result 0] 1] nearly equal $standard by fraction [ expr abs([lindex [lindex $result 0] 1] - $standard) / $standard ]"
                ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard <  0.00025} ] $ModuleName $SubTestDes


                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $Image1Id ] != -1 } ] $ModuleName "Destroying image $Image1Id"
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $Image2Id ] != -1 } ] $ModuleName "Destroying image $Image2Id"
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $Image3Id ] != -1 } ] $ModuleName "Destroying image $Image3Id"
                ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $MaskId ] != -1 } ] $ModuleName "Destroying mask $MaskId"

                return [ StopModule  $LogFile $ModuleName ]

                return $MODULE_SUCCESS
}

