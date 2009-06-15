/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMarchingCubesImageToMeshFilter.h,v $
  Language:  C++
  Date:      $Date: 2008-10-10 19:02:15 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMarchingCubesImageToMeshFilter_h
#define __itkMarchingCubesImageToMeshFilter_h

#include "itkMesh.h"
#include "itkImageToMeshFilter.h"
#include "itkVector.h"
#include "itkCovariantVector.h"
#include "itkDefaultStaticMeshTraits.h"
#include "itkImageRegionConstIterator.h"
#include "itkThresholdSegmentationLevelSetImageFilter.h"
#include "itkCentralDifferenceImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkShapedNeighborhoodIterator.h"
//Sophie's includes
#include "itkMesh.h"
#include "itkTriangleCell.h"

namespace itk
{

/** \class MarchingCubesImageToMeshFilter
 *
 * \par This class tries to construct a 3D mesh surface based on 3D image and
 * surface value.  It can be used to integrate a region-based segmentation
 * method and a deformable model into one hybrid framework. 
 *
 * \par To construct a mesh, we need to construct elements in a voxel and
 * combine those elements later to form the final mesh. Before going through
 * every voxel in the 3D volume, we first determine those cells with a mixture
 * of voxel vertices both greater and less than  the surface value that
 * contribute to the 3D PointSet.
 *
 * \par These cells are subdivided into subcells by trilinear interpolation and a
 * gradient vector is computed at each node using central differences. The cube
 * vertices are tested to find those cubes with a mixture of vertex values both
 * greater and less than  the surface value The center coordiantes of these
 * cubes and corresponding interpolated normalized gradient vectors are an
 * element in the 3D mesh. The surface is defined by surface cube center
 * coordinates x, y, z,  and the surface normal components nx, ny, nz estimated
 * by trilinear interploation of the gradient vectors followed by
 * normalization. We then merge all these mesh elements into one 3D mesh.
 * 
 * \par PARAMETERS The SurfaceValue parameter is used to identify the object in
 * the 3D image. In most applications, pixels in the object region are greater
 * than the SurfaceValue ssigned to "1", so the default value of ObjectValue is
 * set to "1"
 *
 * This implementation of the algorithm is an improvement over the reference
 * paper below in the following aspects:
 *
 * 1) Surface points are computed accurately by using floating point math,
 *    as opposed to the integer math used in the reference.
 *
 * 2) The resample image filter is used instead of point by point interpolation
 *    to compute the subcell intensity values.
 *
 * \par REFERENCE
 * H. Cine, W. Lorensen, S. Ludke, B. Teeter, C. Crawford, "Two algorithms for
 * three-dimensional reconstruction of tomograms" Medical Physics 15 (3) 320
 * (1988) 
 * 
 * \par INPUT
 * The input should be a 3D image and surface value. 
 * 
 **/

template <class TInputImage, class TOutputPointSet>
class ITK_EXPORT MarchingCubesImageToMeshFilter : public ImageToMeshFilter< TInputImage, TOutputPointSet >
{
public:
  /** Standard "Self" typedef. */
  typedef MarchingCubesImageToMeshFilter       Self;
  typedef ImageToMeshFilter< 
    TInputImage, TOutputPointSet >             Superclass;
  typedef SmartPointer<Self>                   Pointer;
  typedef SmartPointer<const Self>             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Run-time type information (and related methods). */
  itkTypeMacro(MarchingCubesImageToMeshFilter, ImageToMeshFilter);

  /** Hold on to the type information specified by the template parameters. */
  typedef TOutputPointSet                             OutputMeshType;
  typedef typename OutputMeshType::MeshTraits         MeshTraits;
  typedef typename OutputMeshType::PointType          MeshPointType;
  typedef typename OutputMeshType::PointIdentifier    PointIdentifier;
  typedef typename MeshTraits::PixelType              MeshPixelType;  

  /** Some convenient typedefs. */
  typedef typename OutputMeshType::Pointer                OutputMeshPointer;
  typedef typename OutputMeshType::PointsContainer        PointsContainer;
  typedef typename OutputMeshType::PointDataContainer     PointDataContainer;
  typedef typename PointsContainer::Pointer               PointsContainerPointer;
  typedef typename PointDataContainer::Pointer            PointDataContainerPointer;
  
  /** Dimension of the input image */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension );

  /** Input Image Type Definition. */
  typedef TInputImage                                     InputImageType;
  typedef typename InputImageType::Pointer                InputImagePointer;
  typedef typename InputImageType::ConstPointer           InputImageConstPointer;
  typedef typename InputImageType::PixelType              InputPixelType;
  typedef typename InputImageType::SpacingType            SpacingType;
  typedef typename InputImageType::PointType              OriginType;
  typedef typename InputImageType::PointType              ImagePointType;
  typedef typename InputImageType::RegionType             RegionType;
  typedef typename InputImageType::SizeType               SizeType;
  typedef typename InputImageType::IndexType              IndexType;
  //SOPHIE'S CODE
  typedef typename InputImageType::PixelType              PixelTypeArray[8];
  typedef typename InputImageType::IndexType              IndexTypeArray[8];
  typedef typename itk::Mesh< InputPixelType, 3 >
                                                          MeshType;
  typedef typename MeshType::CellType                     CellType;
  typedef typename itk::TriangleCell< CellType >          TriangleType;
  //END SOPHIE'S CODE

  typedef ContinuousIndex<float,InputImageDimension>      ContinuousIndexType;

  typedef typename NumericTraits< 
    InputPixelType >::RealType                     InputPixelRealType;
     
  typedef ImageRegionConstIterator< InputImageType > InputImageIterator;
  
  /** Array of integers that define the resolution of the subdivision */
  typedef FixedArray< unsigned int, InputImageDimension >         SubdivideFactorArray;

  itkSetMacro( SurfaceValue, InputPixelRealType );

  /** accept the input image */
  virtual void SetInput( const InputImageType * inputImage );

  


protected:
  MarchingCubesImageToMeshFilter();
  ~MarchingCubesImageToMeshFilter();
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();
  virtual void GenerateOutputInformation(){}; // do nothing

  // Types related to the Neighborhood iterator.
  typedef NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> FaceCalculatorType;
  typedef typename FaceCalculatorType::FaceListType                           FaceListType;
  typedef typename FaceListType::iterator                                     FaceIterator;
  typedef ConstShapedNeighborhoodIterator<InputImageType>                     NeighborhoodIteratorType;

  // Typedef related to computation of gradients
  typedef CentralDifferenceImageFunction< TInputImage, double >      GradientCalculatorType;
  typedef typename GradientCalculatorType::Pointer                   GradientCalculatorPointer;
  typedef typename GradientCalculatorType::OutputType                CovariantVectorType;

private:
  MarchingCubesImageToMeshFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typedef typename InputImageType::SizeType                       InputImageSizeType;
  typedef ResampleImageFilter< InputImageType, InputImageType >   ResampleFilterType;
  typedef LinearInterpolateImageFunction< InputImageType >        InterpolatorType;
  typedef typename ResampleFilterType::Pointer                    ResampleFilterPointer;
  typedef typename InterpolatorType::Pointer                      InterpolatorPointer;
  
  InputPixelRealType                    m_SurfaceValue;
  std::vector< CovariantVectorType >    m_ListOfGradientsOnCell;
  GradientCalculatorPointer             m_GradientCalculator;
  ResampleFilterPointer                 m_ResampleFilter;
  InterpolatorPointer                   m_Interpolator;
  SubdivideFactorArray                  m_SubdivideFactors;
  PointIdentifier                       m_NumberOfPoints;
  PixelTypeArray                        m_PixelTypeArray;
  IndexTypeArray                        m_IndexTypeArray;
 
  // Type used for representing the index of the marching cubes table 
  typedef unsigned char                                              TableIndexType;

  mutable TableIndexType                m_TableIndex;

  class VertexPairType
    {
    public:
       
    VertexPairType()
      {
      Vertex1 = 0;
      Vertex2 = 0;
      }
     
    VertexPairType( unsigned int a, unsigned int b )
      {
      Vertex1 = a;
      Vertex2 = b;
      }
    unsigned int   Vertex1; 
    unsigned int   Vertex2; 
    };

   // There are 13 entries in the table, but only 12 represent real
   // edge pairs. This is done to be able to use "0" as the code for
   // "no-edge".
   VertexPairType                       m_EdgeIndexToVertexIndex[13];

   typedef unsigned int                 VertexTypeId;

   class ListOfTrianglesType
     {
     public:
     VertexTypeId Triangle[15];
     };

   ListOfTrianglesType                  m_CubeConfigurationCodeToListOfTriangle[256];



   class DirectedPointType
     {
     public:
       MeshPointType     point; 
       MeshPixelType     gradient;  // We assume that the Mesh pixel type is a CovariantVector.
     };

   void AddTriangleToOutputMesh( 
     const DirectedPointType & point1, 
     const DirectedPointType & point2, 
     const DirectedPointType & point3 );

  // Check whether the neighborhood is cut by the iso-hyper-surface
  bool IsSurfaceInside( const NeighborhoodIteratorType & walker );
 
  // Compute the gradients on each one of the voxels in a cell
  void ComputeCentralDifferences( const NeighborhoodIteratorType & cellRegionWalker );

  // Find intersection of the surface along the edge using linear interpolation.
  void InterpolateEdge( 
    const VertexPairType & vertexPair,
    const NeighborhoodIteratorType & cellRegionWalker,
    DirectedPointType & outputDirectedPoint );

  // Generate triangles for current configuration
  void GenerateTriangles( const NeighborhoodIteratorType & cellRegionWalker );


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMarchingCubesImageToMeshFilter.txx"
#endif

#endif
