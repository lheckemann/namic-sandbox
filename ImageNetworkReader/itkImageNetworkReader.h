/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageNetworkReader.h,v $
  Language:  C++
  Date:      $Date: 2007-09-06 18:31:36 $
  Version:   $Revision: 1.31 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageNetworkReader_h
#define __itkImageNetworkReader_h

#include "itkImageIOBase.h"
#include "itkImageSource.h"
#include "itkExceptionObject.h"
#include "itkSize.h"
#include "itkImageRegion.h"
#include "itkDefaultConvertPixelTraits.h"

namespace itk
{

/** \class ImageNetworkReaderException
 *
 * \brief Base exception class for network IO.
 */
class ImageNetworkReaderException : public ExceptionObject 
{
public:
  /** Run-time information. */
  itkTypeMacro( ImageNetworkReaderException, ExceptionObject );

  /** Constructor. */
  ImageNetworkReaderException(const char *file, unsigned int line, 
                           const char* message = "Error in IO",
                           const char* loc = "Unknown") :
    ExceptionObject(file, line, message, loc)
  {
  }

  /** Constructor. */
  ImageNetworkReaderException(const std::string &file, unsigned int line, 
                           const char* message = "Error in IO",
                           const char* loc = "Unknown") : 
    ExceptionObject(file, line, message, loc)
  {
  }
};


/** \brief Data source that reads image data over a network.
 *
 * This source object uses CURL to download images over a
 * network, then uses the standard image IO factories to
 * read the image from a cache file.  The ImageIOBase
 * classes serve the same function as in ImageFileReader.
 *
 * The URI points to the image to be read.  Implementation
 * of the GenerateOutputInformation call requires caching the
 * image header.  By default, 4K of the header is downloaded,
 * but the size may be adjusted through Set/GetHeaderFetchSize.
 * For file formats with two components, i.e.  NRRD and Analyze,
 * the extention of the extra file may be specified through
 * Set/GetDataExtention.  Files are cached in the current directory,
 * or the Set/GetTemporaryDirectory.
 *
 * NB: the URI is assumed to end with the extention
 * of the image type, as the image file readers often check
 * file extentions when implementing CanReadFile.
 *
 * \sa ImageFileReader
 * \sa ImageIOBase
 *
 * \ingroup IOFilters
 *
 */
/** \class ImageNetworkReader
 */
template <class TOutputImage,
          class ConvertPixelTraits=DefaultConvertPixelTraits< 
                   ITK_TYPENAME TOutputImage::IOPixelType > >
class ITK_EXPORT ImageNetworkReader : public ImageSource<TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ImageNetworkReader           Self;
  typedef ImageSource<TOutputImage> Superclass;
  typedef SmartPointer<Self>        Pointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageNetworkReader, ImageSource);

  /** The size of the output image. */
  typedef typename TOutputImage::SizeType  SizeType;

  /** The size of the output image. */
  typedef typename TOutputImage::IndexType  IndexType;

  /** The region of the output image. */
  typedef typename TOutputImage::RegionType  ImageRegionType;

  /** The pixel type of the output image. */
  typedef typename TOutputImage::InternalPixelType OutputImagePixelType;
  
  /** Specify the URI to read. This is forwarded to the IO instance. */
  itkSetStringMacro(URI);
  itkGetStringMacro(URI);
  
  /** Specify the temporary directory for downloaded files.
    * NB: if not specified, will create temp files in the current directory. */
  itkSetStringMacro(TemporaryDirectory);
  itkGetStringMacro(TemporaryDirectory);
  
  /** Extention of the data for the URI.
    * set to blank ("") for no separate data file */
  itkSetStringMacro(DataExtention);
  itkGetStringMacro(DataExtention);
  
  /** Set/Get the ImageIO helper class. Often this is created via the object
   * factory mechanism that determines whether a particular ImageIO can
   * read a certain file. This method provides a way to get the ImageIO 
   * instance that is created. Or you can directly specify the ImageIO
   * to use to read a particular file in case the factory mechanism will
   * not work properly (e.g., unknown or unusual extension). */
  void  SetImageIO( ImageIOBase * imageIO );
  itkGetObjectMacro(ImageIO,ImageIOBase);

  /** Set/Get how much of the header we fetch to get the image information.
   * NB: the header is downloaded twice, but is usually small.
   */
  itkSetMacro(HeaderFetchSize,unsigned long int);
  itkGetMacro(HeaderFetchSize,unsigned long int);
  
  /** Prepare the allocation of the output image during the first back
   * propagation of the pipeline. */
  virtual void GenerateOutputInformation(void);

  /** Give the reader a chance to indicate that it will produce more
   * output than it was requested to produce. ImageNetworkReader cannot
   * currently read a portion of an image (since the ImageIO objects
   * cannot read a portion of an image), so the ImageNetworkReader must
   * enlarge the RequestedRegion to the size of the image on disk. */
  virtual void EnlargeOutputRequestedRegion(DataObject *output);
  
protected:
  ImageNetworkReader();
  ~ImageNetworkReader();
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Does the real work. */
  virtual void GenerateData();

  virtual void CalculateTemporaryFilenames();

  ImageIOBase::Pointer m_ImageIO;
  bool                 m_UserSpecifiedImageIO; // keep track whether the
                                               // ImageIO is user specified 
  /** Convert a block of pixels from one type to another. */
  void DoConvertBuffer(void* buffer, unsigned long numberOfPixels);

  std::string m_URI; // The file to be read
  std::string m_DataURI; // The data file to be read
  std::string m_TemporaryDirectory;
  std::string m_DataExtention;
  std::string m_CachedHeaderFilename;
  std::string m_CachedDataFilename;
  mutable TimeStamp m_HeaderCacheTimeStamp;
  mutable TimeStamp m_DataCacheTimeStamp;
  // How much of the header do we want to fetch? 16K should be plenty.
  unsigned long int m_HeaderFetchSize;

  /** Cache the header */
  virtual void CacheHeader();
  virtual void CacheHeaderAndData();
  virtual void CacheFile ( std::string uri, std::string fn, unsigned long int MaxSize );

  bool m_UseStreaming;
  
private:
  ImageNetworkReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  std::string m_ExceptionMessage;  


  // The region that the ImageIO class will return when we ask it to produce a
  // requested region.
  ImageRegionType m_StreamableRegion; 
};


} //namespace ITK

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageNetworkReader.txx"
#endif

#endif // __itkImageNetworkReader_h
