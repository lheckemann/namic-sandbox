INCLUDE_DIRECTORIES(
  ${TensorDemonsRegistration_SOURCE_DIR}/Original/Cxx
  ${TensorDemonsRegistration_BINARY_DIR}/Original/Cxx
  )

CONFIGURE_FILE(
    ${TensorDemonsRegistration_SOURCE_DIR}/Original/Cxx/vtkAGConfigure.h.in 
    ${TensorDemonsRegistration_BINARY_DIR}/Original/Cxx/vtkAGConfigure.h 
  )

ADD_SUBDIRECTORY(Cxx)
ADD_SUBDIRECTORY(Testing)
