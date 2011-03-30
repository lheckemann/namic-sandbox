REM Remember the home directory to return there after script completion
set HOME_DIR=%CD%

:case1
REM ---------------------------------------------------------
SET CASENO=01
chdir /d %RESULT_DATA_DIR%\%CASENO%
REM Registration with volumetric mask
%SLICE_REG_EXE% --volumeImage DeformImage-B2_IntraOp.mha --sliceImage %INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --rigidTransform ComputedRigidTransform.txt --rigidFieldImage ComputedRigidField.mha --resampledRigidMovingImage ComputedRigidVolume.mha --deformable --gridSpacing 30 --maxDeformation 10 --defTransform ComputedDefTransform.txt --resampledDefMovingImage ComputedDefVolume.mha --defFieldImageFilename ComputedDefField.mha
REM Compute difference of ground truth and computed deformation field
%EVAL_REG_ERROR_EXE% --deformationReference=DeformImage-B2_IntraOpDefField.mha --deformationComputed=ComputedDefField.mha --deformationDifferenceVector=DefFieldDiff.mha --deformationDifferenceMagnitude=DefFieldDiffMag.mha --mask=DeformImage-B2_IntraOpSeg.mha 
%EVAL_REG_ERROR_EXE% --deformationReference=DeformImage-B2_IntraOpDefField.mha --deformationComputed=ComputedRigidField.mha --deformationDifferenceVector=RigidFieldDiff.mha --deformationDifferenceMagnitude=RigidFieldDiffMag.mha --mask=DeformImage-B2_IntraOpSeg.mha
REM ---------------------------------------------------------

:end
chdir /d %HOME_DIR%
