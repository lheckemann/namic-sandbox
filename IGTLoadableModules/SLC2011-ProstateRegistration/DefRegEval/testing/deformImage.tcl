set outputDir "c:/devel/DefRegEval-bin/bin/Testing"
set binDir "c:/devel/DefRegEval-bin/bin/Release"

set dataDir "c:/devel/DefRegEval/data"
set inputGrayscaleImage "$dataDir/Images/Prostate/B2_PreOp.mha"
set inputSegmentedImage "$dataDir/Images/Prostate/B2_PreOpSeg.mha"

######################
# Set up logging

set ::logFilename "$outputDir/DeformImage.log"
proc log { txt } {
  puts "$txt"
  set logFile [open $::logFilename a]
  puts $logFile "$txt"
  close $logFile
}

###################### 
# Execute data processing

log "Start"

log "Extract and smooth surface mesh from segmented image"
set startTime "[clock seconds]"
exec "$binDir/ExtractSurface.exe" \
    --OrganImageInputFn=$inputSegmentedImage \
    --OrganSurfMeshOutputFn=$outputDir/DeformImage_PreDeform.stl \
    --CombinedSurfMeshOutputFn=$outputDir/DeformImage_PreDeform.smesh \
    --SupportPosition -5 -85 160 \
    --SupportRadius=120 \
    --OrganImageThreshold=0.5
log " [expr [clock seconds]-$startTime]s"

log "Create volumetric mesh (tetrahedron mesh in abaqus format) from surface mesh"
set startTime "[clock seconds]"
exec "$binDir/tetgen.exe" -NEFpq1.6a100.0gA $outputDir/DeformImage_PreDeform.smesh
log " [expr [clock seconds]-$startTime]s"

log "Create FEM model for deformation simulation"
set startTime "[clock seconds]"
exec "$binDir/CreateFemModel" \
  --CombinedVolMeshInputFn=$outputDir/DeformImage_PreDeform.1.mesh \
  --FemModelOutputFn=$outputDir/DeformImage_FemModel.feb \
  --SupportFixedPosition -40 -10 160 \
  --SupportFixedRadius=65 \
  --ProbePosition1 -8 -116 40 \
  --ProbePosition2 -8 -116 180 \
  --ProbeRadius=12 \
  --ProbeForce -2 40 2 \
  --SolverTimeSteps=10
log " [expr [clock seconds]-$startTime]s"
    
  #--ProbePosition1 -8 -116 40 \
  #--ProbePosition2 -8 -116 180 \
  #--ProbeForce -2 10 2 \
  
    # --ProbePosition1 -12 -120 80 \
    # --ProbeForce -8 40 2 \  

log "Compute deformation using the FEM model"
set startTime "[clock seconds]"
exec "$binDir/FEBio.exe" -i $outputDir/DeformImage_FemModel.feb
log " [expr [clock seconds]-$startTime]s"

log "Get deformed image"
set startTime "[clock seconds]"
exec "$binDir/GetDeformedImage.exe" \
  --inputModel=$outputDir/DeformImage_FemModel.plt \
  --referenceImage=$inputGrayscaleImage \
  --outputImage=$outputDir/DeformImage_PostDeformSeg.mha \
  --outputFullImage=$outputDir/DeformImage_PostDeform.mha \
  --outputDeformationFieldImage=$outputDir/DeformImage_DefField.mha \
  --outputVolumeMesh=$outputDir/DeformImage_PostDeform.vtu
log " [expr [clock seconds]-$startTime]s"

log "Done. Exit now."
