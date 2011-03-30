set DEFREGEVAL_BIN_DIR=c:/devel/DefRegEval-bin/bin/Release
set SLICER_BIN_DIR=C:/devel/Slicer-3.6/Slicer3-build
set INPUT_DATA_DIR=c:/MedicalImages/APT-MRI/20110330_SliceToVolSimulation/inputs
set RESULT_DATA_DIR=c:\MedicalImages\APT-MRI\20110330_SliceToVolSimulation\results

REM Remember the home directory to return there after script completion
set HOME_DIR=%CD%
REM Slicer paths define ITK and VTK paths. Add them to the system path only once (checked by verifying that the Slicer-added TCLLIBPATH variable is present alrady or not)  
if not defined TCLLIBPATH call %SLICER_BIN_DIR%/bin/release/Slicer3SetupPaths.bat

goto :case7

:case1
REM ---------------------------------------------------------
SET CASENO=01
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case2
REM ---------------------------------------------------------
SET CASENO=02
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 3 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case3
REM ---------------------------------------------------------
SET CASENO=03
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 18 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case4
REM ---------------------------------------------------------
SET CASENO=04
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 6 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case5
REM ---------------------------------------------------------
SET CASENO=05
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 6 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case6
REM ---------------------------------------------------------
SET CASENO=06
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------



:case7
REM ---------------------------------------------------------
SET CASENO=07
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 18 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case8
REM ---------------------------------------------------------
SET CASENO=08
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case9
REM ---------------------------------------------------------
SET CASENO=09
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------


:end
chdir /d %HOME_DIR%
