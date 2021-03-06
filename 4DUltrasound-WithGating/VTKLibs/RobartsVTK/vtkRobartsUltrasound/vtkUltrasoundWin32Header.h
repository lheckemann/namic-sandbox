/*=========================================================================

  Program:   AtamaiUltrasound for VTK
  Module:    $RCSfile: vtkUltrasoundWin32Header.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.2 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

// .NAME vtkUltrasoundWin32Header - manage Windows system differences
// .SECTION Description
// The vtkUltrasoundWin32Header captures some system differences between Unix
// and Windows operating systems.

#ifndef __vtkUltrasoundWin32Header_h
#define __vtkUltrasoundWin32Header_h

#include <QueensOpenIGTLibsConfigure.h>

#if defined(WIN32) && !defined(QUEENSOPENIGT_STATIC)
#if defined(vtkOpenIGTUltrasound_EXPORTS)
#define VTK_ULTRASOUND_EXPORT __declspec( dllexport )
#else
#define VTK_ULTRASOUND_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_ULTRASOUND_EXPORT
#endif

#endif

