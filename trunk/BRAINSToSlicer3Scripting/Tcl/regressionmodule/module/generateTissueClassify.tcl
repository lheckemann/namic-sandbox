# \author        Hans J. Johnson"
# \date         4/25/2002
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result        1 in case testing is complete or 0 in case of a fatal error
#
# Test Performed
# -----------------------------------------------------------------------

#
#        genPlugs            - Have Script generate class plugs
#        genModel            - Have Script generate Tissue class plugs
#        applyModel          - Have Script generate classified image
#        t1                  - T1 image filename
#        t2                  - T2 image filename
#        pd                  - PD image filename
#        pickTrainRegion     - Region for training class plugs Talairach box or Mask
#        talairach           - Talairach parameters
#        talairachBox        - Talairach Box for plug picking
#        regionMask          - Mask for plug picking
#        vbMethod            - Venous blood method
#        vb1                 - Venous blood traces
#        vb2                 - Venous blood traces allows for both left and right
#        vbModeImg           - Image in which to pick venous blood
#        random              - Random seed for plug picking
#        coverage            - Coverage in the image for plugs
#        permissiveness      - Permissiveness of the plug picking
#        numPlugs            - NUmber of plugs toi be choosen at a time
#        numGrey             - Number of grey plugs
#        numWhite            - Number of white plugs
#        numCsf              - Number of csf plugs
#        meanOutlier         - Mean outlier for plugs
#        varOutlier          - Variance outlier for plugs
#        plugSize            - Plug size in mm
#        numXpart            - Number of paritions in X
#        numYpart            - Number of paritions in Y
#        numZpart            - Number of paritions in Z
#        greyMaskName        - Grey mask plug filename
#        whiteMaskName       - White mask plug filename
#        csfMaskName         - Csf mask plug filename
#        bloodMaskName       - Blood mask plug filename
#        trimGross           - Trim gross
#        trimSpatial         - Trim spatial
#        histEqual           - Histogram equalization
#        spatial_X           - Spatial X model
#        spatial_Y           - Spatial Y model
#        spatial_Z           - Spatial Z model
#        spatial_XX          - Spatial XY model
#        spatial_YY          - Spatial YY model
#        spatial_ZZ          - Spatial XX model
#        spatial_XY          - Spatial XY model
#        spatial_XZ          - Spatial XZ model
#        spatial_YZ          - Spatial YZ model
#        modelName           - Model filename
#        excludeMask         - T1 image exclusion mask
#        bgbwVals            - Basal threshold method
#        bg                  - BG ROI filename
#        bw                  - BW ROI filename
#        threshold           - Basal threshold value
#        segmentImage        - Tissue classified image filename
proc testbrains2TissueClassify {genPlugs genModel applyModel t1 t2 pd pickTrainRegion talairach talairachBox regionMask \
                            vbMethod vb1 vb2 vbModeImg random coverage permissiveness numPlugs numGrey numWhite numCsf \
                            meanOutlier varOutlier plugSize numXpart numYpart numZpart \
                            brainMaskName greyMaskName whiteMaskName csfMaskName bloodMaskName \
                            trimGross trimSpatial histEqual spatial_X spatial_Y spatial_Z spatial_XX spatial_YY spatial_ZZ \
                            spatial_XY spatial_XZ spatial_YZ modelName excludeMask bgbwVals bg bw threshold segmentImage \
                            LogFile ModuleName} {

    global B2_IPL_FLAG
    global classModel classImage brainMask
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE

#   Plug means:  g-w-c-b   X  t1-t2-pd
#    centers[1][1] = 80.186111;
#    centers[1][2] = 117.773056;
#    centers[1][3] = 173.143585;

#    centers[2][1] = 113.979065;
#    centers[2][2] = 84.270554;
#    centers[2][3] = 140.521729;

#    centers[3][1] = 30.087282;
#    centers[3][2] = 222.857208;
#    centers[3][3] = 190.218933;

#    centers[4][1] = 63.392307;
#    centers[4][2] = 10.019822;
#    centers[4][3] = 40.471096;

    if {$t1 != "."} {
        set T1 [b2 load image $t1]
        set SubTestDes "load T1 image $t1"
        ReportTestStatus $LogFile  [ expr {$T1 != -1 } ] $ModuleName $SubTestDes
        lappend imageList $T1
        lappend imageNameList "T1"
        lappend meanRankTable "80.186111 113.979065 30.087282 10.8"
    }

    if {$t2 != "."} {
        set T2 [b2 load image $t2]
        set SubTestDes "load T2 image $t2"
        ReportTestStatus $LogFile  [ expr {$T2 != -1 } ] $ModuleName $SubTestDes
        lappend imageList $T2
        lappend imageNameList "T2"
        lappend meanRankTable "117.773056 84.270554 222.857208 68.8"
    }

    if {$pd != "."} {
        set PD [b2 load image $pd]
        set SubTestDes "load PD image $pd"
        ReportTestStatus $LogFile  [ expr {$PD != -1 } ] $ModuleName $SubTestDes
        lappend imageList $PD
        lappend imageNameList "PD"
        lappend meanRankTable "173.143585 140.521729 190.218933 47.0"
    }

    if {$genPlugs == 1} {
        if {$pickTrainRegion == "box"} {
            set talPar [b2 load talairach-parameters $talairach]
        set SubTestDes "load Talairach parameters $talairach."
        ReportTestStatus $LogFile  [ expr {$talPar != -1 } ] $ModuleName $SubTestDes
            set brainBox [b2 load talairach-box $talairachBox]
            set brainMask [b2 convert talairach-box to mask $talPar $brainBox]
            ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $talPar ] != -1 } ] $ModuleName "Destroying talairach-parameters $talPar"
            ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-box $brainBox ] != -1 } ] $ModuleName "Destroying talairach-box $brainBox"
        } else {
            set brainMask [b2 load mask $regionMask]
        }
        set sResult [b2 save mask $brainMaskName brains2 $brainMask]
        if {$sResult == -1} {
        }


        if {$vbMethod == "traces"} {
            if {$vb1 != "."} {
                set vbRoi1 [b2 load roi $vb1]
                set vbMask1 [b2 convert roi to mask $vbRoi1]
                ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $vbRoi1 ] != -1 } ] $ModuleName "Destroying roi $vbRoi1"
            }

            if {$vb2 != "."} {
                set vbRoi2 [b2 load roi $vb2]
                set vbMask2 [b2 convert roi to mask $vbRoi2]
                ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $vbRoi2 ] != -1 } ] $ModuleName "Destroying roi $vbRoi2"
            }

            if {($vb1 != ".") && ($vb2 != ".")} {
                set vbMask [b2 or masks $vbMask1 $vbMask2]
                ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $vbMask1 ] != -1 } ] $ModuleName "Destroying mask $vbMask1"
                ReportTestStatus $LogFile  [ expr { [ b2 destroy mask $vbMask2 ] != -1 } ] $ModuleName "Destroying mask $vbMask2"
            } elseif {$vb1 != "."} {
                set vbMask $vbMask1
            } else {
                set vbMask $vbMask2
            }
        } elseif {$vbMethod == "top"} {
            set vbMask -1
        } elseif {$vbMethod == "bottom"} {
            set vbMask -2
        } else {
            set vbMask -3
        }

        if {$vbMethod != "traces"} {
            if {$vbModeImg == "T1"} {
                set bi 0
            } elseif {$vbModeImg == "T2"} {
                set bi 1
            } else {
                set bi 2
            }

            set plugArgs "pb= $vbMask bi= $bi"
        } else {
            set plugArgs "pb= $vbMask"
        }

        set com1 "set plugList \[b2 generate class-plugs \{$imageList\} {gm wm csf} \{$meanRankTable\} $brainMask r= $random n= $numPlugs \
                        c= $coverage p= $permissiveness mo= $meanOutlier vo= $varOutlier ps= $plugSize dx= $numXpart \
                        dy= $numYpart dz= $numZpart gm= $numGrey wm= $numWhite csf= $numCsf $plugArgs\]"
        eval $com1


        if {$plugList == -1} {
        }

        set sResult [b2 save mask $greyMaskName brains2 [lindex $plugList 0]]
        if {$sResult == -1} {
        }
        set sResult [b2 save mask $whiteMaskName brains2 [lindex $plugList 1]]
        if {$sResult == -1} {
        }
        set sResult [b2 save mask $csfMaskName brains2 [lindex $plugList 2]]
        if {$sResult == -1} {
        }
        set sResult [b2 save mask $bloodMaskName brains2 [lindex $plugList 3]]
        if {$sResult == -1} {
        }
    }

    if {($genPlugs == 0) && ($genModel == 1)} {
        set mask [ b2 load mask $greyMaskName]
        if {$mask == -1} {
        }
        lappend plugList $mask

        set mask [ b2 load mask $whiteMaskName]
        if {$mask == -1} {
        }
        lappend plugList $mask

        set mask [ b2 load mask $csfMaskName]
        if {$mask == -1} {
        }
        lappend plugList $mask

        set mask [ b2 load mask $bloodMaskName]
        if {$mask == -1} {
        }
        lappend plugList $mask

        if {$pickTrainRegion == "box"} {
            set talPar [b2 load talairach-parameters $talairach]
            if {$talPar == -1} {
            }

            set brainBox [b2 load talairach-box $talairachBox]
            if {$brainBox == -1} {
            }
            set brainMask [b2 convert talairach-box to mask $talPar $brainBox]
            ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-parameters $talPar ] != -1 } ] $ModuleName "Destroying talairach-parameters $talPar"
            ReportTestStatus $LogFile  [ expr { [ b2 destroy talairach-box $brainBox ] != -1 } ] $ModuleName "Destroying talairach-box $brainBox"
        } else {
            set brainMask [b2 load mask $regionMask]
            if {$brainMask == -1} {
            }
        }
    }

    if {$genModel == 1} {
            set com2 " set classModel \[b2 generate class-model \{$imageNameList\} \{$imageList\} {gm wm csf vb} \{$plugList\} $brainMask \
                    hist-eq= $histEqual tg= $trimGross ts= $trimSpatial x= $spatial_X y= $spatial_Y z= $spatial_Z \
                    xx= $spatial_XX yy= $spatial_YY zz= $spatial_ZZ xy= $spatial_XY xz= $spatial_XZ yz= $spatial_YZ\]"

        eval $com2

        if {$classModel == -1} {
        }

        set sResult [b2 save tissue-class $modelName brains2 $classModel]
        if {$sResult == -1} {
        }
    }

    if {($genModel == 0) && ($applyModel == 1)} {
        set classModel [b2 load tissue-class $modelName]
        if {$classModel == -1} {
        }
    }

    if {$applyModel == 1} {
        if {$excludeMask != "."} {
            set t1ExcludeMask [b2 load mask $excludeMask]
            if {$t1ExcludeMask == -1} {
            }
            append classifyArgs "ex_t1= $t1ExcludeMask "
        }

        if {$bgbwVals == "traces"} {
            set bgRoi [b2 load roi $bg]
            if {$bgRoi == -1} {
            }

            set bwRoi [b2 load roi $bw]
            if {$bwRoi == -1} {
            }

            set bgMask [b2 convert roi to mask $bgRoi]
            if {$bgMask == -1} {
            }
            set bwMask [b2 convert roi to mask $bwRoi]
            if {$bwMask == -1} {
            }

            ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $bgRoi ] != -1 } ] $ModuleName "Destroying roi $bgRoi"
            ReportTestStatus $LogFile  [ expr { [ b2 destroy roi $bwRoi ] != -1 } ] $ModuleName "Destroying roi $bwRoi"

            set classifyArgs "bg= $bgMask bw= $bwMask"
        } else {
            set classifyArgs "cth= $threshold "
        }

        set com3 "set classImage \[b2 apply class-model \{$imageNameList\} \{$imageList\} $classModel $classifyArgs\]"
        eval $com3
        if {$classImage == -1} {
        }

        set sResult [b2 save image $segmentImage strictAnalyze75 $classImage]
        if {$sResult == -1} {
        }

        # Check Segmentation Results
            set ACPCdir [getACPCdir $segmentImage]
            set errorFlag [catch {set f [open ${OUTPUT_DIR}/check_segment.brains2 w]}]
            if {$errorFlag != 0} {
                return
            }
            puts $f "set class \[b2 load image $segmentImage\]"
            if {$t1 != "."} {
                puts $f "set t1 \[b2 load image $t1\]"
            }
            if {$t2 != "."} {
                puts $f "set t2 \[b2 load image $t2\]"
            }
            if {$pd != "."} {
                puts $f "set pd \[b2 load image $pd\]"
            }
            puts $f "set model \[b2 load tissue-class $modelName\]"
            puts $f "set gm \[b2 load mask $greyMaskName\]"
            puts $f "set wm \[b2 load mask $whiteMaskName\]"
            puts $f "set csf \[b2 load mask $csfMaskName\]"
            puts $f "set blood \[b2 load mask $bloodMaskName\]"
            puts $f "make_discrete_image \$class"
            puts $f "b2 show tracker"
            puts $f "set kappa \[b2 get tissue-class kappa \$model\]"
            puts $f "dialog_measuregui \"Tissue Class Model kappas \[b2 object-name tissue-class \$model\]:\\n\[lindex \$kappa 0\]\\n\[lindex \$kappa 1\]\\n\[lindex \$kappa 2\]\\n\[lindex \$kappa 3\]\\n"
            puts $f "set qa \[checkSegmentQuality\]"
            puts $f "if \{\$qa == -1\} \{tk_dialog .qaerror \"Error\" \"Failed to record segment check\" \"\" 0 \"Cancel\" \}"
            close $f
            exec echo "Tissue Classification Complete\nCheck Classification using:\nbrains2 -c ${OUTPUT_DIR}/check_segment.brains2"
        }
    set ACPCdir [getACPCdir $segmentImage]
    set scanId [getScanIdFromACPCdir $ACPCdir]
    set cthFile $ACPCdir/standard/${scanId}_tissueClass.log
    if {[file exists $ACPCdir/standard] && [file writable $ACPCdir/standard]} {
        if {[file exists $cthFile] == 0} {
            set userChangeMod 1
        } else {
            set userChangeMod 0
        }
        set errorFlag [catch {set f [open $cthFile a]}]
        if {$errorFlag != 0} {
            return
        }
        puts $f "***************************** [exec date] *****************************"
        if {$genPlugs == 1} {
            puts $f "Plug Generation"
            puts $f "---------------"
            puts $f "T1 Image: $t1"
            puts $f "T2 Image: $t2"
            puts $f "PD Image: $pd"
            puts $f $com1
            puts $f "Grey Plugs: $greyMaskName"
            puts $f "White Plugs: $whiteMaskName"
            puts $f "CSF Plugs: $csfMaskName"
            puts $f "Blood Plugs: $bloodMaskName"
        }
        if {$genModel == 1} {
            puts $f "Model Generation"
            puts $f "----------------"
            puts $f "T1 Image: $t1"
            puts $f "T2 Image: $t2"
            puts $f "PD Image: $pd"
            puts $f "Grey Plugs: $greyMaskName"
            puts $f "White Plugs: $whiteMaskName"
            puts $f "CSF Plugs: $csfMaskName"
            puts $f "Blood Plugs: $bloodMaskName"
            puts $f $com2
            puts $f "Tissue Class Model: $modelName"
        }
        if {$applyModel == 1} {
            puts $f "Apply Generation"
            puts $f "----------------"
            puts $f "T1 Image: $t1"
            puts $f "T2 Image: $t2"
            puts $f "PD Image: $pd"
            puts $f "Tissue Class Model: $modelName"
            puts $f $com3
            puts $f "Tissue Classified Image: $segmentImage"
        }
        close $f
        if {$userChangeMod == 1} {
            if {[info exists B2_IPL_FLAG] && ($B2_IPL_FLAG == 1)} {
                if {[catch { exec chgrp stdwkup $cthFile } squabble] != 0 } {puts "exec failed: $squabble" }
            }
            if {[catch { exec chmod 664 $cthFile } squabble] != 0 } {puts "exec failed: $squabble" }
        }
    }
}



proc generateTissueClassify {pathToRegressionDir dateString} {
    global B2_TALAIRACH_DIR
    global classModel classImage brainMask

    set ModuleName "generateTissueClassify"
    set ModuleAuthor "Greg Harris"
    set ModuleDescription "Testing of b2 generate class model"
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE
    set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]
#   close $LogFile
#   set LogFile stderr

set classify_results_dir "${OUTPUT_DIR}/genTissueClass_$dateString"
set orig_results_dir "$pathToRegressionDir/SGI/MR/B2-downsampled/TEST/20_ACPC"
set experiment_dir "TEST"
if {[catch { exec mkdir -p $classify_results_dir/$experiment_dir } squabble] != 0 } {puts "exec failed: $squabble" }

### Just run modified version of testbrains2TissueClassify

# The seed string '1326' was the result of a search for a seed with high derived kappas.

testbrains2TissueClassify 1 1 1 \
$orig_results_dir/ANON0006_20_T1.hdr \
$orig_results_dir/ANON0006_20_T2.hdr  \
$orig_results_dir/ANON0006_20_PD.hdr  \
mask \
$orig_results_dir/Talairach.bnd \
$B2_TALAIRACH_DIR/brain_box  \
$orig_results_dir/ANON0006_brain_cut.mask \
traces $orig_results_dir/harris/l_seg_vb.zroi \
$orig_results_dir/harris/l_seg_vb.zroi T1 \
1326 0.85 0.10 500 500 200 80 \
2.5 10.0 2.0 1 1 1 \
$classify_results_dir/$experiment_dir/brain_region.mask \
$classify_results_dir/$experiment_dir/grey_plugs.mask \
$classify_results_dir/$experiment_dir/white_plugs.mask \
$classify_results_dir/$experiment_dir/csf_plugs.mask \
$classify_results_dir/$experiment_dir/blood_plugs.mask \
0.0 0.0 1 \
1 1 1 1 1 1 1 1 1 \
$classify_results_dir/TEST_20_Tissue_Class.mdl \
. \
const . \
. \
197 \
$classify_results_dir/TEST_20_segment.hdr $LogFile $ModuleName


array set MinValueArray [ join  {
{gm_kappa 0.970} {wm_kappa 0.970} {csf_kappa 0.970} {vb_kappa 0.995}
}
]

set output [ b2  get tissue-class kappa $classModel ]
array set ValueArray [ join $output ]
foreach {key} [array names MinValueArray] {
          set SubTestDes "Comparing to kappa values for $key, is $MinValueArray($key)  <= $ValueArray($key)  "
          ReportTestStatus $LogFile  [expr { $MinValueArray($key) <= $ValueArray($key) }] $ModuleName $SubTestDes
}

set stdClassified [b2 load image ${orig_results_dir}/STD_20_segment.hdr]
set clipToBrainOnly [b2 erode mask $brainMask 1]


set shouldbezero [b2 subtract images [list $classImage $stdClassified]]
set volumetable [b2 measure image mask $clipToBrainOnly $shouldbezero]
          set SubTestDes "Check whether error mean=[lindex [lindex $volumetable 0] 1] is close to zero."
          ReportTestStatus $LogFile  [expr {abs([lindex [lindex $volumetable 0] 1]) <= 0.5 }] $ModuleName $SubTestDes

          set SubTestDes "Check whether error StdDev=[lindex [lindex $volumetable 3] 1] is less than 1.0."
          ReportTestStatus $LogFile  [expr {[lindex [lindex $volumetable 3] 1] < 1.0 }] $ModuleName $SubTestDes

   set objecttypes { image  roi  mask  gtsurface  transform  talairach-parameters  talairach-box  histogram  landmark  palette  table  tissue-class }
   foreach {currentobjtype} $objecttypes  {
          ReportTestStatus $LogFile  [expr {[b2 destroy every $currentobjtype ] != -1 }] $ModuleName "Destroying every $currentobjtype"
   }
          return [ StopModule  $LogFile $ModuleName ]
}


