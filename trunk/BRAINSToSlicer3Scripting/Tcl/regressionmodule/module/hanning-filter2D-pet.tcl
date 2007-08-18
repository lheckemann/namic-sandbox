# \author       Greg Harris
# \date         1/20/2004
# \brief        proc hanning-filter2D-pet
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
# set pathToRegressionDir "../regressiontest"
# set dateString "DEBUG"
proc hanning-filter2D-pet { pathToRegressionDir dateString } {
        set ModuleName "hanning-filter2D-pet"
        set ModuleAuthor "hjohnson"
        set ModuleDescription "Testing of b2_hanning-filter2D-pet filter"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]

#        close $LogFile
#        set LogFile stderr

# Run Tests
############ load prerequisite files ###################

#Clean all images, masks, and ROI's to ensure masks id's start at 0
#set pathToRegressionDir /scratch/purify/src/regressiontest

    set SubTestDes "Load requisite objects for hanning-filter2D-pet testing"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/phantom/AnalyzeImages/impulse.hdr]
      if { ! [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error reading image.\nFailed to load Image."
      }

set all_mask [ b2_threshold_image $TestImageID 0.00 absolute-value= True ]


## historical mode 4mm filter
set filter_4mm_h [ b2_hanning-filter2D-pet $TestImageID filter-size= 4.1 historical-mode= 1 ]
set output [ b2_measure_image_mask $all_mask $filter_4mm_h ]
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filter_4mm_h ] != -1 } ] $ModuleName "Destroying image $filter_4mm_h"

set known_output {
{Mean 0.003051757579669} {Mean-Absolute-Deviation 0.006096809637313} {Variance 0.014004286376430} {Standard-Deviation 0.118339707522159} {Skewness 47.856956054233621} {Kurtosis 2523.664307246962380} {Minimum 0.000000000000000} {Maximum 7.374862670898438} {Min-Loc-X 0} {Min-Loc-Y 0} {Min-Loc-Z 0} {Max-Loc-X 16} {Max-Loc-Z 15}
}
CoreMeasuresEpsilonTest "Hanning 2.33mm historical-mode=1" 0.0001 $known_output $output $LogFile $ModuleName

## historical mode 0mm filter
set filter_0mm_h [ b2_hanning-filter2D-pet $TestImageID filter-size= 0 historical-mode= 1 ]
set output [ b2_measure_image_mask $all_mask $filter_0mm_h ]
set known_output {
{Mean 0.003051757812500} {Mean-Absolute-Deviation 0.006103329360485} {Variance 0.305175781250000} {Standard-Deviation 0.552427172801990} {Skewness 181.002763842861270} {Kurtosis 32761.000183102190931} {Minimum 0.000000000000000} {Maximum 100.000000000000000} {Min-Loc-X 0} {Min-Loc-Y 0} {Min-Loc-Z 0} {Max-Loc-X 15} {Max-Loc-Y 15} {Max-Loc-Z 15}
}
CoreMeasuresEpsilonTest "Hanning 0mm historical-mode=1" 0.0001 $known_output $output $LogFile $ModuleName
set subtract_0mm_h  [ b2_subtract_images [ list $filter_0mm_h $TestImageID ] ]
set output [ b2_measure_image_mask $all_mask $subtract_0mm_h ]
set known_output {
{Mean 0.000000000000000} {Mean-Absolute-Deviation 0.000000000000000} {Variance 0.000000000000000} {Standard-Deviation 0.000000000000000} {Skewness 0.000000000000000} {Kurtosis 0.000000000000000} {Minimum 0.000000000000000} {Maximum 0.000000000000000} {Min-Loc-X 0} {Min-Loc-Y 0} {Min-Loc-Z 0} {Max-Loc-X 0} {Max-Loc-Y 0} {Max-Loc-Z 0}
}
CoreMeasuresEpsilonTest "subtraction 0mm historical-mode=0" 0.0001 $known_output $output $LogFile $ModuleName
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filter_0mm_h ] != -1 } ] $ModuleName "Destroying image $filter_0mm_h"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $subtract_0mm_h ] != -1 } ] $ModuleName "Destroying image $subtract_0mm_h"

## historical mode 4mm filter
set filter_4mm [ b2_hanning-filter2D-pet $TestImageID filter-size= 4.1 historical-mode= 0 ]
set output [ b2_measure_image_mask $all_mask $filter_4mm ]
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filter_4mm ] != -1 } ] $ModuleName "Destroying image $filter_4mm"

set known_output {
{Mean 0.003051757637877} {Mean-Absolute-Deviation 0.006094388315045} {Variance 0.010719849380951} {Standard-Deviation 0.103536705476612} {Skewness 41.866927865325266} {Kurtosis 1931.108074366541587} {Minimum 0.000000000000000} {Maximum 6.250000000000000} {Min-Loc-X 0} {Min-Loc-Y 0} {Min-Loc-Z 0} {Max-Loc-X 15} {Max-Loc-Y 15} {Max-Loc-Z 15}
 }
CoreMeasuresEpsilonTest "Hanning 2.33mm historical-mode=0" 0.0001 $known_output $output $LogFile $ModuleName

## historical mode 0mm filter
set filter_0mm [ b2_hanning-filter2D-pet $TestImageID filter-size= 0 historical-mode= 0 ]
set output [ b2_measure_image_mask $all_mask $filter_0mm ]
set known_output {
{Mean 0.003051757812500} {Mean-Absolute-Deviation 0.006103329360485} {Variance 0.305175781250000} {Standard-Deviation 0.552427172801990} {Skewness 181.002763842861270} {Kurtosis 32761.000183102190931} {Minimum 0.000000000000000} {Maximum 100.000000000000000} {Min-Loc-X 0} {Min-Loc-Y 0} {Min-Loc-Z 0} {Max-Loc-X 15} {Max-Loc-Y 15} {Max-Loc-Z 15}
 }
CoreMeasuresEpsilonTest "Hanning 0mm historical-mode=0" 0.0001 $known_output $output $LogFile $ModuleName
set subtract_0mm  [ b2_subtract_images [ list $filter_0mm $TestImageID ] ]
set output [ b2_measure_image_mask $all_mask $subtract_0mm ]
set known_output {
{Mean 0.000000000000000} {Mean-Absolute-Deviation 0.000000000000000} {Variance 0.000000000000000} {Standard-Deviation 0.000000000000000} {Skewness 0.000000000000000} {Kurtosis 0.000000000000000} {Minimum 0.000000000000000} {Maximum 0.000000000000000} {Min-Loc-X 0} {Min-Loc-Y 0} {Min-Loc-Z 0} {Max-Loc-X 0} {Max-Loc-Y 0} {Max-Loc-Z 0}
}
CoreMeasuresEpsilonTest "subtraction 0mm historical-mode=0" 0.0001 $known_output $output $LogFile $ModuleName
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filter_0mm ] != -1 } ] $ModuleName "Destroying image $filter_0mm"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $subtract_0mm ] != -1 } ] $ModuleName "Destroying image $subtract_0mm"


    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $all_mask ] != -1 } ] $ModuleName "Destroying mask $all_mask"
    return [ StopModule  $LogFile $ModuleName ]
}

