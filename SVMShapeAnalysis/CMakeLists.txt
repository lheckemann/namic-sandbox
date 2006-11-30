PROJECT(MIT-Golland)

# single directory for all libs and binaries
SET (LIBRARY_OUTPUT_PATH
  ${PROJECT_BINARY_DIR}/bin
  CACHE PATH
  "Single directory for all libraries.")
SET (EXECUTABLE_OUTPUT_PATH
  ${PROJECT_BINARY_DIR}/bin
  CACHE PATH
  "Single directory for all executables.")
MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH
  EXECUTABLE_OUTPUT_PATH
)

# set ITK directory
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
  "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)

# information for testing
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
ENABLE_TESTING()

# set SVM include directory
FIND_PATH (SVM_INCLUDE_DIRECTORY itkSupportVectorMachine.h
  ${CMAKE_SOURCE_DIR}/Code/SVM/)
INCLUDE_DIRECTORIES(${SVM_INCLUDE_DIRECTORY})

# set SVM I/O include directory
FIND_PATH (SVMIO_INCLUDE_DIRECTORY itkSVMFileReader.h
  ${CMAKE_SOURCE_DIR}/Code/SVMIO/)
INCLUDE_DIRECTORIES(${SVMIO_INCLUDE_DIRECTORY})

# set project library source directory
FIND_PATH (LIB_SRC_INCLUDE_DIRECTORY JackknifeClassificationErrorEstimatorBase.h
  ${CMAKE_SOURCE_DIR}/Code/Libs/)
INCLUDE_DIRECTORIES(${LIB_SRC_INCLUDE_DIRECTORY})

# set project application source directory
FIND_PATH (APP_SRC_INCLUDE_DIRECTORY SVMApplicationUtils.h
  ${CMAKE_SOURCE_DIR}/Code/Apps/)
INCLUDE_DIRECTORIES(${APP_SRC_INCLUDE_DIRECTORY})

# set testing source directory
FIND_PATH (TESTING_SRC_DIRECTORY SVMTestingUtils.h
  ${CMAKE_SOURCE_DIR}/Testing/)
INCLUDE_DIRECTORIES(${TESTING_SRC_DIRECTORY})

# source paths should not usually be changed
MARK_AS_ADVANCED (
  SVM_INCLUDE_DIRECTORY
  SVMIO_INCLUDE_DIRECTORY
  LIB_SRC_INCLUDE_DIRECTORY
  APP_SRC_INCLUDE_DIRECTORY
  TESTING_SRC_DIRECTORY
)

# add application utils (common to all apps) to a library
ADD_LIBRARY(SVMApplicationUtils 
  ${APP_SRC_INCLUDE_DIRECTORY}/SVMApplicationUtils.cxx
  )

# add testing utils (common to all tests) to a library
ADD_LIBRARY(SVMTestingUtils
  ${TESTING_SRC_DIRECTORY}/SVMTestingUtils.cxx
)

# build svmTrain application
ADD_EXECUTABLE(svmTrain 
  ${APP_SRC_INCLUDE_DIRECTORY}/SVMTrainApplication.cxx)
TARGET_LINK_LIBRARIES(svmTrain
  SVMApplicationUtils
  ITKIO
  ITKCommon
)

# build svmClassify application
ADD_EXECUTABLE(svmClassify 
  ${APP_SRC_INCLUDE_DIRECTORY}/SVMTestApplication.cxx)
TARGET_LINK_LIBRARIES(svmClassify
  SVMApplicationUtils
  ITKIO
  ITKCommon
  )

# build svmJackknife application
ADD_EXECUTABLE(svmJackknife 
  ${APP_SRC_INCLUDE_DIRECTORY}/SVMJackknifeClassificationErrorApplication.cxx)
TARGET_LINK_LIBRARIES(svmJackknife
  SVMApplicationUtils
  ITKIO
  ITKCommon
  )

# build svmDDCalculator application
ADD_EXECUTABLE(svmDDCalculator
  ${APP_SRC_INCLUDE_DIRECTORY}/SVMDiscriminativeDirectionCalculatorApplication.cxx)
TARGET_LINK_LIBRARIES(svmDDCalculator
  SVMApplicationUtils
  ITKIO
  ITKCommon
  )

# add test for svm training, testing, jackknife, discriminative dirs
ADD_EXECUTABLE(SVMTests
  ${TESTING_SRC_DIRECTORY}/SVMTests.cxx)
TARGET_LINK_LIBRARIES(SVMTests
  SVMTestingUtils
  ITKIO
  ITKCommon
  )
ADD_TEST(SVMTests 
  ${EXECUTABLE_OUTPUT_PATH}/SVMTests)

