/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkFITSImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2005/10/10 19:33:50 $
  Version:   $Revision: 1.19 $  

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// TODO: Fix the above copyright, which maybe should be IIC, and the RCS stuff,
// which is not right for Subversion.


#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <math.h>
#include <zlib.h>

#include <itkExceptionObject.h>
#include <itkByteSwapper.h>
#include <itkMetaDataObject.h>
#include <itksys/SystemTools.hxx>

#include <wcs.h>

#include "itkFITSImageIO.h"
#include <grparser.h> // for FITS NGP_MAX_ARRAY_DIM

extern "C" { char* GetFITShead(const char* filepath, bool); }

using std::string;
using std::stringstream;
using std::ostream;
using std::cerr;
using std::endl;
using std::vector;

// Forward declarations:
static bool checkExtension(const string&);
static string getAllFitsErrorMessages(int status);
static inline double square(double);

namespace itk {

//-----------------------------------------------------------------------------
// CanReadFile(): virtual method of FITSImageIO
//   implements pure virtual method of ImageIOBase
//-----------------------------------------------------------------------------

/*method*/ bool
FITSImageIO::CanReadFile(const char* const filepath) 
{ 
  cerr << "DEBUG: Entering FITSImageIO::CanReadFile()." << endl; //d

  if (!filepath or *filepath == 0) {
    itkDebugMacro(<< "No filename specified.");
    return false;
  } else if (::checkExtension(filepath)) {
    cerr << "DEBUG: Exiting FITSImageIO::CanReadFile() with true." << endl; //d
    return true;
  } else {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
  }
}


//-----------------------------------------------------------------------------
// CanWriteFile(): virtual method of FITSImageIO
//   implements pure virtual method of ImageIOBase
//-----------------------------------------------------------------------------

/*method*/ bool
FITSImageIO::CanWriteFile(const char* const name)
{

  
  // Return false because we currently don't implement writing FITS files:
  return false;

  // TODO: Implement this if ever need to write FITS files.

  const string filepath = name;
  if (filepath == "") {
    itkDebugMacro(<< "No filename specified.");
  }

  bool extensionFound = ::checkExtension(name);

  if(!extensionFound) {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
  }

  if (extensionFound) {
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
// ReadImageInformation(): virtual method of FITSImageIO
//   implements pure virtual method of ImageIOBase
//-----------------------------------------------------------------------------

//! Read information about the FITS file and put the cursor of the
//! stream just before the first data pixel.

/*method*/ void
FITSImageIO::ReadImageInformation()
{
  // CFITSIO writes to this to indicate errors:
  int status = 0;   

  // Open the FITS file:
  ::fits_open_file(&m_fitsFile, this->GetFileName(), READONLY, &status);
  if (status) {
    itkExceptionMacro("FITSImageIO could not open FITS file: \""
                      << this->GetFileName() << " for reading: "
                      << ::getAllFitsErrorMessages(status) << '.');
  }

  // TODO: Use CFITSIO to populate the MetaDataDictionary.
  //
  // Documentation regarding this:
  //
  //
  //    CFITSIO Quick Start Guide, p. 4: How to get the list of tags in the
  //    header.
  //
  //    CFITSIO User's Guide, p. 34 
   

  // TODO: Needed when we populate MetaDataDictionary:
  // typedef string defaultValueType;  

  // TODO: We may want to open the FITS file in the ctor rather than
  // in each method, since if we end up opening a compressed file
  // multiple times, in different methods, the file will end up being
  // uncompressed multiple times.

  cerr << "DEBUG: Entering FITSImageIO::ReadImageInformation()." << endl; //d

  // Get the dimensions and type of the image:
  int numOfAxes;
  long lengthOfAxisInPixels[NGP_MAX_ARRAY_DIM];
  int bitsPerPixel;
  ::fits_get_img_param(m_fitsFile, NGP_MAX_ARRAY_DIM, &bitsPerPixel,
                       &numOfAxes, lengthOfAxisInPixels, &status);

  cerr << "NAXIS=" << numOfAxes << endl; //d
  cerr << "DIMS="; //d
  for (long i = 0; i < numOfAxes; ++i) {  //d
    cerr << " " << lengthOfAxisInPixels[i]; //d
  }
  cerr << endl;
  cerr << "BPP= " << bitsPerPixel << endl;

  if (numOfAxes != 3) {
    itkExceptionMacro("FITSImageIO cannot handle FITS files that are anything"
                      " other than three dimensional.");
    
    // TODO: Remove this restriction?
  }

  // BEGIN getting RA and Dec of border pixels.

  const bool verbose = true;
  char* const header = GetFITShead(this->GetFileName(), !verbose);

       // Note: header is malloc'ed, so we must free it, rather than delete it.

       // TODO: Implement the above without reading the file, since in the
       // future, we want to be able to rely on CFITSIO's ability to read files
       // over the network, or to have CFITSIO do slicing and transformation of
       // the file, based on filename.
  
  if (!header) {
    itkExceptionMacro("FITSImageIO could not get FITS headers from file: \""
                      << this->GetFileName() << ".");
  }

  WorldCoor* wcs = wcsinit(header);
  free(header);                 // TODO: Do this via RAII.
  double lowerLeftRA, lowerLeftDec;
  pix2wcs(wcs, 1, 1, &lowerLeftRA, &lowerLeftDec);
  double lowerRightRA, lowerRightDec;
  pix2wcs(wcs, lengthOfAxisInPixels[0], 1, &lowerRightRA, &lowerRightDec);
  double upperLeftRA, upperLeftDec;
  pix2wcs(wcs, 1, lengthOfAxisInPixels[1], &upperLeftRA, &upperLeftDec);
  free(wcs);                    // TODO: Do this via RAII.

  cerr << "LL: RA=" << lowerLeftRA << ' ' << "Dec=" << lowerLeftDec << endl; //d
  cerr << "UL: RA=" << upperLeftRA << ' ' << "Dec=" << upperLeftDec << endl; //d
  cerr << "LR: RA=" << lowerRightRA << ' ' << "Dec=" << lowerRightDec << endl; //d


  // Make some RAs negative (by substracting 360 degrees) if the image crosses
  // the equinox.  Otherwise, we will incorrectly think that we are
  // representing a huge area of the sky, instead of a small area:
  const double xRaDiff = lowerRightRA - lowerLeftRA;
  if (xRaDiff > 180.0) lowerRightRA -= 360.0;
  else if (xRaDiff < -180.0) lowerLeftRA -= 360.0;

  const double yRaDiff = upperLeftRA - lowerLeftRA;
  if (yRaDiff > 180.0) upperLeftRA -= 360.0;
  else if (yRaDiff < -180.0) lowerLeftRA -= 360.0;

  cerr << "LL: RA=" << lowerLeftRA << ' ' << "Dec=" << lowerLeftDec << endl; //d
  cerr << "UL: RA=" << upperLeftRA << ' ' << "Dec=" << upperLeftDec << endl; //d
  cerr << "LR: RA=" << lowerRightRA << ' ' << "Dec=" << lowerRightDec << endl; //d


  // TODO: Someday we should probably think of a better way to do things than
  // this terrible linear approximation with RA and Dec.  This method won't
  // work near the poles or for large areas of the sky.  The above method for
  // removing an RA discontinuity should be improved for large areas of the sky
  // (in the unlikely case that we were to continue to take this sort of
  // approach at all), as we would have to determine the direction that RA is
  // moving along each axis by looking at a small increment along the axis.
  // Without testing the direction of movement for a small increment, we can't
  // tell whether the image is flipped along the axis or extends more than 180
  // degrees around the celstial sphere.

  // END getting RA and Dec of border pixels.

  // We are now going to calculate the vectors that provide the information on
  // how to transform from from pixel coordinates into spatial coordinates.
  // [TODO: this method only works for linear transformation, which is not
  // right when we are near the poles or for wide fields of view, but it's the
  // best we can do within the framework we have at the moment.]  There are
  // five vectors that provide the required information, three direction cosine
  // vectors, a spacing vector, and an origin vector.  The direction cosine
  // vectors are unit vectors that indicate the rotations involved in the
  // coordinate transformation, the spacing vector indicates any scaling
  // involved, and the origin vector indicates the physical coordinates that
  // correspond to the image origin.

  // The first four of these vectors (the three direction cosine vectors and
  // the spacing vector) can really be thought of as three direction vectors
  // that have been factored out into unit vectors (the direction cosines) and
  // a scaling vector.  And, in fact, we will now calculate the four vectors by
  // first determining the three direction vectors and then performing the
  // factoring.

  // Calculate the direction vectors:
  double iToRA = (lowerRightRA - lowerLeftRA) / (lengthOfAxisInPixels[0]-1);
  double iToDec = (lowerRightDec - lowerLeftDec) / (lengthOfAxisInPixels[0]-1);
  double jToRA = (upperLeftRA - lowerLeftRA) / (lengthOfAxisInPixels[1]-1);
  double jToDec = (upperLeftDec - lowerLeftDec) / (lengthOfAxisInPixels[1]-1);

  cerr << "iToRA=" << iToRA << endl;  //d
  cerr << "iToDec=" <<  iToDec << endl; //d
  cerr << "jToRA=" << jToRA << endl; //d
  cerr << "jToDec=" << jToDec << endl; //d

  // TODO: Extend the following to more than three dimensions.

  vector<double> directionVectors[3];
  for (int axis = 0; axis < 3; ++axis) directionVectors[axis].resize(3);
  directionVectors[0][0] = iToRA;
  directionVectors[0][1] = iToDec;
  directionVectors[0][2] = 0;

  directionVectors[1][0] = jToRA;
  directionVectors[1][1] = jToDec;
  directionVectors[1][2] = 0;

  directionVectors[2][0] = 0;
  directionVectors[2][1] = 0;
  directionVectors[2][2] = 1;

  // TODO: Extract the dimensions for the velocity spectrum from the FITS 
  // file rather than punting and just setting it to 1 as we have done above.

  // Normalize the direction vectors into direction cosines, while also
  // populating the spacing vector:
  double spacing[3];
  vector<double> directionCosines[3];
  for (int i = 0; i < 3; ++i) {
    directionCosines[i].resize(3);
    spacing[i] = sqrt(square(directionVectors[i][0]) + 
                      square(directionVectors[i][1]) +
                      square(directionVectors[i][2]));
    for (int j = 0; j < 3; ++j) directionVectors[i][j] /= spacing[i];
  }

  // TODO: The following is a gross hack.  The problem with not doing this hack
  // is that the velocity spectrum dimension is likely to have a completely
  // different scale than the other two dimensions, and Slicer isn't clever
  // enough yet to handle such an issue in any reasonable way.  It will either
  // show the dimension as completely scrunched up, or it will be elongated
  // (and then clipped) to the point of uselessness.  So for now, we just set
  // the spacing to be the same as it is for Right Ascension:
  spacing[2] = spacing[0];

  // Make the origin vector:
  const double origin[3] = { lowerLeftRA, lowerLeftDec, 0 };

  // Set up the ITK image:
  this->SetNumberOfComponents(1);
  this->SetPixelType(SCALAR);
  this->SetComponentType(FLOAT);
  this->SetNumberOfDimensions(numOfAxes);
  for (int axis = 0; axis < numOfAxes; ++axis) {
    this->SetDimensions(axis, lengthOfAxisInPixels[axis]);
    this->SetOrigin(axis, origin[axis]);
    this->SetSpacing(axis, spacing[axis]);
    cerr << "spacing=" << axis << "," << spacing[axis] << " "; //d
    cerr << "directions="; //d
    for (int j = 0; j < 3; ++j) { //d
      cerr << directionCosines[axis][j] << " "; //d
    } //d
    this->SetDirection(axis, directionCosines[axis]);
  }
  cerr << endl; //d


  // TODO: To populate the MetaDataDictionary, start with this code
  // and make it work right:

//    // Get the header
//    int numberOfHeaderEntries = 0;
//    ::fits_get_hdrspace(fptr, &numberOfHeaderEntries, NULL, &status);

//    // TODO: check this number by looking at cfits code...
//    const unsigned int maxRecordLength = 1024;

//    char charKey[ maxRecordLength ];
//    char charValue[ maxRecordLength ];
//    char* dontNeedTheComment = NULL;

//    // Get the records and populate the MetaDataDictionary 
//    MetaDataDictionary& thisDic = this->GetMetaDataDictionary();

//    for(unsigned int keyentry=0; keyentry < numberOfHeaderEntries; keyentry++)
//      { 
//        unsigned int keyentryFortran = keyentry + 1;

//        // Get the record from cfits
//        ::fits_read_keyn(fptr, keyentry, charKey, charValue,
//                      dontNeedTheComment, &status);
                        

//        if (status != 0) {
//         break;  // TODO: report an error
//        }
 
//       string stringKey   = charKey;
//       string stringValue = charValue;

//       // Put the record in the MetaDataDictionary
//       itk::EncapsulateMetaData<defaultValueType>(thisDic, stringKey,
//                                               stringValue);
//     }

}


//-----------------------------------------------------------------------------
// Read(): virtual method of FITSImageIO
//   implements pure virtual method of ImageIOBase
//-----------------------------------------------------------------------------

/*method*/ void
FITSImageIO::Read(void* const buffer)
{
  // TODO: At some point we might want to preserve the native pixel
  // type, rather than converting everything into a float.  Achieving
  // this is complicated, however, by the fact that ITK image types
  // are parameterized by the pixel type.

  float* bufferAsFloats = static_cast<float*>(buffer);

  // Make a pixel location array that represents the origin pixel; i.e., [1,
  // 1, 1, ...]:
  const unsigned nDimensions = this->GetNumberOfDimensions();
  long origin[nDimensions];
  for (int i = 0; i < nDimensions; ++i) {
    origin[i] = 1;
  }

  // Calculate the total number of pixels in the image:
  long nPixels = 1;
  for (int i = 0; i < nDimensions; ++i) {
    nPixels *= GetDimensions(i);
  }

  // Read the FITS image into the ITK image buffer:
  int status = 0;
  ::fits_read_pix(m_fitsFile, TFLOAT, origin, nPixels, NULL,
                  bufferAsFloats, NULL, &status);
  if (status) {
    itkExceptionMacro("FITSImageIO::Read() could not read the primary data "
                      "array from FITS file \""
                      << this->GetFileName() << "\": "
                      << ::getAllFitsErrorMessages(status) << ".");
  }
                

//   // TODO: Remove this terrible hack.
//   // Scale the values by 1,000:
//   const float* const lastPixel = bufferAsFloats + nPixels;
//   for (float* pixelPtr = bufferAsFloats; pixelPtr < lastPixel; ++pixelPtr) {
//     *pixelPtr = 1000 * *pixelPtr;
//   }

  // Close the FITS file:
  ::fits_close_file(m_fitsFile, &status);
  if (status) {
    itkExceptionMacro("FITSImageIO::Read() could not close FITS file \""
                      << this->GetFileName()
                      << "\" after reading the primary data array: "
                      << ::getAllFitsErrorMessages(status) << ".");
  }
}


//-----------------------------------------------------------------------------
// WriteImageInformation(): virtual method of FITSImageIO
//   implements pure virtual method of ImageIOBase
//-----------------------------------------------------------------------------

/*method*/ void 
FITSImageIO::WriteImageInformation()
{
  // TODO: Implement with help of cfitsio
}


//-----------------------------------------------------------------------------
// Write(): virtual method of FITSImageIO
//   implements pure virtual method of ImageIOBase
//-----------------------------------------------------------------------------

//! The Write() function is not yet implemented.

/*method*/ void 
FITSImageIO::Write(const void* const buffer) 
{
  // TODO: Maybe implement this someday.

  itkExceptionMacro("FITSImageIO::Write() not implemented.");
}


//-----------------------------------------------------------------------------
// PrintSelf(): virtual method of FITSImageIO
//   overrides partially implement method of ImageIOBase
//-----------------------------------------------------------------------------

/*method*/ void
FITSImageIO::PrintSelf(ostream& os, const Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "PixelType " << m_PixelType << "\n";
}

} // END namespace itk


//*****************************************************************************
//***                     Local procedures                                  ***
//*****************************************************************************

//-----------------------------------------------------------------------------
// toLower(): local proc
//-----------------------------------------------------------------------------

// Converts \a s to lowercase.

/*local proc*/ void
toLower(string& s)
{
   for (string::iterator p = s.begin(); p != s.end( ); ++p) {
     *p = tolower(*p);
   }
}


//-----------------------------------------------------------------------------
// endMatchesP(): local proc
//-----------------------------------------------------------------------------

// Returns true iff \a extension is on the end of \a filepath.

/*local proc*/ static bool
endMatchesP(const string& filepath, const string& extension)
{
  typedef string::size_type StrLen;
  const StrLen extensionLength = extension.length();
  const StrLen filepathLength = filepath.length();
  string filepathEnd = filepath.substr(filepathLength - extensionLength);
  if (filepathEnd == extension) {
    return true;
  }
  else return false;
}


//-----------------------------------------------------------------------------
// checkExtension(): local proc
//-----------------------------------------------------------------------------

// Returns true iff \a filepath ends with an extension that indicates
// that it is a FITS file.

/*local proc*/ static bool
checkExtension(const string& filepath)
{
  string loweredFilepath = filepath;
  toLower(loweredFilepath);
  if (endMatchesP(loweredFilepath, ".fits")    or
      endMatchesP(loweredFilepath, ".fits.gz") or
      endMatchesP(loweredFilepath, ".fit")     or
      endMatchesP(loweredFilepath, ".fit.gz")  or
      endMatchesP(loweredFilepath, ".fits.Z")  or
      endMatchesP(loweredFilepath, ".fit.Z"))
    {
      return true;
    }
  else return false;

  // TODO: Extend to deal with FITS bracket filtering, etc.
}


//-----------------------------------------------------------------------------
// getAllFitsErrorMessages(): local proc
//-----------------------------------------------------------------------------

/*local proc*/ string
getAllFitsErrorMessages(const int status)
{
  string retval;
  char fitsError[FLEN_ERRMSG];
//   while (::fits_read_errmsg(fitsError)) {
//     retval += fitsError;
//   }
  retval = "FITSIO status = ";
  stringstream ss;
  ss << status;
  retval += ss.str();
  retval += ": ";
  ::fits_get_errstatus(status, fitsError);
  retval += fitsError;
  return retval;
}


//-----------------------------------------------------------------------------
// square() local inline proc
//-----------------------------------------------------------------------------

/*local proc*/ inline double
square(double x)
{
  return x * x;
}
