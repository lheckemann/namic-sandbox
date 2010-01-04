#include <iostream>

#include "itkVariableLengthVector.h"
#include "itkVectorImage.h"
#include "itkVariableLengthVectorCastImageFilter.h"
#include "itkTestingMacros.h"

int main( int argc, char * argv[])
{
  const unsigned int  Dimension = 2;
  typedef signed short                                          InputComponentType;
  typedef itk::Vector< InputComponentType, 5 >                  InputPixelType;
  typedef unsigned char                                         OutputComponentType;
  typedef itk::Image< InputPixelType, Dimension >               InputImageType;
  typedef itk::VectorImage< OutputComponentType, Dimension >    OutputImageType; 

  typedef itk::VariableLengthVectorCastImageFilter<
    InputImageType, OutputImageType >               FilterType;

  FilterType::Pointer filter = FilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( filter );

  return EXIT_SUCCESS;
}
