/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/Crop.cxx $
  Language:  C++
  Date:      $Date: 2006-12-20 16:00:24 -0500 (Wed, 20 Dec 2006) $
  Version:   $Revision: 1892 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkSize.h"
#include "itkExtractImageFilter.h"

#include "itkPluginUtilities.h"
#include "CropCLP.h"

template<class T> int DoIt( int argc, char * argv[], T )
{

  PARSE_ARGS;

  typedef    T       InputPixelType;
  typedef    T       OutputPixelType;

  typedef itk::Image< InputPixelType,  3 >   InputImageType;
  typedef itk::Image< OutputPixelType, 3 >   OutputImageType;

  typedef itk::ImageFileReader< InputImageType >  ReaderType;
  typedef itk::ImageFileWriter< OutputImageType > WriterType;

  typedef itk::Size<3> SizeType;
  typedef itk::Index<3> IndexType;

  typedef itk::ExtractImageFilter<
    InputImageType, OutputImageType >  FilterType;

  typename ReaderType::Pointer reader1 = ReaderType::New();
  itk::PluginFilterWatcher watchReader1(reader1, "Read Volume 1",
                                        CLPProcessInformation);
  
  typename FilterType::InputImageRegionType region;
  SizeType  size = {iMaxValue-iMinValue, jMaxValue-jMinValue, kMaxValue-kMinValue};
  IndexType index = {iMinValue, jMinValue, kMinValue};
  //SizeType sizeL = {iMinValue, jMinValue, kMinValue};
  //SizeType sizeU = {iMaxValue, jMaxValue, kMaxValue};

  reader1->SetFileName( inputVolume1.c_str() );

  typename FilterType::Pointer filter = FilterType::New();
  /*
  filter->SetLowerBoundaryCropSize (sizeL);
  filter->SetUpperBoundaryCropSize (sizeU);
  filter->UpdateLargestPossibleRegion();
  */
  region.SetIndex(index);
  region.SetSize(size);
  filter->SetExtractionRegion(region);

  itk::PluginFilterWatcher watchFilter(filter,
                                       "Crop images",
                                       CLPProcessInformation);

  filter->SetInput( 0, reader1->GetOutput() );

  typename WriterType::Pointer writer = WriterType::New();
  itk::PluginFilterWatcher watchWriter(writer,
                                       "Write Volume",
                                       CLPProcessInformation);
  writer->SetFileName( outputVolume.c_str() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

  return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
  
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType (inputVolume1, pixelType, componentType);

    // This filter handles all types on input, but only produces
    // signed types
    
    switch (componentType)
      {
      case itk::ImageIOBase::UCHAR:
      case itk::ImageIOBase::CHAR:
        return DoIt( argc, argv, static_cast<char>(0));
        break;
      case itk::ImageIOBase::USHORT:
      case itk::ImageIOBase::SHORT:
        return DoIt( argc, argv, static_cast<short>(0));
        break;
      case itk::ImageIOBase::UINT:
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0));
        break;
      case itk::ImageIOBase::ULONG:
      case itk::ImageIOBase::LONG:
        return DoIt( argc, argv, static_cast<long>(0));
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt( argc, argv, static_cast<float>(0));
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt( argc, argv, static_cast<double>(0));
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }
  catch( itk::ExceptionObject &excep)
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
