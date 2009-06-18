
INCLUDE_DIRECTORIES(
  ${CUDA_TOOLKIT_INCLUDE}
  ${CUDA_CUT_INCLUDE_DIR}
  )

IF(WIN32)
  CUDA_ADD_EXECUTABLE(BrightnessContrast 
    BrightnessContrast.cu
    ImageUtils.cpp
    SimpleCudaGLWindow.cpp
    main.cpp
    )
ELSE(WIN32)
  CUDA_ADD_EXECUTABLE(BrightnessContrast 
    BrightnessContrast.cu
    ImageUtils.cpp
    main.cpp
    )
ENDIF(WIN32)
