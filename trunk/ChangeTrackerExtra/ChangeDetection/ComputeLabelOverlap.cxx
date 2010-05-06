/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkShapeLabelObjectAccessorsTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2009-08-07 15:16:20 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageDuplicator.h"

int main(int argc, char * argv[])
{
  if( argc != 4 )
    {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " label1 label2 overlap_label ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int dim = 3;
 
  typedef unsigned char                           PixelType;
  typedef itk::OrientedImage< PixelType, dim >            ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ConstIterType;
  typedef itk::ImageRegionIterator<ImageType> IterType;
  typedef itk::ImageDuplicator<ImageType> DupType;

  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName(argv[1]);
  reader1->Update();


  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName(argv[2]);
  reader2->Update();

  ImageType::Pointer image1 = reader1->GetOutput();
  ImageType::Pointer image2 = reader2->GetOutput();
  ImageType::Pointer output;

  DupType::Pointer dup = DupType::New();
  dup->SetInputImage(image1);
  dup->Update();

  output = dup->GetOutput();
  output->FillBuffer(0);

  ConstIterType it1(image1, image1->GetBufferedRegion());
  IterType it2(output, output->GetBufferedRegion());

  it1.GoToBegin(); 

  for(;!it1.IsAtEnd();++it1,++it2){
    ImageType::IndexType idx;
    ImageType::PointType pt;
    it2.Set(0);
    if(!it1.Get())
      continue;
    idx = it1.GetIndex();
    image1->TransformIndexToPhysicalPoint(idx, pt);
    image2->TransformPhysicalPointToIndex(pt, idx);
    if(image2->GetPixel(idx))
      it2.Set(1);
  }

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(argv[3]);
  writer->SetInput(output);
  writer->SetUseCompression(1);
  writer->Update();

  return 0;
}
