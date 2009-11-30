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
#include "itkVector.h"
#include "itkMatrix.h"

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
  typedef  itk::Vector<double,3> VectorType;

  ImageType::PointType ReflectPoint(itk::Matrix<double,3,3>, ImageType::PointType pt);

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

       std::cout << "LPS: " << planePts[i] << std::endl;
//       std::cout << "IJK: " << index << std::endl;
      }
    }
  else
    {
    std::cerr << "No seeds specified." << std::endl;
    return -1;
    }
  
  // calculate normal vector c to the plane defined by the input points
  VectorType va, vb, planeNormal, planePt;

  va[0] = planePts[0][0]-planePts[1][0];
  va[1] = planePts[0][1]-planePts[1][1];
  va[2] = planePts[0][2]-planePts[1][2];

  vb[0] = planePts[0][0]-planePts[2][0];
  vb[1] = planePts[0][1]-planePts[2][1];
  vb[2] = planePts[0][2]-planePts[2][2];
  
  planeNormal[0] = va[1]*vb[2]-va[2]*vb[1];
  planeNormal[1] = va[2]*vb[0]-va[0]*vb[2];
  planeNormal[2] = va[0]*vb[1]-va[1]*vb[0];

  double n = planeNormal.GetNorm();
  planeNormal[0] /= n;
  planeNormal[1] /= n;
  planeNormal[2] /= n;


  planePt[0] = planePts[0][0];
  planePt[1] = planePts[0][1];
  planePt[2] = planePts[0][2];


  IterType itIn(input, input->GetLargestPossibleRegion());
  itIn.GoToBegin();
  for(;!itIn.IsAtEnd();++itIn){
    ImageType::PointType pt, ptReflected, ptProjected;
    ImageType::IndexType idx;

    if(!itIn.Get())
      continue;
    input->TransformIndexToPhysicalPoint(itIn.GetIndex(), pt);

    VectorType labelPt;
    labelPt[0] = pt[0];
    labelPt[1] = pt[1];
    labelPt[2] = pt[2];
    double distance = planeNormal*(labelPt-planePt);
    //ptReflected = labelPt-planeNormal*distance;
    ptProjected[0] = labelPt[0]-distance*planeNormal[0];
    ptProjected[1] = labelPt[1]-distance*planeNormal[1];
    ptProjected[2] = labelPt[2]-distance*planeNormal[2];
//    pt[0] = -labelPt[0]+2.*ptReflected[0];
//    ptReflected[1] = -labelPt[1]+2.*ptReflected[1];
//    ptReflected[2] = -labelPt[2]+2.*ptReflected[2];

    ptReflected[0] = ptProjected[0]-distance*planeNormal[0];
    ptReflected[1] = ptProjected[1]-distance*planeNormal[1];
    ptReflected[2] = ptProjected[2]-distance*planeNormal[2];

    if(!output->TransformPhysicalPointToIndex(ptReflected, idx)){
      std::cerr << "Point falls outside the image!" << std::endl;
      continue;
    }
    // this is my convention -- bump label id by 1
    output->SetPixel(idx, itIn.Get()+1);
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
  
ImageType::PointType ReflectPoint(itk::Matrix<double,3,3> proj, ImageType::PointType pt){
  ImageType::PointType out, v;
  out = proj*pt;

//  out[0] = -pt[0]+2.*out[0];
//  out[1] = -pt[1]+2.*out[1];
//  out[2] = -pt[2]+2.*out[2];

  return out;
}
