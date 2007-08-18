# \author       Hans J. Johnson"
# \date         3/19/2002
# \brief        b2 measure volume
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
proc measureVolume { pathToRegressionDir dateString } {
        set ModuleName "measureVolume"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Testing of 'b2 measure volume \[roi|mask\]"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#       close $LogFile
#       set LogFile stderr

# Run Tests
############ load prerequisite files ###################

#Clean all images, masks, and ROI's to ensure masks id's start at 0
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every mask ] != -1 } ] $ModuleName "Destroying every mask"
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every roi ] != -1 } ] $ModuleName "Destroying every roi"
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every image ] != -1 } ] $ModuleName "Destroying every image"
##ReportTestStatus $LogFile  [ expr { [ b2_destroy_every gtsurface ] != -1 } ] $ModuleName "Destroying every gtsurface"

    set SubTestDes "Load requisite objects for volume creation - brainMask"
      set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$brainMask != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating volume(s).\nFailed to load mask."
      }

    set SubTestDes "Load requisite objects for volume creation - cranRoi"
      set cranRoi [b2_convert_mask_to_roi Coronal $brainMask  ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$cranRoi != -1 } ] $ModuleName $SubTestDes ]} {
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
set KnownValues {
{CubicCentimeters 1482.303900241851807}
}

set output [ b2_measure_volume_mask $brainMask ]
puts $output
CoreMeasuresEpsilonTest "volume mask" 0.0001 $KnownValues $output $LogFile $ModuleName

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

set KnownValues2  {
{CubicCentimeters 1482.303900241851807}
}
set output2 [ b2_measure_volume_roi $cranRoi ]
puts $output2
CoreMeasuresEpsilonTest "volume roi" 0.0001 $KnownValues2 $output2 $LogFile $ModuleName

set KnownValues3 {
{CubicCentimeters 41.051693916320801}
}
set output3 [ b2_measure_volume_roi $cranRoi  min= 100 max= 102 ]
puts $output3
CoreMeasuresEpsilonTest "volume roi in slice range" 0.0001 $KnownValues3 $output3 $LogFile $ModuleName

# Clean up Files
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainMask ] != -1 } ] $ModuleName "Destroying mask $brainMask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $cranRoi ] != -1 } ] $ModuleName "Destroying roi $cranRoi"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask [lindex [lindex $output2 1] 1] ] != -1 } ] $ModuleName "Destroying mask [lindex [lindex $output2 1] 1]"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask [lindex [lindex $output3 1] 1] ] != -1 } ] $ModuleName "Destroying mask [lindex [lindex $output3 1] 1]"


# Free memory
    return [ StopModule  $LogFile $ModuleName ]
      return $MODULE_SUCCESS
}

