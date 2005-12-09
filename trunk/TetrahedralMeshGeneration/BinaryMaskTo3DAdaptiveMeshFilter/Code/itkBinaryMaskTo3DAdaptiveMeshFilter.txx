/*=========================================================================

  Copyright (c) 2005 Andriy Fedorov, 
  College of William and Mary, VA and 
  Surgical Planning Lab, Harvard Medical School
  
  Adaptive 3D Tetrahedral mesh generation filter from binary mask. Algorithm 
  is described in the PhD thesis of Neil Molino (Stanford University, 2004). 
  Numerical part has been implemented by Aloys d'Aiasche. 
=========================================================================*/

#ifndef _itkBinaryMaskTo3DAdaptiveMeshFilter_txx
#define _itkBinaryMaskTo3DAdaptiveMeshFilter_txx

#include "itkBinaryMaskTo3DAdaptiveMeshFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkNumericTraits.h"

namespace itk
{

template<class TInputImage, class TOutputMesh>
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::BinaryMaskTo3DAdaptiveMeshFilter()
{
  // Modify superclass default values, can be overridden by subclasses
  this->SetNumberOfRequiredInputs(1);
  m_SubdivInclusion = true;
  m_SubdivCurvature = true;
  m_KeepOutside = false;
  m_NResolutions = 1;
  m_NumberOfPoints = 0;
  m_NumberOfTets = 0;
  m_BCCSpacing = 10;
  m_TmpDirectory = "";
  m_InputImagePrefix = "";

  m_ThirdFaceEdgeLT[0] = -1;
  m_ThirdFaceEdgeLT[1] = -1;
  m_ThirdFaceEdgeLT[2] = 5;
  m_ThirdFaceEdgeLT[3] = 4;
  m_ThirdFaceEdgeLT[4] = 3;
  m_ThirdFaceEdgeLT[5] = 2;
  m_ThirdFaceEdgeLT[6] = -1;
  m_ThirdFaceEdgeLT[7] = -1;
  m_ThirdFaceEdgeLT[8] = 4;
  m_ThirdFaceEdgeLT[9] = 5;
  m_ThirdFaceEdgeLT[10] = 2;
  m_ThirdFaceEdgeLT[11] = 3;
  m_ThirdFaceEdgeLT[12] = 5;
  m_ThirdFaceEdgeLT[13] = 4;
  m_ThirdFaceEdgeLT[14] = -1;
  m_ThirdFaceEdgeLT[15] = -1;
  m_ThirdFaceEdgeLT[16] = 1;
  m_ThirdFaceEdgeLT[17] = 0;
  m_ThirdFaceEdgeLT[18] = 4;
  m_ThirdFaceEdgeLT[19] = 5;
  m_ThirdFaceEdgeLT[20] = -1;
  m_ThirdFaceEdgeLT[21] = -1;
  m_ThirdFaceEdgeLT[22] = 0;
  m_ThirdFaceEdgeLT[23] = 1;
  m_ThirdFaceEdgeLT[24] = 3;
  m_ThirdFaceEdgeLT[25] = 2;
  m_ThirdFaceEdgeLT[26] = 1;
  m_ThirdFaceEdgeLT[27] = 0;
  m_ThirdFaceEdgeLT[28] = -1;
  m_ThirdFaceEdgeLT[29] = -1;
  m_ThirdFaceEdgeLT[30] = 2;
  m_ThirdFaceEdgeLT[31] = 3;
  m_ThirdFaceEdgeLT[32] = 0;
  m_ThirdFaceEdgeLT[33] = 1;
  m_ThirdFaceEdgeLT[34] = -1;
  m_ThirdFaceEdgeLT[35] = -1;

  m_FaceToEdgesLT[0] = 0;
  m_FaceToEdgesLT[1] = 2;
  m_FaceToEdgesLT[2] = 5;
  m_FaceToEdgesLT[3] = 2;
  m_FaceToEdgesLT[4] = 4;
  m_FaceToEdgesLT[5] = 1;
  m_FaceToEdgesLT[6] = 4;
  m_FaceToEdgesLT[7] = 0;
  m_FaceToEdgesLT[8] = 3;
  m_FaceToEdgesLT[9] = 3;
  m_FaceToEdgesLT[10] = 5;
  m_FaceToEdgesLT[11] = 1;

  m_IncidentEdgesLT[0] = 2;
  m_IncidentEdgesLT[1] = 4;
  m_IncidentEdgesLT[2] = 2;
  m_IncidentEdgesLT[3] = 5;
  m_IncidentEdgesLT[4] = 4;
  m_IncidentEdgesLT[5] = 0;
  m_IncidentEdgesLT[6] = 4;
  m_IncidentEdgesLT[7] = 1;
  m_IncidentEdgesLT[8] = 0;
  m_IncidentEdgesLT[9] = 2;
  m_IncidentEdgesLT[10] = 0;
  m_IncidentEdgesLT[11] = 3;
  m_IncidentEdgesLT[12] = 3;
  m_IncidentEdgesLT[13] = 5;
  m_IncidentEdgesLT[14] = 3;
  m_IncidentEdgesLT[15] = 4;
  m_IncidentEdgesLT[16] = 5;
  m_IncidentEdgesLT[17] = 1;
  m_IncidentEdgesLT[18] = 5;
  m_IncidentEdgesLT[19] = 0;
  m_IncidentEdgesLT[20] = 1;
  m_IncidentEdgesLT[21] = 3;
  m_IncidentEdgesLT[22] = 1;
  m_IncidentEdgesLT[23] = 2;
}

template<class TInputImage, class TOutputMesh>
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::~BinaryMaskTo3DAdaptiveMeshFilter()
{
}

  
template<class TInputImage,class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::SetInput(const InputImageType* image)
{ 
  this->ProcessObject::SetNthInput(0, 
                                   const_cast< InputImageType * >( image ) );
}

/** Generate the data */
template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::GenerateData()
{
  unsigned i, j;//, CurrentRes = 0;
  this->Initialize();

  this->CreateMesh();


  // Do the refinement using the specified subdivision 
  // criteria and the number of resolutions
  m_CurrentResol = 1;
  while(m_CurrentResol<m_NResolutions){
    
    // --> DEBUG
    for(typename std::list<RGMTetra_ptr>::iterator tI=m_Tetras.begin();
      tI!=m_Tetras.end();tI++){
      unsigned char conf = GetTetraEdgeConf(*tI);
    }
    // <-- DEBUG

    std::cout << "Creating resolution " << m_CurrentResol << std::endl;
    std::cout << "Input: " << m_Tetras.size() << " tets" << std::endl;
    // Copy all tetrahedra to a temporary list
    std::list<RGMTetra_ptr> prev_level_tetras;
    std::insert_iterator<std::list<RGMTetra_ptr> >
      pltI(prev_level_tetras, prev_level_tetras.begin());
    copy(m_Tetras.begin(), m_Tetras.end(), pltI);
    m_Tetras.clear();
    
    // Subdivide RED tetras (based on user criteria)
    bool new_edges_split = false;
    unsigned red_tetras_cnt = 0;
    while(prev_level_tetras.size()){
      RGMTetra_ptr curT;

      curT = prev_level_tetras.front();
      prev_level_tetras.pop_front();
      

      if(TetraUnderrefined(curT)){
        if(curT->parent || curT->level!=m_CurrentResol-1){
          m_PendingTetras.push_back(curT);
          continue;
        }
        new_edges_split = true;
        red_tetras_cnt++;
        FinalizeRedTetra(curT);
      } else {
        m_PendingTetras.push_back(curT);
      }
    } // while(prev_level_size)

    std::cout << red_tetras_cnt << " tets were Red-subdivided, "  << std::endl;
    // --> DEBUG
    for(typename std::list<RGMTetra_ptr>::iterator tI=m_PendingTetras.begin();
      tI!=m_PendingTetras.end();tI++){
      unsigned char conf = GetTetraEdgeConf(*tI);
    }
    // <-- DEBUG

    // Enforce conformancy of the tetras neighbouring to 
    // the Red-subdivided ones. Subdivisions may incur new ones, that's why
    // there are two loops. TODO(?): bound the complexity of Red-Green
    // subdivision procedure.
    while(new_edges_split){
      new_edges_split = false;
      std::cout << m_PendingTetras.size() << " tets are pending" << std::endl;
      assert(prev_level_tetras.empty());
      std::insert_iterator<std::list<RGMTetra_ptr> >
        pltI(prev_level_tetras, prev_level_tetras.begin());
      copy(m_PendingTetras.begin(), m_PendingTetras.end(), pltI);
      m_PendingTetras.clear();
      
      while(prev_level_tetras.size()){//prev_level_size--){
        RGMTetra_ptr curT;
        unsigned split_edges_ids[6], split_edges_total;
        RGMEdge_ptr split_edges[6];
        
        curT = prev_level_tetras.front();
        prev_level_tetras.pop_front();

        split_edges_total = GetTetraEdgeConf(curT);
        
        if(curT->parent){
          // this tetra is Green: cannot subdivide, should split the parent to
          // make it Red
          // 1: disconnect all the siblings
          RGMTetra_ptr curT_parent;
          unsigned parent_edges_split, parent_child_cnt;
          unsigned child_edges_split;
          
          curT_parent = curT->parent;
          parent_edges_split = 0;
          parent_child_cnt = 0;

          for(i=0;i<6;i++)
            if(curT_parent->edges[i]->midv)
              parent_edges_split++;
          
          parent_child_cnt = curT_parent->num_greens;
          
          child_edges_split = 0;
          for(i=0;i<parent_child_cnt;i++){
            if(&(curT_parent->greens[i])!=curT){
              typename std::list<RGMTetra_ptr>::iterator gtlI =
                find(prev_level_tetras.begin(), 
                  prev_level_tetras.end(),
                  &(curT_parent->greens[i]));
              if(gtlI==prev_level_tetras.end())
                assert(0);
              prev_level_tetras.erase(gtlI);
            }
            child_edges_split += GetTetraEdgeConf(&(curT_parent->greens[i]));
          }
          
          if(child_edges_split){
            if(curT_parent->num_greens == 4){
              for(i=1;i<=5;i+=2){
                unsigned edge_id = i;
                RGMVertex_ptr v0, v1;
                
                v0 = (curT_parent->greens[3]).edges[edge_id]->nodes[0];
                v1 = (curT_parent->greens[3]).edges[edge_id]->nodes[1];
                m_TmpEdgeMap[std::pair<RGMVertex_ptr,RGMVertex_ptr>(v0,v1)] =
                  (curT_parent->greens[3]).edges[i];
              } // for(each edge of ...)
            } // if(curT_parent->num_greens == 4)

            for(i=0;i<parent_child_cnt;i++)
              RemoveTetraAllocated(&(curT_parent->greens[i]));
            delete [] curT_parent->greens;
            curT_parent->greens = NULL;
            FinalizeRedTetra(curT_parent);
            new_edges_split = true;
          } // if(child_edges_split)
          else {
            for(i=0;i<parent_child_cnt;i++)
              m_PendingTetras.push_back(&(curT_parent->greens[i]));
          } // if(child_edges_split) else
          continue; // ???
        } // if(curT->parent)
        split_edges_total = 0;
        for(i=0;i<6;i++)
          if(curT->edges[i]->midv)
            split_edges_ids[split_edges_total++] = i;
        
        switch(split_edges_total){
        case 0:
        case 1:
          m_PendingTetras.push_back(curT);
          break;
        case 2:
          if(split_edges_ids[0] != (split_edges_ids[1]^1)){
            SplitEdge(curT->edges[m_ThirdFaceEdgeLT[split_edges_ids[0]*6+split_edges_ids[1]]]);
            new_edges_split = true;
          }
          m_PendingTetras.push_back(curT);
          break;
        case 3:
          if(split_edges_ids[2] != m_ThirdFaceEdgeLT[split_edges_ids[0]*6+split_edges_ids[1]]){
            FinalizeRedTetra(curT);
            new_edges_split = true;
          } else {
            m_PendingTetras.push_back(curT);
          }
          break;
        default:
          FinalizeRedTetra(curT);
          new_edges_split = true;
        }
      } // while(prev_level_size)
    } // while(new_edges_split)

    // Here we know that no new split edges will be introduced, and we can
    // finalize the subdivisions of Green tetrahedra.
    while(m_PendingTetras.size()){
      RGMTetra_ptr curT;

      curT = m_PendingTetras.front();
      m_PendingTetras.pop_front();
      if(!curT->edges_split)
        m_Tetras.push_back(curT);
      else
        FinalizeGreenTetra(curT);
    } // while(pending_tetra_cnt)
    m_CurrentResol++;
  } // while(CurrentRes<m_NResolutions)
  
    
  // Discard the tetrahedra which are outside the object surface
  if(!m_KeepOutside){
    unsigned total_tets = m_Tetras.size();
    for(i=0;i<total_tets;i++){
      RGMTetra_ptr curT;
      RGMVertex_ptr vertices[4];
      bool have_inside_verts = false;
      
      curT = m_Tetras.front();
      m_Tetras.pop_front();
      vertices[0] = curT->edges[0]->nodes[0];
      vertices[1] = curT->edges[0]->nodes[1];
      vertices[2] = curT->edges[1]->nodes[0];
      vertices[3] = curT->edges[1]->nodes[1];
      
      for(j=0;j<4;j++){
        if(DistanceAtPoint(vertices[j]->coords)<=0.0){
          have_inside_verts = true;
          break;
        }
      }
      
      if(!have_inside_verts){
        if(!curT->parent)
          RemoveTetra(curT);
        else
          RemoveTetraAllocated(curT);
      } else {
        m_Tetras.push_back(curT);
      }
    }
  }
    
  // Prepare the surface for deformation. Here we use the heuristic
  // described by Molino: first, the set of enveloped vertices is
  // selected, so that each of the vertices has all of the incident edges
  // at least 25% inside the surface. Next all of the tetrhedra that do
  // not have at least one of the enveloped vertices are removed. There
  // is additional step to make sure the surface is manifold, but this is
  // in the TODO list.
      
  std::cout << "Finding enveloped vertices...";
  FindEnvelopedVertices();
  
  std::set<RGMVertex_ptr> all_vertices;
  for(typename std::list<RGMTetra_ptr>::iterator tI=m_Tetras.begin();
    tI!=m_Tetras.end();tI++){
    RGMTetra_ptr curT = *tI;
    all_vertices.insert(curT->edges[0]->nodes[0]);
    all_vertices.insert(curT->edges[0]->nodes[1]);
    all_vertices.insert(curT->edges[1]->nodes[0]);
    all_vertices.insert(curT->edges[1]->nodes[1]);
  }
  
  std::cout << "OK" << std::endl;
  std::cout << "Total vertices: " << all_vertices.size() << std::endl;
  std::cout << "Out of envelope vertices: " << m_OutOfEnvelopeVertices.size() << std::endl;
  std::cout << "Finding enveloped tetrahedra... (out of " << m_Tetras.size() << " total) ";
  
  
  // Now all of the tetrahedra which do not contain any of the enveloped
  // vertices are discarded
  int num_tetras = m_Tetras.size();
  int num_discarded = 0;
  for(j=0;j<num_tetras;j++){
    RGMTetra_ptr curT;
    int i, out_of_env_cnt = 0;
    RGMVertex_ptr vertices[4];

    curT = m_Tetras.front();
    m_Tetras.pop_front();
    
    // DEBUG -->
    assert(!curT->greens);
    // <--
     
    vertices[0] = curT->edges[0]->nodes[0];
    vertices[1] = curT->edges[0]->nodes[1];
    vertices[2] = curT->edges[1]->nodes[0];
    vertices[3] = curT->edges[1]->nodes[1];
    
    out_of_env_cnt = 0;
    for(i=0;i<4;i++){
      if(m_OutOfEnvelopeVertices.find(vertices[i])!=
        m_OutOfEnvelopeVertices.end())
        out_of_env_cnt++;
    }

    if(out_of_env_cnt>3)
      num_discarded++;
    else {
      m_Tetras.push_back(curT);
    }
    /*
      {
      if(!curT->parent)
        RemoveTetra(curT);
      else
        RemoveTetraAllocated(curT);
    } else 
      m_Tetras.push_back(curT);
      */
  }

  std::cout << num_discarded << " tets have been discarded" << std::endl;
  // Prepare the output, make sure that all terahedra are oriented
  // consistently
  std::map<RGMVertex_ptr,unsigned long> vertex2id;
  typename OutputMeshType::Pointer output_mesh = this->GetOutput();
  for(typename std::list<RGMTetra_ptr>::iterator tI=m_Tetras.begin();
    tI!=m_Tetras.end();tI++){
    RGMVertex_ptr thisT_nodes[4];
    unsigned long thisT_point_ids[4];
    
    /* orient3d() returns a negative value if the first three points appear in
     * counterclockwise order when viewed from the 4th point */
    if(orient3d(
        (*tI)->edges[0]->nodes[0]->coords, 
        (*tI)->edges[0]->nodes[1]->coords, 
        (*tI)->edges[1]->nodes[0]->coords, 
        (*tI)->edges[1]->nodes[1]->coords)<0){
      thisT_nodes[0] = (*tI)->edges[0]->nodes[0];
      thisT_nodes[1] = (*tI)->edges[0]->nodes[1];
    } else {
      thisT_nodes[1] = (*tI)->edges[0]->nodes[0];
      thisT_nodes[0] = (*tI)->edges[0]->nodes[1];
    }
    
    thisT_nodes[2] = (*tI)->edges[1]->nodes[0];
    thisT_nodes[3] = (*tI)->edges[1]->nodes[1];

    for(i=0;i<4;i++){
      if(vertex2id.find(thisT_nodes[i])==vertex2id.end()){
        OPointType new_point;
        new_point[0] = thisT_nodes[i]->coords[0];
        new_point[1] = thisT_nodes[i]->coords[1];
        new_point[2] = thisT_nodes[i]->coords[2];

        output_mesh->SetPoint(m_NumberOfPoints, new_point);
        vertex2id[thisT_nodes[i]] = m_NumberOfPoints;
        m_NumberOfPoints++;
      }
      thisT_point_ids[i] = vertex2id[thisT_nodes[i]];
    }
    
    TetCellAutoPointer newTet;
    newTet.TakeOwnership(new TetCell);
    newTet->SetPointIds(thisT_point_ids);
    output_mesh->SetCell(m_NumberOfTets, newTet);
    output_mesh->SetCellData(m_NumberOfTets, 
      (typename OMeshTraits::CellPixelType) 0.0);
    m_NumberOfTets++;
  }

  // Prevent useless re-execution of the filter (as described in
  // BinaryMask3DMeshSource)
  this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());

  // TODO: memory deallocation
}

/** Initialize the class fields */
template<class TInputImage, class TOutputMesh>
bool
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::Initialize()
{
  bool read_failed = false;
  
  if(this->GetNumberOfInputs() < 1)
    itkExceptionMacro(<<"Input image missing");
  
  std::cout << "Initializing..." << std::endl;
  
  // Initialize non-constant class members
  m_Interpolator = InterpolatorType::New();
  
  // Set the input image
  this->m_InputImage = static_cast<InputImageType*>(this->ProcessObject::GetInput(0));
  
  // Compute the distance image
  //  1. Cast the input image to the internal format
  
  // Read in all required images, if they are available
  if(m_InputImagePrefix.size()){

    // (1) resampled to unit-voxel input image
    try{
      InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
      reader->SetFileName((m_TmpDirectory+m_InputImagePrefix+"Ready.mhd").c_str());
      reader->Update();
      this->m_ReadyInputImage = reader->GetOutput();
    }catch(ExceptionObject &e){
      InternalImageWriterType::Pointer writer = InternalImageWriterType::New();
      PrepareInputImage();
      writer->SetFileName((m_TmpDirectory+m_InputImagePrefix+"Ready.mhd").c_str());
      writer->SetInput(this->m_ReadyInputImage);
      try{
        writer->Update();
      }catch(ExceptionObject &eo){
        // this should not disrupt the overall execution
      }
    }
    // (2) Distance transform of the input image
    try{
      InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
      reader->SetFileName((m_TmpDirectory+m_InputImagePrefix+"DT.mhd").c_str());
      reader->Update();
      m_DistanceImage = reader->GetOutput();
    } catch(ExceptionObject &e){
      InternalImageWriterType::Pointer writer = InternalImageWriterType::New();
      PrepareDistanceImage();
      writer->SetFileName((m_TmpDirectory+m_InputImagePrefix+"DT.mhd").c_str());
      writer->SetInput(m_DistanceImage);
      try{
        writer->Update();
      } catch(ExceptionObject &e){
      }
    }
    // (3) derivative image
    try{
      InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
      reader->SetFileName((m_TmpDirectory+m_InputImagePrefix+"RG.mhd").c_str());
      reader->Update();
      m_DistanceDerivativeImage = reader->GetOutput();
    } catch(ExceptionObject &e){
      InternalImageWriterType::Pointer writer = InternalImageWriterType::New();
      PrepareDerivativeImage();
      writer->SetFileName((m_TmpDirectory+m_InputImagePrefix+"RG.mhd").c_str());
      writer->SetInput(m_DistanceDerivativeImage);
      try{
        writer->Update();
      } catch(ExceptionObject &e){
      }
    }
    // (4) curvature image
    try{
      InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
      reader->SetFileName((m_TmpDirectory+m_InputImagePrefix+"Curv.mhd").c_str());
      reader->Update();
      m_CurvatureImage = reader->GetOutput();
    } catch(ExceptionObject &e){
      InternalImageWriterType::Pointer writer = InternalImageWriterType::New();
      PrepareCurvatureImage();
      writer->SetFileName((m_TmpDirectory+m_InputImagePrefix+"Curv.mhd").c_str());
      writer->SetInput(m_CurvatureImage);
      try{
        writer->Update();
      } catch(ExceptionObject &e){
      }
    }
    assert(m_DistanceImage!=m_CurvatureImage);
  } else {
    PrepareInputImage();
    PrepareDistanceImage();
    PrepareDerivativeImage();
    PrepareCurvatureImage();
  }
  
  typename InternalImageType::SizeType input_size =
    this->m_ReadyInputImage->GetLargestPossibleRegion().GetSize();
  this->m_dimX = input_size[0];
  this->m_dimY = input_size[1];
  this->m_dimZ = input_size[2];

  m_InputOrigin = m_InputImage->GetOrigin();

  // Initialize the interpolator
  m_Interpolator->SetInputImage(m_DistanceImage);
  
  // Find out the appropriate value of the BCC spacing
  m_BCCSpacing = FindBCCSpacing();
  std::cout << "Initialization done" << std::endl;
}

/* Calculates the curvature image based on the distance map. This code is
 * adapted with minimal modifications from the initial version of the mesher
 * by Aloys d'Aiasche
 */
template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::PrepareCurvatureImage(){

  //creation of the curvature image
  InternalImageType::Pointer CurvImage = InternalImageType::New(); //doit etre de type Image<float,3>
  InternalImageType::IndexType start;
  for (int i=0; i<3; i++) 
    start[i] = 0;
  InternalImageType::SizeType size;
  for (int i=0; i<3; i++) 
    size[i] = (unsigned int) m_DistanceImage->GetBufferedRegion().GetSize()[i];
  InternalImageType::RegionType region; 
  region.SetSize( size ); 
  region.SetIndex( start );
  CurvImage->SetSpacing(m_DistanceImage->GetSpacing());
  CurvImage->SetRegions( region ); 
  CurvImage->Allocate(); //created
  vnl_vector<float> gradient;
  gradient.set_size(3);


  std::cout << "computing the Gaussian filtering of the distance map" << std::endl;

  int okk;
  std::cout << "computing the curvature of the distance map" << std::endl;
  float DisGetPixel;
  float curvaturemax;
  float normgrad;
  InternalImageType::IndexType pIndex,pIndex2;
  InternalImageType::IndexType pIndex3,pIndex4,pIndex5,pIndex6;
  InternalImageType::IndexType pIndex7,pIndex8,pIndex9;
  int xp,yp,zp,xm,ym,zm;
  vnl_matrix<float> hessian;
  hessian.set_size(3,3);
  vnl_matrix<float> pma;
  pma.set_size(3,3);
  vnl_matrix<float> interm;
  interm.set_size(3,3);

  int compteurtest =-1;

  typedef itk::Matrix< double,3,3 > MatrixType;
  typedef itk::SymmetricEigenSystem< double,3 > SystemType;
  typedef itk::FixedArray< double,3 > EigenValueType;
  SystemType::Pointer system = SystemType::New();
  EigenValueType eivalue;
  MatrixType *systemMatrix = new MatrixType;

  // for each voxel in the distance image
  for(int lz=0;lz<size[2];lz++)
    for(int ly=0;ly<size[1];ly++) 
      for(int lx=0;lx<size[0];lx++){
        curvaturemax=0;
        pIndex[0] = lx; pIndex[1] = ly; pIndex[2] = lz;

        DisGetPixel = m_DistanceImage->GetPixel(pIndex);
        if ((DisGetPixel != -1)&&(DisGetPixel != 1))  // consider only surface pixels
          CurvImage->SetPixel(pIndex,-1);
        else{
          compteurtest++;
          //computation for x + 1/2
          if(lx<(size[0]-1)){
            pIndex2[0] = lx+1;
            pIndex2[1] = ly;
            pIndex2[2] = lz;
            okk=1;
          } else 
            okk=0;
          if(okk)
            if(((DisGetPixel==-1)&&(m_DistanceImage->GetPixel(pIndex2) == 1))||
              ((DisGetPixel==1)&&(m_DistanceImage->GetPixel(pIndex2) == -1))){
              if (lx>0) xm = -1; else xm = 0;
              if (lx<(size[0]-1)) xp = 1; else xp = 0;
              if (ly>0) ym = -1; else ym = 0;
              if (ly<(size[1]-1)) yp = 1; else yp = 0;
              if (lz>0) zm = -1; else zm = 0;
              if (lz<(size[2]-1)) zp = 1; else zp = 0;
              pIndex3[0] = lx; pIndex3[1] = ly+ym; pIndex3[2] = lz;
              pIndex4[0] = lx; pIndex4[1] = ly+yp; pIndex4[2] = lz;
              pIndex5[0] = lx+xp; pIndex5[1] = ly+ym; pIndex5[2] = lz;
              pIndex6[0] = lx+xp; pIndex6[1] = ly+yp; pIndex6[2] = lz;

              gradient[0] = m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                m_DistanceDerivativeImage->GetPixel(pIndex);
              gradient[1] = (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                m_DistanceDerivativeImage->GetPixel(pIndex3) +
                m_DistanceDerivativeImage->GetPixel(pIndex6) - 
                m_DistanceDerivativeImage->GetPixel(pIndex5))/4.0;
              pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zm;
              pIndex4[0] = lx; pIndex4[1] = ly; pIndex4[2] = lz+zp;
              pIndex5[0] = lx+xp; pIndex5[1] = ly; pIndex5[2] = lz+zm;
              pIndex6[0] = lx+xp; pIndex6[1] = ly; pIndex6[2] = lz+zp;
              gradient[2] = 
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                m_DistanceDerivativeImage->GetPixel(pIndex3) +
                m_DistanceDerivativeImage->GetPixel(pIndex6) - 
                m_DistanceDerivativeImage->GetPixel(pIndex5))/4.0;

              normgrad = sqrt(gradient[0]*gradient[0] + 
                gradient[1]*gradient[1] +
                gradient[2]*gradient[2]);

              if (lx<(size[0]-2)){
                pIndex2[0] = lx+2; pIndex2[1] = ly; pIndex2[2] = lz;
                pIndex3[0] = lx+xp; pIndex3[1] = ly; pIndex3[2] = lz;
                pIndex4[0] = lx+xm; pIndex4[1] = ly; pIndex4[2] = lz;
              }
              else{
                pIndex2[0] = lx+xp; pIndex2[1] = ly; pIndex2[2] = lz;
                pIndex3[0] = lx+xp; pIndex3[1] = ly; pIndex3[2] = lz;
                pIndex4[0] = lx+xm; pIndex4[1] = ly; pIndex4[2] = lz;
              }
              hessian[0][0] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex)) -
                (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex4)))/2.0;

              pIndex2[0] = lx+xp; pIndex2[1] = ly+yp; pIndex2[2] = lz;
              pIndex3[0] = lx; pIndex3[1] = ly+yp; pIndex3[2] = lz;
              pIndex4[0] = lx+xp; pIndex4[1] = ly+ym; pIndex4[2] = lz;
              pIndex5[0] = lx; pIndex5[1] = ly+ym; pIndex5[2] = lz;
              hessian[1][0] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex3))-
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex5)))/2.0;
              hessian[0][1] = hessian[1][0];

              pIndex2[0] = lx+xp; pIndex2[1] = ly; pIndex2[2] = lz+zp;
              pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zp;
              pIndex4[0] = lx+xp; pIndex4[1] = ly; pIndex4[2] = lz+zm;
              pIndex5[0] = lx; pIndex5[1] = ly; pIndex5[2] = lz+zm;
              hessian[2][0] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex3))-
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex5)))/2.0;
              hessian[0][2] = hessian[2][0];

              pIndex2[0] = lx; pIndex2[1] = ly+yp; pIndex2[2] = lz;
              pIndex3[0] = lx; pIndex3[1] = ly+ym; pIndex3[2] = lz;
              pIndex4[0] = lx+xp; pIndex4[1] = ly+yp; pIndex4[2] = lz;
              pIndex5[0] = lx+xp; pIndex5[1] = ly; pIndex5[2] = lz;
              pIndex6[0] = lx+xp; pIndex6[1] = ly+ym; pIndex6[2] = lz;

              hessian[1][1] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  2*m_DistanceDerivativeImage->GetPixel(pIndex) + 
                  m_DistanceDerivativeImage->GetPixel(pIndex3)) + 
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 2*m_DistanceDerivativeImage->GetPixel(pIndex5) +
                 m_DistanceDerivativeImage->GetPixel(pIndex6)))/2.0;

              pIndex2[0] = lx; pIndex2[1] = ly+yp; pIndex2[2] = lz+zp;
              pIndex3[0] = lx; pIndex3[1] = ly+ym; pIndex3[2] = lz+zp;
              pIndex4[0] = lx; pIndex4[1] = ly+yp; pIndex4[2] = lz+zm;
              pIndex5[0] = lx; pIndex5[1] = ly+zm; pIndex5[2] = lz+zm;
              pIndex6[0] = lx+xp; pIndex6[1] = ly+yp; pIndex6[2] = lz+zp;
              pIndex7[0] = lx+xp; pIndex7[1] = ly+ym; pIndex7[2] = lz+zp;
              pIndex8[0] = lx+xp; pIndex8[1] = ly+yp; pIndex8[2] = lz+zm;
              pIndex9[0] = lx+xp; pIndex9[1] = ly+ym; pIndex9[2] = lz+zm;

              hessian[2][1] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex3)) -
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex5)) +
                (m_DistanceDerivativeImage->GetPixel(pIndex6) -
                 m_DistanceDerivativeImage->GetPixel(pIndex7)) -
                (m_DistanceDerivativeImage->GetPixel(pIndex8) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex9)))/8.0;
              hessian[1][2] = hessian[2][1];

              pIndex2[0] = lx; pIndex2[1] = ly; pIndex2[2] = lz+zp;
              pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zm;
              pIndex4[0] = lx+xp; pIndex4[1] = ly; pIndex4[2] = lz+zp;
              pIndex5[0] = lx+xp; pIndex5[1] = ly; pIndex5[2] = lz;
              pIndex6[0] = lx+xp; pIndex6[1] = ly; pIndex6[2] = lz+zm;

              hessian[2][2] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  2*m_DistanceDerivativeImage->GetPixel(pIndex)) +
                (m_DistanceDerivativeImage->GetPixel(pIndex3) + 
                 m_DistanceDerivativeImage->GetPixel(pIndex4)) -
                (2*m_DistanceDerivativeImage->GetPixel(pIndex5) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex6)))/2.0;

              //we have the hessian matrix for the point x+1/2
              //we can compute: P = I-NNt
              //and the eigen values of PHP/|normgrad|
              for(int a=0;a<3;a++) gradient[a] = gradient[a]/normgrad;

              pma[0][0] = 1 - gradient[0]*gradient[0];
              pma[0][1] = gradient[0]*gradient[1];
              pma[0][2] = gradient[0]*gradient[2];
              pma[1][0] = pma[0][1];
              pma[1][1] = 1 - gradient[1]*gradient[1];
              pma[1][2] = gradient[1]*gradient[2];
              pma[2][0] = pma[0][2];
              pma[2][1] = pma[1][2];
              pma[2][2] = 1 - gradient[2]*gradient[2];

              interm = pma*hessian;
              interm = interm*pma;

              //############################################# compute the eigen values #########
              for(int mj=0;mj<3;mj++)
                for(int mi=0;mi<3;mi++)
                  (*systemMatrix)[mj][mi]= (double) interm[mj][mi];

              system->SetMatrix(systemMatrix);
              system->Update();
              eivalue = *(system->GetEigenValues());

              //  //a simple test: one of the eigen values has to equal zero.
              //  if((abs(eivalue[0]*eivalue[1]*eivalue[2]))>0.001)
              //    cout<<"cpt: "<<compteurtest<<": eigen values too important: "<<
              //      eivalue[0]<<","<<eivalue[1]<<","<<eivalue[2]<<endl;

              if ((fabs(eivalue[0])+fabs(eivalue[1])+fabs(eivalue[2]))>curvaturemax)
                curvaturemax = fabs(eivalue[0])+fabs(eivalue[1])+fabs(eivalue[2]);

              }// if(m_DistImage->GetPixel(pIndex) == 1)
          //#######################################################################################
          //computation for y + 1/2
          if(ly<(size[1]-1)){pIndex2[0] = lx;pIndex2[1] = ly+1;pIndex2[2] = lz;okk=1;} else okk=0;
          if(okk)
            if(((DisGetPixel==-1)&&(m_DistanceImage->GetPixel(pIndex2) == 1))||
              ((DisGetPixel==1)&&(m_DistanceImage->GetPixel(pIndex2) == -1))){ 
              if (lx>0) xm = -1; else xm = 0;
              if (lx<(size[0]-1)) xp = 1; else xp = 0;
              if (ly>0) ym = -1; else ym = 0;
              if (ly<(size[1]-1)) yp = 1; else yp = 0;
              if (lz>0) zm = -1; else zm = 0;
              if (lz<(size[2]-1)) zp = 1; else zp = 0;
              pIndex3[0] = lx+xp; pIndex3[1] = ly; pIndex3[2] = lz;
              pIndex4[0] = lx+xm; pIndex4[1] = ly; pIndex4[2] = lz;
              pIndex5[0] = lx+xp; pIndex5[1] = ly+yp; pIndex5[2] = lz;
              pIndex6[0] = lx+xm; pIndex6[1] = ly+yp; pIndex6[2] = lz;
              gradient[0] = (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                m_DistanceDerivativeImage->GetPixel(pIndex4) +
                m_DistanceDerivativeImage->GetPixel(pIndex5) - 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/4.0;
              gradient[1] = m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                m_DistanceDerivativeImage->GetPixel(pIndex);

              pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zp;
              pIndex4[0] = lx; pIndex4[1] = ly; pIndex4[2] = lz+zm;
              pIndex5[0] = lx; pIndex5[1] = ly+yp; pIndex5[2] = lz+zp;
              pIndex6[0] = lx; pIndex6[1] = ly+yp; pIndex6[2] = lz+zm;
              gradient[2] = (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                m_DistanceDerivativeImage->GetPixel(pIndex4)+
                m_DistanceDerivativeImage->GetPixel(pIndex5) - 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/4.0;


              normgrad = sqrt(gradient[0]*gradient[0] + 
                gradient[1]*gradient[1] +
                gradient[2]*gradient[2]);

              pIndex2[0] = lx+xp; pIndex2[1] = ly; pIndex2[2] = lz;
              pIndex3[0] = lx+xm; pIndex3[1] = ly; pIndex3[2] = lz;
              pIndex4[0] = lx+xp; pIndex4[1] = ly+yp; pIndex4[2] = lz;
              pIndex5[0] = lx; pIndex5[1] = ly+yp; pIndex5[2] = lz;
              pIndex6[0] = lx+xm; pIndex6[1] = ly+yp; pIndex6[2] = lz;
              hessian[0][0] = (m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                2*m_DistanceDerivativeImage->GetPixel(pIndex) +
                m_DistanceDerivativeImage->GetPixel(pIndex3) + 
                m_DistanceDerivativeImage->GetPixel(pIndex4) -
                2*m_DistanceDerivativeImage->GetPixel(pIndex5) + 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/2.0;

              hessian[1][0] = ((m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex6)) - 
                (m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex3)))/2.0;
              hessian[0][1] = hessian[1][0];

              pIndex2[0] = lx+xp; pIndex2[1] = ly; pIndex2[2] = lz+zp;
              pIndex3[0] = lx+xm; pIndex3[1] = ly; pIndex3[2] = lz+zp;
              pIndex4[0] = lx+xp; pIndex4[1] = ly; pIndex4[2] = lz+zm;
              pIndex5[0] = lx+xm; pIndex5[1] = ly; pIndex5[2] = lz+zm;
              pIndex6[0] = lx+xp; pIndex6[1] = ly+yp; pIndex6[2] = lz+zp;
              pIndex7[0] = lx+xm; pIndex7[1] = ly+yp; pIndex7[2] = lz+zp;
              pIndex8[0] = lx+xp; pIndex8[1] = ly+yp; pIndex8[2] = lz+zm;
              pIndex9[0] = lx+xm; pIndex9[1] = ly+yp; pIndex9[2] = lz+zm;

              hessian[2][0] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex3)) -
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex5)) +
                (m_DistanceDerivativeImage->GetPixel(pIndex6) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex7))-
                (m_DistanceDerivativeImage->GetPixel(pIndex8) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex9)))/8.0;
              hessian[0][2] = hessian[2][0];

              if (ly<(size[1]-2)){
                pIndex2[0] = lx; pIndex2[1] = ly+2; pIndex2[2] = lz;
                pIndex3[0] = lx; pIndex3[1] = ly+yp; pIndex3[2] = lz;
                pIndex4[0] = lx; pIndex4[1] = ly+ym; pIndex4[2] = lz;
              }
              else{
                pIndex2[0] = lx; pIndex2[1] = ly+yp; pIndex2[2] = lz;
                pIndex3[0] = lx; pIndex3[1] = ly+yp; pIndex3[2] = lz;
                pIndex4[0] = lx; pIndex4[1] = ly+ym; pIndex4[2] = lz;
              }

              hessian[1][1] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex)) - 
                (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex4)))/2.0;

              pIndex2[0] = lx; pIndex2[1] = ly+yp; pIndex2[2] = lz+zp;
              pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zp;
              pIndex4[0] = lx; pIndex4[1] = ly+yp; pIndex4[2] = lz+zm;
              pIndex5[0] = lx; pIndex5[1] = ly; pIndex5[2] = lz+zm;

              hessian[2][1] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex3)) - 
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex5)))/2.0;
              hessian[1][2] = hessian[2][1];

              pIndex6[0] = lx; pIndex6[1] = ly+yp; pIndex6[2] = lz;

              hessian[2][2] = (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                2*m_DistanceDerivativeImage->GetPixel(pIndex) +
                m_DistanceDerivativeImage->GetPixel(pIndex5) + 
                m_DistanceDerivativeImage->GetPixel(pIndex2) -
                2*m_DistanceDerivativeImage->GetPixel(pIndex6) + 
                m_DistanceDerivativeImage->GetPixel(pIndex4))/2.0;

              //we have the hessian matrix for the point y+1/2
              //we can compute: P = I-NNt
              //and the eigen values of PHP/|normgrad|

              for(int a=0;a<3;a++) gradient[a] = gradient[a]/normgrad;
              pma[0][0] = 1 - gradient[0]*gradient[0];
              pma[0][1] = gradient[0]*gradient[1];
              pma[0][2] = gradient[0]*gradient[2];
              pma[1][0] = pma[0][1];
              pma[1][1] = 1 - gradient[1]*gradient[1];
              pma[1][2] = gradient[1]*gradient[2];
              pma[2][0] = pma[0][2];
              pma[2][1] = pma[1][2];
              pma[2][2] = 1 - gradient[2]*gradient[2];

              interm = pma*hessian;
              interm = interm*pma;
              //############################################# compute the eigen values #########
              for(int mj=0;mj<3;mj++)
                for(int mi=0;mi<3;mi++)
                  (*systemMatrix)[mj][mi]= (double) interm[mj][mi];

              system->SetMatrix(systemMatrix);
              system->Update();
              eivalue = *(system->GetEigenValues());

              //a simple test: one of the eigen values has to equal zero.
              //  if((abs(eivalue[0]*eivalue[1]*eivalue[2]))>0.001)
              //    cout<<"cpt: "<<compteurtest<<": eigen values too important: "<<
              //      eivalue[0]<<","<<eivalue[1]<<","<<eivalue[2]<<endl;

              if ((fabs(eivalue[0])+fabs(eivalue[1])+fabs(eivalue[2]))>curvaturemax)
                curvaturemax = fabs(eivalue[0])+fabs(eivalue[1])+fabs(eivalue[2]);

              }// if(m_DistImage->GetPixel(pIndex) == 1)

          //#######################################################################################
          //computation for z + 1/2
          if(lz<(size[2]-1)){pIndex2[0] = lx;pIndex2[1] = ly;pIndex2[2] = lz+1;okk=1;} else okk=0;
          if(okk)
            if(((DisGetPixel==-1)&&(m_DistanceImage->GetPixel(pIndex2) == 1))||
              ((DisGetPixel==1)&&(m_DistanceImage->GetPixel(pIndex2) == -1)))
              { 
              if (lx>0) xm = -1; else xm = 0;
              if (lx<(size[0]-1)) xp = 1; else xp = 0;
              if (ly>0) ym = -1; else ym = 0;
              if (ly<(size[1]-1)) yp = 1; else yp = 0;
              if (lz>0) zm = -1; else zm = 0;
              if (lz<(size[2]-1)) zp = 1; else zp = 0;
              pIndex3[0] = lx+xp; pIndex3[1] = ly; pIndex3[2] = lz;
              pIndex4[0] = lx+xm; pIndex4[1] = ly; pIndex4[2] = lz;
              pIndex5[0] = lx+xp; pIndex5[1] = ly; pIndex5[2] = lz+zp;
              pIndex6[0] = lx+xm; pIndex6[1] = ly; pIndex6[2] = lz+zp;
              gradient[0] = (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                m_DistanceDerivativeImage->GetPixel(pIndex4) +
                m_DistanceDerivativeImage->GetPixel(pIndex5) - 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/4.0;

              pIndex3[0] = lx; pIndex3[1] = ly+yp; pIndex3[2] = lz;
              pIndex4[0] = lx; pIndex4[1] = ly+ym; pIndex4[2] = lz;
              pIndex5[0] = lx; pIndex5[1] = ly+yp; pIndex5[2] = lz+zp;
              pIndex6[0] = lx; pIndex6[1] = ly+ym; pIndex6[2] = lz+zp;
              gradient[1] = (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                m_DistanceDerivativeImage->GetPixel(pIndex4) +
                m_DistanceDerivativeImage->GetPixel(pIndex5) - 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/4.0;

              gradient[2] = m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                m_DistanceDerivativeImage->GetPixel(pIndex);

              normgrad =  sqrt(gradient[0]*gradient[0] + 
                gradient[1]*gradient[1] +
                gradient[2]*gradient[2]);

              pIndex3[0] = lx+xp; pIndex3[1] = ly; pIndex3[2] = lz;
              pIndex4[0] = lx+xm; pIndex4[1] = ly; pIndex4[2] = lz;
              pIndex5[0] = lx+xp; pIndex5[1] = ly; pIndex5[2] = lz+zp;
              pIndex6[0] = lx+xm; pIndex6[1] = ly; pIndex6[2] = lz+zp;

              hessian[0][0] = (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                2*m_DistanceDerivativeImage->GetPixel(pIndex) +
                m_DistanceDerivativeImage->GetPixel(pIndex4) + 
                m_DistanceDerivativeImage->GetPixel(pIndex5) -
                2*m_DistanceDerivativeImage->GetPixel(pIndex2) + 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/2.0;

              pIndex2[0] = lx+xp; pIndex2[1] = ly+yp; pIndex2[2] = lz;
              pIndex3[0] = lx+xm; pIndex3[1] = ly+yp; pIndex3[2] = lz;
              pIndex4[0] = lx+xp; pIndex4[1] = ly+ym; pIndex4[2] = lz;
              pIndex5[0] = lx+xm; pIndex5[1] = ly+ym; pIndex5[2] = lz;
              pIndex6[0] = lx+xp; pIndex6[1] = ly+yp; pIndex6[2] = lz+zp;
              pIndex7[0] = lx+xm; pIndex7[1] = ly+yp; pIndex7[2] = lz+zp;
              pIndex8[0] = lx+xp; pIndex8[1] = ly+ym; pIndex8[2] = lz+zp;
              pIndex9[0] = lx+xm; pIndex9[1] = ly+ym; pIndex9[2] = lz+zp;

              hessian[1][0] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex3))-
                (m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex5)) +
                (m_DistanceDerivativeImage->GetPixel(pIndex6) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex7)) -
                (m_DistanceDerivativeImage->GetPixel(pIndex8) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex9)))/8.0;
              hessian[0][1] = hessian[1][0];

              pIndex2[0] = lx+xp; pIndex2[1] = ly; pIndex2[2] = lz+zp;
              pIndex3[0] = lx+xm; pIndex3[1] = ly; pIndex3[2] = lz+zp;
              pIndex4[0] = lx+xp; pIndex4[1] = ly; pIndex4[2] = lz;
              pIndex5[0] = lx+xm; pIndex5[1] = ly; pIndex5[2] = lz;

              hessian[0][2] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex)) - 
                (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex4)))/2.0;
              hessian[2][0] = hessian[0][2];

              pIndex2[0] = lx; pIndex2[1] = ly+yp; pIndex2[2] = lz;
              pIndex3[0] = lx; pIndex3[1] = ly+ym; pIndex3[2] = lz;
              pIndex4[0] = lx; pIndex4[1] = ly+yp; pIndex4[2] = lz+zp;
              pIndex5[0] = lx; pIndex5[1] = ly; pIndex5[2] = lz+zp;
              pIndex6[0] = lx; pIndex6[1] = ly+ym; pIndex6[2] = lz+zp;

              hessian[1][1] = (m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                2*m_DistanceDerivativeImage->GetPixel(pIndex) +
                m_DistanceDerivativeImage->GetPixel(pIndex3) + 
                m_DistanceDerivativeImage->GetPixel(pIndex4) -
                2*m_DistanceDerivativeImage->GetPixel(pIndex5) + 
                m_DistanceDerivativeImage->GetPixel(pIndex6))/2.0;

              hessian[1][2] = ((m_DistanceDerivativeImage->GetPixel(pIndex4) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex6)) - 
                (m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex3)))/2.0;
              hessian[2][1] = hessian[1][2];

              if (lz<(size[2]-2)){
                pIndex2[0] = lx; pIndex2[1] = ly; pIndex2[2] = lz+2;
                pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zp;
                pIndex4[0] = lx; pIndex4[1] = ly; pIndex4[2] = lz+zm;
              }
              else{
                pIndex2[0] = lx; pIndex2[1] = ly; pIndex2[2] = lz+zp;
                pIndex3[0] = lx; pIndex3[1] = ly; pIndex3[2] = lz+zp;
                pIndex4[0] = lx; pIndex4[1] = ly; pIndex4[2] = lz+zm;
              }

              hessian[1][1] = ((m_DistanceDerivativeImage->GetPixel(pIndex2) - 
                  m_DistanceDerivativeImage->GetPixel(pIndex)) - 
                (m_DistanceDerivativeImage->GetPixel(pIndex3) - 
                 m_DistanceDerivativeImage->GetPixel(pIndex4)))/2.0;

              //we have the hessian matrix for the point z+1/2
              //we can compute: P = I-NNt
              //and the eigen values of PHP/|normgrad|
              for(int a=0;a<3;a++) gradient[a] = gradient[a]/normgrad;
              pma[0][0] = 1 - gradient[0]*gradient[0];
              pma[0][1] = gradient[0]*gradient[1];
              pma[0][2] = gradient[0]*gradient[2];
              pma[1][0] = pma[0][1];
              pma[1][1] = 1 - gradient[1]*gradient[1];
              pma[1][2] = gradient[1]*gradient[2];
              pma[2][0] = pma[0][2];
              pma[2][1] = pma[1][2];
              pma[2][2] = 1 - gradient[2]*gradient[2];

              interm = pma*hessian;
              interm = interm*pma;
              //############################################# compute the eigen values #########
              for(int mj=0;mj<3;mj++)
                for(int mi=0;mi<3;mi++)
                  {
                  (*systemMatrix)[mj][mi]= (double) interm[mj][mi];
                  }

              system->SetMatrix(systemMatrix);
              system->Update();
              eivalue = *(system->GetEigenValues());

              //a simple test: one of the eigen values has to equal zero.
              //  if((abs(eivalue[0]*eivalue[1]*eivalue[2]))>0.001)
              //    cout<<"cpt: "<<compteurtest<<": eigen values too important: "<<
              //      eivalue[0]<<","<<eivalue[1]<<","<<eivalue[2]<<endl;

              if ((fabs(eivalue[0])+fabs(eivalue[1])+fabs(eivalue[2]))>curvaturemax)
                curvaturemax = (float)fabs(eivalue[0])+fabs(eivalue[1])+fabs(eivalue[2]);

              }// if(m_DistImage->GetPixel(pIndex) == 1)
          CurvImage->SetPixel(pIndex,curvaturemax);
          }//else
        }//end triple for: lz,ly,lx;
  std::cout << "Curvature computation finished" << std::endl;
  m_CurvatureImage = CurvImage;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::CreateMesh()
{
  m_CurrentResol = 0;

  if (this->GetNumberOfInputs() < 1)
    {
    std::cout << "BinaryMaskTo3DAdaptiveMeshFilter : Binary image mask not set" << std::endl;
    return;
    }

  typename InputImageType::ConstPointer m_InputImage = 
    static_cast<const InputImageType * >(this->ProcessObject::GetInput(0) );

  this->CreateBCC();
  std::cout << "BCC created" << std::endl;

  // If requested, need to remove the elements which are guaranteed to be
  // outside the object
  if(!m_KeepOutside){
    // "The tetrahedron is guaranteed not to intersect the interface
    // if the minimum valued of |phi| at a node is larger than the longest
    // edge length" [Molino]
    // Of course, a tetrahedron is not discarded if it has both negative and
    // positive values of distance at the vertices.
    unsigned long total_tets = m_Tetras.size(), i, j;
    for(i=0;i<total_tets;i++){
      float min_distance = m_BCCSpacing*10.0;
      float max_edge = 0.0;
      bool have_neg = false;
      bool discard_tetra = true;
      RGMTetra_ptr thisT;
      RGMVertex_ptr vertices[4];
      
      thisT = m_Tetras.front();
      m_Tetras.pop_front();
      vertices[0] = thisT->edges[0]->nodes[0];
      vertices[1] = thisT->edges[0]->nodes[1];
      vertices[2] = thisT->edges[1]->nodes[0];
      vertices[3] = thisT->edges[1]->nodes[1];
      for(j=0;j<4;j++){
        float distance = DistanceAtPoint(
          &vertices[j]->coords[0]);
        if(distance<=0.0)
          have_neg = true;
        min_distance = fminf(min_distance, fabsf(distance));
      }
      
      if(have_neg){
        // certainly crosses the surface or is inside
        discard_tetra = false;
      } else {
        for(j=0;j<6;j++)
          if(min_distance<DistanceBwPoints(thisT->edges[j]->nodes[0]->coords, 
              thisT->edges[j]->nodes[1]->coords))
            discard_tetra = false;
      }
      if(discard_tetra)
        RemoveBCCTetra(thisT);
      else
        m_Tetras.push_back(thisT);
    }
  }
  
}

/** PrintSelf */
template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
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
template<class TInputImage, class TOutputMesh>
float
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::DistanceAtPoint(double *coords){

#ifdef MODEL_SPHERE
  double center[3];
  double radius = 30;
  center[0] = 50.5;
  center[1] = 50.5;
  center[2] = 50.5;
  return sqrt((center[0]-coords[0])*(center[0]-coords[0]) + 
    (center[1]-coords[1])*(center[1]-coords[1]) + 
    (center[2]-coords[2])*(center[2]-coords[2])) - radius;
#endif // MODEL_SPHERE

  float distance = 0;
  typename InterpolatorType::ContinuousIndexType input_index;

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
template<class TInputImage, class TOutputMesh>
float
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::DistanceBwPoints(double *c0, double *c1){
  return sqrt((c0[0]-c1[0])*(c0[0]-c1[0])+(c0[1]-c1[1])*(c0[1]-c1[1])+
    (c0[2]-c1[2])*(c0[2]-c1[2]));
}

/* Return true if the tetraheron crosses the surface and has to 
 * be subdivided */
template<class TInputImage, class TOutputMesh>
bool
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::SubdivInclusionTest(RGMTetra_ptr thisT){
  int i, j;
  InternalImageType::IndexType indices[4];
  InternalPixelType dist_vals[4];
  RGMVertex_ptr vertices[4];

  vertices[0] = thisT->edges[0]->nodes[0];
  vertices[1] = thisT->edges[0]->nodes[1];
  vertices[2] = thisT->edges[1]->nodes[0];
  vertices[3] = thisT->edges[1]->nodes[1];

  bool have_neg = false, have_pos = false;
  for(i=0;i<4;i++){
    InternalImageType::IndexType index;
    InternalImageType::PointType point;
    InternalImageType::PixelType pixel;
    bool is_inside;
    
    for(j=0;j<3;j++)
      point[j] = vertices[i]->coords[j];
    pixel = DistanceAtPoint(&vertices[i]->coords[0]);

    if(pixel>0.0)
      have_pos = true;
    else
      have_neg = true;
  }
  if(have_pos && have_neg)
    return true;
  else
    return false;

}

/* Returns true if the tetrahedron is in the high curvature region
 * and has to be subdivided. This code has been adopted with minor changes
 * from the initial implementation of the mesher by Aloys d'Aiasche */
template<class TInputImage, class TOutputMesh>
bool
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::SubdivCurvatureTest(RGMTetra_ptr thisT){
  //Ajouter un critère de distance des noeuds

  RGMVertex_ptr vertices[4];
  InternalImageType::IndexType index;
  InternalImageType::PointType point;
  InternalImageType::PixelType pixel;
  int gb=0;
  double minx,maxx,miny,maxy,minz,maxz;

  vertices[0] = thisT->edges[0]->nodes[0];
  vertices[1] = thisT->edges[0]->nodes[1];
  vertices[2] = thisT->edges[1]->nodes[0];
  vertices[3] = thisT->edges[1]->nodes[1];

  // pre: we have the curvature map    
  // post: comparison between the length of the elements and the eigenvalues
  vnl_matrix<double> ttd;
  ttd.set_size(4,3);

  // trouver les 4 noeuds et leurs positions. 
  for(unsigned int n=0;n<4;n++)
    for (int auk=0;auk<3;auk++)
      ttd[n][auk] = vertices[n]->coords[auk];

  //prendre distance entre les deux premiers noeuds et aussi entre premier et troisième noeud.
  float distlg1 = sqrt ((ttd[0][0]-ttd[1][0])*(ttd[0][0]-ttd[1][0]) + 
    (ttd[0][1]-ttd[1][1])*(ttd[0][1]-ttd[1][1]) +
    (ttd[0][2]-ttd[1][2])*(ttd[0][2]-ttd[1][2]));
  float distlg2 = sqrt ((ttd[0][0]-ttd[1][0])*(ttd[0][0]-ttd[1][0]) + 
    (ttd[0][1]-ttd[1][1])*(ttd[0][1]-ttd[1][1]) +
    (ttd[0][2]-ttd[1][2])*(ttd[0][2]-ttd[1][2]));
  float dist2compare;
  if (distlg1<distlg2) dist2compare = distlg1;
  else dist2compare = distlg2;

  minx=100000;maxx=-1;
  miny=100000;maxy=-1;
  minz=100000;maxz=-1;
  
  // regarder ds un cube avec les noeuds de position max
  for(int yu=0;yu<4;yu++){
    if (ttd[yu][0]<minx) minx=ttd[yu][0];
    if (ttd[yu][0]>maxx) maxx=ttd[yu][0];
    if (ttd[yu][1]<miny) miny=ttd[yu][1];
    if (ttd[yu][1]>maxy) maxy=ttd[yu][1];
    if (ttd[yu][2]<minz) minz=ttd[yu][2];
    if (ttd[yu][2]>maxz) maxz=ttd[yu][2];
  }

  //on va regarder si les elements sont egaux à -1 et si ils sont ds le tetraedre.
  InternalImageType::IndexType pixelIndex;
  double ax,ay,az,bx,by,bz,cx,cy,cz,dx,dy,dz;
  double deter,testtd,signtd;
  int insidethetrahedron;
  float currentvalueCurvImage;
  float nbreelem = 0;
  float nbreupper = 0;
  for(double xtd=minx;xtd<maxx;xtd++)
    for(double ytd=miny;ytd<maxy;ytd++)
      for(double ztd=minz;ztd<maxz;ztd++){
        InternalImageType::PointType pixelPoint;
        bool point_is_inside;
        insidethetrahedron = 1;
        pixelPoint[0] = xtd;
        pixelPoint[1] = ytd;
        pixelPoint[2] = ztd;
        
        point_is_inside = m_CurvatureImage->TransformPhysicalPointToIndex(pixelPoint,pixelIndex);
        assert(point_is_inside);
        currentvalueCurvImage = m_CurvatureImage->GetPixel(pixelIndex);
        if(currentvalueCurvImage<0)
          insidethetrahedron = 0;

        if(insidethetrahedron){
          double dxtd = (double) xtd;
          double dytd = (double) ytd;
          double dztd = (double) ztd;

          for(int ko=0;ko<5;ko++)
            if(insidethetrahedron){
              if(ko==0){
                ax=ttd[0][0];ay=ttd[0][1];az=ttd[0][2];
                bx=ttd[1][0];by=ttd[1][1];bz=ttd[1][2];
                cx=ttd[2][0];cy=ttd[2][1];cz=ttd[2][2];
                dx=ttd[3][0];dy=ttd[3][1];dz=ttd[3][2];}
              else if(ko==1){
                ax=dxtd;ay=dytd;az=dztd;
                bx=ttd[1][0];by=ttd[1][1];bz=ttd[1][2];
                cx=ttd[2][0];cy=ttd[2][1];cz=ttd[2][2];
                dx=ttd[3][0];dy=ttd[3][1];dz=ttd[3][2];}
              else if(ko==2){
                ax=ttd[0][0];ay=ttd[0][1];az=ttd[0][2];
                bx=dxtd;by=dytd;bz=dztd;
                cx=ttd[2][0];cy=ttd[2][1];cz=ttd[2][2];
                dx=ttd[3][0];dy=ttd[3][1];dz=ttd[3][2];}
              else if(ko==3){
                ax=ttd[0][0];ay=ttd[0][1];az=ttd[0][2];
                bx=ttd[1][0];by=ttd[1][1];bz=ttd[1][2];
                cx=dxtd;cy=dytd;cz=dztd;
                dx=ttd[3][0];dy=ttd[3][1];dz=ttd[3][2];}
              else if(ko==4){
                ax=ttd[0][0];ay=ttd[0][1];az=ttd[0][2];
                bx=ttd[1][0];by=ttd[1][1];bz=ttd[1][2];
                cx=ttd[2][0];cy=ttd[2][1];cz=ttd[2][2];
                dx=dxtd;dy=dytd;dz=dztd;}

                deter = ax*(by*(cz-dz)-cy*(bz-dz)+dy*(bz-cz))
                  - ay*(bx*(cz-dz)-cx*(bz-dz)+dx*(bz-cz))
                  + az*(bx*(cy-dy)-cx*(by-dy)+dx*(by-cy))
                  - (bx*(cy*dz-cz*dy)-cx*(by*dz-bz*dy)+dx*(by*cz-bz*cy));

                //petit test
                if (ko==0) testtd=deter;
                else testtd=testtd-deter;
                //fin petit test

                if(ko==0)
                  if (deter>0)signtd=1;
                  else if (deter<0)signtd=-1;
                  else std::cout << "element degenerated" << std::endl;
                else if(deter*signtd<0) insidethetrahedron=0;
              }//end for(int ko=0;ko<5;ko++)
          }//end if(insidethetrahedron) premier

        if(insidethetrahedron)
          if ((testtd>0.001)||(testtd<-0.001)) 
            std::cout << "problem of determinant in td" << std::endl;

        //we are in the tetrahedron. Now we have to compare with the length of tetrahedron edges.
        if(insidethetrahedron)
          if(currentvalueCurvImage>0.0001)
            if ((1/dist2compare) < currentvalueCurvImage) {nbreelem++; nbreupper++;}
            else nbreelem++;
        }//end triple for xtd,ytd,ztd

  //comparison with the length of the edges.
  //if more than 12% of the curvatures are larger than the length of the tetrahedron, we divide
  if (nbreupper > (0.12*nbreelem)) gb=1;

  return gb;

}

/* Returns true if the tetrahedron is completely outside the object surface */
template<class TInputImage, class TOutputMesh>
bool
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::IsTetraOutside(RGMTetra_ptr thisT){
  bool have_neg = false;
  int i;
  RGMVertex_ptr vertices[4];
  
  vertices[0] = thisT->edges[0]->nodes[0];
  vertices[1] = thisT->edges[0]->nodes[1];
  vertices[2] = thisT->edges[1]->nodes[0];
  vertices[3] = thisT->edges[1]->nodes[1];
  
  for(i=0;i<4;i++)
    if(DistanceAtPoint(&vertices[i]->coords[0])<0.0){
      have_neg = true;
      break;
    }

  return !have_neg;
}

/* Generates the initial BCC lattice */
template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::CreateBCC(){
  // TODO: make the BCC conformant with the image boundaries

  // The outer BCC is 1 spacing distance larger than the inner BCC in all
  // dimensions. The number of tetrahedra in level 0 can be calculated 
  // Start traversal of the vertices of outer BCC, the first vertex has
  // coordinates (m_x0+spacing,m_y0+spacing,m_z0+spacing). 

  // Generate the BCC vertices
  double dx, dy, dz;
  unsigned i, j, k;
  unsigned dsi, dsj, dsk;
  double half_spacing = this->m_BCCSpacing/2.0;
  double m_x0, m_y0, m_z0;
  
  m_x0 = m_InputOrigin[0];
  m_y0 = m_InputOrigin[1];
  m_z0 = m_InputOrigin[2];

  RGMVertex_ptr *BCC, *iBCC;

  dsi = (int) (this->m_dimX/this->m_BCCSpacing + 1);
  dsj = (int) (this->m_dimY/this->m_BCCSpacing + 1);
  dsk = (int) (this->m_dimZ/this->m_BCCSpacing + 1);

  BCC = new RGMVertex_ptr[(dsi+1)*(dsj+1)*(dsk+1)];
  iBCC = new RGMVertex_ptr[dsi*dsj*dsk];


  for(dz=m_z0,k=0;k<dsk;dz+=m_BCCSpacing,k++){
    for(dy=m_y0,j=0;j<dsj;dy+=m_BCCSpacing,j++){
      for(dx=m_x0,i=0;i<dsi;dx+=m_BCCSpacing,i++){
        BCC[k*(dsi+1)*(dsj+1)+j*(dsi+1)+i] = InsertVertex(dx, dy, dz);
        if(dx!=m_x0+m_dimX && dy!=m_y0+m_dimY && dz!=m_z0+m_dimZ){
          iBCC[k*dsi*dsj+j*dsi+i] = InsertVertex(dx+half_spacing, dy+half_spacing, dz+half_spacing);
        }
      }
    }
  }

  int ii, ik, ij;
  RGMEdge_ptr *BCC_edges = new RGMEdge_ptr[dsj*dsk*dsi*3];
  RGMEdge_ptr *iBCC_edges = new RGMEdge_ptr[dsj*dsk*dsi*3];
  RGMEdge_ptr *d_edges = new RGMEdge_ptr[dsk*dsj*dsi*8];
  int BCC_edges_total = 0, iBCC_edges_total = 0,
      d_edges_total = 0;

  for(k=1,ik=0;k<dsk;k++,ik++){
    for(j=1,ij=0;j<dsj;j++,ij++){
      for(i=1,ii=0;i<dsi;i++,ii++){
        BCC_edges[ik*dsi*dsj*3 + ij*dsi*3 + ii*3 + 0] = 
          InsertEdge(
            BCC[(k-1)*(dsi+1)*(dsj+1) + j*(dsi+1) + i], 
            BCC[k*(dsi+1)*(dsj+1) + j*(dsi+1) + i]);
        BCC_edges[ik*dsi*dsj*3 + ij*dsi*3 + ii*3 + 1] = 
          InsertEdge(
            BCC[k*(dsi+1)*(dsj+1) + j*(dsi+1) + (i-1)], 
            BCC[k*(dsi+1)*(dsj+1) + j*(dsi+1) + i]);
        BCC_edges[ik*dsi*dsj*3 + ij*dsi*3 + ii*3 + 2] = 
          InsertEdge(
            BCC[k*(dsi+1)*(dsj+1) + (j-1)*(dsi+1) + i], 
            BCC[k*(dsi+1)*(dsj+1) + j*(dsi+1) + i]);

        // iBCC_edge[i] is parallel to BCC_edge[i]
        iBCC_edges[ik*dsi*dsj*3 + ij*dsi*3 + ii*3 + 0] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            iBCC[(ik+1)*dsi*dsj + ij*dsi + ii]);
        iBCC_edges[ik*dsi*dsj*3 + ij*dsi*3 + ii*3 + 1] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            iBCC[ik*dsi*dsj + ij*dsi + (ii+1)]);
        iBCC_edges[ik*dsi*dsj*3 + ij*dsi*3 + ii*3 + 2] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            iBCC[ik*dsi*dsj + (ij+1)*dsi + ii]);

        // reverse Z traversal of the BCC vertices above the current iBCC node
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 0] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[k*(dsi+1)*(dsj+1) + j*(dsi+1) + i]);
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 1] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[k*(dsi+1)*(dsj+1) + j*(dsi+1) + (i-1)]);
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 2] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[k*(dsi+1)*(dsj+1) + (j-1)*(dsi+1) + i]);
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 3] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[k*(dsi+1)*(dsj+1) + (j-1)*(dsi+1) + (i-1)]);


        // reverse Z traversal of the BCC vertices below the current iBCC node
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 4] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[(k-1)*(dsi+1)*(dsj+1) + j*(dsi+1) + i]);
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 5] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[(k-1)*(dsi+1)*(dsj+1) + j*(dsi+1) + (i-1)]);
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 6] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[(k-1)*(dsi+1)*(dsj+1) + (j-1)*(dsi+1) + i]);
        d_edges[ik*dsi*dsj*8 + ij*dsi*8 + ii*8 + 7] = 
          InsertEdge(
            iBCC[ik*dsi*dsj + ij*dsi + ii], 
            BCC[(k-1)*(dsi+1)*(dsj+1) + (j-1)*(dsi+1) + (i-1)]);
      }
    }
  }


  for(k=1;k<dsk-2;k++){
    //      std::cout << "k=" << k << "(" << dsk-2 << ")" << std::endl;
    for(j=1;j<dsj-2;j++){
      //        std::cout << "j=" << k << "(" << dsj-2 << ")" << std::endl;
      for(i=1;i<dsi-2;i++){
        //          std::cout << "i=" << k << "(" << dsi-2 << ")" << std::endl;
        // 4 tetrahedra each sharing one edge of the 
        // iBCC cube
        RGMTetra_ptr new_tetra;

        // iBCC_edges[0]
        // edge 0 is opposite to 1, 
        // 2 -- to 3, and 4 -- to 5
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 0],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 0],   
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 5],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 1],   
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 4]);   
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 0],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 0],
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 6],
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 4],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 2]);
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + (j-1)*dsi*3 + i*3 + 1],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 0],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 2],
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 7],
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 6],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 3]);

        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + (i-1)*3 + 2],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 0],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 1],
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 7],
          d_edges[(k+1)*dsi*dsj*8 + j*dsi*8 + i*8 + 5],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 3]);

        // iBCC_edges[1]
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 0],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 0],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 5],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 1],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 4]);
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 0],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 3],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 1],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 2]);
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + (j-1)*dsi*3 + i*3 + 0],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 2],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 7],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 3],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 6]);
        new_tetra = InsertTetraOriented(
          BCC_edges[(k-1)*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 4],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 7],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + (i+1)*8 + 5],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 6]);

        // iBCC_edges[2]
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 0],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 0],   
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 6],
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 2],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 4]);
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 0],   
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 3],
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 2],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 1]);
        new_tetra = InsertTetraOriented(
          BCC_edges[k*dsi*dsj*3 + j*dsi*3 + (i-1)*3 + 0],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 1],   
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 7],
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 3],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 5]);
        new_tetra = InsertTetraOriented(
          BCC_edges[(k-1)*dsi*dsj*3 + j*dsi*3 + i*3 + 1],
          iBCC_edges[k*dsi*dsj*3 + j*dsi*3 + i*3 + 2],
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 4],   
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 7],
          d_edges[k*dsi*dsj*8 + (j+1)*dsi*8 + i*8 + 6],   
          d_edges[k*dsi*dsj*8 + j*dsi*8 + i*8 + 5]);
      }
    }
  }

  delete [] BCC;
  delete [] iBCC;
  delete [] BCC_edges;
  delete [] iBCC_edges;
  delete [] d_edges;
}


/* Initial BCC spacing together with the number of resolutions ultimately
 * identify the final size of the mesh. It is also important to correctly
 * find the BCC spacing in order to represent all features of the mesh. */
template<class TInputImage, class TOutputMesh>
double
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::FindBCCSpacing(){
  double spacing;
  spacing = fmin(m_dimX, m_dimY);
  spacing = fmin(spacing, m_dimZ);
  return spacing/m_BCCSpacing;
}

/* Inserts a vertex */
template<class TInputImage, class TOutputMesh>
typename BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>::RGMVertex_ptr
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::InsertVertex(double cx, double cy, double cz){
  RGMVertex_ptr new_vertex = new RGMVertex;
  new_vertex->coords[0] = cx;
  new_vertex->coords[1] = cy;
  new_vertex->coords[2] = cz;
  m_Vertices.push_back(new_vertex);
  return new_vertex;
}

/* Inserts an edge */
template<class TInputImage, class TOutputMesh>
typename BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>::RGMEdge_ptr
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::InsertEdge(RGMVertex_ptr v0, RGMVertex_ptr v1){
  RGMEdge_ptr new_edge = new RGMEdge;
  new_edge->nodes[0] = v0;  
  new_edge->nodes[1] = v1;
  new_edge->children[0] = NULL;
  new_edge->children[0] = NULL;
  new_edge->midv = NULL;
  m_Edges.push_back(new_edge);
  return new_edge;
}

/* Insert tetrahedron, do the orientation check */
/* WARNING: only for the 0th level BCC!!! */
template<class TInputImage, class TOutputMesh>
typename BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>::RGMTetra_ptr
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::InsertTetraOriented(RGMEdge_ptr e0, RGMEdge_ptr e1, RGMEdge_ptr e2,
  RGMEdge_ptr e3, RGMEdge_ptr e4, RGMEdge_ptr e5){
  // Take e0 edge, edge e1 is opposite to e0.
  // Convention: imagine e0->nodes[0] as the front-most node of the
  // tetrahedron, then we want to order two edges sharing this vertex in
  // clockwise direction, identify their orientation (positive if
  // e_i->nodes[0] == e0->nodes[0]. For the edges of the face opposite to
  // e0->nodes[0] want to have them in clockwise direction too, same for
  // orientation.
  int i, j;
  RGMTetra_ptr new_tetra = new RGMTetra;

  new_tetra->parent = NULL;
  new_tetra->subdiv = 0;
  new_tetra->edges_split = 0;
  new_tetra->edges_orient = 0;
  new_tetra->level = 0;
  new_tetra->greens = NULL;
  new_tetra->num_greens = 0;

  new_tetra->edges[0] = e0;
  e0->neihood.push_back(new_tetra);
  new_tetra->edges[1] = e1;
  e1->neihood.push_back(new_tetra);
  new_tetra->edges[2] = e2;
  e2->neihood.push_back(new_tetra);
  new_tetra->edges[3] = e3;
  e3->neihood.push_back(new_tetra);
  new_tetra->edges[4] = e4;
  e4->neihood.push_back(new_tetra);
  new_tetra->edges[5] = e5;
  e5->neihood.push_back(new_tetra);

#ifdef DO_CHECKS
  for(i=0;i<6;i+=2){
    assert(new_tetra->edges[i]->nodes[0] !=
      new_tetra->edges[i+1]->nodes[0]);
    assert(new_tetra->edges[i]->nodes[0] !=
      new_tetra->edges[i+1]->nodes[1]);
    assert(new_tetra->edges[i]->nodes[1] !=
      new_tetra->edges[i+1]->nodes[0]);
    if(new_tetra->edges[i]->nodes[1] ==
      new_tetra->edges[i+1]->nodes[1]){
      assert(0);
    }
  }
#endif // DO_CHECKS


  if(e2->nodes[0] != e0->nodes[0] &&
    e2->nodes[1] != e0->nodes[0]){
    // swap e2 and e3
    new_tetra->edges[2] = e3;
    new_tetra->edges[3] = e2;
  }
  if(e4->nodes[0] != e0->nodes[0] &&
    e4->nodes[1] != e0->nodes[0]){
    new_tetra->edges[4] = e5;
    new_tetra->edges[5] = e4;
  }

  // next we need to know the orientation of each of the edges relative to e0
  if(orient3d(&e0->nodes[0]->coords[0], &e0->nodes[1]->coords[0], 
      &e1->nodes[0]->coords[0], &e1->nodes[1]->coords[0])>0){ // DEST(e0)+e1 are in CW
    // e2 should share ORIG(e1)
    if(new_tetra->edges[4]->nodes[0] != e1->nodes[1] &&
      new_tetra->edges[4]->nodes[1] != e1->nodes[1]){
      // swap second with third pair of tetrahedron edges
      RGMEdge_ptr e_tmp;
      e_tmp = new_tetra->edges[2];
      new_tetra->edges[2] = new_tetra->edges[4];
      new_tetra->edges[4] = e_tmp;
      e_tmp = new_tetra->edges[3];
      new_tetra->edges[3] = new_tetra->edges[5];
      new_tetra->edges[5] = e_tmp;
    }
    if(new_tetra->edges[1]->nodes[1] != new_tetra->edges[3]->nodes[0])
      new_tetra->edges_orient |= (1<<3);
    if(new_tetra->edges[1]->nodes[1] != new_tetra->edges[5]->nodes[1])
      new_tetra->edges_orient |= (1<<5);
  } else {
    //std::cout << "orient3d()<0" << std::endl;
    new_tetra->edges_orient |= 2;
    // e2 should share DEST(e1)
    if(new_tetra->edges[4]->nodes[0] != e1->nodes[0] &&
      new_tetra->edges[4]->nodes[1] != e1->nodes[0]){
      // swap second with third pair of tetrahedron edges
      RGMEdge_ptr e_tmp;;
      e_tmp = new_tetra->edges[2];
      new_tetra->edges[2] = new_tetra->edges[4];
      new_tetra->edges[4] = e_tmp;
      e_tmp = new_tetra->edges[3];
      new_tetra->edges[3] = new_tetra->edges[5];
      new_tetra->edges[5] = e_tmp;
    }
    if(new_tetra->edges[1]->nodes[0] != new_tetra->edges[3]->nodes[0])
      new_tetra->edges_orient |= (1<<3);
    if(new_tetra->edges[1]->nodes[1] != new_tetra->edges[5]->nodes[1])
      new_tetra->edges_orient |= (1<<5);
  }
  // Now all edges of new_tetra are ordered:
  // e0, e2, and e4 share ORIG(e0) and go in CW,
  // and e1, e3, and e5 are in CW order. 

  // Find the orientation of each of the edges.
  // Edges sharing a node with e0 and have same 
  // ORIG that e0 have orientation 0.
  // Edges of the face opposite to ORIG(e0) and go in CW
  // have orientation 0. 
  // Otherwise orientation is 1.
  if(new_tetra->edges[0]->nodes[0] != new_tetra->edges[2]->nodes[0])
    new_tetra->edges_orient |= (1<<2);
  if(new_tetra->edges[0]->nodes[0] != new_tetra->edges[4]->nodes[0])
    new_tetra->edges_orient |= (1<<4);

  new_tetra->parent = NULL;
  new_tetra->subdiv = 0;
  new_tetra->edges_split = 0;

#ifdef VERBOSE
  std::cout << "Tetra #" << new_tetra->id << ": " <<
    new_tetra->edges[0] << "," << new_tetra->edges[1] << "," <<
    new_tetra->edges[2] << "," << new_tetra->edges[3] << "," <<
    new_tetra->edges[4] << "," << new_tetra->edges[5] << std::endl;
#endif

//  if(res>=m_current_level)
    m_Tetras.push_back(new_tetra);
//  else
//    m_pending_tetras.push_back(new_tetra);

  return new_tetra;
}

/* Go through the list of tetrahedra, and remove those which are completely
 * outside of the object's surface */
template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::RemoveBCCTetra(RGMTetra_ptr thisT){
  // This operation is safe, because tetrahedra do not have any descendants
  // yet. We go trhough all the edges and remove thisT from the
  // neighbourhoods. Edges with 0 neighbourhood lists can be deallocated.
  // Search is O(c) operation, because it's possible to bound the maximum
  // number of tets incident on an edge.
  for(int i=0;i<6;i++){
    typename std::list<RGMTetra_ptr>::iterator enI =
      find(thisT->edges[i]->neihood.begin(),
        thisT->edges[i]->neihood.end(), thisT);
    thisT->edges[i]->neihood.erase(enI);
    if(!thisT->edges[i]->neihood.size())
      delete thisT->edges[i];
  }
  delete thisT;
}

/* Return true if the tetrahedron has to be split based on the user specified
 * criteria */
template<class TInputImage, class TOutputMesh>
bool
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::TetraUnderrefined(RGMTetra_ptr thisT){
  if(m_SubdivInclusion)
    if(SubdivInclusionTest(thisT))
      return true;
 
  for(typename std::list<SubdivisionTestFunctionPointer>::iterator
    scI=m_SubdivisionCriteria.begin();
    scI!=m_SubdivisionCriteria.end();
    scI++)
    if((*scI)(thisT->edges[0]->nodes[0]->coords, 
        thisT->edges[0]->nodes[1]->coords,
        thisT->edges[1]->nodes[0]->coords, 
        thisT->edges[1]->nodes[1]->coords, this))
      return true;
  
  if(m_SubdivCurvature)
    if(SubdivCurvatureTest(thisT))
      return true;

  return false;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::FinalizeRedTetra(RGMTetra_ptr thisT){
  // 12 new face edges
  int i, j, k, face_id;
  static RGMTetra_ptr previousTetra = NULL;
  std::set<RGMVertex_ptr> vertices_here;
  RGMEdge_ptr  new_edge;
  RGMEdge_ptr  face_edges[4][3];
  RGMEdge_ptr  internal_edge;
  RGMTetra_ptr orig_tetra_ptr = thisT;
  typename std::map<std::pair<RGMVertex_ptr,RGMVertex_ptr>,RGMEdge_ptr,ltVertexPair>::iterator mI;

  thisT->edges_split = 0x3F;

  // mark the tetrahedron and split each of its edges
  for(i=0;i<6;i++){

    if(!thisT->edges[i]->midv){
      SplitEdge(thisT->edges[i]);
    } else{
      assert(thisT->edges[i]->children[0]);
      assert(thisT->edges[i]->children[1]);
    }

    if(!thisT->edges[i]->children[0]){
      std::cout << "FAILURE, i=" << i << std::endl;
      assert(0);
    }
    assert(thisT->edges[i]->children[1]);
  } // for(all edges of thisT)

  if(!thisT->edges[1]->children[0]){
    std::cout << "FAILURE, i=" << i << std::endl;
    assert(0);
  }

  // choose the internal edge so that it is the shortest out of three
  // possible choices. This will create new tetras which are exactly those
  // as BCC tetras resulting from a mesh with cells 1/2 the size! [Molino]
  double internal_edge_length[3];
  internal_edge_length[0] = DistanceBwPoints(thisT->edges[0]->midv->coords, thisT->edges[1]->midv->coords);
  internal_edge_length[1] = DistanceBwPoints(thisT->edges[2]->midv->coords, thisT->edges[3]->midv->coords);
  internal_edge_length[2] = DistanceBwPoints(thisT->edges[4]->midv->coords, thisT->edges[5]->midv->coords);
  if(internal_edge_length[0]>internal_edge_length[1] &&
    internal_edge_length[0]>internal_edge_length[2]){
    RGMTetra rT;
    if(internal_edge_length[1]<internal_edge_length[2]){
      // reorder so that pair 2-3 is first
      rT.edges[0] = thisT->edges[2];
      rT.edges[1] = thisT->edges[3];
      rT.edges[2] = thisT->edges[4];
      rT.edges[3] = thisT->edges[5];
      rT.edges[4] = thisT->edges[0];
      rT.edges[5] = thisT->edges[1];
      rT.edges_orient = GetTetraEdgeOrient(&rT);
    } else {
      // reorder so that pair 4-5 is first
      rT.edges[0] = thisT->edges[4];
      rT.edges[1] = thisT->edges[5];
      rT.edges[2] = thisT->edges[0];
      rT.edges[3] = thisT->edges[1];
      rT.edges[4] = thisT->edges[2];
      rT.edges[5] = thisT->edges[3];
      rT.edges_orient = GetTetraEdgeOrient(&rT);
    }
    /*
    std::cout << "Need to reorder edges..." << std::endl;
    std::cout << "Lengths: " << internal_edge_length[0] << " ";
    std::cout << internal_edge_length[1] << " " << internal_edge_length[2] << std::endl;
    std::cout << "Tetra before: ";
     */
    thisT->edges[0] = rT.edges[0];
    thisT->edges[1] = rT.edges[1];
    thisT->edges[2] = rT.edges[2];
    thisT->edges[3] = rT.edges[3];
    thisT->edges[4] = rT.edges[4];
    thisT->edges[5] = rT.edges[5];
    thisT->edges_orient = rT.edges_orient;
  }
  internal_edge = InsertEdge(thisT->edges[0]->midv, thisT->edges[1]->midv);

  // 2) create 3 new edges for each of the faces, and put
  // them in a hashtable (order vertices of an edge new by 
  // pointer to facilitate lookup)
  // go through the faces
  for(face_id=0;face_id<4;face_id++){
    std::pair<RGMVertex_ptr,RGMVertex_ptr>  new_face_edge;
    unsigned char face_midv[3];
    // face_edges_order keeps all face edges listed in CW direction,
    // with the first edge opposing ORIG(e0) on the faces incident to 
    // ORIG(e0), and with the first edge parallel to e1 on the face opposite
    // to ORIG(e0)
    unsigned char face_edges_order[3] = {0,1,2};
    unsigned char tmp_midv;

    face_midv[0] = m_FaceToEdgesLT[face_id*3+0];
    face_midv[1] = m_FaceToEdgesLT[face_id*3+1];
    face_midv[2] = m_FaceToEdgesLT[face_id*3+2];

    // sort the face edges by the address of the midsection point
    if(thisT->edges[face_midv[0]]->midv <
      thisT->edges[face_midv[1]]->midv){   
      if(thisT->edges[face_midv[0]]->midv <
        thisT->edges[face_midv[2]]->midv){ 
        if(thisT->edges[face_midv[1]]->midv <
          thisT->edges[face_midv[2]]->midv){
          // do nothing
          // order of midpoints is {e0m, e2m, e5m}  {e4m, e0m, e3m}
          // edges are {e0m->e2m, e0m->e5m, e2m->e5m} {e4m->e0m, e4m->e3m, e0m->e3m}
          // order of face edges is {0,2,1}
          face_edges_order[1] = 2;
          face_edges_order[2] = 1;
        } else {  // b>=c
          // order of midpoints is {e0m, e5m, e2m} {e4m,e3m,e0m}
          // edges are {e0m->e5m, e0m->e2m, e5m->e2m} {e4m->e3m,e4m->e0m,e3m->e0m}
          // order of edges is {2,0,1}
          tmp_midv = face_midv[2];
          face_midv[2] = face_midv[1];
          face_midv[1] = tmp_midv;
          face_edges_order[0] = 2;
          face_edges_order[1] = 0;
          face_edges_order[2] = 1;
        }
      } else { // a>=c
        // order of midpoints is {e5m, e0m, e2m}
        // edges are {e5m->e0m, e5m->e2m, e0m->e2m}
        // order of edges is {2,1,0}
        tmp_midv = face_midv[0];
        face_midv[0] = face_midv[2];
        face_midv[2] = face_midv[1];
        face_midv[1] = tmp_midv;
        face_edges_order[0] = 2;
        face_edges_order[1] = 1;
        face_edges_order[2] = 0;
      }
    } else { // a>=b
      if(thisT->edges[face_midv[1]]->midv <
        thisT->edges[face_midv[2]]->midv){
        if(thisT->edges[face_midv[0]]->midv <
          thisT->edges[face_midv[2]]->midv){
          // order of midpoints is {e2m, e0m, e5m}
          // edges are {e2m->e0m, e2m->e5m, e0m->e5m}
          // order of edges is {0,1,2}
          tmp_midv = face_midv[0];
          face_midv[0] = face_midv[1];
          face_midv[1] = tmp_midv;
          face_edges_order[0] = 0;
          face_edges_order[1] = 1;
          face_edges_order[2] = 2;            
        } else { // a>=c
          // order of midpoints is {e2m, e5m, e0m}
          // edges are {e2m->e5m, e2m->e0m, e5m->e0m}
          // order is {1,0,2}
          tmp_midv = face_midv[0];
          face_midv[0] = face_midv[1];
          face_midv[1] = face_midv[2];
          face_midv[2] = tmp_midv;
          face_edges_order[0] = 1;
          face_edges_order[1] = 0;
          face_edges_order[2] = 2;            
        }
      } else { // b>=c
        // order of midpoints is {e5m, e2m, e0m} {e3m,e0m,e4m}
        // edges are {e5m->e2m, e5m->e0m, e2m->e0m} {e3m->e0m,e3m->e4m,e0m->e4m}
        // order is {2,1,0} {1,2,0}
        tmp_midv = face_midv[0];
        face_midv[0] = face_midv[2];
        face_midv[2] = tmp_midv;
        face_edges_order[0] = 1;    // this means, 0th edge which is 0->1 goes to the 2nd position
        face_edges_order[1] = 2;
        face_edges_order[2] = 0;            
      }
    }

    if(!thisT->edges[1]->children[0]){
      std::cout << "FAILURE! face_id=" << face_id << std::endl;
      assert(0);
    }

    face_edges[face_id][face_edges_order[0]] = 
      GetPutTmpEdge(thisT->edges[face_midv[0]]->midv, 
        thisT->edges[face_midv[1]]->midv);
    face_edges[face_id][face_edges_order[1]] = 
      GetPutTmpEdge(thisT->edges[face_midv[0]]->midv, 
        thisT->edges[face_midv[2]]->midv);
    face_edges[face_id][face_edges_order[2]] = 
      GetPutTmpEdge(thisT->edges[face_midv[1]]->midv, 
        thisT->edges[face_midv[2]]->midv);
  }

  // Finally, create internal tetrahedra...
  bool edge_orientations[6];
  for(i=0;i<6;i++)
    edge_orientations[i] = (thisT->edges_orient >> i) & 1;

  // By convention, children edges preserve parent orientation, and 
  // the first child is (ORIG(e) --> e->midv);
  // we also know the orientation of parent edges
  // AND we also know the relative positions of the new
  // edges, so that we don't need to reshuffle the edges'
  // pairs for new tetrahedra

  RGMTetra_ptr new_tetra;

  // Child #0
  // ORIG(e0) + 3 midpoints of edges incident to it
  new_tetra = InsertTetra(thisT->level+1,
    thisT->edges[0]->children[0],                    // first child of e0
    face_edges[1][0],                                // first edge in second face
    thisT->edges[2]->children[edge_orientations[2]], // child of e1
    face_edges[2][0],
    thisT->edges[4]->children[edge_orientations[4]],
    face_edges[0][0]);

  // orientation of edges that have parents is inherited from the parents
  new_tetra->edges_orient = (((unsigned char)edge_orientations[2])<<2) | 
    (((unsigned char)edge_orientations[4])<<4);
  // orientations of other edges has to be identified
  if(face_edges[1][0]->nodes[0] != thisT->edges[2]->midv)
    new_tetra->edges_orient |= (1<<1);
  if(face_edges[2][0]->nodes[0] != thisT->edges[4]->midv)
    new_tetra->edges_orient |= (1<<3);
  if(face_edges[0][0]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<5);

  // Child #1
  // DEST(e0) + 3 midpoints of edges incident to it
  new_tetra = InsertTetra(thisT->level+1,
    thisT->edges[0]->children[1], face_edges[3][0],
    face_edges[0][2], thisT->edges[3]->children[!edge_orientations[3]],
    face_edges[2][1], thisT->edges[5]->children[edge_orientations[5]]);

  new_tetra->edges_orient = (((unsigned char)edge_orientations[3])<<3) | 
    (((unsigned char)edge_orientations[5])<<5);
  if(face_edges[3][0]->nodes[0] != thisT->edges[5]->midv)
    new_tetra->edges_orient |= (1<<1);
  if(face_edges[0][2]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<2);
  if(face_edges[2][1]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<4);

  // Child #2
  // DEST(e2) + 3 midpoints of edges incident to it
  assert(thisT->edges[1]->children[0]);
  assert(thisT->edges[1]->children[1]);
  new_tetra = InsertTetra(thisT->level+1,
    face_edges[0][1], thisT->edges[1]->children[edge_orientations[1]],
    thisT->edges[2]->children[!edge_orientations[2]], face_edges[3][1],
    face_edges[1][2], thisT->edges[5]->children[!edge_orientations[5]]);

  if(new_tetra->edges[0]->nodes[0] == thisT->edges[2]->midv){
    new_tetra->edges_orient = 
      (((unsigned char)edge_orientations[1])<<1) | 
      (((unsigned char)edge_orientations[2])<<2) |
      (((unsigned char)edge_orientations[5])<<5);

    if(new_tetra->edges[3]->nodes[0] != thisT->edges[1]->midv)
      new_tetra->edges_orient |= (1<<3);
    if(new_tetra->edges[4]->nodes[0] != thisT->edges[2]->midv)
      new_tetra->edges_orient |= (1<<4);
  } else {
    RGMEdge_ptr tmp_edge;

    tmp_edge = new_tetra->edges[2];
    new_tetra->edges[2] = new_tetra->edges[3];
    new_tetra->edges[3] = tmp_edge;
    tmp_edge = new_tetra->edges[4];
    new_tetra->edges[4] = new_tetra->edges[5];
    new_tetra->edges[5] = tmp_edge;

    if(new_tetra->edges[1]->nodes[0] != thisT->edges[1]->midv)
      new_tetra->edges_orient |= (1<<1);
    if(new_tetra->edges[2]->nodes[0] != thisT->edges[5]->midv)
      new_tetra->edges_orient |= (1<<2);
    if(new_tetra->edges[3]->nodes[1] != thisT->edges[2]->midv)
      new_tetra->edges_orient |= (1<<3);
    if(new_tetra->edges[4]->nodes[0] != thisT->edges[5]->midv)
      new_tetra->edges_orient |= (1<<4);
    if(new_tetra->edges[5]->nodes[0] != thisT->edges[2]->midv)
      new_tetra->edges_orient |= (1<<5);
  }

  // Child #3
  // DEST(e4) + 3 midpoints of edges incident to it
  new_tetra = InsertTetra(thisT->level+1,
    face_edges[2][2], thisT->edges[1]->children[!edge_orientations[1]],
    face_edges[1][1], thisT->edges[3]->children[edge_orientations[3]],
    thisT->edges[4]->children[!edge_orientations[4]], face_edges[3][2]);

  if(new_tetra->edges[0]->nodes[0] == thisT->edges[4]->midv){
    new_tetra->edges_orient = (((unsigned char)edge_orientations[1])<<1) | 
      (((unsigned char)edge_orientations[3])<<3) |
      (((unsigned char)edge_orientations[4])<<4);

    if(new_tetra->edges[2]->nodes[0] != thisT->edges[4]->midv)
      new_tetra->edges_orient |= (1<<2);
    if(new_tetra->edges[5]->nodes[0] != thisT->edges[3]->midv)
      new_tetra->edges_orient |= (1<<5);
  } else {
    RGMEdge_ptr tmp_edge;

    tmp_edge = new_tetra->edges[2];
    new_tetra->edges[2] = new_tetra->edges[3];
    new_tetra->edges[3] = tmp_edge;
    tmp_edge = new_tetra->edges[4];
    new_tetra->edges[4] = new_tetra->edges[5];
    new_tetra->edges[5] = tmp_edge;

    if(new_tetra->edges[1]->nodes[1] != thisT->edges[1]->midv)
      new_tetra->edges_orient |= (1<<1);
    if(new_tetra->edges[2]->nodes[0] != thisT->edges[3]->midv)
      new_tetra->edges_orient |= (1<<2);
    if(new_tetra->edges[3]->nodes[0] != thisT->edges[1]->midv)
      new_tetra->edges_orient |= (1<<3);
    if(new_tetra->edges[4]->nodes[0] != thisT->edges[3]->midv)
      new_tetra->edges_orient |= (1<<4);
    if(new_tetra->edges[5]->nodes[0] != thisT->edges[4]->midv)
      new_tetra->edges_orient |= (1<<5);
  }

  // Child #4
  // internal_edge + midpoints of e2 and e4
  new_tetra = InsertTetra(thisT->level+1, 
    internal_edge, face_edges[1][0],
    face_edges[0][0], face_edges[1][1],
    face_edges[2][0], face_edges[1][2]);
  new_tetra->level = thisT->level+1;

  if(face_edges[1][0]->nodes[0] != thisT->edges[2]->midv)
    new_tetra->edges_orient |= (1<<1);
  if(face_edges[0][0]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<2);
  if(face_edges[1][1]->nodes[0] != thisT->edges[4]->midv)
    new_tetra->edges_orient |= (1<<3);
  if(face_edges[2][0]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<4);
  if(face_edges[1][2]->nodes[0] != thisT->edges[1]->midv)
    new_tetra->edges_orient |= (1<<5);

  // Child #5
  // internal_edge + midpoints of e4 and e3
  new_tetra = InsertTetra(thisT->level+1,
    internal_edge, face_edges[2][2],
    face_edges[2][0], face_edges[3][2],
    face_edges[2][1], face_edges[1][1]);
  new_tetra->level = thisT->level+1;

  if(face_edges[2][2]->nodes[0] != thisT->edges[4]->midv)
    new_tetra->edges_orient |= (1<<1);
  if(face_edges[2][0]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<2);
  if(face_edges[3][2]->nodes[0] != thisT->edges[3]->midv)
    new_tetra->edges_orient |= (1<<3);
  if(face_edges[2][1]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<4);
  if(face_edges[1][1]->nodes[0] != thisT->edges[1]->midv)
    new_tetra->edges_orient |= (1<<5);

  // Child #6
  // internal_edge + midpoints of e3 and e5
  new_tetra = InsertTetra(thisT->level+1,
    internal_edge, face_edges[3][0],
    face_edges[2][1], face_edges[3][1],
    face_edges[0][2], face_edges[3][2]);
  new_tetra->level = thisT->level+1;

  if(face_edges[3][0]->nodes[0] != thisT->edges[3]->midv)
    new_tetra->edges_orient |= (1<<1);
  if(face_edges[2][1]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<2);
  if(face_edges[3][1]->nodes[0] != thisT->edges[5]->midv)
    new_tetra->edges_orient |= (1<<3);
  if(face_edges[0][2]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<4);
  if(face_edges[3][2]->nodes[0] != thisT->edges[1]->midv)
    new_tetra->edges_orient |= (1<<5);

  // Child #7
  // internal_edge + midpoints of e2 and e5
  new_tetra = InsertTetra(thisT->level+1,
    internal_edge, face_edges[0][1],
    face_edges[0][2], face_edges[1][2],
    face_edges[0][0], face_edges[3][1]);
  new_tetra->level = thisT->level+1;

  if(face_edges[0][1]->nodes[0] != thisT->edges[5]->midv)
    new_tetra->edges_orient |= (1<<1);
  if(face_edges[0][2]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<2);
  if(face_edges[1][2]->nodes[0] != thisT->edges[2]->midv)
    new_tetra->edges_orient |= (1<<3);
  if(face_edges[0][0]->nodes[0] != thisT->edges[0]->midv)
    new_tetra->edges_orient |= (1<<4);
  if(face_edges[3][1]->nodes[0] != thisT->edges[1]->midv)
    new_tetra->edges_orient |= (1<<5);

  RemoveTetra(thisT);
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::FinalizeGreenTetra(RGMTetra_ptr thisT){
  int split_edges_ids[3];
  int edges_split_total;
  int first_split_edge;
  int split_edge_orient, opposite_edge;
  int i, j, k;
  unsigned num_children;

  assert(thisT->greens==NULL);
  assert(thisT->parent==NULL);

  edges_split_total = 0;
  first_split_edge = -1;
  // identify the configuration
  for(i=0;i<6;i++){
    if(thisT->edges[i]->midv){
      split_edges_ids[edges_split_total++] = i;
      if(first_split_edge==-1)
        first_split_edge = i;
      thisT->edges_split |= 1<<i;
    }
  }
  //    assert(edges_split_total==TetraEdgeConf(thisT));

  opposite_edge = first_split_edge ^ 1;
  split_edge_orient = (thisT->edges_orient>>first_split_edge) & 1;

  switch(edges_split_total){
  case 0:{
    assert(0);
    break;
  }
  case 1:{
    //std::cout << "Green: 1" << std::endl;
    // need to create two face edges, which connect the midpoint with the
    // vertices of the opposite edge (unless they have already been
    // created). Flip the last bit to get the opposite edge.
    RGMEdge_ptr face_edges[2];
    RGMTetra_ptr new_tetra;
    RGMVertex_ptr v0ptr, v1ptr;

    num_children = 2;
    thisT->greens = new RGMTetra[2];

      face_edges[0] = GetPutTmpEdge(thisT->edges[opposite_edge]->nodes[0],
        thisT->edges[first_split_edge]->midv);
      face_edges[1] = GetPutTmpEdge(thisT->edges[opposite_edge]->nodes[1],
        thisT->edges[first_split_edge]->midv);

      // disregard order of edges, because GREEN children will not be split
      new_tetra = InsertTetraAllocated(thisT->level+1,
        thisT->edges[first_split_edge]->children[0], thisT->edges[opposite_edge],
        thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+first_split_edge*2]], face_edges[0],
        thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+first_split_edge*2+1]], face_edges[1],
        &thisT->greens[0]);
      new_tetra->parent = thisT;

    new_tetra = InsertTetraAllocated(thisT->level+1,
      thisT->edges[first_split_edge]->children[1], thisT->edges[opposite_edge],
      face_edges[0], thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+first_split_edge*2]],
      face_edges[1], thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+first_split_edge*2+1]],
      &thisT->greens[1]);
    new_tetra->parent = thisT;

    thisT->num_greens = 2;
    break;
  }
  case 2:{
    //std::cout << "Green:2" << std::endl;
    RGMEdge_ptr face_edges[4];
    RGMEdge_ptr internal_edge;
    RGMTetra_ptr new_tetra;
    int opposite_edge = first_split_edge ^ 1;
    RGMVertex_ptr v0ptr, v1ptr;
    int fse_cw_nei; // first_split_edge CW neighbor ID


    num_children = 4;
    thisT->greens = new RGMTetra[4];

    if(split_edges_ids[0] != (split_edges_ids[1]^1))
      assert(0);

    assert(thisT->edges[first_split_edge]->children[0]);
    assert(thisT->edges[opposite_edge]->children[0]);

    face_edges[0] = GetPutTmpEdge(thisT->edges[opposite_edge]->nodes[0],
      thisT->edges[first_split_edge]->midv);
    face_edges[1] = GetPutTmpEdge(thisT->edges[opposite_edge]->nodes[1],
      thisT->edges[first_split_edge]->midv);
    face_edges[2] = GetPutTmpEdge(thisT->edges[first_split_edge]->nodes[0],
      thisT->edges[opposite_edge]->midv);
    face_edges[3] = GetPutTmpEdge(thisT->edges[first_split_edge]->nodes[1],
      thisT->edges[opposite_edge]->midv);

    internal_edge = InsertEdge(thisT->edges[first_split_edge]->midv,
      thisT->edges[opposite_edge]->midv);

    if( (((thisT->edges_orient)>>first_split_edge)&1) ^ // this is the orientation of opposite_edge 
      (((thisT->edges_orient)>>opposite_edge)&1) ){   //   relative to first_split_edge

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+first_split_edge*2]],
        face_edges[2], face_edges[1],
        thisT->edges[opposite_edge]->children[1], thisT->edges[first_split_edge]->children[0],
        &thisT->greens[0]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+first_split_edge*2+1]],
        thisT->edges[opposite_edge]->children[0], thisT->edges[first_split_edge]->children[0],
        face_edges[2], face_edges[0],
        &thisT->greens[1]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+first_split_edge*2]],
        face_edges[3], face_edges[0],
        thisT->edges[opposite_edge]->children[0], thisT->edges[first_split_edge]->children[1],
        &thisT->greens[2]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+first_split_edge*2+1]],
        thisT->edges[opposite_edge]->children[1], thisT->edges[first_split_edge]->children[1],
        face_edges[3], face_edges[1],
        &thisT->greens[3]);
      new_tetra->parent = thisT;
    } else {
      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+first_split_edge*2]],
        face_edges[2], face_edges[0],
        thisT->edges[opposite_edge]->children[0], thisT->edges[first_split_edge]->children[0],
        &thisT->greens[0]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+first_split_edge*2+1]],
        thisT->edges[opposite_edge]->children[1], thisT->edges[first_split_edge]->children[0],
        face_edges[2], face_edges[1],
        &thisT->greens[1]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+first_split_edge*2]],
        face_edges[3], face_edges[1],
        thisT->edges[opposite_edge]->children[1], thisT->edges[first_split_edge]->children[1],
        &thisT->greens[2]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        internal_edge, thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+first_split_edge*2+1]],
        thisT->edges[opposite_edge]->children[0], thisT->edges[first_split_edge]->children[1],
        face_edges[3], face_edges[0],
        &thisT->greens[3]);
      new_tetra->parent = thisT;
    }
    thisT->num_greens = 4;
    break;
  }
  case 3:{
    //std::cout << "Green: 3" << std::endl;
    RGMEdge_ptr long_face_edges[3];
    RGMEdge_ptr short_face_edges[3];
    RGMEdge_ptr split_edges[3];
    RGMTetra_ptr new_tetra;
    int opposite_edge = first_split_edge ^ 1;
    RGMVertex_ptr opposite_vertex;

    num_children = 4;
    assert(split_edges_ids[0] == 
      m_ThirdFaceEdgeLT[split_edges_ids[1]*6+split_edges_ids[2]]);

    thisT->greens = new RGMTetra[4];

    // which 3 edges are split?
    split_edges_ids[0] = first_split_edge;
    split_edges[0] = thisT->edges[first_split_edge];
    split_edges[1] = thisT->edges[split_edges_ids[1]];
    split_edges[2] = thisT->edges[split_edges_ids[2]];

    RGMTetra tmp_tetra;
    bool tmp_orientations[6];

    tmp_tetra.edges[0] = thisT->edges[split_edges_ids[0]];
    tmp_tetra.edges[1] = thisT->edges[split_edges_ids[0]^1];

    tmp_tetra.edges[2] = thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+split_edges_ids[0]*2]];
    tmp_tetra.edges[3] = thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+split_edges_ids[0]*2]];
    tmp_tetra.edges[4] = thisT->edges[m_IncidentEdgesLT[12*split_edge_orient+split_edges_ids[0]*2+1]];
    tmp_tetra.edges[5] = thisT->edges[m_IncidentEdgesLT[12*(!split_edge_orient)+split_edges_ids[0]*2+1]];

    tmp_tetra.edges_split = 0;
    tmp_tetra.parent = NULL;

    tmp_orientations[1] = ((thisT->edges_orient>>split_edges_ids[0])&1) ^
      ((thisT->edges_orient>>split_edges_ids[1])&1);
    tmp_orientations[2] = !(tmp_tetra.edges[2]->nodes[0]==tmp_tetra.edges[0]->nodes[0]);
    tmp_orientations[3] = !(tmp_tetra.edges[3]->nodes[1]==tmp_tetra.edges[0]->nodes[1]);
    tmp_orientations[4] = !(tmp_tetra.edges[4]->nodes[0]==tmp_tetra.edges[0]->nodes[0]);
    tmp_orientations[5] = !(tmp_tetra.edges[5]->nodes[0]==tmp_tetra.edges[0]->nodes[1]);
    
    tmp_tetra.edges_orient = 0;
    for(j=0;j<6;j++)
      tmp_tetra.edges_orient |= (tmp_orientations[j]<<j);

    if(tmp_tetra.edges[2]->midv){

      assert(tmp_tetra.edges[2]->midv);
      assert(tmp_tetra.edges[5]->midv);

      opposite_vertex = tmp_tetra.edges[4]->nodes[!tmp_orientations[4]];

      long_face_edges[0] = GetPutTmpEdge(opposite_vertex, tmp_tetra.edges[0]->midv);
      long_face_edges[1] = GetPutTmpEdge(opposite_vertex, tmp_tetra.edges[2]->midv);
      long_face_edges[2] = GetPutTmpEdge(opposite_vertex, tmp_tetra.edges[5]->midv);

      // Should it be GetTmpEdge?..
      short_face_edges[0] = GetPutTmpEdge(tmp_tetra.edges[0]->midv, tmp_tetra.edges[2]->midv);
      short_face_edges[1] = GetPutTmpEdge(tmp_tetra.edges[0]->midv, tmp_tetra.edges[5]->midv);
      short_face_edges[2] = GetPutTmpEdge(tmp_tetra.edges[2]->midv, tmp_tetra.edges[5]->midv);

      new_tetra = InsertTetraAllocated(thisT->level+1,
        tmp_tetra.edges[1], short_face_edges[2],
        tmp_tetra.edges[2]->children[!tmp_orientations[2]], long_face_edges[2],
        tmp_tetra.edges[5]->children[!tmp_orientations[5]], long_face_edges[1],
        &thisT->greens[0]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        tmp_tetra.edges[3], short_face_edges[1],
        tmp_tetra.edges[5]->children[tmp_orientations[5]], long_face_edges[0],
        tmp_tetra.edges[0]->children[1], long_face_edges[2],
        &thisT->greens[1]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        tmp_tetra.edges[4], short_face_edges[0],
        tmp_tetra.edges[0]->children[0], long_face_edges[1],
        tmp_tetra.edges[2]->children[tmp_orientations[2]], long_face_edges[0],
        &thisT->greens[2]);
      new_tetra->parent = thisT;
    } else {

      if(!tmp_tetra.edges[4]->midv){
        assert(0);
      }
      assert(tmp_tetra.edges[3]->midv);

      opposite_vertex = tmp_tetra.edges[2]->nodes[!tmp_orientations[2]];

      long_face_edges[0] = GetPutTmpEdge(opposite_vertex, tmp_tetra.edges[0]->midv);
      long_face_edges[1] = GetPutTmpEdge(opposite_vertex, tmp_tetra.edges[4]->midv);
      long_face_edges[2] = GetPutTmpEdge(opposite_vertex, tmp_tetra.edges[3]->midv);

      // Should it be GetTmpEdge?..
      short_face_edges[0] = GetPutTmpEdge(tmp_tetra.edges[0]->midv, tmp_tetra.edges[4]->midv);
      short_face_edges[1] = GetPutTmpEdge(tmp_tetra.edges[0]->midv, tmp_tetra.edges[3]->midv);
      short_face_edges[2] = GetPutTmpEdge(tmp_tetra.edges[4]->midv, tmp_tetra.edges[3]->midv);

      new_tetra = InsertTetraAllocated(thisT->level+1,
        tmp_tetra.edges[1], short_face_edges[2],
        tmp_tetra.edges[4]->children[!tmp_orientations[4]], long_face_edges[2],
        tmp_tetra.edges[3]->children[tmp_orientations[3]], long_face_edges[1],
        &thisT->greens[0]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1, 
        tmp_tetra.edges[5], short_face_edges[1],
        tmp_tetra.edges[3]->children[!tmp_orientations[3]], long_face_edges[0],
        tmp_tetra.edges[0]->children[1], long_face_edges[2],
        &thisT->greens[1]);
      new_tetra->parent = thisT;

      new_tetra = InsertTetraAllocated(thisT->level+1,
        tmp_tetra.edges[2], short_face_edges[0],
        tmp_tetra.edges[0]->children[0], long_face_edges[1],
        tmp_tetra.edges[4]->children[tmp_orientations[4]], long_face_edges[0],
        &thisT->greens[2]);
      new_tetra->parent = thisT;
    }
    new_tetra = InsertTetraAllocated(thisT->level+1,
      long_face_edges[0], short_face_edges[2],
      long_face_edges[1], short_face_edges[1],
      long_face_edges[2], short_face_edges[0],
      &thisT->greens[3]);
    new_tetra->parent = thisT;

    thisT->num_greens = 4;
    break;
  }
  default:{
    std::cout << edges_split_total << " edges split.... Tetra type is " << 
      ((int)thisT->edges_split) << std::endl;
    assert(0);
  }
  }

}

template<class TInputImage, class TOutputMesh>
typename BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>::RGMEdge_ptr
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::GetPutTmpEdge(RGMVertex_ptr v0, RGMVertex_ptr v1){
  RGMEdge_ptr face_edge = NULL;
  std::pair<RGMVertex_ptr, RGMVertex_ptr> v_pair;
  typename std::map<std::pair<RGMVertex_ptr, RGMVertex_ptr>, RGMEdge_ptr>::iterator
    edge_mapI;

  if(v0>v1){
    v_pair.first = v1;
    v_pair.second = v0;
  } else {
    v_pair.first = v0;
    v_pair.second = v1;
  }

  if((edge_mapI = m_TmpEdgeMap.find(v_pair)) == m_TmpEdgeMap.end()){
    face_edge = InsertEdge(v_pair.first, v_pair.second);
    m_TmpEdgeMap[v_pair] = face_edge;
  } else {
    face_edge = (*edge_mapI).second;
    if((face_edge->nodes[0] != v_pair.first) ||
      (face_edge->nodes[1] != v_pair.second)){
      std::cout << "Edge ptr: " << face_edge << std::endl;
      std::cout << "Lookup nodes: " << v_pair.first << ", " 
        << v_pair.second << std::endl;
      std::cout << "Edge nodes: " << face_edge->nodes[0] << ", " 
        << face_edge->nodes[1] << std::endl;
      std::cout << "Neighbors: ";
      for(typename std::list<RGMTetra_ptr>::iterator nlI = face_edge->neihood.begin();
        nlI!=face_edge->neihood.end();nlI++)
        std::cout << *nlI << " ";
      std::cout << std::endl;
      assert(0);
    }
    m_TmpEdgeMap.erase(edge_mapI);
  }
  return face_edge;
}

template<class TInputImage, class TOutputMesh>
typename BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>::RGMTetra_ptr
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::InsertTetra(unsigned char resolution, 
  RGMEdge_ptr e0, RGMEdge_ptr e1, RGMEdge_ptr e2, 
  RGMEdge_ptr e3, RGMEdge_ptr e4, RGMEdge_ptr e5){
  // assume that edge orientation is correct
  int i, j;
  RGMTetra_ptr new_tetra = new RGMTetra;

  new_tetra->parent = NULL;
  new_tetra->subdiv = 0;
  new_tetra->edges_split = 0;
  new_tetra->edges_orient = 0;
  new_tetra->level = resolution;
  new_tetra->greens = NULL;
  new_tetra->num_greens = 0;

  assert(e0);
  assert(e1);
  assert(e2);
  assert(e3);
  assert(e4);
  assert(e5);

  new_tetra->edges[0] = e0;
  e0->neihood.push_back(new_tetra);
  new_tetra->edges[1] = e1;
  e1->neihood.push_back(new_tetra);
  new_tetra->edges[2] = e2;
  e2->neihood.push_back(new_tetra);
  new_tetra->edges[3] = e3;
  e3->neihood.push_back(new_tetra);
  new_tetra->edges[4] = e4;
  e4->neihood.push_back(new_tetra);
  new_tetra->edges[5] = e5;
  e5->neihood.push_back(new_tetra);

  new_tetra->parent = NULL;
  new_tetra->subdiv = 0;
  new_tetra->edges_split = 0;

  if(resolution>=m_CurrentResol)
    m_Tetras.push_back(new_tetra);
  else
    m_PendingTetras.push_back(new_tetra);

  return new_tetra;
}

template<class TInputImage, class TOutputMesh>
unsigned char
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::GetTetraEdgeConf(RGMTetra_ptr thisT){
  unsigned char edges_split_total = 0;

  thisT->edges_split = 0;
  for(int i=0;i<6;i++)
    if(thisT->edges[i]->midv){
      thisT->edges_split |= (1<<i);
      edges_split_total++;
    }
  return edges_split_total;
}

template<class TInputImage, class TOutputMesh>
unsigned char
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::GetTetraEdgeOrient(RGMTetra_ptr thisT){
  RGMEdge_ptr e0, e1, e2, e3, e4, e5, e6;
  
  e0 = thisT->edges[0];
  e1 = thisT->edges[1];
  e2 = thisT->edges[2];
  e3 = thisT->edges[3];
  e4 = thisT->edges[4];
  e5 = thisT->edges[5];
  thisT->edges_orient = 0;   

  if(e2->nodes[0] != e0->nodes[0] &&
    e2->nodes[1] != e0->nodes[0]){
    // swap e2 and e3
    thisT->edges[2] = e3;
    thisT->edges[3] = e2;
  }
  if(e4->nodes[0] != e0->nodes[0] &&
    e4->nodes[1] != e0->nodes[0]){
    thisT->edges[4] = e5;
    thisT->edges[5] = e4;
  }

  if(orient3d(&(e0->nodes[0]->coords[0]), 
      &(e0->nodes[1]->coords[0]), 
      &(e1->nodes[0]->coords[0]), 
      &(e1->nodes[1]->coords[0])) > 0){ 

    if(thisT->edges[4]->nodes[0] != e1->nodes[1] &&
      thisT->edges[4]->nodes[1] != e1->nodes[1]){
      // swap second with third pair of tetrahedron edges
      RGMEdge_ptr e_tmp;
      e_tmp = thisT->edges[2];
      thisT->edges[2] = thisT->edges[4];
      thisT->edges[4] = e_tmp;
      e_tmp = thisT->edges[3];
      thisT->edges[3] = thisT->edges[5];
      thisT->edges[5] = e_tmp;
    }

    if(thisT->edges[1]->nodes[1] != thisT->edges[3]->nodes[0])
      thisT->edges_orient |= (1<<3);
    if(thisT->edges[1]->nodes[0] != thisT->edges[5]->nodes[1])
      thisT->edges_orient |= (1<<5);
  } else {
    thisT->edges_orient |= 2;
    if(thisT->edges[4]->nodes[0] != e1->nodes[0] &&
      thisT->edges[4]->nodes[1] != e1->nodes[0]){
      // swap second with third pair of tetrahedron edges
      RGMEdge_ptr e_tmp;;
      e_tmp = thisT->edges[2];
      thisT->edges[2] = thisT->edges[4];
      thisT->edges[4] = e_tmp;
      e_tmp = thisT->edges[3];
      thisT->edges[3] = thisT->edges[5];
      thisT->edges[5] = e_tmp;
    }

    if(thisT->edges[1]->nodes[0] != thisT->edges[3]->nodes[0])
      thisT->edges_orient |= (1<<3);
    if(thisT->edges[1]->nodes[1] != thisT->edges[5]->nodes[1])
      thisT->edges_orient |= (1<<5);
  }
  // Now all edges of new_tetra are ordered:
  // e0, e2, and e4 share ORIG(e0) and go in CW,
  // and e1, e3, and e5 are in CW order. 

  // Find the orientation of each of the edges.
  // Edges sharing a node with e0 and have same 
  // ORIG that e0 have orientation 0.
  // Edges of the face opposite to ORIG(e0) and go in CW
  // have orientation 0. Otherwise orientation is 1.
  if(thisT->edges[0]->nodes[0] != thisT->edges[2]->nodes[0])
    thisT->edges_orient |= (1<<2);
  if(thisT->edges[0]->nodes[0] != thisT->edges[4]->nodes[0])
    thisT->edges_orient |= (1<<4);
  return thisT->edges_orient;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::RemoveTetraAllocated(RGMTetra_ptr thisT){
  int i;

  for(i=0;i<6;i++){
    int neihood_size;

    neihood_size = thisT->edges[i]->neihood.size();
    typename std::list<RGMTetra_ptr>::iterator enI = find(thisT->edges[i]->neihood.begin(),
      thisT->edges[i]->neihood.end(), thisT);
    assert(enI!=thisT->edges[i]->neihood.end());
    thisT->edges[i]->neihood.erase(enI);
  }
}

template<class TInputImage, class TOutputMesh>
typename BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>::RGMTetra_ptr
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::InsertTetraAllocated(int res, 
  RGMEdge_ptr e0, RGMEdge_ptr e1, RGMEdge_ptr e2,
  RGMEdge_ptr e3, RGMEdge_ptr e4, RGMEdge_ptr e5,
  RGMTetra_ptr thisT){
  // assume that edge orientation is correct
  int i, j;
  RGMTetra_ptr new_tetra = thisT;

  new_tetra->parent = NULL;
  new_tetra->subdiv = 0;
  new_tetra->edges_split = 0;
  new_tetra->edges_orient = 0;
  new_tetra->level = res;
  new_tetra->greens = NULL;
  new_tetra->num_greens = 0;

  assert(e0);
  assert(e1);
  assert(e2);
  assert(e3);
  assert(e4);
  assert(e5);

  new_tetra->edges[0] = e0;
  e0->neihood.push_back(new_tetra);
  new_tetra->edges[1] = e1;
  e1->neihood.push_back(new_tetra);
  new_tetra->edges[2] = e2;
  e2->neihood.push_back(new_tetra);
  new_tetra->edges[3] = e3;
  e3->neihood.push_back(new_tetra);
  new_tetra->edges[4] = e4;
  e4->neihood.push_back(new_tetra);
  new_tetra->edges[5] = e5;
  e5->neihood.push_back(new_tetra);

  new_tetra->parent = NULL;
  new_tetra->subdiv = 0;
  new_tetra->edges_split = 0;
  new_tetra->level = res;

  if(res>=m_CurrentResol)
    m_Tetras.push_back(new_tetra);
  else
    m_PendingTetras.push_back(new_tetra);

  return new_tetra;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::RemoveTetra(RGMTetra_ptr thisT){
  int i;
    
  for(i=0;i<6;i++){
    int neihood_size;

    neihood_size = thisT->edges[i]->neihood.size();
    typename std::list<RGMTetra_ptr>::iterator enI = find(thisT->edges[i]->neihood.begin(),
      thisT->edges[i]->neihood.end(), thisT);
    assert(enI!=thisT->edges[i]->neihood.end());
    thisT->edges[i]->neihood.erase(enI);
  }
  delete thisT;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::SplitEdge(RGMEdge_ptr thisE){
  RGMVertex_ptr newV = new RGMVertex;

  newV->coords[0] = (thisE->nodes[0]->coords[0] + 
    thisE->nodes[1]->coords[0])/2.0;
  newV->coords[1] = (thisE->nodes[0]->coords[1] +
    thisE->nodes[1]->coords[1])/2.0;
  newV->coords[2] = (thisE->nodes[0]->coords[2] +
    thisE->nodes[1]->coords[2])/2.0;
  thisE->midv = newV;
  m_Vertices.push_back(newV);

  thisE->children[0] = 
    InsertEdge(thisE->nodes[0], thisE->midv);
  thisE->children[1] = 
    InsertEdge(thisE->midv, thisE->nodes[1]);
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::AddSubdivisionTest(SubdivisionTestFunctionPointer f){
  m_SubdivisionCriteria.push_back(f);
}

/* This function takes the number between 0 and 1, which specifies what part
 * of the edge has to be inside the surface. By default, at least 25% of all 
 * edges incident to a vertex have to be inside the object to be enveloped.*/
template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::FindEnvelopedVertices(float percent_inside = 0.5){
  int i, j;
  
  for(typename std::list<RGMTetra_ptr>::iterator tI=m_Tetras.begin();
    tI!=m_Tetras.end();tI++){
    RGMTetra_ptr curT;
    //double *vertices[4];

    curT = *tI;
    for(i=0;i<6;i++){
      float dist0, dist1;
      double divpoint[3];

      dist0 = DistanceAtPoint(curT->edges[i]->nodes[0]->coords);
      dist1 = DistanceAtPoint(curT->edges[i]->nodes[1]->coords);

      if(dist0<0 && dist1<0){
        continue;
      }
      
      if(dist0>0 && dist1>0){
        m_OutOfEnvelopeVertices.insert(curT->edges[i]->nodes[0]);
        m_OutOfEnvelopeVertices.insert(curT->edges[i]->nodes[1]);
        continue;
      }

      //assert(dist0*dist1<0);
      RGMVertex_ptr vp0, vp1;
      if(dist0<0.0){
        vp0 = curT->edges[i]->nodes[0];
        vp1 = curT->edges[i]->nodes[1];
      } else {
        vp0 = curT->edges[i]->nodes[1];
        vp1 = curT->edges[i]->nodes[0];
      }
      


      divpoint[0] = percent_inside*vp0->coords[0] + 
        (1.0-percent_inside)*vp1->coords[0];
      divpoint[1] = percent_inside*vp0->coords[1] + 
        (1.0-percent_inside)*vp1->coords[1];
      divpoint[2] = percent_inside*vp0->coords[2] + 
        (1.0-percent_inside)*vp1->coords[2];

      if(DistanceAtPoint(divpoint)>0.0)
        // The vertex is not sufficiently inside the surface.
        m_OutOfEnvelopeVertices.insert(vp0);
      m_OutOfEnvelopeVertices.insert(vp1);
    }
  }
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::PrepareInputImage(){
  typename CastFilterType::Pointer cast_filter = CastFilterType::New();
  typename ResamplerType::Pointer resampler = ResamplerType::New();
  IdentityTransformType::Pointer transform = IdentityTransformType::New();
  typename InputImageType::SpacingType input_spacing = 
    this->m_InputImage->GetSpacing();
  typename InputImageType::SpacingType output_spacing;
  typename InputImageType::SizeType input_size =
    this->m_InputImage->GetLargestPossibleRegion().GetSize();
  typename InputImageType::SizeType output_size;
  typedef NearestNeighborInterpolateImageFunction<InternalImageType, double>
    ResampleInterpolatorType;
  typename ResampleInterpolatorType::Pointer resample_interpolator = 
    ResampleInterpolatorType::New();

  // 1) Cast the input to the internal image type
  cast_filter->SetInput(this->m_InputImage);
  cast_filter->Update();

  // 2) Resample the input image to unit sized voxels
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
  resampler->SetOutputOrigin(this->m_InputImage->GetOrigin());
  resampler->SetSize(output_size);
  resampler->SetInput(cast_filter->GetOutput());
  resampler->Update();

  this->m_ReadyInputImage = resampler->GetOutput();
  std::cout << "Input image ready" << std::endl;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::PrepareDistanceImage(){
  DistanceFilterType::Pointer distance_filter =
      DistanceFilterType::New();
  distance_filter->SetInput(this->m_ReadyInputImage);
  distance_filter->Update();
  this->m_DistanceImage = distance_filter->GetOutput();
  std::cout << "Distance image ready" << std::endl;
}

template<class TInputImage, class TOutputMesh>
void
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
::PrepareDerivativeImage(){
  RGFType::Pointer rg_filter = RGFType::New();
  rg_filter->SetInput(this->m_DistanceImage);
  rg_filter->SetSigma(1);
  rg_filter->Update();
  m_DistanceDerivativeImage = rg_filter->GetOutput();
  std::cout << "InputImageReady" << std::endl;
}

} /** end namespace itk. */

#endif

/*
template<class TInputImage, class TOutputMesh>
TYPE
BinaryMaskTo3DAdaptiveMeshFilter<TInputImage,TOutputMesh>
*/
/*
  // DEBUG
  // Save enveloped vertices
    {
      int i;
      std::ofstream foutenv("outofenvelope_verts1.vtk");

      foutenv << "# vtk DataFile Version 3.0" << std::endl;
      foutenv << "vtk output" << std::endl << "ASCII" << std::endl;
      foutenv << "DATASET UNSTRUCTURED_GRID" << std::endl;
      foutenv << "POINTS " << m_OutOfEnvelopeVertices.size() << " float" << std::endl;

      for(typename std::set<RGMVertex_ptr>::iterator
        mI=m_OutOfEnvelopeVertices.begin();mI!=m_OutOfEnvelopeVertices.end();mI++){
        foutenv << (*mI)->coords[0] << " " << (*mI)->coords[1] << " " << (*mI)->coords[2] << std::endl;
      }
      
      foutenv << std::endl << "CELLS " << m_OutOfEnvelopeVertices.size() 
        << " " << m_OutOfEnvelopeVertices.size()*2 << std::endl;

      for(i=0;i<m_OutOfEnvelopeVertices.size();i++)
        foutenv << "1 " << i << std::endl;

      foutenv << std::endl << "CELL_TYPES " << m_OutOfEnvelopeVertices.size() << std::endl;
      for(i=0;i<m_OutOfEnvelopeVertices.size();i++)
        foutenv << "1" << std::endl;
      foutenv << std::endl << "FIELD FieldData 1" << std::endl;
      foutenv << "distVals 1 " << m_OutOfEnvelopeVertices.size() << " float" << std::endl;
      for(typename std::set<RGMVertex_ptr>::iterator sI=m_OutOfEnvelopeVertices.begin();
        sI!=m_OutOfEnvelopeVertices.end();sI++){
        float dist = DistanceAtPoint((*sI)->coords);
        foutenv << dist << std::endl;
      }
      foutenv.close();
  }
*/
