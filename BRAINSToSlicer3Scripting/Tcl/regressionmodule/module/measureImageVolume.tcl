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
proc measureImageVolume { pathToRegressionDir dateString } {
        set ModuleName "measureImageVolume"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Testing of b2_measure_image_roi mask"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#        close $LogFile
#        set LogFile stderr

# Run Tests
############ load prerequisite files ###################

#Clean all images, masks, and ROI's to ensure masks id's start at 0
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every mask ] != -1 } ] $ModuleName "Destroying every mask"
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every roi ] != -1 } ] $ModuleName "Destroying every roi"
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every image ] != -1 } ] $ModuleName "Destroying every image"
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every gtsurface ] != -1 } ] $ModuleName "Destroying every gtsurface"

    set SubTestDes "Load requisite objects for volume creation - stereo image"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_stereo.hdr]
      if { ! [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error reading image.\nFailed to load Image."
      }
    set SubTestDes "Load requisite objects for volume creation - brainMask"
      set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$brainMask != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating volume(s).\nFailed to load mask."
      }

    set SubTestDes "Load requisite objects for volume creation - hemRoi"
      set hemRoi [b2_convert_mask_to_roi Coronal $brainMask  ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$hemRoi != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating volume(s).\nFailed to create ROI from mask."
      }

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

        set output [ b2_measure_image_mask $brainMask $TestImageID]
          set standards {
{Mean 135.789818910665275} {Mean-Absolute-Deviation 57.342890663550520} {Variance 4419.842171388293536} {Standard-Deviation 66.481893560489794} {Skewness 1.327814335331516} {Kurtosis -1.087230908271656} {Minimum 0.000000000000000} {Maximum 250.000000000000000}
    }
          CoreMeasuresEpsilonTest "1. volume mask" 0.0000001 $standards $output  $LogFile $ModuleName

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

        set output2 [ b2_measure_image_roi $hemRoi $TestImageID]
          set standards {
{Mean 135.789818910665275} {Mean-Absolute-Deviation 57.342890663550520} {Variance 4419.842171388293536} {Standard-Deviation 66.481893560489794} {Skewness 1.327814335331516} {Kurtosis -1.087230908271656} {Minimum 0.000000000000000} {Maximum 250.000000000000000}
    }
          CoreMeasuresEpsilonTest "2. volume roi" 0.0000001 $standards $output2  $LogFile $ModuleName

        set output3 [ b2_measure_image_roi $hemRoi $TestImageID min= 100 max= 102 ]
          set standards {
{Mean 134.568034502118110} {Mean-Absolute-Deviation 61.430583814302743} {Variance 4917.855228276749585} {Standard-Deviation 70.127421371933750} {Skewness 1.289339479935582} {Kurtosis -1.215440156558832} {Minimum 0.000000000000000} {Maximum 250.000000000000000}
    }
          CoreMeasuresEpsilonTest "3. slice range volume roi" 0.0000001 $standards $output3  $LogFile $ModuleName


#######################################################################################################################
#######################################################################################################################
        set output4 [ b2_measure_image_mask $brainMask $TestImageID functional-flag= true]
          set standards {
{Mean 135.915269967905346} {Mean-Absolute-Deviation 57.277133636247079} {Variance 4406.874772253224364} {Standard-Deviation 66.384296126819208} {Skewness 1.326921776548464} {Kurtosis -1.090300604489888} {Minimum 1.000000000000000} {Maximum 250.000000000000000}
    }
          CoreMeasuresEpsilonTest "4. volume mask" 0.0000001 $standards $output4  $LogFile $ModuleName

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

        set output5 [ b2_measure_image_roi $hemRoi $TestImageID functional-flag= true]
          set standards {
{Mean 135.915269967905346} {Mean-Absolute-Deviation 57.277133636247079} {Variance 4406.874772253224364} {Standard-Deviation 66.384296126819208} {Skewness 1.326921776548464} {Kurtosis -1.090300604489888} {Minimum 1.000000000000000} {Maximum 250.000000000000000}
    }
          CoreMeasuresEpsilonTest "5. volume roi" 0.0000001 $standards $output5  $LogFile $ModuleName

        set output6 [ b2_measure_image_roi $hemRoi $TestImageID min= 100 max= 102 functional-flag= true ]
          set standards {
{Mean 134.616128867558473} {Mean-Absolute-Deviation 61.406752840571443} {Variance 4913.138462887955029} {Standard-Deviation 70.093783339808070} {Skewness 1.289126746293943} {Kurtosis -1.216125671585208} {Minimum 1.000000000000000} {Maximum 250.000000000000000}
    }
          CoreMeasuresEpsilonTest "6. slice range volume roi" 0.0000001 $standards $output6  $LogFile $ModuleName

puts $output
puts $output2
puts $output3
puts $output4
puts $output5
puts $output6

# Clean up Files
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainMask ] != -1 } ] $ModuleName "Destroying mask $brainMask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $hemRoi ] != -1 } ] $ModuleName "Destroying roi $hemRoi"
    array set ValueArray2 [ join $output2 ]
    array set ValueArray3 [ join $output3 ]
    array set ValueArray5 [ join $output5 ]
    array set ValueArray6 [ join $output6 ]
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $ValueArray2(MaskHandle) ] != -1 } ] $ModuleName "Destroying mask $ValueArray2(MaskHandle)"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $ValueArray3(MaskHandle) ] != -1 } ] $ModuleName "Destroying mask $ValueArray3(MaskHandle)"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $ValueArray5(MaskHandle) ] != -1 } ] $ModuleName "Destroying mask $ValueArray5(MaskHandle)"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $ValueArray6(MaskHandle) ] != -1 } ] $ModuleName "Destroying mask $ValueArray6(MaskHandle)"


# Free memory
    return [ StopModule  $LogFile $ModuleName ]
      return $MODULE_SUCCESS
}

