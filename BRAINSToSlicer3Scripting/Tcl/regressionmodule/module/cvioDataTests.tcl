# This test is out-dated and should not be run.

# \author    Vincent A. Magnotta
# \date        $Date: 2005-03-08 16:40:34 -0600 (Tue, 08 Mar 2005) $
# \brief    This module tests the various utility programs created
#           to extract infotmation from the raw CVIO files.
# \fn        proc cvioDataTests {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Runs the
#
# To Do
#------------------------------------------------------------------------
# Nothing
#



proc cvioDataTests {pathToRegressionDir dateString} {

    set ModuleName "cvioDataTests"
    set ModuleAuthor "Vincent A. Magnotta"
    set ModuleDescription "Test the cvioDataTests program"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]


########################################
########################################
# Run Tests
########################################
########################################



    ### Create Directories
    catch {exec mkdir -p $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO}

    if {[file exists $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/stimulus.txt]} {
        exec rm $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/stimulus.txt
    }

    if {[file exists $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/response.txt]} {
        exec rm $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/response.txt
    }

    if {[file exists $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/implicitReduce.txt]} {
        exec rm $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/implicitReduce.txt
    }


    ### Run the stimulus conversion program
    set rawStimulusToTableBin "[file dirname [info nameofexecutable]]/rawStimulusToTable"
    set SubTestDes "Does file exist? $rawStimulusToTableBin"
    if { [ ReportTestStatus $LogFile  [file exist $rawStimulusToTableBin] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Run rawStimulusToTable from path $rawStimulusToTableBin"
    set result [catch {exec $rawStimulusToTableBin -f $pathToRegressionDir/SGI/CVIO/fmri05_stim_raw.cvio -s $pathToRegressionDir/SGI/CVIO/stim -o $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/stimulus.txt} cmdData]
    puts "Run rawStimulusToTable - Result $result"
    if { [ ReportTestStatus $LogFile  [catch {exec diff $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/stimulus.txt $pathToRegressionDir/SGI/CVIO/fmri05-Stimulus.txt}] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    ### Run the response conversion program
    set rawResponseToTableBin "[file dirname [info nameofexecutable]]/rawResponseToTable"
    set SubTestDes "Does file exist? $rawResponseToTableBin"
    if { [ ReportTestStatus $LogFile  [file exist $rawResponseToTableBin] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Run rawResponseToTable from path $rawResponseToTableBin"
    set result [catch {exec $rawResponseToTableBin -f $pathToRegressionDir/SGI/CVIO/fmri05_response_raw.cvio -o $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/response.txt} cmdData]
    puts "Run rawResponseToTable - Result $result"
    if { [ ReportTestStatus $LogFile  [catch {exec diff $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/response.txt $pathToRegressionDir/SGI/CVIO/fmri05-SubjectResponses.txt}] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    ### Run the implicit learning stim-response merging program
    set filterImplicitLearningBin "[file dirname [info nameofexecutable]]/filterImplicitLearning"
    set SubTestDes "Does file exist? $filterImplicitLearningBin"
    if { [ ReportTestStatus $LogFile  [file exist $filterImplicitLearningBin] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Run filterImplicitLearning from path $rawResponseToTableBin"
    set result [catch {exec $filterImplicitLearningBin -r $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/response.txt -s $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/stimulus.txt -o $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/implicitReduce.txt} cmdData]
    puts "Run filterImplicitLearning - Result $result"
    if { [ ReportTestStatus $LogFile  [catch {exec diff $pathToRegressionDir/${OUTPUT_DIR}/TEST/CVIO/implicitReduce.txt $pathToRegressionDir/SGI/CVIO/fmri05-Implicit-StimResponsePair.txt}] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }


    return [ StopModule  $LogFile $ModuleName ]

    return $MODULE_SUCCESS
}

