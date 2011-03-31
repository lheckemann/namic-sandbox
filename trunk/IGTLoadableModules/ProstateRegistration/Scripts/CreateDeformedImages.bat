REM Remember the home directory to return there after script completion
set HOME_DIR=%CD%

goto :case21

:case01
REM ---------------------------------------------------------
SET CASENO=01
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case02
REM ---------------------------------------------------------
SET CASENO=02
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case03
REM ---------------------------------------------------------
SET CASENO=03
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case04
REM ---------------------------------------------------------
SET CASENO=04
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 7 0.45 --SupportMaterialProperties 5 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case05
REM ---------------------------------------------------------
SET CASENO=05
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 20 30 4 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case06
REM ---------------------------------------------------------
SET CASENO=06
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 20 30 4 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case07
REM ---------------------------------------------------------
SET CASENO=07
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 20 30 4 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case08
REM ---------------------------------------------------------
SET CASENO=08
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 5 45 2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case09
REM ---------------------------------------------------------
SET CASENO=09
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 5 45 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case10
REM ---------------------------------------------------------
SET CASENO=10
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition -40 -10 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 5 45 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case11
REM ---------------------------------------------------------
SET CASENO=11
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case12
REM ---------------------------------------------------------
SET CASENO=12
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case13
REM ---------------------------------------------------------
SET CASENO=13
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case14
REM ---------------------------------------------------------
SET CASENO=14
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -10 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 7 0.45 --SupportMaterialProperties 5 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case15
REM ---------------------------------------------------------
SET CASENO=15
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -20 30 4 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case16
REM ---------------------------------------------------------
SET CASENO=16
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -20 30 4 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case17
REM ---------------------------------------------------------
SET CASENO=17
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -20 30 4 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case18
REM ---------------------------------------------------------
SET CASENO=18
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -5 45 2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case19
REM ---------------------------------------------------------
SET CASENO=19
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -5 45 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case20
REM ---------------------------------------------------------
SET CASENO=20
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 30 -20 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -5 45 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case21
REM ---------------------------------------------------------
SET CASENO=21
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 0 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case22
REM ---------------------------------------------------------
SET CASENO=22
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 0 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case23
REM ---------------------------------------------------------
SET CASENO=23
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 0 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case24
REM ---------------------------------------------------------
SET CASENO=24
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 0 40 2 --SolverTimeSteps=5 --OrganMaterialProperties 7 0.45 --SupportMaterialProperties 5 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case25
REM ---------------------------------------------------------
SET CASENO=25
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -20 20 4 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case26
REM ---------------------------------------------------------
SET CASENO=26
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -20 20 4 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case27
REM ---------------------------------------------------------
SET CASENO=27
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce -20 20 4 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case28
REM ---------------------------------------------------------
SET CASENO=28
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 25 15 -2 --SolverTimeSteps=5 --OrganMaterialProperties 21 0.45 --SupportMaterialProperties 11 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case29
REM ---------------------------------------------------------
SET CASENO=29
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 25 15 -2 --SolverTimeSteps=5 --OrganMaterialProperties 15 0.45 --SupportMaterialProperties 9 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------

:case30
REM ---------------------------------------------------------
SET CASENO=30
mkdir %RESULT_DATA_DIR%\%CASENO%
chdir /d %RESULT_DATA_DIR%\%CASENO%
%DEFREGEVAL_BIN_DIR%/ExtractSurface.exe --OrganImageInputFn=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOpSeg.mha --CombinedSurfMeshOutputFn=DeformImage-PreOpSegSurface.smesh --SupportPosition -5 -85 160 --SupportRadius=120 --OrganImageThreshold=0.5
%DEFREGEVAL_BIN_DIR%/tetgen.exe -NEFpq1.6a100.0gA DeformImage-PreOpSegSurface.smesh
%DEFREGEVAL_BIN_DIR%/CreateFemModel.exe --CombinedVolMeshInputFn=DeformImage-PreOpSegSurface.1.mesh --FemModelOutputFn=DeformImage-FemDeform.feb --SupportFixedPosition 0 -5 160 --SupportFixedRadius=65 --ProbePosition1 -12 -120 80 --ProbePosition2 -12 -120 200 --ProbeRadius=10 --ProbeForce 25 15 -2 --SolverTimeSteps=5 --OrganMaterialProperties 11 0.45 --SupportMaterialProperties 7 0.3
%DEFREGEVAL_BIN_DIR%/FEBio -i DeformImage-FemDeform.feb
%DEFREGEVAL_BIN_DIR%/GetDeformedImage.exe --inputModel=DeformImage-FemDeform.plt --referenceImage=%INPUT_DATA_DIR%/DefRegEvalSampleData/B2_PreOp.mha --outputImage=DeformImage-B2_IntraOpSeg.mha --outputFullImage=DeformImage-B2_IntraOp.mha --outputDeformationFieldImage=DeformImage-B2_IntraOpDefField.mha --outputVolumeMesh=DeformImage-B2_IntraOpVolumeMesh.vtu
REM ---------------------------------------------------------


:end
chdir /d %HOME_DIR%
