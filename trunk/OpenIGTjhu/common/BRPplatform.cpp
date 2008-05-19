#include "BRPplatform.h"

#ifdef _WIN32_WINDOWS
# include "BRPplatform_windows.cpp"
#else
# include "BRPplatform_posix.cpp"
#endif 
