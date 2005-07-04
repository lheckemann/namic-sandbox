/*=========================================================================

  Copyright (c) 2005 Andriy Fedorov, 
  College of William and Mary, VA and 
  Surgical Planning Lab, Harvard Medical School
  
=========================================================================*/

#ifndef _itkVolumeBoundaryCompressionMeshFilter_txx
#define _itkVolumeBoundaryCompressionMeshFilter_txx

#include "itkVolumeBoundaryCompressionMeshFilter.h"
#include "itkNumericTraits.h"

namespace itk
{

template<class TInputMesh, class TOutputMesh, class TInputImage>
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::VolumeBoundaryCompressionMeshFilter()
{
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::~VolumeBoundaryCompressionMeshFilter()
{
}

template<class TInputMesh,class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::SetInput(const InputMeshType* image)
{ 
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< InputMeshType * >( image ) );
}

template<class TInputMesh,class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::SetInput(const InputImageType* image)
{ 
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< InputImageType * >( image ) );
}

/** Generate the data */
template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::GenerateData()
{
}

/** Initialize the class fields */
template<class TInputMesh, class TOutputMesh, class TInputImage>
bool
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::Initialize()
{
}

/** PrintSelf */
template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  /*
  os << indent
     << "ObjectValue: " 
     << static_cast<NumericTraits<unsigned char>::PrintType>(m_ObjectValue)
     << std::endl;

  os << indent
     << "NumberOfNodes: "
     << m_NumberOfNodes
     << std::endl;

  os << indent
     << "NumberOfCells: "
     << m_NumberOfCells
     << std::endl;
     */
}

/* Returns the interpolated value of distance at the specified coordinate.
 * Will perform the inside test.
 */
template<class TInputMesh, class TOutputMesh, class TInputImage>
float
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh, TInputImage>
::DistanceAtPoint(double *coords){
  typename InterpolatorType::ContinuousIndexType input_index;
  float distance = 0;

  input_index[0] = coords[0];
  input_index[1] = coords[1];
  input_index[2] = coords[2];
  if(m_Interpolator->IsInsideBuffer(input_index))
    distance = (float)m_Interpolator->EvaluateAtContinuousIndex(input_index);
  else {
    std::cerr << "DistanceAtPoint(): Point [" << coords[0] << ", " << coords[1] << ", " 
      << coords[2] << "] is outside the image boundaries" << std::endl;
    assert(0);
  }
  return distance;
}

/* Computes the distance between two points in space */
template<class TInputMesh, class TOutputMesh, class TInputImage>
float
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::DistanceBwPoints(double *c0, double *c1){
  return sqrt((c0[0]-c1[0])*(c0[0]-c1[0])+(c0[1]-c1[1])*(c0[1]-c1[1])+
    (c0[2]-c1[2])*(c0[2]-c1[2]));
}

} /** end namespace itk. */

#endif

/*
template<class TInputImage, class TOutputMesh>
TYPE
VolumeBoundaryCompressionMeshFilter<TInputImage,TOutputMesh>
*/

