project(RegistrationMetrics)

cmake_minimum_required(VERSION 2.6)

# Slicer3
find_package(Slicer3 REQUIRED)
include(${Slicer3_USE_FILE})

# GenerateCLP
#find_package(GenerateCLP REQUIRED)
#include(${GenerateCLP_USE_FILE})

# Default install prefix
slicer3_set_default_install_prefix_for_external_projects()


#####################
# DSC plugin

set(CLP RegistrationMetrics)
set(${CLP}_SOURCE ${CLP}.cxx)
generateclp(${CLP}_SOURCE ${CLP}.xml)

add_executable(${CLP} ${${CLP}_SOURCE})
slicer3_set_plugins_output_path(${CLP})
target_link_libraries(${CLP} ${ITK_LIBRARIES})

#####################


#####################
# Tests for the plugin

enable_testing()


#####################
