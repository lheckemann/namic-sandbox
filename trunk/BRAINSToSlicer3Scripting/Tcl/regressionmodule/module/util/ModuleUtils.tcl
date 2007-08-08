# \author  Hans J. Johnson
# \date    3/14/2002
# \brief  Provides a common procedure for starting a module
# \result  Places result into file
# \fn    proc StartModule {} 
# \param ModuleName        - The name of the module from which the test was run
# \param ModuleAuthor     - The developer that wrote the module
# \param ModuleDescription   - A description of the test that was run
# \return An open file for writing


global MODULE_STATUS
global MODULE_SUCCESS
global MODULE_FAILURE

set MODULE_SUCCESS 0
set MODULE_FAILURE 1

# \author  Hans J. Johnson
# \date    3/14/2002
# \brief  Provides a common report format for all tests
# \fn    proc ReportTestStatus {} 
# \param OutputFile - A previously opened file
# \param DidTestSuccede - Query of whether to print REGR_ERROR: or REGR_PASSED:  
#                         NOTE: 
# \param ModuleName    - The name of the module from which the test was run
# \param SubTestDescription - A description of the test that was run
# \result  Places result into file
# \retult returns the value of DidTestSuccede
proc ReportTestStatus { OutputFile DidTestSuccede  ModuleName SubTestDescription } {
  global MODULE_STATUS;
  global MODULE_SUCCESS;
  global MODULE_FAILURE;
#  set timeflag [exec date];
  set timeflag "";
  if { $DidTestSuccede  == 1} {
    puts $OutputFile "REGR_PASSED: $timeflag $ModuleName : $SubTestDescription";
  } else {
    puts $OutputFile "-REGR_ERROR: $timeflag $ModuleName : $SubTestDescription";
#    set MODULE_STATUS [ eval $MODULE_STATUS+$MODULE_FAILURE ];
    set MODULE_STATUS $MODULE_FAILURE;
  }
  return $DidTestSuccede;
}

proc StartModule { ModuleName ModuleAuthor ModuleDescription dateString } {
    global env
    global MODULE_STATUS
    global MODULE_SUCCESS;
    global MODULE_FAILURE;
    set MODULE_STATUS $MODULE_SUCCESS
if { 0 == 1 } {
    if {([info exist env(ARCH)] == 0) || ([info exist env(ABI)] == 0)} {
        puts "SETUP_ERROR: Regression test $ModuleName. Enviromental variables ARCH and ABI must be set"
        set env(ARCH) "UNKNOWN_ARCH"
        set env(ABI) "UNKNOWN_ABI"
        puts "SETUP_ERROR: Enviromental variables ARCH and ABI set to $env(ARCH) and $env(ABI)."
        #return $MODULE_FAILURE
    }
    if {[file exists $env(ARCH)] == 0} {
        exec mkdir $env(ARCH)
    }

    if {[file exists $env(ARCH)/$env(ABI)] == 0} {
        exec mkdir $env(ARCH)/$env(ABI)
    }

    set errorFlag [catch {set LogFile [open $env(ARCH)/$env(ABI)/${ModuleName}_$dateString w]}]
    fconfigure $LogFile -buffering line
    if {$errorFlag != 0} {
        puts "SETUP_ERROR: Failed to open module ($ModuleName) output file."
        return $MODULE_FAILURE
    }
}
#Make sure we have a clean slate, but do not destroy displays
    DestroyAllObjects

    set LogFile stdout
    puts $LogFile "Starting Regression Test Module - $ModuleName"
    puts $LogFile "Regression Test Author - $ModuleAuthor"
    puts $LogFile "Regression Purpose: $ModuleDescription"
    puts -nonewline  $LogFile "Starting Time "
    puts $LogFile [exec date]
    return $LogFile
}


# \author  Hans J. Johnson
# \date    3/14/2002
# \brief  Provides a common procedure for stopping a module
# \result  Places result into file
# \fn    proc StopModule {} 
# \param LogFile        - The open file to write in
# \param ModuleName     - The name of the module from which the test was run
# \return     - nothing
proc StopModule { LogFile ModuleName } {
    global MODULE_STATUS
    global MODULE_SUCCESS;
    global MODULE_FAILURE;
    puts "------------------------------------------------------------------------------"
    puts "------------------------------------------------------------------------------"
    puts "------------------------------------------------------------------------------"
    puts "------------------------------------------------------------------------------"

    set objecttypes { image  roi  mask  gtsurface  transform talairach-parameters  talairach-box  histogram  landmark  palette  table  tissue-class  blob }
    foreach {currentobjtype} $objecttypes  {
          set objectlist [ b2 object-list $currentobjtype ]
          set SubTestDescription "List of $currentobjtype destroyed implicitly destroyed: $objectlist"
          puts "$SubTestDescription  length  [llength $objectlist ]"
          ReportTestStatus  $LogFile  [expr { [llength $objectlist ] == 0 }]  $ModuleName $SubTestDescription
          b2 destroy every $currentobjtype
    }

    puts -nonewline  $LogFile "Ending Time "
    puts $LogFile [exec date]
    puts $LogFile "Regression Test Module - FaultDetectionStatus_$MODULE_STATUS - $ModuleName Completed"
    # We no longer close the module log file because we refactored it to always be stdout.
    # close $LogFile
    return $MODULE_STATUS
}


# \author    Hans J. Johnson
# \date     $Date: 2006-10-12 16:55:01 -0500 (Thu, 12 Oct 2006) $
# \brief    This module tests common components of loading any image file format, i.e., if it
#           is a valid image, then these items must be testable:
# \fn        proc CoreImageTest {}
# \params  They should be self expanatory
# \result    1 in case testing is complete or 0 in case of a fatal error
proc CoreImageTest { ImageTypeName TestImageID ImageType ImageMin ImageMax Dimensions Resolutions LogFile ModuleName SubTestDes} {
    global MODULE_SUCCESS ;
    global MODULE_FAILURE ;

        set lentype [llength  $ImageType] ;
        set testimageMin [b2 image min $TestImageID] ;
        set testimageMax [b2 image max $TestImageID] ;
        set Epsilon 0.001 ;
        # Epsilon has the same value as in ExpandedImageTest for calling CoreMeasuresEpsilonTest

        for {set currtype 0} { $currtype < $lentype } {incr currtype} {
            set type [b2 get image type $TestImageID $currtype] ;
            set SubTestDes "$ImageTypeName type($currtype): does [lindex $ImageType $currtype] equal $type" ;
            ReportTestStatus $LogFile  [ expr  {$type == [lindex $ImageType $currtype]}   ] $ModuleName $SubTestDes ;

            set Criterion [expr { abs([lindex $ImageMin $currtype] * $Epsilon) }] ;
            if { $Criterion < $Epsilon } { set Criterion $Epsilon }
            set SubTestDes "$ImageTypeName $ImageType image min($currtype): does new ([lindex $testimageMin $currtype]) approximate known ([lindex $ImageMin $currtype]) to within $Epsilon (comparing [expr { abs([lindex $testimageMin $currtype] - [lindex $ImageMin $currtype]) }] difference against scaled error threshold $Criterion)" ;
            ReportTestStatus $LogFile  [ expr { abs([lindex $testimageMin $currtype] - [lindex $ImageMin $currtype]) <=  $Criterion } ] $ModuleName $SubTestDes ;

            set Criterion [expr { abs([lindex $ImageMax $currtype] * $Epsilon) }] ;
            if { $Criterion < $Epsilon } { set Criterion $Epsilon }
            set SubTestDes "$ImageTypeName $ImageType image max($currtype): does new ([lindex $testimageMax $currtype]) approximate known ([lindex $ImageMax $currtype]) to within $Epsilon (comparing [expr { abs([lindex $testimageMax $currtype] - [lindex $ImageMax $currtype]) }] difference against scaled error threshold $Criterion)" ;
            ReportTestStatus $LogFile  [ expr { abs([lindex $testimageMax $currtype] - [lindex $ImageMax $currtype]) <=  $Criterion } ] $ModuleName $SubTestDes ;
        }

        set dims [b2 get dims image $TestImageID] ;
        puts "dims is $dims" ;
        set lendims [llength $Dimensions] ;
        set SubTestDes "$ImageTypeName $ImageType  num dims image: does [llength $dims] equal $lendims" ;
        ReportTestStatus $LogFile  [ expr { [llength $dims] == $lendims } ] $ModuleName $SubTestDes ;

        for {set currdim 0} { $currdim < $lendims } {incr currdim} {
            set SubTestDes "$ImageTypeName $ImageType dim($currdim): does [lindex $dims $currdim] equal [lindex $Dimensions $currdim]" ;
            ReportTestStatus $LogFile  [ expr  {[lindex $dims $currdim] == [lindex $Dimensions $currdim]}   ] $ModuleName $SubTestDes ;
        }

        set res [b2 get res image $TestImageID] ;
        puts "res is $res" ;
        set lenres [llength $Resolutions] ;
        set SubTestDes "$ImageTypeName $ImageType num res image: does [llength $res] equal $lenres" ;
        ReportTestStatus $LogFile  [ expr  {[llength $res] == $lenres}   ] $ModuleName $SubTestDes ;

        for {set currres 0} { $currres < $lenres } {incr currres} {
            set SubTestDes "$ImageTypeName $ImageType res($currres): does [lindex $res $currres] equal [lindex $Resolutions $currres]" ;
            ReportTestStatus $LogFile  [ expr  {[lindex $res $currres] == [lindex $Resolutions $currres]}   ] $ModuleName $SubTestDes ;
        }
        return $MODULE_SUCCESS
}


# \author    Hans J. Johnson
# \date     $Date: 2006-10-12 16:55:01 -0500 (Thu, 12 Oct 2006) $
# \brief    This module tests common components of loading any image file format, i.e., if it
#           is a valid image, then these items must be testable:
# \fn        proc ExpandedImageTest {}
# \result    1 in case testing is complete or 0 in case of a fatal error
proc ExpandedImageTest { ImageTypeName TestImageID ImageType ImageMin ImageMax Dimensions Resolutions KnownPixelValues LogFile ModuleName SubTestDes} {
    global MODULE_SUCCESS
    global MODULE_FAILURE

    CoreImageTest $ImageTypeName $TestImageID $ImageType $ImageMin $ImageMax $Dimensions $Resolutions  $LogFile $ModuleName $SubTestDes

    set FullMask [ b2 threshold image $TestImageID 0.0 absolute-value= True ]
    set output_measures [ b2 measure image mask $FullMask $TestImageID ]
    puts "$SubTestDes PIXELVALUES $output_measures"

    set Epsilon 0.001
    # Epsilon has the same value as in CoreImageTest for testing max and min.

    CoreMeasuresEpsilonTest $SubTestDes $Epsilon $KnownPixelValues $output_measures  $LogFile $ModuleName

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $FullMask ] != -1 } ] $ModuleName "Destroying FullMask $FullMask"

    return $MODULE_SUCCESS
}

# \author    Hans J. Johnson
# \date     $Date: 2006-10-12 16:55:01 -0500 (Thu, 12 Oct 2006) $
# \brief    This module tests common components of loading any mask file format, i.e., if it 
#           is a valid image, then these items must be testable:
# \fn        proc CoreMaskTest {}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc CoreMaskTest { TestMaskName TestMaskID NumDims XDims YDims ZDims NumRes XRes YRes ZRes LogFile ModuleName SubTestDes} {
    global MODULE_SUCCESS
    global MODULE_FAILURE
        set dims [b2 get dims mask $TestMaskID]
        set SubTestDes "$TestMaskName dims mask: does [llength $dims] equal $NumDims"
        ReportTestStatus $LogFile  [ expr {[llength $dims] == $NumDims} ] $ModuleName $SubTestDes

        set SubTestDes "$TestMaskName dim(0): does [lindex $dims 0] equal $XDims"
        ReportTestStatus $LogFile  [ expr  {[lindex $dims 0] == $XDims}   ] $ModuleName $SubTestDes

        set SubTestDes "$TestMaskName dim(1): does [lindex $dims 1] equal $YDims"
        ReportTestStatus $LogFile  [ expr  {[lindex $dims 1] == $YDims}   ] $ModuleName $SubTestDes

        set SubTestDes "$TestMaskName dim(2): does [lindex $dims 2] equal $ZDims"
        ReportTestStatus $LogFile  [ expr  {[lindex $dims 2] == $ZDims}   ] $ModuleName $SubTestDes

        set res [b2 get res mask $TestMaskID]
        set SubTestDes "$TestMaskName res mask: does [llength $res] equal $NumRes"
        ReportTestStatus $LogFile  [ expr  {[llength $res] == $NumRes}   ] $ModuleName $SubTestDes

        set SubTestDes "$TestMaskName res(0): does [lindex $res 0] equal $XRes"
        ReportTestStatus $LogFile  [ expr  {[lindex $res 0] == $XRes}   ] $ModuleName $SubTestDes

        set SubTestDes "$TestMaskName res(1): does [lindex $res 1] equal $YRes"
        ReportTestStatus $LogFile  [ expr  {[lindex $res 1] == $YRes}   ] $ModuleName $SubTestDes

        set SubTestDes "$TestMaskName res(2): does [lindex $res 2] equal $ZRes"
        ReportTestStatus $LogFile  [ expr  {[lindex $res 2] == $ZRes}   ] $ModuleName $SubTestDes
    return $MODULE_SUCCESS
}



# \author   Greg Harris
# \date     $Date: 2006-10-12 16:55:01 -0500 (Thu, 12 Oct 2006) $
# \brief    This module tests members of a b2 list of measures against known values.
# \fn        proc CoreMeasuresEpsilonTest {}
# \result    1 
proc CoreMeasuresEpsilonTest { TestMeasuresName Epsilon KnownMeasuresList ObservedMeasuresList LogFile ModuleName} {
    global MODULE_SUCCESS
    global MODULE_FAILURE

        set SubTestDes "$TestMeasuresName preliminaries - KnownMeasures provided"
        ReportTestStatus $LogFile  [ expr {$KnownMeasuresList != -1} ] $ModuleName $SubTestDes

        set SubTestDes "$TestMeasuresName preliminaries - ObservedMeasures provided"
        ReportTestStatus $LogFile  [ expr {$ObservedMeasuresList != -1} ] $ModuleName $SubTestDes

        set SubTestDes "$TestMeasuresName preliminaries - Measures are lists of appropriate length"
        ReportTestStatus $LogFile  [ expr {[llength $KnownMeasuresList] <= [llength $ObservedMeasuresList]} ] $ModuleName $SubTestDes

        array set KnownValueArray [ join $KnownMeasuresList ]
        array set ObservedValueArray [ join $ObservedMeasuresList ]

        foreach {key} [array names KnownValueArray] {
      set Criterion [expr { abs($KnownValueArray($key) * $Epsilon) } ]
      if { $Criterion < $Epsilon } { set Criterion $Epsilon }
      set SubTestDes "Comparing to known $TestMeasuresName measurements for $key, does new($ObservedValueArray($key)) approximate known($KnownValueArray($key))  to within $Epsilon (comparing [expr { abs($ObservedValueArray($key) - $KnownValueArray($key)) }] difference against scaled error threshold $Criterion )"
            ReportTestStatus $LogFile  [ expr { abs($ObservedValueArray($key) - $KnownValueArray($key)) <= $Criterion  } ] $ModuleName $SubTestDes

        }

    return $MODULE_SUCCESS
}

# \author   Greg Harris
# \date     $Date: 2006-10-12 16:55:01 -0500 (Thu, 12 Oct 2006) $
# \brief    This module tests one measure against a known value.
# \fn        proc SingleMeasureEpsilonTest {}
# \result    the value of the test status expr.
proc SingleMeasureEpsilonTest { TestMeasuresName Epsilon ObservedValue KnownValue LogFile ModuleName} {
    set Criterion [expr { abs($KnownValue * $Epsilon) }]
    if { $Criterion < $Epsilon } { set Criterion $Epsilon }
    set SubTestDes "Comparing to known $TestMeasuresName measurement, does new($ObservedValue) approximate known($KnownValue)  to within $Epsilon (comparing [expr { abs($ObservedValue - $KnownValue) }] difference against scaled error threshold $Criterion)"
    return [ReportTestStatus $LogFile  [ expr { abs($ObservedValue - $KnownValue) <=  $Criterion } ] $ModuleName $SubTestDes]
}


# \author   Greg Harris
# \date     $Date: 2006-10-12 16:55:01 -0500 (Thu, 12 Oct 2006) $
# \brief    This module computes sum of squares of an error image.
# \fn        proc SumSquared  {}
# \result    the sum of squares.
proc RootSumSquared { DifferenceImage {minY 0}} {
    set DiffSquaredImage [b2 multiply images [list $DifferenceImage $DifferenceImage]]
    set EveryVoxelMask [b2 threshold image $DiffSquaredImage 0 absolute-value= true]
    if {$minY != 0} {
        set UpperVoxelMask [b2 split mask $EveryVoxelMask y $minY +]
        b2 destroy mask $EveryVoxelMask
    } else {
        set UpperVoxelMask $EveryVoxelMask
    }
    set meas_vol [b2 measure volume mask $UpperVoxelMask]
    set meas_tbl [b2 measure Image Mask $UpperVoxelMask $DiffSquaredImage]
    b2 destroy image $DiffSquaredImage
    b2 destroy mask $UpperVoxelMask
    set Volume [lindex [lindex $meas_vol 0] 1]
    set Mean [lindex [lindex $meas_tbl 0] 1]
    return [expr sqrt($Volume * $Mean)]
}

