/*=========================================================================

  Program:   Slicer3
  Language:  C++

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif



#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageDuplicator.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "ThreePointMirrorCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace {

} // end of anonymous namespace


  typedef   char           PixelType;
  typedef itk::OrientedImage< PixelType, 3 >  ImageType;
  typedef itk::ImageDuplicator< ImageType > DuplicatorType;

  typedef  itk::ImageFileReader< ImageType > ReaderType;
  typedef  itk::ImageFileWriter< ImageType > WriterType;
  typedef  itk::ImageRegionIteratorWithIndex<ImageType> IterType;

  ImageType::PointType ReflectPoint(ImageType::PointType n, double d, ImageType::PointType pt);

int main( int argc, char *argv[] )
{
  PARSE_ARGS;


  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName( inputVolume.c_str() );
  reader->Update();

  ImageType::Pointer input = reader->GetOutput();
  DuplicatorType::Pointer dup = DuplicatorType::New();
  dup->SetInputImage(input);
  dup->Update();
  ImageType::Pointer output = dup->GetOutput();
  output->FillBuffer(0);
  
  writer->SetFileName( outputVolume.c_str() );

  ImageType::PointType planePts[3];
  if (seed.size() > 0)
    {
    for (::size_t i=0; i<3; ++i)
      {
      // seeds come in ras, convert to lps
      planePts[i][0] = -seed[i][0];  
      planePts[i][1] = -seed[i][1];
      planePts[i][2] =  seed[i][2];

//       std::cout << "LPS: " << lpsPoint << std::endl;
//       std::cout << "IJK: " << index << std::endl;
      }
    }
  else
    {
    std::cerr << "No seeds specified." << std::endl;
    return -1;
    }
  
  // calculate normal vector c to the plane defined by the input points
  ImageType::PointType va, vb, vc;

  va[0] = planePts[0][0]-planePts[1][0];
  va[1] = planePts[0][1]-planePts[1][1];
  va[2] = planePts[0][2]-planePts[1][2];

  vb[0] = planePts[0][0]-planePts[2][0];
  vb[1] = planePts[0][1]-planePts[2][1];
  vb[2] = planePts[0][2]-planePts[2][2];
  
  vc[0] = va[1]*vb[2]-va[2]*vb[1];
  vc[1] = va[2]*vb[0]-va[0]*vb[2];
  vc[2] = va[0]*vb[1]-va[1]*vb[0];

  // calculate the plane equation
  double d = -vc[0]*planePts[0][0]-vc[1]*planePts[0][1]-vc[2]*planePts[0][2];

  IterType itIn(input, input->GetLargestPossibleRegion());

  itIn.GoToBegin();
  for(;!itIn.IsAtEnd();++itIn){
    ImageType::PointType pt, ptReflected;
    ImageType::IndexType idx;

    if(!itIn.Get())
      continue;
    input->TransformIndexToPhysicalPoint(itIn.GetIndex(), pt);
    ptReflected = ReflectPoint(vc, d, pt);

    output->TransformPhysicalPointToIndex(ptReflected, idx);
    output->SetPixel(idx, itIn.Get());
  }

  writer->SetInput(output);

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }

  return 0;
}
  
// from http://mathworld.wolfram.com/Reflection.html
ImageType::PointType ReflectPoint(ImageType::PointType n, double d, ImageType::PointType pt){
  double D;
  ImageType::PointType outPt;
  D = 2.*(n[0]*pt[0]+n[1]*pt[1]+n[2]*pt[2]+d)/(pt[0]*pt[0]+pt[1]*pt[1]+pt[2]*pt[2]);
  outPt[0] = pt[0]-D*n[0];
  outPt[1] = pt[1]-D*n[1];
  outPt[2] = pt[2]-D*n[2];

  return outPt;
}
