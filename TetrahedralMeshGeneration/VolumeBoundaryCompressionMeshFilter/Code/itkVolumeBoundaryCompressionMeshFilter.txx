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
  m_OutputMesh->SetPoints(m_InputMesh->GetPoints());
  m_OutputMesh->SetCellLinks(m_InputMesh->GetCellLinks());
  m_OutputMesh->SetCells(m_InputMesh->GetCells());
  m_OutputMesh->SetCellData(m_InputMesh->GetCellData());
  for(unsigned int dim=0;dim<3;dim++) // only 3D is handled
    m_OutputMesh->SetBoundaryAssignments(dim,
      m_InputMesh->GetBoundaryAssignments(dim));

  // Only nodal coordinates will change after the deformation
  Deform();


  // free the mesh data
  if(m_SurfaceFileName.size()){
    if(surf_error.mesh)
      __free_raw_model(surf_error.mesh);
    free(surf_error.verror);
    free(surf_error.info);
    free_face_error(surf_error.fe);
  }
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
    static_cast<InputImageType*>(this->ProcessObject::GetInput(1));

  std::cout << "Distance image..." << std::endl;
  // Compute the distance image
  //  1. Cast the input image to the internal format
  // TODO: make sure the input image is binary!
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

  // Initialize the interpolator
  m_Interpolator = InterpolatorType::New();
  m_Interpolator->SetInputImage(m_DistanceImage);

  // Mesh input/output initialization
  std::cout << "Mesh initialization..." << std::endl;
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
  
  // Walk through the input mesh, extract surface vertices
  std::cout << "Extracting surface vertices..." << std::endl;
  typename std::map<TetFace,unsigned int> face2cnt;
  typename std::map<TetFace,unsigned int>::iterator face2cntI;
  unsigned int i, j, k;
  const unsigned int tet_face_LUT[16] = 
    { 0, 1, 2, 3,
      0, 1, 3, 2,
      0, 2, 3, 1,
      1, 2, 3, 0 };
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
      TetFace thisFace(face_points[tet_face_LUT[i*4]], 
        face_points[tet_face_LUT[i*4+1]],
        face_points[tet_face_LUT[i*4+2]],
        face_points[tet_face_LUT[i*4+3]]);
      face2cntI = face2cnt.find(thisFace);
      if(face2cntI != face2cnt.end())
        face2cnt[thisFace]++;
      else
        face2cnt[thisFace] = 1;
    }
    cellIterator++;
  }

  // TODO: add surface manifold test!
  
  std::set<unsigned int> surfaceVerticesSet;
  for(face2cntI=face2cnt.begin();face2cntI!=face2cnt.end();
    face2cntI++){
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
        
  std::insert_iterator<std::vector<unsigned int> >
    viI(m_SurfaceVertices, m_SurfaceVertices.begin());
  copy(surfaceVerticesSet.begin(), surfaceVerticesSet.end(), viI);
  surfaceVerticesSet.clear();
  std::sort(m_SurfaceVertices.begin(), m_SurfaceVertices.end());
  
 
  i = 0;
  for(std::vector<unsigned int>::iterator vI=m_SurfaceVertices.begin();
    vI!=m_SurfaceVertices.end();vI++,i++){
    m_SurfaceVertex2Pos[*vI] = i;
  }
  
  std::cout << m_SurfaceVertices.size() << " surface vertices found" << std::endl;
  std::cout << m_SurfaceFaces.size() << " surface faces found" << std::endl;
  
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
  std::cout << "PETSc initialized" << std::endl;
  
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

     // DEBUG
    // Save the surface

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
  
    /*
    {
      std::ofstream fsurf("undeformed_surf.vtk");
      fsurf << "# vtk DataFile Version 3.0" << std::endl;
      fsurf << "vtk output" << std::endl << "ASCII" << std::endl << "DATASET POLYDATA" << std::endl;
      fsurf << "POINTS " << m_SurfaceVertices.size() << " float" << std::endl;
//      fsurf << m_SurfaceVertices.size() << " " << m_SurfaceFaces.size() << std::endl;
      for(j=0;j<m_SurfaceVertices.size();j++)
        fsurf << m_PETScWrapper.m_Mesh->vertices[0][m_SurfaceVertices[j]] << " "
          << m_PETScWrapper.m_Mesh->vertices[1][m_SurfaceVertices[j]] << " "
          << m_PETScWrapper.m_Mesh->vertices[2][m_SurfaceVertices[j]] << std::endl;
      fsurf << std::endl;
      fsurf << "POLYGONS " << m_SurfaceFaces.size() << " " << m_SurfaceFaces.size()*4 << std::endl;
      for(typename std::vector<TetFace>::iterator fI=m_SurfaceFaces.begin();
        fI!=m_SurfaceFaces.end();fI++){
        TetFace thisFace = *fI;
        fsurf << "3 " << m_SurfaceVertex2Pos[thisFace.nodes[0]] << " " 
          << m_SurfaceVertex2Pos[thisFace.nodes[1]] << " " 
          << m_SurfaceVertex2Pos[thisFace.nodes[2]] << std::endl;
      }
      fsurf.close();
    }
    */

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

/* Computes the distance between two points in space */
template<class TInputMesh, class TOutputMesh, class TInputImage>
float
VolumeBoundaryCompressionMeshFilter<TInputMesh,TOutputMesh,TInputImage>
::DistanceBwPoints(double *c0, double *c1){
  return sqrt((c0[0]-c1[0])*(c0[0]-c1[0])+(c0[1]-c1[1])*(c0[1]-c1[1])+
    (c0[2]-c1[2])*(c0[2]-c1[2]));
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
    for(std::vector<int>::iterator lI=nonzero_displ.begin();
      lI!=nonzero_displ.end();lI++,i++){
      for(j=0;j<3;j++){
        iter_Indices[i*3+j] = 3*m_SurfaceVertices[(*lI)]+j;
        iter_Displacements[i*3+j] = U[3*(*lI)+j];
      }
    }

    std::cout << "Iter               " << curIter << "              " << max_distance << std::endl;

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

    m_PETScWrapper.Deform(m_PETScWrapper.m_Mesh, bc, NULL, DEF_E, DEF_NU);
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

} /** end namespace itk. */

#endif

/*
template<class TInputImage, class TOutputMesh>
TYPE
VolumeBoundaryCompressionMeshFilter<TInputImage,TOutputMesh>
*/
