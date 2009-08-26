PROJECT( SlicerExecutionModel )

##NOTE:  Only build these items if they have not already been identified.
##       This allows for these items to be skipped if building against Slicer versions
SET(BUILD_TESTING off)
SET(TCLAP_DIR ${CMAKE_CURRENT_BINARY_DIR}/tclap CACHE PATH "Location of TCLAPConfig.cmake" FORCE)
ADD_SUBDIRECTORY(tclap)

SET(ModuleDescriptionParser_DIR ${CMAKE_CURRENT_BINARY_DIR}/ModuleDescriptionParser CACHE PATH "Location of ModuleDescriptionParserConfig.cmake" FORCE)
ADD_SUBDIRECTORY(ModuleDescriptionParser)

SET(GenerateCLP_DIR ${CMAKE_CURRENT_BINARY_DIR}/GenerateCLP CACHE PATH "Location of GenerateCLPConfig.cmake" FORCE)
ADD_SUBDIRECTORY(GenerateCLP)
