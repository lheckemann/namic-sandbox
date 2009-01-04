/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkKdTreeGeneratorTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005-07-26 15:55:12 $
  Version:   $Revision: 1.4 $

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
#include "itkVector.h"
#include "itkListSample.h"
#include "itkKdTree.h"
#include "itkKdTreeGenerator.h"
#include "itkEuclideanDistance.h"
#include "itkPointSetToListAdaptor.h"


int main(int argc, char *argv[])
{

  if( argc < 2)
    {
    std::cerr << "Usage: " << argv[0] << " vtkInputFilename " << std::endl;
    return EXIT_FAILURE;
    }


  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< double, Dimension > MeshType;
 
  typedef itk::VTKPolyDataReader<MeshType>  ReaderType;

  //
  // Read mesh file
  //

  std::cout << "Read " << argv[1] << std::endl;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  MeshType::Pointer mesh = reader->GetOutput();


  typedef MeshType::PointType  MeasurementVectorType ;

//  typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType ;

  typedef itk::Statistics::PointSetToListAdaptor< MeshType > SampleType;

  SampleType::Pointer sample = SampleType::New() ;

  sample->SetMeasurementVectorSize( Dimension );

  sample->SetPointSet( mesh );

  typedef itk::Statistics::KdTreeGenerator< SampleType > TreeGeneratorType ;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New() ;

  treeGenerator->SetSample( sample ) ;
  treeGenerator->SetBucketSize( 16 ) ;

  try
    {
    treeGenerator->Update() ;
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  typedef TreeGeneratorType::KdTreeType TreeType ;
  typedef TreeType::NearestNeighbors NeighborsType ;
  typedef TreeType::KdTreeNodeType NodeType ;

  TreeType::Pointer tree = treeGenerator->GetOutput() ;

  NodeType* root = tree->GetRoot() ;

  if ( root->IsTerminal() )
    {
    std::cout << "Root node is a terminal node." << std::endl ;
    }
  else
    {
    std::cout << "Root node is not a terminal node." << std::endl ;
    }

  unsigned int partitionDimension ;
  float partitionValue ;
  root->GetParameters( partitionDimension, partitionValue) ;
  std::cout << "Dimension chosen to split the space = " 
            << partitionDimension << std::endl ;
  std::cout << "Split point on the partition dimension = "
            << partitionValue << std::endl ;

  std::cout << "Address of the left child of the root node = "
            << root->Left() << std::endl ;
  
  std::cout << "Address of the right child of the root node = "
            << root->Right() << std::endl ;

  MeasurementVectorType queryPoint ;
  queryPoint[0] = 10.0 ;
  queryPoint[1] = 7.0 ;

  typedef itk::Statistics::EuclideanDistance< MeasurementVectorType > DistanceMetricType ;
  DistanceMetricType::Pointer distanceMetric = DistanceMetricType::New() ;
  DistanceMetricType::OriginType origin( Dimension ) ;
  for ( unsigned int i = 0 ; i < MeasurementVectorType::Length ; ++i )
    {
    origin[i] = queryPoint[i] ;
    }
  distanceMetric->SetOrigin( origin ) ;

  unsigned int numberOfNeighbors = 3 ;
  TreeType::InstanceIdentifierVectorType neighbors ;
  tree->Search( queryPoint, numberOfNeighbors, neighbors ) ; 
  
  std::cout << "kd-tree knn search result:" << std::endl 
            << "query point = [" << queryPoint << "]" << std::endl
            << "k = " << numberOfNeighbors << std::endl ;
  std::cout << "measurement vector : distance" << std::endl ;
  for ( unsigned int i = 0 ; i < numberOfNeighbors ; ++i )
    {
    std::cout << "[" << tree->GetMeasurementVector( neighbors[i] )
              << "] : "  
              << distanceMetric->Evaluate( tree->GetMeasurementVector( neighbors[i])) << std::endl ;
    }

  double radius = 437.0 ;

  tree->Search( queryPoint, radius, neighbors ) ; 
  
  std::cout << "kd-tree radius search result:" << std::endl
            << "query point = [" << queryPoint << "]" << std::endl
            << "search radius = " << radius << std::endl ;
  std::cout << "measurement vector : distance" << std::endl ;
  for ( unsigned int i = 0 ; i < neighbors.size() ; ++i )
    {
    std::cout << "[" << tree->GetMeasurementVector( neighbors[i] )
              << "] : "  
              << distanceMetric->Evaluate( tree->GetMeasurementVector( neighbors[i])) << std::endl ;
    }    
  
  std::cout << "Test passed." << std::endl;

  return EXIT_SUCCESS;
}
