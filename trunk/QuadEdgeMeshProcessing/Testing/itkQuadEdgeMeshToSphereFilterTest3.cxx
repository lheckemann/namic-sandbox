/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshToSphereFilterTest3.cxx,v $
  Language:  C++
  Date:      $Date: 2008-03-10 19:46:31 $
  Version:   $Revision: 1.37 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkQuadEdgeMesh.h"
#include "itkTransformMeshFilter.h"
#include "itkVersorTransform.h"

#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "VNLIterativeSparseSolverTraits.h"
#include "itkQuadEdgeMeshParamMatrixCoefficients.h"

#include "itkQuadEdgeMeshToSphereFilter.h"
#include "itkVector.h"

int main( int argc, char** argv )
{
  if( argc != 5 )
    {
    std::cout <<"It requires 3 arguments" <<std::endl;
    std::cout <<"1-Input FileName" <<std::endl;
    std::cout <<"1-Output FileName" <<std::endl;
    std::cout <<"axis radius" <<std::endl;
    return EXIT_FAILURE;
    }
    // ** TYPEDEF **
  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >                 MeshType;
  typedef MeshType::Pointer                             MeshPointer;
  typedef MeshType::CellIdentifier                      CellIdentifier;

  typedef itk::VTKPolyDataReader< MeshType >            ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >            WriterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update( );
    }
  catch( itk::ExceptionObject & exp )
    {
    std::cerr << "Exception thrown while reading the input file " << std::endl;
    std::cerr << exp << std::endl;
    return EXIT_FAILURE;
    }

  MeshPointer mesh = reader->GetOutput();

  //choose the seeds along the shortest axis
  unsigned short ax = atoi(argv[3]);

  //walk through all of the points 
  //get the one with max and min in axis direction.
  float minAxis = 1000000.0;
  float maxAxis = -1000000.0;

  //float maxZ = 0.0;
  //float pi = 3.14159;
  MeshType::PointType sphereCenter;
  sphereCenter.Fill(0.0);
 
  MeshType::PointsContainerConstPointer points = mesh->GetPoints();
  typedef MeshType::PointsContainerConstIterator PointsContainerConstIterator;
  PointsContainerConstIterator p_It = points->begin();
  typedef MeshType::QEPrimal QEPrimal;

  typedef MeshType::CellIdentifier CellIdentifier;
  CellIdentifier backCell,frontCell;

  typedef MeshType::PointIdentifier PointIdentifier;
  //PointIdentifier maxZId,maxAxisId = 0;

  while (p_It != points->end())
  {
    MeshType::PointType p = p_It.Value();

    //look for seedFace at minAxis
    if (p[ax] < minAxis)
    {
      if ( p.GetEdge() != (QEPrimal*)0 )
      {
             QEPrimal* e = p.GetEdge();
             if (e->GetLeft() != mesh->m_NoFace)
             {
               backCell = e->GetLeft();
               minAxis = p[ax];
             }
       }
    }

    //look for seedFace at maxAxis
    //also remember the poindId at maxAxis
    if (p[ax] > maxAxis)
      {
         if (p.GetEdge() != (QEPrimal*)0)
         {
             QEPrimal* e = p.GetEdge();
             if (e->GetLeft() != mesh->m_NoFace)
             {
               frontCell = e->GetLeft();
               maxAxis = p[ax];
             }
         }
       }

     p_It++;
  }

  //std::cout<<"north polar point Id is: "<<maxZId<<std::endl;

  std::vector <CellIdentifier> seedList;
  seedList.push_back(backCell);
  seedList.push_back(frontCell);

  itk::OnesMatrixCoefficients< MeshType >    coeff0;

  typedef VNLIterativeSparseSolverTraits< Coord >  SolverTraits;

  typedef itk::QuadEdgeMeshToSphereFilter<
    MeshType, MeshType, SolverTraits > FilterType;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( mesh );
  filter->SetCoefficientsMethod( &coeff0 );
  filter->SetRadius(atof(argv[4]));
  filter->SetSeedFaces(seedList);
  filter->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  return EXIT_SUCCESS;
}
