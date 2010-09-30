project(RegistrationTools)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0012 NEW)
ENDIF(COMMAND CMAKE_POLICY)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

link_directories(${ModuleDescriptionParser_BINARY_DIR})

#set(CLP Cast)
#set(${CLP}_SOURCE ${CLP}.cxx)
#message("Cast source is ${${CLP}_SOURCE}, should be ${CLP}.cxx")
#generateclp(${CLP}_SOURCE ${CLP}.xml)
#add_executable(${CLP} ${${CLP}_SOURCE})
#target_link_libraries(${CLP} ITKIO ITKCommon ITKNumerics)

set(CLP MetricExploration)
set(${CLP}_SOURCE ${CLP}.cxx)
message("Cast source is ${${CLP}_SOURCE}, should be ${CLP}.cxx")
generateclp(${CLP}_SOURCE ${CLP}.xml)
add_executable(${CLP} ${${CLP}_SOURCE})
target_link_libraries(${CLP} ITKIO ITKCommon ITKNumerics)

set(CLP GeneratePerturbedTransforms)
set(${CLP}_SOURCE ${CLP}.cxx)
message("Cast source is ${${CLP}_SOURCE}, should be ${CLP}.cxx")
generateclp(${CLP}_SOURCE ${CLP}.xml)
add_executable(${CLP} ${${CLP}_SOURCE})
target_link_libraries(${CLP} ITKIO ITKCommon ITKNumerics)

add_executable(VersorTest VersorTest.cxx)
target_link_libraries(VersorTest ITKIO ITKCommon)
