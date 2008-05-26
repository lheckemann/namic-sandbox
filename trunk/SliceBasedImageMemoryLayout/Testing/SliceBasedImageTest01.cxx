/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: Image3.cxx,v $
  Language:  C++
  Date:      $Date: 2005/02/08 03:51:52 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkSliceBasedImage.h"
#include "itkImageRegionIterator.h"


int main(int argc, char * argv [])
{

  const unsigned int Dimension = 3;

  typedef itk::Index< Dimension >       PixelType;

  typedef itk::SliceBaseImage< PixelType, Dimension > ImageType;

  typedef itk::ImageRegionIterator< ImageType >  IteratorType;


  ImageType::Pointer image = ImageType::New();


  ImageType::IndexType start;
  ImageType::SizeType  size;

  size[0]  = 200;  // size along X
  size[1]  = 200;  // size along Y
  size[2]  = 200;  // size along Z

  start[0] =   0;  // first index on X
  start[1] =   0;  // first index on Y
  start[2] =   0;  // first index on Z

  ImageType::RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
  
  
  image->SetRegions( region );
  image->Allocate();

  IteratorType itr( image, region );

  itr.GoToBegin();

  while( !itr.IsAtEnd() )
    {
    itr.Set( itr.GetIndex() );
    ++itr;
    }
 
  return 0;

}

