/* 
 * Here is where system computed values get stored.
 * These values should only change when the target compile platform changes.
 */

#define QUEENSOPENIGT_BUILD_SHARED_LIBS
#ifndef QUEENSOPENIGT_BUILD_SHARED_LIBS
#define QUEENSOPENIGT_STATIC
#endif

#if defined(_MSC_VER) && !defined(QUEENSOPENIGT_STATIC)
#pragma warning ( disable : 4275 )
#endif
