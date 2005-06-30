# This project is designed to be built outside the Insight source tree.
PROJECT(blockmatching)

INCLUDE_DIRECTORIES(
  ${PointLandmarkDetection_SOURCE_DIR}/Code
  ${PointLandmarkDetection_SOURCE_DIR}/Tests
  )


FOREACH(test 
    itkTensorUnitaryFunctionImageFiltersTest
    itkDiadicProductOfGradientRecursiveGaussianFilterTest
    itkOuterProductImageFilterTest
    )
  ADD_EXECUTABLE(${test} ${test}.cxx)
  TARGET_LINK_LIBRARIES(${test}
    ITKCommon
    ITKIO
    ITKBasicFilters
    ITKNumerics )
  ADD_TEST(${test} "${EXECUTABLE_OUTPUT_PATH}/${test}")
ENDFOREACH(test)
