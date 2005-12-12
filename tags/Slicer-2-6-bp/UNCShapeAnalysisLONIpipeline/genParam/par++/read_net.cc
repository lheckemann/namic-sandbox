static const char SccsId[] = "%W% %G%";

#include "vertex.hh"
#include <malloc.h>
#define BLOCK 10240

int read_vertex(FILE *file, Vertex *p)
{
  int  conv0, conv1;

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
  int    scanned, size;

  fscanf(file, " {%n", &scanned); if (scanned < 1)         return  -1;
  size = BLOCK;
  if (!(*net = (Vertex*) malloc(size*sizeof(Vertex))))         return  -2;
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

  if (!(*net = (Vertex*) realloc(*net, *count*sizeof(Vertex))))       return  -5;
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
      if (net->vert[i].neighb[j     ] > i &&    // 1st corner creates a square
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
