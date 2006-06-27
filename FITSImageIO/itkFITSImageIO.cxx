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
#include "itkFITSImageIO.h"
#include "itkExceptionObject.h"
#include "itkByteSwapper.h"
#include "itkMetaDataObject.h"
#include <iostream>
#include <list>
#include <string>
#include <math.h>
#include <zlib.h>

#include "fitsio.h"
#include <itksys/SystemTools.hxx>

namespace itk
{


/** Constructor */
FITSImageIO::FITSImageIO()
{
}


/** Destructor */
FITSImageIO::~FITSImageIO()
{
}


bool FITSImageIO::CanReadFile( const char* filename ) 
{ 
  // First check the filename extension
  bool extensionFound = CheckExtension(filename);

  if( !extensionFound )
    {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
    }

  // TODO: add here further verfication if cfits provides it...

  return true;
}
  


bool FITSImageIO::CanWriteFile( const char * name )
{
  std::string filename = name;
  if ( filename == "" )
    {
    itkDebugMacro(<< "No filename specified.");
    }

  bool extensionFound = CheckExtension(name);

  if( !extensionFound )
    {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
    }

  if( extensionFound )
    {
    return true;
    }
  return false;

}


 
void FITSImageIO::Read(void* buffer)
{
   // here is the fun part...
   //
   // TODO: get the buffer here...
   //

}

/** 
 *  Read Information about the Gipl file
 *  and put the cursor of the stream just before the first data pixel
 */
void FITSImageIO::ReadImageInformation()
{
   // Delegate to cfits and populate the MetaDataDictionary.
   // page 4  Quick Start Guide (how to get the list of tags in the header)
   // page 34 Cfits IO User's guide.
   //
   
   typedef std::string defaultValueType;

   // This is the safe but slow way of opening the file...
   // reconsider this for compressed files...
   fitsfile * fptr;


   int status = 0;
   int numberOfHeaderEntries = 0;

   // Open the file
   fits_open_file( &fptr, m_FileName.c_str(), READONLY, &status );

   // Get the header
   fits_get_hdrspace( fptr, &numberOfHeaderEntries, NULL, &status );

   const unsigned int maxRecordLength = 1024; // check this number by looking at cfits code...

   char charKey[ maxRecordLength ];
   char charValue[ maxRecordLength ];
   char * dontNeedTheComment = NULL;

   // Get the records and populate the MetaDataDictionary 
   MetaDataDictionary & thisDic=this->GetMetaDataDictionary();

   for(unsigned int keyentry=0; keyentry < numberOfHeaderEntries; keyentry++)
      { 
      unsigned int keyentryFortran = keyentry+1;

      // Get the record from cfits
      fits_read_keyn( fptr, keyentry, charKey, charValue, dontNeedTheComment, &status );

     if( status  != 0 )
        {
        break;  // TODO: report an error
        }
 
      std::string stringKey   = charKey;
      std::string stringValue = charValue;

      // Put the record in the MetaDataDictionary
      EncapsulateMetaData<defaultValueType>( thisDic, stringKey, stringValue );

      }

   fits_close_file( fptr, &status );

   // TODO: Gather : Dimension, NumberOfPixels, Spacing and Origin 

   // TODO: Convert status to a meaningful string and use it to throw and exception....

}


void 
FITSImageIO
::WriteImageInformation(void)
{
  // TODO: Implement writing by invoking cfits
}



/** The write function is not implemented */
void 
FITSImageIO
::Write( const void* buffer) 
{
}

/** Print Self Method */
void FITSImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "PixelType " << m_PixelType << "\n";
}


bool FITSImageIO::CheckExtension(const char* filename)
{

  if ( filename == NULL  )
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }

  std::string fname = filename;

  if ( fname.empty()  )
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }

  bool extensionValid = false;

  std::string extension = itksys::SystemTools::GetFilenameExtension( filename );

  if( extension == "fits" )
    {
    extensionValid = true;
    }

  if( extension == "fit" )
    {
    extensionValid = true;
    }

  //  TODO: add gz ... combinations

  return extensionValid;
}



} // end namespace itk
