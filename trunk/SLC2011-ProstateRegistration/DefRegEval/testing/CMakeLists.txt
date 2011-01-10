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
