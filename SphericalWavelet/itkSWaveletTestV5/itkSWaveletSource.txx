/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    $RCSfile: itkSWaveletSource.txx,v $
Language:  C++
Date:      $Date: 2006/02/05 20:57:46 $
Version:   $Revision: 1.8 $

Copyright (c) Insight Software Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distribuetd WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSWaveletSource_txx
#define __itkSWaveletSource_txx


#include "itkSWaveletSource.h"

namespace itk
{

  /*
   *
   */
  template<class TOutputMesh>
  SWaveletSource<TOutputMesh>
  ::SWaveletSource()
  {
    /*
     * Create the output
     */
    typename TOutputMesh::Pointer output = TOutputMesh::New();
    this->ProcessObject::SetNumberOfRequiredOutputs(1);
    this->ProcessObject::SetNthOutput(0, output.GetPointer());
    m_Center.Fill(0);
    m_Scale.Fill(1);
    m_Resolution = 2;
    SWtransformDone = false;
    inverseSWtransformDone = false;
  }

  /*
   *
   */
  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::GenerateData()
  {
    GenerateSWaveletStructure();
    // generates and stores SW structure.

    typename OutputMeshType::Pointer outputMesh = this->GetOutput();  

    outputMesh->SetCellsAllocationMethod( OutputMeshType::CellsAllocatedDynamicallyCellByCell );

    // take out the finest level of subdivision to assign to the itkMesh
    std::vector< PointType > verts =  m_allVerts.back();
    std::vector< std::vector < unsigned long > > tris = m_allTriangles.back(); 
    std::vector< unsigned long > tri; // single triangle
    //    verts = m_allVerts[m_Resolution - 1];
    //    tris = m_allTriangles[m_Resolution - 1];

    typename std::vector< PointType >::const_iterator itVerts;
    typename std::vector< PointType >::const_iterator itVertsEnd = verts.end();
    unsigned long lPtIdx = 0;
    for (itVerts = verts.begin(); itVerts != itVertsEnd; ++itVerts, ++lPtIdx)
      {
        PointType pt = *itVerts;
        outputMesh->SetPoint(lPtIdx, pt);
      }

    typename std::vector< std::vector < unsigned long > >::const_iterator itTris = tris.begin();
    typename std::vector< std::vector < unsigned long > >::const_iterator itTrisEnd = tris.end();

    for (unsigned long cellIdx = 0; itTris != itTrisEnd; ++itTris, ++cellIdx)
      {
        tri = *itTris;
        unsigned long* pTri = &(tri[0]);
        this->AddCell(outputMesh, pTri, cellIdx);
      }

  } // Generate Data


  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::GenerateSWaveletStructure()
  {
    unsigned long tripoints[] = {0,1,2};

    std::vector< PointType > verts; // array of all vertices coordinates at one level
    std::vector< unsigned long > tri; // single triangle
    std::vector< std::vector < unsigned long > > tris; // array of all triangles at one level

    std::vector< std::vector < unsigned long > > A; // A set at one level
    std::vector< std::vector < unsigned long > > B; // A set at one level
    std::vector< std::vector < unsigned long > > C; // A set at one level

    PointType p1;
    unsigned long idx = 0;

    // vert 0
    p1[0] = 0 * m_Scale[0] + m_Center[0];
    p1[1] = 0 * m_Scale[1] + m_Center[1];
    p1[2] = 1 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //outputMesh->SetPoint(idx++, p1);

    // vert 1
    p1[0] = 0.8944272 * m_Scale[0] + m_Center[0];
    p1[1] = 0 * m_Scale[1] + m_Center[1];
    p1[2] = 0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 2
    p1[0] = 0.2763932 * m_Scale[0] + m_Center[0];
    p1[1] = 0.8506508 * m_Scale[1] + m_Center[1];
    p1[2] = 0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 3
    p1[0] = -0.7236068 * m_Scale[0] + m_Center[0];
    p1[1] = 0.5257311 * m_Scale[1] + m_Center[1];
    p1[2] = 0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 4  
    p1[0] = -0.7236068 * m_Scale[0] + m_Center[0];
    p1[1] = -0.5257311 * m_Scale[1] + m_Center[1];
    p1[2] = 0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 5
    p1[0] = 0.2763932 * m_Scale[0] + m_Center[0];
    p1[1] = -0.8506508 * m_Scale[1] + m_Center[1];
    p1[2] = 0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 6
    p1[0] = 0.7236068 * m_Scale[0] + m_Center[0];
    p1[1] = -0.5257311 * m_Scale[1] + m_Center[1];
    p1[2] = -0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 7
    p1[0] = 0.7236068 * m_Scale[0] + m_Center[0];
    p1[1] = 0.5257311 * m_Scale[1] + m_Center[1];
    p1[2] = -0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 8
    p1[0] = -0.2763932 * m_Scale[0] + m_Center[0];
    p1[1] = 0.8506508 * m_Scale[1] + m_Center[1];
    p1[2] = -0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 9
    p1[0] = -0.8944272 * m_Scale[0] + m_Center[0];
    p1[1] = 0 * m_Scale[1] + m_Center[1];
    p1[2] = -0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 10
    p1[0] = -0.2763932 * m_Scale[0] + m_Center[0];
    p1[1] = -0.8506508 * m_Scale[1] + m_Center[1];
    p1[2] = -0.4472136 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // vert 11
    p1[0] = 0 * m_Scale[0] + m_Center[0];
    p1[1] = 0 * m_Scale[1] + m_Center[1];
    p1[2] = -1 * m_Scale[2] + m_Center[2];
    verts.push_back(p1);
    //  outputMesh->SetPoint(idx++, p1);

    // Three points ranked Counter-ClockWise when looking from Outside, is this important?
    tripoints[0] = 0; tripoints[1] = 1; tripoints[2] = 2; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 0 );

    tripoints[0] = 0; tripoints[1] = 2; tripoints[2] = 3; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 1 );

    tripoints[0] = 0; tripoints[1] = 3; tripoints[2] = 4; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri); 
    //  this->AddCell( outputMesh, tripoints, 2 );

    tripoints[0] = 0; tripoints[1] = 4; tripoints[2] = 5; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 3 );

    tripoints[0] = 0; tripoints[1] = 5; tripoints[2] = 1; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 4 );

    tripoints[0] = 2; tripoints[1] = 1; tripoints[2] = 7; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 5 );

    tripoints[0] = 3; tripoints[1] = 2; tripoints[2] = 8; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 6 );

    tripoints[0] = 4; tripoints[1] = 3; tripoints[2] = 9; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 7 );

    tripoints[0] = 5; tripoints[1] = 4; tripoints[2] = 10; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 8 );

    tripoints[0] = 1; tripoints[1] = 5; tripoints[2] = 6; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 9 );

    tripoints[0] = 7; tripoints[1] = 1; tripoints[2] = 6; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 10 );

    tripoints[0] = 8; tripoints[1] = 2; tripoints[2] = 7; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 11 );

    tripoints[0] = 9; tripoints[1] = 3; tripoints[2] = 8; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 12 );

    tripoints[0] = 10; tripoints[1] = 4; tripoints[2] = 9; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 13 );

    tripoints[0] = 6; tripoints[1] = 5; tripoints[2] = 10; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 14 );

    tripoints[0] = 11; tripoints[1] = 6; tripoints[2] = 10; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 15 );

    tripoints[0] = 11; tripoints[1] = 7; tripoints[2] = 6; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 16 );

    tripoints[0] = 11; tripoints[1] = 8; tripoints[2] = 7; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 17 );

    tripoints[0] = 11; tripoints[1] = 9; tripoints[2] = 8; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 18 );

    tripoints[0] = 11; tripoints[1] = 10; tripoints[2] = 9; 
    tri.assign(tripoints, tripoints+3);
    tris.push_back(tri);
    //  this->AddCell( outputMesh, tripoints, 19 );

    m_allVerts.push_back(verts);
    m_allTriangles.push_back(tris);
    m_allA.push_back(A);
    m_allB.push_back(B);
    m_allC.push_back(C);
    // Although A, B, C are empty, they each occupy a position in m_allX for level 0.

    for (unsigned int i = 0; i < m_Resolution; i++) 
      {
        unsigned long nOfVerts = 10*pow(4,i) + 2; // for icosahedron.
        unsigned long nOfNewVerts = 10*pow(4,i+1) + 2; // for icosahedron.

        // store the relationship from point to cell, i.e. for each
        // point, which cells contain it?  For each point in the mesh,
        // generate a vector, storing the id number of triangles containing
        // this point.  The vectors of all the points form a vector:
        // pointTris

        std::vector< std::vector< unsigned long > > pointTris( nOfVerts );
        std::vector< std::vector< unsigned long > >::iterator itTris = tris.begin();
        std::vector< std::vector< unsigned long > >::iterator itTrisEnd = tris.end();

        for ( unsigned long iCell = 0; itTris != itTrisEnd; ++itTris, ++iCell) 
          {
            pointTris[(*itTris)[0]].push_back(iCell);
            pointTris[(*itTris)[1]].push_back(iCell);
            pointTris[(*itTris)[2]].push_back(iCell);
          } // for itTris

        A.resize(nOfNewVerts);
        B.resize(nOfNewVerts);
        C.resize(nOfNewVerts);

        std::vector< PointType > newVerts(nOfNewVerts);
        std::vector< std::vector < unsigned long > > newTris;

        std::pair< unsigned long, unsigned long > edge; // single edge, denoted by its two end points
        std::map< std::pair< unsigned long, unsigned long >, unsigned long> edges; //array of all edges
        std::map< std::pair< unsigned long, unsigned long >, unsigned long>::iterator itEdges1, itEdges2;


        PointType v1, v2, v3;
        const unsigned long *tp;
        unsigned long pointIdx,cellIdx=0;
        unsigned long pointIdxOffset = nOfVerts;
        pointIdx = pointIdxOffset;
        unsigned long newIdx[3] = {0,1,2};

        for( itTris = tris.begin(); itTris != itTrisEnd; ++itTris ) 
          {
            tri = *itTris;
            tp = &(tri[0]);
            // tp[0], tp[1] and tp[2] has the index of the three points of
            // current triangle cell, respectively.
            v1 = verts[tp[0]];
            v2 = verts[tp[1]];
            v3 = verts[tp[2]];

            newVerts[tp[0]] = v1;
            newVerts[tp[1]] = v2;
            newVerts[tp[2]] = v3;

            A[tp[0]].resize(1,-1); // for unsigned long, -1==4294967295
            A[tp[1]].resize(1,-1);
            A[tp[2]].resize(1,-1);
            B[tp[0]].resize(1,-1);
            B[tp[1]].resize(1,-1);
            B[tp[2]].resize(1,-1);
            C[tp[0]].resize(1,-1);
            C[tp[1]].resize(1,-1);
            C[tp[2]].resize(1,-1);

            // Just copy those three points into the subdivided mesh.
            // Next will deal with the mid-points.

            // point 1
            itEdges1 = edges.find( std::make_pair(tp[0], tp[1]) );
            itEdges2 = edges.find( std::make_pair(tp[1], tp[0]) );
            if ( itEdges1 == edges.end() && itEdges2 == edges.end())
              {
                newIdx[0]=pointIdx;
                edges.insert(std::make_pair( std::make_pair(tp[0], tp[1]), pointIdx ));
                setABC(pointIdx, tp[0], tp[1], A, B, C, tris, pointTris);
                newVerts[pointIdx++] = this->Divide(v1, v2);
              }
            else
              {
                if ( itEdges1 != edges.end() )
                  {
                    newIdx[0] = itEdges1->second;
                  }
                else 
                  {
                    newIdx[0] = itEdges2->second;
                  }
              }

            // point 2
            itEdges1 = edges.find( std::make_pair(tp[1], tp[2]) );
            itEdges2 = edges.find( std::make_pair(tp[2], tp[1]) );
            if ( itEdges1 == edges.end() && itEdges2 == edges.end())
              {
                newIdx[1]=pointIdx;
                edges.insert(std::make_pair( std::make_pair(tp[1], tp[2]), pointIdx ));
                setABC(pointIdx, tp[1], tp[2], A, B, C, tris, pointTris);
                newVerts[pointIdx++] = this->Divide(v2, v3);
              }
            else
              {
                if ( itEdges1 != edges.end() )
                  {
                    newIdx[1] = itEdges1->second;
                  }
                else 
                  {
                    newIdx[1] = itEdges2->second;
                  }
              }


            // point 3
            itEdges1 = edges.find( std::make_pair(tp[2], tp[0]) );
            itEdges2 = edges.find( std::make_pair(tp[0], tp[2]) );
            if ( itEdges1 == edges.end() && itEdges2 == edges.end())
              {
                newIdx[2]=pointIdx;
                edges.insert(std::make_pair( std::make_pair(tp[2], tp[0]), pointIdx ));
                setABC(pointIdx, tp[2], tp[0], A, B, C, tris, pointTris);
                newVerts[pointIdx++] = this->Divide(v3, v1);
              }
            else
              {
                if ( itEdges1 != edges.end() )
                  {
                    newIdx[2] = itEdges1->second;
                  }
                else 
                  {
                    newIdx[2] = itEdges2->second;
                  }
              }


            std::vector< unsigned long > newTri; // new single triangle

            tripoints[0] = tp[0]; 
            tripoints[1] = newIdx[0]; 
            tripoints[2] = newIdx[2]; 
            newTri.assign(tripoints, tripoints+3);
            newTris.push_back(newTri);

            tripoints[0] = newIdx[0];
            tripoints[1] = tp[1];
            tripoints[2] = newIdx[1];
            newTri.assign(tripoints, tripoints+3);
            newTris.push_back(newTri);


            tripoints[0] = newIdx[1];
            tripoints[1] = tp[2];
            tripoints[2] = newIdx[2]; 
            newTri.assign(tripoints, tripoints+3);
            newTris.push_back(newTri);

            tripoints[0] = newIdx[0];
            tripoints[1] = newIdx[1]; 
            tripoints[2] = newIdx[2]; 
            newTri.assign(tripoints, tripoints+3);
            newTris.push_back(newTri);
          } //     for(itTris)


        m_allVerts.push_back(newVerts);
        m_allTriangles.push_back(newTris);

        verts = newVerts;
        tris = newTris;  // update for next level computation

        m_allA.push_back(A);
        m_allB.push_back(B);
        m_allC.push_back(C);

        // //         display all values in A
        //         std::cout<<A.size()<<std::endl;
        //         for(unsigned long l=0; l<A.size(); ++l) {
        //           std::cout<<"size of "<<l<<" -th: "<< A[l].size()<<"\t";
        //           std::cout<<A[l][0]<<"\t"<<A[l][1]<<std::endl;
        //         }

        // //         display all values in B
        //         std::cout<<B.size()<<std::endl;
        //         for(unsigned long l=0; l<B.size(); ++l) {
        //           std::cout<<"size of "<<l<<" -th: "<< B[l].size()<<"\t";
        //           std::cout<<B[l][0]<<"\t"<<B[l][1]<<std::endl;
        //         }

        // //         display all values in C
        //         std::cout<<C.size()<<std::endl;
        //         for(unsigned long l=0; l<C.size(); ++l) {
        //           std::cout<<"size of "<<l<<" -th: "<< C[l].size()<<"\t";
        //           std::cout<<C[l][0]<<"\t"<<C[l][1]<<"\t"<<C[l][2]<<"\t"<<C[l][3]<<std::endl;
        //         }


        // #ifdef DEBUGing
        //         std::cerr<<verts.size()<<std::endl;        
        //         std::cerr<<tris.size()<<std::endl;        
        // #endif        

      } // for int i = 0, i < m_Resolution

    if (m_Resolution > 0) setInteg();

  }



  template<class TOutputMesh>
  typename SWaveletSource<TOutputMesh>::PointType
  SWaveletSource<TOutputMesh>
  ::Divide( const PointType & p1, const PointType & p2) const
  {
    PointType p;
    PointType f;

    VectorType d;
    VectorType c;

    d = p2 - p1;
    p = p1 + (d * 0.5);
    c = p - m_Center;

    f[0] = m_Scale[0] / c.GetNorm();
    f[1] = m_Scale[1] / c.GetNorm();
    f[2] = m_Scale[2] / c.GetNorm();

    c[0] *= f[0];
    c[1] *= f[1];
    c[2] *= f[2];

    return (m_Center + c);
  }



  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::setABC(unsigned long lnewPtIdx, const unsigned long& l1, const unsigned long& l2,
           std::vector< std::vector< unsigned long > > &A,
           std::vector< std::vector< unsigned long > > &B,
           std::vector< std::vector< unsigned long > > &C,
           const std::vector< std::vector< unsigned long > > &tris,
           const std::vector< std::vector< unsigned long > > &pointTris)
  {
    // for set A:
    A[lnewPtIdx].push_back(l1);
    A[lnewPtIdx].push_back(l2);

    // for set B:
    // 1st, From l1, l2, find the two triangles containing l1 and l2.
    std::vector< unsigned long > trisContainingl1 = pointTris[l1];
    std::vector< unsigned long > trisContainingl2 = pointTris[l2];
    std::vector< unsigned long > trisIntersection(trisContainingl1.size() + trisContainingl2.size());
    std::vector< unsigned long >::iterator itv, endIter;

    sort(trisContainingl1.begin(), trisContainingl1.end());
    sort(trisContainingl2.begin(), trisContainingl2.end());

    endIter = set_intersection(trisContainingl1.begin(), trisContainingl1.end(),
                               trisContainingl2.begin(), trisContainingl2.end(),
                               trisIntersection.begin());
    trisIntersection.erase(endIter, trisIntersection.end());
    // trisIntersection contains two triangle idx'es, both of them contain l1 and l2

    // 2nd, in those two triangles, find two points distinct from l1, and l2
    unsigned long lB1, lB2;// Id of points in B;
    for (unsigned long it = 0; it < 3; ++it)
      {
        if ( (tris[trisIntersection[0]])[it] != l1 && (tris[trisIntersection[0]])[it] != l2 )
          lB1 = (tris[trisIntersection[0]])[it];
        if ( (tris[trisIntersection[1]])[it] != l1 && (tris[trisIntersection[1]])[it] != l2 )
          lB2 = (tris[trisIntersection[1]])[it];
      }
    B[lnewPtIdx].push_back(lB1);
    B[lnewPtIdx].push_back(lB2);

    // for set C:
    // 1/4
    // 1st, From l1, lB1, find the two triangles containing l1 and lB1.
    std::vector< unsigned long > trisContaininglB1 = pointTris[lB1];
    trisIntersection.resize(trisContainingl1.size() + trisContaininglB1.size());

    sort(trisContaininglB1.begin(), trisContaininglB1.end());

    endIter = set_intersection(trisContainingl1.begin(), trisContainingl1.end(),
                               trisContaininglB1.begin(), trisContaininglB1.end(),
                               trisIntersection.begin());
    trisIntersection.erase(endIter, trisIntersection.end());
    // trisIntersection contains two triangle idx'es, both of them contain l1 and lB1

    // 2nd, in those two triangles, find two points distinct from l1, and lB1
    unsigned long lC1;// Id of one point in C;
    for (unsigned long it = 0; it < 3; ++it)
      {
        if ( (tris[trisIntersection[0]])[it] != l1 
             && (tris[trisIntersection[0]])[it] != lB1
             && (tris[trisIntersection[0]])[it] != l2 )
          lC1 = (tris[trisIntersection[0]])[it];

        if ( (tris[trisIntersection[1]])[it] != l1 
             && (tris[trisIntersection[1]])[it] != lB1
             && (tris[trisIntersection[1]])[it] != l2 )
          lC1 = (tris[trisIntersection[1]])[it];
      }
    C[lnewPtIdx].push_back(lC1);

    // 2/4
    // 1st, From l1, lB2, find the two triangles containing l1 and lB2.
    std::vector< unsigned long > trisContaininglB2 = pointTris[lB2];
    trisIntersection.resize(trisContainingl1.size() + trisContaininglB2.size());

    sort(trisContaininglB2.begin(), trisContaininglB2.end());

    endIter = set_intersection(trisContainingl1.begin(), trisContainingl1.end(),
                               trisContaininglB2.begin(), trisContaininglB2.end(),
                               trisIntersection.begin());
    trisIntersection.erase(endIter, trisIntersection.end());
    // trisIntersection contains two triangle idx'es, both of them contain l1 and lB2

    // 2nd, in those two triangles, find two points distinct from l1, and lB2
    unsigned long lC2;// Id of one point in C;
    for (unsigned long it = 0; it < 3; ++it)
      {
        if ( (tris[trisIntersection[0]])[it] != l1 
             && (tris[trisIntersection[0]])[it] != lB2
             && (tris[trisIntersection[0]])[it] != l2 )
          lC2 = (tris[trisIntersection[0]])[it];

        if ( (tris[trisIntersection[1]])[it] != l1 
             && (tris[trisIntersection[1]])[it] != lB2
             && (tris[trisIntersection[1]])[it] != l2 )
          lC2 = (tris[trisIntersection[1]])[it];
      }
    C[lnewPtIdx].push_back(lC2);

    // 3/4
    // 1st, From l2, lB2, find the two triangles containing l2 and lB2.
    trisIntersection.resize(trisContainingl2.size() + trisContaininglB2.size());

    endIter = set_intersection(trisContainingl2.begin(), trisContainingl2.end(),
                               trisContaininglB2.begin(), trisContaininglB2.end(),
                               trisIntersection.begin());
    trisIntersection.erase(endIter, trisIntersection.end());
    // trisIntersection contains two triangle idx'es, both of them contain l2 and lB2

    // 2nd, in those two triangles, find two points distinct from l2, and lB2
    unsigned long lC3;// Id of one point in C;
    for (unsigned long it = 0; it < 3; ++it)
      {
        if ( (tris[trisIntersection[0]])[it] != l2 
             && (tris[trisIntersection[0]])[it] != lB2
             && (tris[trisIntersection[0]])[it] != l1 )
          lC3 = (tris[trisIntersection[0]])[it];

        if ( (tris[trisIntersection[1]])[it] != l2 
             && (tris[trisIntersection[1]])[it] != lB2
             && (tris[trisIntersection[1]])[it] != l1 )
          lC3 = (tris[trisIntersection[1]])[it];
      }
    C[lnewPtIdx].push_back(lC3);


    // 4/4
    // 1st, From l2, lB1, find the two triangles containing l2 and lB1.
    trisIntersection.resize(trisContainingl2.size() + trisContaininglB1.size());

    endIter = set_intersection(trisContainingl2.begin(), trisContainingl2.end(),
                               trisContaininglB1.begin(), trisContaininglB1.end(),
                               trisIntersection.begin());
    trisIntersection.erase(endIter, trisIntersection.end());
    // trisIntersection contains two triangle idx'es, both of them contain l2 and lB1

    // 2nd, in those two triangles, find two points distinct from l2, and lB1
    unsigned long lC4;// Id of one point in C;
    for (unsigned long it = 0; it < 3; ++it)
      {
        if ( (tris[trisIntersection[0]])[it] != l1 
             && (tris[trisIntersection[0]])[it] != lB1
             && (tris[trisIntersection[0]])[it] != l2 )
          lC4 = (tris[trisIntersection[0]])[it];

        if ( (tris[trisIntersection[1]])[it] != l1 
             && (tris[trisIntersection[1]])[it] != lB1
             && (tris[trisIntersection[1]])[it] != l2 )
          lC4 = (tris[trisIntersection[1]])[it];
      }
    C[lnewPtIdx].push_back(lC4);
  }


  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::setInteg( void )
  {
    m_allInteg.resize(m_Resolution+1); // note that size() = lastIdx + 1, so m_Resolution+1

    std::vector< PointType > verts =  m_allVerts.back();
    std::vector< std::vector < unsigned long > > tris = m_allTriangles.back(); 

    std::vector< std::vector< unsigned long > > pointTris( verts.size() );
    std::vector< std::vector< unsigned long > >::iterator itTris = tris.begin();
    std::vector< std::vector< unsigned long > >::iterator itTrisEnd = tris.end();
    for ( unsigned long iCell = 0; itTris != itTrisEnd; ++itTris, ++iCell) {
      pointTris[(*itTris)[0]].push_back(iCell);
      pointTris[(*itTris)[1]].push_back(iCell);
      pointTris[(*itTris)[2]].push_back(iCell);
    } // for itTris

    std::vector< double > vdAreaOfTris(tris.size()); // area of each triangle
    std::vector< double >::iterator itvdAreaOfTris = vdAreaOfTris.begin();
    //    std::vector< double >::iterator itvdAreaOfTrisEnd = itvdAreaOfTris.end();

    PointType p0, p1, p2;
    double v0x, v0y, v0z, v1x, v1y, v1z;
    for (itTris = tris.begin(); itTris != itTrisEnd; ++itTris, ++itvdAreaOfTris) {
      p0 = verts[(*itTris)[0]];
      p1 = verts[(*itTris)[1]];
      p2 = verts[(*itTris)[2]];
      v0x = p1[0] - p0[0]; // a
      v0y = p1[1] - p0[1]; // b
      v0z = p1[2] - p0[2]; // c
      v1x = p2[0] - p0[0]; // x
      v1y = p2[1] - p0[1]; // y
      v1z = p2[2] - p0[2]; // z


      double dArea = 0.5*sqrt( (v0y*v1z - v0z*v1y)*(v0y*v1z - v0z*v1y) 
                               + (v0z*v1x - v0x*v1z)*(v0z*v1x - v0x*v1z) 
                               + (v0x*v1y - v0y*v1x)*(v0x*v1y - v0y*v1x) );

      //       std::cerr<<"[ "<<(*itTris)[0]<<", "<<(*itTris)[1]<<", "<<(*itTris)[2]<<"] :"
      //                <<"\t\t"<<"( "<<p0[0]<<", "<<p0[1]<<", "<<p0[2]<<") "
      //                <<"  ( "<<p1[0]<<", "<<p1[1]<<", "<<p1[2]<<") "
      //                <<"  ( "<<p2[0]<<", "<<p2[1]<<", "<<p2[2]<<") "
      //                <<"\t\t\t\t Area: "<<dArea<<std::endl;

      *itvdAreaOfTris = dArea;
    } // itTris

    typename std::vector< PointType >::const_iterator itVerts = verts.begin();
    typename std::vector< PointType >::const_iterator itVertsEnd = verts.end();
    std::vector< std::vector< unsigned long > >::const_iterator itPointTris = pointTris.begin();

    std::vector< double > vInteg(verts.size()); // Integ at this level
    std::vector< double >::iterator itvInteg = vInteg.begin();

    for (itVerts = verts.begin(); itVerts != itVertsEnd; ++itVerts, ++itPointTris, ++itvInteg) {
      double vol = 0;
      for (int i = 0; i<(*itPointTris).size(); ++i) {
        vol += 1*vdAreaOfTris[ (*itPointTris)[i] ]; // height * area
      }
      *itvInteg = vol/3;
    } // itVerts

    m_allInteg.back() = vInteg; // The integ for finest level is by
    // computation, the rest are by
    // recursive computation, as following:

    // It's been verified that the area integration at the finest level is correct.

    for (int iRes = m_Resolution - 1; iRes >= 0; --iRes) {

      std::vector< double > vIntegLower((m_allInteg[iRes+1]).begin(), 
                                        (m_allInteg[iRes+1]).begin() + (m_allVerts[iRes]).size() );

      std::vector< double >::iterator itvInteg = (m_allInteg[iRes+1]).begin() + (m_allVerts[iRes]).size();
      std::vector< double >::iterator itvIntegEnd = (m_allInteg[iRes+1]).end();
      for (unsigned long m = (m_allVerts[iRes]).size(); itvInteg != itvIntegEnd; ++itvInteg, ++m) {
        // A:
        vIntegLower[m_allA[iRes+1][m][0]] += 0.5*(*itvInteg);
        vIntegLower[m_allA[iRes+1][m][1]] += 0.5*(*itvInteg);
        // B:
        vIntegLower[m_allB[iRes+1][m][0]] += 0.125*(*itvInteg);
        vIntegLower[m_allB[iRes+1][m][1]] += 0.125*(*itvInteg);
        // C:
        vIntegLower[m_allC[iRes+1][m][0]] -= 0.0625*(*itvInteg);
        vIntegLower[m_allC[iRes+1][m][1]] -= 0.0625*(*itvInteg);
        vIntegLower[m_allC[iRes+1][m][2]] -= 0.0625*(*itvInteg);
        vIntegLower[m_allC[iRes+1][m][3]] -= 0.0625*(*itvInteg);
      } // for m
      m_allInteg[iRes] = vIntegLower;
    } // for iRes

    for (int i = 0; i<m_allInteg.size(); ++i) {
      itvInteg = (m_allInteg[i]).begin();
      double sum = 0;
      std::vector< double >::iterator itvIntegEnd = (m_allInteg[i]).end();
      for (; itvInteg != itvIntegEnd; ++itvInteg) {
        //      std::cerr<<*itvInteg<<std::endl;
        sum += *itvInteg;
      }
      //      std::cerr<<"sum of all integration at level:\t"<<i<<"\t"<<sum<<std::endl;
    }
    //    std::cerr<<std::endl<<std::endl<<std::endl<<std::endl;
  } // setInteg()


  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::SetScalarFunction( const std::vector< double >& vF )
  {
    if ( vF.size() != ((m_allVerts.back()).size()) ) {
      std::cerr<<"Scalar function is defined on the finest subdivision."
               <<"Thus it must have the same number of vertices as the finest subdivision."
               <<std::endl;
      exit(-1);
    }
    else {
      m_ScalarFunction = vF;
    }
  }

  template<class TOutputMesh>
  std::vector< double >
  SWaveletSource<TOutputMesh>
  ::GetScalarFunction( void )
  {
    if (!m_ScalarFunction.empty()) {
      return m_ScalarFunction;
    }
    else {
      std::cerr<<"Scalar function hasn't been assigned yet."<<std::endl;
      exit(-1);
    }
  }

  template<class TOutputMesh>
  std::vector< double >
  SWaveletSource<TOutputMesh>
  ::GetReconstructedScalarFunction( void )
  {
    if ( inverseSWtransformDone == false ) {
      std::cerr<<"Backward transform has not been done."<<std::endl;
      exit(-1);
    }
    else {
      return m_ReconstructedLamda.back();
    }
  }

  template<class TOutputMesh>
  std::vector< double >
  SWaveletSource<TOutputMesh>
  ::GetCoefficients( void )
  {
    if ( SWtransformDone == false ) {
      std::cerr<<"Forward transform has not been done."<<std::endl;
      exit(-1);
    }
    else {
      return m_FinalGamma;
    }

  }

  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::SWaveletTransform()
  {
    std::vector< std::vector< double > > m_Lamda( m_Resolution+1 );
    m_Lamda.back() = m_ScalarFunction;

    std::vector< std::vector< double > > m_Gamma( m_Resolution );

    // Gamma stores the detail of each level.
    // Gamma of level N has the (# of Verts) of level N+1
    // The begining elements, from 0 to (# of Verts) of this level, are 0.

    for (int iRes = m_Resolution - 1; iRes >= 0; --iRes) {

      unsigned long lM = (m_allVerts[iRes]).size();
      unsigned long lM1 = (m_allVerts[iRes+1]).size();

      std::vector< double > gamma( lM1 );
      m_Gamma[iRes].resize( lM1 );

      std::vector< double > lamda( lM ); // Scaling function coeffients.
      m_Lamda[iRes].resize(lM);

      // Analysis_Stage I
      for (unsigned long m = lM; m < lM1; ++m) {
        gamma[m] = m_Lamda[iRes+1][m] 
          - 0.5*   ( m_Lamda[iRes+1][m_allA[iRes+1][m][0]] + m_Lamda[iRes+1][m_allA[iRes+1][m][1]] )
          - 0.125* ( m_Lamda[iRes+1][m_allB[iRes+1][m][0]] + m_Lamda[iRes+1][m_allB[iRes+1][m][1]] )
          + 0.0625*( m_Lamda[iRes+1][m_allC[iRes+1][m][0]] + m_Lamda[iRes+1][m_allC[iRes+1][m][1]]
                     + m_Lamda[iRes+1][m_allC[iRes+1][m][2]] + m_Lamda[iRes+1][m_allC[iRes+1][m][3]] );
      } // for m

      // Analysis_Stage II
      lamda.assign( (m_Lamda[iRes+1]).begin(), (m_Lamda[iRes+1]).begin() + lM );
      for (unsigned long m = lM; m < lM1; ++m) {
        unsigned long kA0 = m_allA[iRes+1][m][0];
        lamda[kA0] += gamma[m]*m_allInteg[iRes+1][m]/(2*m_allInteg[iRes][kA0]);

        unsigned long kA1 = m_allA[iRes+1][m][1];
        lamda[kA1] += gamma[m]*m_allInteg[iRes+1][m]/(2*m_allInteg[iRes][kA1]);
      } // for m

      m_Lamda[iRes] = lamda;
      m_Gamma[iRes] = gamma;
    } // for iRes

//     std::cerr<<"SW transform done."<<std::endl;

    m_FinalGamma.resize(m_Resolution+1);
    m_FinalGamma[0] = m_Lamda[0];
    for (int i = 1; i<= m_Resolution; ++i) {
      m_FinalGamma[i] = m_Gamma[i-1];
    }

    m_Lamda.clear();
    m_Gamma.clear();

    SWtransformDone = true;
  }


  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::inverseSWaveletTransform()
  {
    if ( SWtransformDone == false ) {
      std::cerr<<"Forward transform has not been done."<<std::endl;
      exit(-1);
    }

    m_ReconstructedLamda.resize( m_Resolution+1 );
    m_ReconstructedLamda[0] = m_FinalGamma[0]; // It's the Lamda at the coarsest level

    for (int iRes = 0; iRes <= m_Resolution-1; ++iRes) {

      unsigned long lM = (m_allVerts[iRes]).size();
      unsigned long lM1 = (m_allVerts[iRes+1]).size();

      // Synthesis_Stage I      
      std::vector< double > lamda( m_ReconstructedLamda[iRes] ); // Scaling function coeffients.
      lamda.resize(lM1);

      for (unsigned long m = lM; m < lM1; ++m) {
        unsigned long kA0 = m_allA[iRes+1][m][0];
        lamda[kA0] -= m_FinalGamma[iRes+1][m]*m_allInteg[iRes+1][m]/(2*m_allInteg[iRes][kA0]);

        unsigned long kA1 = m_allA[iRes+1][m][1];
        lamda[kA1] -= m_FinalGamma[iRes+1][m]*m_allInteg[iRes+1][m]/(2*m_allInteg[iRes][kA1]);
      } // for m

      // Synthesis_Stage II
      for (unsigned long m = lM; m < lM1; ++m) {
        lamda[m] = m_FinalGamma[iRes+1][m] 
          + 0.5*   ( lamda[m_allA[iRes+1][m][0]] + lamda[m_allA[iRes+1][m][1]] )
          + 0.125* ( lamda[m_allB[iRes+1][m][0]] + lamda[m_allB[iRes+1][m][1]] )
          - 0.0625*( lamda[m_allC[iRes+1][m][0]] + lamda[m_allC[iRes+1][m][1]]
                     + lamda[m_allC[iRes+1][m][2]] + lamda[m_allC[iRes+1][m][3]] );
      } // for m

      m_ReconstructedLamda[iRes+1] = lamda;
    } // for iRes    
    
    inverseSWtransformDone = true;
//     std::cerr<<"Size of reconstructed Lamda."<<m_ReconstructedLamda.size()<<std::endl;
//     std::cerr<<"SW Backward transform done."<<std::endl;
  }

  template<class TOutputMesh>
  const std::vector< typename SWaveletSource<TOutputMesh>::PointType >&
  SWaveletSource<TOutputMesh>
  ::GetVerts( int level )
  {
    if ( level <= m_Resolution && level>=0 ) {
      return m_allVerts[level];
    }
    else {
      std::cerr<<"Request level exceeds boundary. Halt."<<std::endl;
      exit(-1);
    }
  }

  template<class TOutputMesh>
  const std::vector< typename SWaveletSource<TOutputMesh>::PointType >&
  SWaveletSource<TOutputMesh>
  ::GetVerts( void )
  {
    return m_allVerts[m_Resolution];
  }


  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::AddCell( OutputMeshType * mesh, const unsigned long * pointIds, unsigned long idx)
  {
    CellAutoPointer testCell;
    testCell.TakeOwnership( new TriCellType );
    testCell->SetPointIds(pointIds);
    mesh->SetCell(idx, testCell );
  }


  template<class TOutputMesh>
  void
  SWaveletSource<TOutputMesh>
  ::PrintSelf( std::ostream& os, Indent indent ) const
  {
    Superclass::PrintSelf(os,indent);

    os << indent << "Center: " << m_Center << std::endl;
    os << indent << "Scale: " << m_Scale << std::endl;
    os << indent << "Resolution: " << m_Resolution << std::endl;
  }


} //end of namespace itk


#endif

