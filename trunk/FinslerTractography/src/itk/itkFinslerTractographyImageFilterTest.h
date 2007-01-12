#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/* User Parameters */
const unsigned int Dimension = 3;
bool readb0 = true;
bool writeReferenceImageToFile = true;
bool writeTensorsImageToFile = true;
bool writeFAImageToFile = true;
bool writeRAImageToFile = true;

/* ITK Headers */
#include "itkVectorImage.h"
#include "itkNrrdImageIO.h"
#include "itkMetaDataDictionary.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkDiffusionTensor3DReconstructionImageFilter.h"
#include "itkTensorFractionalAnisotropyImageFilter.h"
#include "itkTensorRelativeAnisotropyImageFilter.h"
#include "itkImageFileWriter.h"
#include <iostream>

#include "itkFinslerTractographyImageFilter.h"

/* ITK Typedefs */
unsigned int numberOfImages = 0;
double b0 = 0;
  
typedef unsigned short    DWIPixelComponentType;
typedef itk::VectorImage< DWIPixelComponentType,
                          Dimension >  DWIImageType;
typedef itk::ImageRegionConstIterator<
  DWIImageType >     DWIIteratorType;

typedef itk::ImageFileReader< DWIImageType >  ReaderType;

typedef itk::DiffusionTensor3DReconstructionImageFilter< 
  DWIPixelComponentType, DWIPixelComponentType,
  double > TensorReconstructionImageFilterType;
  
typedef itk::Image< DWIPixelComponentType,
                    Dimension > ReferenceImageType;
typedef ReferenceImageType      GradientImageType;
typedef itk::ImageRegionIterator<
  GradientImageType >  GradientIteratorType;
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

/** Finsler Tractography ITK Typedefs */
typedef double  ArrivalTimesPixelType;
typedef itk::Image< ArrivalTimesPixelType, Dimension > ArrivalTimesImageType;
typedef itk::FinslerTractographyImageFilter<
  DWIImageType, ArrivalTimesImageType > FinslerTractographyFilterType;
