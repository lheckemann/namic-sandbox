
INCLUDE_DIRECTORIES(
  ${CUDA_TOOLKIT_INCLUDE}
  ${CUDA_CUT_INCLUDE_DIR}
  )

CUDA_ADD_EXECUTABLE(ImageRotate 
  ImageRotate.cu
  ImageUtils.cpp
  SimpleCudaGLWindow.cpp
  main.cpp
  )
