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
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"

// IMAGE STUFF
typedef itk::Image< unsigned char, 3 > InputImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::ConnectedComponentImageFilter< InputImageType, InputImageType > ConnectedComponentType;
typedef itk::RelabelComponentImageFilter< InputImageType, InputImageType > RelabelType;
typedef itk::LabelStatisticsImageFilter<InputImageType, InputImageType> LabelStatisticsImageFilterType;
typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > BinaryThresholdFilterType;
typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< InputImageType > IteratorType;
typedef itk::ImageFileWriter< InputImageType > WriterType;
