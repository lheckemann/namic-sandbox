/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLaplaceBeltramiFilter.txx,v $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLaplaceBeltramiFilter_txx
#define __itkLaplaceBeltramiFilter_txx

#include "itkLaplaceBeltramiFilter.h"
#include "itkExceptionObject.h"

#include "vnl/vnl_math.h"
#include "vnl/vnl_cross.h"
#include "vnl/algo/vnl_sparse_symmetric_eigensystem.h"

#include <float.h>  // for DBL_MIN


namespace itk
{
/**
 *
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TCompRep >
::LaplaceBeltramiFilter()
{
  this->m_EigenValueCount = 0;

}

/**
 * Set the number of eigenvalues to produce
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh, TCompRep>
::SetEigenValueCount( unsigned int evCount )
{
  this->m_EigenValueCount = evCount;
};

/**
 * Get the Laplace Beltrami operator
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh, TCompRep>
::GetLBOperator( LBMatrixType& lbOp )
{
  lbOp = this->m_LBOperator;
};

/**
 * Get the areas for each vertex
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh, TCompRep>
::GetVertexAreas( LBMatrixType& lbVa )
{
  lbVa = this->m_VertexAreas;
};

/**
 *
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TCompRep >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
}

/**
 *
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TCompRep >
::SetInput(TInputMesh *input)
{
  this->ProcessObject::SetNthInput(0, input);
}

/**
 * This method causes the filter to generate its output.
 */
template <class TInputMesh, class TOutputMesh, typename TCompRep>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh, TCompRep >
::GenerateData(void)
{
  typedef typename TInputMesh::PointsContainer  InputPointsContainer;
  typedef typename TOutputMesh::PointsContainer OutputPointsContainer;

  typedef typename TInputMesh::PointsContainerConstPointer
    InputPointsContainerConstPointer;
  typedef typename TOutputMesh::PointsContainerPointer
    OutputPointsContainerPointer;

  InputMeshConstPointer  inputMesh      =  this->GetInput();
  OutputMeshPointer      outputMesh     =  this->GetOutput();

  unsigned int dim = 3;
  unsigned int sides = 3;

  if( !inputMesh )
    {
    itkExceptionMacro(<<"Missing Input Mesh");
    }

  if( !outputMesh )
    {
    itkExceptionMacro(<<"Missing Output Mesh");
    }

  outputMesh->SetBufferedRegion( outputMesh->GetRequestedRegion() );

  InputPointsContainerConstPointer  inPoints  = inputMesh->GetPoints();
  OutputPointsContainerPointer outPoints = outputMesh->GetPoints();

  const unsigned int numberOfPoints = inputMesh->GetNumberOfPoints();

  outPoints->Reserve( numberOfPoints );
  outPoints->Squeeze();  // in case the previous mesh had
                         // allocated a larger memory

  unsigned int cellCount = inputMesh->GetNumberOfCells();
  unsigned int vertexCount = inputMesh->GetNumberOfPoints();
  itk::Array<double> faceAreas(cellCount);
  itk::Array<double> vertexAreas(vertexCount);
  itk::Array<unsigned int> vertexCounts(vertexCount);

  faceAreas.Fill(0.0);
  vertexAreas.Fill(0.0);
  vertexCounts.Fill(0);

  CellAutoPointer cellPtr;

  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    unsigned int aCellNumberOfPoints = cellPtr->GetNumberOfPoints();
    if( aCellNumberOfPoints != dim )
      {
      itkExceptionMacro("cell has " << aCellNumberOfPoints << " points\n"
      "This filter can only process triangle meshes.");
      return;
      }

    const unsigned long *tp;
    tp = cellPtr->GetPointIds();

    InputPointType v0,v1,v2;
    inputMesh->GetPoint((int)(tp[0]), &v0);
    inputMesh->GetPoint((int)(tp[1]), &v1);
    inputMesh->GetPoint((int)(tp[2]), &v2);

    faceAreas(cellId) = vcl_abs(vnl_cross_3d(
        (v1-v0).GetVnlVector(), (v2-v0).GetVnlVector()).two_norm()/2.0);

    for (unsigned int vertexIx = 0; vertexIx < 3; vertexIx++)
      {
      vertexAreas(tp[vertexIx]) += faceAreas(cellId);
      vertexCounts(tp[vertexIx])++;
      }
    }

  for (unsigned int vertexId = 0; vertexId < vertexCount; vertexId++)
    {
    vertexAreas(vertexId) /= vertexCounts(vertexId);
    }

  m_LBOperator.set_size(vertexCount, vertexCount);
  unsigned int i0, i1, i2;
  unsigned int ix[sides];
  for (unsigned int i = 0; i < sides; i++)
    {
    ix[i] = i;
    }

  for (unsigned int faceIx = 0; faceIx < cellCount; faceIx++)
    {
    for (unsigned int l = 0; l < sides; l++)
      {
      inputMesh->GetCell( faceIx, cellPtr );
      const unsigned long *tp;
      tp = cellPtr->GetPointIds();

      InputPointType v0,v1,v2;
      inputMesh->GetPoint((int)(tp[ix[0]]), &v0);
      inputMesh->GetPoint((int)(tp[ix[1]]), &v1);
      inputMesh->GetPoint((int)(tp[ix[2]]), &v2);

      vnl_vector<double> e0((v2 - v1).GetVnlVector());
      vnl_vector<double> e1((v0 - v2).GetVnlVector());
      vnl_vector<double> e2((v1 - v0).GetVnlVector());

      vnl_c_vector<double>::multiply(
                e0.data_block(), e1.data_block(), e1.data_block(), e0.size());
      vnl_c_vector<double>::multiply(
                e0.data_block(), e2.data_block(), e2.data_block(), e0.size());
      vnl_c_vector<double>::multiply(
                e0.data_block(), e0.data_block(), e0.data_block(), e0.size());

      double c0 = e0.sum();
      double c1 = e1.sum();
      double c2 = e2.sum();

      double faceArea = faceAreas(faceIx);

      m_LBOperator(tp[ix[0]], tp[ix[1]]) -= c1 / faceArea;
      m_LBOperator(tp[ix[0]], tp[ix[2]]) -= c2 / faceArea;
      m_LBOperator(tp[ix[0]], tp[ix[0]]) -= c0 / faceArea;

      unsigned int last = ix[sides - 1];
      for(unsigned int i = sides - 1; i > 0; i--)
        {
        ix[i] = ix[i - 1];
        }
      ix[0] = last;

      }
    }

  // saves area as a sparse diagonal matrix
  m_VertexAreas.set_size(vertexCount, vertexCount);
  for (unsigned int k = 0; k < vertexCount; k++)
    {
    m_VertexAreas(k, k) = vertexAreas(k);
    }

  // compute harmonics?
  if (this->m_EigenValueCount)
    {

    // Need to solve the following eigensystem:
    // A * V = B * V * D, or
    // m_LBOperator * eVectors = m_VertexAreas * eVectors * eValues
    // the vnl_sparse_symmetric_eigensystem class works on the following system:
    // A * x = lambda * x, where x is an eigenvector and lambda an eigenvalue...
    // so, to use this class:
    //   (1) create a new sparse matrix C by dividing each nonzero coefficient
    //       of A, say A[k][l], by sqrt(B[k][k]*B[l][l]);
    //   (2) Solve the eigenvalue problem C*x = lambda*x for the first N
    //       eigenvectors; then A and C have the same eigenvalues.
    //   (3) let W[k][l] be the lth coordinate of the kth eigenvector of
    //       C (I am not sure of what the correct order of indices is, you may
    //       need to switch them). Define V[k][l] = W[k][l]/sqrt(B[l][l])
    //   (4) Divide all coefficients V[k][l] by sqrt(sum_l V[k][l]^2 B[l][l])
    //       for normalization.

    vnl_sparse_matrix< TCompRep > C(m_LBOperator);
    
    for(unsigned int rowIx = 0; rowIx < C.rows(); rowIx++)
    {
    for(unsigned int colIx = 0; colIx < C.cols(); colIx++)
      {
        if (C(rowIx, colIx))
        {
          C(rowIx, colIx) /= sqrt(m_VertexAreas(rowIx, rowIx) *
                            m_VertexAreas(colIx, colIx));
        }
      }
    }

    // get the eigenvalues for the sparse symmetric matrix
    vnl_sparse_symmetric_eigensystem sse;
    sse.CalculateNPairs(C, this->m_EigenValueCount, false);

    // get the output
    m_Harmonics.set_size(this->m_EigenValueCount, vertexCount);

    for (unsigned int k = 0; k < this->m_EigenValueCount; k++)
      {
      vnl_vector< TCompRep > eigenvector(sse.get_eigenvector(k));

      double evFactorSum = 0.0;
      for (unsigned int eigIx = 0; eigIx < eigenvector.size(); eigIx++)
      {
        eigenvector(eigIx) = eigenvector(eigIx) / sqrt(m_VertexAreas(eigIx, eigIx));
        evFactorSum += eigenvector(eigIx) * eigenvector(eigIx) * m_VertexAreas(eigIx, eigIx);
      }
      eigenvector /= sqrt(evFactorSum);
      m_Harmonics.set_row(k, eigenvector);
      }
    }
  outputMesh = inputMesh;
}

} // end namespace itk

#endif
