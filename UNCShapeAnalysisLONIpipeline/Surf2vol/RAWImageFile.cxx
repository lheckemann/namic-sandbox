//#include <io.h>
#include <sys/stat.h>
/*#include <fcntl.h>
#include <ctype.h>*/
#include "RAWImageFile.h"

//#define DEBUG

#ifndef S_ISREG
#define S_ISREG(m)  (((S_IFREG & m) != 0) ? true : false)
#endif


const int UNKNOWN_GREY_VALUE = 99999;


enum order_t { ORDERERR = 0, NORMAL = 1, BYTEREV,
    PAIRREV, REVERSE, NATIVE };

order_t Order() {

  union {
      unsigned int  i;
      unsigned char  c[4];
  }  word;

  order_t  Machine_Byte_Order;

  for (register int j = 0; j < 4; j++) word.c[j] = j + 1;
    if (word.i == 000100401404) Machine_Byte_Order = NORMAL;
    else if (word.i == 00400601001) Machine_Byte_Order = REVERSE;
    else if (word.i == 00200202003) Machine_Byte_Order = BYTEREV;
    else Machine_Byte_Order = PAIRREV;
  return Machine_Byte_Order;
}


using namespace std;

Image3D * RAWImageFile::readRAW3(const char * filename)
{
    Image3D * image;

    bool machineIsLSB;
    bool swapBytes;
    char lsbStr[4];
    int xDim, yDim, zDim;
    int size;
    bool findMin, findMax;
    int minIntensity, maxIntensity;

    GreyValue * voxelArray;

    float xSpacing, ySpacing, zSpacing;
    float xOrig, yOrig, zOrig;
    Vector3D worldOriginPos;

    FILE * fp;
    struct stat buf;

    if (stat(filename, &buf) != 0 || S_ISREG(buf.st_mode) == false) {
    cout << "Not a valid file." << endl;
    return NULL;
    }
    fp = fopen(filename, "rb");
    skipComments(fp);

    // Determine the byte ordering of the machine

    order_t bo = Order();
  if (bo == REVERSE) {    
        machineIsLSB = true;
        cout << "Byte order detected as LSB" << endl;
    }
    else if (bo == NORMAL) {
        machineIsLSB = false;
        cout << "Byte order detected as MSB" << endl;
    }
  else {
        cout << "Machine has an unsupported byte order" << endl;
        return NULL;
    }

    // Determine file byte format LSB or MSB first
    fscanf(fp, "%3s ", &(lsbStr[0]));
    lsbStr[3] = '\0';

    if (strcmp(&(lsbStr[0]), "lsb") == 0) {
        if (machineIsLSB)
            swapBytes = false;
        else
            swapBytes = true;
    }
    else if (strcmp(&(lsbStr[0]), "msb") == 0) {
        if (machineIsLSB)
            swapBytes = true;
        else
            swapBytes = false;
    }
    else {  // Not a raw3 file
        cout << "Not a valid file" << endl;
        return NULL;
    }

    fscanf(fp, "%d %d %d ", &xDim, &yDim, &zDim);
    size = xDim * yDim * zDim;
#ifdef DEBUG
  cout << "Image dimensions: " << xDim << " x " << yDim << " x " << zDim << '\n';
#endif
    // Read in the world coordinate transformation parameters
    fscanf(fp, "%f %f %f %f %f %f ",
        &xSpacing, &ySpacing, &zSpacing, &xOrig, &yOrig, &zOrig);
#ifdef DEBUG
  cout << "Image spacings: " << xSpacing << ", " << ySpacing << ", " << zSpacing << '\n';
  cout << "Image origins: " << xOrig << ", " << yOrig << ", " << zOrig << '\n';
#endif

    // Determine the minimum and maximum values
    // or find them later if they are unknown (signified by UNKOWN_PIXVAL)
    fscanf(fp, "%d %d ", &minIntensity, &maxIntensity);
    if(minIntensity == UNKNOWN_GREY_VALUE)
        findMin = true;
    else
        findMin = false;
    if(maxIntensity == UNKNOWN_GREY_VALUE)
        findMax = true;
    else
        findMax = false;

    worldOriginPos.set(xOrig, yOrig, zOrig);

    // allocate space for the voxel array
    voxelArray = new GreyValue[xDim * yDim * zDim];

    short * temparray = new short[size];

    // Start back at the beginning of the file in binary read mode
    fseek(fp, 0, SEEK_SET);

    // Skip over the header of the file (comments plus dimensions, etc.)
    char c = '\0';
    int spaceCount = 0;
    while(spaceCount < 12)
    {
        fread(&c, 1, 1, fp);
        if(c == '#')
        {
            while(c != '\n')
                fread(&c, 1, 1, fp);
        }

        else if(c == ' ')
            spaceCount++;
    }

    // Read in the binary data
    fread(&(temparray[0]), sizeof(short), size, fp);
    fclose(fp);

    bool xFlip = false,
         yFlip = false,
         zFlip = false;

    // If any of our scale values are negative, 
    // we make them positive, flip the image, and recalculate the translation
    if(xSpacing < 0)
    {
        xFlip = true;
        xSpacing = -xSpacing;
        worldOriginPos.setX(worldOriginPos.getX() - ((double) xDim * xSpacing));
    }

    if(ySpacing < 0)
    {
        yFlip = true;
        ySpacing = -ySpacing;
        worldOriginPos.setY(worldOriginPos.getY() - ((double) yDim * ySpacing));
    }

    if(zSpacing < 0)
    {
        zFlip = true;
        zSpacing = -zSpacing;
        worldOriginPos.setZ(worldOriginPos.getZ() - ((double) zDim * zSpacing));
    }
#ifdef DEBUG
  if (xFlip || yFlip || zFlip) {
    cout << "Image adjusted spacings: " << xSpacing << ", " << ySpacing
      << ", " << zSpacing << '\n';
    cout << "Image adjusted origin: ";
    worldOriginPos.print();
  }
#endif

    short initVal;
    int xIndex, yIndex, zIndex;
    int i;

    // First loop finds the min or the max if needed (will also swap byte order if needed)
    if(findMin || findMax)
    {
        if(findMin)
            minIntensity = temparray[0];
        if(findMax)
            maxIntensity = temparray[0];

        for(i = 0; i < size; i++)
        {
            initVal = temparray[i];

            if(swapBytes)
                initVal = ((initVal >> 8) & 0x00FF) | (initVal << 8);

            if(findMin && initVal < minIntensity)
                minIntensity = initVal;
            if(findMax && initVal > maxIntensity)
                maxIntensity = initVal;
        }
    }

    GreyValue Xlatepixval, finalpixval;
    const double SCALECONST = (double) MAX_GREY_VALUE / (maxIntensity - minIntensity);

    i = 0;
    // Second loop fits values into the GREYVAL range
    for(int slicenum = 0; slicenum < zDim; slicenum++)
    {
        for(int rownum = 0; rownum < yDim; rownum++)
        {
            for(int colnum = 0; colnum < xDim; colnum++)
            {
                initVal = temparray[i];

                if(swapBytes)
                    initVal = ((initVal >> 8) & 0x00FF) | (initVal << 8);

                Xlatepixval = initVal - minIntensity;
                finalpixval = (GreyValue) (SCALECONST * Xlatepixval);

                if(xFlip)
                    xIndex = xDim - colnum - 1;
                else
                    xIndex = colnum;

                if(yFlip)
                    yIndex = yDim - rownum - 1;
                else
                    yIndex = rownum;

                if(zFlip)
                    zIndex = zDim - slicenum - 1;
                else
                    zIndex = slicenum;

                voxelArray[xIndex + xDim * (yIndex + yDim * zIndex)] = finalpixval;
        i++;
            }
        }
    }

    image = new Image3D;

    if(image != NULL)
    {
        image->setVoxels(voxelArray, xDim, yDim, zDim);
        image->setSpacing(xSpacing, ySpacing, zSpacing);
        image->setOriginPixelPos(worldOriginPos);
    }

    delete [] temparray;

    return image;
    // Calculate the extent of the image bounding box
//  double width = (double) xDim * xSpacing;
//  double height = (double) yDim * ySpacing;
//  double depth = (double) zDim * zSpacing;
//  double maxExtent;

//    if(width <= height)
//        maxExtent = height;
//    else
//        maxExtent = width;

//    if(maxExtent < depth)
//        maxExtent = depth;

//    xExtent = width / maxExtent;
//    yExtent = height / maxExtent;
//    zExtent = depth / maxExtent;
//    scaleFactor = maxExtent;

//  computeConversionFactors();
}


/*-------------------------------------------------------------------------------
Author: Isabelle Corouge
Date:
Description: lit un raw qui n'a pas d'entete.
In:
Out:
-------------------------------------------------------------------------------*/
Image3D * RAWImageFile::ReadmyRaw(const char * filename, 
          int xDim, int yDim, int zDim,
          float xSpacing, float ySpacing, float zSpacing,
          float xOrig, float yOrig, float zOrig)
{
  Image3D * image;

  bool machineIsLSB;
  bool swapBytes;
  //char lsbStr[4];
  int size;
  bool findMin, findMax;
  int minIntensity, maxIntensity;
  
  GreyValue * voxelArray;

  Vector3D worldOriginPos;

  FILE * fp;
  //struct stat buf;

  //  if (stat(filename, &buf) != 0 || S_ISREG(buf.st_mode) == false) {
  //  cout << "Not a valid file." << endl;
  //  return NULL;
  // }

  fp = fopen(filename, "rb");
  skipComments(fp);

  // Determine the byte ordering of the machine
  order_t bo = Order();
  if (bo == REVERSE) {    
    machineIsLSB = true;
    cout << "Byte order detected as LSB" << endl;
  }
  else if (bo == NORMAL) {
    machineIsLSB = false;
    cout << "Byte order detected as MSB" << endl;
  }
  else {
    cout << "Machine has an unsupported byte order" << endl;
    return NULL;
  }

  // Determine file byte format LSB or MSB first
  // fscanf(fp, "%3s ", &(lsbStr[0]));
  //lsbStr[3] = '\0';

  //if (strcmp(&(lsbStr[0]), "lsb") == 0) {
    if (machineIsLSB)
      swapBytes = false;
    else
      swapBytes = true;
    //}
    //else if (strcmp(&(lsbStr[0]), "msb") == 0) {
    //if (machineIsLSB)
    //  swapBytes = true;
    //else
    //  swapBytes = false;
    //}
    //else {  // Not a raw3 file
    //cout << "Not a valid file" << endl;
    //return NULL;
    //}

    //    fscanf(fp, "%d %d %d ", &xDim, &yDim, &zDim);
    size = xDim * yDim * zDim;

    // Read in the world coordinate transformation parameters
    //    fscanf(fp, "%f %f %f %f %f %f ", &xSpacing, &ySpacing, &zSpacing, &xOrig, &yOrig, &zOrig);

    // Determine the minimum and maximum values
    // or find them later if they are unknown (signified by UNKOWN_PIXVAL)
    //fscanf(fp, "%d %d ", &minIntensity, &maxIntensity);
    minIntensity = MIN_GREY_VALUE;
    maxIntensity = MAX_GREY_VALUE;
    if(minIntensity == UNKNOWN_GREY_VALUE)
      findMin = true;
    else
      findMin = false;
    if(maxIntensity == UNKNOWN_GREY_VALUE)
      findMax = true;
    else
      findMax = false;

    worldOriginPos.set(xOrig, yOrig, zOrig);

    // allocate space for the voxel array
    voxelArray = new GreyValue[xDim * yDim * zDim];
    short * temparray = new short[size];

    // Start back at the beginning of the file in binary read mode
    fseek(fp, 0, SEEK_SET);

    // Skip over the header of the file (comments plus dimensions, etc.)
//      char c = '\0';
//      int spaceCount = 0;
//      while(spaceCount < 12) {
//        fread(&c, 1, 1, fp);
//        if(c == '#') {
//    while(c != '\n')
//      fread(&c, 1, 1, fp);
//        }
//        else if(c == ' ')
//    spaceCount++;
//      }

    // Read in the binary data
    fread(&(temparray[0]), sizeof(short), size, fp);
    fclose(fp);

    bool xFlip = false, yFlip = false, zFlip = false;
    
    // If any of our scale values are negative, 
    // we make them positive, flip the image, and recalculate the translation
    if(xSpacing < 0) {
      xFlip = true;
      xSpacing = -xSpacing;
      worldOriginPos.setX(worldOriginPos.getX() - ((double) xDim * xSpacing));
    }
    if(ySpacing < 0){
      yFlip = true;
      ySpacing = -ySpacing;
      worldOriginPos.setY(worldOriginPos.getY() - ((double) yDim * ySpacing));
    }

    if(zSpacing < 0) {
      zFlip = true;
      zSpacing = -zSpacing;
      worldOriginPos.setZ(worldOriginPos.getZ() - ((double) zDim * zSpacing));
    }

    short initVal;
    int xIndex, yIndex, zIndex;
    int i;

    // First loop finds the min or the max if needed (will also swap byte order if needed)
    if(findMin || findMax) {
      if(findMin)
    minIntensity = temparray[0];
      if(findMax)
    maxIntensity = temparray[0];
      for(i = 0; i < size; i++) {
    initVal = temparray[i];
    if(swapBytes)
      initVal = ((initVal >> 8) & 0x00FF) | (initVal << 8);
    if(findMin && initVal < minIntensity)
      minIntensity = initVal;
    if(findMax && initVal > maxIntensity)
      maxIntensity = initVal;
      }
    }

    GreyValue Xlatepixval, finalpixval;
    const double SCALECONST = (double) MAX_GREY_VALUE / (maxIntensity - minIntensity);

    i = 0;
    // Second loop fits values into the GREYVAL range
    for(int slicenum = 0; slicenum < zDim; slicenum++) {
      for(int rownum = 0; rownum < yDim; rownum++){
  for(int colnum = 0; colnum < xDim; colnum++){

    initVal = temparray[i];
    if(swapBytes)
      initVal = ((initVal >> 8) & 0x00FF) | (initVal << 8);

    Xlatepixval = initVal - minIntensity;
    finalpixval = (GreyValue) (SCALECONST * Xlatepixval);

    if(xFlip) xIndex = xDim - colnum - 1;
    else xIndex = colnum;
    
    if(yFlip)  yIndex = yDim - rownum - 1;
    else yIndex = rownum;
    
    if(zFlip) zIndex = zDim - slicenum - 1;
    else zIndex = slicenum;

    voxelArray[xIndex + xDim * (yIndex + yDim * zIndex)] = finalpixval;
    i++;
  }
      }
    }

    image = new Image3D;
    if(image != NULL) {
        image->setVoxels(voxelArray, xDim, yDim, zDim);
        image->setSpacing(xSpacing, ySpacing, zSpacing);
        image->setOriginPixelPos(worldOriginPos);
    }

    delete [] temparray;

    return image;
}


void RAWImageFile::write(const char * filename, Image3D & image)
{
    FILE * fpBIN;
    // FILE * fpBIN;

    int xDim, yDim, zDim;
    double xSpacing, ySpacing, zSpacing;
    Vector3D originPos;

    GreyValue * voxels;

    char * filenameASCII = new char [255];
    char * filenameBIN = new char [255];
    strcat(strcat(strcpy(filenameASCII,filename),"ASCII"),".raw");
    strcat(strcpy(filenameBIN,filename),"_BIN.raw");
    //strcat(strcpy(filenameBIN,filename),".raw");
    //fpASCII = fopen(filenameASCII, "wb");
    fpBIN = fopen(filenameBIN, "wb");
    //if(fpASCII == NULL || fpBIN == NULL)
      if(fpBIN == NULL)
        return;

    xDim = image.getXDim();
    yDim = image.getYDim();
    zDim = image.getZDim();

    xSpacing = image.getXSpacing();
    ySpacing = image.getYSpacing();
    zSpacing = image.getZSpacing();

    originPos = image.getOriginPixelPos();

    voxels = image.getVoxels();
  

//      fprintf(fp, "lsb ");
//      fprintf(fp, "%d %d %d ", xDim, yDim, zDim);
//      fprintf(fp, "%f %f %f ", xSpacing, ySpacing, zSpacing);
//      fprintf(fp, "%f %f %f ", originPos.getX(), originPos.getY(), originPos.getZ());
//      fprintf(fp, "%d %d ", (int) (minint * MAX_GREY_VALUE), (int) (maxint * MAX_GREY_VALUE));
 
    // ecriture binaire
    fwrite(voxels, xDim * yDim * zDim, sizeof(GreyValue), fpBIN);
   
    // ecriture ascii
    //for (int i=0 ; i< xDim*yDim*zDim ; i++)
    // fprintf(fpASCII,"%u ",voxels[i]);
    
    //  fclose(fpASCII);
    fclose(fpBIN);
}

/*-------------------------------------------------------------------------------
Author: Isabelle Corouge
Date:
Description: lit un raw qui n'a pas d'entete.
In:
Out:
-------------------------------------------------------------------------------*/
void RAWImageFile::writeRAW3(const char * filename, Image3D & image)
{
    FILE * fp;

    int xDim, yDim, zDim;
    double xSpacing, ySpacing, zSpacing;
    Vector3D originPos;

    GreyValue * voxels;

    fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    xDim = image.getXDim();
    yDim = image.getYDim();
    zDim = image.getZDim();

    xSpacing = image.getXSpacing();
    ySpacing = image.getYSpacing();
    zSpacing = image.getZSpacing();

    originPos = image.getOriginPixelPos();

    voxels = image.getVoxels();
      
    double minint = image.getMinIntensityWindow();
    double maxint = image.getMaxIntensityWindow();
    
    fprintf(fp, "lsb ");
    fprintf(fp, "%d %d %d ", xDim, yDim, zDim);
    fprintf(fp, "%f %f %f ", xSpacing, ySpacing, zSpacing);
    fprintf(fp, "%f %f %f ", originPos.getX(), originPos.getY(), originPos.getZ());
    fprintf(fp, "%d %d ", (int) (minint * MAX_GREY_VALUE), (int) (maxint * MAX_GREY_VALUE));

    fwrite(voxels, xDim * yDim * zDim, sizeof(GreyValue), fp);

    fclose(fp);
}

void RAWImageFile::skipComments(FILE * fp)
{
    char c;

    c = getc(fp);
    while((c == '#' || isspace(c)) && c != EOF)
    {
        while(c != '\n' && c != EOF)
            c = getc(fp);

        if(c != EOF)
            c = getc(fp);
    }

    ungetc(c, fp);
}
