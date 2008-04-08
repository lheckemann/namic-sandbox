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

#ifndef _INC_IO_SIGNA5
#define _INC_IO_SIGNA5

#define _DEBUG_GENRDR

/*#include <stdint.h>*/

#include "io_base.h"
#include "igtl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*** Control Header data ***/

typedef struct {
  igtl_int32   pixOffset;     /* byte displacement to pixel data*/
  igtl_int32   width;         /* width */
  igtl_int32   height;        /* height */
  igtl_int32   depth;         /* depth (bits) */
  igtl_int32   compression;   /* compression (0=asis,1=rectangular,2=packed,
                              3=compressed,4=compressed&packed) */
  igtl_int32   bgShade;       /* background shade to use for non-image */
  igtl_uint16  eacSum;        /* 16 bit end_around_carry sum of pixels */ 
  igtl_int32   pImageId;      /* ptr to unique image identifier */
  igtl_int32   lImageId;      /* length of unique image identifier */
  igtl_int32   pUnpackHeader; /* ptr to unpack header */
  igtl_int32   lUnpackHeader; /* length of unpack header */
  igtl_int32   pCompHeader;   /* ptr to compression header */
  igtl_int32   lCompHeader;   /* length of compression header */
  igtl_int32   pHistHeader;   /* ptr to histogram header */
  igtl_int32   lHistHeader;   /* length of histogram header */
  igtl_int32   pTextPlane;    /* ptr to text plane */
  igtl_int32   lTextPlane;    /* length of text plane */
  igtl_int32   pGraphPlane;   /* ptr to graphics plane */
  igtl_int32   lGraphPlane;   /* length of graphics plane */
  igtl_int32   pDBPlane;      /* ptr to data base header */
  igtl_int32   lDBPlane;      /* length of data base header */
  igtl_int32   vStoredPixAdd; /* value to add to stored pixels */
  igtl_int32   pUserData;     /* ptr to user defined data */
  igtl_int32   lUserData;     /* length of user defined data */
  igtl_int32   pSuiteHeader;  /* ptr to suite header */
  igtl_int32   lSuiteHeader;  /* length of suite header */
  igtl_int32   pExamHeader;   /* ptr to exam header */
  igtl_int32   lExamHeader;   /* length of exam header */
  igtl_int32   pSerHeader;    /* ptr to series header */
  igtl_int32   lSerHeader;    /* length of series header */
  igtl_int32   pImgHeader;    /* ptr to image header */
  igtl_int32   lImgHeader;    /* length of image header */
} ControlHeader;


/*** Exam Header data ***/

typedef struct {
  igtl_int8    suiteId[4];     /* suite ID */
  igtl_uint16  examNo;         /* exam number */
  igtl_int8    patientId[13];  /* patient ID */
  igtl_int8    patientName[25];/* patient name */
  igtl_int16   patientAge;     /* patient age */
  igtl_int16   patientSex;     /* patient sex */
  igtl_int8    examType[3];    /* exam type - "MR" or "CT" */
} ExamHeader;


/*** Series Header data ***/

typedef struct {
  igtl_int16   serNo;          /* series number */
  igtl_int8    anatomRef[3];   /* anatomical reference */
  igtl_int8    scanProt[25];   /* scan protocol name */
} SeriesHeader;


/*** Image Header data ***/
typedef struct {
  /** common part for CT and MR **/
  igtl_int16   imgNo;          /* image number */
  igtl_float32 sliceThick;     /* slice thickness mm */
  igtl_int16   matrixX;        /* matrix size - X */
  igtl_int16   matrixY;        /* matrix size - Y */
  igtl_float32 dispFovX;       /* display field of view - X (mm) */
  igtl_float32 dispFovY;       /* display field of view - Y (mm) */
  igtl_float32 imgDimX;        /* image dimension - X */
  igtl_float32 imgDimY;        /* image dimension - Y */
  igtl_float32 pixSizeX;       /* pixel size - X */
  igtl_float32 pixSizeY;       /* pixel size - Y */
  igtl_int8    pixDataId[14];  /* pixel data ID */
  igtl_int8    ivContAgent[17];/* iv contrast agent */
  igtl_int8    oralContAgent[17]; /* oral contrast agent */

  igtl_float32 imgLoc;         /* image location */
  igtl_float32 imgCR;          /* image centre R mm (ie. X +ve to right) */
  igtl_float32 imgCA;          /* image centre A mm (ie. Y +ve to anterior) */
  igtl_float32 imgCS;          /* image centre S mm (ie. Z +ve to superior) */
  igtl_float32 imgTlhcR;       /* image TLHC R mm (ie. X +ve to right) */
  igtl_float32 imgTlhcA;       /* image TLHC A mm (ie. Y +ve to anterior) */
  igtl_float32 imgTlhcS;       /* image TLHC S mm (ie. Z +ve to superior) */
  igtl_float32 imgTrhcR;       /* image TRHC R mm (ie. X +ve to right) */
  igtl_float32 imgTrhcA;       /* image TRHC A mm (ie. Y +ve to anterior) */
  igtl_float32 imgTrhcS;       /* image TRHC S mm (ie. Z +ve to superior) */
  igtl_float32 imgBrhcR;       /* image BRHC R mm (ie. X +ve to right) */
  igtl_float32 imgBrhcA;       /* image BRHC A mm (ie. Y +ve to anterior) */
  igtl_float32 imgBrhcS;       /* image BRHC S mm (ie. Z +ve to superior) */

  /** part for MR **/
  igtl_int32   tr;             /* repetition time(usec) */
  igtl_int32   ti;             /* inversion time(usec) */
  igtl_int32   te;             /* echo time(usec) */
  igtl_int16   nEchoes;        /* number of echoes */
  igtl_int16   echoNo;         /* echo number */
  igtl_float32 nex;            /* NEX */
  igtl_int8    seqName[33];    /* pulse sequence name */
  igtl_int8    coilName[17];   /* coil name */
  igtl_int16   etl;            /* ETL for FSE */
} ImageHeader;

typedef struct {
  igtl_int32   width;
  igtl_int32   height;
  igtl_int32   depth;
  igtl_uint8*  imageData;
  ExamHeader   eh;
  SeriesHeader sh;
  ImageHeader  ih;
} GenesisImageInfo;


int  readControlHeader(FILE *fp, ControlHeader *header, int baseOffset);
int  readExamHeader(FILE *fp, ExamHeader *header, int baseOffset);
int  readSeriesHeader(FILE *fp, SeriesHeader *header, int baseOffset);
int  readImageHeader(FILE *fp,ImageHeader *header, int baseOffset);
int  readGenesisFile(char* filename, GenesisImageInfo *imageInfo);
int  freeGenesisImageData(GenesisImageInfo *imageInfo);
void printGenesisImageInfo(GenesisImageInfo* imageInfo);
int  convertByteOrder(GenesisImageInfo *imageInfo);

#define GENRDR_MAGIC_NUMBER     0x494d4746    /* "IMGF" */


#ifdef __cplusplus
}
#endif

#endif /* _INC_IO_SIGNA5 */

