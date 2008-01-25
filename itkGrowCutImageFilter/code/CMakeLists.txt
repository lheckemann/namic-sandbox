PROJECT(itkGrowCutImageFilterTest)


FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR.")
ENDIF(ITK_FOUND)


#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project


#########Set any libraries that your project depends on.
# the order IS sensitive.
SET(Libraries
ITKCommon
ITKBasicFilters
ITKIO
)



SET(CurrentExe "itkGrowCutImageFilterTest")
ADD_EXECUTABLE(${CurrentExe} itkGrowCutImageFilterTest.cxx) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})
