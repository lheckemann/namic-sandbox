/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMarchingCubesImageToMeshFilterTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2008-11-21 20:00:48 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageFileReader.h" 
#include "itkMarchingCubesImageToMeshFilter.h"
#include "itkImage.h"
#include "itkPointSet.h"
#include "itkQuadEdgeMesh.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkQuadEdgeMeshVTKPolyDataReader.h"


int main( int argc, char * argv[] )
{
  
  if( argc < 4 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage outputFile.vtk  isoSurfaceValue ";
    return EXIT_FAILURE;
    }
  
  typedef   signed short  ImagePixelType;
  const     unsigned int   ImageDimension = 3;
  const     unsigned int   TopologicalDimension = 1;

  typedef   itk::Image< ImagePixelType, ImageDimension >   ImageType;

  typedef   itk::CovariantVector< float, ImageDimension >  NormalType;

  typedef   float          PointCoordinateRepresentation;

  //
  // It is important for the Mesh to be of "Dynamic" traits type.
  // That makes possible to insert points in an efficient manner.
  //
  typedef   itk::DefaultDynamicMeshTraits< 
    NormalType, ImageDimension, 
    TopologicalDimension, PointCoordinateRepresentation >  PointSetTraits;

  typedef   itk::QuadEdgeMesh< NormalType, ImageDimension, PointSetTraits >    PointSetType;

  typedef   itk::MarchingCubesImageToMeshFilter< ImageType, PointSetType > FilterType;

  typedef   itk::ImageFileReader< ImageType  >        ReaderType;

  
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown during reading" << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  FilterType::Pointer marchingCubesFilter = FilterType::New();

  marchingCubesFilter->SetInput( reader->GetOutput() );

  marchingCubesFilter->SetSurfaceValue( atof( argv[3] ) );

  try
    {
    marchingCubesFilter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  PointSetType::ConstPointer pointSet = marchingCubesFilter->GetOutput();

  //
  // Exercise the Print() method.
  //
  marchingCubesFilter->Print( std::cout );

  std::cout << pointSet->GetNumberOfPoints() << std::endl;

  typedef itk::QuadEdgeMeshVectorDataVTKPolyDataWriter< PointSetType >   WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( pointSet );
  writer->SetFileName( argv[2] );
  writer->Write();

  return EXIT_SUCCESS;
}

