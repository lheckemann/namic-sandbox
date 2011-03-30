set DATA_DIR=c:/MedicalImages/APT-MRI/20110322_FemSimulationsHadi
set SLICER_BIN_DIR=C:/devel/Slicer-3.6/Slicer3-build
set PROSTATE_REG_BIN_DIR=c:/devel/ProstateRegistration-bin
set DEFREGEVAL_BIN_DIR=c:/devel/DefRegEval-bin

set SLICE_REG_EXE=%PROSTATE_REG_BIN_DIR%/SliceToVolumeRegistration-bin/lib/Slicer3/Plugins/Release\SliceRegistration.exe
set EVAL_REG_ERROR_EXE=%DEFREGEVAL_BIN_DIR%/bin/Release/EvaluateRegistrationError.exe 

REM call %SLICER_BIN_DIR%/bin/release/Slicer3SetupPaths.bat

REM Registration with volumetric mask
REM %SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_1.mha --volumeImage %DATA_DIR%/preop_B2.mha --volumeImageMask %DATA_DIR%/simIntraop_B2_1_prostate_volume_rect_label.nrrd --transform RigidTransform.txt --deformable --gridspacing 30 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha 

REM Registration without mask
REM %SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_1.mha --volumeImage %DATA_DIR%/preop_B2.mha --transform RigidTransform.txt --deformable --gridspacing 30 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha 

REM Compute difference of ground truth and computed deformation field
REM %EVAL_REG_ERROR_EXE% --deformationReference=%DATA_DIR%/gtField_B2_1.mha --deformationComputed=DefField.mha --deformationDifferenceVector=DefFieldDiff.mha --deformationDifferenceMagnitude=DefFieldDiffMag.mha --mask=%DATA_DIR%/simIntraop_B2_1_prostate_contour_label.nrrd

REM ----------------------- Case 2 -----------------

REM Registration with volumetric mask
REM %SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_2.mha --volumeImage %DATA_DIR%/preop_B2.mha --transform RigidTransform.txt --deformable --gridspacing 30 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha --volumeImageMask %DATA_DIR%/simIntraop_B2_1_prostate_volume_rect_label.nrrd
REM %SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_2.mha --volumeImage %DATA_DIR%/preop_B2.mha --transform RigidTransform.txt --deformable --gridspacing 20 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha --volumeImageMask %DATA_DIR%/simIntraop_B2_1_prostate_volume_rect_label.nrrd
REM %SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_2.mha --volumeImage %DATA_DIR%/preop_B2.mha --transform RigidTransform.txt --deformable --gridspacing 15 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha --volumeImageMask %DATA_DIR%/simIntraop_B2_1_prostate_volume_rect_label.nrrd
%SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_2.mha --volumeImage %DATA_DIR%/preop_B2.mha --transform RigidTransform.txt --deformable --gridspacing 40 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha --volumeImageMask %DATA_DIR%/simIntraop_B2_1_prostate_volume_rect_label.nrrd

REM Registration without mask
REM %SLICE_REG_EXE% --sliceImage %DATA_DIR%/simIntraop_B2_2.mha --volumeImage %DATA_DIR%/preop_B2.mha --transform RigidTransform.txt --deformable --gridspacing 10 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha 

REM Compute difference of ground truth and computed deformation field
REM %EVAL_REG_ERROR_EXE% --deformationReference=%DATA_DIR%/gtField_B2_2.mha --deformationComputed=DefField.mha --deformationDifferenceVector=DefFieldDiff.mha --deformationDifferenceMagnitude=DefFieldDiffMag.mha

REM ----------------------- New simulated data set -----------------

REM Registration with volumetric mask
%SLICE_REG_EXE% --volumeImage c:/devel/DefRegEval-bin/bin/Testing/DeformImage-B2_IntraOp.mha --sliceImage c:/devel/DefRegEval/data/Images/Prostate/B2_PreOp.mha --transform RigidTransform.txt --deformable --gridspacing 30 --maxdeformation 10 --defTransform DefTransform.txt --resampledmovingfilename DefVolume.mha --deffieldfilename DefField.mha

REM Compute difference of ground truth and computed deformation field
%EVAL_REG_ERROR_EXE% --deformationReference=c:/devel/DefRegEval-bin/bin/Testing/DeformImage-B2_IntraOpDefField.mha --deformationComputed=DefField.mha --deformationDifferenceVector=DefFieldDiff.mha --deformationDifferenceMagnitude=DefFieldDiffMag.mha


