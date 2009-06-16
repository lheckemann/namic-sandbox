/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2009-02-05 19:04:56 $
  Version:   $Revision: 1.150 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"

#include "Example2.h"

int main( int argc, char * argv [] )
{
  if( argc < 2 )
    {
    std::cerr << "Missing Arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFileName " << std::endl;
    return EXIT_FAILURE;
    }

  typedef unsigned char PixelType;

  const unsigned int Dimension = 2;
 
  typedef itk::Image< PixelType, Dimension >   ImageType;

  typedef itk::ImageFileReader< ImageType >    ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName( argv[1] );

  reader->Update();

  const ImageType * image = reader->GetOutput();

  const PixelType * pixelBuffer = image->GetBufferPointer();

  byte * imageSource = const_cast< PixelType * >( pixelBuffer );
  byte * imageDestination;
  int  stride;
  ROI size;

  ImageType::RegionType region = image->GetBufferedRegion();
  ImageType::SizeType imageSize = region.GetSize();

  size.width  = imageSize[0];
  size.height = imageSize[1];

  CopyITKImageToCUDA( imageSource, imageDestination, stride, size );
  
  std::cout << "Stride = " << stride << std::endl;
  return EXIT_SUCCESS;
}
