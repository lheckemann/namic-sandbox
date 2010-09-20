/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkShiftScalarsQuadEdgeMeshFilter.h,v $
  Language:  C++
  Date:      $Date: 2010-09-17 16:26:05 $
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkShiftScalarsQuadEdgeMeshFilter_h
#define __itkShiftScalarsQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"


namespace itk
{

/**
 * \class ShiftScalarsQuadEdgeMeshFilter
 * \brief shifts and scales the scalar values of the input mesh
 *
 * Each point's scalar value is shifted by m_Shift and scaled by 
 * m_Scale. 
 * The default values of m_Shift and m_Scale are 0.0 and 1.0.
 *
 * If the scalar values are moved out of the range of OutputPixelType,
 * they are set to be NonpositiveMin and max of the pixel type.
 *
 * The output mesh will have the same geometry as the input.
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TOutputMesh  >
class ShiftScalarsQuadEdgeMeshFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
{
public:
  typedef ShiftScalarsQuadEdgeMeshFilter                                  Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >     Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( ShiftScalarsQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro( Self );

  typedef TInputMesh                                                InputMeshType;
  typedef typename InputMeshType::PixelType                         InputPixelType; 
  typedef typename InputMeshType::PointDataContainer                InputPointDataContainer;
  typedef typename InputMeshType::PointDataContainerConstPointer    InputPointDataContainerConstPointer;

  typedef TOutputMesh                                               OutputMeshType;
  typedef typename OutputMeshType::PixelType                        OutputPixelType; 
  typedef typename OutputMeshType::PointDataContainer               OutputPointDataContainer;
  typedef typename OutputMeshType::PointDataContainerPointer        OutputPointDataContainerPointer;

  /** Set/Get the mesh that will be processed. */
  void SetInputMesh ( const InputMeshType * mesh );
  const InputMeshType * GetInputMesh( void ) const;

  /** Set/Get the amount to Shift each Pixel. The shift is followed by a Scale. */
  itkSetMacro(Shift,double);
  itkGetConstMacro(Shift,double);

  /** Set/Get the amount to Scale each Pixel. The Scale is applied after the Shift. */
  itkSetMacro(Scale,double);
  itkGetConstMacro(Scale,double);

protected:
  ShiftScalarsQuadEdgeMeshFilter();
  ~ShiftScalarsQuadEdgeMeshFilter();

  void GenerateData();

private:

  ShiftScalarsQuadEdgeMeshFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  double m_Scale;
  double m_Shift;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkShiftScalarsQuadEdgeMeshFilter.txx"
#endif

#endif
