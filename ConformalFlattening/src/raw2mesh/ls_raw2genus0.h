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

#include "itkRescaleIntensityImageFilter.h"
#include "itkTPGACLevelSetImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"

// IMAGE STUFF
typedef itk::Image< float, 3 > InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > ThresholdFilterType;
typedef itk::ConnectedComponentImageFilter< InputImageType, InputImageType > ConnectedComponentType;
typedef itk::RelabelComponentImageFilter< InputImageType, InputImageType > RelabelType;
typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< InputImageType> IteratorType;
typedef itk::ImageFileWriter< InputImageType > WriterType;

typedef itk::RescaleIntensityImageFilter< InputImageType, InputImageType > CastFilterType;
typedef itk::CurvatureAnisotropicDiffusionImageFilter< InputImageType, InputImageType > SmoothingFilterType;
typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< InputImageType, InputImageType > GradientFilterType;
typedef itk::SigmoidImageFilter< InputImageType, InputImageType > SigmoidFilterType;
typedef itk::FastMarchingImageFilter< InputImageType, InputImageType > FastMarchingFilterType;
typedef itk::TPGACLevelSetImageFilter< InputImageType, InputImageType > TPGACFilterType;
typedef FastMarchingFilterType::NodeContainer  NodeContainer;
typedef FastMarchingFilterType::NodeType       NodeType;
