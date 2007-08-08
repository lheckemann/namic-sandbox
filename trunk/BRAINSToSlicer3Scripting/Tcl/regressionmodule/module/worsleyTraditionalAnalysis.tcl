# \author    Greg Harris
# \date        $Date: 2007-05-25 14:16:07 -0500 (Fri, 25 May 2007) $
# \brief    This module tests the "iplTclScripts/iplWorsleyBackwardsCompatible.tcl"
# \fn        proc worsleyTraditionalAnalysis {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# Test the TraditionalWorsleyAnalysis_* commands.
#
# To Do
#------------------------------------------------------------------------
# None
#


proc worsleyTraditionalAnalysis {pathToRegressionDir dateString} {
    set ModuleName "worsleyTraditionalAnalysis"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Test all the old worsley analysis routines"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString ]

########################################
########################################
# Setup test files
########################################
########################################

global wors_file_prefix
set wors_file_prefix "WORS"

set wors_results_dir "${OUTPUT_DIR}/worsTraditional_$dateString"
if {[catch { exec mkdir -p ${wors_results_dir} } squabble] != 0 } {puts "exec failed: $squabble" }

set currfile [open "${wors_results_dir}/mr1.m" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/ANON014_T1.img"
close $currfile

set currfile [open "${wors_results_dir}/mr3.m" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON026/MR3/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${pathToRegressionDir}/petdata/ANON026/MR3/10_ACPC/ANON012_T1.img"
close $currfile

set currfile [open "${wors_results_dir}/mr2.m" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON028/MR2/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${pathToRegressionDir}/petdata/ANON028/MR2/10_ACPC/ANON010_T1.img"
close $currfile

set currfile [open "${wors_results_dir}/avg.s" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "NewWorsley"
puts $currfile "3"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_MR_ANON027.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_MR_ANON026.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_MR_ANON028.raw"
close $currfile

set currfile [open "${wors_results_dir}/1aa.p" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${pathToRegressionDir}/petdata/ANON029/pet4/pcjscon010__447.ima.air16"
puts $currfile "/10_ACPC.mfd"
puts $currfile "/pet.mfd"
puts $currfile "${pathToRegressionDir}/petdata/ANON029/pet4/pcjscon010__447.ima"
puts $currfile "1aa"
puts $currfile "18.33"
close $currfile

set currfile [open "${wors_results_dir}/1fa.p" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/pet1/pcanon______417.imf.air"
puts $currfile "/10_ACPC.mfd"
puts $currfile "/pet.mfd"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/pet1/pcanon______417.imf"
puts $currfile "1fa"
puts $currfile "18.33"
close $currfile

set currfile [open "${wors_results_dir}/1fp.p" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/pet1/pcanon______417.imf.par"
puts $currfile "/10_ACPC.mfd"
puts $currfile "/pet.mfd"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/pet1/pcanon______417.imf"
puts $currfile "1fp"
puts $currfile "18.33"
close $currfile

set currfile [open "${wors_results_dir}/b2_diff1.d" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_pet1_HF_18_PET_1fa.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_pet4_HF_18_PET_1aa.raw"
puts $currfile "1fa-1aa"
close $currfile

set currfile [open "${wors_results_dir}/b2_diff2.d" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_pet1_HF_18_PET_1fa.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_pet1_HF_18_PET_1fp.raw"
puts $currfile "1fa-1fp"
close $currfile

set currfile [open "${wors_results_dir}/b2_diff3.d" w]
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_pet1_HF_18_PET_1fp.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_pet4_HF_18_PET_1aa.raw"
puts $currfile "1fp-1aa"
close $currfile

set currfile [open "${wors_results_dir}/b2_study.t" w]
puts $currfile "${wors_results_dir}"
puts $currfile "IRIX64_NewWorsley_x-x"
puts $currfile "3"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_1fa-1aa_nDIFF_ANON027.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_1fa-1fp_nDIFF_ANON027.raw"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_1fp-1aa_nDIFF_ANON027.raw"
puts $currfile "0"
puts $currfile "0"
close $currfile

set currfile [open "${wors_results_dir}/b2_blob1.b" w]
puts $currfile "${wors_results_dir}"
puts $currfile "NewWorsley"
puts $currfile "3"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${pathToRegressionDir}/petdata/ANON027/MR1/10_ACPC/gene"
puts $currfile "${wors_results_dir}/${wors_file_prefix}_T_IRIX64_NewWorsley_x-x.raw"
puts $currfile "3.61"
puts $currfile "0.1"
puts $currfile "30.0"
close $currfile



########################################
########################################
# Run Tests
########################################
########################################


    set SubTestDes "Worsley MR Test 1"
    set m_list [ TraditionalWorsleyAnalysis_m ${wors_results_dir}/mr1.m ]
    if { [ ReportTestStatus $LogFile  [ expr {$m_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_m from generated mr1.m" [lindex $m_list 0] T1 0.0 255.0 {128 80 128 1} {0.999755859375000 1.536132812500000 1.317138671875000 1.000000000000000} {{Mean 128.912255859374994} {Mean-Absolute-Deviation 63.523863239309321} {Variance 5337.099749608678394} {Standard-Deviation 73.055456672371008} {Skewness 1.289221648694028} {Kurtosis -1.230046278043810} {Minimum 0.000000000000000} {Maximum 255.000000000000000} {Min-Loc-X 18} {Min-Loc-Y 18} {Min-Loc-Z 122} {Max-Loc-X 119} {Max-Loc-Y 65} {Max-Loc-Z 73} {Center-Of-Mass-X 63.822615064951520} {Center-Of-Mass-Y 36.529204090349197} {Center-Of-Mass-Z 62.515970012334655}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $m_list 0 ]
    b2 destroy every transform


    set SubTestDes "Worsley MR Test 2"
    set m_list [ TraditionalWorsleyAnalysis_m ${wors_results_dir}/mr2.m ]
    if { [ ReportTestStatus $LogFile  [ expr {$m_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_m from generated mr2.m" [lindex $m_list 0] T1 0.0 255.0 {128 80 128 1} {1.071166992187500 1.498046875000000 1.388549804687500 1.000000000000000} {{Mean 129.016865539550793} {Mean-Absolute-Deviation 63.557429361092076} {Variance 5338.238270101357557} {Standard-Deviation 73.063248422865499} {Skewness 1.287587058615207} {Kurtosis -1.235638275243394} {Minimum 0.000000000000000} {Maximum 255.000000000000000} {Min-Loc-X 92} {Min-Loc-Y 2} {Min-Loc-Z 98} {Max-Loc-X 127} {Max-Loc-Y 4} {Max-Loc-Z 68} {Center-Of-Mass-X 63.251027642674003} {Center-Of-Mass-Y 37.794127850257475} {Center-Of-Mass-Z 61.604682519532567}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $m_list 0 ]
    b2 destroy every transform


    set SubTestDes "Worsley MR Test 3"
    set m_list [ TraditionalWorsleyAnalysis_m ${wors_results_dir}/mr3.m ]
    if { [ ReportTestStatus $LogFile  [ expr {$m_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_m from generated mr3.m" [lindex $m_list 0] T1 0.0 255.0 {128 80 128 1} {1.047363281250000 1.485351562500000 1.340942382812500 1.000000000000000} {{Mean 129.392368316650391} {Mean-Absolute-Deviation 63.227237471664559} {Variance 5268.839465782996740} {Standard-Deviation 72.586771975222845} {Skewness 1.285119676024224} {Kurtosis -1.242038437545685} {Minimum 0.000000000000000} {Maximum 255.000000000000000} {Min-Loc-X 90} {Min-Loc-Y 1} {Min-Loc-Z 95} {Max-Loc-X 127} {Max-Loc-Y 3} {Max-Loc-Z 68} {Center-Of-Mass-X 63.082550194751192} {Center-Of-Mass-Y 37.196007722180973} {Center-Of-Mass-Z 61.344947069132907}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $m_list 0 ]
    b2 destroy every transform


    set SubTestDes "Worsley 'S' Test"
    set s_list [ TraditionalWorsleyAnalysis_s ${wors_results_dir}/avg.s ]
    if { [ ReportTestStatus $LogFile  [ expr {$s_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_s average image from generated avg.s" [lindex $s_list 0] DIFF 6.333333492279053 255.0 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean 129.107166809545390} {Mean-Absolute-Deviation 54.682639219075419} {Variance 4192.906958086269697} {Standard-Deviation 64.752659853370275} {Skewness 1.357994635256281} {Kurtosis -1.017350805959474} {Minimum 6.333333492279053} {Maximum 255.000000000000000} {Min-Loc-X 122} {Min-Loc-Y 68} {Min-Loc-Z 112} {Max-Loc-X 89} {Max-Loc-Y 6} {Max-Loc-Z 97} {Center-Of-Mass-X 63.384985943702127} {Center-Of-Mass-Y 37.173309325534312} {Center-Of-Mass-Z 61.821216696460091}} $LogFile $ModuleName $SubTestDes 
    
    ExpandedImageTest "TraditionalWorsley_s standard deviation image from generated avg.s" [lindex $s_list 1] DIFF 0.0 146.936492919921875 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean 31.168253617646315} {Mean-Absolute-Deviation 20.950121258323264} {Variance 711.331579939727703} {Standard-Deviation 26.670800136848683} {Skewness 1.805016974472599} {Kurtosis 1.107984740202482} {Minimum 0.000000000000000} {Maximum 146.936492919921875} {Min-Loc-X 107} {Min-Loc-Y 67} {Min-Loc-Z 66} {Max-Loc-X 40} {Max-Loc-Y 7} {Max-Loc-Z 101} {Center-Of-Mass-X 62.883505394994778} {Center-Of-Mass-Y 37.344670399824892} {Center-Of-Mass-Z 63.692928075469261}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $s_list 0 ]
    b2 destroy image [ lindex $s_list 1 ]


    set SubTestDes "Worsley PET Test 1"
    set p_list [ TraditionalWorsleyAnalysis_p ${wors_results_dir}/1fp.p ]
    if { [ ReportTestStatus $LogFile  [ expr {$p_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_p from generated 1fp.p" [lindex $p_list 0] Hanning-FLOW 0.0 94.885848999023438 {128 80 128} {0.999755859375000 1.536132812500000 1.317138671875000} {{Mean 27.650513541756119} {Mean-Absolute-Deviation 22.158528042828614} {Variance 599.534286779983518} {Standard-Deviation 24.485389251142884} {Skewness 1.230594211521699} {Kurtosis -1.343026638070888} {Minimum 0.000000000000000} {Maximum 94.885848999023438} {Min-Loc-X 112} {Min-Loc-Y 64} {Min-Loc-Z 112} {Max-Loc-X 64} {Max-Loc-Y 43} {Max-Loc-Z 27} {Center-Of-Mass-X 64.550460030197812} {Center-Of-Mass-Y 34.344823556910853} {Center-Of-Mass-Z 58.668757654795812}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $p_list 0 ]
    b2 destroy every transform


    set SubTestDes "Worsley PET Test 2"
    set p_list [ TraditionalWorsleyAnalysis_p ${wors_results_dir}/1fa.p ]
    if { [ ReportTestStatus $LogFile  [ expr {$p_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_p from generated 1fa.p" [lindex $p_list 0] Hanning-FLOW 0.0 96.315071105957031 {128 80 128} {0.999755859375000 1.536132812500000 1.317138671875000} {{Mean 27.607716383366459} {Mean-Absolute-Deviation 22.175799692841668} {Variance 600.141416390459426} {Standard-Deviation 24.497783907742747} {Skewness 1.230614995653776} {Kurtosis -1.342215501202753} {Minimum 0.000000000000000} {Maximum 96.315071105957031} {Min-Loc-X 112} {Min-Loc-Y 66} {Min-Loc-Z 112} {Max-Loc-X 66} {Max-Loc-Y 42} {Max-Loc-Z 31} {Center-Of-Mass-X 65.358182630957486} {Center-Of-Mass-Y 34.342113714845517} {Center-Of-Mass-Z 61.813556153103917}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $p_list 0 ]
    b2 destroy every transform


    set SubTestDes "Worsley PET Test 3"
    set p_list [ TraditionalWorsleyAnalysis_p ${wors_results_dir}/1aa.p ]
    if { [ ReportTestStatus $LogFile  [ expr {$p_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_p from generated 1aa.p" [lindex $p_list 0] Hanning-COUNT -217.178802490234375 1540.410766601562500 {128 80 128} {0.999755859375000 1.536132812500000 1.317138671875000} {{Mean 455.641426718162961} {Mean-Absolute-Deviation 323.918589875652344} {Variance 129245.955666987923905} {Standard-Deviation 359.507935471510734} {Skewness 1.223808676794011} {Kurtosis -1.392119305301520} {Minimum -217.178802490234375} {Maximum 1540.410766601562500} {Min-Loc-X 60} {Min-Loc-Y 10} {Min-Loc-Z 0} {Max-Loc-X 65} {Max-Loc-Y 31} {Max-Loc-Z 13} {Center-Of-Mass-X 64.483225182147692} {Center-Of-Mass-Y 34.358648346855162} {Center-Of-Mass-Z 62.952808363622076}} $LogFile $ModuleName $SubTestDes 
    
    b2 destroy image [ lindex $p_list 0 ]
    b2 destroy every transform



    set SubTestDes "Worsley DIFF Test 1"
    set d_list [ TraditionalWorsleyAnalysis_d ${wors_results_dir}/b2_diff1.d ]
    if { [ ReportTestStatus $LogFile  [ expr {$d_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_d DIFF from generated diff1.d" [lindex $d_list 0] FLOW -56.156932830810547 66.580093383789062 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean -0.000000354890653} {Mean-Absolute-Deviation 5.083804551607046} {Variance 85.418028341239321} {Standard-Deviation 9.242187421884459} {Skewness 2.483037513451657} {Kurtosis 4.802668228765014} {Minimum -56.156932830810547} {Maximum 66.580093383789062} {Min-Loc-X 66} {Min-Loc-Y 31} {Min-Loc-Z 6} {Max-Loc-X 66} {Max-Loc-Y 62} {Max-Loc-Z 22}} $LogFile $ModuleName $SubTestDes 
    
    CoreMaskTest "TraditionalWorsley_d mask from generated diff1.d" [ lindex $d_list 1 ] 3 128 80 128 3 1.029999971389771 1.350000023841858 1.480000019073486 $LogFile $ModuleName $SubTestDes
    set output_measures [ b2 measure volume mask [ lindex $d_list 1 ] ]
    puts "$SubTestDes MASKVOLUME $output_measures"
    CoreMeasuresEpsilonTest "TraditionalWorsley_d mask size from generated diff1.d" 0.001 {{CubicCentimeters 1357.762961682011110}} $output_measures $LogFile $ModuleName
    
    b2 destroy image [ lindex $d_list 0 ]
    b2 destroy mask [ lindex $d_list 1 ]


    set SubTestDes "Worsley DIFF Test 2"
    set d_list [ TraditionalWorsleyAnalysis_d ${wors_results_dir}/b2_diff2.d ]
    if { [ ReportTestStatus $LogFile  [ expr {$d_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_d DIFF from generated diff2.d" [lindex $d_list 0] FLOW -33.825290679931641 29.086746215820312 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean -0.000000584367081} {Mean-Absolute-Deviation 3.338537035941679} {Variance 38.859697547853450} {Standard-Deviation 6.233754691023176} {Skewness 2.473901587365669} {Kurtosis 4.230149429424253} {Minimum -33.825290679931641} {Maximum 29.086746215820312} {Min-Loc-X 78} {Min-Loc-Y 17} {Min-Loc-Z 12} {Max-Loc-X 66} {Max-Loc-Y 35} {Max-Loc-Z 73}} $LogFile $ModuleName $SubTestDes 
    
    CoreMaskTest "TraditionalWorsley_d mask from generated diff2.d"  [ lindex $d_list 1 ] 3 128 80 128 3 1.029999971389771 1.350000023841858 1.480000019073486 $LogFile $ModuleName $SubTestDes
    set output_measures [ b2 measure volume mask [ lindex $d_list 1 ] ]
    puts "$SubTestDes MASKVOLUME $output_measures"
    CoreMeasuresEpsilonTest "TraditionalWorsley_d mask size from generated diff2.d" 0.001 {{CubicCentimeters 1348.473420496263770}} $output_measures $LogFile $ModuleName
    
    b2 destroy image [ lindex $d_list 0 ]
    b2 destroy mask [ lindex $d_list 1 ]


    set SubTestDes "Worsley DIFF Test 3"
    set d_list [ TraditionalWorsleyAnalysis_d ${wors_results_dir}/b2_diff3.d ]
    if { [ ReportTestStatus $LogFile  [ expr {$d_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    ExpandedImageTest "TraditionalWorsley_d DIFF from generated diff3.d" [lindex $d_list 0] FLOW -54.134273529052734 78.196975708007812 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean -0.000002143398160} {Mean-Absolute-Deviation 5.781567230810443} {Variance 111.203932001017748} {Standard-Deviation 10.545327496148127} {Skewness 2.453066599463124} {Kurtosis 4.517324279488292} {Minimum -54.134273529052734} {Maximum 78.196975708007812} {Min-Loc-X 98} {Min-Loc-Y 34} {Min-Loc-Z 118} {Max-Loc-X 65} {Max-Loc-Y 63} {Max-Loc-Z 21}} $LogFile $ModuleName $SubTestDes 
    
    CoreMaskTest "TraditionalWorsley_d mask from generated diff3.d"  [ lindex $d_list 1 ] 3 128 80 128 3 1.029999971389771 1.350000023841858 1.480000019073486 $LogFile $ModuleName $SubTestDes
    set output_measures [ b2 measure volume mask [ lindex $d_list 1 ] ]
    puts "$SubTestDes MASKVOLUME $output_measures"
    CoreMeasuresEpsilonTest "TraditionalWorsley_d mask size from generated diff3.d" 0.001 {{CubicCentimeters 1336.535310523191583}} $output_measures $LogFile $ModuleName
    
    b2 destroy image [ lindex $d_list 0 ]
    b2 destroy mask [ lindex $d_list 1 ]


    set SubTestDes "Worsley 'T' Test generation"
    set t_list [ TraditionalWorsleyAnalysis_t ${wors_results_dir}/b2_study.t ]
    if { [ ReportTestStatus $LogFile  [ expr {$t_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set SubTestDes "Worsley 'T' Test corrected T image"
    ExpandedImageTest "TraditionalWorsley_t Worsley-T image from generated b2_study.t" [lindex $t_list 0] DIFF -5.857890605926514 6.903920650482178 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean 0.027535887386072} {Mean-Absolute-Deviation 0.502557786054082} {Variance 0.831747804461994} {Standard-Deviation 0.912002085777217} {Skewness 2.596103490359092} {Kurtosis 5.680181428282761} {Minimum -5.857890605926514} {Maximum 6.903920650482178} {Min-Loc-X 66} {Min-Loc-Y 31} {Min-Loc-Z 6} {Max-Loc-X 66} {Max-Loc-Y 62} {Max-Loc-Z 22} {Center-Of-Mass-X 109.847390796607556} {Center-Of-Mass-Y 55.535038670445189} {Center-Of-Mass-Z 48.083909886244882}} $LogFile $ModuleName $SubTestDes 
    
    set SubTestDes "Worsley 'T' Test UNcorrected PairedDiff-T image"
    ExpandedImageTest "TraditionalWorsley_t pointwise PairedDiff-T image from generated b2_study.t" [lindex $t_list 4] DIFF -126.618103027343750 3.927069902420044 {128 80 128 1} {1.029999971389771 1.350000023841858 1.480000019073486 1.000000000000000} {{Mean 0.110297946558757} {Mean-Absolute-Deviation 0.812646270721163} {Variance 1.803807975649824} {Standard-Deviation 1.343059185460501} {Skewness 2.825630231324550} {Kurtosis 63.685514149530320} {Minimum -126.618103027343750} {Maximum 3.927069902420044} {Min-Loc-X 79} {Min-Loc-Y 25} {Min-Loc-Z 34} {Max-Loc-X 47} {Max-Loc-Y 5} {Max-Loc-Z 78} {Center-Of-Mass-X 80.942407967573672} {Center-Of-Mass-Y 30.165060434359905} {Center-Of-Mass-Z 77.460890991801989}} $LogFile $ModuleName $SubTestDes 

    set SubTestDes "Worsley 'T' Test lambda table"
    set observed_lambda_data [ b2 get table data [ lindex $t_list 2 ] ]
    puts "$SubTestDes LAMBDATABLE $observed_lambda_data "
    set expected_lambda_data {{0.652675628662109 0.059356756508350 0.060146041214466} {0.059356756508350 1.435884594917297 0.081402674317360} {0.060146041214466 0.081402674317360 0.767586052417755} {0.619021356105804 26.238796234130859 5.867800712585449} {0.100762799382210 0.155557766556740 0.132750988006592} {632734.000000000000000 1316.590942382812500 123.002075195312500} {6.903920650482178 0.000000054675770 0.000000000000000} {-5.857890605926514 0.000030917173717 0.000000000000000} {1.799999952316284 11.631477355957031 0.000000000000000} {1.899999976158142 11.263747215270996 0.000000000000000} {2.000000000000000 10.653104782104492 0.000000000000000} {2.099999904632568 9.864589691162109 0.000000000000000} {2.199999809265137 8.959475517272949 0.000000000000000} {2.299999713897705 7.992680549621582 0.000000000000000} {2.399999618530273 7.011048316955566 0.000000000000000} {2.499999523162842 6.052478790283203 0.000000000000000} {2.599999427795410 5.145782947540283 0.000000000000000} {2.699999332427979 4.311135292053223 0.000000000000000} {2.799999237060547 3.560953378677368 0.000000000000000} {2.899999141693115 2.901045799255371 0.000000000000000} {2.999999046325684 2.331898689270020 0.000000000000000} {3.099998950958252 1.849963188171387 0.000000000000000} {3.199998855590820 1.448869585990906 0.000000000000000} {3.299998760223389 1.120490431785583 0.000000000000000} {3.399998664855957 0.855829656124115 0.000000000000000} {3.499998569488525 0.645720303058624 0.000000000000000} {3.599998474121094 0.481336623430252 0.000000000000000} {3.699998378753662 0.354537338018417 0.000000000000000} {3.799998283386230 0.258071094751358 0.000000000000000} {3.899998188018799 0.185665458440781 0.000000000000000} {3.999998092651367 0.132032901048660 0.000000000000000} {4.099997997283936 0.092818312346935 0.000000000000000} {4.199997901916504 0.064509443938732 0.000000000000000} {4.299997806549072 0.044328693300486 0.000000000000000} {4.399997711181641 0.030119651928544 0.000000000000000} {4.499997615814209 0.020237045362592 0.000000000000000} {4.599997520446777 0.013446316123009 0.000000000000000} {4.699997425079346 0.008835736662149 0.000000000000000} {4.799997329711914 0.005742330104113 0.000000000000000} {4.899997234344482 0.003691140096635 0.000000000000000} {4.999997138977051 0.002346820197999 0.000000000000000} {5.099997043609619 0.001475923694670 0.000000000000000} {5.199996948242188 0.000918184174225 0.000000000000000} {5.299996852874756 0.000565059017390 0.000000000000000} {5.399996757507324 0.000344009138644 0.000000000000000} {5.499996662139893 0.000207192046219 0.000000000000000} {5.599996566772461 0.000123457022710 0.000000000000000} {5.699996471405029 0.000072779810580 0.000000000000000} {5.799996376037598 0.000042449202738 0.000000000000000} {5.899996280670166 0.000024496424885 0.000000000000000} {5.999996185302734 0.000013986873455 0.000000000000000} {6.099996089935303 0.000007901930076 0.000000000000000} {6.199995994567871 0.000004417232503 0.000000000000000} {6.299995899200439 0.000002443313178 0.000000000000000} {6.399995803833008 0.000001337302365 0.000000000000000} {6.499995708465576 0.000000724283780 0.000000000000000} {6.599995613098145 0.000000388171856 0.000000000000000} {6.699995517730713 0.000000205865049 0.000000000000000} {6.799995422363281 0.000000108041647 0.000000000000000} {6.899995326995850 0.000000056112061 0.000000000000000}}

    set SubTestDes "Lambda Table number of lines"
    ReportTestStatus $LogFile  [ expr {[llength $expected_lambda_data] == [llength $observed_lambda_data] } ] $ModuleName $SubTestDes

    for {set i 0} {$i < [llength $expected_lambda_data]} {incr i} {
        
        set SubTestDes "Lambda Table length of line $i"
        ReportTestStatus $LogFile  [ expr {[llength [lindex $expected_lambda_data $i]] == [llength [lindex $observed_lambda_data $i]] } ] $ModuleName $SubTestDes
        
        for {set j 0} {$j < [llength [lindex $expected_lambda_data 0]]} {incr j} {
            set SubTestDes "Lambda Table Comparison Location ($i,$j)"
            SingleMeasureEpsilonTest $SubTestDes 0.0001 [lindex [lindex $observed_lambda_data $i] $j] [lindex [lindex $expected_lambda_data $i] $j] $LogFile $ModuleName
        }
    }
    
    set SubTestDes "Resel Size"
    set observed_resel_size [ lindex $t_list 3 ]
    puts "$SubTestDes RESELSIZE $observed_resel_size "
    set expected_resel_size 5.867800805595641
    SingleMeasureEpsilonTest $SubTestDes 0.0001 $observed_resel_size $expected_resel_size $LogFile $ModuleName
    
    b2 destroy image [ lindex $t_list 0 ]
    b2 destroy talairach-parameters [ lindex $t_list 1 ]
    b2 destroy table [ lindex $t_list 2 ]
    b2 destroy image [ lindex $t_list 4 ]


    set SubTestDes "Worsley Blob list Test"
    set b_list [ TraditionalWorsleyAnalysis_b ${wors_results_dir}/b2_blob1.b ]
    if { [ ReportTestStatus $LogFile  [ expr {$b_list != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set blob_filename [ lindex $b_list 2 ]
    set blob_file [ open $blob_filename ]
    set i 0
    while {[gets $blob_file line] >= 0} {
        puts "$line"
        incr i
        if {$i > 23} {
          lappend observed_blob_list "$line"
        }
    }
    puts "BLOBLIST $observed_blob_list"
    set expected_blob_list {{blobnumber 1  } {  extval -5.857891} {  volume 3826.000000 (7873.678223)} {  Extloc 66.000000 31.000000 6.000000 (0.000000 -35.522278 -61.274757)} {  CoMloc 69.000000 27.000000 9.000000 (3.471002 -31.681786 -67.074547)} {blobnumber 2  } {  extval 6.903921} {  volume 3598.000000 (7404.468262)} {  Extloc 66.000000 62.000000 22.000000 (0.000000 -11.593563 -20.593750)} {  CoMloc 65.000000 60.000000 26.000000 (-0.764568 -5.363235 -22.664137)} {blobnumber 3  } {  extval -4.535305} {  volume 290.000000 (596.802612)} {  Extloc 68.000000 43.000000 53.000000 (2.229508 34.699833 -45.510143)} {  CoMloc 67.000000 43.000000 53.000000 (1.414586 34.468220 -45.550919)} {blobnumber 4  } {  extval 3.926273} {  volume 216.000000 (444.515045)} {  Extloc 63.000000 64.000000 60.000000 (-3.090909 45.148026 -17.999998)} {  CoMloc 64.000000 61.000000 60.000000 (-2.084455 45.334599 -21.842590)} {blobnumber 5  } {  extval 4.349037} {  volume 211.000000 (434.225311)} {  Extloc 108.000000 42.000000 60.000000 (46.819672 45.148026 -46.823864)} {  CoMloc 107.000000 42.000000 59.000000 (46.064171 43.471504 -46.251057)} {blobnumber 6  } {  extval -4.015543} {  volume 70.000000 (144.055801)} {  Extloc 103.000000 21.000000 114.000000 (41.245903 125.748360 -74.411934)} {  CoMloc 102.000000 21.000000 113.000000 (40.592972 123.680046 -74.749748)} {blobnumber 7  } {  extval 3.652565} {  volume 23.000000 (47.332619)} {  Extloc 67.000000 64.000000 79.000000 (1.114754 73.507401 -17.999998)} {  CoMloc 66.000000 63.000000 79.000000 (0.533147 73.442505 -19.860735)} {blobnumber 8  } {  extval -4.049503} {  volume 21.000000 (43.216740)} {  Extloc 29.000000 23.000000 118.000000 (-38.121212 131.718750 -71.784492)} {  CoMloc 29.000000 23.000000 117.000000 (-37.630596 130.226151 -72.159851)} {blobnumber 9  } {  extval 3.706249} {  volume 19.000000 (39.100861)} {  Extloc 92.000000 5.000000 78.000000 (28.983606 72.014801 -95.431412)} {  CoMloc 92.000000 5.000000 78.000000 (28.807590 71.307777 -95.431412)} {blobnumber 10  } {  extval -3.717230} {  volume 6.000000 (12.347639)} {  Extloc 36.000000 19.000000 111.000000 (-30.909092 121.270561 -77.039368)} {  CoMloc 36.000000 19.000000 111.000000 (-30.565657 120.773026 -77.039368)} {blobnumber 11  } {  extval -3.716213} {  volume 4.000000 (8.231760)} {  Extloc 33.000000 20.000000 113.000000 (-34.000000 124.255753 -75.725647)} {  CoMloc 33.000000 20.000000 112.000000 (-34.000000 123.509460 -75.725647)} {blobnumber 12  } {  extval -3.615148} {  volume 1.000000 (2.057940)} {  Extloc 35.000000 20.000000 114.000000 (-31.939394 125.748360 -75.725647)} {  CoMloc 35.000000 20.000000 114.000000 (-31.939394 125.748360 -75.725647)} {blobnumber 13  } {  extval -3.624706} {  volume 1.000000 (2.057940)} {  Extloc 30.000000 21.000000 114.000000 (-37.090912 125.748360 -74.411934)} {  CoMloc 30.000000 21.000000 114.000000 (-37.090912 125.748360 -74.411934)} {blobnumber 14  } {  extval -3.662732} {  volume 1.000000 (2.057940)} {  Extloc 31.000000 21.000000 115.000000 (-36.060604 127.240959 -74.411934)} {  CoMloc 31.000000 21.000000 115.000000 (-36.060604 127.240959 -74.411934)} {blobnumber 15  } {  extval -3.612302} {  volume 1.000000 (2.057940)} {  Extloc 38.000000 19.000000 113.000000 (-28.848486 124.255753 -77.039368)} {  CoMloc 38.000000 19.000000 113.000000 (-28.848486 124.255753 -77.039368)} {blobnumber 16  } {  extval -3.678593} {  volume 1.000000 (2.057940)} {  Extloc 32.000000 21.000000 116.000000 (-35.030304 128.733551 -74.411934)} {  CoMloc 32.000000 21.000000 116.000000 (-35.030304 128.733551 -74.411934)} {blobnumber 17  } {  extval -3.655208} {  volume 1.000000 (2.057940)} {  Extloc 105.000000 25.000000 116.000000 (43.475410 128.733551 -69.157059)} {  CoMloc 105.000000 25.000000 116.000000 (43.475410 128.733551 -69.157059)} {blobnumber 18  } {  extval -3.662122} {  volume 1.000000 (2.057940)} {  Extloc 103.000000 25.000000 117.000000 (41.245903 130.226151 -69.157059)} {  CoMloc 103.000000 25.000000 117.000000 (41.245903 130.226151 -69.157059)} {blobnumber 19  } {  extval -3.692624} {  volume 1.000000 (2.057940)} {  Extloc 32.000000 22.000000 118.000000 (-35.030304 131.718750 -73.098213)} {  CoMloc 32.000000 22.000000 118.000000 (-35.030304 131.718750 -73.098213)} {blobnumber 20  } {  extval -3.641990} {  volume 1.000000 (2.057940)} {  Extloc 28.000000 24.000000 118.000000 (-39.151516 131.718750 -70.470779)} {  CoMloc 28.000000 24.000000 118.000000 (-39.151516 131.718750 -70.470779)} {blobnumber 21  } {  extval -3.653581} {  volume 1.000000 (2.057940)} {  Extloc 101.000000 25.000000 118.000000 (39.016392 131.718750 -69.157059)} {  CoMloc 101.000000 25.000000 118.000000 (39.016392 131.718750 -69.157059)} {blobnumber 22  } {  extval -3.659071} {  volume 1.000000 (2.057940)} {  Extloc 33.000000 22.000000 119.000000 (-34.000000 133.211349 -73.098213)} {  CoMloc 33.000000 22.000000 119.000000 (-34.000000 133.211349 -73.098213)} {blobnumber 23  } {  extval -3.628569} {  volume 1.000000 (2.057940)} {  Extloc 29.000000 24.000000 119.000000 (-38.121212 133.211349 -70.470779)} {  CoMloc 29.000000 24.000000 119.000000 (-38.121212 133.211349 -70.470779)} {blobnumber 24  } {  extval -3.619215} {  volume 1.000000 (2.057940)} {  Extloc 99.000000 25.000000 119.000000 (36.786884 133.211349 -69.157059)} {  CoMloc 99.000000 25.000000 119.000000 (36.786884 133.211349 -69.157059)} {blobnumber 25  } {  extval -3.644837} {  volume 1.000000 (2.057940)} {  Extloc 96.000000 34.000000 119.000000 (33.442623 133.211349 -57.333603)} {  CoMloc 96.000000 34.000000 119.000000 (33.442623 133.211349 -57.333603)} {blobnumber 26  } {  extval -3.676967} {  volume 1.000000 (2.057940)} {  Extloc 93.000000 21.000000 120.000000 (30.098362 134.703949 -74.411934)} {  CoMloc 93.000000 21.000000 120.000000 (30.098362 134.703949 -74.411934)} {blobnumber 27  } {  extval -3.610471} {  volume 1.000000 (2.057940)} {  Extloc 94.000000 34.000000 120.000000 (31.213114 134.703949 -57.333603)} {  CoMloc 94.000000 34.000000 120.000000 (31.213114 134.703949 -57.333603)} {blobnumber 28  } {  extval -3.676967} {  volume 1.000000 (2.057940)} {  Extloc 34.000000 23.000000 121.000000 (-32.969696 136.196548 -71.784492)} {  CoMloc 34.000000 23.000000 121.000000 (-32.969696 136.196548 -71.784492)}}

    set SubTestDes "Blob text number of lines"
    ReportTestStatus $LogFile  [ expr {[llength $expected_blob_list] == [llength $observed_blob_list] } ] $ModuleName $SubTestDes

    while { [llength $expected_blob_list] >= 5 } {

        set expected_blobnumber [lindex $expected_blob_list 0]
        set observed_blobnumber [lindex $observed_blob_list 0]
        set expected_extval [lindex $expected_blob_list 1]
        set observed_extval [lindex $observed_blob_list 1]
        set expected_volume [lindex $expected_blob_list 2]
        set observed_volume [lindex $observed_blob_list 2]
        set expected_ExtLoc [lindex $expected_blob_list 3]
        set observed_ExtLoc [lindex $observed_blob_list 3]
        set expected_CoMloc [lindex $expected_blob_list 4]
        set observed_CoMloc [lindex $observed_blob_list 4]

        set expected_blob_list [lrange $expected_blob_list 5 end]
        set observed_blob_list [lrange $observed_blob_list 5 end]


        for {set j 0} {$j < [llength $expected_blobnumber]} {incr j} {
            set obs_lr [string trim [lindex $observed_blobnumber $j] "()"]
            set xpc_lr [string trim [lindex $expected_blobnumber $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "blobnumber keyword --- is '[lindex $observed 1]' == '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr {[lindex $expected_blobnumber $j] == [lindex $observed_blobnumber $j] } ] $ModuleName $SubTestDes
            } else {
                SingleMeasureEpsilonTest "blobnumber" 0.0 [lindex $observed 1] [lindex $expected 1] $LogFile $ModuleName
            }
        }


        for {set j 0} {$j < [llength $expected_extval]} {incr j} {
            set obs_lr [string trim [lindex $observed_extval $j] "()"]
            set xpc_lr [string trim [lindex $expected_extval $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "extval keyword --- is '[lindex $observed 1]' == '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr {[lindex $expected_extval $j] == [lindex $observed_extval $j] } ] $ModuleName $SubTestDes
            } else {
                SingleMeasureEpsilonTest "extval" 0.0001 [lindex $observed 1] [lindex $expected 1] $LogFile $ModuleName
            }
        }


        set j 0
            set obs_lr [string trim [lindex $observed_volume $j] "()"]
            set xpc_lr [string trim [lindex $expected_volume $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "volume keyword --- is '[lindex $observed 1]' == '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr {[lindex $expected_volume $j] == [lindex $observed_volume $j] } ] $ModuleName $SubTestDes
            } else {
                set SubTestDes "volume keyword detection failure: found '[lindex $observed_volume $j]' expected '[lindex $expected_volume $j]'?"
                ReportTestStatus $LogFile  [ expr {0 == 1} ] $ModuleName $SubTestDes
            }
        set j 1
            set obs_lr [string trim [lindex $observed_volume $j] "()"]
            set xpc_lr [string trim [lindex $expected_volume $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "volume keyword detected at j==1: found '[lindex $observed_volume $j]' expected '[lindex $expected_volume $j]'?"
                ReportTestStatus $LogFile  [ expr {0 == 1} ] $ModuleName $SubTestDes
            } else {
                set SubTestDes "volume in pixels --- is '[lindex $observed 1]' within 1 of '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr { abs ( [lindex $expected 1] - [lindex $observed 1] ) <= 1 } ] $ModuleName $SubTestDes

            }
        set j 2
            set voxels_denom [lindex $observed 1]
            set obs_lr [string trim [lindex $observed_volume $j] "()"]
            set xpc_lr [string trim [lindex $expected_volume $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "volume keyword detected at j==2: found '[lindex $observed_volume $j]' expected '[lindex $expected_volume $j]'?"
                ReportTestStatus $LogFile  [ expr {0 == 1} ] $ModuleName $SubTestDes
            } else {
                set criterion 2.06;#[expr { [lindex $expected 1] / $voxels_denom } ]
                set SubTestDes "volume in space units --- is '[lindex $observed 1]' within 1 voxel of '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr { abs ( [lindex $expected 1] - [lindex $observed 1] ) <= $criterion } ] $ModuleName $SubTestDes
            }

        for {set j 0} {$j < 4} {incr j} {
            set obs_lr [string trim [lindex $observed_ExtLoc $j] "()"]
            set xpc_lr [string trim [lindex $expected_ExtLoc $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "ExtLoc keyword --- is '[lindex $observed 1]' == '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr {[lindex $expected_ExtLoc $j] == [lindex $observed_ExtLoc $j] } ] $ModuleName $SubTestDes
            } else {
                set SubTestDes "ExtLoc in voxels --- is '[lindex $observed 1]' within 1 of '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr { abs ( [lindex $expected 1] - [lindex $observed 1] ) <= 1 } ] $ModuleName $SubTestDes
            }
        }
        for {set j 4} {$j < 7} {incr j} {
            set obs_lr [string trim [lindex $observed_ExtLoc $j] "()"]
            set xpc_lr [string trim [lindex $expected_ExtLoc $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "ExtLoc keyword detected at j==${j}: found '[lindex $observed_ExtLoc $j]' expected '[lindex $expected_ExtLoc $j]'?"
                ReportTestStatus $LogFile  [ expr {0 == 1} ] $ModuleName $SubTestDes
            } else {
                set SubTestDes "ExtLoc in space units --- is '[lindex $observed 1]' within 1 voxel of '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr { abs ( [lindex $expected 1] - [lindex $observed 1] ) <= 1015625 } ] $ModuleName $SubTestDes
            }
        }

        
        for {set j 0} {$j < 4} {incr j} {
            set obs_lr [string trim [lindex $observed_CoMloc $j] "()"]
            set xpc_lr [string trim [lindex $expected_CoMloc $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "CoMloc keyword --- is '[lindex $observed 1]' == '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr {[lindex $expected_CoMloc $j] == [lindex $observed_CoMloc $j] } ] $ModuleName $SubTestDes
            } else {
                set SubTestDes "CoMloc in voxels --- is '[lindex $observed 1]' within 1 of '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr { abs ( [lindex $expected 1] - [lindex $observed 1] ) <= 1 } ] $ModuleName $SubTestDes
            }
        }
        for {set j 4} {$j < 7} {incr j} {
            set obs_lr [string trim [lindex $observed_ExtLoc $j] "()"]
            set xpc_lr [string trim [lindex $expected_ExtLoc $j] "()"]
            set observed "expr $obs_lr"
            set expected "expr $xpc_lr"
            if { [catch {eval $observed}] == 1  ||  [catch {eval $expected}] == 1} {
                set SubTestDes "CoMloc keyword detected at j==${j}: found '[lindex $observed_CoMloc $j]' expected '[lindex $expected_CoMloc $j]'?"
                ReportTestStatus $LogFile  [ expr {0 == 1} ] $ModuleName $SubTestDes
            } else {
                set SubTestDes "CoMloc in space units --- is '[lindex $observed 1]' within 1 voxel of '[lindex $expected 1]'?"
                ReportTestStatus $LogFile  [ expr { abs ( [lindex $expected 1] - [lindex $observed 1] ) <= 1015625 } ] $ModuleName $SubTestDes
            }
        }

        
    }


    
    b2 destroy blob [ lindex $b_list 0 ]



    return [ StopModule  $LogFile $ModuleName ]
}

