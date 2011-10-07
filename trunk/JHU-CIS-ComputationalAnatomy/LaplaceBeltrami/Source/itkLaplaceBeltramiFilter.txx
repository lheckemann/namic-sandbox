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
#include "vnl/vnl_copy.h"
#include "vnl/vnl_copy.cxx"
#include "vnl/vnl_sparse_matrix.txx"
#include "vnl/algo/vnl_sparse_symmetric_eigensystem.h"

#include <float.h>  // for DBL_MIN
#include "itkArray.h"

// debug
#include <fstream>

namespace itk
{
/**
 *
 */
template <class TInputMesh, class TOutputMesh>
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::LaplaceBeltramiFilter()
{
  SetEigenValueCount(0);
  SetHarmonicScaleValue(0.0);
  SetBoundaryConditionType(DirichletCondition);
}

/**
 * Get the Laplace Beltrami operator
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetLBOperator( LBMatrixType& lbOp ) const
{
  lbOp = this->m_LBOperator;
}

/**
 * Get the areas for each vertex
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetVertexAreas( LBMatrixType& lbVa ) const
{
  lbVa = this->m_VertexAreas;
}

/**
 * Get the surface harmonics at each vertex
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetHarmonics( HarmonicSetType& harms ) const
{
  harms = this->m_Harmonics;
}

/**
 * Get the eigenvalues of the solution
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::GetEigenvalues( EigenvalueSetType& harms ) const
{
  harms = this->m_Eigenvalues;
}

/**
 *
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent << "Harmonic " << this->m_Harmonics << std::endl;
  os << indent << "Eigen Value Count " << this->m_EigenValueCount << std::endl;
  os << indent << "Vertex Areas dimensions " << this->m_VertexAreas.rows() << " x "
     << this->m_VertexAreas.cols() << std::endl;
  os << indent << "Laplace Beltrami Operator dimensions "
     << this->m_LBOperator.rows() << " x "
     << this->m_LBOperator.cols() << std::endl;
  os << indent << "BoundaryConditionType "
     << this->m_BoundaryConditionType << std::endl;

}

/**
 *
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::SetInput(TInputMesh *input)
{
  this->ProcessObject::SetNthInput(0, input);
}

/**
 * This method causes the filter to generate its output.
 */
template <class TInputMesh, class TOutputMesh>
void
LaplaceBeltramiFilter< TInputMesh, TOutputMesh >
::GenerateData(void)
{
  typedef typename TInputMesh::PointsContainer  InputPointsContainer;
  typedef typename TOutputMesh::PointsContainer OutputPointsContainer;

  typedef typename TInputMesh::PointsContainerConstPointer
    InputPointsContainerConstPointer;
  typedef typename TOutputMesh::PointsContainerPointer
    OutputPointsContainerPointer;

  this->CopyInputMeshToOutputMesh();

  InputMeshConstPointer  inputMesh      =  this->GetInput();
  OutputMeshPointer      outputMesh     =  this->GetOutput();

  unsigned int sides = 3;

  InputPointsContainerConstPointer  inPoints  = inputMesh->GetPoints();

  unsigned int cellCount = inputMesh->GetNumberOfCells();
  unsigned int vertexCount = inputMesh->GetNumberOfPoints();

  itk::Array<double> faceAreas(cellCount);
  itk::Array<double> vertexAreas(vertexCount);
  itk::Array<unsigned int> vertexCounts(vertexCount);

  faceAreas.Fill(0.0);
  vertexAreas.Fill(0.0);
  vertexCounts.Fill(0);

  InputCellAutoPointer cellPtr;

  // compute areas of faces and vertices
  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    unsigned int aCellNumberOfPoints = cellPtr->GetNumberOfPoints();
    if( aCellNumberOfPoints != sides )
      {
      itkExceptionMacro("cell has " << aCellNumberOfPoints << " points\n"
      "This filter can only process triangle meshes.");
      return;
      }

    const unsigned long *tp;
    tp = cellPtr->GetPointIds();

    InputPointType v1;
    InputPointType v2;
    InputPointType v3;
    inputMesh->GetPoint((int)(tp[0]), &v1);
    inputMesh->GetPoint((int)(tp[1]), &v2);
    inputMesh->GetPoint((int)(tp[2]), &v3);

    // determine if face is obtuse
    vnl_vector<double> x12(InputPointDimension);
    vnl_copy((v2-v1).GetVnlVector(), x12);
    vnl_vector<double> x13(InputPointDimension);
    vnl_copy((v3-v1).GetVnlVector(), x13);
    vnl_vector<double> x23(InputPointDimension);
    vnl_copy((v3-v2).GetVnlVector(), x23);

    double norm12 = x12.two_norm();
    double norm13 = x13.two_norm();
    double norm23 = x23.two_norm();

    double c1 = vnl_c_vector<double>::dot_product(x12.data_block(), x13.data_block(), x12.size());
    c1 /= norm12 * norm13;
    double c2 =  - vnl_c_vector<double>::dot_product(x12.data_block(), x23.data_block(), x12.size());
    c2 /= norm12 * norm23;
    double c3 = vnl_c_vector<double>::dot_product(x13.data_block(), x23.data_block(), x13.size());
    c3 /= norm13 * norm23;

    faceAreas(cellId) = vnl_cross_3d(x12, x13).two_norm() / 2.0;

    if (c1 < 0.0)  // if the projection is negative, angle 1 is obtuse
      {
      vertexAreas(tp[0]) += faceAreas(cellId) / 2.0;
      vertexAreas(tp[1]) += faceAreas(cellId) / 4.0;
      vertexAreas(tp[2]) += faceAreas(cellId) / 4.0;
      }
    else if (c2 < 0.0)  // if the projection is negative, angle 2 is obtuse
      {
      vertexAreas(tp[0]) += faceAreas(cellId) / 4.0;
      vertexAreas(tp[1]) += faceAreas(cellId) / 2.0;
      vertexAreas(tp[2]) += faceAreas(cellId) / 4.0;
      }
    else if (c3 < 0.0)  // if the projection is negative, angle 3 is obtuse
      {
      vertexAreas(tp[0]) += faceAreas(cellId) / 4.0;
      vertexAreas(tp[1]) += faceAreas(cellId) / 4.0;
      vertexAreas(tp[2]) += faceAreas(cellId) / 2.0;
      }
    else  // no obtuse angles
      {
      double cot1 = c1 / sqrt(1.0 - c1 * c1);
      double cot2 = c2 / sqrt(1.0 - c2 * c2);
      double cot3 = c3 / sqrt(1.0 - c3 * c3);
      vertexAreas(tp[0]) += (
        vnl_c_vector<double>::dot_product(x12.data_block(), x12.data_block(), x12.size()) * cot3 +
        vnl_c_vector<double>::dot_product(x13.data_block(), x13.data_block(), x13.size()) * cot2) / 8.0;
      vertexAreas(tp[1]) += (
        vnl_c_vector<double>::dot_product(x12.data_block(), x12.data_block(), x12.size()) * cot3 +
        vnl_c_vector<double>::dot_product(x23.data_block(), x23.data_block(), x23.size()) * cot1) / 8.0;
      vertexAreas(tp[2]) += (
        vnl_c_vector<double>::dot_product(x13.data_block(), x13.data_block(), x13.size()) * cot2 +
        vnl_c_vector<double>::dot_product(x23.data_block(), x23.data_block(), x23.size()) * cot1) / 8.0;
      }
    }

  const double FaceAreaTolerance = 1.0e-15;
  for (unsigned int vertIx = 0; vertIx < vertexCount; vertIx++)
    {
    if (vertexAreas(vertIx) < FaceAreaTolerance)
      {
      itkExceptionMacro("Vertex " << vertIx << " has virtually no face area:  "
        << vertexAreas(vertIx) << "\n");
      return;
      }
    }

  // compute edges and detect boundary
  vnl_sparse_matrix<unsigned int> edgeMatrix(vertexCount, vertexCount);
  itk::Array2D<unsigned int> edges(cellCount * 3, 2);
  edges.Fill(0);
  unsigned int edgeCount = 0;

  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    const unsigned long *tp;
    tp = cellPtr->GetPointIds();

    if (edgeMatrix(tp[0], tp[1]) == 0)
      {
      edgeMatrix(tp[0], tp[1]) = edgeCount + 1;
      edgeMatrix(tp[1], tp[0]) = edgeCount + 1;
      edges(edgeCount, 0) = tp[0];
      edges(edgeCount, 1) = tp[1];
      edgeCount++;
      }
    if (edgeMatrix(tp[1], tp[2]) == 0)
      {
      edgeMatrix(tp[1], tp[2]) = edgeCount + 1;
      edgeMatrix(tp[2], tp[1]) = edgeCount + 1;
      edges(edgeCount, 0) = tp[1];
      edges(edgeCount, 1) = tp[2];
      edgeCount++;
      }
    if (edgeMatrix(tp[0], tp[2]) == 0)
      {
      edgeMatrix(tp[2], tp[0]) = edgeCount + 1;
      edgeMatrix(tp[0], tp[2]) = edgeCount + 1;
      edges(edgeCount, 0) = tp[2];
      edges(edgeCount, 1) = tp[0];
      edgeCount++;
      }
    }

  edges = edges.extract(edgeCount, 2);
  typedef vnl_sparse_matrix<unsigned int> EdgeFaceMatrixType;
  EdgeFaceMatrixType edgeFace(edgeCount, cellCount);

  for (unsigned int cellId = 0; cellId < cellCount; cellId++)
    {
    inputMesh->GetCell( cellId, cellPtr );
    const unsigned long *tp;
    tp = cellPtr->GetPointIds();
    edgeFace(edgeMatrix(tp[0], tp[1]) - 1, cellId) = 1;
    edgeFace(edgeMatrix(tp[1], tp[2]) - 1, cellId) = 1;
    edgeFace(edgeMatrix(tp[2], tp[0]) - 1, cellId) = 1;
    }

  itk::Array<unsigned int> boundaryEdge(edgeCount);
  itk::Array<unsigned int> boundaryVertex(vertexCount);

  boundaryEdge.Fill(0);
  boundaryVertex.Fill(0);

  itk::Array2D<double> edgeAngles(edgeCount, 2);
  edgeAngles.Fill(0.0);

  for (unsigned int edgeIx = 0; edgeIx < edgeCount; edgeIx++)
    {
    EdgeFaceMatrixType::row aRow = edgeFace.get_row(edgeIx);
    for (unsigned int u = 0; u < aRow.size(); u++)
      {
      int faceIx = aRow[u].first;

      // get vertices of the face
      inputMesh->GetCell( faceIx, cellPtr );
      const unsigned long *tp;
      tp = cellPtr->GetPointIds();

      long i1 = -1;
      long i2 = -1;

      for (unsigned int vIx = 0; vIx < 3; vIx++)
        {
        if (tp[vIx] == edges(edgeIx, 0))
          {
          i1 = vIx;
          }
        if (tp[vIx] == edges(edgeIx, 1))
          {
          i2 = vIx;
          }
        }

      if( i1 < 0 || i2 < 0 )
        {
        itkExceptionMacro("failed to find edges for cell " << faceIx << "\n");
        return;
        }

      unsigned int s = i1 + i2;
      unsigned int i3;
      if (s == 1)
        {
        i3 = 2;
        }
      else if (s == 2)
        {
        i3 = 1;
        }
      else if (s == 3)
        {
        i3 = 0;
        }
      else
        {
        itkExceptionMacro("bad vertex indices " << i1 << ", " << i2 << " for face " << faceIx << "\n");
        return;
        }

      InputPointType verts[3];
      inputMesh->GetPoint((int)(tp[0]), &verts[0]);
      inputMesh->GetPoint((int)(tp[1]), &verts[1]);
      inputMesh->GetPoint((int)(tp[2]), &verts[2]);

      vnl_vector<double> x1(InputPointDimension);
      vnl_copy((verts[i1] - verts[i3]).GetVnlVector(), x1);
      vnl_vector<double> x2(InputPointDimension);
      vnl_copy((verts[i2] - verts[i3]).GetVnlVector(), x2);
      vnl_vector<double> v21Xv32(InputPointDimension);
      vnl_copy(vnl_cross_3d((verts[1] - verts[0]).GetVnlVector(),
                              (verts[2] - verts[0]).GetVnlVector()), v21Xv32);
      vnl_vector<double> x1Xx2 = vnl_cross_3d(x1, x2);

      double a = vnl_c_vector<double>::dot_product(
        x1Xx2.data_block(), v21Xv32.data_block(), x1Xx2.size());
      double b = vnl_c_vector<double>::dot_product(x1.data_block(), x2.data_block(), x1.size());

      if (a > 0)
        {
        edgeAngles(edgeIx, u) = b / sqrt(a);
        }
      else
        {
        edgeAngles(edgeIx, u) = b / sqrt(-a);
        }
      }

    if (aRow.size() == 1)
      {
      // have a boundary edge
      boundaryEdge(edgeIx) = 1;
      boundaryVertex(edges(edgeIx, 0)) = 1;
      boundaryVertex(edges(edgeIx, 1)) = 1;
      edgeAngles(edgeIx, 1) = 0;
      }
    }

  // compute the Laplacian matrix
  m_LBOperator.set_size(vertexCount, vertexCount);

  for (unsigned int edgeIx = 0; edgeIx < edgeCount; edgeIx++)
    {
    double laplacian = 
        (edgeAngles(edgeIx, 0) + edgeAngles(edgeIx, 1)) / 2.0;
    m_LBOperator(edges(edgeIx, 0), edges(edgeIx, 1)) = laplacian;
    m_LBOperator(edges(edgeIx, 1), edges(edgeIx, 0)) = laplacian;
    }

  m_VertexAreas.set_size(vertexCount, vertexCount);
  for (unsigned int vertexIx = 0; vertexIx < vertexCount; vertexIx++)
    {
    m_VertexAreas(vertexIx, vertexIx) = vertexAreas(vertexIx);
    double accum = 0.0;
    LBMatrixType::row aRow = m_LBOperator.get_row(vertexIx);
    for (unsigned int u = 0; u < aRow.size(); u++)
      {
      accum += aRow[u].second;
      }
    m_LBOperator(vertexIx, vertexIx) = -accum;
    }

  // compute harmonics?
  if (this->m_EigenValueCount)
    {

    // Need to solve the following eigensystem:
    // A * V = B * V * D, or
    // m_LBOperator * eVectors = m_VertexAreas * eVectors * eValues

    int elementCount = vertexCount;
    LBMatrixType A, B;

    if (!boundaryVertex.sum())
      {
      // closed surface
      A = m_LBOperator;
      B = m_VertexAreas;
      }
    else if (m_BoundaryConditionType == VonNeumanCondition)
      {
      // for Von Neuman, zero out the "area" of the boundary vertices
      // before solving standard eigenvalue problem
      A = m_LBOperator;
      B.set_size(m_VertexAreas.rows(), m_VertexAreas.cols());

      elementCount = 0;
      // Zero out diagonal matrix for boundary values
      for (unsigned int ix = 0; ix < vertexCount; ix++)
        {
        if (!boundaryVertex(ix))
          {
          B(ix, ix) = m_VertexAreas(ix, ix);
          elementCount++;
          }
        }
      }
    else if (m_BoundaryConditionType == DirichletCondition)
      {
      // non-boundary point count
      elementCount -= boundaryVertex.sum();
      A.set_size(elementCount, elementCount);
      B.set_size(elementCount, elementCount);

      int colCount = 0;
      int rowCount = 0;
      // collapse matrix by removing boundary values
      for (unsigned int ix = 0; ix < vertexCount; ix++)
        {
        colCount = 0;
        if (!boundaryVertex(ix))
          {
          B(rowCount, rowCount) = m_VertexAreas(ix, ix);
          for (unsigned int jx = 0; jx < vertexCount; jx++)
            {
            if (!boundaryVertex(jx))
              {
              if (m_LBOperator(ix, jx))
                {
                A(rowCount, colCount) = m_LBOperator(ix, jx);
                }
              colCount++;
              }
            }
          rowCount++;
          }
        }
      }
    else
      {
      itkExceptionMacro("Unknown Boundary Condition Type:  " << m_BoundaryConditionType << "\n");
      return;
      }

    vnl_sparse_symmetric_eigensystem sse;
    int eigensystemResult = 0;
    if ((eigensystemResult =
            sse.CalculateNPairs(A, B, this->m_EigenValueCount,
                      1.0e-10, 0, false, true,
                      10000, 1.0)) != 0)
      {
      // failed to compute eigenvalues
      itkExceptionMacro("Eigensystem failure with result:  " <<
                                                eigensystemResult << "\n");
      return;
      }

    // get the output
    this->m_Eigenvalues.set_size(this->m_EigenValueCount);
    this->m_Harmonics.set_size(this->m_EigenValueCount, vertexCount);
    for (unsigned int evIx = 0; evIx < this->m_EigenValueCount; evIx++)
      {
      this->m_Eigenvalues(evIx) = sse.get_eigenvalue(evIx);
      vnl_vector< double > sseEigenvector = sse.get_eigenvector(evIx);
      vnl_vector< double > eigenvector(vertexCount);
      eigenvector = 0.0;

      int nonboundaryCount = 0;
      for (unsigned int vxIx = 0; vxIx < vertexCount; vxIx++)
        {
        if (!boundaryVertex(vxIx) ||
            (m_BoundaryConditionType != DirichletCondition))
          {
          eigenvector(vxIx) = sseEigenvector(nonboundaryCount++);
          }
        }
      // the eigenvectors come out of ARPACK++ backwards compared to vnl/MATLAB
      this->m_Harmonics.set_row((this->m_EigenValueCount - 1) - evIx, eigenvector);
      }

    this->SetSurfaceHarmonic(0);
    }
  else
    {
    this->m_Harmonics.clear();
    }
}

/**
 * Get a single surface harmonic
 */
template <class TInputMesh, class TOutputMesh>
bool
LaplaceBeltramiFilter<TInputMesh, TOutputMesh>
::SetSurfaceHarmonic( unsigned int harmonic )
{
  if ( harmonic >= this->m_Harmonics.rows() )
    {
    return false;  // FIXME: Why to fail silently ? shouldn't this throw an exception ?
    }

  OutputMeshPointer surface = this->GetOutput();

  vnl_vector<double> thisHarm = m_Harmonics.get_row(harmonic);
  if (this->m_HarmonicScaleValue)
    {
    // find max abs value of this harmonic
    double maxAbs = thisHarm.inf_norm();
    thisHarm *= ((this->m_HarmonicScaleValue - 1.0) / 2.0 )/ maxAbs;
    thisHarm += this->m_HarmonicScaleValue / 2.0;
    }

  for (unsigned int k = 0; k < this->m_Harmonics.cols(); k++)
    {
    surface->SetPointData(k, thisHarm(k));
    }

  this->Modified();
      
  return true;
}

} // end namespace itk

#endif
