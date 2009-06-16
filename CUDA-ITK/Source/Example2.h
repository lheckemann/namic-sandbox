/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImage.h,v $
  Language:  C++
  Date:      $Date: 2009-02-05 19:04:56 $
  Version:   $Revision: 1.150 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "cutil.h"
#include "cutil_inline.h"


/**
* \brief Simple 2D size / region_of_interest structure
*
*  Simple 2D size / region_of_interest structure
*/
typedef struct  
{
  int width;      //!< ROI width
  int height;      //!< ROI height
} ROI;



/**
*  One-byte unsigned integer type
*/
typedef unsigned char byte;



/**
**************************************************************************
*  Memory allocator, returns aligned format frame with 32bpp float pixels.
*
* \param width      [IN] - Width of image buffer to be allocated
* \param height      [IN] - Height of image buffer to be allocated
* \param pStepBytes    [OUT] - Step between two sequential rows
*  
* \return Pointer to the created plane
*/
float *MallocPlaneFloat(int width, int height, int *pStepBytes);

/**
**************************************************************************
*  Copies byte plane to float plane
*
* \param ImgSrc        [IN] - Source byte plane
* \param StrideB      [IN] - Source plane stride
* \param ImgDst        [OUT] - Destination float plane
* \param StrideF      [IN] - Destination plane stride
* \param Size        [IN] - Size of area to copy
*  
* \return None
*/
void CopyByte2Float(byte *ImgSrc, int StrideB, float *ImgDst, int StrideF, ROI Size);


/**
**************************************************************************
*  Float round to nearest value
*
* \param num      [IN] - Float value to round
*  
* \return The closest to the input float integer value
*/
float round_f(float num);


/**
**************************************************************************
*  The routine clamps the input value to integer byte range [0, 255]
*
* \param x      [IN] - Input value
*  
* \return Pointer to the created plane
*/
int clamp_0_255(int x);


/**
**************************************************************************
*  Copies float plane to byte plane (with clamp)
*
* \param ImgSrc        [IN] - Source float plane
* \param StrideF      [IN] - Source plane stride
* \param ImgDst        [OUT] - Destination byte plane
* \param StrideB      [IN] - Destination plane stride
* \param Size        [IN] - Size of area to copy
*  
* \return None
*/
void CopyFloat2Byte(float *ImgSrc, int StrideF, byte *ImgDst, int StrideB, ROI Size);


/**
**************************************************************************
*  Memory deallocator, deletes aligned format frame.
*
* \param ptr      [IN] - Pointer to the plane
*  
* \return None
*/
void FreePlane(void *ptr);


/**
 *  Copy data from an ITK memory buffer to a CUDA buffer
 */
void CopyITKImageToCUDA(byte *ImgSrc, byte *ImgDst, int Stride, ROI Size);

