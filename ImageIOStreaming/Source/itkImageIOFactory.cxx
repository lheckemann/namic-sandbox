/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageIOFactory.cxx,v $
  Language:  C++
  Date:      $Date: 2007/03/22 14:28:51 $
  Version:   $Revision: 1.33 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include "itkImageIOFactory.h"
#if 0
#include "itkBioRadImageIOFactory.h"
#include "itkBMPImageIOFactory.h"
#include "itkGDCMImageIOFactory.h"
#include "itkDICOMImageIO2Factory.h"
#endif
#include "itkNiftiImageIOFactory.h"
#if 0
#include "itkAnalyzeImageIOFactory.h"
#endif
#include "itkAnalyzeObjectLabelMapImageIOFactory.h"
#if 0
#include "itkGiplImageIOFactory.h"
#include "itkJPEGImageIOFactory.h"
#include "itkLSMImageIOFactory.h"
#endif
#include "itkMetaImageIOFactory.h"
#if 0
#include "itkPNGImageIOFactory.h"
#include "itkNrrdImageIOFactory.h"
#include "itkTIFFImageIOFactory.h"
#include "itkVTKImageIOFactory.h"
#include "itkStimulateImageIOFactory.h"
#endif
#include "itkMutexLock.h"
#include "itkMutexLockHolder.h"

namespace itk
{

ImageIOBase::Pointer
ImageIOFactory::CreateImageIO(const char* path, FileModeType mode)
{

  RegisterBuiltInFactories();

  std::list<ImageIOBase::Pointer> possibleImageIO;
  std::list<LightObject::Pointer> allobjects =
    ObjectFactoryBase::CreateAllInstance("itkImageIOBase");
  for(std::list<LightObject::Pointer>::iterator i = allobjects.begin();
      i != allobjects.end(); ++i)
    {
    ImageIOBase* io = dynamic_cast<ImageIOBase*>(i->GetPointer());
    if(io)
      {
      std::cout << "Factory Candidate " << io->GetNameOfClass() << std::endl;
      possibleImageIO.push_back(io);
      }
    else
      {
      std::cerr << "Error ImageIO factory did not return an ImageIOBase: "
                << (*i)->GetNameOfClass()
                << std::endl;
      }
    }
  for(std::list<ImageIOBase::Pointer>::iterator k = possibleImageIO.begin();
      k != possibleImageIO.end(); ++k)
    {
    if( mode == ReadMode )
      {
      if((*k)->CanReadFile(path))
        {
        return *k;
        }
      }
    else if( mode == WriteMode )
      {
      if((*k)->CanWriteFile(path))
        {
        return *k;
        }

      }
    }
  return 0;
}

void
ImageIOFactory::RegisterBuiltInFactories()
{
  static bool firstTime = true;

  static SimpleMutexLock mutex;
    {
    // This helper class makes sure the Mutex is unlocked
    // in the event an exception is thrown.
    MutexLockHolder<SimpleMutexLock> mutexHolder( mutex );
    if( firstTime )
      {
#if 0
     ObjectFactoryBase::RegisterFactory( GDCMImageIOFactory::New() );
#endif
     ObjectFactoryBase::RegisterFactory( MetaImageIOFactory::New() );
#if 0
     ObjectFactoryBase::RegisterFactory( PNGImageIOFactory::New() );
     ObjectFactoryBase::RegisterFactory( VTKImageIOFactory::New() );
     ObjectFactoryBase::RegisterFactory( GiplImageIOFactory::New() );
     ObjectFactoryBase::RegisterFactory( BioRadImageIOFactory::New() );
     ObjectFactoryBase::RegisterFactory( LSMImageIOFactory::New()); //should be before TIFF
#endif
      ObjectFactoryBase::RegisterFactory( NiftiImageIOFactory::New());
#if 0
     ObjectFactoryBase::RegisterFactory( AnalyzeImageIOFactory::New());
     ObjectFactoryBase::RegisterFactory( StimulateImageIOFactory::New());
     ObjectFactoryBase::RegisterFactory( JPEGImageIOFactory::New());
     ObjectFactoryBase::RegisterFactory( TIFFImageIOFactory::New());
     ObjectFactoryBase::RegisterFactory( NrrdImageIOFactory::New() );
     ObjectFactoryBase::RegisterFactory( BMPImageIOFactory::New() );
     ObjectFactoryBase::RegisterFactory( DICOMImageIO2Factory::New() );
#endif
      ObjectFactoryBase::RegisterFactory( AnalyzeObjectLabelMapImageIOFactory::New() );
      firstTime = false;
      }
    }
}

} // end namespace itk