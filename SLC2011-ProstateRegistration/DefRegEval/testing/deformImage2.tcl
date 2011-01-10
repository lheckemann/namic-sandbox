set workDir "c:/devel/DefRegEval-bin/bin/Testing"
set binDir "c:/devel/DefRegEval-bin/bin/Release"
set dataDir "c:/devel/DefRegEval/data"
set febioDir "c:/devel/DefRegEval/tools/febio/Release"
set ::logFilename "deformimage.log"

######################

proc log { txt } {
  puts "$txt"
  set logFile [open $::logFilename a]
  puts $logFile "$txt"
  close $logFile
}

###################### 

log "Start"

log "Extract and smooth surface mesh from segmented image"
set startTime "[clock seconds]"
exec "$::binDir/ExtractSurface.exe" \
    --OrganImageInputFn=$dataDir/Images/Prostate/B2/PreOpSeg.mha \
    --CombinedSurfMeshOutputFn=$workDir/PreOpSegSurface.smesh \
    --SupportPosition -5 -85 160 \
    --SupportRadius=120 \
    --OrganImageThreshold=0.5
log " [expr [clock seconds]-$startTime]s"

log "Create volumetric mesh (tetrahedron mesh in abaqus format) from surface mesh"
set startTime "[clock seconds]"
exec "$::binDir/tetgen.exe" -NEFpq1.6a100.0gA $workDir/PreOpSegSurface.smesh
log " [expr [clock seconds]-$startTime]s"

log "Create FEM model for deformation simulation"
set startTime "[clock seconds]"
exec "$::binDir/CreateFemModel" \
  --CombinedVolMeshInputFn=$workDir/PreOpSegSurface.1.mesh \
  --FemModelOutputFn=$workDir/FemDeform.feb \
  --SupportFixedPosition -40 -10 160 \
  --SupportFixedRadius=65 \
  --ProbePosition1 -12 -120 80 \
  --ProbePosition2 -12 -120 200 \
  --ProbeRadius=10 \
  --ProbeForce 10 40 2 \
  --SolverTimeSteps=10
  
    # --ProbeForce -8 40 2 \
log " [expr [clock seconds]-$startTime]s"

log "Compute deformation using the FEM model"
set startTime "[clock seconds]"
exec "$::febioDir/FEBio.exe" -i $workDir/FemDeform.feb
log " [expr [clock seconds]-$startTime]s"

log "Get deformed image"
set startTime "[clock seconds]"
exec "$::binDir/GetDeformedImage.exe" \
  --inputModel=$workDir/FemDeform.plt \
  --referenceImage=$dataDir/Images/Prostate/B2/PreOp.mha \
  --outputImage=$workDir/B2_IntraOpSeg.mha \
  --outputFullImage=$workDir/B2_IntraOp.mha \
  --outputDeformationFieldImage=$workDir/B2_IntraOpDefField.mha \
  --outputVolumeMesh=$workDir/B2_IntraOpVolumeMesh.vtu
log " [expr [clock seconds]-$startTime]s"

log "Done. Exit now."
