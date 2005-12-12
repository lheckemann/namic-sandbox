/* 
 * extraction of surfacenet
 *
 * author:  Martin Styner 
 *  
 * changes:
 *
 */

#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>    // for exit, system
#include <math.h>
#include <errno.h>

#include "TVoxelVolume.h"
#include "TSurfaceNet.h"

#include <itkImage.h>
#include <itkImageFileReader.h> 
#include <itkImageRegionIterator.h>
#include <string>

#include "argio.hh"

#include "surfnetExtract.hh"

using namespace itk;
using namespace std;

struct  Vertex  {int  x, y, z, count, neighb[14];};

struct  Net     {int nvert, nface, *face; Vertex *vert;};

int     read_net(FILE*, Net*);

static int debug = 0;
 
int main(const int argc, const char **argv)
{
  char tmp_string[256];
  
  if (argc <=2 || ipExistsArgument(argv, "-usage")) {
    cout << "surfnetExtract 1.0 version (Sept. 5. 2001)" << endl;
    cout << "usage: surfnetExtract infile outfile [-v]"<< endl;
    cout << endl;
    cout << " infile      input segmented data set (0 = bg, object > 0 )" << endl;
    cout << " outfile     output net file" << endl;
    cout << endl << endl;
    exit(0);
  }
  
  char segfile[250];
  char outfile[250];

  strncpy(segfile,argv[1],249);
  strncpy(outfile,argv[2],249);
  debug = ipExistsArgument(argv,"-v"); 
  
  const int Dimension = 3;
  typedef unsigned char ImagePixelType;
  typedef Image<ImagePixelType,Dimension>  ImageType;
  typedef ImageFileReader<ImageType> VolumeReaderType;
  ImageType::Pointer image ;
  typedef   ImageRegionIterator< ImageType > Iterator;

  VolumeReaderType::Pointer imageReader = VolumeReaderType::New();

  if (debug)
    cout << "extracting surfacenet from Volume " <<  segfile<< " to " << outfile << endl;

  try
    {
      imageReader->SetFileName(segfile) ;
      imageReader->Update() ;
      image = imageReader->GetOutput() ;
    }
  catch (ExceptionObject e)
    {
      e.Print(cout) ;
      exit(0) ;
    }
  
  Iterator iter (image, image->GetLargestPossibleRegion());
  while ( !iter.IsAtEnd() )
    {
      if (iter.Get()) {
  iter.Set(255);
      }
      ++iter;
    }
  
  ImageType::RegionType imageRegion = image->GetBufferedRegion();
  int dim[3];
  dim[0] = imageRegion.GetSize(0);
  dim[1] = imageRegion.GetSize(1);
  dim[2] = imageRegion.GetSize(2);
  ImageType::IndexType nullIndex;
  nullIndex[0] = 0;
  nullIndex[1] = 0;
  nullIndex[2] = 0;

  ImagePixelType *data = &((*image)[nullIndex]);

  TVoxelVolume vox(dim[0], dim[1], dim[2], (const unsigned char *) data);
  TSurfaceNet sn(vox, 127);
  ofstream netstr(outfile);
  sn.print(netstr);

  Net net; 
  FILE *stream; 
  int status;

  if (!(stream= fopen(outfile, "r"))) {
    cerr << "cannot open " << outfile << endl;
    exit (-1); 
  }
  if (status = read_net(stream, &net)) {
    fprintf(stderr, "read_net exited with %d at position %d in file '%s'\n",
            status, ftell(stream), outfile);
    exit(status);
  }
  fclose(stream);

  
  int euler_num = net.nvert-net.nface;
  
  if (euler_num != 2) {
    cout << "Warning: Euler equation not satisfied. Euler Number = "
   << euler_num << endl;
  } else {
    cout << "Euler Number ok = " << euler_num << endl;
  }

  string outfile_pts(outfile);
  outfile_pts.append(".pts");
  string outfile_tri(outfile);
  outfile_tri.append(".triangle");
  string outfile_iv(outfile);
  outfile_iv.append(".iv");

  // pts
  FILE * file = fopen(outfile_pts.c_str() , "w");
  if (!file) {
    cerr << "Error: open of file failed." << outfile_pts << endl;
    return (0);
  }
  fprintf(file,"%d \n", net.nvert);
  for (int i = 0; i < net.nvert; i++) {
    //vert
    fprintf(file,"%d ", (net.vert[i]).x);
    fprintf(file,"%d ", (net.vert[i]).y);
    fprintf(file,"%d \n", (net.vert[i]).z);
  }
  fclose(file);
  // triangles
  file = fopen(outfile_tri.c_str() , "w");
  if (!file) {
    cerr << "Error: open of file failed." << outfile_tri << endl;
    return (0);
  }
  for (int i = 0; i < net.nface; i++) {
    //first triangle
    fprintf(file,"%d ", net.face[4*i+0]);
    fprintf(file,"%d ", net.face[4*i+1]);
    fprintf(file,"%d \n", net.face[4*i+2]);
    // second triangle
    fprintf(file,"%d ", net.face[4*i+2]);
    fprintf(file,"%d " , net.face[4*i+3]);
    fprintf(file,"%d \n", net.face[4*i+0]);
  }
  fclose(file);

  FILE *dfile = fopen(outfile_iv.c_str(), "w");
  if (!dfile) {
    std::cout << "Could not open input file " << outfile_iv << std::endl;
     return -1;
  }
  // Prepare the destination file for iv format
  fprintf(dfile,"#Inventor V2.1 ascii \n \n");
  // the coordinates
  fprintf(dfile,"Separator { \n ");
  fprintf(dfile," Material { diffuseColor [ 0 0 1 ] } \n");
  fprintf(dfile,"        Coordinate3 { \n");
  fprintf(dfile,"               point [ \n");
  for (int i = 0; i < net.nvert; i++) {
    //vert
    fprintf(dfile,"%d ", (net.vert[i]).x);
    fprintf(dfile,"%d ", (net.vert[i]).y);
    fprintf(dfile,"%d \n", (net.vert[i]).z);
  }
  fprintf(dfile,"               ] \n");// close of point
  fprintf(dfile,"             } \n"); // close of Coordinate3

  // the triangles
  fprintf(dfile,"        IndexedTriangleStripSet { \n");
  fprintf(dfile,"               coordIndex[\n");
  for (int i = 0; i < net.nface; i++) {
    //first triangle
    fprintf(dfile,"%d, ", net.face[4*i+0]);
    fprintf(dfile,"%d, ", net.face[4*i+1]);
    fprintf(dfile,"%d, -1,\n", net.face[4*i+2]);
    // second triangle
    fprintf(dfile,"%d, ", net.face[4*i+2]);
    fprintf(dfile,"%d, " , net.face[4*i+3]);
    fprintf(dfile,"%d, -1,\n", net.face[4*i+0]);
  }
  fprintf(dfile,"              ]\n"); // coordIndex  
  fprintf(dfile,"        }\n "); // IndexedTriangleStripSet
  fprintf(dfile,"       }\n"); // Separator
  fprintf(dfile,"\n"); // empty line

  fclose(dfile);

  return 0;
}

int read_vertex(FILE *file, Vertex *p)
{
  int   conv0, conv1;

  if (fscanf(file, " { { %d , %d , %d } , { %d", &p->x, &p->y, &p->z, p->neighb)
      != 4) return 1;
  for (p->count= 1;
       fscanf(file, " , %d", p->neighb+p->count) == 1;
       p->count++);
  fscanf(file, " %n } } %n", &conv0, &conv1);
  return conv1-conv0 < 2;
}



int read_vertices(FILE *file, Vertex **net, int *count)
{
  int           scanned, size;

  fscanf(file, " {%n", &scanned); if (scanned < 1)                   return  -1;
  const int BLOCK = 10240;
  size = BLOCK;
  if (!(*net = (Vertex*) malloc(size*sizeof(Vertex))))               return  -2;
  *count= 0;

  while (!read_vertex(file, *net+*count)) {
    (*count)++;
    if (*count >= size) {
      size+= BLOCK;
      fprintf(stderr, "--db-- read_points(): increasing size to %d\n", size);
      if (!(*net = (Vertex*) realloc(*net, size*sizeof(Vertex))))    return  -3;
    }
    fscanf(file, " "); fscanf(file, ",%n", &scanned); if(scanned <1) return  -4;
  }
  
  fprintf(stderr, "--db-- read in %d vertices.\n", *count);

  if (!(*net = (Vertex*) realloc(*net, *count*sizeof(Vertex))))      return  -5;
  fscanf(file, " "); fscanf(file, "}%n", &scanned); if (scanned < 1) return  -6;
  return 0;
}


int read_net(FILE *stream, Net *net)
{
  int status, i, j, *f;

  if (status = read_vertices(stream, &net->vert, &net->nvert)) return status;

  net->face = (int*  )malloc(16*net->nvert * sizeof(int  ));

  f= net->face;
  for (i= 0; i < net->nvert; i++) {
    for (j=0; j < net->vert[i].count-1; j+=2) {
      int second= (j+2)%net->vert[i].count;
      if (net->vert[i].neighb[j     ] > i &&      // 1st corner creates a square
          net->vert[i].neighb[j+1   ] > i &&
          net->vert[i].neighb[second] > i) {
        *f++= i;
        *f++= net->vert[i].neighb[j     ];
        *f++= net->vert[i].neighb[j+1   ];
        *f++= net->vert[i].neighb[second];
      }
    }
  }
  net->nface= (f - net->face)/4;
  net->face= (int*)realloc(net->face, net->nface * 4 * sizeof(int));
  return 0;
}
