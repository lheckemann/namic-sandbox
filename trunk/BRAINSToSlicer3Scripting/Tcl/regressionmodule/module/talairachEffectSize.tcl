# \author        Greg Harris"
# \date
# \brief        b2 compute effect-size image
# \fn                proc talairachEffectSize {pathToRegressionDir dateString}
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------
# b2 convert Talairach-Box to Mask tested simply.
#
# To Do
#------------------------------------------------------------------------
#
# To Test the Test
# -----------------------------------------------------------------------


proc talairachEffectSize {pathToRegressionDir dateString} {
    set ModuleName "talairachEffectSize"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 compute effect-size image"
    global MODULE_SUCCESS
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr



# Run Tests


#####################################
#####################################

    set pd1 [b2 load image ${pathToRegressionDir}/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
    set t21 [b2 load image ${pathToRegressionDir}/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr]
    set tal1 [b2 load Talairach-Parameters ${pathToRegressionDir}/SGI/MR/4x-B1/TEST/10_ACPC/talairach_parameters]
    set tx1 [b2 convert talairach-parameters to talairach-transform $tal1]
    set tx2 [b2 convert talairach-parameters to talairach-transform $tal1]
    b2 set transform $tx1 image $pd1
    b2 set transform $tx2 image $t21

    set pd2 [b2 load image ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_pd_fit.hdr]
    set t22 [b2 load image ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/ANON0009_10_t2_fit.hdr]
    set tal2 [b2 load Talairach-Parameters ${pathToRegressionDir}/SGI/MR/5x-B2/TEST/10_ACPC/Talairach.bnd]
    set tx3 [b2 convert talairach-parameters to talairach-transform $tal2]
    set tx4 [b2 convert talairach-parameters to talairach-transform $tal2]
    b2 set transform $tx3 image $pd2
    b2 set transform $tx4 image $t22

    set pd3 [b2 load image ${pathToRegressionDir}/SGI/MR/B2-Classify/TEST/10_ACPC/ANON0006_10_pd_fit.hdr]
    set t23 [b2 load image ${pathToRegressionDir}/SGI/MR/B2-Classify/TEST/10_ACPC/ANON0006_10_t2_fit.hdr]
    set tal3 [b2 load Talairach-Parameters ${pathToRegressionDir}/SGI/MR/B2-Classify/TEST/10_ACPC/Talairach.bnd]
    set tx5 [b2 convert talairach-parameters to talairach-transform $tal3]
    set tx6 [b2 convert talairach-parameters to talairach-transform $tal3]
    b2 set transform $tx5 image $pd3
    b2 set transform $tx6 image $t23

    set k7 [b2 create bounded-image $pd1 25 145 45 155 25 175]
    set k8 [b2 create bounded-image $t21 25 145 45 155 25 175]
    set k9 [b2 create bounded-image $pd2 25 145 45 155 25 175]
    set k10 [b2 create bounded-image $t22 25 145 45 155 25 175]
    set k11 [b2 create bounded-image $pd3 25 145 45 155 25 175]
    set k12 [b2 create bounded-image $t23 25 145 45 155 25 175]
    set i7 [lindex $k7 0]
    set i8 [lindex $k8 0]
    set i9 [lindex $k9 0]
    set i10 [lindex $k10 0]
    set i11 [lindex $k11 0]
    set i12 [lindex $k12 0]
    set tx7 [lindex $k7 1]
    set tx8 [lindex $k8 1]
    set tx9 [lindex $k9 1]
    set tx10 [lindex $k10 1]
    set tx11 [lindex $k11 1]
    set tx12 [lindex $k12 1]
    b2 set transform -1 image $i7
    b2 set transform -1 image $i8
    b2 set transform -1 image $i9
    b2 set transform -1 image $i10
    b2 set transform -1 image $i11
    b2 set transform -1 image $i12

    set pdStats [b2 stat images [list $i7 $i9 $i11] requests= [list Mean StandardDeviation] data-type= F32]
    set t2Stats [b2 stat images [list $i8 $i10 $i12] requests= [list Mean StandardDeviation] data-type= F32]

    array set pdStatsArray [ join $pdStats ]
    array set t2StatsArray [ join $t2Stats ]

    set i17 [b2 compute effect-size image $pdStatsArray(Mean) $pdStatsArray(StandardDeviation) $t2StatsArray(Mean) $t2StatsArray(StandardDeviation) n1= 3 n2= 3 study= PD_T2]
    set SubTestDes "b2 compute effect-size image should return an image"
    if { [ ReportTestStatus $LogFile  [ expr {$i17 != -1 } ] $ModuleName $SubTestDes] == 0} {
        return $MODULE_FAILURE
    }

    set i18 [b2 sum images [list $i7 $i8 $i9 $i10 $i11 $i12]]
    set m1 [b2 threshold image $i18 700.0]
    set observedMeasureImageOutput [b2 measure Image Mask $m1 $i17]
    set knownMeasureImageOutput {
    {Mean 4.938280774178279} {Mean-Absolute-Deviation 2.623791703872172} {Variance 11.290034141946530} {Standard-Deviation 3.360064603835249} {Skewness 1.990342155330241} {Kurtosis 4.171264422969362} {Minimum -0.723182320594788} {Maximum 70.148056030273438}
    }
    set SubTestDes "Computed effect-size"
    puts "$SubTestDes observedMeasureImageOutput $observedMeasureImageOutput"
    CoreMeasuresEpsilonTest "Computed effect-size image ImageMeasures" 0.001 $knownMeasureImageOutput $observedMeasureImageOutput $LogFile $ModuleName
    set observedMeasureVolumeOutput [b2 measure volume mask m1]
    set knownMeasureVolumeOutput {
    {CubicCentimeters 1258.722999998452678}
    }
    puts "$SubTestDes observedMeasureVolumeOutput $observedMeasureVolumeOutput"
    CoreMeasuresEpsilonTest "Computed effect-size image ThresholdVolumeMeasures" 0.001 $knownMeasureVolumeOutput $observedMeasureVolumeOutput $LogFile $ModuleName

    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters ${tal1} ] != -1 } ] $ModuleName "Destroying talairach-parameters ${tal1}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters ${tal2} ] != -1 } ] $ModuleName "Destroying talairach-parameters ${tal2}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters ${tal3} ] != -1 } ] $ModuleName "Destroying talairach-parameters ${tal3}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${pd1} ] != -1 } ] $ModuleName "Destroying image ${pd1}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${pd2} ] != -1 } ] $ModuleName "Destroying image ${pd2}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${pd3} ] != -1 } ] $ModuleName "Destroying image ${pd3}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${t21} ] != -1 } ] $ModuleName "Destroying image ${t21}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${t22} ] != -1 } ] $ModuleName "Destroying image ${t22}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${t23} ] != -1 } ] $ModuleName "Destroying image ${t23}"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx1} ] != -1 } ] $ModuleName "Destroying transform ${tx1}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx2} ] != -1 } ] $ModuleName "Destroying transform ${tx2}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx3} ] != -1 } ] $ModuleName "Destroying transform ${tx3}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx4} ] != -1 } ] $ModuleName "Destroying transform ${tx4}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx5} ] != -1 } ] $ModuleName "Destroying transform ${tx5}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx6} ] != -1 } ] $ModuleName "Destroying transform ${tx6}"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i7} ] != -1 } ] $ModuleName "Destroying image ${i7}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i8} ] != -1 } ] $ModuleName "Destroying image ${i8}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i9} ] != -1 } ] $ModuleName "Destroying image ${i9}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i10} ] != -1 } ] $ModuleName "Destroying image ${i10}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i11} ] != -1 } ] $ModuleName "Destroying image ${i11}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i12} ] != -1 } ] $ModuleName "Destroying image ${i12}"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx7} ] != -1 } ] $ModuleName "Destroying transform ${tx7}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx8} ] != -1 } ] $ModuleName "Destroying transform ${tx8}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx9} ] != -1 } ] $ModuleName "Destroying transform ${tx9}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx10} ] != -1 } ] $ModuleName "Destroying transform ${tx10}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx11} ] != -1 } ] $ModuleName "Destroying transform ${tx11}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy transform ${tx12} ] != -1 } ] $ModuleName "Destroying transform ${tx12}"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $pdStatsArray(Mean) ] != -1 } ] $ModuleName "Destroying image $pdStatsArray(Mean)"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $pdStatsArray(StandardDeviation) ] != -1 } ] $ModuleName "Destroying image $pdStatsArray(StandardDeviation)"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $t2StatsArray(Mean) ] != -1 } ] $ModuleName "Destroying image $t2StatsArray(Mean)"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image $t2StatsArray(StandardDeviation) ] != -1 } ] $ModuleName "Destroying image $t2StatsArray(StandardDeviation)"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i17} ] != -1 } ] $ModuleName "Destroying image ${i17}"
    ReportTestStatus $LogFile  [ expr { [ b2 destroy image ${i18} ] != -1 } ] $ModuleName "Destroying image ${i18}"

    ReportTestStatus $LogFile  [ expr { [ b2 destroy mask ${m1} ] != -1 } ] $ModuleName "Destroying mask ${m1}"
#####################################
#####################################


    return [ StopModule  $LogFile $ModuleName ]
}

