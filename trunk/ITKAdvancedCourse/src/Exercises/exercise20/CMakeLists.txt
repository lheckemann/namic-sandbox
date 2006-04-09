# This project is designed to be built outside the Insight source tree.
PROJECT(Exercise20)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_EXECUTABLE(registration       registration.cxx       )
ADD_EXECUTABLE(registrationAnswer registrationAnswer.cxx )

TARGET_LINK_LIBRARIES(registration       ITKIO ITKNumerics ITKCommon )
TARGET_LINK_LIBRARIES(registrationAnswer ITKIO ITKNumerics ITKCommon )
