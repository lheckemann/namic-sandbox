include_directories( ${MoreSearchRegistration_SOURCE_DIR} )

set(MoreSearchTests_SOURCE
    FixedRotationSimilarity3DTransformTest.cxx
    FRPROptimizerTest.cxx)

create_test_sourcelist(MoreSearchTestsList
                       MoreSearchTests.cxx
                       ${MoreSearchTests_SOURCE})

ADD_EXECUTABLE(MoreSearchTests ${MoreSearchTestsList} )
TARGET_LINK_LIBRARIES(MoreSearchTests ${ITK_LIBRARIES})

add_test(FixedRotationSimilarity3DTransformTest
         MoreSearchTests
         FixedRotationSimilarity3DTransformTest)

add_test(FRPROptimizerTest
         MoreSearchTests
         FRPROptimizerTest)
