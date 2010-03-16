/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryMask3DQuadEdgeMeshSourceTest.cxx,v $
  Language:  C++
  Date:      $Date: 2009-04-06 11:27:19 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkOrientedImage.h"
#include "itkImageFileReader.h"
#include "itkQuadEdgeMesh.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkQuadEdgeMeshQuadricDecimation.h"
#include "itkQuadEdgeMeshSquaredEdgeLengthDecimation.h"
#include "itkQuadEdgeMeshNormalFilter.h"

int main(int argc, char **argv){

  // Define the dimension of the images
  const unsigned int Dimension = 3;

  // Declare the types of the output images
  typedef itk::OrientedImage<unsigned short,   Dimension>   ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThreshType;

  // Declare the type of the index,size and region to initialize images
  typedef itk::Index<Dimension>                     IndexType;
  typedef itk::Size<Dimension>                      SizeType;
  typedef itk::ImageRegion<Dimension>               RegionType;
  typedef ImageType::PixelType                      PixelType;

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  // Declare the type of the Mesh
  typedef itk::Vector<float,3> MeshDataType;
  typedef itk::QuadEdgeMesh<MeshDataType, 3>              MeshType;
  typedef MeshType::PointType                       PointType;
  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType >  MeshWriterType;
//  typedef itk::VTKPolyDataWriter<MeshType> MeshWriterType;

  typedef itk::QuadEdgeMeshNormalFilter< MeshType, MeshType > NormalFilterType;
  typedef itk::BinaryMask3DMeshSource< ImageType, MeshType >   MeshSourceType;

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " binary_image output_mesh" << std::endl;
    return -1;
  }

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  
  ThreshType::Pointer thresh = ThreshType::New();
  thresh->SetInput(reader->GetOutput());
  thresh->SetLowerThreshold(1);
  thresh->SetUpperThreshold(255);
  thresh->SetInsideValue(255);  

  MeshSourceType::Pointer meshSource = MeshSourceType::New();

  meshSource->SetInput( thresh->GetOutput() );
  meshSource->SetObjectValue( 255 );


  try
    {
    meshSource->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown during Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << meshSource->GetNumberOfNodes() << std::endl;
  std::cout << meshSource->GetNumberOfCells() << std::endl;


  // decimate the mesh
  typedef itk::NumberOfFacesCriterion< MeshType > CriterionType;
  typedef itk::QuadEdgeMeshSquaredEdgeLengthDecimation< 
    MeshType, MeshType, CriterionType > DecimationType;

  CriterionType::Pointer criterion = CriterionType::New();
  criterion->SetTopologicalChange( false );
  criterion->SetNumberOfElements( meshSource->GetNumberOfCells()/atoi(argv[3]) );

  DecimationType::Pointer decimate = DecimationType::New();
  decimate->SetInput( meshSource->GetOutput() );
  decimate->SetCriterion( criterion );
  decimate->Update();
  
  std::cout << "Decimation complete" << std::endl;

  NormalFilterType::Pointer normals = NormalFilterType::New( );
  normals->SetInput( decimate->GetOutput() );
  normals->Update( );
  std::cout << "Normals calculated" << std::endl;


  /*
  MeshWriterType::Pointer writer = MeshWriterType::New();
  writer->SetInput( normals->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();
  */

  return EXIT_SUCCESS;

}
