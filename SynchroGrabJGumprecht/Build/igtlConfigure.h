/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __IGTL_CONFIGURE_H
#define __IGTL_CONFIGURE_H

/* #undef OpenIGTLink_PLATFORM_MACOSX */
/* #undef OpenIGTLink_PLATFORM_LINUX */
/* #undef OpenIGTLink_PLATFORM_WIN32 */
#ifdef OpenIGTLink_PLATFORM_WIN32
  #ifndef _WIN32
    #define _WIN32
  #endif
  #ifndef WIN32
    #define WIN32
  #endif
  #define IGTLCommon_EXPORTS
#endif

/* #undef OpenIGTLink_USE_PTHREADS */
/* #undef OpenIGTLink_USE_WIN32_THREADS */

#endif /*__IGTL_CONFIGURE_H*/



