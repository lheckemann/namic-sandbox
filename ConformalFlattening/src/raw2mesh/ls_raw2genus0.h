// Disable warning for long symbol names in this file only
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

//
// ITK headers
//
#include <iostream>
#include <fstream>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkFileOutputWindow.h"

#include "itkTPGACLevelSetImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"

// IMAGE STUFF
typedef itk::Image< unsigned char, 3 > InputImageType;
typedef itk::Image< unsigned long, 3 > LongImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > ThresholdFilterType;
typedef itk::ConnectedComponentImageFilter< InputImageType, LongImageType > ConnectedComponentType;
typedef itk::RelabelComponentImageFilter< LongImageType, InputImageType > RelabelType;
typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< InputImageType> IteratorType;
typedef itk::ImageFileWriter< InputImageType > WriterType;

typedef itk::CurvatureAnisotropicDiffusionImageFilter< InputImageType, InputImageType > SmoothingFilterType;
typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType, InputImageType > GradientFilterType;
typedef itk::SigmoidImageFilter< InputImageType, InputImageType > SigmoidFilterType;
typedef itk::FastMarchingImageFilter< InternalImageType, InternalImageType > FastMarchingFilterType;
typedef itk::TPGACLevelSetImageFilter< InternalImageType, InternalImageType > TPGACFilterType;
typedef FastMarchingFilterType::NodeContainer  NodeContainer;
typedef FastMarchingFilterType::NodeType       NodeType;


