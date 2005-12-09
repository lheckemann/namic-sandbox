#ifndef __RAW3IO_H__
#define __RAW3IO_H__

#include <string.h>
/*
  *
  * (c) Copyright, 1986-1995
  * Biomedical Imaging Resource
  * Mayo Foundation
  *
  * dbh.h
  *
  *
  * database sub-definitions
  */

 /* Acceptable values for hdr.dime.datatype */
#define DT_NONE                         0
#define DT_UNKNOWN                      0
#define DT_BINARY                       1
#define DT_CHAR              256
#define DT_SHORT            65535
#define DT_INT              4294967295

//strings for popen with compression
#define GZIPCOMMAND  "|gzip -cf > %s"
#define GUNZIPCOMMAND  "|gzip -cdfq %s"
#define COMPRESSCOMMAND  "|compress -c > %s"
#define UNCOMPRESSCOMMAND  "|uncompress -c %s"


struct raw3header {                          
  bool lsb;
  int dim[3];
  float pixdim[3];
  float origin[3];
  int offset;
  int datatype;
};

class Raw3IO {
 private:
  struct raw3header _hdr;
  bool swapped_data;
  unsigned char *_p;//specific type here for memory allocation (essentially void *)

  int openFileWrite(const char *pathname, FILE *& file);
  int openFileRead(const char *pathname, FILE *& file);
  void byteSwap(void *data, int numBytes, int numVals);
  void byteSwap(void *data, int numBytes);

 public:
  Raw3IO();         // Ctor
  Raw3IO(int xsize, int ysize, int zsize, int datatype = DT_SHORT, void *data = NULL, float xpixdim = 1.0, float ypixdim = 1.0, float zpixdim = 1.0);
  ~Raw3IO();        // Dtor

  void init_hdr();//init required and default fields

  int load(const char *);
  int save(const char *);
  int loadHDR(const char *hdr_pathname);
  int saveHDR(const char *hdr_pathname) ;
  int saveIMG(const char *hdr_pathname) ;

  int setDim(int xsize, int ysize, int zsize, int nbytes);
  char* readElem(FILE* file);
  inline void *address(void) { return _p; }

  // accessor functions
  inline int getXsize(void) { return _hdr.dim[0]; }
  inline int getYsize(void) { return _hdr.dim[1]; }
  inline int getZsize(void) { return _hdr.dim[2]; }
  inline int getDataType(void) { return _hdr.datatype; }
  inline float getVoxelXScale(void) { return _hdr.pixdim[0]; }
  inline float getVoxelYScale(void) { return _hdr.pixdim[1]; }
  inline float getVoxelZScale(void) { return _hdr.pixdim[2]; }
};

#endif
