# \author    Hans J. Johnson"
# \date     $Date: 2007-05-17 15:21:50 -0500 (Thu, 17 May 2007) $
# \brief    This module tests common components of loading any image file format, i.e., if it
#           is a valid image, then these items must be testable:
# \fn        proc CoreSaveImageTest {}
# \params  They should be self expanatory
# \result    1 in case testing is complete or 0 in case of a fatal error
proc CoreSaveImageTest { ImageTypeName SaveFileName TestMaskID ImageType ImageMin ImageMax ImageMean ImageStd Dimensions Resolutions Strict LogFile ModuleName} {
    global OUTPUT_DIR;
    global MODULE_SUCCESS;
    global MODULE_FAILURE ;

    set SubTestDes "$ImageTypeName Save Image - Load Saved Image" ;
#   Specify a signed data type for in-memory storage.  This corresponds to a "principled decision"
#   that converting storage types from signed (on disk) to unsigned (in memory) and
#   then back to signed (in the measurement slices) throws away the valid offset information.
#   ((But maybe this can be corrected.))
    set SaveImageID [b2_load_image $SaveFileName data-type= F32] ;
    ReportTestStatus $LogFile  [ expr  {$SaveImageID != -1}   ] $ModuleName $SubTestDes;
    if  {$SaveImageID == -1} {
        return $MODULE_FAILURE;
    }

    set type [b2_get_image_type $SaveImageID 0] ;
    puts "XX type = $type"
    puts "XX ImageType = $ImageType"
    set lentype [llength  $ImageType] ;
    set SubTestDes "$ImageTypeName $ImageType image length of type: $lentype" ;
    
    ReportTestStatus $LogFile  [ expr { [llength $type] == $lentype } ] $ModuleName $SubTestDes ;
    for {set currtype 0} { $currtype < $lentype } {incr currtype} {
#set SubTestDes "$ImageTypeName $ImageType type($currtype)" ;
        set SubTestDes "$ImageTypeName $ImageType type($currtype): [lindex $type $currtype ] == [lindex $ImageType $currtype]" ;
        ReportTestStatus $LogFile  [ expr  {[lindex $type $currtype ] == [lindex $ImageType $currtype]}   ] $ModuleName $SubTestDes ;
    }
    
    set SubTestDes "$ImageTypeName $ImageType image min" ;
    set testimageMin [b2_image_min $SaveImageID] ;
    set lentype [llength  $ImageMin] ;
    ReportTestStatus $LogFile  [ expr { [llength $testimageMin] == $lentype } ] $ModuleName $SubTestDes ;
    for {set currtype 0} { $currtype < $lentype } {incr currtype} {
        set SubTestDes "$ImageTypeName $ImageType min($currtype): computed-> [lindex $testimageMin $currtype ] known-> [lindex $ImageMin $currtype]" ;
        ReportTestStatus $LogFile  [ expr  {[lindex $testimageMin $currtype ] == [lindex $ImageMin $currtype]}   ] $ModuleName $SubTestDes ;
    }

    set SubTestDes "$ImageTypeName $ImageType image max" ;
    set testimageMax [b2_image_max $SaveImageID] ;
    set lentype [llength  $ImageMax] ;
    ReportTestStatus $LogFile  [ expr { [llength $testimageMax] == $lentype } ] $ModuleName $SubTestDes ;
    for {set currtype 0} { $currtype < $lentype } {incr currtype} {
        set SubTestDes "$ImageTypeName $ImageType max($currtype): computed-> [lindex $testimageMax $currtype ] known-> [lindex $ImageMax $currtype]" ;
        ReportTestStatus $LogFile  [ expr  {[lindex $testimageMax $currtype ] == [lindex $ImageMax $currtype]}   ] $ModuleName $SubTestDes ;
    }

    set SubTestDes "$ImageTypeName $ImageType dims image" ;
    set dims [b2_get_dims_image $SaveImageID] ;
    set lendims [llength $Dimensions] ;
    ReportTestStatus $LogFile  [ expr { [llength $dims] == $lendims } ] $ModuleName $SubTestDes ;

    for {set currdim 0} { $currdim < $lendims } {incr currdim} {
        set newdim [lindex $dims $currdim] ;
        set knowndim [lindex $Dimensions $currdim] ;
        set SubTestDes "$ImageTypeName $ImageType dim($currdim) does $newdim == $knowndim" ;
        ReportTestStatus $LogFile  [ expr  {$newdim == $knowndim}   ] $ModuleName $SubTestDes ;
    }

    set SubTestDes "$ImageTypeName $ImageType res image" ;
    set res [b2_get_res_image $SaveImageID] ;
    set lenres [llength $Resolutions] ;
    ReportTestStatus $LogFile  [ expr  {[llength $res] == $lenres}   ] $ModuleName $SubTestDes ;

    for {set currres 0} { $currres < $lenres } {incr currres} {
        set newres [lindex $res $currres] ;
        set knownres [lindex $Resolutions $currres] ;
        set SubTestDes "$ImageTypeName $ImageType res($currres) does $newres == $knownres" ;
        ReportTestStatus $LogFile  [ expr  {$newres == $knownres}   ] $ModuleName $SubTestDes ;
    }

    set result [b2_measure_image_mask $TestMaskID $SaveImageID] ;
    set SubTestDes "$ImageTypeName Save Image - Measure Saved Image" ;
    ReportTestStatus $LogFile  [ expr  {$result != -1}   ] $ModuleName $SubTestDes ;
    set SubTestDes "$ImageTypeName Save Image - Measured Image Mean (new: [lindex [lindex $result 0] 1]; expected: $ImageMean)" ;
    ReportTestStatus $LogFile  [ expr  {abs([lindex [lindex $result 0] 1] - $ImageMean) / $ImageMean < 0.0001}   ] $ModuleName $SubTestDes ;
    set SubTestDes "$ImageTypeName Save Image - Measured Image StdDev  (new: [lindex [lindex $result 3] 1]; expected: $ImageStd)" ;
    ReportTestStatus $LogFile  [ expr  {abs([lindex [lindex $result 3] 1] - $ImageStd) / $ImageStd < 0.0001}   ] $ModuleName $SubTestDes ;

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $SaveImageID ] != -1 } ] $ModuleName "Destroying image $SaveImageID" ;
    puts "Released memory for $SaveFileName" ;

    return $MODULE_SUCCESS ;
}



# \author    Hans J. Johnson"
# \date        $Date: 2007-05-17 15:21:50 -0500 (Thu, 17 May 2007) $
# \brief    This module tests the loading of various image file formats
# \fn        proc loadImage {pathToRegressionDir dateString}
# \param    string pathToRegressionDir    - Path to the regresssion test directory
# \param    string dateString            - String to label output file
# \result    1 in case testing is complete or 0 in case of a fatal error
proc saveImage {pathToRegressionDir dateString} {
########################################
########################################
#Pretest must be available in all modules
########################################
########################################
        set ModuleName "saveImage" ;
        set ModuleAuthor "Hans J. Johnson" ;
        set ModuleDescription "Test the b2_save_image command and saving various image file formats" ;
        global OUTPUT_DIR;
        global MODULE_SUCCESS;
        global MODULE_FAILURE ;
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString] ;

########################################
########################################
# Run Tests
########################################
########################################

## TODO:  Read in a smaller set of trimodal images
    set SubTestDes "saveImage - Load RGB Image test" ;
    set b2cmd [format "b2_load_image {%s %s %s}" \
              $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr \
              $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_t2_fit.hdr \
              $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_pd_fit.hdr]
    set RgbTestImageID [eval $b2cmd]
    if { [ ReportTestStatus $LogFile  [ expr {$RgbTestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE
    }
    set RgbTestMaskID [ b2_threshold_image $RgbTestImageID 0 absolute-value= True ] ;
    set RgbImageType "T1" ;
    set RgbImageMin {0.000000 0.000000 0.000000} ;
    set RgbImageMax  {255.000000 255.000000 255.000000} ;
    set RgbImageMean  25.331608851750691 ;
#        set RgbImageMean  27.045659780502319 ;
    set RgbImageStd  43.517092386806489 ;
#        set RgbImageStd  45.373369306353894 ;
    set RgbDimensions "256 256 192" ;
    set RgbResolutions "1.015625 1.015625 1.015625" ;



    set SubTestDes "Save Image - Load Image test" ;
    set FullTestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1.hdr] ;
    if { [ ReportTestStatus $LogFile  [ expr {$FullTestImageID != -1 } ] $ModuleName $SubTestDes ] == 0} {
        return $MODULE_FAILURE ;
    }
    set ScaleReductionTransform [ b2_create_affine-transform  { 23 17 11 } { 5 6 7 }  [b2_get_dims_image i1] [b2_get_res_image i1 ] dx= 30 ] ;
    b2 set Transform $ScaleReductionTransform image $FullTestImageID ;
    set TestImageID [ b2_resample_image $FullTestImageID plane= coronal ] ;
    set TestMaskID [ b2_threshold_image $TestImageID 0 absolute-value= True ] ;

    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image     ${FullTestImageID} ] != -1 } ] $ModuleName "Destroying image $FullTestImageID" ;
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_transform ${ScaleReductionTransform} ] != -1 } ] $ModuleName "Destroying transform $ScaleReductionTransform" ;
##Set values common to all single modal images
    set ImageType "T1";
    set ImageMin 0.000000;
#    set ImageMax  253.000000 ;
    set ImageMax  246.000000 ;
#    set ImageMean 63.923971169495466 ;
    set ImageMean 62.192513368983960 ;
#    set ImageStd 39.841412773156328 ;
    set ImageStd 39.754369655775093 ;
    set Dimensions "23 17 11" ;
    set Resolutions "5.000000000000000 6.000000000000000 7.000000000000000" ;

    ### Create Directories
    if {[catch { exec mkdir -p ${OUTPUT_DIR}/TEST/10_ACPC } squabble] != 0 } {puts "exec failed: $squabble" } ;

###############################################################################################
######################### ANALYZE  and BRAINS2 Filter #########################################
###############################################################################################

    foreach {curr_filter} {"strictAnalyze75" "b2analyze" "brains2" "nifti"} {
        switch  ${curr_filter} {
            "brains2" {
                set ImageType "T1";
                set ImageMax  246.000000 ;
            }
            "strictAnalyze75" {
                set ImageType "T1";
                set ImageMax  246.000000 ;
            }
            "b2analyze" {
                set ImageType "T1";
                set ImageMax  246.000000 ;
            }
            "nifti" {
                set ImageType "T1";
                set ImageMax  246.000000 ;
##HACK For NIFTI  These should not be different, but there is something wierd going on.
            }
        }
############################### ORIENT DEFAULT Images ###########################################
        set ImageTypeName "ORIENT-DEFAULT-${curr_filter}" ;
        set SaveFileName ${OUTPUT_DIR}/TEST/10_ACPC/strict_analyze_T1.hdr ;
        set SubTestDes "Save $ImageTypeName $ImageType test" ;
        set errorTest [b2_save_image $SaveFileName strictAnalyze75 $TestImageID ] ;
        if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
            CoreSaveImageTest $ImageTypeName $SaveFileName $TestMaskID $ImageType $ImageMin $ImageMax $ImageMean $ImageStd $Dimensions $Resolutions 1 $LogFile $ModuleName ;
        }

############################### NN_INTERPOLATION Images ###########################################

        b2_set_interpolation nearest-neighbor
        set ImageTypeName "ORIENT-nearest-neighbor-${curr_filter}" ;
        set SaveFileName ${OUTPUT_DIR}/TEST/10_ACPC/analyze_T1_nearest-neighbor.hdr ;
        set SubTestDes "Save $ImageTypeName $ImageType test" ;
        set errorTest [b2_save_image $SaveFileName strictAnalyze75 $TestImageID ] ;
        if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
            CoreSaveImageTest $ImageTypeName $SaveFileName $TestMaskID $ImageType $ImageMin $ImageMax $ImageMean $ImageStd $Dimensions $Resolutions 0 $LogFile $ModuleName ;
        }
        b2_set_interpolation trilinear

############################### Orient Images ###########################################
        foreach {curr_orient} { "Coronal" "Axial" "Sagittal" } {
            set ImageTypeName "ORIENT-${curr_orient}-${curr_filter}" ;
            set SaveFileName ${OUTPUT_DIR}/TEST/10_ACPC/analyze_T1_${curr_orient}.hdr ;
            set SubTestDes "Save $ImageTypeName $ImageType test" ;
            set errorTest [b2_save_image $SaveFileName strictAnalyze75 $TestImageID plane= ${curr_orient}] ;
            if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
                CoreSaveImageTest $ImageTypeName $SaveFileName $TestMaskID $ImageType $ImageMin $ImageMax $ImageMean $ImageStd $Dimensions $Resolutions 0 $LogFile $ModuleName ;
            }
        }

############################### COMMON Storage Types Images ###########################################
#TODO: double check that all valid data types are being tested here
##NOTE Invalid strictAnalyzeTypes: Unsigned-16bit Unsigned-32bit Signed-64bit Unsigned-64bit Double
##NOTE Invalid b2analze types    :                               Signed-64bit Unsigned-64bit Double
        switch  ${curr_filter} {
            "brains2" {
                set aux_types { "unsigned-8bit" "signed-16bit" "signed-32bit" "float-single" "unsigned-16bit" "unsigned-32bit" } ;
#TODO:  the 255 seems suspicious here,  it should be investigated further at a later time, I would think that brains2, strictAnalyze75 and b2analyze would produce the same values for reading and writing with respect to brains2 interpretation.
                set ImageMax  255.000000 ;
                set FileExt "hdr"
            }
            "strictAnalyze75" {
                set aux_types { "unsigned-8bit" "signed-16bit" "signed-32bit" "float-single" } ;
                set ImageMax  246.000000 ;
                set FileExt "hdr"
            }
            "b2analyze" {
                set aux_types { "unsigned-8bit" "signed-16bit" "signed-32bit" "float-single" } ;
                set ImageMax  246.000000 ;
                set FileExt "hdr"
            }
            "nifti" {
                set aux_types { "unsigned-8bit" "signed-16bit" "signed-32bit" "float-single" } ;
                set ImageMax  246.000000 ;
                set FileExt "nii.gz";
                set ImageType  "UNKNOWN";
            }
        }

        foreach {curr_datatype} ${aux_types}  {
            set ImageTypeName "STORAGE-${curr_filter}-${curr_datatype}" ;
            set SaveFileName ${OUTPUT_DIR}/TEST/10_ACPC/${curr_filter}-${curr_datatype}_T1.${FileExt} ;
            set SubTestDes "Save $ImageTypeName $ImageType test" ;
            set errorTest [b2_save_image $SaveFileName  ${curr_filter} $TestImageID data-type= ${curr_datatype}] ;
            if { [ ReportTestStatus $LogFile  [ expr {$errorTest != -1 } ] $ModuleName $SubTestDes ]} {
                CoreSaveImageTest $ImageTypeName $SaveFileName $TestMaskID $ImageType $ImageMin $ImageMax $ImageMean $ImageStd $Dimensions $Resolutions 0 $LogFile $ModuleName ;
            }
        }
############################### RGB Analyze Images ###########################################
##hack no nifti filter for trimodal
        if {  ${curr_filter} != "nifti" } {
          set RgbImageTypeName "Trimodal ${curr_filter}" ;
          set SaveFileName ${OUTPUT_DIR}/TEST/10_ACPC/rgb_${curr_filter}_T1.${FileExt} ;
          set SubTestDes "Save $ImageTypeName $ImageType test" ;
          set errorTest [b2_save_image $SaveFileName  ${curr_filter} $RgbTestImageID] ;
          if { [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
              CoreSaveImageTest $RgbImageTypeName $SaveFileName $RgbTestMaskID $RgbImageType $RgbImageMin $RgbImageMax $RgbImageMean $RgbImageStd $RgbDimensions $RgbResolutions 0 $LogFile $ModuleName ;
          }
        }
    }


################################# Test for invalid arguements in saving Images ###############
    set SubTestDes "required arguement test (1)" ;
    set errorTest [b2_save_image] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;


    set SubTestDes "required arguement test (2)" ;
    set errorTest [b2_save_image ${OUTPUT_DIR}/TEST/10_ACPC/junk.hdr] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "required arguement test (3)" ;
    set errorTest [b2_save_image ${OUTPUT_DIR}/TEST/10_ACPC/junk.hdr strictAnalyze75] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "arguement number test" ;
    set errorTest [b2_save_image ${OUTPUT_DIR}/TEST/10_ACPC/junk.hdr strictAnalyze75 $TestImageID junk= ] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "optional arguement test" ;
    set errorTest [b2_save_image ${OUTPUT_DIR}/TEST/10_ACPC/junk.hdr strictAnalyze75 $TestImageID junk= test] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "ANALYZE invalid filename test";
    set errorTest [b2_save_image /invalid_directory_name/TEST/10_ACPC/junk.hdr strictAnalyze75 $TestImageID];
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes;

    set SubTestDes "ANALYZE invalid filter-suffix test";
    set errorTest [b2_save_image ${OUTPUT_DIR}/TEST/10_ACPC/junk.hdr strictAnalyze75 $TestImageID filter-suffix= -invalid];
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes;

    set SubTestDes "BRAINS2 invalid filename test" ;
    set errorTest [b2_save_image /invalid_directory_name/TEST/10_ACPC/junk.hdr brains2 $TestImageID] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;

    set SubTestDes "BRAINS2 invalid filter-suffix test" ;
    set errorTest [b2_save_image ${OUTPUT_DIR}/TEST/10_ACPC/junk.hdr brains2 $TestImageID filter-suffix= -invalid] ;
    ReportTestStatus $LogFile  [ expr {$errorTest == -1 } ] $ModuleName $SubTestDes ;

#### Cleaning up
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $RgbTestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID" ;
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $TestMaskID ] != -1 } ] $ModuleName "Destroying mask $TestMaskID" ;
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID" ;
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $RgbTestImageID ] != -1 } ] $ModuleName "Destroying image $RgbTestImageID" ;
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_every image ] != -1 } ] $ModuleName "Destroying every image" ;
    #We're not "supposed" to do this but StopModule will flag an error if we don't.

########################################
########################################
#Posttest must be available in all modules
########################################
########################################
    return [ StopModule  $LogFile $ModuleName ];
}

