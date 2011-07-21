#include "BRAINSDemonWarp2DTemplates.h"

#ifdef WIN32
#define MODULE_IMPORT __declspec(dllimport)
#else
#define MODULE_IMPORT
#endif

extern "C" MODULE_IMPORT int VBRAINSDemonWarp2DPrimary(int, char* []);


// main function built in BRAINSDemonWarp2DPrimary.cxx so that testing only builds
// templates once.
int main(int argc, char *argv[])
{
  return VBRAINSDemonWarp2DPrimary(argc, argv);
}
