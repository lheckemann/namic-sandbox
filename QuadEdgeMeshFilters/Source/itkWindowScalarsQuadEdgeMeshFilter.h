/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkWindowScalarsQuadEdgeMeshFilter.h,v $
  Language:  C++
  Date:      $Date: 2010-05-25 16:26:05 $
  Version:   $Revision: 1.0 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkWindowScalarsQuadEdgeMeshFilter_h
#define __itkWindowScalarsQuadEdgeMeshFilter_h

#include "itkQuadEdgeMeshToQuadEdgeMeshFilter.h"


namespace itk
{

/**
 * \class WindowScalarsQuadEdgeMeshFilter
 * \brief Applies a linear transformation to the scalar levels of the
 * input mesh that are inside a user-defined interval. Values below this
 * interval are mapped to a constant. Values over the interval are mapped 
 * to another constant. 
 * 
 *
 * Scalar of each point of the input mesh is set to be equal to be
 * m_WindowMinimum if it is lower than that value and to be m_WindowMaximum
 * if it is upper than that value to make the scalar range of the input 
 * mesh to be within [m_WindowMinimum, m_WindowMaximum].
 * It is required that m_WindowMinimum != m_WindowMaximum

 * And then the linear transformation is performed to the windowed scalars.
 *
 * The output mesh will have the same geometry as the input.
 *
 * \ingroup MeshFilters
 *
 */
template< class TInputMesh, class TOutputMesh  >
class WindowScalarsQuadEdgeMeshFilter :
  public QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh  >
{
public:
  typedef WindowScalarsQuadEdgeMeshFilter                                 Self;
  typedef QuadEdgeMeshToQuadEdgeMeshFilter< TInputMesh, TOutputMesh >     Superclass;
  typedef SmartPointer< Self >                                            Pointer;
  typedef SmartPointer< const Self >                                      ConstPointer;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( WindowScalarsQuadEdgeMeshFilter, QuadEdgeMeshToQuadEdgeMeshFilter );

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

  /** Get the minimum value of the input. */
  itkGetMacro( InputMinimum, InputPixelType );

  /** Get the maximum value of the input. */
  itkGetMacro( InputMaximum, InputPixelType );

  /** Set/Get the minimum value of the window. */
  itkSetMacro( WindowMinimum, InputPixelType );
  itkGetMacro( WindowMinimum, InputPixelType );

  /** Set/Get the maximum value of the window. */
  itkSetMacro( WindowMaximum, InputPixelType );
  itkGetMacro( WindowMaximum, InputPixelType );

  /** Set/Get the scalar value of the Output */
  itkSetMacro( OutputMinimum, OutputPixelType );
  itkGetMacro( OutputMinimum, OutputPixelType );

  /** Set/Get the scalar value of the Output */
  itkSetMacro( OutputMaximum, OutputPixelType );
  itkGetMacro( OutputMaximum, OutputPixelType );

  /** Set/Get the window width and level.  This is an alternative API
   * to using the SetWindowMinimum()/SetWindowMaximum(). The window
   * minimum and maximum are set as [level-window/2,
   * level+window/2]. */
  void SetWindowLevel(const InputPixelType& window,
                      const InputPixelType& level);
  InputPixelType GetWindow() const;
  InputPixelType GetLevel() const;

protected:
  WindowScalarsQuadEdgeMeshFilter();
  ~WindowScalarsQuadEdgeMeshFilter();

  void GenerateData();

private:

  WindowScalarsQuadEdgeMeshFilter( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  InputPixelType     m_InputMinimum;
  InputPixelType     m_InputMaximum;

  OutputPixelType    m_WindowMinimum;
  OutputPixelType    m_WindowMaximum;

  OutputPixelType    m_OutputMinimum;
  OutputPixelType    m_OutputMaximum;

  double m_Scale;
  double m_Shift;
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkWindowScalarsQuadEdgeMeshFilter.txx"
#endif

#endif
