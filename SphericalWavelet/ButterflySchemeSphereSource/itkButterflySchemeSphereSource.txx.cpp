/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRegularSphereMeshSourceButterflyScheme.txx,v $
  Language:  C++
  Date:      $Date: 2006/02/05 20:57:46 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkButterflySchemeSphereSource_txx
#define __itkButterflySchemeSphereSource_txx

#include "itkButterflySchemeSphereSource.h"
#include "itkButterflyStructure.h"
#include <vector>


#ifndef M_PI
#define M_PI 3.1415926535897932
#endif


namespace itk {

template
<typename Type, 
unsigned int VDimension>
ButterflySchemeSphereSource<Type, VDimension>
::ButterflySchemeSphereSource()
{

  m_Center.Fill(0);
  m_Scale.Fill(1);
  m_Resolution = 2;
}


/*template
<typename Type, 
unsigned int VDimension = 3>
ButterflySchemeSphereSource<Type, VDimension>
::~ButterflySchemeSphereSource()
{
//destructor
}*/

template<typename Type, unsigned int VDimension>
void
ButterflySchemeSphereSource<Type, VDimension>
::SetSubdivisionLevel(int subdiv){
m_Resolution=subdiv;
}


/*template<typename Type, unsigned int VDimension>
typename MeshPointer
ButterflySchemeSphereSource<Type, VDimension>
::GetTemplateMesh(){
 
    MeshType::Pointer templateMesh = MeshType::New(); 
 return templateMesh;
}*/

template<typename Type, unsigned int VDimension>
void
ButterflySchemeSphereSource<Type, VDimension>
::GenerateData()
{
  unsigned long tripoints[3] = {0,1,2};
  typename MeshType::Pointer my_mesh = MeshType::New();  

  my_mesh->SetCellsAllocationMethod( MeshType::CellsAllocatedDynamicallyCellByCell );

  PointsContainerPointer  myPoints = my_mesh->GetPoints();
  const unsigned long X =.525731112119133606; 
  const unsigned long Z =.850650808352039932;
  PointType p1;
  unsigned long idx = 0;

  p1[0] = -X* m_Scale[0] + m_Center[0];
  p1[1] = 0 * m_Scale[1] + m_Center[1];
  p1[2] = Z * m_Scale[2] + m_Center[2];
  K[0]=idx;
  my_mesh->SetPoint(idx++, p1);
  

  p1[0] = X * m_Scale[0] + m_Center[0];
  p1[1] = 0 * m_Scale[1] + m_Center[1];
  p1[2] = Z * m_Scale[2] + m_Center[2];
  K[0][1]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = -X * m_Scale[0] + m_Center[0];
  p1[1] = 0 * m_Scale[1] + m_Center[1];
  p1[2] = -Z * m_Scale[2] + m_Center[2];
  K[0][2]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = X * m_Scale[0] + m_Center[0];
  p1[1] =  0 * m_Scale[1] + m_Center[1];
  p1[2] = -Z* m_Scale[2] + m_Center[2];
  K[0][3]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = 0* m_Scale[0] + m_Center[0];
  p1[1] = Z* m_Scale[1] + m_Center[1];
  p1[2] = X* m_Scale[2] + m_Center[2];
  K[0][4]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = 0* m_Scale[0] + m_Center[0];
  p1[1] = Z* m_Scale[1] + m_Center[1];
  p1[2] = -X* m_Scale[2] + m_Center[2];
  K[0][5]=idx;
  my_mesh->SetPoint(idx++, p1);
   
  p1[0] = 0* m_Scale[0] + m_Center[0];
  p1[1] = -Z* m_Scale[1] + m_Center[1];
  p1[2] = X* m_Scale[2] + m_Center[2];
  K[0][6]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = 0* m_Scale[0] + m_Center[0];
  p1[1] = -Z* m_Scale[1] + m_Center[1];
  p1[2] = -X* m_Scale[2] + m_Center[2];
  K[0][7]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = Z* m_Scale[0] + m_Center[0];
  p1[1] = X* m_Scale[1] + m_Center[1];
  p1[2] = 0* m_Scale[2] + m_Center[2];
  K[0][8]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] = -Z* m_Scale[0] + m_Center[0];
  p1[1] = X* m_Scale[1] + m_Center[1];
  p1[2] = 0* m_Scale[2] + m_Center[2];
  K[0][9]=idx;
  my_mesh->SetPoint(idx++, p1);

  p1[0] = Z* m_Scale[0] + m_Center[0];
  p1[1] = -X* m_Scale[1] + m_Center[1];
  p1[2] = 0* m_Scale[2] + m_Center[2];
  K[0][10]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  p1[0] =-Z* m_Scale[0] + m_Center[0];
  p1[1] = -X* m_Scale[1] + m_Center[1];
  p1[2] = 0* m_Scale[2] + m_Center[2];
  K[0][11]=idx;
  my_mesh->SetPoint(idx++, p1);
  
  static unsigned long triang[20][3] = 
  {
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
  };

  
 for (unsigned long i=0; i<20; i++)
 {
  tripoints[0] = triang[i][1]; tripoints[1] =triang[i][2]; tripoints[2] = triang[i][3]; 
  this->AddCell(my_mesh, tripoints, i );
 }
  
 for (unsigned int i = 0; i < m_Resolution; i++) 
    {

 std::vector< std::vector<unsigned long> > m_pointL;
  std::vector< std::vector<unsigned long> >  m_cellP;

    MeshType::CellsContainerPointer myCells = my_mesh->GetCells();
    MeshType::CellsContainer::Iterator cells = myCells->Begin();
    //MeshType::CellsContainer::Iterator cellEnd = myCells->End();

    MeshType::Pointer result = MeshType::New(); // new mesh into which we are going to transfer the "old" points and to which we are going to add the "new" ones
    PointType v1, v2, v3;
    PointType* v1_pt;
    PointType* v2_pt;
    PointType* v3_pt;
    v1_pt = &v1;  v2_pt = &v2;  v3_pt = &v3;
    const unsigned long *tp;
    unsigned long pointIdx;
 unsigned long cellIdx;
    unsigned long MpointId=0;
    unsigned long pointIdxOffset = my_mesh->GetNumberOfPoints();
 int numOfCells = my_mesh->GetNumberOfCells();
    pointIdx = pointIdxOffset;
    unsigned long newIdx[3] = {0,1,2};
  
    PointMapType::Pointer handledEdges = PointMapType::New();

//triangles/ points assignements/////////////////////////////////////
 

  int numOfPoints = my_mesh->GetNumberOfPoints();

  

 for ( int itCell = 0;
          itCell < numOfCells;
          ++itCell, ++cells)
    {
      CellType * cellptr = cells.Value(); //cellptr will point to each cell in the mesh
      
      PointIdIterator pntIdIter = cellptr->PointIdsBegin();
      //pntIdIter will point to each point in the current cell
      PointIdIterator pntIdEnd = cellptr->PointIdsEnd();
       
 
      for (; pntIdIter != pntIdEnd; ++pntIdIter)
        {
          m_pointL[*pntIdIter ].push_back(itCell);
    m_cellP[itCell].push_back(*pntIdIter);
        } 
    }

 
////////////////////////////////////////////////////////////////////////////////////
 //go thru all the cells 
    while( cells != myCells->End() ) 
      {
      tp = cells.Value()->GetPointIds();
      my_mesh->GetPoint(tp[0],v1_pt);
      my_mesh->GetPoint(tp[1],v2_pt);
      my_mesh->GetPoint(tp[2],v3_pt);
//copying the points of previous resolution onto the new mesh (resol +1)
      result->SetPoint(tp[0], v1);
      result->SetPoint(tp[1], v2);
      result->SetPoint(tp[2], v3);
   }
   
//point 1
   

      if (!handledEdges->IndexExists(std::make_pair(tp[0], tp[1])) &&
              !handledEdges->IndexExists(std::make_pair(tp[1], tp[0])))
        {
        newIdx[0]=pointIdx;
  M[i][MpointId]=pointIdx;
        MpointId++;
        handledEdges->InsertElement(std::make_pair(tp[0], tp[1]), pointIdx);
        result->SetPoint(pointIdx++, this->Divide(v1,v2) );
 
  //// pixel Structure creation and population

        PixelType BTS1;
  BTS1.generateButterflyPoints(tp[0],tp[1],tp[2],m_pointL,m_cellP);
  result->SetPointData(pointIdx, BTS1);

        }
      else
        {
        if (handledEdges->IndexExists(std::make_pair(tp[0], tp[1]))) 
          {
          newIdx[0] = handledEdges->GetElement(std::make_pair(tp[0], tp[1]));
          }
        else 
          {
          newIdx[0] = handledEdges->GetElement(std::make_pair(tp[1], tp[0]));
          }
        }


      // point 2
      if (!handledEdges->IndexExists(std::make_pair(tp[1], tp[2])) &&
              !handledEdges->IndexExists(std::make_pair(tp[2], tp[1])))
        {
        newIdx[1] = pointIdx;
  M[i][MpointId]=pointIdx;
        MpointId++;
        handledEdges->InsertElement(std::make_pair(tp[1], tp[2]), pointIdx);
        result->SetPoint(pointIdx++, this->Divide(v2,v3));
        
  PixelType BTS2;
  BTS2.generateButterflyPoints(tp[1],tp[2],tp[0],m_pointL,m_cellP);
  result->SetPointData(pointIdx, BTS2);
  
  }
      else
        {
        if (handledEdges->IndexExists(std::make_pair(tp[1], tp[2]))) 
          {
          newIdx[1] = handledEdges->GetElement(std::make_pair(tp[1], tp[2]));
          }
        else 
          {
          newIdx[1] = handledEdges->GetElement(std::make_pair(tp[2], tp[1]));
          }
        }


      // point 3
      if (!handledEdges->IndexExists(std::make_pair(tp[2], tp[0])) &&
              !handledEdges->IndexExists(std::make_pair(tp[0], tp[2])))
        {
        newIdx[2] = pointIdx;
  M[i][MpointId]=pointIdx;
        MpointId++;
        handledEdges->InsertElement(std::make_pair(tp[2], tp[0]), pointIdx);
        result->SetPoint(pointIdx++, this->Divide(v3,v1));
  
  PixelType BTS3;
  BTS3.generateButterflyPoints(tp[2],tp[0],tp[1],m_pointL,m_cellP);
  result->SetPointData(pointIdx, BTS3);
        }
      else
        {
        if (handledEdges->IndexExists(std::make_pair(tp[2], tp[0]))) 
          {
          newIdx[2] = handledEdges->GetElement(std::make_pair(tp[2], tp[0]));
          }
        else 
          {
          newIdx[2] = handledEdges->GetElement(std::make_pair(tp[0], tp[2]));
          }
        }

      tripoints[0] = tp[0]; 
      tripoints[1] = newIdx[0]; 
      tripoints[2] = newIdx[2]; 
      this->AddCell(result,tripoints,cellIdx);
      cellIdx++;

      tripoints[0] = newIdx[0];
      tripoints[1] = tp[1];
      tripoints[2] = newIdx[1];
      this->AddCell(result,tripoints,cellIdx);
      cellIdx++;

      tripoints[0] = newIdx[1];
      tripoints[1] = tp[2];
      tripoints[2] = newIdx[2]; 
      this->AddCell(result,tripoints,cellIdx);
      cellIdx++;

      tripoints[0] = newIdx[0];
      tripoints[1] = newIdx[1]; 
      tripoints[2] = newIdx[2]; 
      this->AddCell(result,tripoints,cellIdx);
      cellIdx++;
      cells++;
    
    // This call does not release memory as ref count is not 1
    //outputMesh->Initialize();
    // Release memory
    cells = myCells->Begin();
    while( cells != myCells->End() )
      {
      const CellInterfaceType * cellToBeDeleted = cells->Value();
      delete cellToBeDeleted;
      cells++;
      }
    my_mesh->SetPoints(result->GetPoints());
    my_mesh->SetCells(result->GetCells());
    my_mesh->SetCellData(result->GetCellData());
    


}
}

template<typename Type, unsigned int VDimension>
typename ButterflySchemeSphereSource <Type, VDimension>::PointType
ButterflySchemeSphereSource <Type, VDimension>
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



template<typename Type, unsigned int VDimension>
void
ButterflySchemeSphereSource <Type, VDimension>
::AddCell( MeshType* mesh, const unsigned long * pointIds, unsigned long idx)
  {
    CellAutoPointer testCell;
    testCell.TakeOwnership( new TriCellType );
    testCell->SetPointIds(pointIds);
    mesh->SetCell(idx, testCell);
  }

/*
template<typename Type, typename Dim>
void
ButterflySchemeSphereSource <typename Type, typename Dim>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Center: " << m_Center << std::endl;
  os << indent << "Scale: " << m_Scale << std::endl;
  os << indent << "Resolution: " << m_Resolution << std::endl;
}

template<typename Type, typename Dim>
std::vector<int> ButterflySchemeSphereSource<typename Type, typename Dim>::VectorIntersection(std::vector<int> v1, std::vector<int> v2){
    
 int nb=0;
 std::vector<int>::iterator vi;
 std::vector <int>::size_type i;
    i = v1.size( );
    for (k=0; k<i ; k++){
    if v1[k]=v2[k]

}
 */
  /*template<typename Type, unsigned int VDimension>
  void
  ButterflySchemeSphereSource <Type, VDimension>
  ::SetPointDataS(PointDataContainer* pointData, MeshType * mesh_)
{
  itkDebugMacro("setting PointData container to " << pointData);
  if(m_PointDataContainer != pointData)
    {
    m_PointDataContainer = pointData;
    mesh_->Modified();
    }
}*/

/*
template<typename Type, unsigned int VDimension>
void
ButterflySchemeSphereSource <Type, VDimension>
::SetPointDataS(PointIdentifier ptId, PixelType data, MeshType * mesh_)
{
  
  if( !m_PointDataContainer )
    {
    mesh_->SetPointData(PointDataContainer::New());
    }

  /**
   * Insert the point data into the container with the given identifier.
  
  m_PointDataContainer->InsertElement(ptId, data);
}
*/



} //end of namespace itk


#endif


