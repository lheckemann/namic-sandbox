#ifdef PLATFORM_WIN32
#include <ace/config-win32.h>
#endif
#ifdef PLATFORM_OSX
#  ifdef PLATFORM_OSX_LEOPARD
#    include <ace/config-macosx-leopard.h>
#  else
#    include <ace/config-macosx-tiger.h>
#  endif
#endif
#ifdef PLATFORM_LINUX
#include <ace/config-linux.h>
#endif
