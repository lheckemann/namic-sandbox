# \author    Greg Harris
# \date        $Date: 2005-07-30 14:38:45 -0500 (Sat, 30 Jul 2005) $
# \brief    This module tests the "b2_stat_images"
# \fn        proc GetImageValue { Image xloc yloc zloc } 
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the b2_stat_images command
#
# To Do
#------------------------------------------------------------------------
# None
#

####
proc GetImageValue { ImageList xloc yloc zloc } {
    set dim [ b2_get_dims_image [ lindex $ImageList 0] ];
    set dim0 [ lindex $dim 0 ];
    set dim1 [ lindex $dim 1 ];
    set dim2 [ lindex $dim 2 ];
    set res [ b2_get_res_image [ lindex $ImageList 0] ];
    set res0 [ lindex $res 0 ];
    set res1 [ lindex $res 1 ];
    set res2 [ lindex $res 2 ];
    set bullet [b2_create_bullet-image 1 0 $dim0 $dim1 $dim2 $res0 $res1 $res2 $xloc $yloc $zloc 1 1 1];
    set bmask [ b2_threshold_image $bullet 1];

    foreach currImage $ImageList {
        set stats [ b2_measure_image_mask $bmask $currImage ];
        array set stats_list [ join $stats ];
        lappend values $stats_list(Mean);
    }
    b2_destroy_image $bullet;
    b2_destroy_mask $bmask;
    return $values;
}

proc statImages {pathToRegressionDir dateString} {


    set ModuleName "statImages"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test the b2_stat_images command"
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


    ############################### Generate a Set of Images and Nested Masks ###########################

    set images ""
    set offvalues ""
    set values ""
    set Masks ""
    set bg 0
    set dims 40
    set off_center [expr $dims/3 ]
    set center [expr $dims/2 ]
    foreach scales [list 1 1.5 2 2.5 3 3.5 4 4.5 5 5.5 6 ] {
        set size [ expr $dims/$scales ]
        set fg [expr 255/$scales]
        set zero_corner [expr $center-$size/2 ]
        set currImage [b2_create_bullet-image $fg $bg $dims $dims $dims 1.0 1.0 1.0 $zero_corner $zero_corner $zero_corner $size $size $size ]
        set SubTestDes "stat images - create phantom image test"
        if { [ ReportTestStatus $LogFile  [ expr {$currImage  != -1 } ] $ModuleName $SubTestDes] == 0} {
            continue
        }
        lappend values [ GetImageValue $currImage $center $center $center ]
        if { $zero_corner <= $off_center } {
           lappend offvalues [ GetImageValue $currImage $off_center $off_center $off_center ]
        }
        set currMask [ b2_threshold_image $currImage 1 ]
        set SubTestDes "stat images - create phantom mask test"
        if { [ ReportTestStatus $LogFile  [ expr {$currMask  != -1 } ] $ModuleName $SubTestDes] == 0} {
            continue
        }
        lappend Masks $currMask
        lappend Images $currImage
    }

    set SubTestDes "stat images - create enough phantom images and masks test"
    if { [ ReportTestStatus $LogFile  [ expr { [llength $Images] == 11 && [llength $Masks] == 11 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }


    # First, test for invalid arguments
    set SubTestDes "required argument test"
    set errorTest [b2_stat_images]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "argument number test"
    set errorTest [b2_stat_images $Images junk= ]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "optional argument test"
    set errorTest [b2_stat_images $Images junk= test]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid image test"
    set errorTest [b2_stat_images 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid mask test (1)"
    set errorTest [b2_stat_images [lindex $Images 0] supplied-masks= 65536]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "invalid mask test (11)"
    set errorTest [b2_stat_images $Images supplied-masks= "65536 [lrange $Masks 1 end]"]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "wrong number of masks test"
    set errorTest [b2_stat_images $Images supplied-masks= [lrange $Masks 1 end]]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes

    set SubTestDes "no requests test"
    set errorTest [b2_stat_images $Images supplied-masks= $Masks]
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes



    ############################ BRAINS2 stat images -- Mean thru T-test #####################################
    set stat_results_list [ b2_stat_images $Images supplied-masks= $Masks requests= {Mean Variance StandardDeviation Skewness Kurtosis Cardinality PairedDifferenceT PairedDifferenceTProbability } ]
    puts "++++++++++++++++++++++++++++++++++++++++++++++++ 'b2_stat_images' produced:"
    puts "$stat_results_list"
    set SubTestDes "stat images - generate statistical results test Long Test"
    if { [ ReportTestStatus $LogFile  [ expr {$stat_results_list != -1 } ] $ModuleName $SubTestDes] != 0} {
        array set stat_results [ join $stat_results_list ]
        set mean_diff_image       $stat_results(Mean) ;
        set variance_diff_image   $stat_results(Variance) ;
        set stddev_diff_image     $stat_results(StandardDeviation) ;
        set skewness_diff_image   $stat_results(Skewness) ;
        set kurtosis_diff_image   $stat_results(Kurtosis) ;
        set num_diffs_image       $stat_results(Cardinality) ;
        set simple_T_image        $stat_results(PairedDifferenceT) ;
        set simple_TProb_image    $stat_results(PairedDifferenceTProbability) ;

        set n_center [ GetImageValue ${num_diffs_image} $center $center $center ]
        set computedMean_center [ GetImageValue ${mean_diff_image} $center $center $center ]
        set computedVariance_center [ GetImageValue ${variance_diff_image} $center $center $center ]
        set computedSkewness_center [ GetImageValue ${skewness_diff_image} $center $center $center ]
        set computedKurtosis_center [ GetImageValue ${kurtosis_diff_image} $center $center $center ]
        set computedStdDev_center [ GetImageValue ${stddev_diff_image} $center $center $center ]
        set computedT_center [ GetImageValue ${simple_T_image} $center $center $center ]
        set computedTProb_center [ GetImageValue ${simple_TProb_image} $center $center $center ]

        set n_offcenter [ GetImageValue ${num_diffs_image}  $off_center $off_center $off_center ]
        set computedMean_offcenter [ GetImageValue ${mean_diff_image} $off_center $off_center $off_center ]
        set computedVariance_offcenter [ GetImageValue ${variance_diff_image} $off_center $off_center $off_center ]
        set computedSkewness_offcenter [ GetImageValue ${skewness_diff_image} $off_center $off_center $off_center ]
        set computedKurtosis_offcenter [ GetImageValue ${kurtosis_diff_image} $off_center $off_center $off_center ]
        set computedStdDev_offcenter [ GetImageValue ${stddev_diff_image} $off_center $off_center $off_center  ]
        set computedT_offcenter [ GetImageValue ${simple_T_image} $off_center $off_center $off_center ]
        set computedTProb_offcenter [ GetImageValue ${simple_TProb_image} $off_center $off_center $off_center ]

        puts "Run the following through octave"
        puts ""
        puts "offvalues=\[$offvalues\]"
        puts "\[p,t,df\]=t_test(offvalues,0)"
        puts "m=mean(offvalues);"
        puts "v=var(offvalues);"
        puts "s=sqrt(v);"
        puts "sk=skewness(offvalues);"
        puts "kt=kurtosis(offvalues);"
        puts "printf(\"set p %18.15f\\n\", p);"
        puts "printf(\"set t %18.15f\\n\", t);"
        puts "printf(\"set m %18.15f\\n\", m);"
        puts "printf(\"set v %18.15f\\n\", v);"
        puts "printf(\"set s %18.15f\\n\", s);"
        puts "printf(\"set sk %18.15f\\n\", sk);"
        puts "printf(\"set kt %18.15f\\n\", kt);"
        puts ""

        # The following was from octave:
        set p  0.016547946793734
        set t  4.869400899430360
        set m 163.500000000000000
        set v 4509.666666666666970
        set s 67.154051751675169
        set sk  0.400455829562467
        set kt -1.940646797864555
        set df 3

        set statStandard "[list Cardinality [expr $df + 1]] [list t $t] [list p $p] [list Mean $m] [list Variance $v] [list StandardDeviation $s] [list Skewness $sk] [list Kurtosis $kt]"
        set statResults "[list Cardinality $n_offcenter] [list t $computedT_offcenter] [list p $computedTProb_offcenter] [list Mean $computedMean_offcenter] [list Variance $computedVariance_offcenter] [list StandardDeviation $computedStdDev_offcenter] [list Skewness $computedSkewness_offcenter] [list Kurtosis $computedKurtosis_offcenter]"
        CoreMeasuresEpsilonTest "off-center" 0.00001 $statStandard $statResults $LogFile $ModuleName

        puts "Run the following through octave"
        puts ""
        puts "values=\[$values\]"
        puts "\[p,t,df\]=t_test(values,0)"
        puts "m=mean(values);"
        puts "v=var(values);"
        puts "s=sqrt(v);"
        puts "sk=skewness(values);"
        puts "kt=kurtosis(values);"
        puts "printf(\"set p %18.15f\\n\", p);"
        puts "printf(\"set t %18.15f\\n\", t);"
        puts "printf(\"set m %18.15f\\n\", m);"
        puts "printf(\"set v %18.15f\\n\", v);"
        puts "printf(\"set s %18.15f\\n\", s);"
        puts "printf(\"set sk %18.15f\\n\", sk);"
        puts "printf(\"set kt %18.15f\\n\", kt);"
        puts ""

        # The following was from octave:
        set p  0.000586924178886
        set t  4.940282653256181
        set m 97.181818181818187
        set v 4256.563636363635851
        set s 65.242345423533294
        set sk  1.248955290491261
        set kt  0.384786916633718
        set df 10

        set statStandard "[list Cardinality [expr $df + 1]] [list t $t] [list p $p] [list Mean $m] [list Variance $v] [list StandardDeviation $s] [list Skewness $sk] [list Kurtosis $kt]"
        set statResults "[list Cardinality $n_center] [list t $computedT_center] [list p $computedTProb_center] [list Mean $computedMean_center] [list Variance $computedVariance_center] [list StandardDeviation $computedStdDev_center] [list Skewness $computedSkewness_center] [list Kurtosis $computedKurtosis_center]"
        CoreMeasuresEpsilonTest "on-center" 0.00001 $statStandard $statResults $LogFile $ModuleName

        ############################ Free up the result images ########################################

        foreach LabelImagePair $stat_results_list {
            set TestImageId [lindex $LabelImagePair 1]
            ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"
        }
    }


    ############################ BRAINS2 stat images -- Min, Max, and Median #################################
    set stat_results_list [ b2_stat_images $Images supplied-masks= $Masks requests= { Median Min Max } ]
    puts "++++++++++++++++++++++++++++++++++++++++++++++++ 'b2_stat_images' produced:"
    puts "$stat_results_list"
    set SubTestDes "stat images - generate statistical results test Short Test"
    if { [ ReportTestStatus $LogFile  [ expr {$stat_results_list != -1 } ] $ModuleName $SubTestDes] != 0} {
        array set stat_results [ join $stat_results_list ]
        set median_diff_image   $stat_results(Median) ;
        set min_diff_image      $stat_results(Minimum) ;
        set max_diff_image     $stat_results(Maximum) ;

        set computedMedian_center [ GetImageValue ${median_diff_image} $center $center $center ]
        set computedMin_center [ GetImageValue ${min_diff_image} $center $center $center ]
        set computedMax_center [ GetImageValue ${max_diff_image} $center $center $center ]

        set computedMedian_offcenter [ GetImageValue ${median_diff_image} $off_center $off_center $off_center ]
        set computedMin_offcenter [ GetImageValue ${min_diff_image} $off_center $off_center $off_center ]
        set computedMax_offcenter [ GetImageValue ${max_diff_image} $off_center $off_center $off_center ]


        puts "Run the following through octave"
        puts ""
        puts "offvalues=\[$offvalues\]"
        puts "m=median(offvalues)"
        puts "l=min(offvalues)"
        puts "u=max(offvalues)"
        puts ""

        # The following was from octave:
        set m 148.50
        set l 102
        set u 255

        set statStandard "[list Median $m] [list LowerBound $l] [list UpperBound $u]"
        set statResults "[list Median $computedMedian_offcenter] [list LowerBound $computedMin_offcenter] [list UpperBound $computedMax_offcenter]"
        CoreMeasuresEpsilonTest "off-center" 0.0000 $statStandard $statResults $LogFile $ModuleName

        puts "Run the following through octave"
        puts ""
        puts "values=\[$values\]"
        puts "m=median(values)"
        puts "l=min(values)"
        puts "u=max(values)"
        puts ""

        # The following was from octave:
        set m 72
        set l 42
        set u 255

        set statStandard "[list Median $m] [list LowerBound $l] [list UpperBound $u]"
        set statResults "[list Median $computedMedian_center] [list LowerBound $computedMin_center] [list UpperBound $computedMax_center]"
        CoreMeasuresEpsilonTest "on-center" 0.0000 $statStandard $statResults $LogFile $ModuleName

        ############################ Free up the result images ########################################

        foreach LabelImagePair $stat_results_list {
            set TestImageId [lindex $LabelImagePair 1]
            ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"
        }
    }


    ############################ Free up the test images and masks ########################################

    foreach TestMaskId $Masks {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskId ] != -1 } ] $ModuleName "Destroying mask $TestMaskId"
    }
    foreach TestImageId $Images {
        ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageId ] != -1 } ] $ModuleName "Destroying image $TestImageId"
    }


    return [ StopModule  $LogFile $ModuleName ]

#    return $MODULE_SUCCESS
}

