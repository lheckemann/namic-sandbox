#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/* Parameters */
const unsigned int Dimension = 3;

/* ITK Headers */
#include "itkVectorImage.h"
#include "itkNrrdImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include <iostream>

/* ITK Typedefs */
typedef unsigned short                         InputPixelType;
typedef itk::VectorImage< InputPixelType,
                          Dimension >          InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;

typedef itk::ImageRegionConstIteratorWithIndex<
  InputImageType > NormalIteratorType;
typedef itk::ImageDirectionalConstIteratorWithIndex<
  InputImageType > DirectionalIteratorType;
  
typedef float                          OutputPixelType;
typedef itk::Image< OutputPixelType, Dimension >       ArrivalTimesImageType;
typedef itk::VectorImage< OutputPixelType, Dimension > ArrivalVectorsImageType;

typedef itk::ImageFileWriter< ArrivalTimesImageType >   ArrivalTimesWriterType;
typedef itk::ImageFileWriter< ArrivalVectorsImageType > ArrivalVectorsWriterType;
