/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkSWaveletSource.h,v $
  Language:  C++
  Date:      $Date: 2006/02/05 20:57:46 $
  Version:   $Revision: 1.5 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkSWaveletSource_h
#define __itkSWaveletSource_h

#include "itkMesh.h"
#include "itkMeshSource.h"
#include "itkTriangleCell.h"
#include "itkMapContainer.h"

#include <iostream>
#include <vector>
#include <map>
#include <utility>

namespace itk
{

/** \class SWaveletSource
 * \brief 
 * Inputs are the center of the mesh, the scale (radius in each dimension) of the mesh
 * and a resolution parameter, which corresponds to the recursion 
 * depth whlie creating a spherical triangle mesh.
 *
 * Don't use recursion depths larger than 5, because mesh generation gets very slow. 
 *
 * \author Thomas Boettger. Division Medical and Biological Informatics, German Cancer Research Center, Heidelberg.
 *
 */
template <class TOutputMesh>
class SWaveletSource : public MeshSource<TOutputMesh>
{
public:
  /** Standard "Self" typedef. */
  typedef SWaveletSource         Self;
  typedef itk::MeshSource<TOutputMesh>  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(SWaveletSource, MeshSource);

  /** Hold on to the type information specified by the template parameters. */
  typedef TOutputMesh OutputMeshType;
  typedef typename OutputMeshType::MeshTraits   MeshTraits;
  typedef typename OutputMeshType::PointType    PointType;
  typedef typename MeshTraits::PixelType        PixelType;  

  /** Some convenient typedefs. */
  typedef typename OutputMeshType::Pointer OutputMeshPointer;
  typedef typename OutputMeshType::CellTraits CellTraits;
  typedef typename OutputMeshType::PointsContainerPointer PointsContainerPointer;
  typedef typename OutputMeshType::PointsContainer   PointsContainer;
  
  /** Define the triangular cell types which form the surface  */
  typedef itk::CellInterface<PixelType, CellTraits>   CellInterfaceType;
  typedef itk::TriangleCell<CellInterfaceType>         TriCellType;
  typedef typename TriCellType::SelfAutoPointer       TriCellAutoPointer;
  typedef typename TriCellType::CellAutoPointer       CellAutoPointer;

  typedef std::pair<unsigned long,unsigned long> IndexPairType;
  typedef itk::MapContainer<IndexPairType, unsigned long> PointMapType;
  typedef typename PointType::VectorType VectorType;

  /** Set the resolution level to be used for generating cells in the Sphere.
   *  High values of this parameter will produce sphere with more triangles. */
  itkSetMacro(Resolution, unsigned int);
  itkGetMacro(Resolution, unsigned int);

  /** Set/Get Coordinates of the Sphere center. */
  itkSetMacro( Center, PointType  );
  itkGetMacro( Center, PointType  );

  /** Set/Get scales of the Sphere. This is a vector of values that can
   * actually be used for generating ellipsoids aligned with the coordinate
   * axis. */
  itkSetMacro( Scale,  VectorType );
  itkGetMacro( Scale,  VectorType );

  void SetScalarFunction( const std::vector< double >& vF );
  std::vector< double > GetScalarFunction( void );

  std::vector< double > GetReconstructedScalarFunction( void );
  std::vector< double > GetCoefficients( void );
      

  std::vector< std::vector< double > > GetGamma();
  std::vector< std::vector< double > > GetLamda();

  const std::vector< PointType >& GetVerts(int level);
  const std::vector< PointType >& GetVerts();

  void SWaveletTransform();
  void inverseSWaveletTransform();

protected:
  SWaveletSource();
  ~SWaveletSource() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  void GenerateData();

  PointType Divide( const PointType & p1, const PointType & p2) const;

  void AddCell( OutputMeshType * mesh, const unsigned long * pointIds, unsigned long idx);

  void setABC(unsigned long lnewPtIdx, const unsigned long& l1, const unsigned long& l2,
              std::vector< std::vector< unsigned long > > &A,
              std::vector< std::vector< unsigned long > > &B,
              std::vector< std::vector< unsigned long > > &C,
              const std::vector< std::vector< unsigned long > > &tris,
              const std::vector< std::vector< unsigned long > > &pointTris);

//   void setS( const std::vector< PointType > &verts; 
//              const std::vector< std::vector< unsigned long > > &tris,
//              const std::vector< std::vector< unsigned long > > &pointTris);
  void setInteg( void );

  void GenerateSWaveletStructure();


  /** model center */
  PointType m_Center; 

  /** models resolution */
  unsigned int m_Resolution;

  /** model scales */
  VectorType m_Scale;

  /** Scalar function difined on each node of the finest mesh */
  std::vector< double > m_ScalarFunction;


  std::vector< std::vector< double > > m_FinalGamma;
  // This variable stores all coeffients needed, the m_FinalGamma[0] is m_Lamda[0] while the rest a reall Gamma's.
  // Thus m_FinalGamma[0]=m_Lamda[0],  m_FinalGamma[1 ~ m_Resolution]=m_Gamma[0~(m_Resolution-1)]
  // In the paper Gamma[-1] = m_Lamda[0] but in coding, -1 can't be idx.

  std::vector< std::vector< double > > m_ReconstructedLamda;

  std::vector< std::vector< PointType > > m_allVerts;
  std::vector< std::vector< std::vector < unsigned long > > > m_allTriangles;
  std::vector< std::vector< std::vector < unsigned long > > > m_allEdges;  
  
  std::vector< std::vector< std::vector < unsigned long > > > m_allA;
  std::vector< std::vector< std::vector < unsigned long > > > m_allB;
  std::vector< std::vector< std::vector < unsigned long > > > m_allC;

  std::vector< std::vector< double > > m_allInteg;

  bool SWtransformDone;
  bool inverseSWtransformDone;

private:
  SWaveletSource(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};



} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSWaveletSource.txx"
#endif

#endif //_itkSWaveletSource_h
