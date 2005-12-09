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
#include "itkTetrahedralMeshReader.h"
#include "itkTetrahedralMeshWriter.h"
#include <set>

#ifdef USE_PETSC 
#define DEF_E 100000.0
#define DEF_NU 0.45
#endif // USE_PETSc

extern "C"{
float exactinit();
double orient3d(double*, double*, double*, double*);
}

namespace itk
{

template<class TInputMesh, class TOutputMesh, class TInputImage>
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::VolumeBoundaryCompressionMeshFilter()
{
  this->SetNumberOfRequiredInputs(2);
  m_CompressionIterations = 3;
  m_SurfaceDisplacements = NULL;
  m_CompressionMethod = FEM_COMPRESSION;
  m_MaxError = 1;
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
  this->m_InputMesh = 
    static_cast<InputMeshType*>(this->ProcessObject::GetInput(0));
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

  // Transfer the connectivity and other unmodified data from the
  // input to the output mesh (copied from TransformMeshFilter)
  m_OutputMesh->SetPointData(m_InputMesh->GetPointData());
  m_OutputMesh->SetPoints(m_InputMesh->GetPoints());
  m_OutputMesh->SetCellLinks(m_InputMesh->GetCellLinks());
  m_OutputMesh->SetCells(m_InputMesh->GetCells());
  m_OutputMesh->SetCellData(m_InputMesh->GetCellData());
  for(unsigned int dim=0;dim<3;dim++) // only 3D is handled
    m_OutputMesh->SetBoundaryAssignments(dim,
      m_InputMesh->GetBoundaryAssignments(dim));

  UpdateSurfaceVerticesAndFaces();

  Initialize();

  switch(m_CompressionMethod){
  case OPTIMIZATION_COMPRESSION:
    Optimize();
    break;
  case FEM_COMPRESSION:
    Deform();
    break;
  default:
    itkExceptionMacro(<<"Unknown compression method requested!");
  }
  //Optimize();

  // Only nodal coordinates will change after the deformation,
  // updated in Deform()

  std::cout << "Mesh orientation before: " << GetMeshOrientation() << std::endl;
  OrientMesh(m_OutputMesh, false);
  std::cout << "Mesh orientation after: " << GetMeshOrientation() << std::endl;
  // free the mesh data
  if(m_SurfaceFileName.size()){
    if(surf_error.mesh)
      __free_raw_model(surf_error.mesh);
    free(surf_error.verror);
    free(surf_error.info);
    free_face_error(surf_error.fe);
  }
}


template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::UpdateSurfaceVerticesAndFaces(){
  // Walk through the input mesh, extract surface vertices
  typedef typename std::map<TetFace,unsigned int> Face2CntMap;
  typedef typename std::map<TetFace,unsigned int>::iterator
    Face2CntMapIterator;
  Face2CntMap face2cnt;
  Face2CntMapIterator face2cntI;
  Face2CntMap** bins;
  unsigned int i, j, k;
  
  //typename std::map<TetFace,unsigned int>::iterator face2cntI;
  const unsigned int tet_face_LUT[16] = 
    { 0, 1, 2, 3,
      0, 1, 3, 2,
      0, 2, 3, 1,
      1, 2, 3, 0 };
  typename TOutputMesh::CellsContainer::ConstIterator cellIterator;
  
  std::cout << "Extracting surface vertices..." << std::endl;
  
  bins = new Face2CntMap*[m_OutputMesh->GetPoints()->Size()];
  for(i=0;i<m_OutputMesh->GetPoints()->Size();i++)
    bins[i] = NULL;

  cellIterator = m_OutputMesh->GetCells()->Begin();
  while(cellIterator != m_OutputMesh->GetCells()->End()){
    OCellType *curCell;
    OutputTetrahedronType *curTet;
    
    curCell = cellIterator.Value();
    if(curCell->GetType() != OCellType::TETRAHEDRON_CELL)
      itkExceptionMacro(<<"Input mesh should be tetrahedral");
    curTet = dynamic_cast<OutputTetrahedronType *>(curCell);

    typename OutputTetrahedronType::PointIdIterator ptI = curTet->PointIdsBegin();
    unsigned int face_points[4];
    for(i=0;i<4;i++)
      face_points[i] = *ptI++;
    for(i=0;i<4;i++){
      TetFace thisFace(face_points[tet_face_LUT[i*4]], 
        face_points[tet_face_LUT[i*4+1]],
        face_points[tet_face_LUT[i*4+2]],
        face_points[tet_face_LUT[i*4+3]]);
      if(bins[thisFace.nodes[0]] == NULL){
        Face2CntMap *new_bin = new Face2CntMap;
        (*new_bin)[thisFace] = 1;
        bins[thisFace.nodes[0]] = new_bin;
      } else {
        Face2CntMap *bin_ptr = bins[thisFace.nodes[0]];
        if(bin_ptr->find(thisFace)!=bin_ptr->end())
          (*bin_ptr)[thisFace]++;
        else
          (*bin_ptr)[thisFace] = 1;
      }
    }
    cellIterator++;
  }

  // TODO: add surface manifold test!
  std::set<unsigned int> surfaceVerticesSet;
  m_SurfaceFaces.clear();
  for(i=0;i<m_OutputMesh->GetPoints()->Size();i++){
    if(!bins[i])
      continue;
    Face2CntMap curFaceMap = *bins[i];
    for(face2cntI=curFaceMap.begin();face2cntI!=curFaceMap.end();face2cntI++){
      unsigned int j;
      switch((*face2cntI).second){
      case 1:{
        TetFace thisFace = (*face2cntI).first;
        for(j=0;j<3;j++)
          surfaceVerticesSet.insert(thisFace.nodes[j]);
        m_SurfaceFaces.push_back(thisFace);
        break;}
      case 2: break;
      case 0:
      default:assert(0);
      }
    }
    delete bins[i];
  }
  delete [] bins;

  // Make sure all faces are oriented consistently
  for(i=0;i<m_SurfaceFaces.size();i++){
    TetFace thisFace = m_SurfaceFaces[i];
    double vertices[4][3];
    typename OutputMeshType::PointType surfacePoint;
    
    for(j=0;j<3;j++){
      m_OutputMesh->GetPoint(thisFace.nodes[j], &surfacePoint);
      for(k=0;k<3;k++)
        vertices[j][k] = surfacePoint[k];
    }
    m_OutputMesh->GetPoint(thisFace.fourth, &surfacePoint);
    for(k=0;k<3;k++)
      vertices[3][k] = surfacePoint[k];
    if(orient3d(&vertices[0][0], &vertices[1][0], &vertices[2][0], &vertices[3][0])>0){
      unsigned tmp_vertex = thisFace.nodes[0];
      thisFace.nodes[0] = thisFace.nodes[1];
      thisFace.nodes[1] = tmp_vertex;
      m_SurfaceFaces[i] = thisFace;
    }
  }
        
  m_SurfaceVertices.clear();
  std::insert_iterator<std::vector<unsigned int> >
    viI(m_SurfaceVertices, m_SurfaceVertices.begin());
  copy(surfaceVerticesSet.begin(), surfaceVerticesSet.end(), viI);
  surfaceVerticesSet.clear();
  std::sort(m_SurfaceVertices.begin(), m_SurfaceVertices.end());
  
  // Update SurfaceVertex2Pos map
  i = 0;
  m_SurfaceVertex2Pos.clear();
  for(std::vector<unsigned int>::iterator vI=m_SurfaceVertices.begin();
    vI!=m_SurfaceVertices.end();vI++,i++){
    m_SurfaceVertex2Pos[*vI] = i;
  }

  std::cout << "UpdateSurfaceVerticesAndFaces() results:" << std::endl;
  std::cout << m_SurfaceVertices.size() << " surface vertices found" << std::endl;
  std::cout << m_SurfaceFaces.size() << " surface faces found" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}


/** Initialize the class fields */
template<class TInputMesh, class TOutputMesh, class TInputImage>
bool
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::Initialize(){
  unsigned i, j, k;
  // TODO: make sure all branches are tested (when no prefix is provided)
  std::cout << "Initializing the compression filter" << std::endl;

  this->m_InputImage =
    static_cast<InputImageType*>(this->ProcessObject::GetInput(1));

  if(m_InputImagePrefix.size()){
    try{
      InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
      reader->SetFileName((m_TmpDirectory+m_InputImagePrefix+"Ready.mhd").c_str());
      reader->Update();
      this->m_ReadyInputImage = reader->GetOutput();
    } catch(ExceptionObject &e){
      InternalImageWriterType::Pointer writer = InternalImageWriterType::New();
      PrepareInputImage();
      writer->SetFileName((m_TmpDirectory+m_InputImagePrefix+"Ready.mhd").c_str());
      writer->SetInput(this->m_ReadyInputImage);
      try{
        writer->Update();
      } catch(ExceptionObject &e){
      }
    }
    try{
      InternalImageReaderType::Pointer reader = InternalImageReaderType::New();
      reader->SetFileName((m_TmpDirectory+m_InputImagePrefix+"DT.mhd").c_str());
      reader->Update();
      this->m_DistanceImage = reader->GetOutput();
    } catch(ExceptionObject &e){
      InternalImageWriterType::Pointer writer = InternalImageWriterType::New();
      PrepareInputImage();
      PrepareDistanceImage();
      writer->SetFileName((m_TmpDirectory+m_InputImagePrefix+"DT.mhd").c_str());
      writer->SetInput(m_DistanceImage);
      try{
        writer->Update();
      } catch(ExceptionObject &e){
      }
    }
  } else {
    PrepareInputImage();
    PrepareDistanceImage();
  }
 
  typename InternalImageType::SizeType input_size =
    this->m_ReadyInputImage->GetLargestPossibleRegion().GetSize();
  this->m_dimX = input_size[0];
  this->m_dimY = input_size[1];
  this->m_dimZ = input_size[2];

  m_InputOrigin = m_InputImage->GetOrigin();

  // Initialize the interpolator
  m_Interpolator = InterpolatorType::New();
  m_Interpolator->SetInputImage(m_DistanceImage);

  // Mesh input/output initialization
  std::cout << "Mesh initialization..." << std::endl;
 
  // Allocate space for surface displacements
  m_SurfaceDisplacements = new double[m_SurfaceVertices.size()*3];
  bzero(m_SurfaceDisplacements, sizeof(double)*m_SurfaceVertices.size()*3);
  
  InputPointsContainerIterator inPointsI;
  InputCellsContainerIterator inCellsI;

#if USE_PETSC
  // Initialize PETSc
  i = PetscInitialize(NULL, NULL, (char*)0, "");
  MPI_Comm_size(PETSC_COMM_WORLD, &m_PETScWrapper.numprocs);
  MPI_Comm_rank(PETSC_COMM_WORLD, &m_PETScWrapper.myid);
  assert(m_PETScWrapper.numprocs==1);
  
  PETScDeformWrapper::tetra_mesh* mesh_ptr;
  m_PETScWrapper.m_Mesh = m_PETScWrapper.create_tetra_mesh(0,0,0,0,0);
  mesh_ptr = m_PETScWrapper.m_Mesh;
  
  for(i=0;i<3;i++)
    mesh_ptr->vertices[i] = new float[m_InputMesh->GetPoints()->Size()];

  
  inPointsI = m_InputMesh->GetPoints()->Begin();
  i = 0;
  while(inPointsI!=m_InputMesh->GetPoints()->End()){
    typename TInputMesh::PointType curPoint;
    curPoint = inPointsI.Value();
    mesh_ptr->vertices[0][i] = curPoint[0];
    mesh_ptr->vertices[1][i] = curPoint[1];
    mesh_ptr->vertices[2][i] = curPoint[2];
    inPointsI++;
    i++;
  }
   
  mesh_ptr->bb[0][0] = 0;
  mesh_ptr->bb[0][1] = 0;
  mesh_ptr->bb[0][2] = 0;
  mesh_ptr->bb[1][0] = this->m_dimX;
  mesh_ptr->bb[1][1] = this->m_dimY;
  mesh_ptr->bb[1][2] = this->m_dimZ;
  mesh_ptr->vox_size[0] = 1.0;
  mesh_ptr->vox_size[1] = 1.0;
  mesh_ptr->vox_size[2] = 1.0;

  mesh_ptr->nvertices = m_InputMesh->GetPoints()->Size();
  mesh_ptr->ncells = m_InputMesh->GetCells()->Size();
  mesh_ptr->cell = new (PETScDeformWrapper::cell*)[mesh_ptr->ncells];
  
  std::cout << "PETSc mesh points initialized" << std::endl;

  inCellsI = m_InputMesh->GetCells()->Begin();
  std::cout << "Initializing cells" << std::endl;
  i = 0;
  while(inCellsI != m_InputMesh->GetCells()->End()){
    InputTetrahedronType *curMeshTet;
    typename InputTetrahedronType::PointIdIterator ptI;
    
    curMeshTet =
      dynamic_cast<InputTetrahedronType*>(inCellsI.Value());
    mesh_ptr->cell[i] = new PETScDeformWrapper::cell;
    ptI = curMeshTet->PointIdsBegin();
    for(j=0;j<4;j++)
      mesh_ptr->cell[i]->vert[j] = *ptI++;
    for(j=0;j<4;j++)
      mesh_ptr->cell[i]->face[j] = 0;//(int)NULL;
    mesh_ptr->cell[i]->label = 0;//(int)NULL;
    i++;
    inCellsI++;
  }
  std::cout << "Initialization of the PETSc wrapper complete!" << std::endl;

  /*
  std::cout << "Checking faces' orientation..." << std::endl;
  // Checking orientation
  for(i=0;i<m_SurfaceFaces.size();i++){
    TetFace thisFace = m_SurfaceFaces[i];
    double vertices[4][3];
    for(j=0;j<3;j++)
      for(k=0;k<3;k++)
        vertices[j][k] = m_PETScWrapper.m_Mesh->vertices[k][thisFace.nodes[j]];  
    for(k=0;k<3;k++)
      vertices[3][k] = m_PETScWrapper.m_Mesh->vertices[k][thisFace.fourth];
    if(orient3d(&vertices[0][0], &vertices[1][0], &vertices[2][0], &vertices[3][0])>0){
      unsigned tmp_vertex = thisFace.nodes[0];
      thisFace.nodes[0] = thisFace.nodes[1];
      thisFace.nodes[1] = tmp_vertex;
      m_SurfaceFaces[i] = thisFace;
    }
  }
*/
     // DEBUG
    // Save the surface
/*
    {
      std::ofstream fsurf("undeformed_surf.raw");
      fsurf << m_SurfaceVertices.size() << " " << m_SurfaceFaces.size() << std::endl;
      for(j=0;j<m_SurfaceVertices.size();j++)
        fsurf << m_PETScWrapper.m_Mesh->vertices[0][m_SurfaceVertices[j]] << " "
          << m_PETScWrapper.m_Mesh->vertices[1][m_SurfaceVertices[j]] << " "
          << m_PETScWrapper.m_Mesh->vertices[2][m_SurfaceVertices[j]] << std::endl;
      for(typename std::vector<TetFace>::iterator fI=m_SurfaceFaces.begin();
        fI!=m_SurfaceFaces.end();fI++){
        TetFace thisFace = *fI;
        fsurf << m_SurfaceVertex2Pos[thisFace.nodes[0]] << " " 
          << m_SurfaceVertex2Pos[thisFace.nodes[1]] << " " 
          << m_SurfaceVertex2Pos[thisFace.nodes[2]] << std::endl;
      }
      fsurf.close();
    }
*/

    
#else // do not USE_PETSC, use FEM ITK Solver
    
  // Initialize the solver
  m_Solver.load.clear();
  m_Solver.el.clear();
  m_Solver.node.clear();

  unsigned int GNcounter;
  // Create nodes
  std::cout << "Initializing the solver with nodes..." << std::endl;
  inPointsI =
    m_InputMesh->GetPoints()->Begin();
  GNcounter = 0;
  while(inPointsI != m_InputMesh->GetPoints()->End()){
    typename TInputMesh::PointType curPoint;
    fem::Node::Pointer newNode;
    curPoint = inPointsI.Value();
    newNode = 
      new fem::Node(curPoint[0], curPoint[1], curPoint[2]);
    newNode->GN = GNcounter;
    m_Solver.node.push_back(fem::FEMP<fem::Node>(newNode));
    GNcounter++;
    ++inPointsI;
  }

  // Initialize the node to position within surface nodes vector map
  i = 0;
  for(std::vector<unsigned int>::iterator vI=m_SurfaceVertices.begin();vI!=m_SurfaceVertices.end();vI++,i++)
    try{
      m_SurfaceNode2Pos[(void*)m_Solver.node.Find(*vI)] = i;
    } catch(ExceptionObject &e){
      std::cout << "Node " << *vI << " not found: " << e << std::endl;
      assert(0);
    }

  // Initialize the material properties
  fem::MaterialLinearElasticity::Pointer material;
  material = fem::MaterialLinearElasticity::New();
  material->GN = 0;
  material->E = 10000;
  material->A = 1.0;
  material->h = 1.0;
  material->I = 1.0;
  material->nu = 0.45;
  material->RhoC = 1.0;
  m_Solver.mat.push_back(fem::FEMP<fem::Material>(material));

  // Create elements 
  std::cout << "Initializing solver with elements..." << std::endl;
  GNcounter = 0;
  inCellsI =
    m_InputMesh->GetCells()->Begin();
  fem::Element3DC0LinearTetrahedronMembrane e0;
  while(inCellsI != m_InputMesh->GetCells()->End()){
    InputTetrahedronType *curMeshTet;
    typename InputTetrahedronType::PointIdIterator ptI;
    fem::Element3DC0LinearTetrahedron::Pointer newFEMTet;
    
    curMeshTet = 
      dynamic_cast<InputTetrahedronType*>(inCellsI.Value());
    newFEMTet = 
      dynamic_cast<fem::Element3DC0LinearTetrahedron*>(e0.Clone());
    ptI = curMeshTet->PointIdsBegin();
    for(i=0;i<4;i++)
      try{
        newFEMTet->SetNode(i, m_Solver.node.Find((unsigned int)*ptI++));
      } catch(ExceptionObject &e){
        std::cout << "Node not found: " << e << std::endl;
        assert(0);
      }

    // all of the elements have the same material assigned
    newFEMTet->SetMaterial(m_Solver.mat.Find(0));
    newFEMTet->GN = GNcounter;
    m_Solver.el.push_back(fem::FEMP<fem::Element>(newFEMTet));
    GNcounter++;
    ++inCellsI;
  }
  
#endif // USE_PETSC

  // If the triangular surface mesh is provided, read it in and initialize
  // data structures
  if(m_SurfaceFileName.size()){
    int rcode;
    dvertex_t bbox_max;
    
    memset(&surf_error, 0, sizeof(surf_error));
    
    surf_info = (struct model_info*) xa_malloc(sizeof(*surf_info));
    rcode = read_fmodel(&surf_model, surf_fname, MESH_FF_AUTO, 1);
    surf_error.mesh = surf_model;
    std::cout << "Surface model read..." << std::endl;
    analyze_model(surf_model, surf_info, 0, false, NULL, NULL);
    std::cout << "... analyzed" << std::endl;
    surf_error.info = surf_info;
    
    fprintf(stdout,"\n                      Model information\n"
      "     (degenerate faces ignored for manifold/closed info)\n\n");
    fprintf(stdout,"Number of vertices:      %i\n",
      surf_model->num_vert);
    fprintf(stdout,"Number of triangles:     %i\n",
      surf_error.mesh->num_faces);
    fprintf(stdout,"Degenerate triangles:    %i\n",
      surf_info->n_degenerate);
    fprintf(stdout,"Number of disjoint parts: %i\n",
      surf_info->n_disjoint_parts);
    fprintf(stdout,"Manifold:                %s\n",
      (surf_info->manifold ? "yes" : "no")); 
    fprintf(stdout,"Originally oriented:     %s\n",
      (surf_info->orig_oriented ? "yes" : "no"));
    fprintf(stdout,"Orientable:              %s\n",
      (surf_info->orientable ? "yes" : "no"));
    fprintf(stdout,"Closed:                  %s\n",
      (surf_info->closed ? "yes" : "no"));
    
    bbox_min.x = surf_model->bBox[0].x;
    bbox_min.y = surf_model->bBox[0].y;
    bbox_min.z = surf_model->bBox[0].z;
    bbox_max.x = surf_model->bBox[1].x;
    bbox_max.y = surf_model->bBox[1].y;
    bbox_max.z = surf_model->bBox[1].z;

    prev_p.x = 0;
    prev_p.y = 0;
    prev_p.z = 0;
    prev_d = 0;

    tl = model_to_triangle_list(surf_model);
    std::cout << std::endl;
    cell_sz = get_cell_size(tl, &bbox_min, &bbox_max, &grid_sz);
    dcl = (dist_cell_lists*) xa_calloc(grid_sz.x*grid_sz.y*grid_sz.z, sizeof(*dcl));
    dcl_buf = NULL;
    dcl_buf_sz = 0;

    fic = triangles_in_cells(tl, grid_sz, cell_sz, bbox_min);
    surf_error.fe = (face_error*) xa_realloc(surf_error.fe, surf_model->num_faces*sizeof(*(surf_error.fe)));
  }
  
//#endif // USE_PETSC
  
  // the set of nodes, elements, and materials of the solver will not change during the
  // deformation. Loads will be changing between timesteps (most likely)
  std::cout << "Initialization complete" << std::endl;
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

  /*
  std::cout << "Computing distance at point (" << coords[0] << "," 
    << coords[1] << "," << coords[2] << ")" << std::endl;
  */
  float distance = 0, // distance from the distance map
        dist_surf = 0;    // distance from the tri surface
  
  typename InterpolatorType::ContinuousIndexType input_index;

  input_index[0] = coords[0];
  input_index[1] = coords[1];
  input_index[2] = coords[2];
  if(m_Interpolator->IsInsideBuffer(input_index))
    distance = (float)m_Interpolator->EvaluateAtContinuousIndex(input_index);
  else {
    std::cerr << "DistanceAtPoint(): Point [" 
                                     << coords[0] << ", " << coords[1] << ", " 
                                       << coords[2] << "] is outside the image boundaries" 
                                       << std::endl;
    assert(0);
  }

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
  
  if(m_SurfaceFileName.size()){
    dvertex_t thisVertex;
    
    thisVertex.x = coords[0];
    thisVertex.y = coords[1];
    thisVertex.z = coords[2];
    dist_surf = dist_pt_surf(thisVertex, tl, fic, 
      grid_sz, cell_sz, bbox_min,
      dcl, &prev_p, prev_d, &dcl_buf, &dcl_buf_sz);
    prev_p = thisVertex;
    prev_d = distance;
    if(distance<0)
      distance = -1.0*dist_surf;
    else
      distance = dist_surf;
  } 

  return distance;
}

/* Returns the interpolated value of distance at the specified coordinate.
 * Will perform the inside test.
 */
template<class TInputMesh, class TOutputMesh, class TInputImage>
float
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh, TInputImage>
::DistanceAtPointVoxel(double *coords){
  float distance = 0, // distance from the distance map
        dist_surf = 0;    // distance from the tri surface
  
  typename InternalImageType::IndexType index;
  typename InternalImageType::PointType point;

  point[0] = coords[0];
  point[1] = coords[1];
  point[2] = coords[2];
  if(m_DistanceImage->TransformPhysicalPointToIndex(point, index))
    distance = (float)m_DistanceImage->GetPixel(index);
  else {
    std::cerr << "DistanceAtPoint(): Point [" 
                                     << coords[0] << ", " << coords[1] << ", " 
                                       << coords[2] << "] is outside the image boundaries" 
                                       << std::endl;
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

/* Align locations of the vetices to improve the quality */
template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::Optimize(){
  unsigned i, j, k;
  
  std::cout << "Using optimization based compression" << std::endl;
  
  UpdateSurfaceVerticesAndFaces();
  UpdateSurfaceDisplacements();

  for(i=0;i<m_CompressionIterations;i++){
    double scaleFactor;
    std::cout << "Iteration " << i << std::endl;
    // DEBUG -->
    /*
      {
      unsigned surface_vert_cnt = m_SurfaceVertices.size();
      grummp_writer->SetFileName("tmp.vmesh");
      grummp_writer->SetInput(m_OutputMesh);
      grummp_writer->Update();
      if(i>1)
        assert(0);
      assert(m_SurfaceVertices.size()==surface_vert_cnt);
      }
    // <-- DEBUG*/


    if(i<=5)
      scaleFactor = .3;
    std::cout << "Surface nodes displacements are updated" << std::endl;
    for(j=0;j<m_SurfaceVertices.size();j++){
      typename OutputMeshType::PointType surfacePoint;
      unsigned long globalSurfaceID;

      globalSurfaceID = m_SurfaceVertices[j];
      m_OutputMesh->GetPoint(globalSurfaceID, &surfacePoint);
      surfacePoint[0] += scaleFactor*m_SurfaceDisplacements[3*j];
      surfacePoint[1] += scaleFactor*m_SurfaceDisplacements[3*j+1];
      surfacePoint[2] += scaleFactor*m_SurfaceDisplacements[3*j+2];
      m_OutputMesh->SetPoint(globalSurfaceID, surfacePoint);
    }

    typename TetrahedralMeshWriter<OutputMeshType>::Pointer
      grummp_writer = TetrahedralMeshWriter<OutputMeshType>::New();
    std::ostringstream fname_stream;
    fname_stream << m_TmpDirectory << "/" << m_InputImagePrefix << "-i" << i << ".vmesh";
    grummp_writer->SetInput(m_OutputMesh);
    grummp_writer->SetFileName(fname_stream.str().c_str());
    std::cout << "Saving GRUMMP mesh into " << fname_stream.str().c_str() << std::endl;
    try{
      grummp_writer->Update();
    }catch(ExceptionObject &e){
      std::cout << "Failed to save mesh for GRUMMP" << std::endl;
      assert(0);
    }

    std::ostringstream cmd_stream;
    cmd_stream << "meshopt3d -i " << m_TmpDirectory << "/" << 
      m_InputImagePrefix << "-i";
    cmd_stream << i << " >& /dev/null"; // -Omf5 for smoothing only
    std::cout << "Running GRUMMP......";
    system(cmd_stream.str().c_str());
    std::cout << "OK" << std::endl;
    
    typename TetrahedralMeshReader<OutputMeshType>::Pointer grummp_reader =
      TetrahedralMeshReader<OutputMeshType>::New();
    fname_stream << ".out";
    grummp_reader->SetFileName(fname_stream.str().c_str());
    try{
      grummp_reader->Update();
    }catch(ExceptionObject &e){
      std::cout << "Failed to read GRUMMP mesh" << std::endl;
      assert(0);
    }
    typename OutputMeshType::Pointer grummpMesh = grummp_reader->GetOutput();
    if(grummpMesh->GetPoints()->Size() !=
      m_OutputMesh->GetPoints()->Size())
      std::cout << "WARNING: number of mesh points has changed after smoothing!" << std::endl;
    if(grummpMesh->GetCells()->Size() != 
      m_OutputMesh->GetCells()->Size())
      std::cout << "WARNING: number of mesh cells has changed after smoothing!" << std::endl;
    m_OutputMesh->SetPoints(grummpMesh->GetPoints());
    m_OutputMesh->SetCells(grummpMesh->GetCells());

    // Evaluate ave and max distance to the surface
    UpdateSurfaceVerticesAndFaces();
    UpdateSurfaceDisplacements();
    
    double max_dist = 0, ave_dist = 0;
    for(j=0;j<m_SurfaceVertices.size();j++){
      double distance;

      distance = m_SurfaceDisplacements[3*j]*m_SurfaceDisplacements[3*j] +
        m_SurfaceDisplacements[3*j+1]*m_SurfaceDisplacements[3*j+1] +
        m_SurfaceDisplacements[3*j+2]*m_SurfaceDisplacements[3*j+2];
      distance = sqrt(distance);
      if(max_dist<distance)
        max_dist = distance;
    }
    std::cout << "Max distance: " << max_dist << std::endl;
    if(max_dist<=m_MaxError)
      break;
    if(MaxSurfaceVoxelDistance()<=1.0)
      break;
  }
}

/* Apply deformation (gradually) to the candidate mesh */
template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::Deform(){
  // Initialize the BC loads. The displacement direction is defined by normals
  // to tetrahedra faces, and then it is scaled by signed distance to the
  // surface.
  unsigned int i, j, k, l, curIter;
  unsigned int curVertexPos;
  unsigned int max_iter = 1;
  double *U;
  double length;
  std::ofstream outfile;
  int mesh_orientation, init_mesh_orientation;
  
  std::cout << "Deformation" << std::endl;

  PETScDeformWrapper::IndDispList* bc;
  bc = new PETScDeformWrapper::IndDispList;
 
  U = new double [m_SurfaceVertices.size()*3];

  
  int *all_Indices = new int[3*m_SurfaceVertices.size()];
  double *all_Displacements = new double[3*m_SurfaceVertices.size()];
  int all_NIndices = 0;
  
  int *iter_Indices = NULL;
  double *iter_Displacements = NULL;
  int iter_NIndices = 0;
  
  std::cout << "Total surface vertices: " << m_SurfaceVertices.size() << std::endl;
  std::cout << "Total surface faces: " << m_SurfaceFaces.size() << std::endl;
  for(i=0;i<m_SurfaceVertices.size();i++){
    for(j=0;j<3;j++){
      all_Indices[all_NIndices] = 3*m_SurfaceVertices[i]+j;
      all_NIndices++;
    }
  }

  bool stop = false;
 
  init_mesh_orientation = GetMeshOrientation();
  std::cout << "Initial mesh orientation is " << init_mesh_orientation << std::endl;

  typename TetrahedralMeshReader<OutputMeshType>::Pointer grummp_reader =
    TetrahedralMeshReader<OutputMeshType>::New();
  typename TetrahedralMeshWriter<OutputMeshType>::Pointer grummp_writer =
    TetrahedralMeshWriter<OutputMeshType>::New();
  
  //m_CompressionIterations = 10;
  for(curIter=0;!stop && curIter< m_CompressionIterations;curIter++){
    // iteration stats
    double min_distance = 10000, max_distance = 0, ave_distance = 0;

    stop = true;

    bzero((void*)U, 3*m_SurfaceVertices.size()*sizeof(double));
    
    double *all_normals = new double[m_SurfaceFaces.size()*3];
    double *vertex_normals = new double[m_SurfaceVertices.size()*3];
    bzero(vertex_normals, sizeof(double)*m_SurfaceVertices.size()*3);
    unsigned normal_id = 0;
    
    // Calculate the displacement unit vectors for all surface vertices
    for(typename std::vector<TetFace>::iterator fI=m_SurfaceFaces.begin();
      fI!=m_SurfaceFaces.end();fI++,normal_id++){
      TetFace thisFace;

      thisFace = *fI;
#if 1
      {
        double vertices[4][3];
        for(j=0;j<3;j++)
          for(k=0;k<3;k++)
            vertices[j][k] = m_PETScWrapper.m_Mesh->vertices[k][thisFace.nodes[j]];
        for(k=0;k<3;k++)
          vertices[3][k] = m_PETScWrapper.m_Mesh->vertices[k][thisFace.fourth];
        if(orient3d(&vertices[0][0], &vertices[1][0], &vertices[2][0], &vertices[3][0])>0){
          unsigned tmp_vertex = thisFace.nodes[0];
          thisFace.nodes[0] = thisFace.nodes[1];
          thisFace.nodes[1] = tmp_vertex;
          m_SurfaceFaces[i] = thisFace;
        }
      }
#endif

      double a[3], b[3], normal[3];
      a[0] = m_PETScWrapper.m_Mesh->vertices[0][thisFace.nodes[2]] 
        - m_PETScWrapper.m_Mesh->vertices[0][thisFace.nodes[1]];
      a[1] = m_PETScWrapper.m_Mesh->vertices[1][thisFace.nodes[2]] 
        - m_PETScWrapper.m_Mesh->vertices[1][thisFace.nodes[1]];
      a[2] = m_PETScWrapper.m_Mesh->vertices[2][thisFace.nodes[2]] 
        - m_PETScWrapper.m_Mesh->vertices[2][thisFace.nodes[1]];
      
      b[0] = m_PETScWrapper.m_Mesh->vertices[0][thisFace.nodes[0]] 
        - m_PETScWrapper.m_Mesh->vertices[0][thisFace.nodes[1]];
      b[1] = m_PETScWrapper.m_Mesh->vertices[1][thisFace.nodes[0]] 
        - m_PETScWrapper.m_Mesh->vertices[1][thisFace.nodes[1]];
      b[2] = m_PETScWrapper.m_Mesh->vertices[2][thisFace.nodes[0]] 
        - m_PETScWrapper.m_Mesh->vertices[2][thisFace.nodes[1]];
     
      normal[0] = a[1]*b[2]-a[2]*b[1];
      normal[1] = a[2]*b[0]-a[0]*b[2];
      normal[2] = a[0]*b[1]-a[1]*b[0];
     
      length = 
        sqrt(normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2]);
     
      normal[0] /= length;
      normal[1] /= length;
      normal[2] /= length;
      
      all_normals[3*normal_id] = normal[0];
      all_normals[3*normal_id+1] = normal[1];
      all_normals[3*normal_id+2] = normal[2];
      //std::cout << normal_id << " " << normal[0] << std::endl;
 
      if(!length){
        std::cout << "Zero length normal: lack of precision!?!?..." << std::endl;
        std::cout << "Failed normal: " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
        std::cout << "Vector a: " << a[0] << "," << a[1] << "," << a[2] << std::endl;
        std::cout << "Vector b: " << b[0] << "," << b[1] << "," << b[2] << std::endl;
        assert(0);
      }
      
      for(j=0;j<3;j++)
        for(k=0;k<3;k++)
          U[3*m_SurfaceVertex2Pos[thisFace.nodes[j]]+k] +=
            normal[k];
    }

    // Scale each of the vectors based on the distance to the surface
    float max_displacement = 0, displacement;
    std::vector<int> nonzero_displ;
    
    for(i=0;i<m_SurfaceVertices.size();i++){
      float distance, length;
      double coords[3];
      unsigned int curVertexID;

      curVertexID = m_SurfaceVertices[i];
      length = sqrtf(U[i*3]*U[i*3] + U[i*3+1]*U[i*3+1] +
        U[i*3+2]*U[i*3+2]);
      assert(length);

      for(j=0;j<3;j++)
        try{
          coords[j] = 
            m_PETScWrapper.m_Mesh->vertices[j][curVertexID];
        } catch(ExceptionObject &e){
          std::cout << "Node not found: " << e << std::endl;
          assert(0);
        }

      distance = DistanceAtPoint(coords);
      ave_distance += fabs(distance);
      if(fabs(distance)<min_distance)
        min_distance = fabs(distance);
      if(fabs(distance)>max_distance)
        max_distance = fabs(distance);
      
      if(curIter<10)
        distance = 0.2*distance;
      if(distance!=0){
        stop = false;
        nonzero_displ.push_back(i);
        iter_NIndices++;
      }

      // the image is unit-spaced
      displacement = 0;
      for(j=0;j<3;j++){
        U[i*3+j] *= (distance/length);
        displacement += U[i*3+j]*U[i*3+j];
      }
      if(displacement>max_displacement)
        max_displacement = sqrt(displacement);
    }
    
    iter_NIndices *= 3;
    iter_Indices = new int[iter_NIndices];
    iter_Displacements = new double[iter_NIndices];

    i = 0;
    std::cout << endl;
    for(std::vector<int>::iterator lI=nonzero_displ.begin();
      lI!=nonzero_displ.end();lI++,i++){
      typename OutputMeshType::PointType surfVertex;
      unsigned globalVertexID;
      globalVertexID = m_SurfaceVertices[(*lI)];
      for(j=0;j<3;j++){
        iter_Indices[i*3+j] = 3*globalVertexID+j;
        iter_Displacements[i*3+j] = U[3*(*lI)+j];
        surfVertex[j] = U[3*(*lI)+j] + 
          m_PETScWrapper.m_Mesh->vertices[j][globalVertexID];
      }
      m_OutputMesh->SetPoint(globalVertexID, surfVertex);
    }

    mesh_orientation = GetMeshOrientation();
    std::cout << "Iteration:" << curIter << "   " << max_distance << ", orient: " << mesh_orientation << std::endl;
    if(mesh_orientation!=init_mesh_orientation){
      std::cerr << "WARNING: Mesh orientation changed.";
      std::cerr << "Mesh contains flipped elements (i.e., throw it away, ";
      std::cerr << "unless the initial mesh was not oriented)" << std::endl;
    }

    std::ostringstream fname_stream;
    fname_stream << m_InputImagePrefix << "-i" << curIter << ".vmesh";
    grummp_writer->SetInput(m_OutputMesh);
    grummp_writer->SetFileName(fname_stream.str().c_str());
    try{
      grummp_writer->Update();
    }catch(ExceptionObject &e){
      std::cout << "Failed to save mesh for GRUMMP" << std::endl;
      assert(0);
    }
    
#if USE_PETSC

    bzero(bc, sizeof(PETScDeformWrapper::IndDispList));
    

    m_PETScWrapper.m_Mesh = 
      m_PETScWrapper.add_edge_info_tetra_mesh(m_PETScWrapper.m_Mesh);
    int iter = 0;
    double min_edge_len;

    //  for(iter;!stop && iter<1;iter++){
    double len = 0, max_len=0;
    unsigned displaced_verts = 0;
    bc->Indices = iter_Indices;
    bc->Displacements = iter_Displacements;
    bc->NIndices = iter_NIndices;
    /*
    std::cout << "VertexID: " << bc->Indices[0]/3 << ", displacements: " << 
      bc->Displacements[0] << ", " << bc->Displacements[1] << ", " << 
      bc->Displacements[2] << std::endl;
    std::cout << "Coordinates: ";
    */
    
    /*
      {
      std::ofstream fverts("bc_before.dat");
      for(int iii=0;iii<bc->NIndices;iii+=3){
        fverts << m_PETScWrapper.m_Mesh->x[iii/3]+bc->Displacements[iii] << 
          ", " << m_PETScWrapper.m_Mesh->y[iii/3]+bc->Displacements[iii+1] <<
          ", " << m_PETScWrapper.m_Mesh->z[iii/3]+bc->Displacements[iii+2] << std::endl;
      }
      }*/
    m_PETScWrapper.Deform(m_PETScWrapper.m_Mesh, bc, NULL, DEF_E, DEF_NU);

    {
      UpdateSurfaceDisplacements();

      double max_dist = 0, ave_dist = 0;
      for(j=0;j<m_SurfaceVertices.size();j++){
        double distance;

        distance = m_SurfaceDisplacements[3*j]*m_SurfaceDisplacements[3*j] +
          m_SurfaceDisplacements[3*j+1]*m_SurfaceDisplacements[3*j+1] +
          m_SurfaceDisplacements[3*j+2]*m_SurfaceDisplacements[3*j+2];
        distance = sqrt(distance);
        if(max_dist<distance)
          max_dist = distance;
      }
      std::cout << "Max distance: " << max_dist << std::endl;
      if(max_dist<=m_MaxError)
        break;
    }
    /*
      {
      std::ofstream fverts("bc_after.dat");
      for(int iii=0;iii<bc->NIndices;iii+=3){
        fverts << m_PETScWrapper.m_Mesh->x[iii/3] << 
          ", " << m_PETScWrapper.m_Mesh->y[iii/3] <<
          ", " << m_PETScWrapper.m_Mesh->z[iii/3] << std::endl;
      }
      }*/

    delete [] iter_Displacements;
    delete [] iter_Indices;
    iter_NIndices = 0;
    

#else // USE_PETSC (use itkFEM)

    // Initialize BC loads for the displaced vertices
    // Go thru all the elements, for each element check each vertex if it is one
    // of the surface vertices initialize the load appropriately
    m_Solver.load.clear();
    unsigned thisGN = 0;
    std::set<void*> initialized_nodes;
    for(fem::Element::ArrayType::iterator 
      e = m_Solver.el.begin();e!=m_Solver.el.end();e++){
      unsigned int curVertexPos;
      assert((*e)->GetNumberOfDegreesOfFreedom() == 12);
      for(i=0;i<4;i++){
        std::map<void*,unsigned int>::iterator nodeI;

        nodeI = m_SurfaceNode2Pos.find((void*)(*e)->GetNode(i));
        if(nodeI != m_SurfaceNode2Pos.end()){
          curVertexPos = (*nodeI).second;
          assert(curVertexPos < m_SurfaceVertices.size());
          if(initialized_nodes.find((void*)(*e)->GetNode(i)) ==
            initialized_nodes.end()){
            initialized_nodes.insert((void*)(*e)->GetNode(i));
            for(j=0;j<3;j++){
              fem::LoadBC::Pointer newLoad;
              newLoad = fem::LoadBC::New();
              newLoad->m_element = *e;
              newLoad->m_dof = i*3+j;
              newLoad->m_value.set_size(1);
              newLoad->m_value = U[curVertexPos*3+j];
              if(fabs((double)U[curVertexPos*3+j]==0))
                newLoad->m_value = 0.0;
              //  std::cout << newLoad->m_value << std::endl;
              newLoad->GN = thisGN;
              m_Solver.load.push_back(fem::FEMP<fem::Load>(newLoad));
              thisGN++;
            } // for (each degree of freedom for a node)
          } // if the node has not been visited yet
        } // if (a node is on the surface)
      } // for (all nodes of a terahedron)
    } // for (all elements of the mesh)

    std::cout << "Solver initialized" << std::endl;
    std::cout << "Total nodes: " << m_Solver.node.size() << std::endl;
    std::cout << "Total elements: " << m_Solver.el.size() << std::endl;
    std::cout << "Total loads: " << m_Solver.load.size() << std::endl;
    std::cout << "Total surface vertices: " << m_SurfaceVertices.size() << std::endl;


    outfile.open("/tmp/solver_before.dat");
    m_Solver.Write(outfile);
    outfile.close();

    // Run the solver!
    m_Solver.GenerateGFN();
    fem::LinearSystemWrapperVNL lsw_vnl;
    fem::LinearSystemWrapperItpack lsw_itpack;
    fem::LinearSystemWrapperDenseVNL lsw_dvnl;
    lsw_itpack.SetMaximumNonZeroValuesInMatrix(200000);
    m_Solver.SetLinearSystemWrapper(&lsw_vnl);
    try{
      std::cout << "AssembleK()..." << std::endl;
      m_Solver.AssembleK();
      std::cout << "DecomposeK()..." << std::endl;
      m_Solver.DecomposeK();
      std::cout << "AssembleF()..." << std::endl;
      m_Solver.AssembleF();
      std::cout << "Solve!!!..." << std::endl;
      m_Solver.Solve();
    }catch(ExceptionObject &e){
      std::cout << "Exception " << e << std::endl;
      assert(0);
    }

    // Update the output mesh nodal coordinates
    i = 0;
    max_displacement = 0;
    for(fem::Solver::NodeArray::iterator n=m_Solver.node.begin();
      n!=m_Solver.node.end();n++){
      typename OutputMeshType::PointType newPoint;

      newPoint[0] = (*n)->GetCoordinates()[0];
      newPoint[1] = (*n)->GetCoordinates()[1];
      newPoint[2] = (*n)->GetCoordinates()[2];

      displacement = 0;
      for(unsigned int d=0, dof;(dof=(*n)->GetDegreeOfFreedom(d))!=fem::Element::InvalidDegreeOfFreedomID; d++){
        newPoint[d] += m_Solver.GetSolution(dof);
        displacement += newPoint[d]*newPoint[d];
      }
      if(displacement>max_displacement)
        max_displacement = displacement;
      std::cout << newPoint << std::endl;
      this->GetOutput()->SetPoint(i, newPoint);
      i++;
    }
#endif // USE_PETSC
//    std::cout << "Max solution displacement is " << sqrtf(max_displacement) << std::endl;
  }
  
#if USE_PETSC
  // Update the nodal coordinates of the output mesh
  for(i=0;i<m_InputMesh->GetPoints()->size();i++){
    typename OutputMeshType::PointType newPoint;
    
    newPoint[0] = m_PETScWrapper.m_Mesh->x[i];
    newPoint[1] = m_PETScWrapper.m_Mesh->y[i];
    newPoint[2] = m_PETScWrapper.m_Mesh->z[i];
    this->GetOutput()->SetPoint(i, newPoint);
  }
#endif // USE_PETSC

  delete [] U;
  delete bc;
  
  delete [] all_Indices;
  delete [] all_Displacements;
  
  
#ifndef USE_PETSC 
  outfile.open("/tmp/solver_after.dat");
  m_Solver.Write(outfile);
  outfile.close();
#endif

  this->GetOutput()->SetBufferedRegion(this->GetOutput()->GetRequestedRegion());
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
int
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::GetMeshOrientation(){
  // TODO: if start using ITKFEM, will have to fix this...
  int i, j, orientation = -1;
  PETScDeformWrapper::tetra_mesh* mesh_ptr;

  mesh_ptr = m_PETScWrapper.m_Mesh;
  for(i=0;i<mesh_ptr->ncells;i++){
    double coords[4][3];
    int thisTorient;
    for(j=0;j<4;j++){
      coords[j][0] = 
        mesh_ptr->vertices[0][mesh_ptr->cell[i]->vert[j]];
      coords[j][1] = 
        mesh_ptr->vertices[1][mesh_ptr->cell[i]->vert[j]];
      coords[j][2] = 
        mesh_ptr->vertices[2][mesh_ptr->cell[i]->vert[j]];
    }
    if(orient3d(&coords[0][0], &coords[1][0], &coords[2][0],
        &coords[3][0])>0){
      thisTorient = 1;
    } else {
      thisTorient = -1;
    }
    if(i==0)
      orientation = thisTorient;
    else
      if(orientation!=thisTorient)
        orientation = 0;
  }
  return orientation;
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
int
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::GetMeshOrientation(const OutputMeshType* input_mesh){
  int i, orientation = -1;
  
  typename OutputMeshType::CellsContainer::ConstIterator outCellsI = input_mesh->GetCells()->Begin();
  while(outCellsI != input_mesh->GetCells()->End()){
    OutputTetrahedronType *curMeshTet;
    typename OutputTetrahedronType::PointIdIterator ptI;
    double coords[4][3];
    unsigned pointIds[4];
    curMeshTet =
      dynamic_cast<OutputTetrahedronType*>(outCellsI.Value());
    ptI = curMeshTet->PointIdsBegin();
    for(i=0;i<4;i++){
      typename OutputMeshType::PointType tetPoint;
      pointIds[i] = *ptI;
      input_mesh->GetPoint(*ptI, tetPoint);
      coords[i][0] = tetPoint[0];
      coords[i][1] = tetPoint[1];
      coords[i][2] = tetPoint[2];
      ptI++;
    }
    if(orient3d(&coords[0][0], &coords[1][0], &coords[2][0],
        &coords[3][0])>0)
      thisOrient = 1;
    else
      thisOrient = -1;
    if(orientation==99)
      orientation = thisOrient;
    else
      if(orientation!=thisOrient)
        orientation = 0;
    outCellsI++;
  }
  return orientation;
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
float
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::MaxSurfaceVoxelDistance(){
  // Surface vertex displacement is defined by the vertex normal scaled by the
  // distance to the 0th level set
  
  unsigned i, j, normal_id=0;
  float discr_dist_max = 0;
  for(i=0;i<m_SurfaceVertices.size();i++){
    double dist, len;
    double pointCoords[3];
    unsigned globalVertexID;
    typename OutputMeshType::PointType surfacePoint;

    globalVertexID = m_SurfaceVertices[i];
    m_OutputMesh->GetPoint(globalVertexID, &surfacePoint);
    pointCoords[0] = surfacePoint[0];
    pointCoords[1] = surfacePoint[1];
    pointCoords[2] = surfacePoint[2];
    dist = DistanceAtPointVoxel(pointCoords);
    if(dist>discr_dist_max)
      discr_dist_max = dist;
  }
  return discr_dist_max;
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::UpdateSurfaceDisplacements(){
  // Surface vertex displacement is defined by the vertex normal scaled by the
  // distance to the 0th level set
  
  unsigned i, j, normal_id=0;
  bzero(m_SurfaceDisplacements, 3*sizeof(double)*m_SurfaceVertices.size());
  // Calculate the displacement unit vectors for all surface vertices
  for(typename std::vector<TetFace>::iterator fI=m_SurfaceFaces.begin();
    fI!=m_SurfaceFaces.end();fI++,normal_id++){
    TetFace thisFace = *fI;
    double a[3], b[3], normal[3];
    unsigned surfaceVertexID;
    typename OutputMeshType::PointType facePoints[3];
      
    m_OutputMesh->GetPoint(thisFace.nodes[0], &facePoints[0]);
    m_OutputMesh->GetPoint(thisFace.nodes[1], &facePoints[1]);
    m_OutputMesh->GetPoint(thisFace.nodes[2], &facePoints[2]);
    for(i=0;i<3;i++){
      a[i] = facePoints[2][i] - facePoints[1][i];
      b[i] = facePoints[0][i] - facePoints[1][i];
    }
    normal[0] = a[1]*b[2]-a[2]*b[1];
    normal[1] = a[2]*b[0]-a[0]*b[2];
    normal[2] = a[0]*b[1]-a[1]*b[0];
    double length = sqrt(normal[0]*normal[0]+
      normal[1]*normal[1]+normal[2]*normal[2]);
    for(j=0;j<3;j++){
      surfaceVertexID = m_SurfaceVertex2Pos[thisFace.nodes[j]];
      m_SurfaceDisplacements[3*surfaceVertexID] +=
        normal[0];
      m_SurfaceDisplacements[3*surfaceVertexID+1] +=
        normal[1];
      m_SurfaceDisplacements[3*surfaceVertexID+2] +=
        normal[2];
    }
  }
  
  // now scale the normals by distance value at the surface vertex
  double discr_dist_max = 0;
  for(i=0;i<m_SurfaceVertices.size();i++){
    double dist, len;
    double pointCoords[3];
    unsigned globalVertexID;
    typename OutputMeshType::PointType surfacePoint;

    globalVertexID = m_SurfaceVertices[i];
    m_OutputMesh->GetPoint(globalVertexID, &surfacePoint);
    pointCoords[0] = surfacePoint[0];
    pointCoords[1] = surfacePoint[1];
    pointCoords[2] = surfacePoint[2];
    len = sqrt(m_SurfaceDisplacements[3*i]*m_SurfaceDisplacements[3*i] +
      m_SurfaceDisplacements[3*i+1]*m_SurfaceDisplacements[3*i+1] +
      m_SurfaceDisplacements[3*i+2]*m_SurfaceDisplacements[3*i+2]);
    dist = DistanceAtPoint(pointCoords);
    m_SurfaceDisplacements[3*i] *= dist/len;
    m_SurfaceDisplacements[3*i+1] *= dist/len;
    m_SurfaceDisplacements[3*i+2] *= dist/len;
    dist = DistanceAtPointVoxel(pointCoords);
    if(dist>discr_dist_max)
      discr_dist_max = dist;
  }
  std::cout << "After update: max discrete distance is " << discr_dist_max << std::endl;
}

/* Orient consistently all tetrahedra of the mesh */
template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::OrientMesh(OutputMeshType* input_mesh, bool orientation){
  int i;
  
  typename OutputMeshType::CellsContainer::ConstIterator outCellsI = input_mesh->GetCells()->Begin();
  while(outCellsI != input_mesh->GetCells()->End()){
    OutputTetrahedronType *curMeshTet;
    typename OutputTetrahedronType::PointIdIterator ptI;
    double coords[4][3];
    unsigned pointIds[4];
    curMeshTet =
      dynamic_cast<OutputTetrahedronType*>(outCellsI.Value());
    ptI = curMeshTet->PointIdsBegin();
    for(i=0;i<4;i++){
      typename OutputMeshType::PointType tetPoint;
      pointIds[i] = *ptI;
      input_mesh->GetPoint(*ptI, &tetPoint);
      coords[i][0] = tetPoint[0];
      coords[i][1] = tetPoint[1];
      coords[i][2] = tetPoint[2];
      ptI++;
    }
    if(orient3d(&coords[0][0], &coords[1][0], &coords[2][0],
        &coords[3][0])>0)
      if(!orientation){
        typename OutputMeshType::PointType tetPoint;
        tetPoint[0] = coords[1][0];
        tetPoint[1] = coords[1][1];
        tetPoint[2] = coords[1][2];
        input_mesh->SetPoint(pointIds[0], tetPoint);
        tetPoint[0] = coords[0][0];
        tetPoint[1] = coords[0][1];
        tetPoint[2] = coords[0][2];
        input_mesh->SetPoint(pointIds[1], tetPoint);
      }
    else
      if(orientation){
        typename OutputMeshType::PointType tetPoint;
        tetPoint[0] = coords[1][0];
        tetPoint[1] = coords[1][1];
        tetPoint[2] = coords[1][2];
        input_mesh->SetPoint(pointIds[0], tetPoint);
        tetPoint[0] = coords[0][0];
        tetPoint[1] = coords[0][1];
        tetPoint[2] = coords[0][2];
        input_mesh->SetPoint(pointIds[1], tetPoint);
      }
    outCellsI++;
  }
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::PrepareInputImage(){
  typename CastFilterType::Pointer cast_filter = CastFilterType::New();
  typename ResamplerType::Pointer resampler = ResamplerType::New();
  IdentityTransformType::Pointer transform = IdentityTransformType::New();
  typename InputImageType::SpacingType input_spacing = m_InputImage->GetSpacing();
  typename InputImageType::SpacingType output_spacing;
  typename InputImageType::SizeType input_size =
    m_InputImage->GetLargestPossibleRegion().GetSize();
  typename InputImageType::SizeType output_size;
  typedef NearestNeighborInterpolateImageFunction<InternalImageType,double>
    ResampleInterpolatorType;
  typename ResampleInterpolatorType::Pointer resample_interpolator = 
    ResampleInterpolatorType::New();

  // 1) Cast the input to the internal image type
  cast_filter->SetInput(m_InputImage);
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
  resampler->SetOutputOrigin(m_InputImage->GetOrigin());
  resampler->SetSize(output_size);
  resampler->SetInput(cast_filter->GetOutput());
  resampler->Update();

  this->m_ReadyInputImage = resampler->GetOutput();
  std::cout << "Input image ready" << std::endl;
}

template<class TInputMesh, class TOutputMesh, class TInputImage>
void
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::PrepareDistanceImage(){
  DistanceFilterType::Pointer distance_filter =
      DistanceFilterType::New();
  distance_filter->SetInput(this->m_ReadyInputImage);
  distance_filter->Update();
  this->m_DistanceImage = distance_filter->GetOutput();
  std::cout << "Distance image ready" << std::endl;
}
} /** end namespace itk. */

#endif

/*
template<class TInputImage, class TOutputMesh>
TYPE
VolumeBoundaryCompressionMeshFilter<TInputImage,TOutputMesh>
*/
