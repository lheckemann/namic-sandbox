/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMeanSquaresMeshToMeshMetricTest1.cxx,v $
  Language:  C++
  Date:      $Date: 2007-09-06 17:44:24 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <math.h>
#include "itkVector.h"
#include "itkQuadEdgeMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkMeshGeneratorHelper.h"
#include <iostream>

static float mapSphericalCoordinatesFunction(float inPhi); 
static itk::Vector<float,3> mapSphericalCoordinatesFunctionGradient(float inPhi, float inTheta, bool printFlag); 

//Really simple example: a sinusoid mapping between 0 and 1 as a function of theta, constant in phi
static float 
mapSphericalCoordinatesFunction(float inTheta) 
{
  float result; 

  float thetaFactor= sin(inTheta); // simplest non-constant function of theta
                                   // that is smooth at 0-2pi boundary

  result= thetaFactor; 

  return result; 
}

static itk::Vector<float,3>
mapSphericalCoordinatesFunctionGradient(float inPhi, float inTheta, bool printFlag) 
{
  itk::Vector<float,3> phiComponent, thetaComponent, result; 
  
  float cosTheta= cos(inTheta); 
  float sinTheta= sin(inTheta); 
  float cosPhi= cos(inPhi); 
  float sinPhi= sin(inPhi);

  //derivative of phiFactor over Theta 
  float functionDerivativeOverTheta= cos(inTheta);

  //Need to multiply dF/dtheta by unit vector 
  //unit vector= cos(phi)*cos(theta) i + cos(phi)*sin(theta) j - sin(phi)k

  thetaComponent[0]= -sinPhi * sinTheta * functionDerivativeOverTheta; 
  thetaComponent[1]= sinPhi * cosTheta * functionDerivativeOverTheta; 
  thetaComponent[2]= 0.0;

  for (unsigned int i=0; i<3; i++)
    {
    result[i]= thetaComponent[i]; 
    }

  if (printFlag)
    {
    std::cout << "  inTheta " << inTheta << "  inPhi " << inPhi
              << "  sinTheta " << sinTheta << "  cosTheta " << cosTheta  
              << "  sinPhi " << sinPhi << "  cosPhi " << cosPhi  
              << "  dfdphi " << functionDerivativeOverTheta
              << "  thetaComponent " << thetaComponent
              << "  phiComponent " << phiComponent << "  result " << result << " \n";
    }
     
  return result; 
}
                                    

int main( int , char * [] )
{
  const unsigned int Dimension = 3;

  typedef itk::QuadEdgeMesh< float, Dimension >   MovingMeshType;
  typedef itk::QuadEdgeMesh< float, Dimension >   FixedMeshType;

  FixedMeshType::Pointer   fixedMesh;
  MovingMeshType::Pointer  movingMesh;

  typedef MeshGeneratorHelper< FixedMeshType, MovingMeshType >  GeneratorType;

  GeneratorType::GenerateMeshes( fixedMesh, movingMesh );


  typedef itk::QuadEdgeMesh<float, 3>   MeshType;
  typedef itk::RegularSphereMeshSource< MeshType >  SphereMeshSourceType;

  SphereMeshSourceType::Pointer  mySphereMeshSource = SphereMeshSourceType::New();

  typedef SphereMeshSourceType::PointType   PointType;
  typedef SphereMeshSourceType::VectorType  VectorType;

  // Set up synthetic data.  

  PointType myCenter; 
  myCenter.Fill( 0.0 );

  VectorType myScale;
  myScale.Fill( 1.0 );
  
  mySphereMeshSource->SetCenter( myCenter );
  mySphereMeshSource->SetResolution( 4.0 );
  mySphereMeshSource->SetScale( myScale );
  mySphereMeshSource->Modified();
  

  try
    {
    mySphereMeshSource->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during source Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "mySphereMeshSource: " << mySphereMeshSource;
  
  MeshType::Pointer myMesh = mySphereMeshSource->GetOutput();

  PointType  myPt;
  float radius, phi, theta; //spherical coordinates
  float myValue; 
  
  std::cout << "Testing itk::RegularSphereMeshSource "<< std::endl;

   for( unsigned int i=0; i < myMesh->GetNumberOfPoints(); i++ )
    {
      
    myMesh->GetPoint(i, &myPt);

    for ( unsigned int j=0; j<3; j++ ) 
      {
        myPt[j]-= myCenter[j];   //coordinates relative to center, if center is not origin
      }

    radius= sqrt(myPt[0]*myPt[0] + myPt[1]*myPt[1] + myPt[2]*myPt[2]); //assuming radius is not valued 1
    theta= atan2(myPt[1], myPt[0]); 
    phi= acos(myPt[2]/radius); 

    myValue= mapSphericalCoordinatesFunction(theta); 

    myMesh->SetPointData(i, myValue);

    std::cout << "Point[" << i << "]: " << myPt << " radius "
              << radius << "  theta " << theta << "  phi "
              << phi  << "  myValue " << myValue << std::endl; 
    }   

  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType >   WriterType;
  WriterType::Pointer myWriter = WriterType::New();
  myWriter->SetInput( myMesh );
  myWriter->SetFileName( "Mesh.vtk" );

  try
    {
    myWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error during myWriter Update() " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


//------------------------------------------------------------
// Set up an Interpolator
//------------------------------------------------------------
  typedef itk::LinearInterpolateMeshFunction< 
                    MeshType> InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  interpolator->SetInputMesh( myMesh );

  interpolator->Initialize();


//------------------------------------------------------------
// Simple Test of derivative computed by Interpolator
//------------------------------------------------------------

// For all cells of mesh, find the middle point, and compare analytical
// value of derivative with value returned by triangle-based estimation. 

  typedef MeshType::CellsContainerPointer  CellsContainerPointer;
  typedef MeshType::CellsContainerIterator CellsContainerIterator;
  typedef MeshType::CellType               CellType;
  typedef CellType::PointType              CellPointType;
  typedef CellType::PointIdIterator        PointIdIterator; 

  CellsContainerPointer cells = myMesh->GetCells();

  unsigned faceId = 0;

  float maximumDifferenceMagnitude= 0.0;

  for( MeshType::CellsContainerIterator cells_it = cells->Begin();
       cells_it != cells->End();
       ++cells_it, faceId++ )
    {
    CellType* cellPointer = cells_it.Value();
    
    if( cellPointer->GetType() != 1 )
      {
      std::cout <<"Face " << faceId << " has " << cellPointer->GetNumberOfPoints() 
                <<" points" << std::endl;
      }
    
    PointType myCellCenter;
    float cellCenterTheta=0.0;
    float cellCenterPhi=0.0; 
    float cellCenterRadius=0.0; 


    PointIdIterator pointIdIterator = cellPointer->PointIdsBegin();
    PointIdIterator pointIdEnd = cellPointer->PointIdsEnd();
    for( unsigned int i = 0; i < 3; i++ )
      {
      myCellCenter[i]= 0.0;
      }

    
    while( pointIdIterator != pointIdEnd )
      {
      CellPointType cellPoint= myMesh->GetPoint(*pointIdIterator);
      for( unsigned int i = 0; i < 3; i++ )
         {
         myCellCenter[i]+= cellPoint[i];          
         }
         pointIdIterator++;
      }
    for( unsigned int i = 0; i < 3; i++ )
      {
      myCellCenter[i]/= cellPointer->GetNumberOfPoints();
      }
    
    cellCenterRadius= sqrt(myCellCenter[0]*myCellCenter[0] +
                           myCellCenter[1]*myCellCenter[1] +
                           myCellCenter[2]*myCellCenter[2]); //assuming radius is not valued 1
    cellCenterTheta= atan2(myCellCenter[1], myCellCenter[0]); 
    cellCenterPhi= acos(myCellCenter[2]/cellCenterRadius); 

    if (faceId==1365) {
       std::cout << " problem case \n";       
       std::cout << " stop here \n";
    }
    
    InterpolatorType::DerivativeType computedDerivative; 
    interpolator->EvaluateDerivative(myCellCenter, computedDerivative);

    itk::Vector<float,3> analyticalDerivative= mapSphericalCoordinatesFunctionGradient(cellCenterPhi, cellCenterTheta, false); 
        
    std::cout << " faceId  " << faceId << "  cell Center " << myCellCenter
              << "  analytical derivative " 
              << analyticalDerivative
              << "  computed derivative "
              << computedDerivative
              << std::endl;

    itk::Vector<float,3> differenceValue;
    float differenceValueMagnitude= 0.0; 

    for ( unsigned int i=0; i<3; i++ )
      {
      differenceValue[i]= analyticalDerivative[i] - computedDerivative[i];
      differenceValueMagnitude+= (differenceValue[i]*differenceValue[i]); 
      }

    differenceValueMagnitude= sqrt(differenceValueMagnitude);

    if ( differenceValueMagnitude >  maximumDifferenceMagnitude )
      {
      maximumDifferenceMagnitude = differenceValueMagnitude;
      
      std::cout << " maximum difference magnitude increasing " << maximumDifferenceMagnitude << " faceId " << faceId << std::endl;
      }

    }

  std::cout << " maximum difference magnitude " << maximumDifferenceMagnitude << std::endl;
  
  std::cout << "Test End "<< std::endl;


  return EXIT_SUCCESS;

}

