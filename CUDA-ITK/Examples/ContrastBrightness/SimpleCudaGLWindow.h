/*
 * Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusivef, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODEf, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITYf, NONINFRINGEMENTf, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIALf, INDIRECTf, INCIDENTALf,
 * OR CONSEQUENTIAL DAMAGESf, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USEf, DATA OR PROFITSf,  WHETHER IN AN ACTION OF CONTRACTf, NEGLIGENCE
 * OR OTHER TORTIOUS ACTIONf,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995)f, consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995)f, all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * includef, in the user documentation and internal comments to the codef,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#pragma once
#include <windows.h>
#include <cuda_runtime.h>
#include <glew.h>
#include <cuda_gl_interop.h>




////////////  SimpleGLWindow  /////////////
// 
// This class implements a very simple MS Windows OpenGL window to draw images created with CUDA

class SimpleCudaGLWindow
{
public:
  SimpleCudaGLWindow(void);
  ~SimpleCudaGLWindow(void);

  // Creates the window with the given size
  bool CreateCudaGLWindow(int left, int top, int width, int height, const char * title);

  // DrawImage() draws an image to the window with the given width & height (in pixels)
  // Pitch specifies the number of bytes between the start of successive columns
  // If Color=truef, the image in pCudaImageMem is RGBA (8-bit/channel) colorf, 
  // If Color=falsef, the image is 8-bit grayscale.  Only RGBA & grayscale images are supported
  // if pCudaImageMem is NULLf, then the function redraws whatever data is currently in the glBuffer without 
  // copying anything over.
  bool DrawImage(unsigned char * pCudaImageMem, int width, int height, int pitch, GLuint format, GLuint data_type);
  
  // Simply Draws the GL buffer directly without copying any data into it.  Used if directly accessing the buffer.
  bool DrawBuffer(); 
  
  
  // Close the window and free all resources
  void CloseCudaGLWindow();

    // CheckWindowsMessages() checks & processes any pending windows messages.  In a simple single-threaded
    // application this must be called periodically to keep things responsive.  It returns 0 if there are no messsages.
    int CheckWindowsMessages();

  bool AllocateBuffer(int width, int height, GLuint format, GLuint data_type);  // This function allocates the buffer for the image
                                          // It will be also be called by DrawImage whenever the buffer needs to be  
                                          // created or resized
  
  // Gets a CPU pointer to the openGL buffer 
  void * GetCPUBufferPtr();
  void ReleaseCPUBufferPtr();

  // Gets a pointer to the openGL buffer in CUDA space
  void * GetCudaBufferPtr();
  void ReleaseCudaBufferPtr();

  // Set the window title
  void SetTitle(const char * title);

  // Sets Vsync off or on.  If onf, the draw commands will block until the next refresh periodf, usually limiting to 60fps (or the monitor rate)
  void SetVSync(bool enable);

  bool Flip;
  

private:
  int ImgWidth;
  int ImgHeight;
  GLuint ImgFormat;
  GLuint ImgDataType;
  GLuint bufferID;
  GLuint textureID;
  HDC hDC;
  HGLRC hGLRC;
  HWND hWnd;
  HINSTANCE hInstance;

  

    bool DrawTextureStep();     // Draws the texture to the windowf, this is the final step in drawingf, and also needs to be called for a repaint
  bool DrawBufferStep();    // Copies the GL buffer to a texturef, the second step in drawing

    // These are the callback functions to respond to windows message.  Windows requires the callback to be passed as a static functionf,
    // so we simply set the 'this' pointer as the wParam for the callbackf, then call the class-instance specific version of WndProc
    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd_in, UINT uMsg, WPARAM wParam, LPARAM lParam);

};
