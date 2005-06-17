extern "C" {
#include <stdio.h>
#include <string.h>
#include "vertex.h"
exit(int);
}

#define SQUARE(x) ((x)*(x))

extern  int  errno;
extern  char  *sys_errlist[];
char    *progname;

main(int argc, char *argv[])
{
  int  i, j, status;
  char  filename[256];
  FILE  *stream;
  Net  net;

  progname= *argv;
  (void)argc;      /* to stop "not used" warning */
  
  fputs("\n\nName of net file: ", stderr);
  scanf("%s", filename);
  if (!(stream= fopen(filename, "r"))) { perror(filename); exit (-1); }
  if (status= read_net(stream, &net)) {
    fprintf(stderr, "read_net exited with %d at position %d in file '%s'\n",
      status, ftell(stream), filename);
    exit(status);
  }
  fclose(stream);

  puts("faces= 1 + {");
  for (i= 0; i < net.nface; i++) {      /* debug output for checking */
    printf("{");
    for (j= 0; j < 3; j++)
      printf("%d, ", net.face[4*i+j]);
    printf("%d}" , net.face[4*i+3]);
    if (i < net.nface-1) puts(",");
    else     puts("};\n");
  }
  
  if (net.nvert != 2+net.nface)
    fputs("-warn- Euler equation not satisfied.\n", stderr);
  
  printf("net  = << \"%s\";\n", filename);
  puts(
"vtx0  = Transpose[net][[1]];\n\
face1  = Select[faces, (Min[#] > 0 && Max[#] <= Length[vtx0])&];\n\
Print[Length[faces]-Length[face1], \"Faces deleted.\"];\n\
        Print[\"Auxiliary structures set up.\"];\n\
{\n\
Show[Graphics3D[Polygon[vtx0  [[#]]    ]&/@ face1]]\n\
}");
}
