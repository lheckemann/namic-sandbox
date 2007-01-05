#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raw3io.h"
#include "qlistbox.h"

#include <iostream>

Raw3IO::Raw3IO() {
  _p = 0;
  memset(&_hdr, 0, sizeof(_hdr));
  setDim(0,0,0,0);
}

Raw3IO::~Raw3IO() {
  setDim(0, 0, 0,0);
}

Raw3IO::Raw3IO(int xsize, int ysize, int zsize, int datatype, void *data, float xpixdim, float ypixdim, float zpixdim)
{
  _p = 0;
  memset(&_hdr, 0, sizeof(_hdr));
  setDim(0,0,0,0);
  int nbytes = 0;
  switch(datatype)
  {
  case DT_CHAR:
    nbytes = sizeof(char);
    break;
  case DT_SHORT:
    nbytes = sizeof(short);
    break;
  case DT_INT:
    nbytes = sizeof(int);
    break;
  default:
    nbytes = 0;
  std::cout << "unsupported Raw3 data type!" << std::endl;
    return;
  }
  _hdr.datatype = datatype;
  setDim(xsize,ysize,zsize,nbytes);
  _hdr.pixdim[0] = xpixdim;
  _hdr.pixdim[1] = ypixdim;
  _hdr.pixdim[2] = zpixdim;

  memcpy(_p,data,xsize*ysize*zsize*nbytes);
}


void Raw3IO::byteSwap(void *data, int numBytes)
{
  if (numBytes <= 1) return;

  unsigned char * p = (unsigned char *)data;
  int i,j;
  unsigned char temp;
  for (i = 0, j = numBytes-1; i < j; i++, j--)
  {
    temp = p[i];
    p[i] = p[j];
    p[j] = temp;
  }
}


void Raw3IO::byteSwap(void *data, int numBytes, int numVals)
{
  if (numBytes <= 1) return;
  int i;

  unsigned char * p = (unsigned char *)data;

  for (i = 0; i < numVals; i++)
  {
    byteSwap((void*)p,numBytes);
    p += numBytes;
  }
}

int
Raw3IO::setDim(int xsize, int ysize, int zsize, int nbytes) {
  if ( _p ) {
    delete [] _p;
    _p = 0;
    _hdr.dim[0] = 0;
    _hdr.dim[1] = 0;
    _hdr.dim[2] = 0;
  }

  if (xsize < 0 || ysize < 0 || zsize < 0 || nbytes < 0)
  {
    std::cout << "Raw3IO::setDim negative dimensions attempted." << std::endl;
    return 0;
  }


    _hdr.dim[0] = xsize;
    _hdr.dim[1] = ysize;
    _hdr.dim[2] = zsize;
  
    _p   = new unsigned char [zsize*ysize*xsize*nbytes];

    if(!_p)
    {
    std::cout << "Raw3IO::setDim New failed." << std::endl;
      return 0;
    }

  return 1;
}

int Raw3IO::openFileRead(const char *pathname, FILE *& file)
{
  char *t1 = strrchr(pathname,'/');//skip past directories
  if (NULL == t1) t1 = strchr(pathname,'.');//no directories, so start at beginning
  else  t1 = strchr(t1,'.');//find first extension (file type)
  char *t2 = NULL;//for finding second extension (.gz,.Z), if any

  if (NULL == t1)//no extension, shouldn't ever happen
  {
    std::cout << "Raw3IO::openFileRead invalid filename (no extension)" << std::endl;
    return 0;
  }

  if (0 != strncmp(t1,".raw3",4))
  {
    std::cout << "Raw3IO::openFileRead invalid filename (not .raw3)" << std::endl;
    return 0;
  }

  t2 = strchr(t1+1,'.');//look for .gz, .Z
  if (NULL != t2)
  {
#ifdef _WIN32
    std::cout << "Raw3IO::openFileRead compression not supported" << std::endl;
    return 0;
#else
    //use pipes and popen to unzip as we read
    char *act_name = NULL;

    if (0==strcmp(t2,".gz"))
    {
      //open pipe
      act_name = new char[strlen(GUNZIPCOMMAND) + strlen(pathname) + 1];
      (void) sprintf(act_name,GUNZIPCOMMAND,pathname);
    }
    else if (0==strcmp(t2,".Z"))
    {
      //open pipe
      act_name = new char[strlen(UNCOMPRESSCOMMAND) + strlen(pathname) + 1];
      (void) sprintf(act_name,UNCOMPRESSCOMMAND,pathname);
    }
    else
    {
      std::cout << "Raw3IO::openFileRead invalid filename (too many extension)" << std::endl;
      return 0;
    }

    if (!(file = (FILE *) popen(act_name+1, "rb"))) 
    {
      std::cout << "Raw3IO::openFileRead error opening file" << std::endl;
      return 0;
    }
    
    if (act_name) delete [] act_name;

    return 2;
#endif
  }
  else
  {
    //no compression, open normally
    if (!(file = (FILE *) fopen(pathname,"rb")))
    {
      std::cout << "AnalyzeVol::openFileRead error opening file" << std::endl;
      return 0;
    }
    
    return 1;
  }
}

int Raw3IO::openFileWrite(const char *pathname, FILE *& file)
{
  QString m_path(pathname);
  QString m_compression="";
  QString m_extension = "";
  if ((m_path.mid(m_path.findRev(".")) == ".gz") || (m_path.mid(m_path.findRev(".")) == ".Z"))
  {
    m_compression = m_path.mid(m_path.findRev("."));
  }
  else
  m_extension = m_path.mid(m_path.findRev("."));


  char *t1 = strrchr(pathname,'/');//skip past directories
  if (NULL == t1) t1 = strchr(pathname,'.');//no directories, so start at beginning
  else  t1 = strchr(t1,'.');//find first extension (file type)
  char *t2 = NULL;//for finding second extension (.gz,.Z), if any

  if (m_extension.isEmpty())//no extension, shouldn't ever happen
  {
    std::cout << "Raw3IO::openFileWrite invalid filename (no extension)" << std::endl;
    return 0;
  }

  std::cout << "Extension:" <<t1<<std::endl; 

  if (m_extension != ".raw3")
  {
    std::cout << "Raw3IO::openFileWrite invalid filename (not .raw3)" << std::endl;
    return 0;
  }


  if (!m_compression.isEmpty())
  {
#ifdef _WIN32
    std::cout << "Raw3IO::openFileWrite compression not supported" << std::endl;
    return 0;
#else
    //use pipes and popen zip as we write
    char *act_name = NULL;

    if (0==strcmp(t2,".gz"))
    {
      //open pipe
      act_name = new char[strlen(GZIPCOMMAND) + strlen(pathname) + 1];
      (void) sprintf(act_name,GZIPCOMMAND,pathname);
    }
    else if (0==strcmp(t2,".Z"))
    {
      //open pipe
      act_name = new char[strlen(COMPRESSCOMMAND) + strlen(pathname) + 1];
      (void) sprintf(act_name,COMPRESSCOMMAND,pathname);
    }
    else
    {
    std::cout << "Raw3IO::openFileWrite invalid filename (too many extensions)" << std::endl;
      return 0;
    }

    if (!(file = (FILE *) popen(act_name+1, "wb"))) 
    {
    std::cout << "Raw3IO::openFileWrite error opening file (popen)" << std::endl;
      return 0;
    }
    
    if (act_name) delete [] act_name;

    return 2;
#endif
  }
  else
  {
    //no compression, open normally
    if (!(file = (FILE *) fopen(pathname,"wb")))
    {
      std::cout << "Raw3IO::openFileWrite error opening file (fopen)"<< std::endl;
      return 0;
    }
    
    return 1;
  }
}

int Raw3IO::load(const char *pathname) 
{
  loadHDR(pathname);
  return 1;
}

int Raw3IO::loadHDR(const char *hdr_pathname) 
{
  FILE  *fp;

  int compression = openFileRead(hdr_pathname,fp);

  int retval = 1;

  if (compression == 0) return 0;


  //Read Header

  //Check for LSB
  if (!strncmp(readElem(fp),"lsb",3))
    swapped_data = true;
  else
    swapped_data = false;  

  //Image size
  _hdr.dim[0] = atoi(readElem(fp));
  _hdr.dim[1] = atoi(readElem(fp));
  _hdr.dim[2] = atoi(readElem(fp));


  //Pixdims
  _hdr.pixdim[0] = atof(readElem(fp));
  _hdr.pixdim[1] = atof(readElem(fp));
  _hdr.pixdim[2] = atof(readElem(fp));

  //std::cout << "pixdim: " <<  _hdr.pixdim[0] << " | " << _hdr.pixdim[1] << " | " << _hdr.pixdim[2] << std::endl;

  //Origin 
  _hdr.origin[0] = atof(readElem(fp));
  _hdr.origin[1] = atof(readElem(fp));
  _hdr.origin[2] = atof(readElem(fp));

  //Offset
  _hdr.offset = atoi(readElem(fp));

  //Image Type
  _hdr.datatype = atoi(readElem(fp));


  if (retval == 0)
    std::cout << "Raw3IO::load Header error reading header" << std::endl;


  //Load image
  int numelem = _hdr.dim[0] * _hdr.dim[1] * _hdr.dim[2];
  int datasize= 0;
  if (_hdr.datatype == DT_CHAR) datasize = sizeof(char);
  if (_hdr.datatype == DT_SHORT) datasize = sizeof(short);
  if (_hdr.datatype == DT_INT) datasize = sizeof(int);
  

  setDim(_hdr.dim[0],_hdr.dim[1],_hdr.dim[2],datasize);

  if (0 == fread(_p,datasize,numelem,fp))
  {
       perror("Raw3IO::loadIMG error reading data file (fread)");
       retval = 0;
  }

  #ifdef _WIN32
   if (swapped_data == true)
    byteSwap((void*)_p,datasize,numelem);
  #else
   if (swapped_data == false)
    byteSwap((void*)_p,datasize,numelem);
  #endif  


#ifndef _WIN32
  if (2 == compression)
    pclose(fp);
  else
#endif
    fclose(fp);

  return retval;
}

/* Read header element */
char* Raw3IO::readElem(FILE* file)
{
  char* text;
  text = (char *)malloc(1000);
  char car ='\0';
  int i=0;
  while (car != ' ')
  {
    fread(&car,1,sizeof(char),file);
    text[i++] = car;
  }
  text[i-1] = '\0';
  return text;
}

int Raw3IO::save(const char *pathname) 
{
  FILE  *fp;

  int compression = openFileWrite(pathname,fp);

  if (compression == 0) return 0;
  int retval = 1;

  //Write Raw3 only in LSB mode and byteswap image data for Windows platforms
  char m_text[40];
  strcpy(m_text,"lsb ");
  if (0 == fwrite(&m_text,4, 1, fp))
  {
    std::cout << "AnalyzeVol::saveHDR error writing hdr" << std::endl;
      retval = 0;
  }

  // Write Image size
  sprintf(m_text,"%i ",_hdr.dim[0]);
  fwrite(&m_text,strlen(m_text), 1, fp);
  sprintf(m_text,"%i ",_hdr.dim[1]);
  fwrite(&m_text,strlen(m_text), 1, fp);
  sprintf(m_text,"%i ",_hdr.dim[2]);
  fwrite(&m_text,strlen(m_text), 1, fp);


  //Write Pixdims
  sprintf(m_text,"%f ",_hdr.pixdim[0]);
  fwrite(&m_text,strlen(m_text), 1, fp);
  sprintf(m_text,"%f ",_hdr.pixdim[1]);
  fwrite(&m_text,strlen(m_text), 1, fp);
  sprintf(m_text,"%f ",_hdr.pixdim[2]);
  fwrite(&m_text,strlen(m_text), 1, fp);

  //Write Origin 
  sprintf(m_text,"%f ",_hdr.origin[0]);
  fwrite(&m_text,strlen(m_text), 1, fp);
  sprintf(m_text,"%f ",_hdr.origin[1]);
  fwrite(&m_text,strlen(m_text), 1, fp);
  sprintf(m_text,"%f ",_hdr.origin[2]);
  fwrite(&m_text,strlen(m_text), 1, fp);

  //Write Offset - Always 0
  strcpy(m_text,"0 ");
  fwrite(&m_text,2, 1, fp);

  //Write Image Type
  sprintf(m_text,"%i ",_hdr.datatype);
  fwrite(&m_text,strlen(m_text), 1, fp);  

  
  //Write Image
  int numelem = _hdr.dim[0] * _hdr.dim[1] * _hdr.dim[2];
  int datasize= 0;
  if (_hdr.datatype == DT_CHAR) datasize = sizeof(char);
  if (_hdr.datatype == DT_SHORT) datasize = sizeof(short);
  if (_hdr.datatype == DT_INT) datasize = sizeof(int);

  #ifdef _WIN32
  //swap bytes
  byteSwap(address(),datasize,numelem);
  #endif

  if (fwrite(address(), datasize, numelem, fp) != (unsigned int)numelem)
  {
    std::cout << "Raw3::saveIMG error writing image" << std::endl;
    retval = 0;
  }

  #ifdef _WIN32
    //swap back
    byteSwap(address(),datasize,numelem);
  #endif


#ifndef _WIN32
  if (2 == compression)
    pclose(fp);
  else
#endif
    fclose(fp);



  return 1;
}

int Raw3IO::saveHDR(const char *hdr_pathname) 
{
  int retval =1;
  return retval;
}

int Raw3IO::saveIMG(const char *img_pathname) 
{
  int retval = 0;
  return retval;

}
