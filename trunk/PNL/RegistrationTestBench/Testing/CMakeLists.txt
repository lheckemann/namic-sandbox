
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
SET(TEST_DATA_ROOT ${CMAKE_SOURCE_DIR})
SET(TEMP ${CMAKE_BINARY_DIR}/Testing/Temporary)

FIND_PATH(PNL_BRAIN_DATA_DIR README.txt)
FIND_PATH(SLICER_SOURCE_DIR CMakeLists.txt)

IF( !SLICER_SOURCE_DIR )
  MESSAGE(FATAL_ERROR "Path to Slicer source must be specified to use access images")
ENDIF( !SLICER_SOURCE_DIR )

SET(SLICER_TESTING_DATA ${SLICER_SOURCE_DIR}/Testing/Data/Input)

INCLUDE_DIRECTORIES( 
  ${SLICER_SOURCE_DIR}/Base/CLI
  ${SLICER_SOURCE_DIR}/Libs/ModuleDescriptionParser/
 )

ADD_EXECUTABLE(RigidRegistrationSlicerModuleTest RigidRegistrationSlicerModuleTest.cxx)
TARGET_LINK_LIBRARIES(RigidRegistrationSlicerModuleTest
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )

ADD_EXECUTABLE(RigidRegistrationSlicerModuleTestLibDriver RigidRegistrationSlicerModuleTestLibDriver.cxx)
TARGET_LINK_LIBRARIES(RigidRegistrationSlicerModuleTestLibDriver 
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  RigidRegistrationSlicerModuleTestLib
  )

ADD_LIBRARY(RigidRegistrationSlicerModuleTestLib SHARED RigidRegistrationSlicerModuleTestLib.cxx)
TARGET_LINK_LIBRARIES(RigidRegistrationSlicerModuleTestLib
  ITKStatistics ITKBasicFilters ITKNumerics ITKIO ITKCommon
  )


MACRO(REGISTER_IMAGES CASEID DIR1 DATASET1 EXTENSION1 DIR2 DATASET2 EXTENSION2)

ADD_TEST(RegistrationBSpline_${CASEID}
  ${EXECUTABLE_OUTPUT_PATH}/DeformableRegistration01
  ${DIR1}/${DATASET1}.${EXTENSION1}
  ${DIR2}/${DATASET2}.${EXTENSION2}
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceAfter.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DifferenceBefore.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField.mha
  1    # Use explicit PDF derivatives
  1    # Cache BSpline weights
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
   5   # Number of BSpline nodes in coarse grid
  10   # Number of BSpline nodes in coarse grid
  1.0  # Initial step length
  200  # Number of iterations
  )

ADD_TEST(BSplineResampleImage_${CASEID}
  ${EXECUTABLE_OUTPUT_PATH}/BSplineResampleImage
  ${DIR1}/${DATASET1}.${EXTENSION1}
  ${DIR2}/${DATASET2}.${EXTENSION2}
  ${TEMP}/finalAffineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/finalBSplineTransform_${DATASET1}_${DATASET2}.txt
  ${TEMP}/${DATASET2}_RegisteredTo_${DATASET1}.mha
  ${TEMP}/${DATASET2}_BSplineTo_${DATASET1}_DeformationField2.mha
  )

ENDMACRO(REGISTER_IMAGES)




#
#  Note that the REGISTER_IMAGES macro expects filenames
#  without extension. It internally assumes .nrrd.
#

REGISTER_IMAGES( T2-DWI
  ${PNL_BRAIN_DATA_DIR}  01053-dwi-filt-Ed-Basline-masked  nrrd
  ${PNL_BRAIN_DATA_DIR}  01053-t2w-realign-masked          nrrd
  )

REGISTER_IMAGES( T2-T2
  ${PNL_BRAIN_DATA_DIR}  01053-t2w-realign-masked   nrrd
  ${PNL_BRAIN_DATA_DIR}  01031-t2w                  nrrd
  )

#
# cross verification
#
REGISTER_IMAGES( T2-T2-Back
  ${PNL_BRAIN_DATA_DIR}  01031-t2w   nrrd
  ${TEMP}  01031-t2w_BSplineTo_01053-t2w-realign-masked  mha
  )

REGISTER_IMAGES( T1-T2
  ${PNL_BRAIN_DATA_DIR}  01053-t1w-realign      nrrd
  ${PNL_BRAIN_DATA_DIR}  01031-t2w              nrrd
  )

REGISTER_IMAGES( T1-T1
  ${PNL_BRAIN_DATA_DIR}  01053-t1w              nrrd
  ${PNL_BRAIN_DATA_DIR}  01031-t1w              nrrd
  )


#
# Other parameters from 
# http://www.na-mic.org/Wiki/index.php/Summer2009:Registration_reproducibility_in_Slicer
#
MACRO(REGISTRATION_THREADS_TEST NTHREADS)
ADD_TEST( RigidRegSlicerModuleTest_${NTHREADS}
  ${EXECUTABLE_OUTPUT_PATH}/RigidRegistrationSlicerModuleTest
  ${SLICER_TESTING_DATA}/MRMeningioma0.nrrd   #  fixed image
  ${SLICER_TESTING_DATA}/MRMeningioma1.nrrd   #  moving image
  10  #  histogrambins
  10  #  spatialsamples
  5   #  iterations 
  0.01  # learningrate 
  100 # translationscale 
  ${TEMP}/RigidRegistrationTransform-CLI_${NTHREADS}.txt  # outputtransform 
  ${NTHREADS}  # number of threads
  )
ENDMACRO(REGISTRATION_THREADS_TEST)


REGISTRATION_THREADS_TEST( 1 )
REGISTRATION_THREADS_TEST( 2 )
REGISTRATION_THREADS_TEST( 3 )
REGISTRATION_THREADS_TEST( 4 )
REGISTRATION_THREADS_TEST( 5 )
REGISTRATION_THREADS_TEST( 6 )

