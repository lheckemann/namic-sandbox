SET( TestDataDir ${DefRegEval_SOURCE_DIR}/data )
 
# Test shape model learning
ADD_TEST(CreateShapeModelTest1
  ${CXX_TEST_PATH}/CreateShapeModel
  --inputTrainingShapesDir=${TestDataDir}/ShapeModels/ProstateShapeModelTraining
  --outputShapeModelDir=${CXX_TEST_PATH}/Testing/CreateShapeModelTest1/ProstateShapeModel  
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
    ${TestDataDir}/ShapeModels/ProstateShapeModel/${file}
    ${CXX_TEST_PATH}/Testing/CreateShapeModelTest1/ProstateShapeModel/${file}  
    )
ENDFOREACH(file)

# Test surface extraction and smoothing (generation of organ and support surface model from a segmented image)
ADD_TEST(DeformImageTest1-ExtractSurface
  ${CXX_TEST_PATH}/ExtractSurface
  --OrganImageInputFn=${TestDataDir}/Images/Prostate/B2_PreOpSeg.mha
  --CombinedSurfMeshOutputFn=${CXX_TEST_PATH}/DeformImage/PreOpSegSurface.smesh
  --SupportPosition -5 -85 160
  --SupportRadius=120
  --OrganImageThreshold=0.5
  )
  
# Test creating volumetric mesh (tetrahedron mesh in abaqus format) from surface mesh
ADD_TEST(DeformImageTest1-tetgen
  ${CXX_TEST_PATH}/tetgen
  -NEFpq1.6a100.0gA
  ${CXX_TEST_PATH}/DeformImage/PreOpSegSurface.smesh
  )

# Test creation of FEM model for deformation simulation
ADD_TEST(DeformImageTest1-CreateFemModel
  ${CXX_TEST_PATH}/CreateFemModel
  --CombinedVolMeshInputFn=${CXX_TEST_PATH}/DeformImage/PreOpSegSurface.1.mesh
  --FemModelOutputFn=${CXX_TEST_PATH}/DeformImage/FemDeform.feb
  --SupportFixedPosition -40 -10 160
  --SupportFixedRadius=65
  --ProbePosition1 -12 -120 80
  --ProbePosition2 -12 -120 200
  --ProbeRadius=10
  --ProbeForce 10 40 2
  --SolverTimeSteps=10
  )

# Test computation of deformation using the FEM model
ADD_TEST(DeformImageTest1-FEBio
  ${CXX_TEST_PATH}/FEBio
  -i ${CXX_TEST_PATH}/DeformImage/FemDeform.feb
  )

# Test getting deformed image"
ADD_TEST(DeformImageTest1-GetDeformedImage
  ${CXX_TEST_PATH}/GetDeformedImage
  --inputModel=${CXX_TEST_PATH}/DeformImage/FemDeform.plt
  --referenceImage=${TestDataDir}/Images/Prostate/B2_PreOp.mha
  --outputImage=${CXX_TEST_PATH}/DeformImage/B2_IntraOpSeg.mha
  --outputFullImage=${CXX_TEST_PATH}/DeformImage/B2_IntraOp.mha
  --outputDeformationFieldImage=${CXX_TEST_PATH}/DeformImage/B2_IntraOpDefField.mha
  --outputVolumeMesh=${CXX_TEST_PATH}/DeformImage/B2_IntraOpVolumeMesh.vtu
  )
