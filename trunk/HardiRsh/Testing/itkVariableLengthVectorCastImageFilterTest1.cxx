#include <iostream>

#include "itkVariableLengthVector.h"
#include "itkImage.h"
#include "itkVariableLengthVectorCastImageFilter.h"
#include "TestingMacros.h"

int main( int argc, char * argv[])
{
  const unsigned int  Dimension = 2;
  typedef signed short                                    InputComponentType;
  typedef unsigned char                                   OutputComponentType;
  typedef itk::VariableLengthVector<InputComponentType>   InputPixelType;
  typedef itk::VariableLengthVector<OutputComponentType>  OutputPixelType;
  typedef itk::Image< InputPixelType, Dimension >         InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >        OutputImageType; 

  typedef itk::VariableLengthVectorCastImageFilter<
    InputImageType, OutputImageType >               FilterType;

  FilterType::Pointer filter = FilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( filter );

  return EXIT_SUCCESS;
}
