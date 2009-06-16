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

#include "Example2.h"


/**
**************************************************************************
*  Memory allocator, returns aligned format frame with 32bpp float pixels.
*
* \param width			[IN] - Width of image buffer to be allocated
* \param height			[IN] - Height of image buffer to be allocated
* \param pStepBytes		[OUT] - Step between two sequential rows
*  
* \return Pointer to the created plane
*/
float *MallocPlaneFloat(int width, int height, int *pStepBytes)
{
	float *ptr;
	*pStepBytes = ((int)ceil((width*sizeof(float))/16.0f))*16;
//#ifdef __ALLOW_ALIGNED_MEMORY_MANAGEMENT
//	ptr = (float *)_aligned_malloc(*pStepBytes * height, 16);
//#else
	ptr = (float *)malloc(*pStepBytes * height);
//#endif
	*pStepBytes = *pStepBytes / sizeof(float);
	return ptr;
}

/**
**************************************************************************
*  Copies byte plane to float plane
*
* \param ImgSrc				[IN] - Source byte plane
* \param StrideB			[IN] - Source plane stride
* \param ImgDst				[OUT] - Destination float plane
* \param StrideF			[IN] - Destination plane stride
* \param Size				[IN] - Size of area to copy
*  
* \return None
*/
void CopyByte2Float(byte *ImgSrc, int StrideB, float *ImgDst, int StrideF, ROI Size)
{
	for (int i=0; i<Size.height; i++)
	{
		for (int j=0; j<Size.width; j++)
		{
			ImgDst[i*StrideF+j] = (float)ImgSrc[i*StrideB+j];
		}
	}
}


/**
**************************************************************************
*  Float round to nearest value
*
* \param num			[IN] - Float value to round
*  
* \return The closest to the input float integer value
*/
float round_f(float num) 
{
	float NumAbs = fabs(num);
	int NumAbsI = (int)(NumAbs + 0.5f);
	float sign = num > 0 ? 1.0f : -1.0f;
	return sign * NumAbsI;
}


/**
**************************************************************************
*  The routine clamps the input value to integer byte range [0, 255]
*
* \param x			[IN] - Input value
*  
* \return Pointer to the created plane
*/
int clamp_0_255(int x)
{
	return (x < 0) ? 0 : ( (x > 255) ? 255 : x );
}


/**
**************************************************************************
*  Copies float plane to byte plane (with clamp)
*
* \param ImgSrc				[IN] - Source float plane
* \param StrideF			[IN] - Source plane stride
* \param ImgDst				[OUT] - Destination byte plane
* \param StrideB			[IN] - Destination plane stride
* \param Size				[IN] - Size of area to copy
*  
* \return None
*/
void CopyFloat2Byte(float *ImgSrc, int StrideF, byte *ImgDst, int StrideB, ROI Size)
{
	for (int i=0; i<Size.height; i++)
	{
		for (int j=0; j<Size.width; j++)
		{
			ImgDst[i*StrideB+j] = (byte)clamp_0_255((int)(round_f(ImgSrc[i*StrideF+j])));
		}
	}
}


/**
**************************************************************************
*  Memory deallocator, deletes aligned format frame.
*
* \param ptr			[IN] - Pointer to the plane
*  
* \return None
*/
void FreePlane(void *ptr)
{
//#ifdef __ALLOW_ALIGNED_MEMORY_MANAGEMENT
//	if (ptr) 
//	{
//		_aligned_free(ptr);
//	}
//#else
	if (ptr) 
	{
		free(ptr);
	}
//#endif
}



void CopyITKImageToCUDA(byte *ImgSrc, byte *ImgDst, int Stride, ROI Size)
{
	//prepare channel format descriptor for passing texture into kernels
	cudaChannelFormatDesc floattex = cudaCreateChannelDesc<float>();

	//allocate device memory
	cudaArray *Src;
	float *Dst;
	size_t DstStride;
	cutilSafeCall(cudaMallocArray(&Src, &floattex, Size.width, Size.height));
	cutilSafeCall(cudaMallocPitch((void **)(&Dst), &DstStride, Size.width * sizeof(float), Size.height));
	DstStride /= sizeof(float);

	//convert source image to float representation
	int ImgSrcFStride;
	float *ImgSrcF = MallocPlaneFloat(Size.width, Size.height, &ImgSrcFStride);
	CopyByte2Float(ImgSrc, Stride, ImgSrcF, ImgSrcFStride, Size);

	//copy from host memory to device
	cutilSafeCall(cudaMemcpy2DToArray(Src, 0, 0,
									   ImgSrcF, ImgSrcFStride * sizeof(float), 
									   Size.width * sizeof(float), Size.height,
									   cudaMemcpyHostToDevice) );


	//copy quantized image block to host
	cutilSafeCall(cudaMemcpy2D(ImgSrcF, ImgSrcFStride * sizeof(float), 
								Dst, DstStride * sizeof(float), 
								Size.width * sizeof(float), Size.height,
								cudaMemcpyDeviceToHost) );

	//convert image back to byte representation
	CopyFloat2Byte(ImgSrcF, ImgSrcFStride, ImgDst, Stride, Size);

	//clean up memory
	cutilSafeCall(cudaFreeArray(Src));
	cutilSafeCall(cudaFree(Dst));
	FreePlane(ImgSrcF);

}
