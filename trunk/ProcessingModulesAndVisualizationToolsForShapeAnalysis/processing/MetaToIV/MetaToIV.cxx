/* 
 * convert Meat to IV
 *
 * author:  Martin Styner 
 *
 */


#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>

#include <itkMeshSpatialObject.h>
#include <itkMesh.h>
#include <itkSpatialObjectWriter.h>
#include <itkSpatialObjectReader.h>
#include <itkTriangleCell.h>
#include <itkDefaultDynamicMeshTraits.h>
#include <itkProcessObject.h>

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

  if (argc <=2 || ipExistsArgument(argv, "-usage") || ipExistsArgument(argv, "-help")) {
    std::cout << "MetaToIV $Revision: 1.1 $" << std::endl;
    std::cout << "usage: ImageStat infile outfile" << std::endl;
    std::cout << std::endl;
    std::cout << "infile               input meta mesh file" << std::endl;
    std::cout << "outfile              output iv file" << std::endl;
    std::cout << std::endl << std::endl;
    exit(0);
  }

  char * infile = strdup(argv[1]);
  char * outfile = strdup(argv[2]);
  debug      = ipExistsArgument(argv, "-v");

  
  typedef itk::SpatialObjectReader<3,float,MeshTrait> ReaderType;
  ReaderType::Pointer readerSH = ReaderType::New();
    try
      {
       readerSH->SetFileName(infile);
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
    MeshSpatialObjectType::Pointer  SOMesh = dynamic_cast<MeshSpatialObjectType*> (curObj);
    MeshType::Pointer mesh = MeshType::New();
    mesh = SOMesh->GetMesh();
    
    // Output iv 

  MeshType::PointsContainerPointer points = mesh->GetPoints();
  int nvert = points->Size();

  FILE *dfile = fopen(outfile, "w");
  if (!dfile) {
     std::cout << "error opening file for writing: " << outfile <<  std::endl;
  }
  // Prepare the destination file for iv format
  fprintf(dfile,"#Inventor V2.1 ascii \n \n");
  // the coordinates
  fprintf(dfile,"Separator { \n ");
  fprintf(dfile," Material { diffuseColor [ 0 0 1 ] } \n");
  fprintf(dfile,"        Coordinate3 { \n");
  fprintf(dfile,"               point [ \n");
  for (int i = 0; i < nvert; i++) {
    //vert
    PointType  curPoint = points->GetElement(i);
    fprintf(dfile,"%lf %lf %lf\n", curPoint[0], curPoint[1], curPoint[2]);
  }
  fprintf(dfile,"               ] \n");// close of point
  fprintf(dfile,"             } \n"); // close of Coordinate3

  // the triangles
  fprintf(dfile,"        IndexedTriangleStripSet { \n");
  fprintf(dfile,"               coordIndex[\n");
  MeshType::CellsContainerPointer       myCells = mesh->GetCells();
  MeshType::CellsContainerIterator      cells = myCells->Begin();
  while( cells != myCells->End() ) {
    MeshType::CellType * cellPtr = cells.Value();
    const unsigned long * face = cellPtr->GetPointIds();
    fprintf(dfile,"%ld, ", face[0]);
    fprintf(dfile,"%ld, ", face[1]);
    fprintf(dfile,"%ld, -1,\n", face[2]);
    ++cells;
  }
  fprintf(dfile,"              ]\n"); // coordIndex    
  fprintf(dfile,"        }\n "); // IndexedTriangleStripSet
  fprintf(dfile,"       }\n"); // Separator
  fprintf(dfile,"\n"); // empty line

}         
