
INCLUDE_DIRECTORIES(
  ${CUDA_TOOLKIT_INCLUDE}
  ${CUDA_CUT_INCLUDE_DIR}
  )

CUDA_ADD_EXECUTABLE(Convolution3x3 
  Convolution3x3.cu
  ImageUtils.cpp
  SimpleCudaGLWindow.cpp
  main.cpp
  )
