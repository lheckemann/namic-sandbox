#include <stdio.h>
// should be replaced by <iostream.h>

struct  Vertex  {int  x, y, z, count, neighb[14];};

struct  Net  {int nvert, nface, *face; Vertex *vert;};

int  read_net(FILE*, Net*);
