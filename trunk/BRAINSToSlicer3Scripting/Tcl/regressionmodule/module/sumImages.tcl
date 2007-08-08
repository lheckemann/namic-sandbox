# \author    Greg Harris"
# \date        $Date: 2006-01-30 13:34:07 -0600 (Mon, 30 Jan 2006) $
# \brief    This module tests the "b2 sum images"
# \fn        proc sumImages {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2 sum images command
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc sumImages {pathToRegressionDir dateString} {


    set ModuleName "sumImages"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2 sum images command"
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
    set SubTestDes "Sum images - Load Image(1) test"
    set Image1Id [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$Image1Id != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load an Image ###########################################
    set SubTestDes "Sum images - Load Image(2) test"
    set Image2Id [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$Image2Id != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load an Image ###########################################
    set SubTestDes "Sum images - Load Image(3) test"
    set Image3Id [b2 load image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
    if { [ ReportTestStatus $LogFile  [ expr {$Image3Id != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ############################### Load a Mask ###########################################
    set SubTestDes "Sum images - Load Mask test"
    set MaskId [b2 load mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment]
    if { [ ReportTestStatus $LogFile  [ expr {$MaskId != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }



    # First Test for invalid arguements
    set SubTestDes "required arguement test (1)"
    set errorTest [b2 sum images]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "arguement number test"
    set b2cmd [format "b2 sum images {%s %s %s} junk=" $Image1Id $Image2Id $Image3Id]
    set errorTest [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional arguement test"
    set b2cmd [format "b2 sum images {%s %s %s} junk= test" $Image1Id $Image2Id $Image3Id]
    set errorTest [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2 sum images {65536}]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################ BRAINS2 SUM IMAGE - 1 Image ########################################
    set SubTestDes "B2 SUM IMAGE test - 1 Image"
    set b2cmd [format "b2 sum images {%s}" $Image1Id ]
    set TestImageId [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$TestImageId == -1 } ] $ModuleName $SubTestDes

#    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (1)"
#    set result [b2 measure image mask $MaskId $Image1Id]
#    ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes

#    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (1)"
#    set resultSum [b2 measure image mask $MaskId $TestImageId]
#    ReportTestStatus $LogFile  [ expr {$resultSum != -1 } ] $ModuleName $SubTestDes

    # Compare Results
#    set SubTestDes "B2 SUM IMAGE(1) - Result comparison"
#    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - [lindex [lindex $resultSum 0] 1]) <  0.00001 } ] $ModuleName $SubTestDes
#    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 SUM IMAGE - 2 Images ########################################
    set SubTestDes "B2 SUM IMAGE test - 2 Image"
    set b2cmd [format "b2 sum images {%s %s}" $Image1Id $Image2Id]
    set TestImageId [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (2)"
    set result [b2 measure image mask $MaskId $TestImageId]
    ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes

    # Compare Results
    set standard 191.573005017184499
    set SubTestDes "B2 SUM IMAGE(2) - Result comparison -- Does [lindex [lindex $result 0] 1] nearly equal $standard by fraction [ expr abs([lindex [lindex $result 0] 1] - $standard) / $standard ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard <  0.00025} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 SUM IMAGE - 3 Images ########################################
    set SubTestDes "B2 SUM IMAGE test - 3 Image"
    set b2cmd [format "b2 sum images {%s %s %s}" $Image1Id $Image2Id $Image3Id]
    set TestImageId [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (3)"
    set result [b2 measure image mask $MaskId $TestImageId]
    ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes
    # Compare Results
    set standard 352.883034368314611
    set SubTestDes "B2 SUM IMAGE(3) - Result comparison -- Does [lindex [lindex $result 0] 1] nearly equal $standard by fraction [ expr abs([lindex [lindex $result 0] 1] - $standard) / $standard ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard <  0.00025} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ############################ BRAINS2 SUM IMAGE - Data Type ########################################
    set SubTestDes "B2 SUM IMAGE test - 4 Image"
    set b2cmd [format "b2 sum images {%s %s %s} data-type= float-single" $Image1Id $Image2Id $Image3Id]
    set TestImageId [eval $b2cmd]
    ReportTestStatus $LogFile  [ expr {$TestImageId != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (4)a"
    set result1 [b2 measure image mask $MaskId $Image1Id]
    ReportTestStatus $LogFile  [ expr {$result1 != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (4)b"
    set result2 [b2 measure image mask $MaskId $Image2Id]
    ReportTestStatus $LogFile  [ expr {$result2 != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (4)c"
    set result3 [b2 measure image mask $MaskId $Image3Id]
    ReportTestStatus $LogFile  [ expr {$result3 != -1 } ] $ModuleName $SubTestDes

    set SubTestDes "B2 SUM IMAGE - Measure Sum Image Test (4)d"
    set result [b2 measure image mask $MaskId $TestImageId]
    ReportTestStatus $LogFile  [ expr {$result != -1 } ] $ModuleName $SubTestDes

    # Compare Results
    set standard [expr [lindex [lindex $result1 0] 1] + [lindex [lindex $result2 0] 1] + \
                            [lindex [lindex $result3 0] 1] ]
    set SubTestDes "B2 SUM IMAGE(4) - Result comparison -- Does [lindex [lindex $result 0] 1] nearly equal $standard by fraction [ expr abs([lindex [lindex $result 0] 1] - $standard) / $standard ]"
    ReportTestStatus $LogFile  [ expr {abs([lindex [lindex $result 0] 1] - $standard) / $standard <  0.00025} ] $ModuleName $SubTestDes

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $Image1Id ] != -1 } ] $ModuleName "Destroying image $Image1Id"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $Image2Id ] != -1 } ] $ModuleName "Destroying image $Image2Id"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $Image3Id ] != -1 } ] $ModuleName "Destroying image $Image3Id"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $MaskId ] != -1 } ] $ModuleName "Destroying mask $MaskId"




    return [ StopModule  $LogFile $ModuleName ]



    return $MODULE_SUCCESS
}

