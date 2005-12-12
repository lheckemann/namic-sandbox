/* 
 * compute the SPHARM coefficients and the associated Mesh
 *
 * author:  Martin Styner 
 *
 */


#include <iostream>
#include <string>
#include <string.h>

#include <itkMeshSpatialObject.h>
#include <itkMesh.h>
#include <itkSpatialObjectWriter.h>
#include <itkSpatialObjectReader.h>
#include <itkTriangleCell.h>
#include <itkDefaultDynamicMeshTraits.h>
#include <itkProcessObject.h>

#include "ParametricMeshToSPHARMSpatialObjectFilter.h"
#include "SphericalHarmonicSpatialObject.h"
#include "SphericalHarmonicMeshSource.h"
#include "SphericalHarmonicCoefficientFileWriter.h"
#include "SphericalHarmonicCoefficientFileReader.h"
#include "itkMesh3DProcrustesAlignFilter.h"

#include "argio.h"


static int debug = 0;

int main( int argc, const char ** argv )
{
  #define dimension 3

  typedef itk::DefaultDynamicMeshTraits< float , dimension, dimension, double > MeshTrait;
  typedef itk::Mesh<float,dimension,MeshTrait> MeshType;

  /** Hold on to the type information specified by the template parameters. */
  typedef  MeshType::Pointer              MeshPointer;
  typedef  MeshTrait::PointType           MeshPointType;
  typedef  MeshTrait::PixelType           MeshPixelType;  

  /** Some convenient typedefs. */
  typedef  MeshType::Pointer              MeshPointer;
  typedef  MeshType::CellTraits           CellTraits;
  typedef  MeshType::PointsContainerPointer PointsContainerPointer;
  typedef  MeshType::PointsContainer      PointsContainer;
  typedef  MeshType::CellsContainerPointer CellsContainerPointer;
  typedef  MeshType::CellsContainer       CellsContainer;
  typedef  MeshType::PointType            PointType;
  typedef  MeshType::CellType             CellType;
  typedef  itk::TriangleCell<CellType>   TriangleType;
   
  typedef itk::MeshSpatialObject<MeshType> MeshSpatialObjectType;
  typedef itk::SpatialObjectWriter<3,float,MeshTrait> MeshWriterType;
  typedef neurolib::ParametricMeshToSPHARMSpatialObjectFilter  SPHARMFilterType;
  typedef itk::Mesh3DProcrustesAlignFilter<MeshType, MeshType> ProcrustesFilterType;

  if (argc <=2 || ipExistsArgument(argv, "-usage") || ipExistsArgument(argv, "-help")) {
    std::cout << "ParaToSPHARMMesh $Revision: 1.6 $" << std::endl;
    std::cout << " compute SPHARM coefs and associated Mesh" << std::endl;
    std::cout << "usage:  <surfaceMesh> <paraMesh> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "surfaceMesh          input surface mesh dataset" << std::endl;
    std::cout << "paraMesh             input para mesh dataset" << std::endl;
    std::cout << "-flipTemplate <coeffile>  set a fliptemplate for normalization of axis-flips" << std::endl;
    std::cout << "-outbase <basefile>  base filename for output" << std::endl;
    std::cout << "-subdivLevel <int>   subdivision level for linear ikosahedron subdivision [20]" << std::endl;
    std::cout << "-spharmDegree <int>  degree of spherical harmonic expansion" << std::endl;
    std::cout << "-regTemplate <meshfile>   set a registration template" << std::endl;
    std::cout << "-v                   verbose mode " << std::endl;
    std::cout << std::endl << std::endl;
    exit(0);
  }
  
  char * inSurfFile = strdup(argv[1]);
  char * inParaFile = strdup(argv[2]);
  debug      = ipExistsArgument(argv, "-v");
  char *outbase = ipGetStringArgument(argv, "-outbase", NULL); 
  char *flipTemplateFile = ipGetStringArgument(argv, "-flipTemplate", NULL); 
  char *regTemplateFile = ipGetStringArgument(argv, "-regTemplate", NULL); 
  int subdivLevel = ipGetIntArgument(argv, "-subdivLevel", 20); 
  int spharmDegree = ipGetIntArgument(argv, "-spharmDegree", 12);
  std::string outFileName ("dummy");  

  char * base_string;
  if (!outbase) {
    base_string = strdup(ipGetBaseName(inSurfFile));
  } else {
    base_string = outbase;
  }

  
  MeshType::Pointer surfaceMesh = MeshType::New();
  MeshType::Pointer paraMesh = MeshType::New();
  typedef itk::SpatialObjectReader<3,float,MeshTrait> ReaderType;
  ReaderType::Pointer readerSH = ReaderType::New();
  neurolib::SphericalHarmonicSpatialObject::Pointer flipTemplateSO = neurolib::SphericalHarmonicSpatialObject::New();
  try
    {
    readerSH->SetFileName(inSurfFile);
    readerSH->Update();
    ReaderType::SceneType::Pointer scene = readerSH->GetScene();  
    ReaderType::SceneType::ObjectListType * objList =  scene->GetObjects(1,NULL);

    ReaderType::SceneType::ObjectListType::iterator it = objList->begin();
    itk::SpatialObject<3> * curObj = *it;
    MeshSpatialObjectType::Pointer  SOMesh = dynamic_cast<MeshSpatialObjectType*> (curObj);
    surfaceMesh = SOMesh->GetMesh();

    }
  catch(itk::ExceptionObject ex)
    {
       std::cout<<ex.GetDescription()<<std::endl;
       return 1;
    }

  try
    {
    readerSH->SetFileName(inParaFile);
    readerSH->Update();
    ReaderType::SceneType::Pointer scene = readerSH->GetScene();  
    ReaderType::SceneType::ObjectListType * objList =  scene->GetObjects(1,NULL);

    ReaderType::SceneType::ObjectListType::iterator it = objList->begin();
    itk::SpatialObject<3> * curObj = *it;
    MeshSpatialObjectType::Pointer  SOMesh = dynamic_cast<MeshSpatialObjectType*> (curObj);
    paraMesh = SOMesh->GetMesh();
    
    }
  catch(itk::ExceptionObject ex)
    {
       std::cout<<ex.GetDescription()<<std::endl;
       return 1;
    }

  if (flipTemplateFile) {
    try 
      {
      typedef neurolib::SphericalHarmonicCoefficientFileReader CoefReaderType;
      CoefReaderType::Pointer coefreader = CoefReaderType::New();
      neurolib::SphericalHarmonicSpatialObject::CoefListType coeflist;
        
      coefreader->SetFileName(flipTemplateFile);
      coefreader->Update();
      coefreader->GetOutput(coeflist);

      flipTemplateSO->SetCoefs(coeflist);
      }  
    catch(itk::ExceptionObject ex)
      {
       std::cout<<ex.GetDescription()<<std::endl;
       return 1;
      }
  }
  if (debug) std::cout << "Preprocessing done" << std::endl;
  try 
    {
      SPHARMFilterType::Pointer spharmFilter = SPHARMFilterType::New();
      spharmFilter->SetInputSurfaceMesh(surfaceMesh);
      spharmFilter->SetInputParametrizationMesh(paraMesh);
      spharmFilter->SetDegree(spharmDegree);
      if (flipTemplateFile) {
         spharmFilter->SetFlipTemplate(flipTemplateSO);
      }
      spharmFilter->GenerateData();
      spharmFilter->Update();
      
      if (debug) std::cout << "saving par aligned data" << std::endl;

      neurolib::SphericalHarmonicSpatialObject::Pointer spharmSO = spharmFilter->GetOutput();
      neurolib::SphericalHarmonicSpatialObject::CoefListType coeflist;
      spharmSO->GetCoefs(coeflist);

      // save associated surface
      neurolib::SphericalHarmonicMeshSource::Pointer meshsrc = neurolib::SphericalHarmonicMeshSource::New();
      meshsrc->SetCoefs(coeflist);
      meshsrc->SetLevel(subdivLevel);
      meshsrc->Update();

      MeshType* meshSH;
      meshSH = meshsrc->GetOutput();

      outFileName.erase();
      outFileName.append(base_string);
      outFileName.append("SPHARM.meta");

      MeshSpatialObjectType::Pointer meshSO = MeshSpatialObjectType::New();
      meshSO->SetMesh(meshSH);
      // Writing the file
      MeshWriterType::Pointer writer = MeshWriterType::New();
      writer->SetInput(meshSO);
      writer->SetFileName(outFileName.c_str());
      writer->Update();

      if (debug) std::cout << "saving par aligned coefs" << std::endl;

      // save coefficients too
      outFileName.erase();
      outFileName.append(base_string);
      outFileName.append("SPHARM.coef");
      typedef neurolib::SphericalHarmonicCoefficientFileWriter CoefWriterType;
      CoefWriterType::Pointer coefwriter = CoefWriterType::New();
        
      coefwriter->SetFileName(outFileName.c_str());
      coefwriter->SetInput(coeflist);
      coefwriter->Update();

      if (debug) std::cout << "generating ellipse aligned data" << std::endl;
      // get EllipseAlign
      neurolib::SphericalHarmonicSpatialObject::CoefListType * ellipsoidAlign;
      ellipsoidAlign = spharmFilter->GetEllipseAlignCoef();

      if (debug) std::cout << "writing ellipse aligned data" << std::endl;
      meshsrc->SetCoefs(*ellipsoidAlign);
      meshsrc->Update();
      MeshType * ellipseMesh = meshsrc->GetOutput();
      outFileName.erase();
      outFileName.append(base_string);
      outFileName.append("SPHARM_ellalign.meta");
      meshSO->SetMesh(ellipseMesh);
      writer->SetInput(meshSO);
      writer->SetFileName(outFileName.c_str());
      writer->Update();
      outFileName.erase();
      outFileName.append(base_string);
      outFileName.append("SPHARM_ellalign.coef");
      coefwriter->SetFileName(outFileName.c_str());
      coefwriter->SetInput(*ellipsoidAlign);
      coefwriter->Update();
      
      if (regTemplateFile) {
    //load regTemplateMesh
    ReaderType::Pointer readerMesh = ReaderType::New();
        readerMesh->SetFileName(regTemplateFile);
    readerMesh->Update();
    ReaderType::SceneType::Pointer scene = readerMesh->GetScene();  
    ReaderType::SceneType::ObjectListType * objList =  scene->GetObjects(1,NULL);
    
    ReaderType::SceneType::ObjectListType::iterator it = objList->begin();
    itk::SpatialObject<3> * curObj = *it;
    MeshSpatialObjectType::Pointer  RegTemplateMeshSO = dynamic_cast<MeshSpatialObjectType*> (curObj);
    MeshType::Pointer RegTemplateMesh = RegTemplateMeshSO->GetMesh();

    ProcrustesFilterType::Pointer procrustesFilter = ProcrustesFilterType::New();
        procrustesFilter->SetNumberOfInputs(2);
        procrustesFilter->SetInput(0, RegTemplateMesh);
        procrustesFilter->SetInput(1, meshSH);
    procrustesFilter->SetUseInitialAverageOff();
    procrustesFilter->SetUseScalingOff();
    procrustesFilter->SetUseSingleIterationOn();
    procrustesFilter->Update();

        MeshType::Pointer  RegisteredMesh = procrustesFilter->GetOutput(1);

    outFileName.erase();
    outFileName.append(base_string);
    outFileName.append("SPHARM_procalign.meta");
    meshSO->SetMesh(RegisteredMesh);
    writer->SetInput(meshSO);
    writer->SetFileName(outFileName.c_str());
    writer->Update();
      }
      

    }
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
      return EXIT_FAILURE;
    }
  

}         
