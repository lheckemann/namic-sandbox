/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $RCSfile: igstkPivotCalibrationAlgorithm.cxx,v $
  Language:  C++
  Date:      $Date: 2009-06-15 20:57:59 $
  Version:   $Revision: 1.2 $

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkPivotCalibrationAlgorithm.h"
#include "vtkObjectFactory.h"

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vtkMatrix4x4.h"

vtkStandardNewMacro(vtkPivotCalibrationAlgorithm);
vtkCxxRevisionMacro(vtkPivotCalibrationAlgorithm, "$Revision: 1.0 $");

const double vtkPivotCalibrationAlgorithm::DEFAULT_SINGULAR_VALUE_THRESHOLD = 1e-1;

//---------------------------------------------------------------------------
vtkPivotCalibrationAlgorithm::vtkPivotCalibrationAlgorithm()

{
  this->SetSingularValueThreshold( DEFAULT_SINGULAR_VALUE_THRESHOLD );
  this->m_Transform = vtkMatrix4x4::New();
  this->bCalibrationComputationError = true;
  std::cerr << "CalibrationAlgorithm object created successfully" << std::endl;
}

//---------------------------------------------------------------------------
vtkPivotCalibrationAlgorithm::~vtkPivotCalibrationAlgorithm()
{
  this->m_Transforms.clear();
}

//---------------------------------------------------------------------------
void vtkPivotCalibrationAlgorithm::AddTransformsVector( std::vector< vtkMatrix4x4* > t )
{
  std::vector< vtkMatrix4x4* >::const_iterator it = t.begin();
  std::vector< vtkMatrix4x4* >::const_iterator transformsEnd = t.end();
  while(it!=transformsEnd)
    {
    this->m_Transforms.push_back((*it));
    it++;
    }
  std::cerr << "Transform vector added successfully" << std::endl;
}

//------------------------------------------------------------------
void vtkPivotCalibrationAlgorithm::SetSingularValueThreshold( double threshold )
{
  this->m_SingularValueThreshold = threshold;
}

//-------------------------------------------------------------------
bool vtkPivotCalibrationAlgorithm::GetErrorStatus()
{
  return this->bCalibrationComputationError;
}

//---------------------------------------------------------------------
void vtkPivotCalibrationAlgorithm::ResetCalibration()
{
  //Reset all calibration variables
  this->m_Transforms.clear();
  this->m_Transform->Zero();
  this->m_RMSE = 0;
  this->m_Translation[0] = 0; this->m_Translation[1] = 0; this->m_Translation[2] = 0;
  this->m_PivotPoint[0] = 0; this->m_PivotPoint[1] = 0; this->m_PivotPoint[2] = 0;
  this->bCalibrationComputationError = true;
  std::cerr << "Calibration Reset successfully" << std::endl;
}

//------------------------------------------------------------------------
void vtkPivotCalibrationAlgorithm::ComputeCalibration()
{
  if(this->m_Transforms.empty())
    {
    bCalibrationComputationError = true;
    return;
    }

  std::cerr << "Starting computation" << std::endl;
  unsigned int rows = 3*this->m_Transforms.size();
  unsigned int columns = 4;

  vnl_matrix< double > A( rows, columns ), minusI( 3, 3, 0 ), R(3,1); //minusI is initialized with ones
  vnl_vector< double > b(rows), x(columns), t(3), r(3);
  minusI( 0, 0 ) = -1;
  minusI( 1, 1 ) = -1;
  minusI( 2, 2 ) = -1;

  //do the computation and set the internal variables
  std::vector<vtkMatrix4x4*>::const_iterator it, transformsEnd = this->m_Transforms.end();
  unsigned int currentRow;
  for( currentRow = 0, it = this->m_Transforms.begin();
       it != transformsEnd;
       it++, currentRow += 3 )
    {
    //Build the vector of independent coefficients
    for (int i = 0; i < 3; i++)
      {
      t(i) = (*it)->GetElement(i, 3);
      }
    t *= -1;
    b.update( t, currentRow );
    //Build the matrix with the equation coefficients
    for (int i = 0; i < 3; i++)
      {
      //for (int j = 0; j < 3; j++)
      R(i, 0) = (*it)->GetElement(i, 2);
      }
    A.update(R, currentRow, 0);
    A.update( minusI, currentRow, 1 );
    }

  //solve the equations
  vnl_svd<double> svdA(A);

  svdA.zero_out_absolute( this->m_SingularValueThreshold );

  //there is a solution only if rank(A)=4 (columns are linearly
  //independent)
  if( svdA.rank() < 4 )
    {
    bCalibrationComputationError = true;
    }
  else
    {
    x = svdA.solve( b );

    //set the RMSE
    this->m_RMSE = ( A * x - b ).rms();

    //set the transformation
    this->m_Transform->Identity();
    this->m_Translation[0] = 0.0;
    this->m_Translation[1] = 0.0;
    this->m_Translation[2] = x[0];
    this->m_Transform->SetElement(2,3,m_Translation[2]);

    //set the pivot point
    this->m_PivotPoint[0] = x[1];
    this->m_PivotPoint[1] = x[2];
    this->m_PivotPoint[2] = x[3];

    //Print out solution to screen
    std::cerr << "Pivot point location: " << this->m_PivotPoint << std::endl;
    std::cerr << "Offset from tip to sensor: " << x[0] << std::endl;
    std::cerr << "Root mean squared error: " << this->m_RMSE << std::endl;

    bCalibrationComputationError = false;
    }
}

//----------------------------------------------------------------------
void vtkPivotCalibrationAlgorithm::GetCalibrationTransform(vtkMatrix4x4* trans)
{
  trans->DeepCopy(this->m_Transform);
}

//-----------------------------------------------------------------------
/*void
PivotCalibrationAlgorithm::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Transforms for pivot calibration: " << std::endl;
  TransformContainerType::const_iterator it,
    endTransforms = this->m_Transforms.end();
  for( it=this->m_Transforms.begin(); it != endTransforms; it++)
    os << indent << *it << std::endl;
}
*/
