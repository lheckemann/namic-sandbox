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

#include <stdint.h>

#include "io_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/*** Control Header data ***/

typedef struct {
  int32_t   pixOffset;     /* byte displacement to pixel data*/
  int32_t   width;         /* width */
  int32_t   height;        /* height */
  int32_t   depth;         /* depth (bits) */
  int32_t   compression;   /* compression (0=asis,1=rectangular,2=packed,
                              3=compressed,4=compressed&packed) */
  int32_t   bgShade;       /* background shade to use for non-image */
  uint16_t  eacSum;        /* 16 bit end_around_carry sum of pixels */ 
  int32_t   pImageId;      /* ptr to unique image identifier */
  int32_t   lImageId;      /* length of unique image identifier */
  int32_t   pUnpackHeader; /* ptr to unpack header */
  int32_t   lUnpackHeader; /* length of unpack header */
  int32_t   pCompHeader;   /* ptr to compression header */
  int32_t   lCompHeader;   /* length of compression header */
  int32_t   pHistHeader;   /* ptr to histogram header */
  int32_t   lHistHeader;   /* length of histogram header */
  int32_t   pTextPlane;    /* ptr to text plane */
  int32_t   lTextPlane;    /* length of text plane */
  int32_t   pGraphPlane;   /* ptr to graphics plane */
  int32_t   lGraphPlane;   /* length of graphics plane */
  int32_t   pDBPlane;      /* ptr to data base header */
  int32_t   lDBPlane;      /* length of data base header */
  int32_t   vStoredPixAdd; /* value to add to stored pixels */
  int32_t   pUserData;     /* ptr to user defined data */
  int32_t   lUserData;     /* length of user defined data */
  int32_t   pSuiteHeader;  /* ptr to suite header */
  int32_t   lSuiteHeader;  /* length of suite header */
  int32_t   pExamHeader;   /* ptr to exam header */
  int32_t   lExamHeader;   /* length of exam header */
  int32_t   pSerHeader;    /* ptr to series header */
  int32_t   lSerHeader;    /* length of series header */
  int32_t   pImgHeader;    /* ptr to image header */
  int32_t   lImgHeader;    /* length of image header */
} ControlHeader;


/*** Exam Header data ***/

typedef struct {
  char      suiteId[4];     /* suite ID */
  uint16_t  examNo;         /* exam number */
  char      patientId[13];  /* patient ID */
  char      patientName[25];/* patient name */
  int16_t   patientAge;     /* patient age */
  int16_t   patientSex;     /* patient sex */
  char      examType[3];    /* exam type - "MR" or "CT" */
} ExamHeader;


/*** Series Header data ***/

typedef struct {
  int16_t   serNo;          /* series number */
  char      anatomRef[3];   /* anatomical reference */
  char      scanProt[25];   /* scan protocol name */
} SeriesHeader;


/*** Image Header data ***/
typedef struct {
  /** common part for CT and MR **/
  int16_t   imgNo;          /* image number */
  float32_t sliceThick;     /* slice thickness mm */
  int16_t   matrixX;        /* matrix size - X */
  int16_t   matrixY;        /* matrix size - Y */
  float32_t dispFovX;       /* display field of view - X (mm) */
  float32_t dispFovY;       /* display field of view - Y (mm) */
  float32_t imgDimX;        /* image dimension - X */
  float32_t imgDimY;        /* image dimension - Y */
  float32_t pixSizeX;       /* pixel size - X */
  float32_t pixSizeY;       /* pixel size - Y */
  char      pixDataId[14];  /* pixel data ID */
  char      ivContAgent[17];/* iv contrast agent */
  char      oralContAgent[17]; /* oral contrast agent */

  float32_t imgLoc;         /* image location */
  float32_t imgCR;          /* image centre R mm (ie. X +ve to right) */
  float32_t imgCA;          /* image centre A mm (ie. Y +ve to anterior) */
  float32_t imgCS;          /* image centre S mm (ie. Z +ve to superior) */
  float32_t imgTlhcR;       /* image TLHC R mm (ie. X +ve to right) */
  float32_t imgTlhcA;       /* image TLHC A mm (ie. Y +ve to anterior) */
  float32_t imgTlhcS;       /* image TLHC S mm (ie. Z +ve to superior) */
  float32_t imgTrhcR;       /* image TRHC R mm (ie. X +ve to right) */
  float32_t imgTrhcA;       /* image TRHC A mm (ie. Y +ve to anterior) */
  float32_t imgTrhcS;       /* image TRHC S mm (ie. Z +ve to superior) */
  float32_t imgBrhcR;       /* image BRHC R mm (ie. X +ve to right) */
  float32_t imgBrhcA;       /* image BRHC A mm (ie. Y +ve to anterior) */
  float32_t imgBrhcS;       /* image BRHC S mm (ie. Z +ve to superior) */

  /** part for MR **/
  int32_t   tr;             /* repetition time(usec) */
  int32_t   ti;             /* inversion time(usec) */
  int32_t   te;             /* echo time(usec) */
  int16_t   nEchoes;        /* number of echoes */
  int16_t   echoNo;         /* echo number */
  float32_t nex;            /* NEX */
  char      seqName[33];    /* pulse sequence name */
  char      coilName[17];   /* coil name */
  int16_t   etl;            /* ETL for FSE */
} ImageHeader;

typedef struct {
  int            width;
  int            height;
  int            depth;
  unsigned char* imageData;
  ExamHeader     eh;
  SeriesHeader   sh;
  ImageHeader    ih;
} GenesisImageInfo;


int  readControlHeader(FILE *fp, ControlHeader *header, int baseOffset);
int  readExamHeader(FILE *fp, ExamHeader *header, int baseOffset);
int  readSeriesHeader(FILE *fp, SeriesHeader *header, int baseOffset);
int  readImageHeader(FILE *fp,ImageHeader *header, int baseOffset);
int  readGenesisFile(char* filename, GenesisImageInfo *imageInfo);
void printGenesisImageInfo(GenesisImageInfo* imageInfo);
int  convertByteOrder(GenesisImageInfo *imageInfo);

#define GENRDR_MAGIC_NUMBER     0x494d4746    /* "IMGF" */


#ifdef __cplusplus
}
#endif

#endif /* _INC_IO_SIGNA5 */

