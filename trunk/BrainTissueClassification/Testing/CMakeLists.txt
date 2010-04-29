#
# Test CLI executables
#
include_regular_expression("^.*$")

enable_testing()

find_package(Slicer3 REQUIRED NO_DEFAULT_PATH)
include(${Slicer3_USE_FILE})

find_package(GenerateCLP REQUIRED)
if(GenerateCLP_FOUND)
  include(${GenerateCLP_USE_FILE})
endif(GenerateCLP_FOUND)

include_directories(${TissueClassification_SOURCE_DIR})
include_directories(${TissueClassification_BINARY_DIR}/Testing)
include_directories(${TissueClassification_BINARY_DIR})
include_directories(${ModuleDescriptionParser_SOURCE_DIR})
include_directories (${TCLAP_SOURCE_DIR}/include)

#
# Slicer3 is needed to resolve shared libraries
#
set(Slicer3_EXE ${Slicer3_DIR}/Slicer3 )
set(TEST_DATA "${TissueClassification_SOURCE_DIR}/Data")
set(TEMP "${Slicer3_DIR}/Testing/Temporary")

##################################
# TissueClassification tests
add_test(TissueClassificationTest ${Slicer3_EXE} --launch TissueClassificationTest
    ModuleEntryPoint
    -b 3 -c 3
    ${TEST_DATA}/OAS1-0001-T1.nrrd ${TEST_DATA}/OAS1-0001-MASK.nrrd
    ${TEMP}/TissueMap.nrrd ${TEMP}/BiasField.nrrd 
  )

set (CLP TissueClassification)
add_executable(${CLP}Test ${CLP}Test.cxx)
add_dependencies(${CLP}Test ${CLP})
target_link_libraries(${CLP}Test ${CLP}Lib)

