
#include <cstdio>
#include <string>
#include <iostream>
#include <limits.h>

int WriteSurfaceIV(const char * surfFilename, int numVertices, double * vertList, 
       int numTriangles, int *triangIndexList)
// writes an iv IndexedTriangleSet/IndexedTriangleStripSet
{
  std::string filename(surfFilename);
  FILE *dfile = fopen(filename.c_str(), "w");
  if (!dfile) {
    std::cout << "Could not open input file " << filename << std::endl;
     return -1;
  }
  // Prepare the destination file for iv format
  fprintf(dfile,"#Inventor V2.1 ascii \n \n");

  // the coordinates
  fprintf(dfile,"Separator { \n ");
  fprintf(dfile," Material { diffuseColor [ 0 0 1 ] } \n");
  fprintf(dfile,"        Coordinate3 { \n");
  fprintf(dfile,"               point [ \n");
  for(int h=0; h<numVertices; h++)  {
    fprintf(dfile,"                    %lf %lf %lf ,\n",
      vertList[h * 3 + 0],vertList[h * 3 + 1],vertList[h * 3 + 2]);
  }
  fprintf(dfile,"               ] \n");// close of point
  fprintf(dfile,"             } \n"); // close of Coordinate3

  // the triangles
  fprintf(dfile,"        IndexedTriangleStripSet { \n");
  fprintf(dfile,"               coordIndex[\n");
  for (int i =0; i < numTriangles; i++)  {
      fprintf(dfile,"                    %d,%d,%d,-1,\n",
        triangIndexList[i * 3 + 0],triangIndexList[i * 3 + 1],triangIndexList[i * 3 + 2]);
    }
  fprintf(dfile,"              ]\n"); // coordIndex  
  fprintf(dfile,"        }\n "); // IndexedTriangleStripSet
  fprintf(dfile,"       }\n"); // Separator
  fprintf(dfile,"\n"); // empty line

  fclose(dfile);

  return 0;
}


int ReadSurface(const char * surfFilename, int &numVertices, double * &vertList, 
         int &numTriangles, int * &triangIndexList)
// read a byu triangulation or a iv IndexedTriangleSet/IndexedTriangleStripSet
{
  FILE *fp;
  
  std::string filename(surfFilename);
  std::string ivsuffix(".iv");
  std::string byusuffix(".byu");
  fp = fopen(surfFilename, "rb");
  if(fp == NULL) {
    std::cout << "Could not open input file " << filename << std::endl;
    return -2;
  }
  
  //remove old triangulation
  if (vertList) { delete vertList; vertList = NULL;}
  if (triangIndexList) { delete triangIndexList; triangIndexList = NULL;}


  if (filename.substr(filename.length()-ivsuffix.length(), ivsuffix.length()) == ivsuffix) {
    //std::cout << "doing iv reading" << std::endl;

    // iv file reading
    // read file linewise
    char line[500], bracket1[100], bracket2[100];
    
    // find coordinate identifier
    bool endread, entry1Found, entry2Found = false;
    endread = entry1Found = entry2Found = false;
    long locFile = 0;
    
    // how many points
    numVertices = 0;
    while (!endread && fgets(line,500,fp)) {
      if (entry1Found && entry2Found) {
  if (strstr(line, "]")) { // closing bracket
    endread = true;
  } else {
    numVertices++;
  }
      } else {
  if (sscanf(line, " Coordinate3 %s ", bracket1)) {
    entry1Found = true;
  } else if (entry1Found && sscanf(line, " point %s ", bracket2)) {
    entry2Found = true;
    locFile = ftell(fp);
  }
      }
    }
    if (numVertices == 0) {
      std::cout << " No vertices found (Coordinate 3 { point [..]}) in file " << filename << std::endl;
      return -3;
    }
    // get vertices first
    vertList = new double[numVertices * 3];
    
    fseek(fp,locFile,SEEK_SET);
    for (int index = 0; index < numVertices; index++){
      fgets(line,500,fp);
      sscanf(line, " %lf %lf %lf ", &(vertList[3*index]),&(vertList[3*index+1]),&(vertList[3*index+2]));
    }

    // how many triangles
    rewind(fp);
    numTriangles = 0;
    endread = entry1Found = entry2Found = false;
    while (!endread && fgets(line,500,fp)) {
      if (entry1Found && entry2Found) {
  if (strstr(line, "]")) { // closing bracket
    endread = true;
  } else {
    numTriangles++;
  }
      } else {
  if (sscanf(line, " IndexedTriangleStripSet %s ", bracket1) ||
      sscanf(line, " IndexedTriangleSet %s ", bracket1)) {
    entry1Found = true;
  } else if (entry1Found && sscanf(line, " coordIndex %s ", bracket2)) {
    entry2Found = true;
    locFile = ftell(fp);
  }
      }
    }
    if (numTriangles == 0) {
      std::cout << " No triangles found (IndexedTriangleSet/IndexedTriangleStripSet 3 { coordIndex [..]}) in file " << filename << std::endl;
      return -4;
    }
    triangIndexList = new int [numTriangles * 3];

    fseek(fp,locFile,SEEK_SET);
    for (int index = 0; index < numTriangles; index++){
      fgets(line,500,fp);
      sscanf(line, " %i , %i , %i ",&(triangIndexList[3*index]),&(triangIndexList[3*index+1]),
       &(triangIndexList[3*index+2]));
    }
    //btw  iv has  0 starting index
  } else  if (filename.substr(filename.length()-byusuffix.length(), byusuffix.length()) == byusuffix) {
    // byu file reading
    
    //Pick up byu1's header information. numEdges is not needed.
    int numEdges;
    fscanf(fp, "1 %d %d %d\n", &numVertices, &numTriangles, &numEdges);
    int edges_per_poly;
    fscanf(fp, "1 %d\n", &edges_per_poly);
    
    vertList = new double[numVertices * 3];
    triangIndexList = new int [numTriangles * 3];
    int temp = numVertices * 3;
    for (int i = 0; i < temp; i += 3) {
      fscanf(fp, "%lf %lf %lf\n",&(vertList[i]),&(vertList[i+1]),&(vertList[i+2]));
    }
    for (int i = 0; i < numTriangles; i ++) {
      fscanf(fp, "%i %i -%i\n",&(triangIndexList[3*i]),&(triangIndexList[3*i+1]),
       &(triangIndexList[3*i+2]));
      
    }
    // BYU has starting index 1 -> subtract 1
    for (int index = 0; index < numTriangles * 3; index++){
      triangIndexList[index] = triangIndexList[index] - 1;
    }
  } else {
    std::cerr << " Surface file needs to be either in byu triangulation or in iv IndexedTriangleSet/IndexedTriangleStripSet format " << std:: endl;
    fclose(fp);
    return -1;
  }
  fclose(fp);

  return 0;
}
