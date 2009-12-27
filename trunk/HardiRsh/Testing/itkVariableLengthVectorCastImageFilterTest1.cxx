#include <iostream>

#include "itkVariableLengthVector.h"
#include "itkImage.h"
#include "itkVariableLengthVectorCastImageFilter.h"
#include "TestingMacros.h"

int main( int argc, char * argv[])
{
  const unsigned int  Dimension = 2;
  typedef signed short                              ComponentType;
  typedef itk::VariableLenghtVector<ComponentType>  PixelType;
  typedef itk::Image< PixelType, Dimension >        InputImageType;
  typedef itk::Image< ComponentType, Dimension >    OutputImageType; 

  typedef itk::VariableLengthVectorCastImageFilter<
    InputImageType, OutputImageType >               FilterType;

  FilterType::Pointer filter = FilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( filter );

  return EXIT_SUCCESS;
}
