#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/* Parameters */
const unsigned int Dimension = 3;

/* ITK Headers */
#include "itkVectorImage.h"
#include "itkVector.h"
#include "itkNrrdImageIO.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageDirectionalConstIteratorWithIndex.h"
#include "itkImageFileWriter.h"
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
typedef itk::Image< OutputPixelType, Dimension > ArrivalTimesImageType;
typedef itk::Image< itk::Vector< OutputPixelType, Dimension >, Dimension >
                    ArrivalVectorsImageType;

typedef itk::ImageFileWriter< ArrivalTimesImageType >   ArrivalTimesWriterType;
typedef itk::ImageFileWriter< ArrivalVectorsImageType > ArrivalVectorsWriterType;
