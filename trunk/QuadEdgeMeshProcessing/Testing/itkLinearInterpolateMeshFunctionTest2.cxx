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
#include "itkVersorRigid3DTransform.h"
#include "itkQuadEdgeMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkMeanSquaresMeshToMeshMetric.h"
#include "itkMeshToMeshRegistrationMethod.h"
#include "itkLinearInterpolateMeshFunction.h"
#include "itkAmoebaOptimizer.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
//#include "itkCommandIterationUpdate.h"

#include <iostream>

const float TWO_PI= M_PI * 2.0; 

static float sineMapSphericalCoordinatesFunction(float inPhi, float inTheta); 
static itk::Vector<float,3> sineMapSphericalCoordinatesFunctionGradient(float inPhi, float inTheta); 

//Combine a sinusoid mapping of theta with a linear mapping between 0 and 1 as a function of phi
static float 
sineMapSphericalCoordinatesFunction(float inPhi, float inTheta) 
{
  float result; 

  float phiFactor= (M_PI - inPhi)/M_PI; //inPhi should be in [0,PI]; peak at North Pole: phi=0.
  float thetaFactor= (sin(inTheta)+1.0)/2.0; //inTheta should be in [-PI,PI]; 

  result= phiFactor * thetaFactor; 

  return result; 
}

static itk::Vector<float,3>
sineMapSphericalCoordinatesFunctionGradient(float inPhi, float inTheta) 
{
   itk::Vector<float,3> result; 
   float derivativeThetaOverX;
   float derivativeThetaOverY;
   //const float derivativeThetaOverZ= 0.0;
   //const float derivativePhiOverX= 0.0;
   //const float derivativePhiOverY= 0.0;
   float derivativePhiOverZ;
   float X,Y,Z, YOverX, YOverXSquared;
   const float SmallThreshold= 1.0e-8; 
   const float OneLessSmallThreshold= 1.0-SmallThreshold;
   
  //float phiFactor= (M_PI - inPhi)/M_PI; //inPhi should be in [0,PI]; peak at North Pole: phi=0.
  //float thetaFactor= (sin(inTheta)+1.0)/2.0; //inTheta should be in [-PI,PI]; 

  //derivative of phiFactor over Phi * thetaFactor
  float functionDerivativeOverPhi= -(sin(inTheta)+1.0)/TWO_PI;
  //derivative of thetaFactor over theta * phiFactor
  float functionDerivativeOverTheta= ((M_PI - inPhi)*cos(inTheta))/TWO_PI;
  
  //Chain rule: deriv F/deriv x = d F/d theta * d theta/d x +
  //                            + d F/d phi * d phi/d x
  //            and so on for y and z...

  // theta= arctan(y/x), therefore d theta/d x = (-y/x^2)/[1+(y/x)^2]  
  //                               d theta/d y = (1/x)/[1+(y/x)^2]
  //                               d theta/d z = 0
  // phi = arccos (z/radius) = arccos(z), therefore d phi/d x= d phi/d y= 0
  //                                                d phi/d z= -1/sqrt(1-z^2)

  X= sin(inPhi)*cos(inTheta);
  Y= sin(inPhi)*sin(inTheta);
  Z= cos(inPhi);

  if (fabs(X)>SmallThreshold)
    {
    YOverX= Y/X;
    YOverXSquared= YOverX*YOverX; 
    derivativeThetaOverX= (-YOverX/X)/(1.0+YOverXSquared);
    derivativeThetaOverY= (1.0/X)/(1.0+YOverXSquared);
    }
  else
    {
    YOverX= 1.0; //give it some value: phi=0 or PI 
    derivativeThetaOverX= 0.0; //undefined at North or South Pole
    derivativeThetaOverY= 0.0; //undefined at North or South Pole
    }

  if (fabs(Z)<=(OneLessSmallThreshold))
    {
    derivativePhiOverZ= (-1.0/sqrt(1.0-(Z*Z))); 
    }
  else
    {
    derivativePhiOverZ= 0.0; //undefined at North or South Pole
    }

  result[0]= functionDerivativeOverTheta * derivativeThetaOverX; 
  result[1]= functionDerivativeOverTheta * derivativeThetaOverY; 
  result[2]= functionDerivativeOverPhi * derivativePhiOverZ; 
     
  return result; 
}
                                    

int main( int argc, char * argv [] )
{

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
  float radius, phi, theta, myTheta; //spherical coordinates
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

#if 0    
    myTheta= theta + M_PI_4; 
    if (myTheta>M_PI) 
      {
      myTheta-= TWO_PI; 
      }
#endif

    myValue= sineMapSphericalCoordinatesFunction(phi, theta); 

    myMesh->SetPointData(i, myValue);

    std::cout << "Point[" << i << "]: " << myPt << " radius "
              << radius << "  myTheta " << myTheta << "  phi "
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
                    MeshType,
                    double > InterpolatorType;

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
    //cellPointer->ComputeCenterOfGravity();
    //myCellCenter= cellPointer->GetCenterOfGravity();
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
         //std::cout << "  cell Center " << myCellCenter <<  "  cellPoint " <<   cellPoint << "   "; 
         pointIdIterator++;
      }
    for( unsigned int i = 0; i < 3; i++ )
    {
      myCellCenter[i]/= cellPointer->GetNumberOfPoints();
    }
    //std::cout << "  final cell Center " << myCellCenter << std::endl;
    cellCenterRadius= sqrt(myCellCenter[0]*myCellCenter[0] +
                           myCellCenter[1]*myCellCenter[1] +
                           myCellCenter[2]*myCellCenter[2]); //assuming radius is not valued 1
    cellCenterTheta= atan2(myCellCenter[1], myCellCenter[0]); 
    cellCenterPhi= acos(myCellCenter[2]/cellCenterRadius); 

#if 0    
    for ( unsigned int i=0; i<3; i++ ) 
      {
        myCellCenter[i]-= myCenter[i];   //coordinates relative to center, if center is not origin
      }
#endif

    InterpolatorType::DerivativeType computedDerivative; 
    interpolator->EvaluateDerivative(myCellCenter, computedDerivative); 

#if 1  
    std::cout << " faceId  " << faceId << "  cell Center " << myCellCenter
              << "  analytical derivative " 
              << sineMapSphericalCoordinatesFunctionGradient(cellCenterPhi, cellCenterTheta)
              << "  computed derivative "
              << computedDerivative
              << std::endl;
#endif    
    }
  std::cout << "Test End "<< std::endl;


  return EXIT_SUCCESS;

}

