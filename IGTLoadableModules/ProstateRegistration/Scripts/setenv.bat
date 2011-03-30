set DEFREGEVAL_BIN_DIR=c:/devel/DefRegEval-bin/bin/Release
set SLICER_BIN_DIR=C:/devel/Slicer-3.6/Slicer3-build
set INPUT_DATA_DIR=c:/MedicalImages/APT-MRI/20110330_SliceToVolSimulation/inputs
set RESULT_DATA_DIR=c:\MedicalImages\APT-MRI\20110330_SliceToVolSimulation\results

REM ---------------------------

REM Slicer paths define ITK and VTK paths. Add them to the system path only once (checked by verifying that the Slicer-added TCLLIBPATH variable is present alrady or not)  
if not defined TCLLIBPATH call %SLICER_BIN_DIR%/bin/release/Slicer3SetupPaths.bat

set PROSTATE_REG_BIN_DIR=c:/devel/ProstateRegistration-bin

set SLICE_REG_EXE=%PROSTATE_REG_BIN_DIR%/SliceToVolumeRegistration-bin/lib/Slicer3/Plugins/Release\SliceRegistration.exe
set EVAL_REG_ERROR_EXE=%DEFREGEVAL_BIN_DIR%/EvaluateRegistrationError.exe 
