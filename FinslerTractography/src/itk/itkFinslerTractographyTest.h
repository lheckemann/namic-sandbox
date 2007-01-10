#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/* User Parameters */
bool writeReferenceImageToFile = true;
bool writeTensorsImageToFile = true;
bool writeFAImageToFile = true;
bool writeRAImageToFile = true;

/* ITK Headers */
#include "itkDiffusionTensor3DReconstructionImageFilter.h"
#include "itkTensorFractionalAnisotropyImageFilter.h"
#include "itkTensorRelativeAnisotropyImageFilter.h"
#include "itkVectorImage.h"
#include "itkNrrdImageIO.h"
#include "itkMetaDataDictionary.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include <iostream>

/* ITK Typedefs */
const unsigned int Dimension = 3;
unsigned int numberOfImages = 0;
bool readb0 = false;
double b0 = 0;
  
typedef unsigned short                            PixelType;
typedef itk::VectorImage< PixelType, Dimension >  ImageType;
typedef itk::ImageFileReader< ImageType >         ReaderType;
typedef itk::DiffusionTensor3DReconstructionImageFilter< 
  PixelType, PixelType, double > TensorReconstructionImageFilterType;
typedef itk::ImageRegionConstIteratorWithIndex<
  ImageType > NormalIteratorType;
typedef itk::ImageDirectionalConstIteratorWithIndex<
  ImageType > DirectionalIteratorType;
  
typedef itk::Image< PixelType, Dimension > ReferenceImageType;
typedef ReferenceImageType                 GradientImageType;

typedef itk::ImageRegionConstIterator<
  ImageType >     DWIIteratorType;
typedef itk::ImageRegionIterator<
  GradientImageType >  IteratorType;

typedef itk::ImageFileWriter<
  GradientImageType >      GradientWriterType;
typedef itk::ImageFileWriter< 
  TensorReconstructionImageFilterType::OutputImageType >
  TensorWriterType;
typedef TensorReconstructionImageFilterType::TensorPixelType
  TensorPixelType;
typedef TensorPixelType::RealValueType  RealValueType;

typedef itk::Image< RealValueType, Dimension >  FAImageType;
typedef itk::TensorFractionalAnisotropyImageFilter< 
  TensorReconstructionImageFilterType::OutputImageType,
  FAImageType > FAFilterType;
typedef itk::ImageFileWriter<
  FAFilterType::OutputImageType >  FAWriterType;

typedef itk::Image< RealValueType, Dimension > RAImageType;
typedef itk::TensorRelativeAnisotropyImageFilter< 
  TensorReconstructionImageFilterType::OutputImageType,
  RAImageType > RAFilterType;
typedef itk::ImageFileWriter<
  RAFilterType::OutputImageType >  RAWriterType;


