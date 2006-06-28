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
#include "itkLabelStatisticsImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkFileOutputWindow.h"

// IMAGE STUFF
typedef itk::Image< unsigned char, 3 > InputImageType;
typedef itk::Image< unsigned long, 3 > LongImageType;
typedef itk::ImageFileReader< InputImageType > ReaderType;
typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > ThresholdFilterType;
typedef itk::ConnectedComponentImageFilter< InputImageType, LongImageType > ConnectedComponentType;
typedef itk::RelabelComponentImageFilter< LongImageType, InputImageType > RelabelType;
typedef itk::InvertIntensityImageFilter< InputImageType, InputImageType > InvertFilterType;
typedef itk::LabelStatisticsImageFilter<InputImageType, InputImageType> LabelStatisticsImageFilterType;
typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > BinaryThresholdFilterType;
typedef itk::ImageRegionConstIterator< InputImageType > ConstIteratorType;
typedef itk::ImageRegionIterator< InputImageType> IteratorType;
typedef itk::ImageFileWriter< InputImageType > WriterType;
