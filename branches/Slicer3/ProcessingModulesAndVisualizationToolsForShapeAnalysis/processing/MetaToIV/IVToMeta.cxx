/* 
 * convert IV To Meta
 *
 * author:  Ipek Oguz & Martin Styner 
 *
 */


#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <vector>
using namespace std ;

#include "argio.h"

vector <float> verts ;
vector <int> tris ;

void readInput ( char *filename ) 
{
  ifstream inputFile ;
  
  inputFile.open ( filename ) ;
  verts.clear () ;
  tris.clear () ;
  
  double x, y, z ;
  int v1, v2, v3 ;
  char line[200], *temp ;
  bool done ;
  
  // skip the beginning 
  done = false ;
  while ( !done )
  {
    inputFile.getline ( line, 200 ) ;
    done = strchr ( line, '[' ) ;
  }

  // read in vertex coordinates
  done = false ;
  while ( !done )
  {
    inputFile.getline ( line, 200 ) ;
    if ( strchr ( line, ']' ) )
    {
      done = true ;
    }
    else
    {
      temp = strtok ( line, " " ) ;
      x = atof ( temp ) ;
      verts.push_back ( x ) ;
      
      temp = strtok ( NULL, " " ) ;
      y = atof ( temp ) ;
      verts.push_back ( y ) ;
      
      temp = strtok ( NULL, " " ) ;
      z = atof ( temp ) ;
      verts.push_back ( z ) ;
      
    }

  }
  
  // skip the in-between part
  done = false ;
  while ( !done )
  {
    inputFile.getline ( line, 200 ) ;
    done = strchr ( line, '[' ) ;
  }

  // read in the triangles
  done = false ;
  while ( !done )
  {
    inputFile.getline ( line, 200 ) ;
    if ( strchr ( line, ']' ) )
    {
      done = true ;
    }
    else
    {
      temp = strtok ( line, "," ) ;
      v1 = atof ( temp ) ;
      tris.push_back ( v1 ) ;
      
      temp = strtok ( NULL, "," ) ;
      v2 = atof ( temp ) ;
      tris.push_back ( v2 ) ;
      
      temp = strtok ( NULL, ",") ;
      v3 = atof ( temp ) ;
      tris.push_back ( v3 ) ;
    }
  }
  
  inputFile.close () ;
  
}

void writeOutput ( char *filename )
{
  ofstream outputFile ;
  outputFile.open ( filename ) ;

  int nVerts = verts.size () / 3 ;
  int nTris = tris.size () / 3 ;
  int i ;

  outputFile << "ObjectType = Mesh" << endl ;
  outputFile << "NDims = 3" << endl ;
  outputFile << "ID = 0" << endl ;
  outputFile << "TransformMatrix = 1 0 0 0 1 0 0 0 1" << endl ;
  outputFile << "Offset = 0 0 0" << endl ;
  outputFile << "CenterOfRotation = 0 0 0" << endl ;
  outputFile << "ElementSpacing = 1 1 1" << endl ;
  outputFile << "PointType = MET_FLOAT" << endl ;
  outputFile << "PointDataType = MET_FLOAT" << endl ;
  outputFile << "CellDataType = MET_FLOAT" << endl ;
  outputFile << "NCellTypes = 1" << endl ;
  outputFile << "PointDim = ID x y ..." << endl ;
  outputFile << "NPoints = " << nVerts << endl ;
  outputFile << "Points = " << endl ;

  for ( i = 0 ; i < nVerts ; i++ )
  {
    outputFile << i << " " << verts[3*i] << " " << verts[3*i+1] << " " << verts[3*i+2] << endl ;
  }

  outputFile << endl ;
  outputFile << "CellType = TRI" << endl ;
  outputFile << "NCells = " << nTris << endl ;
  outputFile << "Cells = " << endl ;
  
  for ( i = 0 ; i < nTris ; i++ )
  {
    outputFile << i << " " << tris[3*i] << " " << tris[3*i+1] << " " << tris[3*i+2] << endl ;
  }

  outputFile.close () ;
}

int main(int argc, const char **argv)
{
  if (argc <=2 || ipExistsArgument(argv, "-usage") || ipExistsArgument(argv, "-help")) {
    std::cout << "IVToMeta $Revision: 1.1 $" << std::endl;
    std::cout << "usage: IVToMeta infile outfile" << std::endl;
    std::cout << std::endl;
    std::cout << "infile               input iv mesh file" << std::endl;
    std::cout << "outfile              output meta file" << std::endl;
    std::cout << std::endl << std::endl;
    exit(0);
  }

  char * infile = strdup(argv[1]);
  char * outfile = strdup(argv[2]);

  readInput ( infile) ;
  writeOutput ( outfile ) ;

  return 0 ;
}

