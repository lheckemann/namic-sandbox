include_directories( ${MoreSearchRegistration_SOURCE_DIR} )

set(MoreSearchTests_SOURCE
    FixedRotationSimilarity3DTransformTest.cxx
    EulerSimilarity3DTransformTest.cxx
    DecomposedAffine3DTransformTest.cxx
    FRPROptimizerTest.cxx)

create_test_sourcelist(MoreSearchTestsList
                       MoreSearchTests.cxx
                       ${MoreSearchTests_SOURCE})

ADD_EXECUTABLE(MoreSearchTests ${MoreSearchTestsList} )
TARGET_LINK_LIBRARIES(MoreSearchTests ${ITK_LIBRARIES})

add_test(FixedRotationSimilarity3DTransformTest
         MoreSearchTests
         FixedRotationSimilarity3DTransformTest)

add_test(EulerSimilarity3DTransformTest
         MoreSearchTests
         EulerSimilarity3DTransformTest)

add_test(DecomposedAffine3DTransformTest
         MoreSearchTests
         DecomposedAffine3DTransformTest)

add_test(FRPROptimizerTest
         MoreSearchTests
         FRPROptimizerTest)
