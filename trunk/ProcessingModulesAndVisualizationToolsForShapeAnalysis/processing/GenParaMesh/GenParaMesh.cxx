/* 
 * compute the spherical parametrization of a binary mask
 *
 * author:  Martin Styner 
 *
 */


#include <iostream>
#include <string>
#include <string.h>

#include <itkImage.h>
#include <itkImageFileReader.h> 
#include <itkMeshSpatialObject.h>
#include <itkMesh.h>
#include <itkSpatialObjectWriter.h>
#include <itkSpatialObjectReader.h>
#include <itkImageFileReader.h>

#include "BinaryMask3DEqualAreaParametricMeshSource.h"
#include "argio.h"


static int debug = 0;

int main( int argc, const char ** argv )
{

  typedef itk::Image<unsigned short,3> ImageType;
  typedef itk::ImageFileReader<ImageType> VolumeReaderType;
  typedef itk::BinaryMask3DEqualAreaParametricMeshSource<ImageType> MeshSourceType;
  typedef MeshSourceType::OutputMeshType OutputMeshType;  
  typedef itk::MeshSpatialObject<OutputMeshType> MeshSpatialObjectType;
  typedef itk::SpatialObjectWriter<3,float,OutputMeshType::MeshTraits> MeshWriterType;
  typedef itk::ImageFileReader< ImageType >  VolumeReaderType;

  if (argc <=1 || ipExistsArgument(argv, "-usage") || ipExistsArgument(argv, "-help")) {
    std::cout << "GenParaMesh $Revision: 1.9 $" << std::endl;
    std::cout << " computes spherical parametrization" << std::endl; 
    std::cout << "usage: ImageStat infile [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "infile               input dataset" << std::endl;
    std::cout << "-outbase <basefile>  base filename for output" << std::endl;
    std::cout << "-iter <number>       number of iterations (default 500)" << std::endl;
    std::cout << "-label <number>      label id in input image (default 1)" << std::endl;
    std::cout << "-initPara meshfile   initialization of Parametrization, file has to be a mesh of same triangle topology as the one extracted from the image" << std::endl;
    std::cout << "-v                   verbose mode " << std::endl;
    std::cout << std::endl << std::endl;
    exit(0);
  }
  
  ImageType::Pointer image ;

  char * infile = strdup(argv[1]);
  debug      = ipExistsArgument(argv, "-v");
  char *outbase = ipGetStringArgument(argv, "-outbase", NULL); 
  char *initParaFileName = ipGetStringArgument(argv, "-initPara", NULL); 
  int label = ipGetIntArgument(argv, "-label", 1);
  int numIterations = ipGetIntArgument(argv, "-iter", 500);
  std::string outFileName ("dummy");  

  char * base_string;
  if (!outbase) {
    base_string = strdup(ipGetBaseName(infile));
  } else {
    base_string = outbase;
  }


  if (debug) std::cout<<"Reading Image: " << std::endl;
  VolumeReaderType::Pointer imageReader = VolumeReaderType::New();
  imageReader->SetFileName(infile) ;
  imageReader->Update() ;
  image = imageReader->GetOutput() ;
  OutputMeshType::Pointer mesh;
  OutputMeshType::Pointer parmesh;


  // if necessary read parmesh
  typedef itk::SpatialObjectReader<3,float,OutputMeshType::MeshTraits> ReaderType;
  ReaderType::Pointer readerSH = ReaderType::New();
  if (initParaFileName) {
    try
      {
       readerSH->SetFileName(initParaFileName);
       readerSH->Update();
      }
    catch(itk::ExceptionObject ex)
      {
       std::cout<<ex.GetDescription()<<std::endl;
       return 1;
      }
    ReaderType::SceneType::Pointer scene1 = readerSH->GetScene();  
    ReaderType::SceneType::ObjectListType * objList =  scene1->GetObjects(1,NULL);
    //TODO: plugin name if multiple object are present
    ReaderType::SceneType::ObjectListType::iterator it = objList->begin();
    itk::SpatialObject<3> * curObj = *it;
    MeshSpatialObjectType::Pointer paraSOMesh = dynamic_cast<MeshSpatialObjectType*> (curObj);
    parmesh = paraSOMesh->GetMesh();
    
  }

  try 
    {
      if (debug) std::cout<<"Creating Para Surface Mesh: " << std::endl;
      MeshSourceType::Pointer meshsrc = MeshSourceType::New();
      meshsrc->SetInput(image);
      meshsrc->SetNumberOfIterations(numIterations);
      meshsrc->SetObjectValue(label);  
      if (initParaFileName) {
       meshsrc->SetInitParametricMesh(parmesh); 
      }
      meshsrc->Update();

      outFileName.erase();
      outFileName.append(base_string);
      outFileName.append("_surf.meta");

      // Output Mesh
      mesh = meshsrc->GetSurfaceMesh();  
      // Create the mesh Spatial Object
      {
       MeshSpatialObjectType::Pointer meshSO = MeshSpatialObjectType::New();
       meshSO->SetMesh(mesh);
       // Writing the file
       MeshWriterType::Pointer writer = MeshWriterType::New();
       writer->SetInput(meshSO);
       writer->SetFileName(outFileName.c_str());
       writer->Update();
      }

      outFileName.erase();
      outFileName.append(base_string);
      outFileName.append("_para.meta");

      // Output Mesh
      parmesh = meshsrc->GetParametrizationMesh();  
      // Create the mesh Spatial Object
      {
       MeshSpatialObjectType::Pointer meshSO = MeshSpatialObjectType::New();
       meshSO->SetMesh(parmesh);
       // Writing the file
       MeshWriterType::Pointer writer = MeshWriterType::New();
       writer->SetInput(meshSO);
       writer->SetFileName(outFileName.c_str());
       writer->Update();
      }
      
    } 
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
      return -1;
    }


}         
