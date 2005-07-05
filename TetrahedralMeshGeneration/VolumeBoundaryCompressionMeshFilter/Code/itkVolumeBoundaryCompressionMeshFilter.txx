/*=========================================================================

  Copyright (c) 2005 Andriy Fedorov, 
  College of William and Mary, VA and 
  Surgical Planning Lab, Harvard Medical School
  
=========================================================================*/

#ifndef _itkVolumeBoundaryCompressionMeshFilter_txx
#define _itkVolumeBoundaryCompressionMeshFilter_txx

#include "itkVolumeBoundaryCompressionMeshFilter.h"
#include "itkNumericTraits.h"
#include "itkCellInterface.h"
#include <set>

namespace itk
{

template<class TInputMesh, class TOutputMesh, class TInputImage>
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::VolumeBoundaryCompressionMeshFilter()
{
  this->SetNumberOfRequiredInputs(2);
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
  this->ProcessObject::SetNthInput(1, 
                                   const_cast< InputImageType * >( image ) );
}

/** Generate the data */
template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::GenerateData()
{

  Initialize();

  // at the end, transfer the connectivity and other unmodified data from the
  // input to the output mesh (copied from TransformMeshFilter)
  m_OutputMesh->SetPointData(m_InputMesh->GetPointData());
  m_OutputMesh->SetCellLinks(m_InputMesh->GetCellLinks());
  m_OutputMesh->SetCells(m_InputMesh->GetCells());
  m_OutputMesh->SetCellData(m_InputMesh->GetCellData());
  for(unsigned int dim=0;dim<3;dim++) // only 3D is handled
    m_OutputMesh->SetBoundaryAssignments(dim,
      m_InputMesh->GetBoundaryAssignments(dim));
}

/** Initialize the class fields */
template<class TInputMesh, class TOutputMesh, class TInputImage>
bool
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::Initialize()
{

  // TODO: make sure all branches are tested (when no prefix is provided)
  std::cout << "Initializing the compression filter" << std::endl;

  if(this->GetNumberOfInputs() < 2){
    std::cout << "VolumeBoundaryCompressionMeshFilter : Binary mask and/or input mesh not set" 
      << std::endl;
    return false;
  }

  InputImagePointer m_InputImage =
    static_cast<const InputImageType*>(this->ProcessObject::GetInput(1));

  // Compute the distance image
  //  1. Cast the input image to the internal format
  
  bool read_failed = false;
  if(m_InputImagePrefix.size()){
    // try to read the distance image
    InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
    reader->SetFileName((std::string("/tmp/")+m_InputImagePrefix+"Resampled.mha").c_str());
    try{
      reader->Update();
      this->m_InputImage = reader->GetOutput();
    }catch(ExceptionObject &e){
      // oh well, have to regenerate
      read_failed = true;
    }
  }

  if(!m_InputImagePrefix.size() || read_failed){
    if(m_InputImagePrefix.size()){
      typename CastFilterType::Pointer cast_filter =
        CastFilterType::New();
      cast_filter->SetInput(m_InputImage);
      cast_filter->Update();
      std::cout << "Image casted" << std::endl;

      //  2. Resample the input image to unit sized voxels
      typename ResamplerType::Pointer resampler = 
        ResamplerType::New();
      IdentityTransformType::Pointer transform =
        IdentityTransformType::New();
      typename InputImageType::SpacingType input_spacing =
        m_InputImage->GetSpacing();
      typename InputImageType::SpacingType output_spacing;
      typename InputImageType::SizeType input_size =
        m_InputImage->GetLargestPossibleRegion().GetSize();
      typename InputImageType::SizeType output_size;
      typedef NearestNeighborInterpolateImageFunction<InternalImageType,double>
        ResampleInterpolatorType;
      typename ResampleInterpolatorType::Pointer resample_interpolator = 
        ResampleInterpolatorType::New();
      //  typename InputImageType::SizeType::SizeValueType InputSizeValueType;


      output_spacing[0] = 1.0;
      output_spacing[1] = 1.0;
      output_spacing[2] = 1.0;

      output_size[0] = static_cast<typename InputImageType::SizeType::SizeValueType>
        (ceil((double)input_size[0]*input_spacing[0]));
      output_size[1] = static_cast<typename InputImageType::SizeType::SizeValueType>
        (ceil((double)input_size[1]*input_spacing[1]));
      output_size[2] = static_cast<typename InputImageType::SizeType::SizeValueType>
        (ceil((double)input_size[2]*input_spacing[2]));

      transform->SetIdentity();
      resampler->SetTransform(transform);
      resampler->SetInterpolator(resample_interpolator);
      resampler->SetOutputSpacing(output_spacing);
      resampler->SetOutputOrigin(m_InputImage->GetOrigin());
      resampler->SetSize(output_size);
      resampler->SetInput(cast_filter->GetOutput());
      resampler->Update();
      std::cout << "Image resampled" << std::endl;

      this->m_InputImage = resampler->GetOutput();

      InternalImageWriterType::Pointer writer = 
        InternalImageWriterType::New();
      writer->SetFileName((std::string("/tmp/")+m_InputImagePrefix+"Resampled.mha").c_str());
      writer->SetInput(this->m_InputImage);
      try{
        writer->Update();
      }catch(ExceptionObject &eo){
        // this should not disrupt the overall execution
      }
    }
  }
  
  typename InputImageType::SizeType input_size =
    this->m_InputImage->GetLargestPossibleRegion().GetSize();
  
  this->m_dimX = input_size[0];
  this->m_dimY = input_size[1];
  this->m_dimZ = input_size[2];

  m_InputOrigin = m_InputImage->GetOrigin();

  //  3. Compute the distance transform on the resampled image
  // Distance and RGF are the slowest computation components. If the user
  // specifies filename, results of these filters will be saved, and before
  // that we will try to read the previously saved results instead of
  // regenerating the data
  //
  read_failed = false;
  if(m_InputImagePrefix.size()){
    // try to read the distance image
    InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
    reader->SetFileName((std::string("/tmp/")+m_InputImagePrefix+"Dist.mha").c_str());
    try{
      reader->Update();
      m_DistanceImage = reader->GetOutput();
    }catch(ExceptionObject &e){
      // oh well, have to regenerate
      read_failed = true;
    }
  }
  if(!m_InputImagePrefix.size() || read_failed){
    DistanceFilterType::Pointer distance_filter =
      DistanceFilterType::New();
    distance_filter->SetInput(this->m_InputImage);
    distance_filter->Update();
    m_DistanceImage = distance_filter->GetOutput();
    std::cout << "Distance computed" << std::endl;
    if(m_InputImagePrefix.size()){
      InternalImageWriterType::Pointer writer = 
        InternalImageWriterType::New();
      writer->SetFileName((std::string("/tmp/")+m_InputImagePrefix+"Dist.mha").c_str());
      writer->SetInput(m_DistanceImage);
      try{
        writer->Update();
      }catch(ExceptionObject &eo){
        // this should not disrupt the overall execution
      }
    }
  }

  // Mesh input/output initialization

  this->m_InputMesh = 
    static_cast<const InputMeshType*>(this->ProcessObject::GetInput(0));
  this->m_OutputMesh = this->GetOutput();
  
  if(!m_InputMesh)
    itkExceptionMacro(<<"Missing input mesh");
  if(!m_OutputMesh)
    itkExceptionMacro(<<"Missing output mesh");

  m_OutputMesh->SetBufferedRegion(m_OutputMesh->GetRequestedRegion());
  
  InputPointsContainerPointer inPoints = m_InputMesh->GetPoints();
  OutputPointsContainerPointer outPoints = m_OutputMesh->GetPoints();
  outPoints->Reserve(m_InputMesh->GetNumberOfPoints());
  outPoints->Squeeze();
  
  // Walk through the input mesh, extract surface vertices
  typename std::map<TetFace,unsigned int> face2cnt;
  typename std::map<TetFace,unsigned int>::iterator face2cntI;
  unsigned int i;
  const unsigned int tet_face_LUT[12] = 
    { 0, 1, 2,
      0, 1, 3,
      0, 2, 3,
      1, 2, 3 };
  InputCellsContainerIterator cellIterator;
  
  cellIterator = m_InputMesh->GetCells()->Begin();
  
  while(cellIterator != m_InputMesh->GetCells()->End()){
    ICellType *curCell;
    InputTetrahedronType *curTet;
    
    curCell = cellIterator.Value();
    if(curCell->GetType() != ICellType::TETRAHEDRON_CELL)
      itkExceptionMacro(<<"Input mesh should be tetrahedral");
    curTet = dynamic_cast<InputTetrahedronType *>(curCell);

    typename InputTetrahedronType::PointIdIterator ptI = curTet->PointIdsBegin();
    unsigned int face_points[4];
    for(i=0;i<4;i++)
      face_points[i] = *ptI++;
    for(i=0;i<4;i++){
      TetFace thisFace(face_points[tet_face_LUT[i*3]], 
        face_points[tet_face_LUT[i*3+1]],
        face_points[tet_face_LUT[i*3+2]]);
      face2cntI = face2cnt.find(thisFace);
      if(face2cntI != face2cnt.end())
        face2cnt[thisFace]++;
      else
        face2cnt[thisFace] = 1;
    }
    cellIterator++;
  }

  std::set<unsigned int> surfaceVerticesSet;
  for(face2cntI=face2cnt.begin();face2cntI!=face2cnt.end();
    face2cntI++){
    unsigned int j;
    switch((*face2cntI).second){
    case 1:{
      TetFace thisFace = (*face2cntI).first;
      for(j=0;j<3;j++)
        surfaceVerticesSet.insert(thisFace.nodes[j]);
      break;}
    case 2: break;
    case 0:
    default:assert(0);
    }
  }
  
  i = 0;
  m_SurfaceVertices.resize(surfaceVerticesSet.size());
  for(std::set<unsigned int>::iterator vsI=surfaceVerticesSet.begin();
    vsI!=surfaceVerticesSet.end();vsI++,i++)
    m_SurfaceVertices[i] = *vsI;
  
  std::cout << m_SurfaceVertices.size() << " surface vertices found" << std::endl;
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

