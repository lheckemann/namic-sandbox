/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkAnalyzeObjectImageIO.cxx,v $
  Language:  C++
  Date:      $Date: 2007/04/02 18:37:40 $
  Version:   $Revision: 1.20 $  

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkAnalyzeObjectImageIO.h"
#include "itkExceptionObject.h"

namespace itk
{

/** Constructor */
AnalyzeObjectImageIO::AnalyzeObjectImageIO()
{

}


/** Destructor */
AnalyzeObjectImageIO::~AnalyzeObjectImageIO()
{
}


bool AnalyzeObjectImageIO::CanReadFile( const char* filename ) 
{ 
  // First check the filename extension
  std::string fname = filename;
  if ( fname == "" )
    {
    itkDebugMacro(<< "No filename specified.");
    }

  bool extensionFound = false;
  std::string::size_type extensionpos = fname.rfind(".obj");
  if ((extensionpos != std::string::npos)
      && (extensionpos == fname.length() - 4))
    {
    extensionFound = true;
    }

  if( !extensionFound )
    {
    itkDebugMacro(<<"The filename extension is not recognized");
    return false;
    }

  // Hans: to insert code:


  return true;

}
  
bool AnalyzeObjectImageIO::CanWriteFile( const char * name )
{
  std::string filename = name;
  if ( filename == "" )
    {
    itkDebugMacro(<< "No filename specified.");
    }

  bool extensionFound = false;
  std::string::size_type extensionpos = filename.rfind(".obj");
  if ((extensionpos != std::string::npos)
      && (extensionpos == filename.length() - 4))
    {
    extensionFound = true;
    }
  
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
 
void AnalyzeObjectImageIO::Read(void* buffer)
{
  // Hans: to insert code here:
}

/** 
 *  Read Information about the BMP file
 *  and put the cursor of the stream just before the first data pixel
 */
void AnalyzeObjectImageIO::ReadImageInformation()
{
  // Hans: to insert code here:
}



void 
AnalyzeObjectImageIO
::WriteImageInformation(void)
{
  
}


/** The write function is not implemented */
void 
AnalyzeObjectImageIO
::Write( const void* buffer) 
{
 
  unsigned int nDims = this->GetNumberOfDimensions();

  if( nDims < 2 || nDims > 4 ) 
    {
    itkExceptionMacro(<< "AnalyzeObjectImageIO cannot write images with a dimension other than  2,3 or 4");
    }

  if(this->GetComponentType() != UCHAR)
    {
    itkExceptionMacro(<<"AnalyzeObjectImageIO supports unsigned char only");
    }
  if( (this->m_NumberOfComponents != 1)  )
    {
    itkExceptionMacro(<<"AnalyzeObjectImageIO supports 1 components only");
    }
  

}

/** Print Self Method */
void AnalyzeObjectImageIO::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace itk
