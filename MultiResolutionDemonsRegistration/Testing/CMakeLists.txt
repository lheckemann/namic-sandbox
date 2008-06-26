
ADD_EXECUTABLE(MultiResolutionDemonsRegistration MultiResolutionDemonsRegistration.cxx )

TARGET_LINK_LIBRARIES(MultiResolutionDemonsRegistration ITKStatistics ITKIO ITKCommon)


FIND_PATH(ICBM_DATA 
  target_icbm.mha
  )

ADD_TEST(MultiResolutionDemonsRegistrationTest01 
  MultiResolutionDemonsRegistration
  ${ICBM_DATA}/target_icbm.mha
  ${ICBM_DATA}/source_icbm.mha
  MultiResolutionDemonsRegistrationTest01.mha
  MultiResolutionDemonsRegistrationTest01.vtk
  )
  
