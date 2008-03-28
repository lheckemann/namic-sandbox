/*=========================================================================

  Program:   Open IGT Link Library
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*
 * File Reader/Writer for GE MR Signa 5.x (Genesis)
 * 
 * Written by J.Tokuda (junichi@atre.t.u-tokyo.ac.jp)
 *
 * Descrpition:
 * MR image reader for GE Genesis Systems.
 * Note that this program doesn't support image pack and
 * compression.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "io_signa5.h"
#include "io_base.h"

#undef _DBG_PRINT

#if (__GNUC__ >= 3)  // in case of GCC 3.x
  #ifdef _DEBUG_GENRDR
    #define DBG_PRINT(...)   fprintf(stderr, "GENRDR>> " __VA_ARGS__)
  #else
    #define DBG_PRINT(...)   
  #endif
#elif defined(__GNUC__) // in case of GCC 2.x
  #ifdef _DEBUG_GENRDR
    #define DBG_PRINT(s...)   fprintf(stderr, "GENRDR>> " s)
  #else
    #define DBG_PRINT(s...)   
  #endif
#endif


int readControlHeader(FILE *fp, ControlHeader *header, int baseOffset)
{
  int32_t  tmp32;
  uint16_t tmp16u;

  if (fp == NULL) {
    DBG_PRINT("Cannot read input file.\n");
    return 0;
  }

  fseek(fp, baseOffset, SEEK_SET);
  /** Check magic number **/
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read magic number.\n");
    return 0;
  }
  if (tmp32 != GENRDR_MAGIC_NUMBER) {
    DBG_PRINT("Illegal magic number.\n");
    return 0;
  }
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read byte displacement to pixel data.\n");
    return 0;
  }
  header->pixOffset = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read image width.\n");
    return 0;
  }
  header->width = tmp32;

  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read image height.\n");
    return 0;
  }
  header->height = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read image depth.\n");
    return 0;
  }
  header->depth = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read compression infomation.\n");
    return 0;
  }
  header->compression = tmp32;

  fseek(fp, baseOffset+32, SEEK_SET);
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read background shade.\n");
    return 0;
  }
  header->bgShade = tmp32;

  fseek(fp, baseOffset+54, SEEK_SET);
  if (readHostUShort(fp, &tmp16u) == 0) {
    DBG_PRINT("Cannot read 16 bit end_around_carry sum.\n");
    return 0;
  }
  header->eacSum = tmp16u;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read unique image identifier pointer.\n");
    return 0;
  }
  header->pImageId = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read unique image identifier length.\n");
    return 0;
  }
  header->lImageId = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read unpack header pointer.\n");
    return 0;
  }
  header->pUnpackHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read unpack header length.\n");
    return 0;
  }
  header->lUnpackHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read compression header pointer.\n");
    return 0;
  }
  header->pCompHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read compression header length.\n");
    return 0;
  }
  header->lCompHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read histogram header pointer.\n");
    return 0;
  }
  header->pHistHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read histogram header length.\n");
    return 0;
  }
  header->lHistHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read text plane pointer.\n");
    return 0;
  }
  header->pTextPlane = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read text plane length.\n");
    return 0;
  }
  header->lTextPlane = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read graphics plane pointer.\n");
    return 0;
  }
  header->pGraphPlane = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read graphics plane length.\n");
    return 0;
  }
  header->lGraphPlane = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read data base header pointer.\n");
    return 0;
  }
  header->pDBPlane = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read data base header length.\n");
    return 0;
  }
  header->lDBPlane = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read value to add to stored pixels.\n");
    return 0;
  }
  header->vStoredPixAdd = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read user defined data pointer.\n");
    return 0;
  }
  header->pUserData = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read user defined data length.\n");
    return 0;
  }
  header->lUserData = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read suite header pointer.\n");
    return 0;
  }
  header->pSuiteHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read suite header length.\n");
    return 0;
  }
  header->lSuiteHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read exam header pointer.\n");
    return 0;
  }
  header->pExamHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read exam header length.\n");
    return 0;
  }
  header->lExamHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read series header pointer.\n");
    return 0;
  }
  header->pSerHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read series header length.\n");
    return 0;
  }
  header->lSerHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read image header pointer.\n");
    return 0;
  }
  header->pImgHeader = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read image header length.\n");
    return 0;
  }
  header->lImgHeader = tmp32;
}


int readExamHeader(FILE *fp, ExamHeader *header, int baseOffset)
{
  uint16_t tmp16u;
  int16_t  tmp16;
  
  if (fp == NULL) {
    DBG_PRINT("Cannot read input file.\n");
    return 0;
  }

  fseek(fp, baseOffset+0, SEEK_SET);
  if (fread(header->suiteId, 4, 1, fp) != 1) {
    DBG_PRINT("Cannot read suite ID.\n");
    return 0;
  }

  fseek(fp, baseOffset+8, SEEK_SET);
  if (readHostUShort(fp, &tmp16u) == 0) {
    DBG_PRINT("Cannot read exam number.\n");
    return 0;
  }
  header->examNo = tmp16u;

  fseek(fp, baseOffset+84, SEEK_SET);
  if (fread(header->patientId, 13, 1, fp) != 1) {
    DBG_PRINT("Cannot read patient ID.\n");
    return 0;
  }
  if (fread(header->patientName, 25, 1, fp) != 1) {
    DBG_PRINT("Cannot read patient name.\n");
    return 0;
  }
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read patient age.\n");
    return 0;
  }
  header->patientAge = tmp16;

  fseek(fp, baseOffset+126, SEEK_SET);
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read patient sex.\n");
    return 0;
  }
  header->patientAge = tmp16;

  fseek(fp, baseOffset+305, SEEK_SET);
  if (fread(header->examType, 3, 1, fp) != 1) {
    DBG_PRINT("Cannot read exam type.\n");
    return 0;
  }

  return 1;
}


int readSeriesHeader(FILE *fp, SeriesHeader *header, int baseOffset)
{
  int16_t tmp16;

  if (fp == NULL) {
    DBG_PRINT("Cannot read input file.\n");
    return 0;
  }

  fseek(fp, baseOffset+10, SEEK_SET);
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read series number.\n");
    return 0;
  }
  header->serNo = tmp16;

  fseek(fp, baseOffset+84, SEEK_SET);
  if (fread(header->anatomRef, 3, 1, fp) != 1) {
    DBG_PRINT("Cannot read anatomical reference.\n");
    return 0;
  }

  fseek(fp, baseOffset+92, SEEK_SET);
  if (fread(header->scanProt, 25, 1, fp) != 1) {
    DBG_PRINT("Cannot read scan protocol.\n");
    return 0;
  }

  return 1;
}


int readImageHeader(FILE *fp,ImageHeader *header, int baseOffset)
{
  int16_t     tmp16;
  int32_t     tmp32;
  float32_t   tmp32f;

  if (fp == NULL) {
    DBG_PRINT("Cannot read input file.\n");
    return 0;
  }

  fseek(fp, baseOffset+12, SEEK_SET);  
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read image number.\n");
    return 0;
  }
  header->imgNo = tmp16;

  fseek(fp, baseOffset+26, SEEK_SET);  
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read slice thickness.\n");
    return 0;
  }
  header->sliceThick = tmp32f;
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read matrix size -- X.\n");
    return 0;
  }
  header->matrixX = tmp16;
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read matrix size -- Y.\n");
    return 0;
  }
  header->matrixY = tmp16;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read display field of view -- X.\n");
    return 0;
  }
  header->dispFovX = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read display field of view -- Y.\n");
    return 0;
  }
  header->dispFovY = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read image dimension -- X.\n");
    return 0;
  }
  header->imgDimX = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read image dimension -- Y.\n");
    return 0;
  }
  header->imgDimY = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read pixel size -- X.\n");
    return 0;
  }
  header->pixSizeX = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read pixel size -- Y.\n");
    return 0;
  }
  header->pixSizeY = tmp32f;
  if (fread(header->pixDataId, 14, 1, fp) != 1) {
    DBG_PRINT("Cannot read pixel data ID.\n");
    return 0;
  }
  if (fread(header->ivContAgent, 17, 1, fp) != 1) {
    DBG_PRINT("Cannot read iv contrast agent.\n");
    return 0;
  }
  if (fread(header->oralContAgent, 17, 1, fp) != 1) {
    DBG_PRINT("Cannot read oral contrast agent.\n");
    return 0;
  }

  fseek(fp, baseOffset+126, SEEK_SET);  
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read image location.\n");
    return 0;
  }
  header->imgLoc = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read image centre -- R.\n");
    return 0;
  }
  header->imgCR = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read image centre -- A.\n");
    return 0;
  }
  header->imgCA = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read image centre -- S.\n");
    return 0;
  }
  header->imgCS = tmp32f;
  
  fseek(fp, baseOffset+154, SEEK_SET);  
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read TLHC -- R.\n");
    return 0;
  }
  header->imgTlhcR = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read TLHC -- A.\n");
    return 0;
  }
  header->imgTlhcA = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read TLHC -- S.\n");
    return 0;
  }
  header->imgTlhcS = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read TRHC -- R.\n");
    return 0;
  }
  header->imgTrhcR = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read TRHC -- A.\n");
    return 0;
  }
  header->imgTrhcA = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read TRHC -- S.\n");
    return 0;
  }
  header->imgTrhcS = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read BRHC -- R.\n");
    return 0;
  }
  header->imgBrhcR = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read BRHC -- A.\n");
    return 0;
  }
  header->imgBrhcA = tmp32f;
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read BRHC -- S.\n");
    return 0;
  }
  header->imgBrhcS = tmp32f;

  fseek(fp, baseOffset+194, SEEK_SET);
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read repetition time.\n");
    return 0;
  }
  header->tr = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read inversion time.\n");
    return 0;
  }
  header->ti = tmp32;
  if (readHostInt(fp, &tmp32) == 0) {
    DBG_PRINT("Cannot read echo time.\n");
    return 0;
  }
  header->te = tmp32;

  fseek(fp, baseOffset+210, SEEK_SET);
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read number of echoes.\n");
    return 0;
  }
  header->nEchoes = tmp16;
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read echoe number.\n");
    return 0;
  }
  header->echoNo = tmp16;
  
  fseek(fp, baseOffset+218, SEEK_SET);
  if (readHostFloat(fp, &tmp32f) == 0) {
    DBG_PRINT("Cannot read NEX -- S.\n");
    return 0;
  }
  header->nex = tmp32f;

  fseek(fp, baseOffset+308, SEEK_SET);
  if (fread(header->seqName, 33, 1, fp) != 1) {
    DBG_PRINT("Cannot read pulse sequence name.\n");
    return 0;
  }
  fseek(fp, baseOffset+362, SEEK_SET);
  if (fread(header->seqName, 17, 1, fp) != 1) {
    DBG_PRINT("Cannot read coil name.\n");
    return 0;
  }
  fseek(fp, baseOffset+640, SEEK_SET);
  if (readHostShort(fp, &tmp16) == 0) {
    DBG_PRINT("Cannot read ETL for FSE.\n");
    return 0;
  }
  header->etl = tmp16;

  return 1;
}


int readGenesisFile(char* filename, GenesisImageInfo *imageInfo)
{
  FILE *fp;
  int imgSize;
  ControlHeader ch;

  imageInfo->imageData = NULL;

  if ((fp = fopen(filename, "r")) == NULL) {
    DBG_PRINT("Cannot open file.\n");
    return 0;
  }

  if (readControlHeader(fp, &ch, 0) == 0) {
    return 0;
  }
  if (readExamHeader(fp, &imageInfo->eh, ch.pExamHeader) == 0) {
    return 0;
  }
  if (readSeriesHeader(fp, &imageInfo->sh, ch.pSerHeader) == 0) {
    return 0;
  }
  if (readImageHeader(fp, &imageInfo->ih, ch.pImgHeader) == 0) {
    return 0;
  }

  imageInfo->width  = ch.width;
  imageInfo->height = ch.height;
  imageInfo->depth  = ch.depth;
  
  imgSize = ch.width * ch.height * (ch.depth / 8);

  fseek(fp, ch.pixOffset, SEEK_SET);
  imageInfo->imageData = malloc(imgSize);
  if (fread(imageInfo->imageData, sizeof(char), imgSize, fp) != imgSize) {
    DBG_PRINT("Cannot read image data.\n");
    return 0;
  }

  return 1;

}

int freeGenesisImageData(GenesisImageInfo* imageInfo)
{
  if (imageInfo->imageData)
    free(imageInfo->imageData);
  imageInfo->imageData = NULL;
  return 1;
}

void printGenesisImageInfo(GenesisImageInfo* imageInfo)
{
  char buf[10];

  printf("----- General Info -----\n");
  printf("Image width:          %d\n",     imageInfo->width);
  printf("Image height:         %d\n",     imageInfo->height);
  printf("Image depth:          %d\n\n",   imageInfo->depth);

  printf("----- Exam Info -----\n");
  strncpy(buf, imageInfo->eh.suiteId, 4);
  buf[4] = '\0';
  printf("Suite ID:             %s\n",     buf);
  printf("Exam Type:            %s\n",     imageInfo->eh.examType);
  printf("Exam No.:             %d\n",     imageInfo->eh.examNo);
  printf("Patient ID:           %s\n",     imageInfo->eh.patientId);
  printf("Patient name:         %s\n",     imageInfo->eh.patientName);
  printf("Patient age:          %d\n",     imageInfo->eh.patientAge);
  printf("Patient sex:          %d\n",     imageInfo->eh.patientSex);
  printf("\n");

  printf("----- Series Info -----\n");
  printf("Series No.:           %d\n",     imageInfo->sh.serNo);
  printf("Anatom ref.:          %s\n",     imageInfo->sh.anatomRef);
  printf("Scan protocol:        %s\n",     imageInfo->sh.scanProt);
  printf("\n");

  printf("----- Image Info -----\n");
  printf("Image No.:            %d\n",     imageInfo->ih.imgNo);
  printf("Slice thickness:      %f (mm)\n",imageInfo->ih.sliceThick);
  printf("Matrix X:             %d\n",     imageInfo->ih.matrixX);
  printf("Matrix Y:             %d\n",     imageInfo->ih.matrixY);
  printf("Display FOV X:        %f\n",     imageInfo->ih.dispFovX);
  printf("Display FOV Y:        %f\n",     imageInfo->ih.dispFovY);
  printf("Dimension X:          %f\n",     imageInfo->ih.imgDimX);
  printf("Dimension Y:          %f\n",     imageInfo->ih.imgDimY);
  printf("Pixel size X:         %f\n",     imageInfo->ih.pixSizeX);
  printf("Pixel size Y:         %f\n",     imageInfo->ih.pixSizeY);
  printf("Pixel data ID:        %s\n",     imageInfo->ih.pixDataId);
  printf("Contrast agent(iv):   %s\n",     imageInfo->ih.ivContAgent);
  printf("Contrast agent(oral): %s\n",     imageInfo->ih.oralContAgent);
  printf("\n");

  printf("Image location:       %f\n",      imageInfo->ih.imgLoc);
  printf("Image center R:       %f (mm)\n", imageInfo->ih.imgCR);
  printf("Image center A:       %f (mm)\n", imageInfo->ih.imgCA);  
  printf("Image center S:       %f (mm)\n", imageInfo->ih.imgCS);
  printf("Image TLHC R:         %f (mm)\n", imageInfo->ih.imgTlhcR);
  printf("Image TLHC A:         %f (mm)\n", imageInfo->ih.imgTlhcA);
  printf("Image TLHC S:         %f (mm)\n", imageInfo->ih.imgTlhcS);
  printf("Image TRHC R:         %f (mm)\n", imageInfo->ih.imgTrhcR);
  printf("Image TRHC A:         %f (mm)\n", imageInfo->ih.imgTrhcA);
  printf("Image TRHC S:         %f (mm)\n", imageInfo->ih.imgTrhcS);
  printf("Image BRHC R:         %f (mm)\n", imageInfo->ih.imgBrhcR);
  printf("Image BRHC A:         %f (mm)\n", imageInfo->ih.imgBrhcA);
  printf("Image BRHC S:         %f (mm)\n", imageInfo->ih.imgBrhcS);
  printf("\n");

  printf("TR:                   %d (us)\n", imageInfo->ih.tr);
  printf("TE:                   %d (us)\n", imageInfo->ih.te);
  printf("Inversion time:       %d (us)\n", imageInfo->ih.ti);
  printf("Number of echoes:     %d\n",      imageInfo->ih.nEchoes);
  printf("NEX:                  %d\n",      imageInfo->ih.nex);
  printf("Pulse Sequence:       %s\n",      imageInfo->ih.seqName);
  printf("Coil:                 %s\n",      imageInfo->ih.coilName);
  printf("ETL for FSE           %d\n",      imageInfo->ih.etl);
  printf("\n");

}


int convertByteOrder(GenesisImageInfo *imageInfo)
{
  int imsize;
  uint16_t *imp16;
  uint16_t *end16;

  uint32_t *imp32;
  uint32_t *end32;
    

  imsize = imageInfo->width * imageInfo->height;
  if (imageInfo->depth == 16) {
    /*DBG_PRINT("Start converting byte order. The image depth is 16 bit.\n");*/
    imp16 = (uint16_t*)imageInfo->imageData;
    end16 = imp16 + imsize;
    while (imp16 < end16) {
      *imp16 = ntohs(*imp16);
      imp16 ++;
    }
  } else if (imageInfo->depth == 32) {
    /*DBG_PRINT("Start converting byte order. The image depth is 32 bit.\n");*/
    imp32 = (uint32_t*)imageInfo->imageData;
    end16 = imp16 + imsize;
    while (imp16 < end16) {
      *imp16 = ntohs(*imp16);
      imp16 ++;
    }
  }
  return 1;
}



