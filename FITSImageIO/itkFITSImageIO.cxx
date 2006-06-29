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
#include <math.h>
#include <zlib.h>

#include <itkExceptionObject.h>
#include <itkByteSwapper.h>
#include <itkMetaDataObject.h>
#include <itksys/SystemTools.hxx>
#include <fitsio.h>

#include "itkFITSImageIO.h"

using std::string;
using std::ostream;
using std::cerr;
using std::endl;

// Forward declarations:
static bool checkExtension(const string&);


namespace itk {

//-----------------------------------------------------------------------------
// CanReadFile(): virtual method of FITSImageIO
//-----------------------------------------------------------------------------

/*method*/ bool
FITSImageIO::CanReadFile(const char* filepath) 
{ 
  if (!filepath || *filepath == 0) {
    itkDebugMacro(<< "No filename specified.");
    return false;
  } else if (::checkExtension(filepath)) {
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
// Read(): virtual method of FITSImageIO
//-----------------------------------------------------------------------------

/*method*/ void
FITSImageIO::Read(void* const buffer)
{
   // here is the fun part...
   //
   // TODO: get the buffer here...
}


//-----------------------------------------------------------------------------
// ReadImageInformation(): virtual method of FITSImageIO
//-----------------------------------------------------------------------------

//! Read information about the FITS file and put the cursor of the
//! stream just before the first data pixel

/*method*/ void
FITSImageIO::ReadImageInformation()
{
   // TODO: Delegate to cfits and populate the MetaDataDictionary.
   // page 4  Quick Start Guide (how to get the list of tags in the header)
   // page 34 Cfits IO User's guide.
   //
   
   typedef string defaultValueType;

   // TODO: We may want to open the FITS file in the ctor rather than
   // in each method, since if we end up opening a compressed file
   // multiple times, in different methods, the file will end up being
   // uncompressed multiple times.

   fitsfile* fptr;
   int status = 0;
   int naxis;

   ::fits_get_img_dim(fptr, &naxis, &status);
   
   cerr << "NAXIS=" << naxis << endl; //d

   // Open the file
   ::fits_open_file(&fptr, m_FileName.c_str(), READONLY, &status);

   // TODO: Gather : Dimension, NumberOfPixels, Spacing and Origin 

   // TODO: Convert status to a meaningful string and use it to throw
   // an exception....


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

   ::fits_close_file(fptr, &status);
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
  if (endMatchesP(loweredFilepath, ".fits") ||
      endMatchesP(loweredFilepath, ".fits.gz") ||
      endMatchesP(loweredFilepath, ".fit") ||
      endMatchesP(loweredFilepath, ".fit.gz") ||
      endMatchesP(loweredFilepath, ".fits.Z") ||
      endMatchesP(loweredFilepath, ".fit.Z"))
    {
      return true;
    }
  else return false;

  // TODO: Extend to deal with FITS bracket filtering, etc.
}



