#
# List of source files
#

INCLUDE_DIRECTORIES(
      ${MultiImageRegistration_SOURCE_DIR}/Source/Common
)
#ADD_SUBDIRECTORY(Common)

SET(Groupwise_SRCS
  GroupwiseRegistrationExample.cxx
  GradientDescentLineSearchOptimizer.h
  GradientDescentLineSearchOptimizer.cxx
 )
GENERATECLP( Groupwise_SRCS  GroupwiseRegistrationExample.xml)

ADD_EXECUTABLE(GroupwiseRegistration ${Groupwise_SRCS})
TARGET_LINK_LIBRARIES(GroupwiseRegistration ITKCommon ITKNumerics ITKIO ITKBasicFilters ITKStatistics )

ADD_LIBRARY(MultiImageRegistrationLIB itkMultiImageRegistrationFilter.cxx
GradientDescentLineSearchOptimizer.cxx)
TARGET_LINK_LIBRARIES(MultiImageRegistrationLIB ITKAlgorithms ITKIO)

INSTALL(TARGETS MultiImageRegistrationLIB GroupwiseRegistration
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
