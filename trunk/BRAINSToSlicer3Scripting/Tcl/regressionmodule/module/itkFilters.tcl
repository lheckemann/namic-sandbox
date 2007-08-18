# \author       Greg Harris
# \date         1/20/2004
# \brief        proc itkFilters
# \param        string pathToRegressionDir        - Path to the regresssion test directory
# \param        string dateString                - String to label output file
# \result       1 in case testing is complete or 0 in case of a fatal error
# Test Performed
# -----------------------------------------------------------------------
# set pathToRegressionDir "../regressiontest"
# set dateString "DEBUG"
proc itkFilters { pathToRegressionDir dateString } {
        set ModuleName "itkFilters"
        set ModuleAuthor "Greg Harris"
        set ModuleDescription "Testing of b2 itk Image filters"
        global MODULE_SUCCESS
        global MODULE_FAILURE
        set LogFile [ StartModule $ModuleName $ModuleAuthor $ModuleDescription $dateString]

#        close $LogFile
#        set LogFile stderr

# Run Tests
############ load prerequisite files ###################

#Clean all images, masks, and ROI's to ensure masks id's start at 0
ReportTestStatus $LogFile  [ expr { [ b2_destroy_every mask ] != -1 } ] $ModuleName "Destroying every mask"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_every roi ] != -1 } ] $ModuleName "Destroying every roi"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_every image ] != -1 } ] $ModuleName "Destroying every image"

    set SubTestDes "Load requisite objects for filter testing - boxed T1 sub-image"
    set TestImageID [b2_load_image $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_T1_boxed.hdr]
      if { ! [ ReportTestStatus $LogFile  [ expr {$TestImageID != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error reading image.\nFailed to load Image."
      }
    set SubTestDes "Load requisite objects for filter testing - whole box mask"
      set brainMask [b2_load_mask $pathToRegressionDir/SGI/MR/4x-B1/TEST/10_ACPC/ANON013_boxed.mask ]
      if { ! [ ReportTestStatus $LogFile  [ expr {$brainMask != -1 } ] $ModuleName $SubTestDes ]} {
          puts "Error creating volume(s).\nFailed to load mask."
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

set known_output {
{Mean 68.491805851864669} {Mean-Absolute-Deviation 24.038835800771203} {Variance 862.605044444790678} {Standard-Deviation 29.370138652120637} {Skewness 1.494976135061417} {Kurtosis -0.415206165613023} {Minimum 0.000000000000000} {Maximum 239.000000000000000} }
CoreMeasuresEpsilonTest "original image" 0.0001 $known_output $output $LogFile $ModuleName

#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

set filtImg4F [b2_itkRecursiveGaussian $TestImageID 4.0 data-type= float-single]
set output4F [b2_measure_image_mask $brainMask $filtImg4F]

set known_output4F  {
{Mean 68.510965018330538} {Mean-Absolute-Deviation 15.622857870805580} {Variance 394.419633189907813} {Standard-Deviation 19.860000835596857} {Skewness 1.645614960784147} {Kurtosis 0.177067030422524} {Minimum 9.090394973754883} {Maximum 103.485847473144531}
}
CoreMeasuresEpsilonTest "itkRecursiveGaussian 4.0 float-single image" 0.0001 $known_output4F $output4F $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImg4F ] != -1 } ] $ModuleName "Destroying image $filtImg4F"
#######################################################################################################################
#######################################################################################################################

set filtImg4B [b2_itkRecursiveGaussian $TestImageID 4.0 data-type= Unsigned-8bit]
set output4B [b2_measure_image_mask $brainMask $filtImg4B]

set known_output4B  {
{Mean 68.010330777066642} {Mean-Absolute-Deviation 15.620244882493514} {Variance 394.466291714068234} {Standard-Deviation 19.861175486714483} {Skewness 1.645643948400332} {Kurtosis 0.177231449075008} {Minimum 9.000000000000000} {Maximum 103.000000000000000}
}
CoreMeasuresEpsilonTest "itkRecursiveGaussian 4.0 unsigned-8-bit image" 0.0001 $known_output4F $output4F $LogFile $ModuleName


ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImg4B ] != -1 } ] $ModuleName "Destroying image $filtImg4B"
#######################################################################################################################
#######################################################################################################################

set filtImgDG [b2_itkDiscreteGaussian $TestImageID 4.0 12 data-type= float-single]
set outputDG [b2_measure_image_mask $brainMask $filtImgDG]

set known_outputDG  {
{Mean 68.487358665866054} {Mean-Absolute-Deviation 19.381929381088330} {Variance 586.359885100723204} {Standard-Deviation 24.214869091133306} {Skewness 1.558012788617803} {Kurtosis -0.208584111677267} {Minimum 3.451355934143066} {Maximum 139.478958129882812}
}
CoreMeasuresEpsilonTest "itkDiscreteGaussian 4.0 float-single image" 0.0001 $known_outputDG $outputDG $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImgDG ] != -1 } ] $ModuleName "Destroying image $filtImgDG"
#######################################################################################################################
#######################################################################################################################

set filtImgSig [b2_itkSigmoid $TestImageID -8.33333333333 65.0 data-type= float-single]
set outputSig [b2_measure_image_mask $brainMask $filtImgSig]

set known_outputSig  {
{Mean 0.401587334554221} {Mean-Absolute-Deviation 0.353669255811707} {Variance 0.152583103716748} {Standard-Deviation 0.390618872709382} {Skewness 1.217619386853368} {Kurtosis -1.419486418037700} {Minimum 0.000000000854931} {Maximum 0.999590456485748}
}
CoreMeasuresEpsilonTest "itkSigmoid (-8.33333333333, 65.0) float-single image" 0.0001 $known_outputSig $outputSig $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImgSig ] != -1 } ] $ModuleName "Destroying image $filtImgSig"
#######################################################################################################################
#######################################################################################################################

set filtImgMed [b2_itkMedian $TestImageID 1 data-type= float-single]
set outputMed [b2_measure_image_mask $brainMask $filtImgMed]

set known_outputMed  {
{Mean 68.527612805678885} {Mean-Absolute-Deviation 23.242009399805511} {Variance 812.803956388586698} {Standard-Deviation 28.509716876682354} {Skewness 1.492115769210495} {Kurtosis -0.466680273984291} {Minimum 1.000000000000000} {Maximum 178.000000000000000}
}
CoreMeasuresEpsilonTest "itkMedian (1) float-single image" 0.0001 $known_outputMed $outputMed $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImgMed ] != -1 } ] $ModuleName "Destroying image $filtImgMed"
#######################################################################################################################
#######################################################################################################################

set filtImgGAD [b2_itkGradientAnisotropicDiffusion $TestImageID iterations= 4 conductance= 0.7 data-type= float-single]
set outputGAD [b2_measure_image_mask $brainMask $filtImgGAD]

set known_outputGAD  {
{Mean 68.491805854449154} {Mean-Absolute-Deviation 23.589095389204999} {Variance 836.485001890956369} {Standard-Deviation 28.922050444098122} {Skewness 1.500488572139150} {Kurtosis -0.394986033630518} {Minimum 1.330168962478638} {Maximum 237.971740722656250}
}
CoreMeasuresEpsilonTest "itkGradientAnisotropicDiffusion float-single image" 0.0001 $known_outputGAD $outputGAD $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImgGAD ] != -1 } ] $ModuleName "Destroying image $filtImgGAD"
#######################################################################################################################
#######################################################################################################################

set filtImgBilat [b2_itkBilateral $TestImageID 1.0 12.0 data-type= float-single]
set outputBilat [b2_measure_image_mask $brainMask $filtImgBilat]

set known_outputBilat  {
{Mean 68.509741538350511} {Mean-Absolute-Deviation 23.466185017088023} {Variance 827.598198947407468} {Standard-Deviation 28.768006516743689} {Skewness 1.499359555807938} {Kurtosis -0.403965197059772} {Minimum 1.936514019966125} {Maximum 233.242019653320312}
}
CoreMeasuresEpsilonTest "itkBilateral float-single image" 0.0001 $known_outputBilat $outputBilat $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImgBilat ] != -1 } ] $ModuleName "Destroying image $filtImgBilat"
#######################################################################################################################
#######################################################################################################################

set filtImgGM [b2_itkGradientMagnitude $TestImageID data-type= float-single]
set outputGM [b2_measure_image_mask $brainMask $filtImgGM]

set known_outputGM  {
{Mean 9.799931537724994} {Mean-Absolute-Deviation 6.275541437401253} {Variance 78.796879954905947} {Standard-Deviation 8.876760667884763} {Skewness 2.703072601928417} {Kurtosis 8.591359420126601} {Minimum 0.000000000000000} {Maximum 107.379516601562500}
}
CoreMeasuresEpsilonTest "itkGradientMagnitude float-single image" 0.0001 $known_outputGM $outputGM $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $filtImgGM ] != -1 } ] $ModuleName "Destroying image $filtImgGM"
#######################################################################################################################
#######################################################################################################################

set outerMask [b2_threshold_image $TestImageID 80]
set innerMask [b2_not_mask $outerMask]
set outerImage [b2_sum_masks [list $outerMask]]
set innerDistImg [b2_itkDanielssonDistanceMap $outerImage data-type= float-single]

set outputInnerDist [b2_measure_image_mask $innerMask $innerDistImg]

set known_outputInnerDist  {
{Mean 2.525002166077090} {Mean-Absolute-Deviation 1.264209186921797} {Variance 2.579158823594116} {Standard-Deviation 1.605975972296633} {Skewness 1.864088753700850} {Kurtosis 1.666157270114108} {Minimum 1.000000000000000} {Maximum 11.357816696166992}
}
CoreMeasuresEpsilonTest "itkDanielssonDistanceMap float-single image" 0.0001 $known_outputInnerDist $outputInnerDist $LogFile $ModuleName

ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $innerDistImg ] != -1 } ] $ModuleName "Destroying image $innerDistImg"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $outerImage ] != -1 } ] $ModuleName "Destroying image $outerImage"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $outerMask ] != -1 } ] $ModuleName "Destroying mask $outerMask"
ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $innerMask ] != -1 } ] $ModuleName "Destroying mask $innerMask"
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################
#######################################################################################################################

puts $output
puts $output4F
puts $output4B
puts $outputDG
puts $outputSig
puts $outputMed
puts $outputGAD
puts $outputBilat
puts $outputGM
puts $outputInnerDist

# Clean up Files
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_mask $brainMask ] != -1 } ] $ModuleName "Destroying mask $brainMask"
    ReportTestStatus $LogFile  [ expr { [ b2_destroy_image $TestImageID ] != -1 } ] $ModuleName "Destroying image $TestImageID"


# Free memory
      return [ StopModule  $LogFile $ModuleName ]
}

