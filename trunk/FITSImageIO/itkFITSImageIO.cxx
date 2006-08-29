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

#include "itkFITSImageIO.h"
#include <grparser.h> // for FITS NGP_MAX_ARRAY_DIM

using std::string;
using std::stringstream;
using std::ostream;
using std::cerr;
using std::endl;

// Forward declarations:
static bool checkExtension(const string&);
static string getAllFitsErrorMessages(int status);

namespace itk {

//-----------------------------------------------------------------------------
// CanReadFile(): virtual method of FITSImageIO
//-----------------------------------------------------------------------------

/*method*/ bool
FITSImageIO::CanReadFile(const char* filepath) 
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
//-----------------------------------------------------------------------------

/*method*/ bool
FITSImageIO::CanWriteFile(const char* const name)
{
  return false;

  // TODO 

  string filepath = name;
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
//-----------------------------------------------------------------------------

//! Read information about the FITS file and put the cursor of the
//! stream just before the first data pixel

/*method*/ void
FITSImageIO::ReadImageInformation()
{
  int status = 0;

  // Open the FITS file:
  ::fits_open_file(&m_fitsFile, this->GetFileName(), READONLY, &status);
  if (status) {
    itkExceptionMacro("FITSImageIO could not open FITS file: \""
                      << this->GetFileName() << " for reading: "
                      << ::getAllFitsErrorMessages(status));
  }

  // TODO: Delegate to cfits and populate the MetaDataDictionary.
  // page 4  Quick Start Guide (how to get the list of tags in the header)
  // page 34 Cfits IO User's guide.
  //
   
  // TODO:
  // typedef string defaultValueType;

  // TODO: We may want to open the FITS file in the ctor rather than
  // in each method, since if we end up opening a compressed file
  // multiple times, in different methods, the file will end up being
  // uncompressed multiple times.

  cerr << "DEBUG: Entering FITSImageIO::ReadImageInformation()." << endl; //d

  // Get the dimensions and type of the image:
  int numOfAxes;
  long dimOfAxis[NGP_MAX_ARRAY_DIM];
  int bitsPerPixel;
  ::fits_get_img_param(m_fitsFile, NGP_MAX_ARRAY_DIM, &bitsPerPixel,
                       &numOfAxes, dimOfAxis, &status);

  cerr << "NAXIS=" << numOfAxes << endl; //d
  cerr << "DIMS="; //d
  for (long i = 0; i < numOfAxes; ++i) {  //d
    cerr << " " << dimOfAxis[i]; //d
  }
  cerr << endl;
  cerr << "BPP= " << bitsPerPixel << endl;

  // Set up the ITK image:
  this->SetNumberOfComponents(1);
  this->SetPixelType(SCALAR);
  this->SetComponentType(FLOAT);
  this->SetNumberOfDimensions(numOfAxes);
  for (int i = 0; i < numOfAxes; ++i) {
    this->SetDimensions(i, dimOfAxis[i]);
    this->SetOrigin(i, 0);
    this->SetSpacing(i, 1.0);
  }


  // TODO: Make the commented-out code below work properly:

//    // Get the header
      int numberOfHeaderEntries = 0;
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
//-----------------------------------------------------------------------------

/*method*/ void
FITSImageIO::Read(void* const buffer)
{
  // TODO: We probably want to preserve the native pixel type, rather
  // than converting everything into a float.

  float* bufferAsFloats = static_cast<float*>(buffer);

  // Make a pixel location array that represents the origin pixel; i.e., [1,
  // 1, 1, ...]:
  const int nDimensions = this->GetNumberOfDimensions();
  long origin[nDimensions];
  for (int i = 0; i < nDimensions; ++i) {
    origin[i] = 1;
  }

  // Calculate the total number of pixels in the image:
  long long nPixels = 1;
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
                      << ::getAllFitsErrorMessages(status));
  }
                

  // TODO: Remove this terrible hack.
  // Scale the values by 1,000:
  const float* const lastPixel = bufferAsFloats + nPixels;
  for (float* pixelPtr = bufferAsFloats; pixelPtr < lastPixel; ++pixelPtr) {
    *pixelPtr = 1000 * *pixelPtr;
  }

  // Close the FITS file:
  ::fits_close_file(m_fitsFile, &status);
  if (status) {
    itkExceptionMacro("FITSImageIO::Read() could not close FITS file \""
                      << this->GetFileName()
                      << "\" after reading the primary data array: "
                      << ::getAllFitsErrorMessages(status));
  }
}


//-----------------------------------------------------------------------------
// WriteImageInformation(): virtual method of FITSImageIO
//-----------------------------------------------------------------------------

/*method*/ void 
FITSImageIO::WriteImageInformation(void)
{
  // TODO: Implement writing by invoking cfits
}


//-----------------------------------------------------------------------------
// Write(): virtual method of FITSImageIO
//-----------------------------------------------------------------------------

//! The Write() function is not yet implemented

/*method*/ void 
FITSImageIO::Write(const void* const buffer) 
{
  // TODO
}


//-----------------------------------------------------------------------------
// PrintSelf(): virtual method of FITSImageIO
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

/*proc*/ void
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

/*proc*/ static bool
endMatchesP(const string& filepath, const string& extension)
{
  typedef string::size_type StrLen;
  StrLen extensionLength = extension.length();
  StrLen filepathLength = filepath.length();
  string filepathEnd = filepath.substr(filepathLength - extensionLength);
  if (filepathEnd == extension) {
    return true;
  }
  else return false;
}


//-----------------------------------------------------------------------------
// checkExtension(): local proc
//-----------------------------------------------------------------------------

// Returns true iff filepath ends with an extension that indicates
// that it is a FITS file.

/*proc*/ static bool
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

/*proc*/ string
getAllFitsErrorMessages(int status)
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
