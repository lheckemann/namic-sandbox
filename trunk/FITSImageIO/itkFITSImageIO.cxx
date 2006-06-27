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
#include <iostream>
#include <list>
#include <string>
#include <math.h>
#include <zlib.h>

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

  // add here further verfication if cfits provides it...

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
   fits_open_file( &fptr, m_FileName, READONLY, &status );

   // Get the header
   fits_get_hdrspace( fptr, &numberOfHeaderEntries, NULL, &status );

   const unsigned int maxRecordLength = 1024; // check this number by looking at cfits code...

   char charKey[ maxRecordLength ];
   char charValue[ maxRecordLength ];
   const char * dontNeedTheComment = NULL;

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
  //not possible to write a Gipl file  
}



/** The write function is not implemented */
void 
FITSImageIO
::Write( const void* buffer) 
{
  CheckExtension(m_FileName.c_str());

  unsigned int nDims = this->GetNumberOfDimensions();

#ifdef __sgi
  // Create the file. This is required on some older sgi's
  std::ofstream tFile(m_FileName.c_str(),std::ios::out);
  tFile.close();                    
#endif

  if (m_IsCompressed)
    {
    m_Internal->m_GzFile = ::gzopen( m_FileName.c_str(), "wb" );
    if( m_Internal->m_GzFile == NULL )
       {
       ExceptionObject exception(__FILE__, __LINE__);
       exception.SetDescription("File cannot be write");
       throw exception;
      }
    }
  else
    {
    m_Ofstream.open(m_FileName.c_str(), std::ios::binary | std::ios::out);
    if( m_Ofstream.fail() )
      {
      ExceptionObject exception(__FILE__, __LINE__);
      exception.SetDescription("File cannot be write");
      throw exception;
      }
    }
 

  unsigned int i;
  for(i=0;i<4;i++)
    {
    unsigned short value;
    if(i<nDims)
      {
      value = this->GetDimensions(i);
      if(m_ByteOrder == BigEndian)
        {
        ByteSwapper<unsigned short>::SwapFromSystemToBigEndian(&value);
        }
      else if (m_ByteOrder == LittleEndian)
        {
        ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&value);
        }
 
      if (m_IsCompressed)
        {
       ::gzwrite( m_Internal->m_GzFile,(char*)&(value),sizeof(unsigned short));
        }
      else
        {
        m_Ofstream.write((char*)&(value),sizeof(unsigned short));
        }
      }
    else
      {
      value = 0;
      if(m_ByteOrder == BigEndian)
        {
        ByteSwapper<unsigned short>::SwapFromSystemToBigEndian(&value);
        }
      else if (m_ByteOrder == LittleEndian)
        {
        ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&value);
        }
       if (m_IsCompressed)
        {
        ::gzwrite( m_Internal->m_GzFile,(char*)&(value),sizeof(unsigned short));
        }
      else
        {
        m_Ofstream.write((char*)&value,sizeof(unsigned short));;
        }
      }
    }


  unsigned short   image_type;
  switch(m_ComponentType)
    {
    case  CHAR : image_type = GIPL_CHAR;break;
    case  UCHAR :  image_type = GIPL_U_CHAR;break;
    case  SHORT :  image_type = GIPL_SHORT;break;
    case  USHORT :  image_type = GIPL_U_SHORT;break;
    case  UINT :  image_type = GIPL_U_INT;break;
    case  INT :  image_type = GIPL_INT;break;
    case  FLOAT :  image_type = GIPL_FLOAT;break;
    case  DOUBLE : image_type = GIPL_DOUBLE;break;
    default:
      itkExceptionMacro ("Invalid type: " << m_ComponentType );
    }

  if(m_ByteOrder == BigEndian)
    {
    ByteSwapper<unsigned short>::SwapFromSystemToBigEndian((unsigned short*)&image_type);
    }
  if(m_ByteOrder == LittleEndian)
    {
    ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian((unsigned short *)&image_type);
    }
 
  if (m_IsCompressed)
    {
   ::gzwrite( m_Internal->m_GzFile,(char*)&image_type,sizeof(unsigned short));
    }
  else
    {
    m_Ofstream.write((char*)&image_type,sizeof(unsigned short));
    }

  /*   10   16  X,Y,Z,T pixel dimensions mm */
  for(i=0;i<4;i++)
    {
    if(i<nDims)
      {
      float value = m_Spacing[i];
      if(m_ByteOrder == BigEndian)
        {
        ByteSwapper<float>::SwapFromSystemToBigEndian((float *)&value);
        }
      if(m_ByteOrder == LittleEndian)
        {
        ByteSwapper<float>::SwapFromSystemToLittleEndian((float *)&value);
        }
      if (m_IsCompressed)
        {
        ::gzwrite( m_Internal->m_GzFile,(char*)&value,sizeof(float));
        }
      else
        {
        m_Ofstream.write((char*)&value,sizeof(float));
        }
      }
    else
      {
      float value = 0;
      if(m_ByteOrder == BigEndian)
        {
        ByteSwapper<float>::SwapFromSystemToBigEndian((float*)&value);
        }
      if(m_ByteOrder == LittleEndian)
        {
        ByteSwapper<float>::SwapFromSystemToLittleEndian((float *)&value);
        }
      if (m_IsCompressed)
        {
        ::gzwrite( m_Internal->m_GzFile,(char*)&value,sizeof(float));
        }
      else
        {
        m_Ofstream.write((char*)&value,sizeof(float));
        }
      }
    }

  char    line1[80];         /*   26   80  Patient / Text field        */

  for(i=0;i<80;i++)
    {
    line1[i]=0; //initialize
    }

  sprintf(line1,"No Patient Information");
  for(i=0;i<80;i++)
    {
    if (m_IsCompressed)
      {
      ::gzwrite( m_Internal->m_GzFile,(char*)&line1[i],sizeof(char));
      }
    else
      {
      m_Ofstream.write((char*)&line1[i],sizeof(char));
      }
    }

  float   matrix[20];        /*  106   80                              */
  for(i=0;i<20;i++)
    {
    matrix[i]=0; //write zeros
    if (m_IsCompressed)
      {
      ::gzwrite( m_Internal->m_GzFile,(char*)&matrix[i],sizeof(float));
      }
    else
      {
      m_Ofstream.write((char*)&matrix[i],sizeof(float));
      }
    }

  char    flag1=0;             /*  186    1  Orientation flag (below)    */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&flag1,sizeof(char));
    }
  else
    {
    m_Ofstream.write((char*)&flag1,sizeof(char));
    }

  char    flag2=0;             /*  187    1                              */
  if (m_IsCompressed)
    {
   ::gzwrite( m_Internal->m_GzFile,(char*)&flag2,sizeof(char));
    }
  else
    {
    m_Ofstream.write((char*)&flag2,sizeof(char));
    }
   
  double  min=0;               /*  188    8  Minimum voxel value         */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&min,sizeof(double));
    }
  else
    {
    m_Ofstream.write((char*)&min,sizeof(double));
    }
 
  double  max=0;               /*  196    8  Maximum voxel value         */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&max,sizeof(double));
    }
  else
    {
    m_Ofstream.write((char*)&max,sizeof(double));
    }
        
  double  origin[4];         /*  204   32  X,Y,Z,T offset              */
  for(i=0;i<4;i++)
    {
    
    if(i<nDims)
      {
       origin[i] = m_Origin[i];
      }
    else
      {
       origin[i] = 0;
      }
    
       
    if(m_ByteOrder == BigEndian)
        {
        ByteSwapper<double>::SwapFromSystemToBigEndian((double*)&origin[i]);
        }
      if(m_ByteOrder == LittleEndian)
        {
        ByteSwapper<double>::SwapFromSystemToLittleEndian((double *)&origin[i]);
        }
    
    
    if (m_IsCompressed)
      {
      ::gzwrite( m_Internal->m_GzFile,(char*)&origin[i],sizeof(double));
      }
    else
      {
      m_Ofstream.write((char*)&origin[i],sizeof(double));
      }
    }

  float   pixval_offset=0;     /*  236    4                            */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&pixval_offset,sizeof(float));
    }
  else
    {
    m_Ofstream.write((char*)&pixval_offset,sizeof(float));
    }

  float   pixval_cal=0;        /*  240    4                              */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&pixval_cal,sizeof(float));
    }
  else
    {
    m_Ofstream.write((char*)&pixval_cal,sizeof(float));
    }
  
  float   user_def1=0;         /*  244    4  Inter-slice Gap             */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&user_def1,sizeof(float));
    }
  else
    {
    m_Ofstream.write((char*)&user_def1,sizeof(float));
    }
       
  float   user_def2=0;         /*  248    4  User defined field          */
  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&user_def2,sizeof(float));
    }
  else
    {
    m_Ofstream.write((char*)&user_def2,sizeof(float));
    }
     
  unsigned int magic_number = GIPL_MAGIC_NUMBER; /*  252    4 Magic Number                 */
  if(m_ByteOrder == BigEndian)
    {
    ByteSwapper<unsigned int>::SwapFromSystemToBigEndian(&magic_number);
    }
  if(m_ByteOrder == LittleEndian)
    {
    ByteSwapper<unsigned int>::SwapFromSystemToLittleEndian(&magic_number);
    }

  if (m_IsCompressed)
    {
    ::gzwrite( m_Internal->m_GzFile,(char*)&magic_number,sizeof(unsigned int));
    }
  else
    {
    m_Ofstream.write((char*)&magic_number,sizeof(unsigned int));
    }


  // Actually do the writing
  //
  this->ComputeStrides();
  if ( m_FileType == ASCII )
    {
    this->WriteBufferAsASCII(m_Ofstream, buffer, this->GetComponentType(),
                             this->GetImageSizeInComponents());
    }
  else //binary
    {
    const unsigned long numberOfBytes      = this->GetImageSizeInBytes();
    const unsigned long numberOfComponents = this->GetImageSizeInComponents();

    // Swap bytes if necessary
    if ( m_ByteOrder == LittleEndian )
      {
      char * tempBuffer = new char[ numberOfBytes ];
      memcpy( tempBuffer, buffer , numberOfBytes );
      SwapBytesIfNecessary(tempBuffer, numberOfComponents );
      if (m_IsCompressed)
        {
        ::gzwrite( m_Internal->m_GzFile,tempBuffer, numberOfBytes );
        }
      else
        {
        m_Ofstream.write( tempBuffer, numberOfBytes );
        }
      delete [] tempBuffer;
      }
    else if ( m_ByteOrder == BigEndian )
      {
      char * tempBuffer = new char[ numberOfBytes ];
      memcpy( tempBuffer, buffer , numberOfBytes );
      SwapBytesIfNecessary(tempBuffer, numberOfComponents );
      if (m_IsCompressed)
        {
        ::gzwrite( m_Internal->m_GzFile,tempBuffer,numberOfBytes);
        }
      else
        {
        m_Ofstream.write( tempBuffer, numberOfBytes );
        }
      delete [] tempBuffer;
      }
    else
      {
      if (m_IsCompressed)
        {
        ::gzwrite( m_Internal->m_GzFile,const_cast<void *>(buffer), numberOfBytes);
        }
      else
        {
         m_Ofstream.write(static_cast<const char*>(buffer), numberOfBytes );
        }
      }
    }

  if (m_IsCompressed)
    {
    ::gzclose(m_Internal->m_GzFile);
    m_Internal->m_GzFile = NULL;
    }
  else
    {
    m_Ofstream.close();
    }
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

  std::string extension = itksys::SystemTools::GetExtension( filename );

  if( extension == "fits" )
    {
    extensionValid = true;
    }

  if( extension == "fit" )
    {
    extensionValid = true;
    }

  //  add gz ... combinations

  return extensionValid;
}



} // end namespace itk
