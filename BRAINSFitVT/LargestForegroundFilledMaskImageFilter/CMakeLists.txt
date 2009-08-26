project(LargestForegroundFilledMaskImageFilter)
cmake_minimum_required(VERSION 2.6)

find_package(ITK)
if(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
else(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build BRAINSTracer without ITK.  Please se ITK_DIR.")
endif(ITK_FOUND)

set(TestName TestLargestForegroundFilledMaskImageFilter)

add_executable(${TestName}
${TestName}.cxx
)

target_link_libraries(${TestName}
${ITK_LIBRARIES}
)
