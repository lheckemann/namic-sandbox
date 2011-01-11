SET( TestSrcDataDir ${DefRegEval_SOURCE_DIR}/data )
SET( TestDstDataDir ${CXX_TEST_PATH}/Testing )

MAKE_DIRECTORY(${TestDstDataDir})

# Test shape model learning
ADD_TEST(CreateShapeModelTest1
  ${EXECUTABLE_OUTPUT_PATH}/CreateShapeModel
  --inputTrainingShapesDir=${TestSrcDataDir}/ShapeModels/ProstateShapeModelTraining
  --outputShapeModelDir=${TestDstDataDir}/CreateShapeModelTest1/ProstateShapeModel  
  )

# Check if the generated shape model matches the reference data
SET(CreateShapeModelTestResultFiles
  eigenValues.txt
  MeanShape.mha
  MeanShape_DMap.mha
  VariationMode_01.mha
  VariationMode_02.mha
  VariationMode_03.mha 
  )
FOREACH(file ${CreateShapeModelTestResultFiles}) 
  ADD_TEST(CreateShapeModelTest1-${file}
    ${CMAKE_COMMAND} -E compare_files
    ${TestSrcDataDir}/ShapeModels/ProstateShapeModel/${file}
    ${TestDstDataDir}/CreateShapeModelTest1/ProstateShapeModel/${file}  
    )
ENDFOREACH(file)

# Test surface extraction and smoothing (generation of organ and support surface model from a segmented image) coords in LPS
ADD_TEST(DeformImageTest1-ExtractSurface
  ${EXECUTABLE_OUTPUT_PATH}/ExtractSurface
  --OrganImageInputFn=${TestSrcDataDir}/Images/Prostate/B2_PreOpSeg.mha
  --CombinedSurfMeshOutputFn=${TestDstDataDir}/DeformImage-PreOpSegSurface.smesh
  --SupportPosition -5 -85 160
  --SupportRadius=120
  --OrganImageThreshold=0.5
  )
  
# Test creating volumetric mesh (tetrahedron mesh in abaqus format) from surface mesh
ADD_TEST(DeformImageTest1-tetgen
  ${EXECUTABLE_OUTPUT_PATH}/tetgen
  -NEFpq1.6a100.0gA
  ${TestDstDataDir}/DeformImage-PreOpSegSurface.smesh
  )

# Test creation of FEM model for deformation simulation
ADD_TEST(DeformImageTest1-CreateFemModel
  ${EXECUTABLE_OUTPUT_PATH}/CreateFemModel
  --CombinedVolMeshInputFn=${TestDstDataDir}/DeformImage-PreOpSegSurface.1.mesh
  --FemModelOutputFn=${TestDstDataDir}/DeformImage-FemDeform.feb
  --SupportFixedPosition -40 -10 160
  --SupportFixedRadius=65
  --ProbePosition1 -12 -120 80
  --ProbePosition2 -12 -120 200
  --ProbeRadius=10
  --ProbeForce 10 40 2
  --SolverTimeSteps=5
  )

# Test computation of deformation using the FEM model
ADD_TEST(DeformImageTest1-FEBio
  ${EXECUTABLE_OUTPUT_PATH}/FEBio
  -i ${TestDstDataDir}/DeformImage-FemDeform.feb
  )

# Test getting deformed image"
ADD_TEST(DeformImageTest1-GetDeformedImage
  ${EXECUTABLE_OUTPUT_PATH}/GetDeformedImage
  --inputModel=${TestDstDataDir}/DeformImage-FemDeform.plt
  --referenceImage=${TestSrcDataDir}/Images/Prostate/B2_PreOp.mha
  --outputImage=${TestDstDataDir}/DeformImage-B2_IntraOpSeg.mha
  --outputFullImage=${TestDstDataDir}/DeformImage-B2_IntraOp.mha
  --outputDeformationFieldImage=${TestDstDataDir}/DeformImage-B2_IntraOpDefField.mha
  --outputVolumeMesh=${TestDstDataDir}/DeformImage-B2_IntraOpVolumeMesh.vtu
  )
