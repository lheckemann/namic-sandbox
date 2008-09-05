#include "vtkTRProstateBiopsyWin32Header.h"

#include "vtksys/ios/iostream"
#include "vtksys/ios/sstream"

#ifdef _WIN32
#  define slicerCerr(x) \
  do { \
    cerr << x << "\n"; \
    vtkstd::ostringstream str; \
    str << x; \
    MessageBox(NULL, str.str().c_str(), "Slicer Error", MB_OK);\
  } while (0)
#else
#  define slicerCerr(x) \
  cerr << x << "\n"
#endif
