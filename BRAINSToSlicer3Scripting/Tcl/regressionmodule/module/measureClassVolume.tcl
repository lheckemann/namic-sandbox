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
proc measureClassVolume { pathToRegressionDir dateString } {
        set ModuleName "measureClassVolume"
        set ModuleAuthor "Hans J. Johnson"
        set ModuleDescription "Testing of b2_measure_class-volume_roi mask"
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

    set SubTestDes "Load requisite objects for volume creation - stereo image"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_stereo.hdr]
      if { ! [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error reading image.\nFailed to load Image."
      }
    set SubTestDes "Load requisite objects for volume creation - brainMask"
      set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment.mask ]
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
set KnownValues  {
{Grey_Matter_CubicCentimeters 778.069473274558163} {White_Matter_CubicCentimeters 388.632925347560615} {CSF_CubicCentimeters 295.586888945129090} {Blood_CubicCentimeters 18.646432399749756} {No_Class_CubicCentimeters 1.368180274963379}
}
set output1 [ b2_measure_class-volume_mask $brainMask $TestImageID]
CoreMeasuresEpsilonTest "volume mask" 0.0001 $KnownValues $output1 $LogFile $ModuleName

set KnownValues2 {
{Grey_Matter_CubicCentimeters 778.069473274558163} {White_Matter_CubicCentimeters 388.632925347560615} {CSF_CubicCentimeters 295.586888945129090} {Blood_CubicCentimeters 18.646432399749756} {No_Class_CubicCentimeters 1.368180274963379}
}
set output2 [ b2_measure_class-volume_roi $cranRoi $TestImageID]
CoreMeasuresEpsilonTest "volume roi" 0.0001 $KnownValues2 $output2 $LogFile $ModuleName

set KnownValues3 {
{Grey_Matter_CubicCentimeters 20.115907951195862} {White_Matter_CubicCentimeters 11.260546199480618} {CSF_CubicCentimeters 9.375622951984404} {Blood_CubicCentimeters 0.284950256347656} {No_Class_CubicCentimeters 0.014666557312012}
}
set output3 [ b2_measure_class-volume_roi $cranRoi $TestImageID min= 100 max= 102 ]
CoreMeasuresEpsilonTest "volume roi in slice range" 0.0001 $KnownValues3 $output3 $LogFile $ModuleName

set KnownValues4 {
{Intensity 108.000000000000000}
}
set output4 [ b2_get_image_value $TestImageID 100 100 100 ]
CoreMeasuresEpsilonTest "interpolate a voxel dead-on" 0.0001 $KnownValues4 $output4 $LogFile $ModuleName

set KnownValues5 {
{Intensity 132.000000000000000}
}
set output5 [ b2_get_image_value $TestImageID 100 100 99 ]
CoreMeasuresEpsilonTest "interpolate a voxel dead-on" 0.0001 $KnownValues5 $output5 $LogFile $ModuleName

set KnownValues6 {
{Intensity 120.000000000000000}
}
set KnownValues6a [list [list Intensity [expr ([lindex [lindex $KnownValues4 0] 1] + [lindex [lindex $KnownValues5 0] 1])/2.0]]]
set output6 [ b2_get_image_value $TestImageID 100 100 99.5 ]
CoreMeasuresEpsilonTest "interpolate half way between two voxels" 0.0001 $KnownValues6 $output6 $LogFile $ModuleName
CoreMeasuresEpsilonTest "interpolate half way between two voxels (computed)" 0.0001 $KnownValues6a $output6 $LogFile $ModuleName

set KnownValues7 {
{Millimeters 627.656250000000000} {Slice 102}
}
set output7 [ b2_measure_roi_perimeter $cranRoi 102 ]
CoreMeasuresEpsilonTest "measure roi perimeter" 0.0001 $KnownValues7 $output7 $LogFile $ModuleName

set KnownValues8 {
{Millimeters 81847.187500000000000}
}
set output8 [ b2_measure_roi_perimeter $cranRoi -1 ]
CoreMeasuresEpsilonTest "measure roi perimeter for whole roi" 0.0001 $KnownValues8 $output8 $LogFile $ModuleName
set SubTestDes "Converting slice '-1' to the label ALL in 'b2_measure_roi_perimeter' output"
ReportTestStatus $LogFile  [ expr {[string equal [lindex [lindex $output8 1] 1] "ALL"] == 1 } ] $ModuleName $SubTestDes

set KnownValues9 {
{xMin 55} {xMax 198} {yMin 61} {yMax 210} {zMin 12} {zMax 172}
}
set output9 [ b2_measure_bounds_mask $brainMask ]
CoreMeasuresEpsilonTest "measure bounds mask" 0.0001 $KnownValues9 $output9 $LogFile $ModuleName

set KnownValues10 {
{Resel_Size 86.258941650390625} {Num_Resels 17184.349609375000000}
}
set output10 [ b2_measure_resel-size_mask $brainMask $TestImageID ]
CoreMeasuresEpsilonTest "measure resel-size mask" 0.0001 $KnownValues10 $output10 $LogFile $ModuleName




puts $output1
puts $output2
puts $output3
puts $output4
puts $output5
puts $output6
puts $output7
puts $output8
puts $output9
puts $output10
# Clean up Files
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainMask ] != -1 } ] $ModuleName "Destroying mask $brainMask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_roi $cranRoi ] != -1 } ] $ModuleName "Destroying roi $cranRoi"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask [lindex [lindex $output2 5] 1] ] != -1 } ] $ModuleName "Destroying mask 1"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask [lindex [lindex $output3 5] 1] ] != -1 } ] $ModuleName "Destroying mask 2"


# Free memory
    return [ StopModule  $LogFile $ModuleName ]
      return $MODULE_SUCCESS
}

