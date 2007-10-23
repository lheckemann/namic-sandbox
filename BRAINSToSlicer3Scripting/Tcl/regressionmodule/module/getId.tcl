# \author    Hans J. Johnson"
# \date        $Date: 2007-02-02 07:20:50 -0600 (Fri, 02 Feb 2007) $
# \brief    This module tests the b2_get_name command
# \fn        proc getIds {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_get_patient-id |scan-id commands
#
# To Do
#------------------------------------------------------------------------
# Nothing
#


proc getId {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR

    set ModuleName "getId"
    set ModuleAuthor "Hans J. Johnson"
    set ModuleDescription "Test the b2_get_patient-id and b2_get_scan-id commands"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################


    set objectTypes {    image \
                        mask \
                        roi \
                        gtsurface \
                        transform \
                        talairach-parameters \
                        tissue-class \
                        histogram \
                        landmark }

    set objectFiles "   $pathToRegressionDir/SGI/MR/4x-B1/TEST/15_002/19771.002.096 \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment.mask \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/96tricia_rcaud.zroi \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_130_l_dec35.gts \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/resample_parameters \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters \
                        $pathToRegressionDir/SGI/MR/B2-Class-Mdl/TEST/05_ACPC/ANON0017_05_Tissue_Class.mdl \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/cran_histogram \
                        $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/user/landmark_locations"

    set patientIds {     4x-B1 \
                        4x-B1 \
                        ANON024 \
                        ANON024 \
                        4x-B1 \
                        4x-B1 \
                        ANONMYZ \
                        4x-B1 \
                        ANON024 }

    set scanIds {        TEST \
                        TEST \
                        ANON013 \
                        ANON013 \
                        TEST \
                        TEST \
                        ANON0017 \
                        TEST \
                        ANON013 }




    # First Test for invalid arguements
    set SubTestDes "Patient-Id required arguement test"
    set errorTest [b2_get_patient-id image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Patient-Id arguement object type test"
    set errorTest [b2_get_patient-id test ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Scan-Id required arguement test"
    set errorTest [b2_get_scan-id image]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "Scan-Id arguement object type test"
    set errorTest [b2_get_scan-id test ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes


    for {set i 0} {$i < [llength $objectTypes]} {incr i} {
### Need special logic to not test gtsurface when GTS is not compiled.
      global BRAINS2_GTS
        if { "gtsurface" == [lindex $objectTypes $i]  && ${BRAINS2_GTS} == "OFF" } {
           continue;
        }
        set SubTestDes "Get Ids [lindex $objectTypes $i] load test: b2 load  [lindex $objectTypes $i] [lindex $objectFiles $i]"
        set testObjectId [b2 load [lindex $objectTypes $i] [lindex $objectFiles $i]]
        ReportTestStatus $LogFile  [ expr {$testObjectId != -1 } ] $ModuleName $SubTestDes

        set SubTestDes "Get Patient-Id [lindex $objectTypes $i] test: b2_get_patient-id [lindex $objectTypes $i] $testObjectId"
        set name [b2_get_patient-id [lindex $objectTypes $i] $testObjectId]
        ReportTestStatus $LogFile  [ expr {$name == [lindex $patientIds $i ]} ] $ModuleName $SubTestDes

        set SubTestDes "Get Scan-Id [lindex $objectTypes $i] test: b2_get_scan-id [lindex $objectTypes $i] $testObjectId"
        set name [b2_get_scan-id [lindex $objectTypes $i] $testObjectId]
        ReportTestStatus $LogFile  [ expr {$name == [lindex $scanIds $i ]} ] $ModuleName $SubTestDes

        ReportTestStatus $LogFile  [ expr { [ b2 destroy  [lindex $objectTypes $i] $testObjectId ] != -1 } ] $ModuleName "Destroying  [lindex $objectTypes $i] $testObjectId"
    }

    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

