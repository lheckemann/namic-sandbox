/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkConformalFlatteningFilter.txx,v $
Language:  C++
Date:      $Date: 2003/09/10 14:28:58 $
Version:   $Revision: 1.22 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkConformalFlatteningFilter_txx
#define _itkConformalFlatteningFilter_txx

#include "itkConformalFlatteningFilter.h"
#include "itkExceptionObject.h"



namespace itk
{
  
  /**
   *
   */
  template <class TInputMesh, class TOutputMesh>
  ConformalFlatteningFilter<TInputMesh,TOutputMesh>
  ::ConformalFlatteningFilter() {
    //  m_Transform = TransformType::New();
  }


  /**
   *
   */
  template <class TInputMesh, class TOutputMesh>
  void 
  ConformalFlatteningFilter<TInputMesh,TOutputMesh>
  ::PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    //   if (m_Transform)
    //     {
    //     os << indent << "Transform: " << m_Transform << std::endl;
    //     }
  }


  /**
   * This method causes the filter to generate its output.
   */
  template <class TInputMesh, class TOutputMesh>
  void 
  ConformalFlatteningFilter<TInputMesh,TOutputMesh>
  ::GenerateData(void) 
  {
  
    typedef typename TInputMesh::PointsContainer  InputPointsContainer;
    typedef typename TOutputMesh::PointsContainer OutputPointsContainer;

    typedef typename TInputMesh::PointsContainerPointer  InputPointsContainerPointer;
    typedef typename TOutputMesh::PointsContainerPointer OutputPointsContainerPointer;

    InputMeshPointer    inputMesh      =  this->GetInput();
    OutputMeshPointer   outputMesh     =  this->GetOutput();
  
    if( !inputMesh )
      {
        itkExceptionMacro(<<"Missing Input Mesh");
      }

    if( !outputMesh )
      {
        itkExceptionMacro(<<"Missing Output Mesh");
      }

    outputMesh->SetBufferedRegion( outputMesh->GetRequestedRegion() );

    InputPointsContainerPointer  inPoints  = inputMesh->GetPoints();
    OutputPointsContainerPointer outPoints = outputMesh->GetPoints();

    outPoints->Reserve( inputMesh->GetNumberOfPoints() );
    outPoints->Squeeze();  // in case the previous mesh had 
    // allocated a larger memory

    typename InputPointsContainer::ConstIterator  inputPoint  = inPoints->Begin();
    typename OutputPointsContainer::Iterator      outputPoint = outPoints->Begin();

    // Create duplicate references to the rest of data on the mesh

    outputMesh->SetPointData(  inputMesh->GetPointData() );
  
    outputMesh->SetCellLinks(  inputMesh->GetCellLinks() );
  
    outputMesh->SetCells(  inputMesh->GetCells() );
    outputMesh->SetCellData(  inputMesh->GetCellData() );
  
    mapping(inputMesh, outputMesh);
    
    //   this->Modified();
  
//     unsigned int maxDimension = TInputMesh::MaxTopologicalDimension;

//     for( unsigned int dim = 0; dim < maxDimension; dim++ ) {
//       outputMesh->SetBoundaryAssignments(  dim,
//                                            inputMesh->GetBoundaryAssignments(dim) );
//     }
  }// GenerateData()

  template <class TInputMesh, class TOutputMesh>
  void
  ConformalFlatteningFilter<TInputMesh,TOutputMesh>::
  mapping( InputMeshPointer iMesh, OutputMeshPointer oMesh) {

    

    const unsigned int numberOfPoints = iMesh->GetNumberOfPoints();
    vnl_sparse_matrix<typename TInputMesh::CoordRepType> D(numberOfPoints, numberOfPoints);
    vnl_vector<typename TInputMesh::CoordRepType> bR(numberOfPoints, 0);
    vnl_vector<typename TInputMesh::CoordRepType> bI(numberOfPoints, 0);

    getDb( iMesh, D , bR, bI);
  
    
    //    linearEqnSolver lesR(D, bR);
    vnl_vector<double> zR = solveLinearEq(D, bR);//lesR.solve();  

    //    linearEqnSolver lesI(D, bI);
    vnl_vector<double> zI = solveLinearEq(D, bI);//lesI.solve();

    std::vector<double> x(numberOfPoints), y(numberOfPoints), z(numberOfPoints);
    std::vector<double>::iterator 
      itX = x.begin(), 
      itY = y.begin(), 
      itZ = z.begin(), 
      itXend = x.end();
  
    for (int it = 0; itX != itXend; ++itX, ++itY, ++itZ, ++it) {
      double r2 = zR(it)*zR(it) + zI(it)*zI(it);
      *itX = 2*zR(it)/(1+r2);
      *itY = 2*zI(it)/(1+r2);
      *itZ = 2*r2/(1+r2) - 1;

      typename TInputMesh::CoordRepType apoint[3] = {*itX, *itY, *itZ};
      //typename TInputMesh::CoordRepType apoint[3] = {zR(it), zI(it), 0};
      //      typename TInputMesh::CoordRepType apoint[3] = {pointXYZ[it][0], pointXYZ[it][1], 0};    
      //       std::cerr<<"zR: "<<zR(it)<<"    zI: "<<zI(it)<<std::endl;
      //       std::cerr<<"x: "<<*itX<<"    y: "<<*itY<<"    z: "<<*itZ<<"   r2: "<<r2<<std::endl;

      //      mesh->SetPoint( it, MeshType::PointType( apoint ));
      oMesh->SetPoint( it,typename TOutputMesh::PointType( apoint ));
    } // for it
    //  MeshType::Pointer newMesh = MeshType::New();

    //
    // set new point position, leaving the triangles' relation unchanged
    //
  
    //  mesh->GetPoints()->Reserve( numberOfPoints );
  
    //   for(unsigned int p =0; p < numberOfPoints; p++)
    //     {

    //       //       typename TInputMesh::CoordRepType* apoint = vtkpoints->GetPoint( p );
      
    
    //     }  
    return;
  }//  mapping()

  template <class TInputMesh, class TOutputMesh>
  void
  ConformalFlatteningFilter<TInputMesh,TOutputMesh>::
  getDb(OutputMeshPointer mesh, 
        vnl_sparse_matrix<typename TInputMesh::CoordRepType> &D,
        vnl_vector<typename TInputMesh::CoordRepType> &bR,
        vnl_vector<typename TInputMesh::CoordRepType> &bI) {
  
    int numOfPoints = mesh->GetNumberOfPoints();
    int numOfCells = mesh->GetNumberOfCells();

    // 1. store the points coordinates: pointXYZ
    std::vector< std::vector<typename TInputMesh::CoordRepType> > pointXYZ( numOfPoints, std::vector<typename TInputMesh::CoordRepType>(3, 0) );

    PointIterator pntIterator = mesh->GetPoints()->Begin();

    for ( int it = 0; it < numOfPoints; ++it, ++pntIterator) {
      PointType pnt = pntIterator.Value();
    
      pointXYZ[it][0] = pnt[0];
      pointXYZ[it][1] = pnt[1];
      pointXYZ[it][2] = pnt[2];
    }

    // 2. store the relationship from point to cell, i.e. for each
    // point, which cells contain it?  For each point in the mesh,
    // generate a vector, storing the id number of triangles containing
    // this point.  The vectors of all the points form a vector:
    // pointCell

    // 3. store the relationship from cell to point, i.e. for each cell,
    // which points does it contains? store in vector: cellPoint
    std::vector< std::vector<int> > pointCell( numOfPoints );
    std::vector< std::vector<int> > cellPoint( numOfCells, std::vector<int>(3, 0) );

    CellIterator cellIt = mesh->GetCells()->Begin();

    for ( int itCell = 0;
          itCell < numOfCells;
          ++itCell, ++cellIt) {
    
      CellType * cellptr = cellIt.Value(); 
      // cellptr will point to each cell in the mesh
      // std::cout << cellptr->GetNumberOfPoints() << std::endl;

      PointIdIterator pntIdIter = cellptr->PointIdsBegin(); 
      //pntIdIter will point to each point in the current cell
      PointIdIterator pntIdEnd = cellptr->PointIdsEnd();

      for (int itPntInCell = 0; pntIdIter != pntIdEnd; ++pntIdIter, ++itPntInCell)
        {
          pointCell[ *pntIdIter ].push_back(itCell);
          cellPoint[itCell][itPntInCell] = *pntIdIter;
        }
    }


    //   //--------------------------------------------------------------
    //   // print out the result
    //   std::cout<<std::endl;
    //   std::cout<<std::endl;
    //   std::cout<<std::endl;
    //   std::cout<<std::endl;

    //   for (int it = 0; it < numOfPoints; ++it) {
    //     std::cout<<"point# "<<it<<" :"
    //              <<"       X: "<<pointXYZ[it][0]
    //              <<"       Y: "<<pointXYZ[it][1]
    //              <<"       Z: "<<pointXYZ[it][2]<<std::endl;
    //   }

    //   for (int it = 0; it < numOfPoints; ++it) {
    //     std::cout<<"point# "<<it<<" is contained by    "<<pointCell[it].size()<<"   cells:"<<std::endl;
    //     for (std::vector<int>::const_iterator vi = pointCell[it].begin();
    //          vi != pointCell[it].end();
    //          ++vi) {
    //       std::cout<<*vi<<"     ";      
    //     }
    //     std::cout<<std::endl;
    //   }

    //   for (int it = 0; it < numOfCells; ++it) {
    //     std::cout<<"cell# "<<it<<" has points: "
    //              <<cellPoint[it][0]<<"  "
    //              <<cellPoint[it][1]<<"  "
    //              <<cellPoint[it][2]<<std::endl;
    //   }
    //   //---------------------------------------------------------------


    // 1. Iterate point P from 0 to the last point in the mesh. 
    // 2. For each P, find its neighbors, each neighbor must:
    //    1) has at least two triangles containing P and itself ---not the boundary.
    //    2) has larger pointId, to avoid re-calculation.
    // 3. For each of P's neighbors, Q, calculate R, S
    // 4. Write the value in matrix.
    std::vector< std::vector<int> >::iterator itP, itPEnd = pointCell.end();
    int idP = 0;
    for ( itP = pointCell.begin(); itP != itPEnd; ++itP, ++idP) {
      std::vector<int> neighborOfP;
      // for each point P, traverse all cells containing it.
      std::vector<int>::iterator itCell = (*itP).begin();
      std::vector<int>::iterator itCellEnd = (*itP).end();

      for (; itCell != itCellEnd; ++itCell) {
        // for each cell containing P, store the point with larger point Id.
        // only three points, don't use for-loop to save time.
        if ( cellPoint[*itCell][0] > idP ) 
          neighborOfP.push_back(cellPoint[*itCell][0]);
        if ( cellPoint[*itCell][1] > idP ) 
          neighborOfP.push_back(cellPoint[*itCell][1]);
        if ( cellPoint[*itCell][2] > idP ) 
          neighborOfP.push_back(cellPoint[*itCell][2]);
      }// ok, now all neighbors of P is stored in neighborOfP;
    
      sort(neighborOfP.begin(), neighborOfP.end());
      std::vector<int>::iterator it;
      it = unique(neighborOfP.begin(), neighborOfP.end());
      neighborOfP.erase(it, neighborOfP.end());

      //-----------------------------------------------
      // print out the neighbors
      //     std::vector<int>::iterator itNeighbor = neighborOfP.begin();
      //     std::vector<int>::iterator itNeighborEnd = neighborOfP.end();
      //     std::cerr<<"The neighbors of "<<idP<<" are: ";
      //     for (; itNeighbor != itNeighborEnd; ++itNeighbor) {
      //       std::cerr<<*itNeighbor<<" , ";
      //     }
      //     std::cerr<<std::endl;
      // ----------------------------------------------------

      // next, from P to each neighbor...
      // note: itP and itQ point at different type of vectors...
      // *itP is a vector containing a list of cell Ids, all of which contains point P
      // idP is the point Id of P
      // *itQ is the point Id of Q (so idP and *itQ are same type)
      std::vector<int>::iterator itQ, itQEnd = neighborOfP.end();
      for ( itQ = neighborOfP.begin(); itQ != itQEnd; ++itQ) {
        // first check whether PQ is a boundary edge:
        std::vector<int> cellsContainingP(*itP), cellsContainingQ(pointCell[*itQ]);
        std::vector<int> cells(cellsContainingP.size() + cellsContainingQ.size());
        std::vector<int>::iterator itv, endIter;

        sort(cellsContainingP.begin(), cellsContainingP.end());
        sort(cellsContainingQ.begin(), cellsContainingQ.end());

        endIter = set_intersection(cellsContainingP.begin(), cellsContainingP.end(),
                                   cellsContainingQ.begin(), cellsContainingQ.end(),
                                   cells.begin());
        cells.erase(endIter, cells.end());
        if (cells.size() != 2) continue;
        // If P and Q are not shared by two triangles, i.e. 1: are not
        // connected by and edge, or, 2: are on the surface boundary
        // thus only shared by one triangle. then skip.  However, in
        // this paper the surface is closed thus there is not boundary.

        // If passed test above, then P and Q are two valid points.
        // i.e. PQ is a valid edge.  i.e. cells now contain two int's,
        // which are the Id of the triangles containing P and Q


        //       //------------------------------------------------------------
        //       //print out valid edge
        //       std::cerr<<idP<<" and "<<*itQ<<" are two valid points"<<std::endl;
        //       std::cerr<<(endIter == cells.end())<<std::endl;
        //       //-----------------------------------------------------------


        // Next we extract R and S from cells
        int itS, itR; // the Id of point S and R;
        for (int it = 0; it < 3; ++it) {
          if (cellPoint[cells[0]][it] != idP && cellPoint[cells[0]][it] != *itQ) 
            itS = cellPoint[cells[0]][it];
          if (cellPoint[cells[1]][it] != idP && cellPoint[cells[1]][it] != *itQ) 
            itR = cellPoint[cells[1]][it];
        }

        std::vector<double> P(pointXYZ[idP]), 
          Q(pointXYZ[*itQ]), 
          R(pointXYZ[itR]),
          S(pointXYZ[itS]);

        std::vector<double> SP(3), SQ(3), RP(3), RQ(3);
        double SPnorm = 0, SQnorm = 0, RPnorm = 0, RQnorm = 0, SPSQinnerProd = 0, RPRQinnerProd = 0;
        for (int it = 0; it<3; ++it) {
          SP[it] = P[it] - S[it]; SPnorm += SP[it]*SP[it];
          SQ[it] = Q[it] - S[it]; SQnorm += SQ[it]*SQ[it]; SPSQinnerProd += SP[it]*SQ[it];
          RP[it] = P[it] - R[it]; RPnorm += RP[it]*RP[it];
          RQ[it] = Q[it] - R[it]; RQnorm += RQ[it]*RQ[it]; RPRQinnerProd += RP[it]*RQ[it];
        } //it
        SPnorm = sqrt(SPnorm);
        SQnorm = sqrt(SQnorm);
        RPnorm = sqrt(RPnorm);
        RQnorm = sqrt(RQnorm);

        double cosS = SPSQinnerProd / (SPnorm * SQnorm); 
        double cosR = RPRQinnerProd / (RPnorm * RQnorm);
        double ctgS = cosS/sqrt(1-cosS*cosS), ctgR = cosR/sqrt(1-cosR*cosR);

        D(idP, *itQ) = -0.5*(ctgS + ctgR);
        D(idP, idP) += 0.5*(ctgS + ctgR); 
        // add to the diagonal element of this line.

        D(*itQ, idP) = -0.5*(ctgS + ctgR); // symmetric
        D(*itQ, *itQ) += 0.5*(ctgS + ctgR); 
        // add to the diagonal element of this line.
      } // itQ
    } // itP

    // compute b = bR + i*bI separately
    std::vector<double> A( pointXYZ[ cellPoint[0][0] ] ), 
      B( pointXYZ[ cellPoint[0][1] ] ), 
      C( pointXYZ[ cellPoint[0][2] ] );
    double ABnorm, CA_BAip; // the inner product of vector C-A and B-A;
    ABnorm = (A[0] - B[0]) * (A[0] - B[0])
      + (A[1] - B[1]) * (A[1] - B[1])
      + (A[2] - B[2]) * (A[2] - B[2]);

    CA_BAip = (C[0] - A[0]) * (B[0] - A[0])
      + (C[1] - A[1]) * (B[1] - A[1])
      + (C[2] - A[2]) * (B[2] - A[2]);

    double theta = CA_BAip / ABnorm; 
    // Here ABnorm is actually the square of AB's norm, which is what we
    // want. So don't bother square the square root.
  
    ABnorm = sqrt(ABnorm); // This is real norm of vector AB.

    std::vector<double> E(3);
    for (int it = 0; it < 3; ++it) 
      E[it] = A[it] + theta*(B[it] - A[it]);

    double CEnorm;
    CEnorm = (C[0] - E[0]) * (C[0] - E[0])
      + (C[1] - E[1]) * (C[1] - E[1])
      + (C[2] - E[2]) * (C[2] - E[2]);
    CEnorm = sqrt(CEnorm); // This is real norm of vector CE.

    bR(cellPoint[0][0]) = -1 / ABnorm;
    bR(cellPoint[0][1]) = 1 / ABnorm;

    bI(cellPoint[0][0]) = (1-theta)/ CEnorm;
    bI(cellPoint[0][1]) = theta/ CEnorm;
    bI(cellPoint[0][2]) = -1 / CEnorm;
  
    return; 
  } //getDb()

  //   template <class TInputMesh, class TOutputMesh>
  //   vnl_vector<typename TInputMesh::CoordRepType>
  //   ConformalFlatteningFilter<TInputMesh,TOutputMesh>
  //   :: solveLinearEq(vnl_matrix<typename TInputMesh::CoordRepType> const& A, 
  //                    vnl_vector<typename TInputMesh::CoordRepType> const& b) {
    
  //     theFunc<typename TInputMesh::CoordRepType> f(A, b);

  //     vnl_conjugate_gradient cg;
  //     vnl_vector<typename TInputMesh::CoordRepType> x(f.dim(), 0);
  //     cg.minimize(x);
  //     return x;
  //   }

  template <class TInputMesh, class TOutputMesh>
  vnl_vector<typename TInputMesh::CoordRepType>
  ConformalFlatteningFilter<TInputMesh,TOutputMesh>
  ::solveLinearEq(vnl_sparse_matrix<typename TInputMesh::CoordRepType> const& A, 
                  vnl_vector<typename TInputMesh::CoordRepType> const& b) {
    
    theFunc<typename TInputMesh::CoordRepType> f(A, b);

    vnl_conjugate_gradient cg(f);
    vnl_vector<typename TInputMesh::CoordRepType> x(f.dim(), 0);
    cg.minimize(x);
    return x;
  }




} // end namespace itk





//-------------------------------------------------------
//for linear equation solver


////////////////////////////////////////////////////////////////////
// implementation of class theFunc
// template <class matrixDataType>
// theFunc<matrixDataType>::theFunc(vnl_matrix<matrixDataType> const& A, 
//                                  vnl_vector<matrixDataType> const& b)
//   :_A(&A), _b(&b), _dim(b.size()), vnl_cost_function(b.size()), _sparse(false) {

//   if (A.rows() != b.size())
//     assert(!"The # of rows in A must be the same as the length of b!");    
// }

// overload construction function for sparse matrix A
template <class matrixDataType>
theFunc<matrixDataType>::theFunc(vnl_sparse_matrix<matrixDataType> const& A, 
                                 vnl_vector<matrixDataType> const& b)
  :_Asparse(&A), _b(&b), _dim(b.size()), vnl_cost_function(b.size()), _sparse(true) {

  if (A.rows() != b.size())
    assert(!"The # of rows in A must be the same as the length of b!");    
}


template <class matrixDataType>
double theFunc<matrixDataType>::f(vnl_vector<matrixDataType> const& x) {  
  matrixDataType r;
  if (_sparse == false) {
    r = 0.5*inner_product(x*(*_A),x)-inner_product((*_b),x);
  }
  else if (_sparse == true) {
    vnl_vector<matrixDataType> tmp;
    _Asparse -> pre_mult(x, tmp);
    r = 0.5*inner_product(tmp,x)-inner_product((*_b),x);
  }

  return r;
}

template <class matrixDataType>
void theFunc<matrixDataType>::gradf(vnl_vector<matrixDataType> const& x, 
                                    vnl_vector<matrixDataType> & g) {
  if (_sparse == false) {
    g = (*_A)*x - (*_b);
  }
  else if (_sparse == true) {
    vnl_vector<matrixDataType> tmp;
    _Asparse -> mult(x, tmp);
    g = tmp - (*_b);
  }
}

// implementation of class theFunc
////////////////////////////////////////////////////////////////////



// ////////////////////////////////////////////////////////////////////
// // implementation of class linearEqnSolver

// linearEqnSolver::linearEqnSolver(vnl_matrix<double> const& A, 
//                                  vnl_vector<double> const& b)
//   :_f(A, b), _cg(_f) {}

// // overload construction function for sparse matrix
// linearEqnSolver::linearEqnSolver(vnl_sparse_matrix<double> const& A, 
//                                  vnl_vector<double> const& b)
//   :_f(A, b), _cg(_f) {}


// vnl_vector<double> linearEqnSolver::solve() {
//   vnl_vector<double> x(_f.dim(), 0);
//   _cg.minimize(x);
//   return x;
// }
// // implementation of class linearEqnSolver
// ////////////////////////////////////////////////////////////////////



#endif
