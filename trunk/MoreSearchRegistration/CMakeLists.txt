PROJECT( MoreSearchRegistration )

  cmake_minimum_required(VERSION 2.6)
        
  # Find ITK
  find_package( ITK REQUIRED )
  include( ${ITK_USE_FILE} )
    
  # Build the command line
  add_executable( MoreSearch MoreSearch.cxx )
  target_link_libraries( MoreSearch ITKStatistics ITKIO ITKNumerics ITKTransformIOReview )
       
  # Build the command line
  add_executable( ExploreObjectiveFunction ExploreObjectiveFunction.cxx )
  target_link_libraries( ExploreObjectiveFunction ITKStatistics ITKIO ITKNumerics ITKTransformIOReview )

  enable_testing()
  subdirs(Testing)
       
