// Author Martin Styner
// reparametrizatio using Ikosahedron or externally given parametrization
// December 2004


#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <string>

#include "Icosahedron_subdiv.h"
#include "argio.h"
#include "read_surface.h"
#include "reparamSurface.h"

static  bool debug =0;

int main(int argc, const char *argv[])
{
 
  // read parameter

  if (argc <3 || ipExistsArgument(argv, "-usage") || ipExistsArgument(argv, "-help")) {
    std::cout << argv[0] << " version 0.8 " << std::endl;
    std::cout << "usage: " << argv[0] <<  " <innet_file> <inpar_file>  [options] "<< std::endl;
    std::cout << "  innet_file   is a surface data file" << std::endl;
    std::cout << "  inpar_file   is a spherical parametrization (R3) as surface data on the unit sphere" << std::endl;
    std::cout << "      readable formats are BYU and IV (openInventor/Coin3d)" << std::endl;
    std::cout << std::endl << "  following options are possible (defauls in brackets): " << std::endl;
    std::cout << "  -outbase <base> [<innet>]  stores the reparametrized surface data at this base" << std::endl;
    std::cout << "  -icosubdiv <l> [10]        level of icosahedron subdivision for reparameterization " << std::endl;
    std::cout << "  -fileRepara <reparam_file> use the paramatrization in this file for reparameterization, parameterization is stored in R3 as surface data on the unit sphere" << std::endl;
    std::cout << "  -v                         verbose mode" << std::endl;
    std::cout << std::endl << " output of the program is the reparameterized surface data and the reparamerization" << std::endl;
    
    exit(0);
  }
  char *innet = strdup(argv[1]);
  char *inpar = strdup(argv[2]);

  char *inRepar = ipGetStringArgument(argv,"-fileRepara",NULL);
  bool fileReparOn = ipExistsArgument(argv, "-fileRepara");
  
  int subdivLevel = ipGetIntArgument(argv,"-icosubdiv",10);
  debug   = ipExistsArgument(argv, "-v");

  std::string outbase (ipGetStringArgument(argv,"-outbase",innet));

  int retval;
  //----------------------------------
  // read net
  //----------------------------------
  int numVertices,numTriangles;
  double * vertList = NULL;
  int *triangIndexList = NULL;

  if (debug) std::cout << "reading surface " << innet << std::endl;
  retval = ReadSurface(innet, numVertices, vertList, numTriangles, triangIndexList);
  if (retval) {
    std::cerr << "error reading file " << innet << std::endl;
    exit(1);
  }

  //----------------------------------
  // read para
  //----------------------------------
  int numParVertices,numParTriangles;
  double * parVertList = NULL;
  int *parTriangIndexList = NULL;

  if (debug) std::cout << "reading parametrization " << inpar << std::endl;
  retval = ReadSurface(inpar, numParVertices,parVertList, numParTriangles, parTriangIndexList);
  if (retval) {
    std::cerr << "error reading file " << inpar << std::endl;
    exit(1);
  }
  
  //----------------------------------
  // check consistency of triangulation (has to be the same for net and para)
  //----------------------------------
  if (debug) std::cout << "checking integrity of surface and parametrization" << std::endl;
  if (numParVertices != numVertices || numParTriangles != numTriangles) {
    std::cerr << "surface and parametrization do not agree in dimension"  << std::endl;
    exit(1);
  }
  for (int i = 0; i < numTriangles * 3; i++) {
    if (triangIndexList[i] != parTriangIndexList[i]) {
      std::cerr << "surface and parametrization do not agree in connectivity"  << std::endl;
      exit(1);
    }
  }

  //----------------------------------
  //either read Reparamatrization file or do linear icosahedron subdivision
  //----------------------------------
  int numReparamVertices, numReparamTriangles;
  double * reparamVertList = NULL;
  int * reparamTriangIndexList = NULL;

  if ( fileReparOn) {
    // read param
    if (debug) std::cout << "reading Re-parametrization " << inRepar << std::endl;
    retval = ReadSurface(inRepar, numReparamVertices, reparamVertList, 
       numReparamTriangles, reparamTriangIndexList);
    if (retval) {
      std::cerr << "error reading file " << inRepar << std::endl;
      exit(1);
    }
  } else {
    if (debug) std::cout << "computing icosahedron subdivision level " << subdivLevel  << std::endl;
    set_up_icosahedron_triangs(subdivLevel, numReparamVertices, reparamVertList, 
             numReparamTriangles, reparamTriangIndexList);
  }
  
  //----------------------------------
  // do reparam mapping
  //----------------------------------
  double * surfReparamVertList = NULL;
  
  if (debug) std::cout << "computing reparameterization "  << std::endl;
  retval = reparamSurface(numReparamVertices, surfReparamVertList, reparamVertList, 
        numVertices, vertList, parVertList,
        numTriangles, triangIndexList);
  if (retval) {
    std::cerr << "error in reparametrization "  << std::endl;
    exit(1);
  }

  //----------------------------------
  // same mapped reparameterization
  //----------------------------------
  std::string outfileNet = outbase + "_RepVert.iv";
  std::string outfilePar = outbase + "_RepPar.iv";
  
  if (debug) std::cout << "writing " << outfileNet  << std::endl;
  retval = WriteSurfaceIV(outfileNet.c_str(), numReparamVertices, surfReparamVertList, 
        numReparamTriangles, reparamTriangIndexList);
  if (retval) {
    std::cerr << "error writing file " << outfileNet << std::endl;
    exit(1);
  }
  if (debug) std::cout << "writing " << outfilePar  << std::endl;
  retval = WriteSurfaceIV(outfilePar.c_str(), numReparamVertices, reparamVertList, 
      numReparamTriangles, reparamTriangIndexList);
  if (retval) {
    std::cerr << "error writing file " << outfilePar << std::endl;
    exit(1);
  }


  // free all memory that was allocated
  delete parTriangIndexList;
  delete triangIndexList;
  delete vertList;
  delete parVertList;
  delete surfReparamVertList;
  delete reparamVertList;
  delete reparamTriangIndexList;

  return 0;
}
