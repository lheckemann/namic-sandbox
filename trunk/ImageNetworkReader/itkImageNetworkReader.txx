/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageNetworkReader.txx,v $
  Language:  C++
  Date:      $Date: 2008-09-16 21:01:10 $
  Version:   $Revision: 1.80 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageNetworkReader_txx
#define __itkImageNetworkReader_txx
#include "itkImageNetworkReader.h"

#include "itkObjectFactory.h"
#include "itkImageIOFactory.h"
#include "itkConvertPixelBuffer.h"
#include "itkImageRegion.h"
#include "itkPixelTraits.h"
#include "itkVectorImage.h"

#include <itksys/SystemTools.hxx>
#include <fstream>

#include <curl/curl.h>


#if defined(ITK_LEAN_AND_MEAN) || defined(__BORLANDC__) || defined(NDEBUG)
#define itkGenericDebugMacro(x)
#else
#define itkGenericDebugMacro(x) \
  { if (::itk::Object::GetGlobalWarningDisplay())   \
    { ::itk::OStringStream itkmsg; \
      itkmsg << "Debug: In " __FILE__ ", line " << __LINE__ << "\n" \
             x  \
             << "\n\n"; \
      ::itk::OutputWindowDisplayDebugText(itkmsg.str().c_str());} \
}
#endif


namespace itk
{

static int HeaderTransferProgress ( void *clientp, double dltotal, double dlnow, double ultotal, double ulnow ) {
  unsigned long int* headersize = (unsigned long int*) clientp;
  itkGenericDebugMacro ( << "Read " << dlnow << " bytes of " << headersize[0] );
  // Always downlod everything if headersize is 0
  if ( headersize[0] == 0 ) { return 0; }
  if ( dlnow > headersize[0] )
    {
    // We've got enough
    itkGenericDebugMacro ( << "Exceeded size, stopping download" );
    return 1;
    }
  return 0;
}



template <class TOutputImage, class ConvertPixelTraits>
ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::ImageNetworkReader()
{
  m_ImageIO = 0;
  m_URI = "";
  m_DataExtention = "";
  m_UserSpecifiedImageIO = false;
  m_HeaderFetchSize = 4096;
  curl_global_init ( CURL_GLOBAL_ALL );
  m_TemporaryDirectory = itksys::SystemTools::GetCurrentWorkingDirectory();
}

template <class TOutputImage, class ConvertPixelTraits>
ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::~ImageNetworkReader()
{
}


template <class TOutputImage, class ConvertPixelTraits>
void ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::CalculateTemporaryFilenames()
{
  // Create the temporary Directory
  if ( m_TemporaryDirectory != "" && !itksys::SystemTools::MakeDirectory ( m_TemporaryDirectory.c_str() ) )
    {
    throw ImageNetworkReaderException(__FILE__, __LINE__, "Failed to create temporary directory", ITK_LOCATION );
    }

  // Get the filename, as whatever is past the trailing slash
  std::string tail;
  std::string::size_type slash_pos = m_URI.find_last_of("/");
  if(slash_pos != std::string::npos)
    {
    tail = m_URI.substr(slash_pos + 1);
    }
  else
    {
    tail = m_URI;
    }
  
  m_CachedHeaderFilename = m_TemporaryDirectory + "/" + tail;
  itkDebugMacro ( << "Filename is: " << tail << " Final path is: " << m_CachedHeaderFilename );
  m_CachedDataFilename = m_CachedHeaderFilename;
  if ( m_DataExtention != "" ) 
    {
    // Substitute the extention
    std::string root = tail;
    std::string::size_type dot_pos;
    dot_pos = tail.find_last_of(".");
    if(dot_pos != std::string::npos)
      {
      root = tail.substr(0, dot_pos);
      }
    m_CachedDataFilename = m_TemporaryDirectory + "/" + root + "." + m_DataExtention;
    root = m_URI;
    dot_pos = m_URI.find_last_of(".");
    if(dot_pos != std::string::npos)
      {
      root = m_URI.substr(0, dot_pos );
      }
    m_DataURI = root + "." + m_DataExtention;
    }
  itkDebugMacro ( << "Header: " << m_URI << " -> " << m_CachedHeaderFilename );
  itkDebugMacro ( << "Data: " << m_DataURI << " -> " << m_CachedDataFilename );
}



template <class TOutputImage, class ConvertPixelTraits>
void ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::CacheHeader()
{
  // Cache the header if the URI is set
  if ( m_URI == "" )
    {
    throw ImageNetworkReaderException(__FILE__, __LINE__, "URI must be specified", ITK_LOCATION);
    }
  this->CalculateTemporaryFilenames();

  if ( m_HeaderCacheTimeStamp.GetMTime() > this->GetMTime() )
    {
    return;
    }
  this->CacheFile ( m_URI, m_CachedHeaderFilename, this->m_HeaderFetchSize );
  m_HeaderCacheTimeStamp.Modified();
  
}

template <class TOutputImage, class ConvertPixelTraits>
void ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::CacheHeaderAndData()
{
  // Cache the header if the URI is set
  if ( m_URI == "" )
    {
    throw ImageNetworkReaderException(__FILE__, __LINE__, "URI must be specified", ITK_LOCATION);
    }
  this->CalculateTemporaryFilenames();

  if ( m_DataCacheTimeStamp.GetMTime() > this->GetMTime() )
    {
    return;
    }
  // Download everything
  this->CacheFile ( m_URI, m_CachedHeaderFilename, 0 );
  if ( m_CachedHeaderFilename != m_CachedDataFilename )
    {
    this->CacheFile ( m_DataURI, m_CachedDataFilename, 0 );
    }
  this->m_HeaderCacheTimeStamp.Modified();
  this->m_DataCacheTimeStamp.Modified();
}


template <class TOutputImage, class ConvertPixelTraits>
void ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::CacheFile ( std::string uri, std::string fn, unsigned long int MaxSize )
{
  // Get Curl started
  CURL* curlHandle = curl_easy_init();
  if ( curlHandle == NULL )
    {
    throw ImageNetworkReaderException(__FILE__, __LINE__, "Failed to initialize CURL", ITK_LOCATION);
    }
  curl_easy_reset ( curlHandle );
  // Start reading 1 meg at most
  curl_easy_setopt ( curlHandle, CURLOPT_PROGRESSFUNCTION, HeaderTransferProgress );
  curl_easy_setopt ( curlHandle, CURLOPT_NOPROGRESS, 0 );
  curl_easy_setopt ( curlHandle, CURLOPT_PROGRESSDATA, &MaxSize );
  curl_easy_setopt ( curlHandle, CURLOPT_WRITEFUNCTION, NULL );
  curl_easy_setopt ( curlHandle, CURLOPT_FOLLOWLOCATION, true);
  curl_easy_setopt ( curlHandle, CURLOPT_URL, uri.c_str() );
  curl_easy_setopt ( curlHandle, CURLOPT_HTTPGET, 1 );
  curl_easy_setopt ( curlHandle, CURLOPT_WRITEFUNCTION, NULL );

  // Open the filename
  FILE* f = fopen ( fn.c_str(), "wb" );
  curl_easy_setopt ( curlHandle, CURLOPT_WRITEDATA, f );
  CURLcode retval = curl_easy_perform ( curlHandle );
  fclose ( f );

  // Data sucessfully read
  if ( retval == CURLE_OK || retval == CURLE_ABORTED_BY_CALLBACK )
    {
    itkDebugMacro ( "CURL success downloading header" );
    }
}



template <class TOutputImage, class ConvertPixelTraits>
void ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  if (m_ImageIO)
    {
    os << indent << "ImageIO: \n";
    m_ImageIO->Print(os, indent.GetNextIndent());
    }
  else
    {
    os << indent << "ImageIO: (null)" << "\n";
    }

  os << indent << "UserSpecifiedImageIO flag: " << m_UserSpecifiedImageIO << "\n";
  os << indent << "m_URI: " << m_URI << "\n";
  os << indent << "m_UseStreaming: " << m_UseStreaming << "\n";
}


template <class TOutputImage, class ConvertPixelTraits>
void 
ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::SetImageIO( ImageIOBase * imageIO)
{
  itkDebugMacro("setting ImageIO to " << imageIO ); 
  if (this->m_ImageIO != imageIO ) 
    {
    this->m_ImageIO = imageIO;
    this->Modified(); 
    } 
  m_UserSpecifiedImageIO = true;
}


template <class TOutputImage, class ConvertPixelTraits>
void 
ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::GenerateOutputInformation(void)
{
  typename TOutputImage::Pointer output = this->GetOutput();

  itkDebugMacro(<<"Reading file for GenerateOutputInformation()" << m_URI);
  
  // Check to see if we can read the file given the name or prefix
  //
  if ( m_URI == "" )
    {
    throw ImageNetworkReaderException(__FILE__, __LINE__, "URI must be specified", ITK_LOCATION);
    }

  // Cache the header
  try
    {
    this->CacheHeader();
    }
  catch(itk::ExceptionObject &err)
    {
    m_ExceptionMessage = err.GetDescription();
    }

  if ( m_UserSpecifiedImageIO == false ) //try creating via factory
    {
    m_ImageIO = ImageIOFactory::CreateImageIO( m_CachedHeaderFilename.c_str(), ImageIOFactory::ReadMode );
    }
  
  if ( m_ImageIO.IsNull() )
    {
    OStringStream msg;
    msg << " Could not create IO object for file "
        << m_CachedHeaderFilename.c_str()
        << " cached from " 
        << m_URI << std::endl;
    if (m_ExceptionMessage.size())
      {
      msg << m_ExceptionMessage;
      }
    else
      {
      msg << "  Tried to create one of the following:" << std::endl;
      std::list<LightObject::Pointer> allobjects = 
        ObjectFactoryBase::CreateAllInstance("itkImageIOBase");
      for(std::list<LightObject::Pointer>::iterator i = allobjects.begin();
          i != allobjects.end(); ++i)
        {
        ImageIOBase* io = dynamic_cast<ImageIOBase*>(i->GetPointer());
        msg << "    " << io->GetNameOfClass() << std::endl; 
        }
      msg << "  You probably failed to set a file suffix, or" << std::endl;
      msg << "    set the suffix to an unsupported type." << std::endl;
      }
    ImageNetworkReaderException e(__FILE__, __LINE__, msg.str().c_str(), ITK_LOCATION);
    throw e;
    return;
    }
  
  // Got to allocate space for the image. Determine the characteristics of
  // the image.
  //
  m_ImageIO->SetFileName(m_CachedHeaderFilename.c_str());
  m_ImageIO->ReadImageInformation();

  SizeType dimSize;
  double spacing[ TOutputImage::ImageDimension ];
  double origin[ TOutputImage::ImageDimension ];
  typename TOutputImage::DirectionType direction;
  std::vector<double> axis;

  for(unsigned int i=0; i<TOutputImage::ImageDimension; i++)
    {
    if ( i < m_ImageIO->GetNumberOfDimensions() )
      {
      dimSize[i] = m_ImageIO->GetDimensions(i);
      spacing[i] = m_ImageIO->GetSpacing(i);
      origin[i]  = m_ImageIO->GetOrigin(i);

      // Please note: direction cosines are stored as columns of the
      // direction matrix
      axis = m_ImageIO->GetDirection(i);
      for (unsigned j=0; j<TOutputImage::ImageDimension; j++)
        {
        if (j < m_ImageIO->GetNumberOfDimensions())
          {
          direction[j][i] = axis[j];
          }
        else
          {
          direction[j][i] = 0.0;
          }
        }
      }
    else
      {
      // Number of dimensions in the output is more than number of dimensions
      // in the ImageIO object (the file).  Use default values for the size,
      // spacing, origin and direction for the final (degenerate) dimensions.
      dimSize[i] = 1;  
      spacing[i] = 1.0;
      origin[i] = 0.0;
      for (unsigned j = 0; j < TOutputImage::ImageDimension; j++)
        {
        if (i == j)
          {
          direction[j][i] = 1.0;
          }
        else
          {
          direction[j][i] = 0.0;
          }
        }
      }
    }
  //
  // TODO: actually figure out what to do about dir cosines in 2d.
  // don't allow degenerate direction cosines
  // This is a pure punt; it will prevent the exception being
  // thrown, but doesn't do the right thing at all -- replacing e.g
  // [0, 0, -1] with [0, 1] doesn't make any real sense.
  // On the other hand, programs that depend on 2D Direction Cosines
  // are pretty much guaranteed to be disappointed if they expect anything
  // meaningful in the direction cosines anyway.
  if(TOutputImage::ImageDimension == 2)
    {
    if(direction[0][0] == 0.0 && direction[1][0] == 0)
      {
      if(direction[0][1] == 0.0)
        {
        direction[0][0] = 1.0;
        }
      if(direction[1][1] == 0.0)
        {
        direction[1][0] = 1.0;
        }
      }
    else if(direction[0][1] == 0.0 && direction[1][1] == 0)
      {
      if(direction[0][0] == 0.0)
        {
        direction[1][0] = 1.0;
        }
      if(direction[1][0] == 0.0)
        {
        direction[1][1] = 1.0;
        }
      }
    }
  output->SetSpacing( spacing );     // Set the image spacing
  output->SetOrigin( origin );       // Set the image origin
  output->SetDirection( direction ); // Set the image direction cosines

  //Copy MetaDataDictionary from instantiated reader to output image.
  output->SetMetaDataDictionary(m_ImageIO->GetMetaDataDictionary());
  this->SetMetaDataDictionary(m_ImageIO->GetMetaDataDictionary());

  IndexType start;
  start.Fill(0);

  ImageRegionType region;
  region.SetSize(dimSize);
  region.SetIndex(start);
 
  // If a VectorImage, this requires us to set the 
  // VectorLength before allocate
  if( strcmp( output->GetNameOfClass(), "VectorImage" ) == 0 ) 
    {
    typedef typename TOutputImage::AccessorFunctorType AccessorFunctorType;
    AccessorFunctorType::SetVectorLength( output, m_ImageIO->GetNumberOfComponents() );
    }
  
  output->SetLargestPossibleRegion(region);
}

template <class TOutputImage, class ConvertPixelTraits>
void
ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::EnlargeOutputRequestedRegion(DataObject *output)
{
  itkDebugMacro (<< "Starting EnlargeOutputRequestedRegion() ");
  typename TOutputImage::Pointer out = dynamic_cast<TOutputImage*>(output);

  // Delegate to the ImageIO the computation of how much the 
  // requested region must be enlarged.

  //
  // The following code converts the ImageRegion (templated over dimension)
  // into an ImageIORegion (not templated over dimension).
  //
  ImageRegionType imageRequestedRegion = out->GetRequestedRegion();

  ImageIORegion ioRequestedRegion( TOutputImage::ImageDimension );

  typedef ImageIORegionAdaptor< TOutputImage::ImageDimension >  ImageIOAdaptor;
  
  ImageIOAdaptor::Convert( imageRequestedRegion, ioRequestedRegion );

  // Tell the IO if we should use streaming while reading
  m_ImageIO->SetUseStreamedReading(m_UseStreaming);

  ImageIORegion ioStreamableRegion  = 
    m_ImageIO->GenerateStreamableReadRegionFromRequestedRegion( ioRequestedRegion );


  ImageIOAdaptor::Convert( ioStreamableRegion, this->m_StreamableRegion );

  //
  // Check whether the imageRequestedRegion is fully contained inside the
  // streamable region or not.
  if( !this->m_StreamableRegion.IsInside( imageRequestedRegion ) )
    {
    itkExceptionMacro(
      << "ImageIO returns IO region that does not fully contain the requested region"
      << "Requested region: " << imageRequestedRegion 
      << "StreamableRegion region: " << this->m_StreamableRegion);
    }
    
  itkDebugMacro (<< "StreamableRegion set to =" << this->m_StreamableRegion );
}


template <class TOutputImage, class ConvertPixelTraits>
void ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::GenerateData()
{
  typename TOutputImage::Pointer output = this->GetOutput();

  itkDebugMacro ( << "ImageNetworkReader::GenerateData() \n" 
     << "Allocating the buffer with the StreamableRegion \n" 
     << this->m_StreamableRegion << "\n");

  output->SetBufferedRegion( this->m_StreamableRegion );
  output->Allocate();

  // Cache header and data
  try
    {
    this->CacheHeaderAndData();
    }
  catch(itk::ExceptionObject &err)
    {
    m_ExceptionMessage = err.GetDescription();
    }

  // Tell the ImageIO to read the file
  //
  OutputImagePixelType *buffer = output->GetPixelContainer()->GetBufferPointer();

  m_ImageIO->SetFileName(m_CachedHeaderFilename.c_str());

  ImageIORegion ioRegion(TOutputImage::ImageDimension);

  typedef ImageIORegionAdaptor< TOutputImage::ImageDimension >  ImageIOAdaptor;
  
  // Convert the m_StreamableRegion from ImageRegion type to ImageIORegion type
  ImageIOAdaptor::Convert( this->m_StreamableRegion, ioRegion );

  itkDebugMacro (<< "ioRegion: " << ioRegion);
 
  m_ImageIO->SetIORegion( ioRegion );

  if ( m_ImageIO->GetComponentTypeInfo()
       == typeid(ITK_TYPENAME ConvertPixelTraits::ComponentType)
       && (m_ImageIO->GetNumberOfComponents()
           == ConvertPixelTraits::GetNumberOfComponents()))
    {
    itkDebugMacro(<< "No buffer conversion required.");
    // allocate a buffer and have the ImageIO read directly into it
    m_ImageIO->Read(buffer);
    return;
    }
  else // a type conversion is necessary
    {
    itkDebugMacro(<< "Buffer conversion required.");
    // note: char is used here because the buffer is read in bytes
    // regardles of the actual type of the pixels.
    ImageRegionType region = output->GetBufferedRegion();
    std::vector<char> loadBuffer(m_ImageIO->GetImageSizeInBytes());

    m_ImageIO->Read(static_cast<void *>(&loadBuffer[0]));
    
    itkDebugMacro(<< "Buffer conversion required from: "
                  << m_ImageIO->GetComponentTypeInfo().name()
                  << " to: "
                  << typeid(ITK_TYPENAME ConvertPixelTraits::ComponentType).name());

    this->DoConvertBuffer(static_cast<void *>(&loadBuffer[0]), region.GetNumberOfPixels());
    }
}

template <class TOutputImage, class ConvertPixelTraits>
void 
ImageNetworkReader<TOutputImage, ConvertPixelTraits>
::DoConvertBuffer(void* inputData,
                  unsigned long numberOfPixels)
{
  // get the pointer to the destination buffer
  OutputImagePixelType *outputData =
    this->GetOutput()->GetPixelContainer()->GetBufferPointer();


  // TODO:
  // Pass down the PixelType (RGB, VECTOR, etc.) so that any vector to
  // scalar conversion be type specific. i.e. RGB to scalar would use
  // a formula to convert to luminance, VECTOR to scalar would use
  // vector magnitude.
  
  
  // Create a macro as this code is a bit lengthy and repetitive
  // if the ImageIO pixel type is typeid(type) then use the ConvertPixelBuffer
  // class to convert the data block to TOutputImage's pixel type
  // see DefaultConvertPixelTraits and ConvertPixelBuffer

  // The first else if block applies only to images of type itk::VectorImage  
  // VectorImage needs to copy out the buffer differently.. The buffer is of
  // type InternalPixelType, but each pixel is really 'k' consecutive pixels.

#define ITK_CONVERT_BUFFER_IF_BLOCK(type)               \
 else if( m_ImageIO->GetComponentTypeInfo() == typeid(type) )   \
   {                                                   \
   if( strcmp( this->GetOutput()->GetNameOfClass(), "VectorImage" ) == 0 ) \
     { \
     ConvertPixelBuffer<                                 \
      type,                                             \
      OutputImagePixelType,                             \
      ConvertPixelTraits                                \
      >                                                 \
      ::ConvertVectorImage(                             \
        static_cast<type*>(inputData),                  \
        m_ImageIO->GetNumberOfComponents(),             \
        outputData,                                     \
        static_cast<int>(numberOfPixels));              \
     } \
   else \
     { \
     ConvertPixelBuffer<                                 \
      type,                                             \
      OutputImagePixelType,                             \
      ConvertPixelTraits                                \
      >                                                 \
      ::Convert(                                        \
        static_cast<type*>(inputData),                  \
        m_ImageIO->GetNumberOfComponents(),             \
        outputData,                                     \
        static_cast<int>(numberOfPixels));              \
      } \
    }
  if(0)
    {
    }
  ITK_CONVERT_BUFFER_IF_BLOCK(unsigned char)
  ITK_CONVERT_BUFFER_IF_BLOCK(char)
  ITK_CONVERT_BUFFER_IF_BLOCK(unsigned short)
  ITK_CONVERT_BUFFER_IF_BLOCK( short)
  ITK_CONVERT_BUFFER_IF_BLOCK(unsigned int)
  ITK_CONVERT_BUFFER_IF_BLOCK( int)
  ITK_CONVERT_BUFFER_IF_BLOCK(unsigned long)
  ITK_CONVERT_BUFFER_IF_BLOCK( long)
  ITK_CONVERT_BUFFER_IF_BLOCK(float)
  ITK_CONVERT_BUFFER_IF_BLOCK( double)
  else
    {
    ImageNetworkReaderException e(__FILE__, __LINE__);
    OStringStream msg;
    msg <<"Couldn't convert component type: "
        << std::endl << "    "
        << m_ImageIO->GetComponentTypeAsString(m_ImageIO->GetComponentType())
        << std::endl << "to one of: "
        << std::endl << "    " << typeid(unsigned char).name()
        << std::endl << "    " << typeid(char).name()
        << std::endl << "    " << typeid(unsigned short).name()
        << std::endl << "    " << typeid(short).name()
        << std::endl << "    " << typeid(unsigned int).name()
        << std::endl << "    " << typeid(int).name()
        << std::endl << "    " << typeid(unsigned long).name()
        << std::endl << "    " << typeid(long).name()
        << std::endl << "    " << typeid(float).name()
        << std::endl << "    " << typeid(double).name()
        << std::endl;
    e.SetDescription(msg.str().c_str());
    e.SetLocation(ITK_LOCATION);
    throw e;
    return;
    }
#undef ITK_CONVERT_BUFFER_IF_BLOCK
}


} //namespace ITK

#endif
