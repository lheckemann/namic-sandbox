#include "itkImage.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkImageRegionIterator.h"

#include "vnl/vnl_math.h"

typedef float           InternalPixelType;
const     unsigned int    Dimension = 3;

typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;

typedef unsigned char OutputPixelType;
typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

typedef itk::BinaryThresholdImageFilter< InternalImageType, OutputImageType > ThresholdingFilterType;


typedef  itk::ImageFileReader< InternalImageType > ReaderType;
typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

//  typedef itk::RescaleIntensityImageFilter< InternalImageType, OutputImageType >   CastFilterType;

typedef  itk::FastMarchingImageFilter< InternalImageType, InternalImageType >    FastMarchingFilterType;

typedef  itk::GeodesicActiveContourLevelSetImageFilter< InternalImageType, InternalImageType >    GeodesicActiveContourFilterType;

typedef FastMarchingFilterType::NodeContainer  NodeContainer;
typedef FastMarchingFilterType::NodeType       NodeType;

typedef itk::ImageFileWriter< InternalImageType > InternalWriterType;

