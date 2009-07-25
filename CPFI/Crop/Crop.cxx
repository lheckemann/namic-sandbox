/*=========================================================================

  Program:   Commandline Module for Image Cropping
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

// NOTE: We use ExtractImageFilter instead of CropImageFilter,
// since CropImageFilter does not allow to spcify 0 for any element
// in the size parameter for upper and lower boundary cropping.
#include "itkExtractImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkMetaDataObject.h"

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

  //typedef itk::CropImageFilter<
  typedef itk::ExtractImageFilter<
       InputImageType, OutputImageType >  FilterType;
  typename ReaderType::Pointer reader = ReaderType::New();
  itk::PluginFilterWatcher watchReader(reader, "Read Volume 1",
                                        CLPProcessInformation);
  reader->SetFileName( inputVolume1.c_str() );
  reader->Update();

  typename FilterType::Pointer filter = FilterType::New();
  itk::PluginFilterWatcher watchFilter(filter,
                                       "Crop images",
                                       CLPProcessInformation);

  //typename InputImageType::SizeType size = reader->GetOutput()->
  //  GetLargestPossibleRegion().GetSize();
  SizeType  size;
  size[0] = iMaxValue-iMinValue;
  size[1] = jMaxValue-jMinValue;
  size[2] = kMaxValue-kMinValue;

  IndexType index;
  index[0] = iMinValue;
  index[1] = jMinValue;
  index[2] = kMinValue;

  filter->SetInput( 0, reader->GetOutput() );

  typename FilterType::InputImageRegionType region;
  region.SetIndex(index);
  region.SetSize(size);

  filter->SetExtractionRegion(region);
  filter->Update();

  typename OutputImageType::Pointer       outputImage = OutputImageType::New();
  typename OutputImageType::SpacingType   spacing     = reader->GetOutput()->GetSpacing();
  typename OutputImageType::PointType     inputOrigin = reader->GetOutput()->GetOrigin();
  typename OutputImageType::DirectionType dir         = reader->GetOutput()->GetDirection();

  // shift the image
  double outputOrigin[3];
  double si = (double)index[0] * spacing[0];
  double sj = (double)index[1] * spacing[1];
  double sk = (double)index[2] * spacing[2];

  // apply the orientation matrix
  outputOrigin[0] = inputOrigin[0] + si*dir[0][0] + sj*dir[0][1] + sk*dir[0][2];
  outputOrigin[1] = inputOrigin[1] + si*dir[1][0] + sj*dir[1][1] + sk*dir[1][2];
  outputOrigin[2] = inputOrigin[2] + si*dir[2][0] + sj*dir[2][1] + sk*dir[2][2];

  outputImage = filter->GetOutput();
  outputImage->SetSpacing(spacing);
  outputImage->SetOrigin(outputOrigin);
  outputImage->Update();

  typename WriterType::Pointer writer = WriterType::New();
  itk::PluginFilterWatcher watchWriter(writer,
                                       "Write Volume",
                                       CLPProcessInformation);
  writer->SetFileName( outputVolume.c_str() );
  writer->SetInput( outputImage );
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
