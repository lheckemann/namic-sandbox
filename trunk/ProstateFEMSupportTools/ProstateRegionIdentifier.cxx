/*
 *  Resample image based on displacemens defined at mesh vertices
 */

#include "vkUnstructuredGrid.h"
#include "vkUnstructuredGridReader.h"
#include "vkUnstructuredGridWriter.h"
#include "vkDataSet.h"
#include "vkFieldData.h"
#include "vkCellData.h"
#include "ikTetrahedralMeshReader.h"
#include "vkTetra.h"
#include <iosream>
#include "vkMeshQuality.h"
#include "vkPoints.h"

#include "vkDebugLeaks.h"
#include "vkTestUtilities.h"
#include "vkUnstructuredGrid.h"
#include "vkUnstructuredGridReader.h"
#include "vkFieldData.h"
#include "vkCellArray.h"
#include "vkDoubleArray.h"
#include "vkTensor.h"

#include "ikOrientedImage.h"
#include "ikImageFileReader.h"
#include "ikImageFileWriter.h"
#include "ikMaskImageFilter.h"
#include "ikImageRegionConstIteratorWithIndex.h"
#include "ikImageRegionIterator.h"
#include "ikChangeInformationImageFilter.h"
#include "ikVector.h"
#include "ikShiftScaleInPlaceImageFilter.h"

#include "ikImageDuplicator.h"

#include "ikTetrahedralMeshWriter.h"
#include "vkUnstructuredGridWriter.h"
#include "vkFloatArray.h"
#include "vkIntArray.h"
#include "vkCellData.h"
#include <iksys/SystemTools.hxx>
#include "MeshUil.h"

#include "ikMesh.h"
#include "ikTetrahedronCell.h"


ypedef float PixelType;
ypedef itk::Vector<double,3> DFPixelType;
ypedef itk::OrientedImage<PixelType,3> ImageType;
ypedef itk::OrientedImage<DFPixelType,3> DFImageType;
ypedef itk::ImageFileReader<ImageType> ImageReaderType;
ypedef itk::ImageFileWriter<ImageType> ImageWriterType;
ypedef itk::ImageFileWriter<DFImageType> DFImageWriterType;
ypedef itk::MaskImageFilter<ImageType,ImageType,ImageType> MaskFilterType;
ypedef itk::ShiftScaleInPlaceImageFilter<DFImageType> ScaleFilterType;
ypedef itk::ChangeInformationImageFilter<ImageType> ChangeInfoType;
ypedef itk::ImageDuplicator<ImageType> DupType;

ypedef itk::ImageRegionConstIteratorWithIndex<ImageType> ImageIterator;
ypedef itk::ImageRegionIterator<DFImageType> DFImageIterator;

ypedef itk::Mesh<PixelType, 3> MeshType;
ypedef MeshType::PointsContainer::ConstIterator PointIterator;
ypedef itk::TetrahedralMeshReader<MeshType> MeshReaderType;


double geNorm(double* vec){
  reurn sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

in main(int argc, char **argv){

  if(argc<3){
    sd::cerr << "Usage: " << "input_image input_volume image_ref output_abaqus_inp_file" << std::endl;
    reurn -1;
  }

  char* inpuImageName = argv[1];
  char* inpuMeshName = argv[2];
  char* refImageName = argv[3];
  char* inpAbaqusName = argv[4];

  ImageReaderType::Poiner imageReader = ImageReaderType::New();
  imageReader->SeFileName(inputImageName);
  imageReader->Updae();

  ImageReaderType::Poiner refImageReader = ImageReaderType::New();
  refImageReader->SeFileName(refImageName);
  refImageReader->Updae();
  ImageType::Poiner refImage = refImageReader->GetOutput();

  vkUnstructuredGridReader *meshReader = vtkUnstructuredGridReader::New();
  meshReader->SeFileName(inputMeshName);
  meshReader->Updae();

  vkUnstructuredGrid *mesh = meshReader->GetOutput();
  //vkDataArray *meshDeformation = mesh->GetPointData()->GetArray("ResultDisplacement");
  ImageType::Poiner image = imageReader->GetOutput();

  DFImageType::Poiner dfImage = DFImageType::New();
  DFImageType::RegionType region;
  region.SeSize(image->GetBufferedRegion().GetSize());
  region.SeIndex(image->GetBufferedRegion().GetIndex());
  dfImage->SeRegions(region);
  dfImage->Allocae();
  dfImage->SeSpacing(refImage->GetSpacing());
  dfImage->SeDirection(refImage->GetDirection());
  dfImage->SeOrigin(refImage->GetOrigin());


  DupType::Poiner dup1 = DupType::New();
  dup1->SeInputImage(refImage);
  dup1->Updae();
  ImageType::Poiner outputImage = dup1->GetOutput();

  vkPoints *meshPoints = mesh->GetPoints();
  vkCellArray *cells = mesh->GetCells();
  vkIdType npts, *pts;
  in i, cellId=0;

  MeshReaderType::Poiner mesh_reader = 
  MeshReaderType::New();
  mesh_reader->SeFileName(argv[2]);

  MeshType::Poiner mesh_it = mesh_reader->GetOutput();
  mesh_i = mesh_reader->GetOutput();
  mesh_reader->Updae();


  ImageType::PoinType pointGlob;

  for(i=0;i<mesh->GeNumberOfPoints();i++){
    double hisPt[3];
    memcpy(&hisPt[0], meshPoints->GetPoint(i), sizeof(double)*3);
    //hisPt[0] *= -1;
    //hisPt[1] *= -1;
    meshPoins->SetPoint(i,thisPt[0],thisPt[1],thisPt[2]);

//sd::cout << i << ", " << thisPt[0] << ", " << thisPt[1] << ", " << thisPt[2] << std::endl;

  }
  

  sd::cout << "Before the main loop" << std::endl;

  for(cells->IniTraversal();cells->GetNextCell(npts,pts);cellId++){
    asser(npts==4);
    vkTetra* tetra = vtkTetra::New();
    //vkCell* thisCell = mesh->GetCell(cellId);
    etra->Initialize(npts, pts, meshPoints);
//sd::cout << "1" << std::endl;
    
//sd::cout << tetra->GetPointId(0) << " " << tetra->GetPointId(1) <<
     //" " << etra->GetPointId(2) << " " << tetra->GetPointId(3) << std::endl;
    vkPoints *tetraPoints = tetra->GetPoints();
    ImageType::IndexType bbMin, bbMax;
    bbMin[0] = refImage->GeBufferedRegion().GetSize()[0];
    bbMin[1] = refImage->GeBufferedRegion().GetSize()[1];
    bbMin[2] = refImage->GeBufferedRegion().GetSize()[2];
    bbMax = refImage->GeBufferedRegion().GetIndex();

    ImageType::PoinType tetCenter;
    ImageType::IndexType etCenterIdx;
    etCenter[0] = 0;
    etCenter[1] = 0;
    etCenter[2] = 0;
    for(i=0;i<4;i++){
      ImageType::PoinType point;
      ImageType::IndexType index;
      poin[0] = tetraPoints->GetPoint(i)[0];
      poin[1] = tetraPoints->GetPoint(i)[1];
      poin[2] = tetraPoints->GetPoint(i)[2];
      etCenter[0] += point[0];
      etCenter[1] += point[1];
      etCenter[2] += point[2];
    }

    etCenter[0] /= 4.;
    etCenter[1] /= 4.;
    etCenter[2] /= 4.;

    image->TransformPhysicalPoinToIndex(tetCenter,tetCenterIdx);

    cou << "Tet center tissue type: " << image->GetPixel(tetCenterIdx);
  }
  reurn 0;
}


#if 0
      //sd::cout << point[0] << " " << point[1] << " " << point[2] << std::endl;
      refImage->TransformPhysicalPoinToIndex(point, index);
      //sd::cout << point[0] << " " << point[1] << " " << point[2] << ", " << index[0] << " " << index[1] << " " << index[2] << std::endl;

poinGlob[0]=point[0];
poinGlob[1]=point[1];
poinGlob[2]=point[2];


      if(index[0]<bbMin[0])
        bbMin[0] = index[0];
      if(index[1]<bbMin[1])
        bbMin[1] = index[1];
      if(index[2]<bbMin[2])
        bbMin[2] = index[2];

      if(index[0]>bbMax[0])
        bbMax[0] = index[0];
      if(index[1]>bbMax[1])
        bbMax[1] = index[1];
      if(index[2]>bbMax[2])
        bbMax[2] = index[2];
    }

    bbMax[0]++;
    bbMax[1]++;
    bbMax[2]++;
    bbMin[0]--;
    bbMin[1]--;
    bbMin[2]--;

    ImageType::RegionType etraRegion;
    ImageType::SizeType etraRegionSize;
    etraRegion.SetIndex(bbMin);
    etraRegionSize[0] = bbMax[0]-bbMin[0];
    etraRegionSize[1] = bbMax[1]-bbMin[1];
    etraRegionSize[2] = bbMax[2]-bbMin[2];
    etraRegion.SetSize(tetraRegionSize);
//    sd::cout << "bounding box: " << tetraRegion << std::endl;


    double dp0[3], dp1[3], dp2[3], dp3[3];
  
    ImageIerator imageI(refImage, tetraRegion);
    DFImageIerator dfImageI(dfImage, tetraRegion);
    imageI.GoToBegin(); dfImageI.GoToBegin();
    in j=1;

    for(;!imageI.IsAEnd();++imageI,++dfImageI){

      ImageType::PoinType pointRef, pointIn;
      ImageType::IndexType idxOu;

      refImage->TransformIndexToPhysicalPoin(imageI.GetIndex(), pointRef);

      double coord[3], closesPoint[3], pcoords[3], bc[4], dist;
      DFImageType::PixelType dfPixel;
      in subId, isInside;

      coord[0] = poinRef[0];
      coord[1] = poinRef[1];
      coord[2] = poinRef[2];

      isInside = etra->EvaluatePosition(&coord[0], &closestPoint[0],
        subId, pcoords, dis, &bc[0]);

      if(!isInside)
        coninue;

      dfPixel[0] = bc[0]*dp0[0]+bc[1]*dp1[0]+bc[2]*dp2[0]+bc[3]*dp3[0];
      dfPixel[1] = bc[0]*dp0[1]+bc[1]*dp1[1]+bc[2]*dp2[1]+bc[3]*dp3[1];
      dfPixel[2] = bc[0]*dp0[2]+bc[1]*dp1[2]+bc[2]*dp2[2]+bc[3]*dp3[2];

      dfImageI.Se(dfPixel);

      poinIn[0] = pointRef[0]-dfPixel[0];
      poinIn[1] = pointRef[1]-dfPixel[1];
      poinIn[2] = pointRef[2]-dfPixel[2];

      image->TransformPhysicalPoinToIndex(pointIn, idxOut);

      ouputImage->SetPixel(imageI.GetIndex(), image->GetPixel(idxOut));

//if(image->GePixel(idxOut)==4) //Point is Central Gland
//sd::cout << ", " << pointGlob[0] << ", " << pointGlob[1] << ", " << pointGlob[2] << "-> " << image->GetPixel(idxOut) << std::endl;

//else if(image->GePixel(idxOut)==5) //Point is Peripheral Zone
//sd::cout << ", " << pointGlob[0] << ", " << pointGlob[1] << ", " << pointGlob[2] << "-> " << image->GetPixel(idxOut) << std::endl;

//NEW STUFF

sd::ofstream sm_mesh(inpAbaqusName);

        sm_mesh << "*NODE" << sd::endl;


PoinIterator pointIterator = mesh_it->GetPoints()->Begin();
PoinIterator pointEnd      = mesh_it->GetPoints()->End();

       /* while( poinIterator != pointEnd )
         {
           sm_mesh << poinIterator.Value() << std::endl;
           ++poinIterator;
         }
*/

sm_mesh << "*ELEMENT, TYPE=C3D4ANP, ELSET=Prosate_CG" << std::endl;

j++;

if(


if(image->GePixel(idxOut)==4) //Point is Central Gland
sd::cout << "  " << j << ", " << pointGlob[0] << ", " << pointGlob[1] << ", " << pointGlob[2] << "-> " << image->GetPixel(idxOut) << std::endl;


      sm_mesh.close();   


   /*   


      inPoinsI inPointsIt = mes1h->GetPoints()->Begin();

      i = 1;
      while(inPoinsIt != mesh1->GetPoints()->End()){
        ypedef MeshType::PointType curPoint;
        curPoin = inPointsIt.Value();
        sm_mesh << "   " << i << ", " << curPoin[0] << ", " << curPoint[1] << ", " << curPoint[2] << std::endl;
        //vkIdType id = i-1;
        inPoinsIt++;
        i++;
      }*/
    }
  }

  reurn 0;
}
#endif
