// Include files for the application
#pragma once
//#pragma warning(disable : 4005)
//#define _CRT_SECURE_NO_DEPRECATE
//#pragma warning( default : 4005)
#pragma warning(disable : 4996)


#include "itkCommand.h"
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "itkNumericSeriesFileNames.h"
#include "itkRawImageIO.h"
#include "itkVTKImageIO.h"
#include "itkTIFFImageIO.h"
#include "itkAnalyzeImageIO.h"
#include "itkGDCMImageIO.h"
#include "itkScalarImageToListAdaptor.h"
#include "itkListSampleToHistogramGenerator.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkAnisotropicFourthOrderLevelSetImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkFastMarchingImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"
#include "itkSimilarity2DTransform.h"
#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkConstantBoundaryCondition.h"
#include "itkBSplineInterpolateImageFunction.h"
#include <itkConnectedComponentImageFilter.h>
#include <itkSignedDanielssonDistanceMapImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNormalizeImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

// registration headers
#include <itkImageRegistrationMethod.h>
#include <itkTranslationTransform.h>
#include <itkAffineTransform.h>
#include <itkCenteredRigid2DTransform.h>
#include <itkIdentityTransform.h>
#include <itkImageToImageMetric.h>
#include <itkMutualInformationImageToImageMetric.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkKullbackLeiblerCompareHistogramImageToImageMetric.h>
#include <itkMeanSquaresImageToImageMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkGradientDescentOptimizer.h>
#include <itkConjugateGradientOptimizer.h>
#include <itkAmoebaOptimizer.h>
#include <itkSingleValuedNonLinearOptimizer.h>
#include <itkGaussianKernelFunction.h>
#include "itkCommand.h"

// vtk connection headers
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "vtkImageImport.h"
#include "vtkImageExport.h"

// the required header files for the vtk classes we are using
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include "vtkImageData.h"
#include "vtkImageShiftScale.h"
#include "vtkImageNoiseSource.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeTextureMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkImageWriter.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolume.h>
#include <vtkCamera.h>
#include <vtkMarchingContourFilter.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataWriter.h>
#include <vtkBoxWidget.h>
#include <vtkMassProperties.h>
#include <vtkCurvatures.h>
#include <vtkLookupTable.h>
#include <vtkVRMLExporter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataReader.h>
#include <vtkQuadricDecimation.h>
#include <vtkCurvatures.h>
#include <vtkMassProperties.h>

#include <iostream>
#include <ostream>
#include <string>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <math.h>



using namespace std;
#include "types.h"
#include "itk2vtk.h"

