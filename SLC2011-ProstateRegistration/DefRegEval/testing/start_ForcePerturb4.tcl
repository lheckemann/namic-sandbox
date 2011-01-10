#######################################
# DefRegEval uses LPS coordinate system (according to ITK and DICOM standard).
# If metaimages are opened in Slicer they are correctly converted to LPS.
# To properly display STL files in Slicer an LPStoRAS transform shall be applied
# on the model (-1 0 0 0; 0 -1 0 0; 0 0 1 0; 0 0 0 1).
# LPS: ITK, DICOM, meta files, ITKSnap ITK
# RAS: Slicer, NIFTI, ITKSnap NIFTI
# AnatomicalOrientation tag (such as RAI) in meta files are ignored (they are
# assumed to be LPS)
#######################################

proc GenerateShapeImage { outputFn } {
  log "GenerateShapeImage"
  set startTime "[clock seconds]"
  exec "$::binDir/GenerateShapeImage.exe" --output=$outputFn --shape-model-dir=$::shapeModelDir >> $::logFilename
  log " [expr [clock seconds]-$startTime]s"
}

proc CreateVolumeMesh { inputFn outputFn } {
  log "CreateVolumeMesh"
  set startTime "[clock seconds]"
  set ::env(TCL_LIBRARY) $::netGenDir/lib/tcl8.3
  set ::env(TIX_LIBRARY) $::netGenDir/lib/tix8.2
  set result [ catch {exec "$::netGenDir/ng431.exe" -geofile=$inputFn -meshfile=$outputFn -fine -batchmode} ]
  if {$result} {
    log " Warning: CreateVolumeMesh returned with error (usually it is not a problem)"
  }
  log " [expr [clock seconds]-$startTime]s"
}

proc ExtractSurface { inputImageFn outputSurfaceFn smoothedOutputImageFn outputSupportSurfaceFn } {
  log "ExtractSurface"
  set startTime "[clock seconds]"
  exec "$::binDir/ExtractSurface.exe" \
    --input=$inputImageFn \
    --outputObject=$outputSurfaceFn --outputObjectImage=$smoothedOutputImageFn \
    --supportPosition 67 71 36 \
    --supportRadius=40 \
    --outputSupport=$outputSupportSurfaceFn
  log " [expr [clock seconds]-$startTime]s"
}

proc CreateFemModel { objectVolumeMeshFn supportVolumeMeshFn outputModelFn fx fy fz} {
  log "CreateFemModel" 
  log " f = $fx $fy $fz"
  set startTime "[clock seconds]"
  exec "$::binDir/CreateFemModel" --inputObject=$objectVolumeMeshFn --inputSupport=$supportVolumeMeshFn \
    --output=$outputModelFn \
    --supportPosition 67 71 36 \
    --supportRadius=40 \
    --supportFixedPosition 67 1 36 \
    --supportFixedRadius=50 \
    --probePosition1 67 95 10 \
    --probePosition2 67 95 45 \
    --probeRadius=8 \
    --probeForce $fx $fy $fz \
    --solverTimeSteps=$::solverTimeSteps
  log " [expr [clock seconds]-$startTime]s"
}

proc SolveFemModel { inputFn } {
  log "SolveFemModel"
  set startTime "[clock seconds]"
  exec "$::feBioDir/FEBio.exe" -i $inputFn
  log " [expr [clock seconds]-$startTime]s"
} 

proc AddImageSegmentationError { inputImageFn outputImageFn px py pz dx dy dz rx ry rz { deformationFieldFn ""} } {
  log "AddImageSegmentationError"
  log " p = $px $py $pz"
  log " d = $dx $dy $dz"
  log " r = $rx $ry $rz"
  set startTime "[clock seconds]"    
  exec "$::binDir/AddImageSegmentationError.exe" --input=$inputImageFn --output=$outputImageFn \
    --deformationPosition $px $py $pz --deformationVector $dx $dy $dz \
    --deformationRegionSize $rx $ry $rz \
    --outputDeformationField=$deformationFieldFn 
  log " [expr [clock seconds]-$startTime]s"
} 

proc GetDeformedImage { inputModelFn referenceImageFn outputDeformedImageFn outputDeformedMeshFn outputDeformationFieldImageFn} {
  log "GetDeformedImage"
  set startTime "[clock seconds]"
  exec "$::binDir/GetDeformedImage.exe" --inputModel=$inputModelFn --referenceImage=$referenceImageFn \
    --outputImage=$outputDeformedImageFn \
    --outputDeformationFieldImage=$outputDeformationFieldImageFn \
    --outputVolumeMesh=$outputDeformedMeshFn
  log " [expr [clock seconds]-$startTime]s"
}

proc RegisterImages { fixedImageFn movingImageFn outputRegisteredImageFn outputDeformationFieldImageFn } {
  log "RegisterImages"
  set startTime "[clock seconds]"
  exec "$::binDir/RegisterImages.exe" \
    --fixedImage=$fixedImageFn --movingImage=$movingImageFn \
    --outputRegisteredImage=$outputRegisteredImageFn \
    --outputDeformationField=$outputDeformationFieldImageFn \
    --numberOfIterations=20
  log " [expr [clock seconds]-$startTime]s"
}  

proc EvaluateRegistrationError { femDefFieldFn regDefFieldFn maskImageFn diffDefFieldFn diffMagDefFieldFn } {
  log "EvaluateRegistrationError"
  set startTime "[clock seconds]"
  exec "$::binDir/EvaluateRegistrationError.exe" \
  --deformationReference=$femDefFieldFn --deformationComputed=$regDefFieldFn \
  --deformationDifferenceVector=$diffDefFieldFn --deformationDifferenceMagnitude=$diffMagDefFieldFn \
  --mask=$maskImageFn
  log " [expr [clock seconds]-$startTime]s"
} 
 

#######################################



set solverTimeSteps 5

proc log { txt } {
  puts "$txt"
  set logFile [open $::logFilename a]
  puts $logFile "$txt"
  close $logFile
}

# Source: http://code.activestate.com/recipes/143083/ 
# {{{ Recipe 143083 (r1): Normally Distributed Random Numbers 
# The following procedure generates two independent normally distributed 
# random numbers with mean 0 and vaviance stdDev^2.
proc randNormal {mean stdDev} {
    global dRandNormal1
    global dRandNormal2
    set u1 rand()
    set u2 rand()
    return [expr $mean + $stdDev * sqrt(-2 * log($u1)) * cos(2 * 3.14159 * $u2)]
    # the second number is not used
    # set dRandNormal2 [expr $stdDev * sqrt(-2 * log($u1)) * sin(2 * 3.14159 * $u2)]
}

proc randomBetween { minVal maxVal } {
  return [expr $minVal+($maxVal-$minVal)*rand() ]
}

#######################################

proc start { workDir { perturbForce 0 } { perturbSegmentation 0 } } {

  set ::logFilename "$workDir/results.txt"

  set rawShapeImageFn "$workDir/rawShapeImage.mha"
  set preOpImageFn "$workDir/PreOpImage.mha"
  set preOpSegmentedImageFn "$workDir/PreOpSegmentedImage.mha"
  set preOpSurfaceMeshFn "$workDir/PreOpSurfaceMesh.stl"
  set preOpVolumeMeshFn "$workDir/PreOpVolumeMesh.vol"
  set supportSurfaceMeshFn "$workDir/SupportSurfaceMesh.stl"
  set supportVolumeMeshFn "$workDir/SupportVolumeMesh.vol"
  set femModelFn "$workDir/FemDeform.feb"
  set femResultFn "$workDir/FemDeform.plt"
  set intraOpImageFn "$workDir/IntraOpImage.mha"
  set intraOpMeshFn "$workDir/DeformFieldVolumeMesh.vtu"
  set intraOpSegmentedImageFn "$workDir/IntraOpSegmentedImage.mha"
  set femDefFieldFn "$workDir/DeformFieldFemImage.mha"
  set regDefFieldFn "$workDir/DeformFieldRegImage.mha"
  set intraOpRegisteredImageFn "$workDir/IntraOpSegmentedRegisteredImage.mha"
  set diffDefFieldFn "$workDir/DeformFieldDiffImage.mha"
  set diffMagDefFieldFn "$workDir/DeformFieldDiffMagImage.mha"

  log "Start"

  # Multiply value by 100 to allow some interpolation (allow some interpolation, don't work always with purely binary images)
  GenerateShapeImage $rawShapeImageFn 

  # Extract and smooth contour
  ExtractSurface $rawShapeImageFn $preOpSurfaceMeshFn $preOpImageFn $supportSurfaceMeshFn

  # Create a tetrahedron mesh in abaqus format
  CreateVolumeMesh $preOpSurfaceMeshFn $preOpVolumeMeshFn
  CreateVolumeMesh $supportSurfaceMeshFn $supportVolumeMeshFn

  # TODO: Randomize force params
  
  if {$perturbForce} {
    set fx [randNormal 0 5]
    set fy [randNormal -60 10]
    set fz [randNormal 0 2]
  } else {
    set fx 0
    set fy 0
    set fz -40
  }  
  CreateFemModel $preOpVolumeMeshFn $supportVolumeMeshFn $femModelFn $fx $fy $fz
  
  # $::femResultFn is generated from $::femModelFn
  SolveFemModel $femModelFn
     
  GetDeformedImage $femResultFn $preOpImageFn $intraOpImageFn $intraOpMeshFn $femDefFieldFn 
  
  if {$perturbSegmentation} {  
    set preOpSegErrorX [randNormal 0 0.5]
    set preOpSegErrorY [randNormal 0 0.5]
    set preOpSegErrorZ [randNormal 0 1.5]
    set intraOpSegErrorX [randomNormal 0 1]
    set intraOpSegErrorY [randomNormal 0 1]
    set intraOpSegErrorZ [randomNormal 0 3]
  } else {
    set preOpSegErrorX 0
    set preOpSegErrorY 0
    set preOpSegErrorZ 0
    set intraOpSegErrorX 0
    set intraOpSegErrorY 0
    set intraOpSegErrorZ 0
  } 
  # error in apex (65, 73, 21) and base (65, 67, 54)
  AddImageSegmentationError $preOpImageFn $preOpSegmentedImageFn 65 73 21  $preOpSegErrorX $preOpSegErrorY $preOpSegErrorZ 25 25 8
  AddImageSegmentationError $intraOpImageFn $intraOpSegmentedImageFn 65 73 21 $intraOpSegErrorX $intraOpSegErrorY $intraOpSegErrorZ  25 25 6
  
  RegisterImages $preOpSegmentedImageFn $intraOpSegmentedImageFn $intraOpRegisteredImageFn $regDefFieldFn

  EvaluateRegistrationError $femDefFieldFn $regDefFieldFn $preOpImageFn $diffDefFieldFn $diffMagDefFieldFn 

  log "End"
}

proc GetDirectoryNameForCase { groupId caseId } {
  set dirname "$::workDir/${groupId}_[format %03i $caseId]"
  file mkdir "$dirname"
  return $dirname 
}


###########################################################

set workDir "c:/Users/andras/Documents/DefRegEval/test"
set binDir "c:/Users/andras/devel/DefRegEval-bin/Release"

set shapeModelDir "$workDir/ShapeModel" 
set netGenDir "$binDir/ng431_rel"
set feBioDir "$binDir/FEBio1p2"

#set workDir "c:/tmp/pre"
#set binDir "c:/devel/DefRegEval-bin/debug"

set groupId ForcePerturb4 
set numberOfExecutions 50

for {set i 1} {$i<=$numberOfExecutions} {incr i} {
  set dirName [GetDirectoryNameForCase $groupId $i]
  puts "-------------- $dirName -----------------"
  # dirname, perturb force, perturb segmentation
  if { [ catch { start $dirName 1 0 } ] } {
    puts "  Error: could not complete the simulation for case $groupId $i"
  }
} 
