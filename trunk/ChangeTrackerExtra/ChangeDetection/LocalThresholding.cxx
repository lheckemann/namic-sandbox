/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBinaryMask3DQuadEdgeMeshSourceTest.cxx,v $
  Language:  C++
  Date:      $Date: 2009-04-06 11:27:19 $
  Version:   $Revision: 1.3 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>

#include "itkOrientedImage.h"
#include "itkContinuousIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkQuadEdgeMesh.h"
#include "itkTriangleCell.h"

#include "itkBinaryMask3DMeshSource.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshScalarDataVTKPolyDataWriter.h"
#include "itkQuadEdgeMeshDecimationCriteria.h"
#include "itkQuadEdgeMeshQuadricDecimation.h"
#include "itkQuadEdgeMeshSquaredEdgeLengthDecimation.h"
#include "itkQuadEdgeMeshNormalFilter.h"
#include "itkQuadEdgeMeshExtendedTraits.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkConstNeighborhoodIterator.h"

#include "itkBSplineScatteredDataPointSetToImageFilter.h"
#include "itkPointSet.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

#include "vtkPolyData.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkDataSet.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkCharArray.h"

#include "vtkMassProperties.h"
#include "vtkWarpVector.h"

#include <algorithm>
#include <assert.h>

#define CWIDTH 5 // CWIDTH*2+1 is the number of points used to calculate NCC
#define SWIDTH 6 // (SWIDTH+CWIDTH)*2+1 is the total number of points sampled o
#define CENTERPT (CWIDTH+SWIDTH)
#define LINESPACING .25
#define SAVE_LINE_PROFILES 0
#define DECIMATION_CONST 1
#define NCCTHRESHOLD .1

  // Define the dimension of the images
  const unsigned int Dimension = 3;
  typedef double PixelType;

  // Declare the types of the output images
  typedef itk::OrientedImage<PixelType,   Dimension>   ImageType; // float to use uniform for all images
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::ImageFileWriter<ImageType> WriterType;
  typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThreshType;
  typedef itk::BSplineInterpolateImageFunction<ImageType,PixelType> InterpolatorType;


  typedef itk::Vector< PixelType, 1 >         SDAVectorType;
  typedef itk::Image< SDAVectorType, Dimension > SDAVectorImageType;
  typedef SDAVectorImageType::PixelType SDAVectorPixelType;

  typedef itk::PointSet< SDAVectorPixelType, Dimension >      PointSetType;  
  typedef PointSetType::Pointer                   PointSetPointer;
  typedef PointSetType::PointType                 PointSetPointType;
  typedef PointSetType::PointsContainerPointer    PointsContainerPointer;
  typedef PointSetType::PointDataContainerPointer PointDataContainerPointer;
  typedef PointSetType::PointDataContainer        PointDataContainer;

  typedef itk::VectorIndexSelectionCastImageFilter< SDAVectorImageType,
    ImageType > IndexFilterType;
  typedef IndexFilterType::Pointer IndexFilterPointer;
  typedef itk::BSplineScatteredDataPointSetToImageFilter< PointSetType,
    SDAVectorImageType > SDAFilterType;
  typedef SDAFilterType::Pointer SDAFilterPointer;


  typedef itk::Index<Dimension>                     IndexType;
  typedef itk::Size<Dimension>                      SizeType;
  typedef itk::ImageRegion<Dimension>               RegionType;
  typedef ImageType::PixelType                      PixelType;

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  // Declare the type of the Mesh
  typedef float CoordType;
  typedef itk::Vector<CoordType,3> VectorType;
  typedef itk::QuadEdgeMeshExtendedTraits <
    VectorType,
    Dimension,
    2,
    CoordType,
    CoordType,
    VectorType,
    bool,
    bool > Traits;
//  typedef itk::QuadEdgeMesh<CoordType, 3>              MeshType;
//  typedef itk::QuadEdgeMesh<CoordType, 3>              OutputMeshType;
  typedef itk::QuadEdgeMesh < VectorType, Dimension, Traits > MeshType;
  typedef itk::QuadEdgeMesh < VectorType, Dimension, Traits > OutputMeshType;
  typedef MeshType::PointType                       PointType;
  typedef itk::QuadEdgeMeshScalarDataVTKPolyDataWriter< MeshType >  MeshWriterType;
//  typedef itk::VTKPolyDataWriter<MeshType> MeshWriterType;

  typedef itk::QuadEdgeMeshNormalFilter< MeshType, OutputMeshType > NormalFilterType;
  typedef itk::BinaryMask3DMeshSource< ImageType, MeshType >   MeshSourceType;
  
  typedef OutputMeshType::PointsContainer::Iterator PointsIterator;
  typedef OutputMeshType::CellsContainer::Iterator CellsIterator;
  typedef OutputMeshType::PointDataContainerIterator PointDataIterator;

  typedef itk::TriangleCell<MeshType::CellType> TriangleType;

void ShiftVector(std::vector<PixelType>&, int);
vtkPolyData* ITKMesh2PolyData(MeshType::Pointer);
void PrintSurfaceStatistics(vtkPolyData*);
vtkFloatArray* SmoothVMF(MeshType::Pointer,vtkFloatArray*);
vtkFloatArray* SmoothVDF(MeshType::Pointer,vtkFloatArray*);
float SurfaceVolume(vtkPolyData*);
void WriteMesh(MeshType::Pointer, const char*);

int main(int argc, char **argv){


  if(argc<4){
    std::cerr << "Usage: " << argv[0] << " binary_image image1 image2" << std::endl;
    return -1;
  }

  const char* inputMaskName = argv[1];
  const char* inputImage1Name = argv[2];
  const char* inputImage2Name = argv[3];
  char* outputMeshName = "mesh.vtk";
  if(argc>4)
    outputMeshName = argv[4];

  std::cout << "Parameters:" << std::endl;
  std::cout << " * correlation window width: " << CWIDTH*2+1 << std::endl;
  std::cout << " * sampling window width:    " << (SWIDTH+CWIDTH)*2+1 << std::endl;
  std::cout << " * line sample spacing:      " << LINESPACING << std::endl;
  std::cout << " * surface decimation const: " << DECIMATION_CONST << std::endl;
  std::cout << " * NCC threshold:            " << NCCTHRESHOLD << std::endl;

  ImageType::Pointer mask;

#ifdef USE_SPHERE_IMAGE
  // Declare the type of the index,size and region to initialize images
  typedef itk::Index<Dimension>                     IndexType;
  typedef itk::Size<Dimension>                      SizeType;
  typedef itk::ImageRegion<Dimension>               RegionType;
  typedef ImageType::PixelType                      PixelType;

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  // Declare the type of the Mesh

  const PixelType backgroundValue = 0;
  const PixelType internalValue   = 1;
  
  SizeType size;
  size[0] = 128;  
  size[1] = 128;  
  size[2] = 128;  

  IndexType start;
  start.Fill(0);

  RegionType region;
  region.SetSize( size );
  region.SetIndex( start );
    
  mask = ImageType::New();

  mask->SetRegions( region );
  mask->Allocate();

  mask->FillBuffer( backgroundValue );

  IteratorType it( mask, region );
  it.GoToBegin();

  PointType             point;
  PointType             center;
  PointType::VectorType radial;
 
  IndexType centralIndex = start;
  centralIndex[0] += size[0] / 2;
  centralIndex[1] += size[1] / 2;
  centralIndex[2] += size[2] / 2;

  mask->TransformIndexToPhysicalPoint( centralIndex, center );
  
  // 
  //  Create a digitized sphere in the middle of the image.
  //
  while( !it.IsAtEnd() ) 
    {
    mask->TransformIndexToPhysicalPoint( it.GetIndex(), point );
    radial = point - center;
    if ( radial.GetNorm() < 30.0)
      {
      it.Set( 255 );
      }
    ++it;
    }

#else // USE_SPHERE_IMAGE
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputMaskName);
  
  
  ThreshType::Pointer thresh = ThreshType::New();
  thresh->SetInput(reader->GetOutput());
  thresh->SetLowerThreshold(1);
  thresh->SetUpperThreshold(255);
  thresh->SetInsideValue(255);  

  mask = thresh->GetOutput();

#endif // USE_SPHERE_IMAGE

  ReaderType::Pointer r1 = ReaderType::New();
  ReaderType::Pointer r2 = ReaderType::New();
  r1->SetFileName(inputImage1Name);
  r2->SetFileName(inputImage2Name);
  r1->Update();
  r2->Update();
  
  ImageType::Pointer image1 = r1->GetOutput(), image2 = r2->GetOutput();

  MeshSourceType::Pointer meshSource = MeshSourceType::New();

  meshSource->SetInput( mask );
  meshSource->SetObjectValue( 255 );
  meshSource->Update();

  std::cout << "MC surface points: " << meshSource->GetNumberOfNodes() << std::endl;
  std::cout << "MC surface cells: " << meshSource->GetNumberOfCells() << std::endl;

  // decimate the mesh
  typedef itk::NumberOfFacesCriterion< MeshType > CriterionType;
  typedef itk::QuadEdgeMeshSquaredEdgeLengthDecimation< 
    MeshType, MeshType, CriterionType > DecimationType;
//  typedef itk::QuadEdgeMeshQuadricDecimation< 
//    MeshType, MeshType, CriterionType > DecimationType;

  CriterionType::Pointer criterion = CriterionType::New();
  criterion->SetTopologicalChange( false );
  std::cout << "Target number of cells after decimation: " << 
    (unsigned) (DECIMATION_CONST*meshSource->GetNumberOfCells()) << std::endl;
  criterion->SetNumberOfElements( unsigned(DECIMATION_CONST*meshSource->GetNumberOfCells()));
  
  DecimationType::Pointer decimate = DecimationType::New();
  decimate->SetInput( meshSource->GetOutput() );
  decimate->SetCriterion( criterion );
  decimate->Update();
  
  MeshType::Pointer mcmesh = meshSource->GetOutput();
  MeshType::Pointer dMesh = decimate->GetOutput();
  std::cout << "Decimation complete" << std::endl;
  std::cout << "Decimated surface points: " << dMesh->GetPoints()->Size() << std::endl;
  std::cout << "Decimated surface cells: " << dMesh->GetCells()->Size() << std::endl;

  WriteMesh(dMesh, "decimated_mesh.vtk");

  /*
   * Uncomment this code to reproduce the problem generating normals
   */
  NormalFilterType::Pointer normals = NormalFilterType::New( );
  normals->SetInput( decimate->GetOutput() );
  //normals->SetWeight(GOURAUD);  // 
  normals->Update( );
  std::cout << "Normals calculated" << std::endl;

  OutputMeshType::Pointer mesh = normals->GetOutput();
  OutputMeshType::PointDataContainerPointer pointData = 
    mesh->GetPointData();
  PointDataIterator pdIt = mesh->GetPointData()->Begin(), pdItEnd = mesh->GetPointData()->End();
  PointsIterator pIt = mesh->GetPoints()->Begin(), pItEnd = mesh->GetPoints()->End();

  // Note: normals are oriented outwards
  InterpolatorType::Pointer interp1 = InterpolatorType::New();
  interp1->SetSplineOrder(3);
  interp1->SetInputImage(image1);
  InterpolatorType::Pointer interp2 = InterpolatorType::New();
  interp2->SetSplineOrder(3);
  interp2->SetInputImage(image2);

  // this is the point set that will be used by bspline interpolator later
  PointSetType::Pointer pointSet = PointSetType::New();
  PointsContainerPointer psContainer = pointSet->GetPoints();
  psContainer->Reserve(mesh->GetPoints()->Size());
  PointDataContainerPointer pdContainer = PointDataContainer::New();
  pdContainer->Reserve(mesh->GetPoints()->Size());
  pointSet->SetPointData( pdContainer );
 
 
  int progress1 = 0, progress2 = 0, totalPoints = mesh->GetPoints()->Size();
  
  // arrays to keep the surface shift measurements
  vtkFloatArray *nccShiftArray = vtkFloatArray::New();
  nccShiftArray->SetNumberOfComponents(3);
  nccShiftArray->SetNumberOfTuples(totalPoints);
  nccShiftArray->SetName("NCCShift");

  vtkFloatArray *dnccShiftArray = vtkFloatArray::New();
  dnccShiftArray->SetNumberOfComponents(3);
  dnccShiftArray->SetNumberOfTuples(totalPoints);
  dnccShiftArray->SetName("dNCCShift");

  vtkFloatArray *nccArray = vtkFloatArray::New();
  nccArray->SetNumberOfComponents(1);
  nccArray->SetNumberOfTuples(totalPoints);
  nccArray->SetName("Correlation");

  vtkFloatArray *dnccArray = vtkFloatArray::New();
  dnccArray->SetNumberOfComponents(1);
  dnccArray->SetNumberOfTuples(totalPoints);
  dnccArray->SetName("dCorrelation");

  vtkCharArray *dirArray = vtkCharArray::New();
  dirArray->SetNumberOfComponents(1);
  dirArray->SetNumberOfTuples(totalPoints);
  dirArray->SetName("Direction");

  vtkCharArray *ddirArray = vtkCharArray::New();
  ddirArray->SetNumberOfComponents(1);
  ddirArray->SetNumberOfTuples(totalPoints);
  ddirArray->SetName("dDirection");

  std::vector<float> dnccRanked;

  while(pIt!=pItEnd){    
    OutputMeshType::PixelType normal = pdIt->Value();
    OutputMeshType::PointType point = pIt->Value();

    unsigned i = 0;
    
//    progress2 = (int) ((float)pIt->Index()/totalPoints*100.);
//    if(progress1 != progress2 && !(progress2%10)){
//      std::cout << progress2 << " ";
//      progress1 = progress2;
//    }

//    std::cout << "Point: " << pIt->Index() << " " << point << std::endl;
//    std::cout << "Normal: " << normal << std::endl;
    

#if SAVE_LINE_PROFILES  
    char fname[10];
    sprintf(&fname[0], "point%i", (int)pIt->Index());    
    std::ofstream pfs1((std::string(fname)+"_p1.dat").c_str());
    std::ofstream pfs2((std::string(fname)+"_p2.dat").c_str());
    std::ofstream sfs((std::string(fname)+"_s.dat").c_str());
    std::ofstream dsfs((std::string(fname)+"_ds.dat").c_str());
    std::ofstream dfs1((std::string(fname)+"_d1.dat").c_str());
    std::ofstream dfs2((std::string(fname)+"_d2.dat").c_str());
#endif // SAVE_LINE_PROFILES

    i = 0;
    std::vector<PixelType> profile1;
    std::vector<PixelType> profile2;
    std::vector<PixelType> dprofile1;
    std::vector<PixelType> dprofile2;
    PixelType mean1 = 0, mean2 = 0, std1 = 0, std2 = 0, delta;
    PixelType dmean1 = 0, dmean2 = 0, dstd1 = 0, dstd2 = 0, ddelta;
    double sampling = LINESPACING;
    for(double step=-CWIDTH-SWIDTH;step<=CWIDTH+SWIDTH;step++,i++){
      ImageType::PointType imagePt;
      ImageType::IndexType imageIdx;
      itk::ContinuousIndex<PixelType,3> imageContIdx;

      imagePt[0] = point[0]+normal[0]*step*sampling;
      imagePt[1] = point[1]+normal[1]*step*sampling;
      imagePt[2] = point[2]+normal[2]*step*sampling;

      ImageType::PixelType v1, v2;
      if(!mask->TransformPhysicalPointToIndex(imagePt,imageIdx)){
        std::cerr << "Point maps outside image!" << std::endl;
        abort();
      } else {
        mask->TransformPhysicalPointToContinuousIndex(imagePt,imageContIdx);
        v1 = interp1->EvaluateAtContinuousIndex(imageContIdx);
        v2 = interp2->EvaluateAtContinuousIndex(imageContIdx);
      }

      profile1.push_back(v1);
      profile2.push_back(v2);
      dprofile1.push_back(v1);      
      dprofile2.push_back(v2);
      mean1 += v1;
      mean2 += v2;
//      if(step<0)
//        mask->SetPixel(imageIdx, mean);
//      else
//        mask->SetPixel(imageIdx, median);
      if(i>0){ // calculate the derivative
        dprofile1[i-1] = profile1[i]-profile1[i-1];
        dprofile2[i-1] = profile2[i]-profile2[i-1];
        dmean1 += dprofile1[i-1];
        dmean2 += dprofile2[i-1];
      }
      if(step == CWIDTH+SWIDTH){
        dprofile1[i] = dprofile1[i-1];
        dprofile2[i] = dprofile2[i-1];
        dmean1 += dprofile1[i];
        dmean2 += dprofile2[i];
      }
      
    }

//    pfs1 << i << " " << mean1 << std::endl;
//    pfs2 << i << " " << mean2 << std::endl;
    
    // search for the shift that maximizes the NCC between the two profiles
    // first, normalize by the mean
    mean1 /= (float)i;
    mean2 /= (float)i;
    dmean1 /= (float)i;
    dmean2 /= (float)i;
    std1 = 0; std2 = 0; dstd1 = 0; dstd2 = 0;
    for(i=0;i<(SWIDTH+CWIDTH)*2+1;i++){      
      profile1[i] -= mean1;
      profile2[i] -= mean2;      
      dprofile1[i] -= dmean1;
      dprofile2[i] -= dmean2;
      std1 += profile1[i]*profile1[i];
      std2 += profile2[i]*profile2[i];
      dstd1 += dprofile1[i]*dprofile1[i];
      dstd2 += dprofile2[i]*dprofile2[i];
    }
    std1 = sqrt(std1);
    std2 = sqrt(std2);
    dstd1 = sqrt(dstd1);
    dstd2 = sqrt(dstd2);

    float dncc, dnccMax = 0, dbestFit = 0;
    float ncc, nccMax = 0, bestFit = 0;

    for(int s=-SWIDTH;s<=SWIDTH;s++){    
      ncc = 0; dncc = 0;
      for(int p1=CENTERPT-CWIDTH-s,p2=CENTERPT-CWIDTH;p1<=CENTERPT+CWIDTH-s;p1++,p2++){
        ncc += profile1[p1]*profile2[p2];
        dncc += dprofile1[p1]*dprofile2[p2];
      }
      ncc /= std1*std2;
      dncc /= dstd1*dstd2;
      if(ncc>nccMax){
        nccMax = ncc;
        bestFit = s;
      }
      if(dncc>dnccMax){
        dnccMax = dncc;
        dbestFit = s;
      }
    }

#if SAVE_LINE_PROFILES
    for(i=0;i<(SWIDTH+CWIDTH)*2+1;i++){
      pfs1 << i << " " << dprofile1[i] << std::endl;
      pfs2 << i << " " << dprofile2[i] << std::endl;
      dfs1 << i << " " << profile1[i] << std::endl;
      dfs2 << i << " " << profile2[i] << std::endl;
    }
#endif // SAVE_LINE_PROFILES

    ShiftVector(profile1, -bestFit);
    ShiftVector(dprofile1, -dbestFit);

    dnccRanked.push_back(dnccMax);

#if SAVE_LINE_PROFILES
    for(i=0;i<(SWIDTH+CWIDTH)*2+1;i++){
      sfs << i << " " << profile1[i] << std::endl;
      dsfs << i << " " << dprofile1[i] << std::endl;
    }
#endif // SAVE_LINE_PROFILES

    nccShiftArray->InsertTuple3(pIt->Index(), normal[0]*bestFit*sampling, 
      normal[1]*bestFit*sampling, normal[2]*bestFit*sampling);
    dnccShiftArray->InsertTuple3(pIt->Index(), normal[0]*dbestFit*sampling, 
      normal[1]*dbestFit*sampling, normal[2]*dbestFit*sampling);
    nccArray->InsertTuple1(pIt->Index(), nccMax);
    dnccArray->InsertTuple1(pIt->Index(), dnccMax);
    dirArray->InsertTuple1(pIt->Index(), bestFit);
    ddirArray->InsertTuple1(pIt->Index(), dbestFit);

//    std::cout << pIt->Index() << ": " << bestFit << "(ncc=" << nccMax << ")" << std::endl;

//    if(pIt->Index()>200)
//      abort();
    ++pIt;++pdIt;
  }
  std::cout << std::endl;

  std::vector<float>::iterator dnccThreshIt = dnccRanked.begin()+NCCTHRESHOLD*dnccRanked.size();
  std::nth_element(dnccRanked.begin(), dnccThreshIt, dnccRanked.end());
  std::cout << "dNCC threshold value: " << *dnccThreshIt << std::endl;

  std::cout << "Smoothing displacements...";
  vtkFloatArray* dnccShiftArraySmooth = SmoothVMF(mesh, dnccShiftArray);
  std::cout << "done" << std::endl;

  for(unsigned i=0;i<mesh->GetPoints()->Size();i++)
    if(dnccArray->GetTuple1(i) < (*dnccThreshIt)){
      dnccShiftArray->InsertTuple3(i, 0, 0, 0);
//      dnccShiftArraySmooth->InsertTuple3(i, 0, 0, 0);
    }

  // convert itk mesh to vtk polydata, save into file together with the
  // surface displacements
  
  // report statistics for the mc surface, decimated surface, and warped
  // surface
  vtkPolyData *vtksurf = ITKMesh2PolyData(mesh);
  vtkPolyData *vtkmcsurf = ITKMesh2PolyData(mcmesh);

  vtksurf->GetPointData()->AddArray(nccShiftArray);
  nccShiftArray->Delete();
  vtksurf->GetPointData()->AddArray(dnccShiftArray);
  dnccShiftArray->Delete();
  vtksurf->GetPointData()->AddArray(dnccShiftArraySmooth);
  dnccShiftArraySmooth->Delete();
  vtksurf->GetPointData()->AddArray(dnccArray);
  dnccArray->Delete();
  vtksurf->GetPointData()->AddArray(nccArray);
  nccArray->Delete();
  vtksurf->GetPointData()->AddArray(dirArray);
  dirArray->Delete();
  vtksurf->GetPointData()->AddArray(ddirArray);
  ddirArray->Delete();

  vtkPolyDataWriter *pdw = vtkPolyDataWriter::New();
  pdw->SetFileName(outputMeshName);
  pdw->SetInput(vtksurf);
  pdw->Update();

  vtkWarpVector *warper = vtkWarpVector::New();
  vtksurf->GetPointData()->SetActiveVectors("dNCCShiftsmooth");
  warper->SetInput(vtksurf);
  warper->Update();

  float vol1, vol2;
  std::cout << " === MC surface statistics: " << std::endl;
  PrintSurfaceStatistics(vtkmcsurf);
  std::cout << " === Decimated surface statistics: " << std::endl;
  PrintSurfaceStatistics(vtksurf);
  vol1 = SurfaceVolume(vtksurf);
  std::cout << " === Warped surface statistics: " << std::endl;
  vtksurf->SetPoints(warper->GetOutput()->GetPoints());
  vol2 = SurfaceVolume(vtksurf);
  PrintSurfaceStatistics(vtksurf);
  std::cout << "Volume change: " << vol2-vol1 << std::endl;

  vtkPolyDataWriter *pdw1 = vtkPolyDataWriter::New();
  pdw1->SetFileName("warped_mesh.vtk");
  pdw1->SetInput(vtksurf);
  pdw1->Update();

  return 0;

  // setup bspline interpolator to get threshold values at each image location
  SDAFilterType::ArrayType ncps;
  ncps.Fill( 10 );

  SDAFilterPointer filter = SDAFilterType::New();
  filter->SetSplineOrder( 3 );
  filter->SetNumberOfControlPoints( ncps );
  filter->SetNumberOfLevels( 3 );

  // Define the parametric domain.
  filter->SetOrigin( mask->GetOrigin() );
  filter->SetSpacing( mask->GetSpacing() );
  ImageType::RegionType inputRegion = mask->GetBufferedRegion();
  filter->SetSize( inputRegion.GetSize() );
  filter->SetInput( pointSet );
  filter->Update();

  IndexFilterPointer componentExtractor = IndexFilterType::New();
  componentExtractor->SetInput( filter->GetOutput() );
  componentExtractor->SetIndex( 0 );
  componentExtractor->Update();

  WriterType::Pointer imageWriter = WriterType::New();
  imageWriter->SetFileName("normals.nrrd");
  imageWriter->SetInput(componentExtractor->GetOutput());
  imageWriter->Update();
  /*
  MeshWriterType::Pointer writer = MeshWriterType::New();
  writer->SetInput( dMesh );
  writer->SetFileName( outputMeshName );
  writer->Update();
  */
 
  return EXIT_SUCCESS;

}

void ShiftVector(std::vector<PixelType>& v, int s){
  int vs = v.size(), i;
//  std::cout << "Shift value: " << s << std::endl;
//  std::cout << "Initial array: ";
//  for(i=0;i<vs;i++)
//    std::cout << v[i] << " ";
//  std::cout << std::endl;
  if(!s)
    return;
  if(s<0) { // shift left
    for(i=vs-1;i>=abs(s);i--){
      v[i] = v[i-abs(s)];
    }
    for(;i>=0;i--)
      v[i] = 0;
 
  } else {
   
    for(i=0;i<vs-abs(s);i++){
      v[i] = v[i+s];
    }
    for(;i<vs;i++)
      v[i] = 0;

  }
//  std::cout << "Shifted array: ";
//  for(i=0;i<vs;i++)
//    std::cout << v[i] << " ";
//  std::cout << std::endl;
}

vtkPolyData* ITKMesh2PolyData(MeshType::Pointer mesh){
  vtkPolyData *surface = vtkPolyData::New();
  vtkPoints *surfacePoints = vtkPoints::New();
//  vtkCellArray *surfaceCells = vtkCellArray::New();

  surfacePoints->SetNumberOfPoints(mesh->GetPoints()->Size());
//  surfaceCells->EstimateSize(mesh->GetCells()->Size(), 3);
  
  PointsIterator pIt = mesh->GetPoints()->Begin(), pItEnd = mesh->GetPoints()->End();
  CellsIterator cIt = mesh->GetCells()->Begin(), cItEnd = mesh->GetCells()->End();

  while(pIt!=pItEnd){
    OutputMeshType::PointType pt = pIt->Value();
    surfacePoints->SetPoint(pIt->Index(), pt[0], pt[1], pt[2]);
    
    ++pIt;
  }

  surface->SetPoints(surfacePoints);
  surface->Allocate();

  while(cIt!=cItEnd){
    OutputMeshType::CellType *cell = cIt->Value();
    OutputMeshType::CellType::PointIdIterator pidIt = cell->PointIdsBegin(); 
    vtkIdType cIds[3];
    cIds[0] = *pidIt;
    cIds[1] = *(pidIt+1);
    cIds[2] = *(pidIt+2);
    surface->InsertNextCell(VTK_TRIANGLE, 3, cIds);

    ++cIt;
  }
  return surface;
}

void PrintSurfaceStatistics(vtkPolyData* surf){

  vtkMassProperties *mp = vtkMassProperties::New();  
  mp->SetInput(surf);
  mp->Update();

  std::cout << "Volume: " << mp->GetVolume() << std::endl;
  std::cout << "Surface area: " << mp->GetSurfaceArea() << std::endl;
  std::cout << "Shape index: " << mp->GetNormalizedShapeIndex() << std::endl;

}

float SurfaceVolume(vtkPolyData* surf){
  vtkMassProperties *mp = vtkMassProperties::New();  
  mp->SetInput(surf);
  mp->Update();
  return mp->GetVolume();
}

/* Take the QE mesh and the array with vectors defined for each point. Iterate
 * over the neighborhood of each surface vertex, and smooth the vectors based
 * in some smoothing filter
 *
 * Vector Median Filter
 *
 */
vtkFloatArray* SmoothVMF(MeshType::Pointer mesh,vtkFloatArray* vec){
  PointsIterator pIt = mesh->GetPoints()->Begin(), pItEnd = mesh->GetPoints()->End();
  CellsIterator cIt = mesh->GetCells()->Begin(), cItEnd = mesh->GetCells()->End();

  vtkFloatArray *newVecArray = vtkFloatArray::New();
  newVecArray->SetNumberOfComponents(3);
  newVecArray->SetNumberOfTuples(mesh->GetPoints()->Size());
  newVecArray->SetName((std::string(vec->GetName())+"smooth").c_str());

  while(pIt!=pItEnd){
    MeshType::QEType* edge = mesh->FindEdge(pIt->Index());
    MeshType::QEType* tmp = edge;
    MeshType::CellIdentifier cellId(0);
    MeshType::PointIdentifier pointId(0);
    MeshType::CellPixelType cellVal;
    unsigned orig = tmp->GetOrigin();

    std::vector<MeshType::PointIdentifier> neighbors;
    neighbors.push_back(orig);
    do {
      neighbors.push_back(tmp->GetDestination());      
      tmp = tmp->GetOnext();
    } while(tmp!=edge);

    // calculate distances from each point to each neighbor
    float cumDist, minDist = 10000;
    unsigned mini = 10000;
    unsigned nnei = neighbors.size(), i, j;
    for(i=0;i<nnei;i++){
      MeshType::CellPixelType n1, n2;
      vnl_vector<MeshType::CellPixelType::ComponentType> v1, v2;
      v1.set_size(3);
      v2.set_size(3);
      double *v1val = vec->GetTuple3(neighbors[i]);
      v1[0] = v1val[0];
      v1[1] = v1val[1];
      v1[2] = v1val[2];

      cumDist = 0;
      for(j=0;j<nnei;j++){
        if(i==j)
          continue;
        double *v2val = vec->GetTuple3(neighbors[j]);
        v2[0] = v2val[0];
        v2[1] = v2val[1];
        v2[2] = v2val[2];
        cumDist += (v1-v2).two_norm();
//        if(pIt->Index()==778 || pIt->Index()==873)
//          std::cout << pIt->Index() << ": " << neighbors[i] << " to " << neighbors[j] << ": " << angle(v1,v2) << std::endl;
      }
//      if(pIt->Index()==778 || pIt->Index()==873)
//        std::cout << "i=" << i << ", cumAngle=" << cumAngle << std::endl;
      if(cumDist<minDist){
          minDist = cumDist;
          mini = i;
       }
    }

//    if(pIt->Index()==778 || pIt->Index()==873)
//      std::cout << pIt->Index() << ": new value: " << neighbors[mini] << std::endl;
    double *median = vec->GetTuple3(neighbors[mini]);    
    newVecArray->InsertTuple3(pIt->Index(), median[0], median[1], median[2]);

    ++pIt;
//    std::cout << std::endl;
  }

  return newVecArray;
}

// Vector Directional Filter
vtkFloatArray* SmoothVDF(MeshType::Pointer mesh,vtkFloatArray* vec){
  PointsIterator pIt = mesh->GetPoints()->Begin(), pItEnd = mesh->GetPoints()->End();
  CellsIterator cIt = mesh->GetCells()->Begin(), cItEnd = mesh->GetCells()->End();

  vtkFloatArray *newVecArray = vtkFloatArray::New();
  newVecArray->SetNumberOfComponents(3);
  newVecArray->SetNumberOfTuples(mesh->GetPoints()->Size());
  newVecArray->SetName((std::string(vec->GetName())+"smooth").c_str());

  while(pIt!=pItEnd){
    MeshType::QEType* edge = mesh->FindEdge(pIt->Index());
    MeshType::QEType* tmp = edge;
    MeshType::CellIdentifier cellId(0);
    MeshType::PointIdentifier pointId(0);
    MeshType::CellPixelType cellVal;
    unsigned orig = tmp->GetOrigin();

    std::vector<MeshType::PointIdentifier> neighbors;
    neighbors.push_back(orig);
    do {
      neighbors.push_back(tmp->GetDestination());      
      tmp = tmp->GetOnext();
    } while(tmp!=edge);

    // calculate angles from each point to each neighbor
    float cumAngle, minAngle = 10000;
    unsigned mini = 10000;
    unsigned nnei = neighbors.size(), i, j;
    for(i=0;i<nnei;i++){
      MeshType::CellPixelType n1, n2;
      vnl_vector<MeshType::CellPixelType::ComponentType> v1, v2;
      v1.set_size(3);
      v2.set_size(3);
      double *v1val = vec->GetTuple3(neighbors[i]);
      v1[0] = v1val[0];
      v1[1] = v1val[1];
      v1[2] = v1val[2];
      v1.normalize();

      cumAngle = 0;
      for(j=0;j<nnei;j++){
        if(i==j)
          continue;
        double *v2val = vec->GetTuple3(neighbors[j]);
        v2[0] = v2val[0];
        v2[1] = v2val[1];
        v2[2] = v2val[2];
        v2.normalize();
        cumAngle += angle(v1,v2);
//        if(pIt->Index()==778 || pIt->Index()==873)
//          std::cout << pIt->Index() << ": " << neighbors[i] << " to " << neighbors[j] << ": " << angle(v1,v2) << std::endl;
      }
//      if(pIt->Index()==778 || pIt->Index()==873)
//        std::cout << "i=" << i << ", cumAngle=" << cumAngle << std::endl;
      if(cumAngle<minAngle){
          minAngle = cumAngle;
          mini = i;
       }
    }

//    if(pIt->Index()==778 || pIt->Index()==873)
//      std::cout << pIt->Index() << ": new value: " << neighbors[mini] << std::endl;
    double *median = vec->GetTuple3(neighbors[mini]);    
    newVecArray->InsertTuple3(pIt->Index(), median[0], median[1], median[2]);

    ++pIt;
//    std::cout << std::endl;
  }

  return newVecArray;
}

void WriteMesh(MeshType::Pointer mesh, const char* fname){
  vtkPolyData *vtksurf = ITKMesh2PolyData(mesh);
  vtkPolyDataWriter *pdw = vtkPolyDataWriter::New();
  pdw->SetFileName(fname);
  pdw->SetInput(vtksurf);
  pdw->Update();
}


/* Local threshold estimation based on the neighborhood */
    /*
    // walk the neighborhood and get median (mean?) difference
    ImageType::SizeType kernelSize;
    kernelSize.Fill(1);
    // how much intensity difference would we see if the boundry shifted with
    // respect to the same image?
    
    itk::ConstNeighborhoodIterator<ImageType> dit1(kernelSize, image1, image1->GetBufferedRegion());
    itk::ConstNeighborhoodIterator<ImageType> dit2(kernelSize, image2, image2->GetBufferedRegion());
    ImageType::IndexType diffIdx1, diffIdx2;
    ImageType::PointType diffPt1, diffPt2;
    
    diffPt1[0] = point[0];//+normal[0]*.5;
    diffPt1[1] = point[1];//+normal[1]*.5;
    diffPt1[2] = point[2];//+normal[2]*.5;
    assert(normal.GetNorm()==1);

    diffPt2[0] = point[0];//+normal[0]*.5;
    diffPt2[1] = point[1];//+normal[1]*.5;
    diffPt2[2] = point[2];//+normal[2]*.5;

    mask->TransformPhysicalPointToIndex(diffPt1,diffIdx1);
    mask->TransformPhysicalPointToIndex(diffPt2,diffIdx2);

    dit1.SetLocation(diffIdx1);
    dit2.SetLocation(diffIdx2);

    std::vector<PixelType> diffValues;
    PixelType mean = 0, median = 0;
    for(i=0;i<dit1.Size();i++){
      PixelType diff = dit1.GetPixel(i)-dit2.GetPixel(i);
      diffValues.push_back(diff);
      mean += diff;
    }
    mean /= dit1.Size();
    std::vector<PixelType>::iterator medianIter = diffValues.begin()+dit1.Size()/2;
    std::nth_element(diffValues.begin(),medianIter,diffValues.end());
    median = *medianIter;

    pdContainer->SetElement(pIt->Index(),mean);
    psContainer->SetElement(pIt->Index(),pIt->Value());
    */


/* online calculation of mean and variance */
      /*
      i++;
      delta = v1 - mean1;
      mean1 = mean1 + delta/(double)i;
      std1 = std1 + delta*(v1-mean1);
      
      delta = v2 - mean2;
      mean2 = mean2 + delta/(double)i;
      std2 = std2 + delta*(v2-mean2);
      
      if(i>1){        
        ddelta = dprofile1[i-2] - mean1;
        dmean1 = dmean1 + ddelta/(double)i;
        dstd1 = dstd1 + ddelta*(dprofile1[i-2]-dmean1);
        
        ddelta = dprofile2[i-2] - mean2;
        dmean2 = dmean2 + ddelta/(double)i;
        dstd2 = dstd2 + ddelta*(dprofile2[i-2]-dmean2);
      }



    std1 = std1/float(i);
    std2 = std2/float(i);
    dstd1 = dstd1/float(i);
    dstd2 = dstd2/float(i);
*/


