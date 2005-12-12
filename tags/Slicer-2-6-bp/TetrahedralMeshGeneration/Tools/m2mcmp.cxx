/*
 * Author: Andriy Fedorov
 *
 * This tool compares the accuracy and quality of the two input tetrahedral 
 * volume meshes.
 * 
 * Quality is assessed using standard VTK MeshQuality class
 * Accuracy is evaluated using two methods:
 *  1) (Mesh volume overlap)/(Mesh 1 volume + Mesh 2 volume)
 *     Mesh volume overlap is computed on voxel basis: for each voxel in the
 *     image volume, it belongs to the mesh volume if its centroid is inside
 *     the mesh (this is tested on a per-element basis). Mesh volumes are also
 *     computed as the sum of all tetrahedral elements.
 *
 *  2) Hausdorff distance
 *     We can evaluate Hausdorff distance between two volume meshes, but I
 *     think it's more useful to evaluate the distance from the surface
 *     produced by Marching cubes algorith to each of the input surfaces.
 *     Marching cubes surface is produced using ITK functionality, while
 *     Hausdorff distance (HD) is evaluated using M.E.S.H. software
 *     (http://mesh.berlios.de). I cannot explain extremely large values for
 *     HD (~40!!!). Will also try to calculate 95% HD.
 *
 *     INPUT: binary image used to produce the meshes, two vtk/vol/vmesh
 *     meshes to be compared
 *
 *     NB: the binary image is assumed to be in MET_USHORT format. Change and
 *     recompile the code if necessary.
 *
 *  PROBLEM: because of the precision (I think) the voxels along the element
 *  faces are not labeled correctly, thus the resulting metric is not precise.
 */

#include "itkTetrahedralMeshReader.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTetrahedralMeshToSurfaceMeshFilter.h"
#include "itkTriangularSurfaceMeshWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include <iostream>

extern "C"{
float exactinit();
double orient3d(double*, double*, double*, double*);
}

using namespace itk;

typedef unsigned short PixelType;
typedef unsigned long FloatPixelType;
typedef Image<PixelType,3> ImageType;
typedef Image<FloatPixelType,3> FloatImageType;
typedef ImageFileReader<ImageType> ImageReaderType;
typedef ImageFileWriter<ImageType> ImageWriterType;
typedef ImageFileWriter<FloatImageType> FloatImageWriterType;
typedef Mesh<PixelType,3> MeshType;
typedef TetrahedralMeshReader<MeshType> MeshReaderType;
typedef TetrahedralMeshToSurfaceMeshFilter<MeshType,MeshType> Vol2SurfConverterType;
typedef TriangularSurfaceMeshWriter<MeshType> SurfWriterType;
typedef TetrahedronCell<MeshType::CellType> TetrahedronType;
typedef ImageRegionConstIteratorWithIndex<ImageType> ConstRegionIteratorType;
typedef ImageRegionIteratorWithIndex<ImageType> RegionIteratorType;
typedef ConnectedComponentImageFilter<ImageType,FloatImageType> CCFilterType;
typedef ConnectedThresholdImageFilter<ImageType,ImageType> FloodFillType;

bool IsPointInsideTetra(MeshType::CoordRepType*, MeshType*, TetrahedronType*);
void GetImageFromMesh(ImageType*, MeshType*, ImageType*);

/* 
 * arg0: binary image
 * arg1: mesh #1
 * arg2: mesh #2
 */
int main(int argc, char **argv){
  ImageReaderType::Pointer image_reader = ImageReaderType::New();
  ImageType::Pointer input_image;
  MeshReaderType::Pointer mesh1_reader = MeshReaderType::New(),
    mesh2_reader = MeshReaderType::New();
  MeshType::Pointer vol_mesh1, vol_mesh2, mc_surf_mesh;
  MeshType::Pointer surf_mesh1, surf_mesh2;
  Vol2SurfConverterType::Pointer vol2surf = Vol2SurfConverterType::New();
  TriangularSurfaceMeshWriter<MeshType>::Pointer surf_writer =
    TriangularSurfaceMeshWriter<MeshType>::New();
  ImageType::Pointer mesh1image, mesh2image;
  ImageWriterType::Pointer image_writer =
    ImageWriterType::New();

  if(argc<4){
    std::cerr << "Three arguments required: (1) binary image," <<
      "(2) mesh #1, (3) mesh #2." << std::endl;
    return -1;
  }
 
  exactinit();

  image_reader->SetFileName(argv[1]);
  try{
    image_reader->Update();
  } catch(ExceptionObject &e){
    std::cout << "Cannot read input image: " << e << std::endl;
    return -1;
  }

  std::cout << "Input image read" << std::endl;
  
  input_image = image_reader->GetOutput();
  mesh1image = ImageType::New();
  mesh2image = ImageType::New();
  ImageType::RegionType region = 
    input_image->GetLargestPossibleRegion();
  ImageType::SpacingType imageSpacing =
    input_image->GetSpacing();
  mesh1image->SetOrigin(input_image->GetOrigin());
  mesh2image->SetOrigin(input_image->GetOrigin());
  mesh1image->SetSpacing(imageSpacing);
  mesh2image->SetSpacing(imageSpacing);
  mesh1image->SetRegions(region);
  mesh2image->SetRegions(region);
  mesh1image->Allocate();
  mesh2image->Allocate();
  mesh1image->FillBuffer(0);
  mesh2image->FillBuffer(0);
  
  mesh1_reader->SetFileName(argv[2]);
  try{
    mesh1_reader->Update();
  } catch(ExceptionObject &e){
    std::cout << "Cannot read input mesh 1: " << e << std::endl;
    return -1;
  }
  vol_mesh1 = mesh1_reader->GetOutput();
  mesh2_reader->SetFileName(argv[3]);
  try{
    mesh2_reader->Update();
  } catch(ExceptionObject &e){
    std::cout << "Cannot read input mesh 2: " << e << std::endl;
    return -1;
  }
  vol_mesh2 = mesh2_reader->GetOutput();
  assert(vol_mesh1 != vol_mesh2);

  std::cout << "Input data read" << std::endl;

  vol2surf->SetInput(vol_mesh1);
  try{
    vol2surf->Update();
  } catch(ExceptionObject &e){
    std::cout << "Volume2Surface failed: " << e << std::endl;
    return -1;
  }
  
  surf_mesh1 = vol2surf->GetOutput();
  std::cout << "Number of points: " << surf_mesh1->GetPoints()->Size() << std::endl;
  surf_writer->SetFileName("test_surf.raw");
  surf_writer->SetInput(surf_mesh1);
  try{
    surf_writer->Update();
  } catch(ExceptionObject &e){
    std::cout << "Surface writer failed: " << e << std::endl;
    return -1;
  }

  GetImageFromMesh(input_image, vol_mesh1, mesh1image);
  GetImageFromMesh(input_image, vol_mesh2, mesh2image);

  /*
  // Add 1 to all voxels (so that the regions inside the image are not treated
  // as background
  RegionIteratorType add1Iter(mesh1image, mesh1image->GetRequestedRegion());
  for(add1Iter.GoToBegin();!add1Iter.IsAtEnd();++add1Iter)
    add1Iter.Set(add1Iter.Value()+1);
  

  FloodFillType::Pointer flooder = FloodFillType::New();
  flooder->SetInput(mesh1image);
  flooder->SetReplaceValue(0);
  flooder->SetLower(0);
  flooder->SetUpper(1);
  ImageType::IndexType seed;
  seed[0] = 1;
  seed[1] = 1;
  seed[2] = 1;
  flooder->AddSeed(seed);
  try{
    flooder->Update();
  } catch(ExceptionObject &e){
    std::cout << "Flood failed: " << e << std::endl;
    return -1;
  }

  mesh1image = flooder->GetOutput();
  image_writer->SetInput(mesh1image);
  image_writer->SetFileName("mesh1-merged.mhd");
  try{
    image_writer->Update();
  } catch(ExceptionObject &e){
    std::cout << "Writer failed: " << e << std::endl;
    return -1;
  }
  std::cout << "Modified image saved!" << std::endl;
  
  RegionIteratorType add1aIter(mesh1image, mesh1image->GetRequestedRegion());
  for(add1aIter.GoToBegin();!add1aIter.IsAtEnd();++add1aIter)
    if(add1aIter.Value()==2)
      add1aIter.Set(1);
  */
  // Find disconnected components (not all of the inside voxels are detected
  // because of numberical errors)
  /*
  CCFilterType::Pointer cc_filter = CCFilterType::New();
  cc_filter->SetInput(mesh1image);
  try{
    cc_filter->Update();
  } catch(ExceptionObject &e){
    std::cout << "ConnectedComponent filter failed: " << e << std::endl;
    return -1;
  }

  FloatImageWriterType::Pointer float_writer =
    FloatImageWriterType::New();
  float_writer->SetInput(cc_filter->GetOutput());
  float_writer->SetFileName("mesh1cc.mhd");
  float_writer->Update();
  std::cout << "CC-filtered image saved" << std::endl;
  */

  // Compute the number of non-zero voxels in the images and the overlapping
  // region of the two images derived from meshes and the input binary image
  unsigned long mesh1nonZ = 0, mesh2nonZ = 0, inputnonZ = 0, overlap = 0;
  unsigned long mesh1overlap = 0, mesh2overlap = 0;
  ConstRegionIteratorType m1iIter(mesh1image, mesh1image->GetRequestedRegion()); 
  ConstRegionIteratorType m2iIter(mesh2image, mesh2image->GetRequestedRegion()); 
  ConstRegionIteratorType inputIter(input_image, input_image->GetRequestedRegion());
  for(m1iIter.GoToBegin(),m2iIter.GoToBegin(),inputIter.GoToBegin();!inputIter.IsAtEnd();
    ++m1iIter,++m2iIter,++inputIter){
    int input_val = inputIter.Value();
    ImageType::IndexType idx = inputIter.GetIndex();
    if(m1iIter.Value()==1){
      mesh1nonZ++;
      if(input_val)
        mesh1overlap++;
    }
    if(m2iIter.Value()==1){
      mesh2nonZ++;
      if(input_val)
        mesh2overlap++;
    }
    if(inputIter.Value()==1)
      inputnonZ++;
    
    if(mesh1image->GetPixel(idx)==1 && mesh2image->GetPixel(idx)==1)
      overlap++;
  }
  
  std::cout << "Input image non-zero voxels: " << inputnonZ << std::endl;
  std::cout << "Mesh 1 image non-zero voxels: " << mesh1nonZ << std::endl;
  std::cout << "Mesh 2 image non-zero voxels: " << mesh2nonZ << std::endl;
  std::cout << "Overlapped non-zero between 1&2 mesh images voxels: " << overlap << std::endl;
  std::cout << "Overlap between mesh 1 and input: " << mesh1overlap << std::endl;
  std::cout << "Overlap between mesh 2 and input: " << mesh2overlap << std::endl;
  std::cout << "Fitness for mesh 1: " << (2.0*mesh1overlap)/(inputnonZ+mesh1nonZ)*100.0 << "%" << std::endl;
  std::cout << "Fitness for mesh 2: " << (2.0*mesh2overlap)/(inputnonZ+mesh2nonZ)*100.0 << "%" << std::endl;
  
    
  image_writer->SetFileName("mesh1image.mhd");
  image_writer->SetInput(mesh1image);
  try{
    image_writer->Update();
  } catch(ExceptionObject &e){
    std::cout << "Failed to save resulting image: " << e << std::endl;
    return -1;
  }
  
  image_writer->SetFileName("mesh2image.mhd");
  image_writer->SetInput(mesh2image);
  try{
    image_writer->Update();
  } catch(ExceptionObject &e){
    std::cout << "Failed to save resulting image: " << e << std::endl;
    return -1;
  }

  return 0;
}

bool IsPointInsideTetra(MeshType::CoordRepType* voxCenter, MeshType* mesh_in, TetrahedronType* tet_in){
  TetrahedronType::PointIdIterator tetPtIterator;
  
  double p[3], v[4][3];
  p[0] = voxCenter[0];
  p[1] = voxCenter[1];
  p[2] = voxCenter[2];

  int ii = 0;
  MeshType::PointType tetVert;
  tetPtIterator = tet_in->PointIdsBegin();
  mesh_in->GetPoint(*tetPtIterator++, &tetVert);
  while(tetPtIterator != tet_in->PointIdsEnd()){
    v[ii][0] = tetVert[0];
    v[ii][1] = tetVert[1];
    v[ii][2] = tetVert[2];
    mesh_in->GetPoint(*tetPtIterator++, &tetVert);
  }
  // check that the tetra has correct orientation
  if(orient3d(&v[0][0], &v[1][0], &v[2][0], &v[3][0])<0){
    double temp;
    for(ii=0;ii<3;ii++){
      temp = v[0][ii];
      v[0][ii] = v[1][ii];
      v[1][ii] = temp;
    }
  }
  // the point is inside iff all of the four tetras formed by the point and
  // the tetra faces have the same orientation
  if(orient3d(&p[0], &v[1][0], &v[2][0], &v[3][0])<0)
    return false;
  if(orient3d(&p[0], &v[0][0], &v[2][0], &v[1][0])<0)
    return false;
  if(orient3d(&p[0], &v[0][0], &v[3][0], &v[2][0])<0)
    return false;
  if(orient3d(&p[0], &v[0][0], &v[1][0], &v[3][0])<0)
    return false;
  return true;
}

void GetImageFromMesh(ImageType* image_in, MeshType* mesh_in, ImageType* mesh1image){
  /* For each voxel in the mesh images, we have to find out if it's inside or
   * outside the mesh. One way is to traverse all voxels in the image, and
   * check if it is inside either of the volume cells. This will give
   * O(N_voxels*N_cells) complexity. However, we can also go through all
   * tetrahedra, and for each tetrahedra find the bounding box of voxels which
   * fits the tetrahedra, then iterate through that region and check
   * inside/outside for all voxels in the bounding box. This will have
   * complexity O(N_cells*N_bb_voxels). If the largest edge in the mesh is L,
   * then it is O(N_cells*L^3). Usually, L<(N_voxels^{1/3}/10)*spacing. This
   * will hopefully be some 1000 times faster ;)
   */
  
  MeshType::PointsContainer::ConstIterator pointsIterator;
  MeshType::CellsContainer::ConstIterator cellsIterator;
  TetrahedronType *tetPtr;
  TetrahedronType::PointIdIterator tetPtIterator;
  ImageType::SpacingType imageSpacing = image_in->GetSpacing();
  
  // Create the image which corresponds to mesh 1
  cellsIterator = mesh_in->GetCells()->Begin();
  unsigned i, j=0;
  while(cellsIterator != mesh_in->GetCells()->End()){
    assert(cellsIterator.Value()->GetType() == MeshType::CellType::TETRAHEDRON_CELL);
    tetPtr = dynamic_cast<TetrahedronType*>(cellsIterator.Value());
    MeshType::PointType mpbb1, mpbb2, tmpPt;  // mesh point bounding box
//    ImageType::PointType bb1, bb2;
//    ImageType::IndexType rbb1, rbb2;
    tetPtIterator = tetPtr->PointIdsBegin();
    mesh_in->GetPoint(*tetPtIterator++, &mpbb1);
    mpbb2 = mpbb1;
    while(tetPtIterator != tetPtr->PointIdsEnd()){
      mesh_in->GetPoint(*tetPtIterator, &tmpPt);
      for(i=0;i<3;i++){
        if(tmpPt[i] < mpbb1[i])
          mpbb1[i] = tmpPt[i];
        if(tmpPt[i] > mpbb2[i])
          mpbb2[i] = tmpPt[i];
      }
      tetPtIterator++;
    }
    // get voxel bounding box
    ImageType::PointType ipbb1, ipbb2;  // image point bounding box
    ImageType::IndexType ibb1, ibb2;    // index bounding box
    for(i=0;i<3;i++){
      ipbb1[i] = mpbb1[i];
      ipbb2[i] = mpbb2[i];
    }
    mesh1image->TransformPhysicalPointToIndex(ipbb1, ibb1);
    mesh1image->TransformPhysicalPointToIndex(ipbb2, ibb2);
    /*
    std::cout << "Mesh bounding box: (" << mpbb1[0] << "," << mpbb1[1] << "," << mpbb1[2] << ")" << std::endl;
    std::cout << "Mesh bounding box: (" << mpbb2[0] << "," << mpbb2[1] << "," << mpbb2[2] << ")" << std::endl;
    std::cout << "Image region bounding box: (" << ibb1[0] << "," << ibb1[1] << "," << ibb1[2] << ")" << std::endl;
    std::cout << "Image region bounding box: (" << ibb2[0] << "," << ibb2[1] << "," << ibb2[2] << ")" << std::endl;
    */
    ImageType::RegionType voxelBBregion;
    ImageType::RegionType::IndexType rStart;
    ImageType::RegionType::SizeType rSize;
    rStart[0] = ibb1[0]-3; // being a paranoid
    rStart[1] = ibb1[1]-3;
    rStart[2] = ibb1[2]-3;
    rSize[0] = ibb2[0] - ibb1[0] + 3;
    rSize[1] = ibb2[1] - ibb1[1] + 3;
    rSize[2] = ibb2[2] - ibb1[2] + 3;
    voxelBBregion.SetSize(rSize);
    voxelBBregion.SetIndex(rStart);
    // Iterate through the voxels within the bounding box
    RegionIteratorType regionIter(mesh1image, voxelBBregion);
    for(regionIter.GoToBegin();!regionIter.IsAtEnd();++regionIter){
      MeshType::CoordRepType voxCenter[3], closestPoint[3], pcoord[3];
      ImageType::IndexType idx = regionIter.GetIndex();
      voxCenter[0] = idx[0]*imageSpacing[0];
      voxCenter[1] = idx[1]*imageSpacing[1];
      voxCenter[2] = idx[2]*imageSpacing[2];
      if(tetPtr->EvaluatePosition(voxCenter, mesh_in->GetPoints(),
          NULL, NULL, NULL, NULL))
        regionIter.Set(1);
      /*
      voxCenter[0] += imageSpacing[0]/2.0;
      voxCenter[1] += imageSpacing[1]/2.0;
      voxCenter[2] += imageSpacing[2]/2.0;
      if(tetPtr->EvaluatePosition(voxCenter, mesh_in->GetPoints(),
          NULL, NULL, NULL, NULL))
        regionIter.Set(1);
      */
      if(IsPointInsideTetra(voxCenter, mesh_in, tetPtr))
        regionIter.Set(1);
    }
    //break;
    j++;
    cellsIterator++;
//    std::cout << "Cell " << j << " of " << vol_mesh1->GetCells()->Size() << std::endl;
  }
}
