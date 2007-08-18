# \author       Hans J. Johnson"
# \date         3/19/2002
# \brief        b2 measure surface
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
proc measureSurface { pathToRegressionDir dateString } {
    global env
        set ModuleName "measureSurface"
        set ModuleAuthor "Greg Harris"
        set ModuleDescription "Testing of b2 measure surface"
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#        close $LogFile
#        set LogFile stderr

# Run Tests
############ load prerequisite files ###################

        set SubTestDes "Load requisite objects for surface measurement- left surface"
        set surfacel [b2 load surface $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_l_dec35.surf ]
        if { ! [ ReportTestStatus $LogFile  [ expr {$surfacel != -1 } ] $ModuleName $SubTestDes ]} {
            puts "Error creating surface(s).\nFailed to load surface)."
        }
    set SubTestDes "Load requisite objects for surface measurement- right surface"
      set surfacer [b2 load surface $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_r_dec35.surf ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$surfacer != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating surface(s).\nFailed to load surface)."
      }
    set SubTestDes "Load requisite objects for surface creation - brainMask"
      set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/cran_mask.segment ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$brainMask != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating surface(s).\nFailed to load mask."
      }

    set SubTestDes "Load requisite objects for surface creation - hemRoi"
      set hemRoi [b2_convert_mask_to_roi Axial $brainMask  ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$hemRoi != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating surface(s).\nFailed to create ROI from mask."
      }

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
array set KnownValueArray [ join  {
{FundalSumArea 98226.826646}
{FundalSumCurvature -29216281.029012}
{FundalMeanCurvature -297.436882}
{FundalSumSqCurvature 12270399877.612782}
{FundalVarianceCurvature 36450.331056}
{FundalStdDevCurvature 190.919698}
{GyralSumArea 106348.740424}
{GyralSumCurvature 31422406.605846}
{GyralMeanCurvature 295.465715}
{GyralSumSqCurvature 12867511322.691416}
{GyralVarianceCurvature 33693.558385}
{GyralStdDevCurvature 183.558052}
{TotalSumArea 204575.567070}
{TotalSumCurvature 2206125.576834}
{TotalMeanCurvature 10.783915}
{TotalSumSqCurvature 25137911200.304199}
{TotalVarianceCurvature 122762.072171}
{TotalStdDevCurvature 350.374189}
{FundalSumPartialArea 98189.784820}
{FundalSumDepth 184768.742877}
{FundalMeanDepth 1.881751}
{FundalSumSqDepth 1043706.942894}
{FundalVarianceDepth 7.088499}
{FundalStdDevDepth 2.662423}
{GyralSumPartialArea 106294.213582}
{GyralSumDepth 293863.159143}
{GyralMeanDepth 2.764620}
{GyralSumSqDepth 2356276.131612}
{GyralVarianceDepth 14.524366}
{GyralStdDevDepth 3.811085}
{TotalSumPartialArea 204483.998402}
{TotalSumDepth 478631.902020}
{TotalMeanDepth 2.340681}
{TotalSumSqDepth 3399983.074506}
{TotalVarianceDepth 11.148345}
{TotalStdDevDepth 3.338914}
 } ]

set output [ b2 measure surface mask $brainMask $surfacel $surfacer ]
array set ValueArray [ join $output ]
foreach {key} [array names KnownValueArray] {
          set SubTestDes "Comparing to surface measurements for $key, does known($KnownValueArray($key))  nearly equal  new($ValueArray($key))  "
          ReportTestStatus $LogFile  [ expr { abs($KnownValueArray($key) - $ValueArray($key)) / abs($KnownValueArray($key)) < 0.0001 } ] $ModuleName $SubTestDes
}

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

set output2 [ b2 measure surface roi $hemRoi $surfacel $surfacer ]
array set ValueArray2 [ join $output2 ]
foreach {key} [array names KnownValueArray] {
          set SubTestDes "Comparing to surface measurements for $key, does known($KnownValueArray($key))  nearly equal  new($ValueArray2($key)),  "
          ReportTestStatus $LogFile  [ expr { abs($KnownValueArray($key) - $ValueArray2($key)) / abs($KnownValueArray($key)) < 0.0001 } ] $ModuleName $SubTestDes
}

# Clean up Files
    b2 destroy surface $surfacel
    b2 destroy surface $surfacer
    b2_destroy_mask  $brainMask
#HACK!!!  This should probably be cleaned up in the code itself
    b2_destroy_mask  [expr $brainMask + 1]
    b2_destroy_roi $hemRoi


# Free memory

    StopModule $LogFile $ModuleName
      return $MODULE_SUCCESS
}

