#include "itkImage.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"

#include "itkRescaleIntensityImageFilter.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

//#include "itkImageRegionIterator.h"

#include "vnl/vnl_math.h"

typedef float           InternalPixelType;
const     unsigned int    Dimension = 3;

typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;

typedef  itk::ImageFileReader< InternalImageType > ReaderType;
typedef  itk::ImageFileWriter< InternalImageType > WriterType;

typedef   itk::CurvatureAnisotropicDiffusionImageFilter< InternalImageType, InternalImageType >  SmoothingFilterType;
typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter< InternalImageType, InternalImageType >  GradientFilterType;

typedef   itk::SigmoidImageFilter< InternalImageType, InternalImageType >  SigmoidFilterType;
