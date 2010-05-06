/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkQuadEdgeMeshSplitFilterTest2.cxx,v $
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
#include "itkVTKPolyDataReader.h"
#include "itkVTKPolyDataWriter.h"

#include "itkQuadEdgeMeshSplitFilter.h"


int main( int argc, char** argv )
{
  if( argc != 4 )
    {
    std::cout <<"Error: requires 3 arguments" <<std::endl;
    std::cout <<"Input-Surface" <<std::endl;
    std::cout <<"Hemisphere-1-Surface Hemisphere-2-Surface" <<std::endl;
    return EXIT_FAILURE;
    }
    // ** TYPEDEF **
  typedef double Coord;

  typedef itk::QuadEdgeMesh< Coord, 3 >               MeshType;
  typedef MeshType::Pointer                           MeshPointer;
  typedef itk::VTKPolyDataReader< MeshType >          ReaderType;
  typedef itk::VTKPolyDataWriter< MeshType >          WriterType;
  
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

  //calculate seedFaces

  //choose the direction along which the seeds are picked
  typedef itk::FixedArray< float, 6> FixedArraySize;

  FixedArraySize bounds;

  bounds = mesh->GetBoundingBox()->GetBounds();

  FixedArraySize size;
  size[0] = bounds[1] - bounds[0];
  size[1] = bounds[3] - bounds[2];
  size[2] = bounds[5] - bounds[4];

  unsigned long axis = 0;
  float maxSize = 1000000.0;
  for (unsigned short i = 0; i<3; i++)
  {
    if (size[i] < maxSize)
    {
        axis = i; 
        maxSize = size[i];
    }
  }

  //walk through all of the points 
  //get the one with max and min in axis direction.
  float minAxis = 1000000.0;
  float maxAxis = -1000000.0;
 
  MeshType::PointsContainerConstPointer points = mesh->GetPoints();
  typedef MeshType::PointsContainerConstIterator PointsContainerConstIterator;
  PointsContainerConstIterator p_It = points->begin();
  typedef MeshType::QEPrimal QEPrimal;
  typedef MeshType::CellIdentifier CellIdentifier;

  CellIdentifier backCell,frontCell;

  while (p_It != points->end())
  {
      MeshType::PointType p = p_It.Value();

      if (p[axis] < minAxis)
      {
         if ( p.GetEdge() != (QEPrimal*)0 )
         {
             QEPrimal* e = p.GetEdge();
             if (e->GetLeft() != mesh->m_NoFace)
             {
               backCell = e->GetLeft();
               minAxis = p[axis];
             }
          }
      }

      if (p[axis] > maxAxis)
      {
         if (p.GetEdge() != (QEPrimal*)0)
         {
             QEPrimal* e = p.GetEdge();
             if (e->GetLeft() != mesh->m_NoFace)
             {
               frontCell = e->GetLeft();
               maxAxis = p[axis];
             }
          }
       }

       p_It++;
  }

  std::vector <CellIdentifier> seedList;
  seedList.push_back(backCell);
  seedList.push_back(frontCell);

  typedef itk::QuadEdgeMeshSplitFilter< MeshType, MeshType > SplitFilterType;
  SplitFilterType::Pointer split = SplitFilterType::New();
  split->SetInput( mesh );
  split->SetSeedFaces(seedList);
  split->Update();
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( split->GetOutput( 0 ) );
  writer->SetFileName( argv[2] );
  writer->Update();
  
  writer->SetInput( split->GetOutput( 1 ) );
  writer->SetFileName( argv[3] );
  writer->Update();

  return EXIT_SUCCESS;
}
