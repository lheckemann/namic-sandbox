#include <stdio.h>
#include <stdlib.h>

#include "QdecProject.h"

int main ( int argc, char** argv ) 
{

  if ( NULL == getenv( "SUBJECTS_DIR" ) ) {
    printf("ERROR: SUBJECTS_DIR is not set\n");
    return 1;
  }

  if (argc != 9)
  {
    printf("Usage:   qdecCmd qdec.table.dat df1 df2 cf2 cf2 "
           "measure hemi smoothing\n");
    printf("Example: qdecCmd qdec.table.dat gender none age none thickness "
           "lh 10\n");
    return 1;
  }
  else
  {
    for ( int i=0; i < argc; i++ ) printf("%s ",argv[i]);
    printf("\n");
  }

  const char* table = argv[1];
  const char* df1 = argv[2];
  const char* df2 = argv[3];
  const char* cf1 = argv[4];
  const char* cf2 = argv[5];
  const char* measure = argv[6];
  const char* hemi = argv[7];
  const char* smoothness = argv[8];

  QdecProject* qdecProject = new QdecProject();

  if ( qdecProject->LoadDataTable( table ) ) return 1;
  else ( qdecProject->DumpDataTable( stdout ) );
  if ( qdecProject->CreateGlmDesign
       ("qdecCmd",
        df1, // first discrete factor
        df2, // second discrete factor
        cf1, // first continuous factor
        cf2, // second continuous factor
        measure,
        hemi,
        atoi(smoothness),
        NULL  /* ProgressUpdateGUI */ )
    ) return 1;
  if ( qdecProject->RunGlmFit() ) return 1;

  exit( 0 );
}
