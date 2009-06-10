#include "cVolOp.h"

#include <float.h>  // for DBL_MIN

#include <fstream>

#include "time.h"


//Constructor
cVolOp::cVolOp()
{
  //  _itkVolumeExists = false;
}


/*
  IO read
 */

cVolume *cVolOp::volreadDouble( const char* filename )
{
  typedef double itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;
  typedef itk::ImageFileReader< itkVolumeType > itkVolReaderType;

  itkVolReaderType::Pointer reader = itkVolReaderType::New();
  reader->SetFileName( filename );
  try 
    {
      reader->Update();
    }
  catch ( itk::ExceptionObject &err)   
    {   
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }

  itkVolumeType::Pointer itkVolume = reader->GetOutput();
  itkVolumeType::RegionType inputRegion = itkVolume->GetLargestPossibleRegion();
  itkVolumeType::SizeType sizeThe = inputRegion.GetSize(); 
        
  int x = sizeThe[0], y = sizeThe[1], z = sizeThe[2];

  const itkVolumeType::SpacingType& spacing = itkVolume->GetSpacing();
  
  //  std::cout<<"Spacing is: "<<spacing[0]<<"\t"<<spacing[1]<<"\t"<<spacing[2]<<std::endl;
        
  cVolume *volume = new cVolume(x, y, z, spacing[0], spacing[1], spacing[2]);// initialize the volume according to the size

  typedef itk::ImageRegionConstIterator< itkVolumeType > itkVolConstIteratorType;
  itkVolConstIteratorType inputIt( itkVolume, inputRegion );
  inputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++inputIt) 
    {
      double a = inputIt.Get();
      volume->setVoxel(it, a );
      //      std::cerr<<a<<'\t';
      //      volume->setVoxel(it, static_cast<voxelType>( inputIt.Get() ) );
    }

  return volume;
}

cVolume *cVolOp::volreadUshort( const char* filename )
{
  typedef unsigned short itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;
  typedef itk::ImageFileReader< itkVolumeType > itkVolReaderType;

  itkVolReaderType::Pointer reader = itkVolReaderType::New();
  reader->SetFileName( filename );
  try 
    {
      reader->Update();
    }
  catch ( itk::ExceptionObject &err)   
    {   
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }

  itkVolumeType::Pointer itkVolume = reader->GetOutput();
  itkVolumeType::RegionType inputRegion = itkVolume->GetLargestPossibleRegion();
  itkVolumeType::SizeType sizeThe = inputRegion.GetSize(); 
        
  int x = sizeThe[0], y = sizeThe[1], z = sizeThe[2];

  const itkVolumeType::SpacingType& spacing = itkVolume->GetSpacing();
  
  //  std::cout<<"Spacing is: "<<spacing[0]<<"\t"<<spacing[1]<<"\t"<<spacing[2]<<std::endl;
        
  cVolume *volume = new cVolume(x, y, z, spacing[0], spacing[1], spacing[2]);// initialize the volume according to the size

  typedef itk::ImageRegionConstIterator< itkVolumeType > itkVolConstIteratorType;
  itkVolConstIteratorType inputIt( itkVolume, inputRegion );
  inputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++inputIt) 
    {
      double a = inputIt.Get();
      volume->setVoxel(it, a );
      //      std::cerr<<a<<'\t';
      //      volume->setVoxel(it, static_cast<voxelType>( inputIt.Get() ) );
    }

  return volume;
}

cVolume *cVolOp::volreadShort( const char* filename )
{
  typedef short itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;
  typedef itk::ImageFileReader< itkVolumeType > itkVolReaderType;

  itkVolReaderType::Pointer reader = itkVolReaderType::New();
  reader->SetFileName( filename );
  try 
    {
      reader->Update();
    }
  catch ( itk::ExceptionObject &err)   
    {   
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }

  itkVolumeType::Pointer itkVolume = reader->GetOutput();
  itkVolumeType::RegionType inputRegion = itkVolume->GetLargestPossibleRegion();
  itkVolumeType::SizeType sizeThe = inputRegion.GetSize(); 
        
  int x = sizeThe[0], y = sizeThe[1], z = sizeThe[2];

  const itkVolumeType::SpacingType& spacing = itkVolume->GetSpacing();
  
  //  std::cout<<"Spacing is: "<<spacing[0]<<"\t"<<spacing[1]<<"\t"<<spacing[2]<<std::endl;
        
  cVolume *volume = new cVolume(x, y, z, spacing[0], spacing[1], spacing[2]);// initialize the volume according to the size

  typedef itk::ImageRegionConstIterator< itkVolumeType > itkVolConstIteratorType;
  itkVolConstIteratorType inputIt( itkVolume, inputRegion );
  inputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++inputIt) 
    {
      double a = inputIt.Get();
      volume->setVoxel(it, a );
      //      std::cerr<<a<<'\t';
      //      volume->setVoxel(it, static_cast<voxelType>( inputIt.Get() ) );
    }

  return volume;
}

cVolume *cVolOp::volreadUchar( const char* filename )
{
  typedef unsigned char itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;
  typedef itk::ImageFileReader< itkVolumeType > itkVolReaderType;

  itkVolReaderType::Pointer reader = itkVolReaderType::New();
  reader->SetFileName( filename );
  try 
    {
      reader->Update();
    }
  catch ( itk::ExceptionObject &err)   
    {   
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }

  itkVolumeType::Pointer itkVolume = reader->GetOutput();
  itkVolumeType::RegionType inputRegion = itkVolume->GetLargestPossibleRegion();
  itkVolumeType::SizeType sizeThe = inputRegion.GetSize(); 
        
  int x = sizeThe[0], y = sizeThe[1], z = sizeThe[2];

  const itkVolumeType::SpacingType& spacing = itkVolume->GetSpacing();
  
  //  std::cout<<"Spacing is: "<<spacing[0]<<"\t"<<spacing[1]<<"\t"<<spacing[2]<<std::endl;
        
  cVolume *volume = new cVolume(x, y, z, spacing[0], spacing[1], spacing[2]);// initialize the volume according to the size

  typedef itk::ImageRegionConstIterator< itkVolumeType > itkVolConstIteratorType;
  itkVolConstIteratorType inputIt( itkVolume, inputRegion );
  inputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++inputIt) 
    {
      double a = inputIt.Get();
      volume->setVoxel(it, a );
      //      std::cerr<<a<<'\t';
      //      volume->setVoxel(it, static_cast<voxelType>( inputIt.Get() ) );
    }

  return volume;
}


/*
  IO write
 */

void cVolOp::volwriteDouble( const char* filename, cVolume *volume )
{
  typedef double itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  //  std::cout<<"output:      "<< x<<"\t"<< y<<"\t"<< z<<std::endl;
                
  itkVolumeType::IndexType start;
  start[0] = 0; // first index on X
  start[1] = 0; // first index on Y
  start[2] = 0; // first index on Z
        
  itkVolumeType::SizeType size;
  size[0] = x; // size aint X
  size[1] = y; // size aint Y
  size[2] = z; // size aint Z
        
  itkVolumeType::RegionType outputRegion;
  outputRegion.SetSize( size );
  outputRegion.SetIndex( start );

  itkVolumeType::Pointer outputVolume = itkVolumeType::New();   
  outputVolume->SetRegions( outputRegion );
  //  outputVolume->SetSpacing(volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingY());
  double sp[] = {volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingZ()};
  outputVolume->SetSpacing(sp);
  outputVolume->Allocate();  
  
  typedef itk::ImageRegionIterator< itkVolumeType > itkVolIteratorType;
  itkVolIteratorType outputIt( outputVolume, outputRegion );
  outputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++outputIt)  
    {
      outputIt.Set( static_cast<itkVoxelType>( volume->getVoxel(it)) );      
    }

  typedef itk::ImageFileWriter< itkVolumeType > itkVolWriterType;
  
  itkVolWriterType::Pointer writer = itkVolWriterType::New();
  writer->SetFileName( filename );
  writer->SetInput( outputVolume );
    
  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    } 

  return;
}


    
void cVolOp::volwriteUchar( const char* filename, cVolume *volume )
{
  typedef unsigned char itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  //  std::cout<<"output:      "<< x<<"\t"<< y<<"\t"<< z<<std::endl;
                
  itkVolumeType::IndexType start;
  start[0] = 0; // first index on X
  start[1] = 0; // first index on Y
  start[2] = 0; // first index on Z
        
  itkVolumeType::SizeType size;
  size[0] = x; // size aint X
  size[1] = y; // size aint Y
  size[2] = z; // size aint Z
        
  itkVolumeType::RegionType outputRegion;
  outputRegion.SetSize( size );
  outputRegion.SetIndex( start );

  itkVolumeType::Pointer outputVolume = itkVolumeType::New();   
  outputVolume->SetRegions( outputRegion );
  //  outputVolume->SetSpacing(volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingY());
  double sp[] = {volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingZ()};
  outputVolume->SetSpacing(sp);
  outputVolume->Allocate();  
  
  typedef itk::ImageRegionIterator< itkVolumeType > itkVolIteratorType;
  itkVolIteratorType outputIt( outputVolume, outputRegion );
  outputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++outputIt)  
    {
      outputIt.Set( static_cast<itkVoxelType>( volume->getVoxel(it)) );      
    }

  typedef itk::ImageFileWriter< itkVolumeType > itkVolWriterType;
  
  itkVolWriterType::Pointer writer = itkVolWriterType::New();
  writer->SetFileName( filename );
  writer->SetInput( outputVolume );
    
  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    } 

  return;
}


void cVolOp::volwriteUshort( const char* filename, cVolume *volume )
{
  typedef unsigned short itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  //  std::cout<<"output:      "<< x<<"\t"<< y<<"\t"<< z<<std::endl;
                
  itkVolumeType::IndexType start;
  start[0] = 0; // first index on X
  start[1] = 0; // first index on Y
  start[2] = 0; // first index on Z
        
  itkVolumeType::SizeType size;
  size[0] = x; // size aint X
  size[1] = y; // size aint Y
  size[2] = z; // size aint Z
        
  itkVolumeType::RegionType outputRegion;
  outputRegion.SetSize( size );
  outputRegion.SetIndex( start );

  itkVolumeType::Pointer outputVolume = itkVolumeType::New();   
  outputVolume->SetRegions( outputRegion );
  //  outputVolume->SetSpacing(volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingY());
  double sp[] = {volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingZ()};
  outputVolume->SetSpacing(sp);
  outputVolume->Allocate();  
  
  typedef itk::ImageRegionIterator< itkVolumeType > itkVolIteratorType;
  itkVolIteratorType outputIt( outputVolume, outputRegion );
  outputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++outputIt)  
    {
      outputIt.Set( static_cast<itkVoxelType>( volume->getVoxel(it)) );      
    }

  typedef itk::ImageFileWriter< itkVolumeType > itkVolWriterType;
  
  itkVolWriterType::Pointer writer = itkVolWriterType::New();
  writer->SetFileName( filename );
  writer->SetInput( outputVolume );
    
  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    } 

  return;
}

void cVolOp::volwriteShort( const char* filename, cVolume *volume )
{
  typedef short itkVoxelType;
  typedef itk::Image< itkVoxelType, itkVolDimension > itkVolumeType;

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  //  std::cout<<"output:      "<< x<<"\t"<< y<<"\t"<< z<<std::endl;
                
  itkVolumeType::IndexType start;
  start[0] = 0; // first index on X
  start[1] = 0; // first index on Y
  start[2] = 0; // first index on Z
        
  itkVolumeType::SizeType size;
  size[0] = x; // size aint X
  size[1] = y; // size aint Y
  size[2] = z; // size aint Z
        
  itkVolumeType::RegionType outputRegion;
  outputRegion.SetSize( size );
  outputRegion.SetIndex( start );

  itkVolumeType::Pointer outputVolume = itkVolumeType::New();   
  outputVolume->SetRegions( outputRegion );
  //  outputVolume->SetSpacing(volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingY());
  double sp[] = {volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingZ()};
  outputVolume->SetSpacing(sp);
  outputVolume->Allocate();  
  
  typedef itk::ImageRegionIterator< itkVolumeType > itkVolIteratorType;
  itkVolIteratorType outputIt( outputVolume, outputRegion );
  outputIt.GoToBegin();

  for (int it = 0; it < x*y*z; ++it, ++outputIt)  
    {
      outputIt.Set( static_cast<itkVoxelType>( volume->getVoxel(it)) );      
    }

  typedef itk::ImageFileWriter< itkVolumeType > itkVolWriterType;
  
  itkVolWriterType::Pointer writer = itkVolWriterType::New();
  writer->SetFileName( filename );
  writer->SetInput( outputVolume );
    
  try 
    { 
      writer->Update(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    } 

  return;
}


/*
  smoothing
 */

cVolume* cVolOp::meanSmooth(cVolume *volume, int kernelSize)
{
  if (volume->initiated() == false)
    {
      std::cerr << "volume not initiated, exiting..." << std::endl;       
      exit(-1);
    }

  int x = volume->getSizeX();
  int y = volume->getSizeY();
  int z = volume->getSizeZ();

  if (kernelSize > x || kernelSize > y || kernelSize > z) 
    {
      std::cerr << "Kernel size too large, exiting..." << std::endl;       
      exit(-1);
    }

  cVolume *smoothVol = new cVolume( volume );

  if ( (int)(kernelSize/2) == ((float)kernelSize)/2 ) kernelSize++;

  int halfKernelSize = (kernelSize - 1)/2;

  double a = 0;

  for (int itx = halfKernelSize; itx < x - halfKernelSize; ++itx)
    for (int ity = halfKernelSize; ity < y - halfKernelSize; ++ity)     
      for (int itz = halfKernelSize; itz < z - halfKernelSize; ++itz)
        {
          a = 0;
          for (int itxx = -halfKernelSize; itxx <= halfKernelSize; ++itxx)
            for (int ityy = -halfKernelSize; ityy <= halfKernelSize; ++ityy)    
              for (int itzz = -halfKernelSize; itzz <= halfKernelSize; ++itzz)  
                {
                  a += volume->getVoxel(itx + itxx, ity + ityy, itz + itzz);
                  //                  a += volume->getVoxel((itz + itzz)*x*y + (ity + ityy)*y + itx + itxx);
                }
          smoothVol->setVoxel( itx, ity, itz, a/kernelSize/kernelSize/kernelSize );
        }

  return smoothVol;
}




void cVolOp::writeRawData( cVolume *volume )
{
  if (volume->initiated() == false) 
    {
      std::cerr<<"Input volume not initialized! exiting..."<<std::endl;
      exit(-1);      
    }

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();

  std::ofstream rawHeader("rawData.header");
  rawHeader<<"sizes: "<<x<<" "<<y<<" "<<z<<std::endl;
  rawHeader.close();
  
  std::ofstream rawFP("rawData.dat");

  for (int it = 0; it < x*y*z; ++it) 
    {
      rawFP<<volume->getVoxel(it)<<std::endl;
    }
  rawFP.close();
  
        
  return;
}
        

itkInternalVolumeType::Pointer cVolOp::cVolume2itkVolume( cVolume *volume )
{
  if (volume->initiated() == false) 
    {
      std::cerr<<"Input volume not initialized! exiting..."<<std::endl;
      exit(-1);      
    }

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();

  itkInternalVolumeType::Pointer _itkVolumePointer = itkInternalVolumeType::New();
        
  itkInternalVolumeType::IndexType start;
  start[0] = 0; // first index on X
  start[1] = 0; // first index on Y
  start[2] = 0; // first index on Z
        
  itkInternalVolumeType::SizeType size;
  size[0] = x; // size aint X
  size[1] = y; // size aint Y
  size[2] = z; // size aint Z
        
  itkInternalVolumeType::RegionType theRegion;
  theRegion.SetSize( size );
  theRegion.SetIndex( start );
        
  _itkVolumePointer->SetRegions( theRegion );
  _itkVolumePointer->Allocate();  
  
  itkVolumeInternalIteratorType theIt( _itkVolumePointer, theRegion );
  theIt.GoToBegin();
  
  for (int it = 0; it < x*y*z; ++it, ++theIt)  
    {
      theIt.Set( volume->getVoxel(it) );
    }

  //  _itkVolumeExists = true;

  return _itkVolumePointer;
}

cVolume *cVolOp::itkVolume2cVolume( itkInternalVolumeType::Pointer itkVol )
{
  //  if ( _itkVolumeExists == false ) return;
  //  if ( volume->initiated() == true ) volume->destroyIm();

  itkInternalVolumeType::RegionType theRegion = itkVol->GetLargestPossibleRegion();
  itkInternalVolumeType::SizeType sizeThe = theRegion.GetSize(); 
        
  int x = sizeThe[0], y = sizeThe[1], z = sizeThe[2];
  
  //  std::cout<< x<<"\t"<< y<<"\t"<< z<<std::endl;
  const itkInternalVolumeType::SpacingType& spacing = itkVol->GetSpacing();
  
  cVolume *volume = new cVolume(x, y, z, spacing[0], spacing[1], spacing[2]);

  itkVolumeInternalConstIteratorType inputIt( itkVol, theRegion );
  inputIt.GoToBegin();
  
  for (int it = 0; it < x*y*z; ++it, ++inputIt) 
    {
      volume->setVoxel( it, inputIt.Get() );
    }

  return volume;
}



cVolume* cVolOp::normalizeVol( cVolume *volume, voxelType theMin, voxelType theMax )
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not yet initialized, could NOT normalize. Exiting..."<<std::endl;
      exit(-1);
    }

  voxelType maxVal = volume->getMaxVoxel(), minVal = volume->getMinVoxel();
  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();

  cVolume *normalizedVol = new cVolume( volume );

  voxelType factor = ( theMax - theMin )/( maxVal - minVal + DBL_MIN );
  
  for (int it = 0; it<x*y*z; ++it) 
    {
      normalizedVol->setVoxel( it, ( volume->getVoxel(it) - minVal )*factor + theMin );
    }

  return normalizedVol;
}

cVolume* cVolOp::logTrans( cVolume *volume )
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not yet initialized, could NOT take LOG. Exiting..."<<std::endl;
      exit(-1);
    }

  // has to shift to positive for log to be well defined.
  voxelType minVal = volume->getMinVoxel();

  if (minVal < 1) 
    {
      std::cerr<<"For log() to behave better, please make sure min value to be greater or equal to 1."<<std::endl<<"Exiting..."<<std::endl;
      exit(-1);
    }

  cVolume *logVol = new cVolume( volume );
  
  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();

  for (int it = 0; it<x*y*z; ++it) 
    {
      logVol->setVoxel(it, log( volume->getVoxel(it) ) ); // natural log.
    }

  return logVol;
}

cVolume* cVolOp::expTrans( cVolume *volume )
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not yet initialized, could NOT take LOG. Exiting..."<<std::endl;
      exit(-1);
    }

  cVolume *expVol = new cVolume( volume );

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();

  for (int it = 0; it<x*y*z; ++it) 
    {
      expVol->setVoxel( it, exp( volume->getVoxel(it) ) );
    }

  return expVol;
}



cVolume * cVolOp::addVolumes( cVolume *volume1, cVolume *volume2 )
{
  if (volume1->getDimension() != volume2->getDimension() )
    {
      std::cerr<<"dimensions don't match! exiting..."<<std::endl;
      exit(-1);
    }
  
  if ( volume1->getSizeX() != volume1->getSizeX() \
       || volume1->getSizeY() != volume1->getSizeY() \
       || volume1->getSizeZ() != volume1->getSizeZ())
    {
      std::cerr<<"sizes do match! exiting..."<<std::endl;
      exit(-1);
    }

  cVolume *sumVol = new cVolume( volume1 );

  int totalSize = (sumVol->getSizeX()) * (sumVol->getSizeY()) * (sumVol->getSizeZ());

  for ( int i = 0; i < totalSize; ++i)
    {
      sumVol->setVoxel(i, sumVol->getVoxel(i) + volume2->getVoxel(i) );
    }

  return sumVol;
}


cVolume * cVolOp::substractVolumes( cVolume *volume1, cVolume *volume2 )
{
  if (volume1->getDimension() != volume2->getDimension() )
    {
      std::cerr<<"dimensions don't match! exiting..."<<std::endl;
      exit(-1);
    }
  
  if ( volume1->getSizeX() != volume1->getSizeX() \
       || volume1->getSizeY() != volume1->getSizeY() \
       || volume1->getSizeZ() != volume1->getSizeZ())
    {
      std::cerr<<"sizes do match! exiting..."<<std::endl;
      exit(-1);
    }

  cVolume *substractmVol =  new cVolume( volume1 );
  int totalSize = (substractmVol->getSizeX()) * (substractmVol->getSizeY()) * (substractmVol->getSizeZ());

  for ( int i = 0; i < totalSize; ++i)
    {
      substractmVol->setVoxel(i, substractmVol->getVoxel(i) - volume2->getVoxel(i));
    }

  return substractmVol;
}



cVolume* cVolOp::sqrtTrans( cVolume *volume )
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not yet initialized, could NOT take sqrt. Exiting..."<<std::endl;
      exit(-1);
    }

  // has to shift to positive for log to be well defined.
  voxelType minVal = volume->getMinVoxel();
  if ( minVal < 0 )
    {
      std::cerr<<"contains negative values, could NOT take sqrt. Exiting..."<<std::endl;
      exit(-1);
    }

  cVolume* sqrtVol = new cVolume( volume );

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  int l = x*y*z;

  for (int it = 0; it < l; ++it) 
    {
      sqrtVol->setVoxel(it, sqrt( volume->getVoxel(it) ) );
    }

  return sqrtVol;
}

cVolume *cVolOp::removeNeg( cVolume *volume )
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not yet initialized. Exiting..."<<std::endl;
      exit(-1);
    }

  cVolume* nonNegVol = new cVolume( volume );

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  int l = x*y*z;

  for (int it = 0; it < l; ++it) 
    {
      nonNegVol->setVoxel(it, (volume->getVoxel(it) >= 0)?volume->getVoxel(it):0 );
    }

  return nonNegVol;
}




cVolume* cVolOp::volcrop( cVolume *volume, int minX, int minY, int minZ, int xSize, int ySize, int zSize)
{
  // note, (minX, minY, minZ) starts from (0, 0, 0)
  // note, the point (minX, minY, minZ) IS in the new image, actually it's one of the corner.

  int szx = volume->getSizeX();
  int szy = volume->getSizeY();
  int szz = volume->getSizeZ();

  if (minX < 0 || minX > szx - 1 || minY < 0 || minY > szy - 1 || minZ < 0 || minZ > szz - 1)
    {
      std::cerr<<"minX or minY or minZ fall out of bound. exiting..."<<std::endl;
      exit(-1);
    }

  if ( minX + xSize > szx - 1)
    {
      xSize = szx - minX;
    }
  else if (minY + ySize > szy - 1)
    {
      ySize = szy - minY;
    }
  else if (minZ + zSize > szz - 1)
    {
      zSize = szz - minZ;
    }


  if (xSize < 1 || ySize < 1 || zSize < 1)
    {
      std::cerr<<"output volume toooooo small. exiting..."<<std::endl;
      exit(-1);
    }

  cVolume *cropedVol = new cVolume(xSize, ySize, zSize);

  for (int ix = 0; ix <= xSize-1; ++ix)
    {
      for (int iy = 0; iy <= ySize-1; ++iy)
        {
          for (int iz = 0; iz <= zSize-1; ++iz)
            {

              cropedVol->setVoxel(ix, iy, iz, volume->getVoxel(minX + ix, minY + iy, minZ + iz));
            }
        }
    }
  
  return cropedVol;  
}

cVolume* cVolOp::GaussSmth( cVolume *volume, int kernelWidth, double varOfGauss)
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not initialized yet. Exiting..."<<std::endl;
      exit(-1);
    }

  itkInternalVolumeType::Pointer itkVol = cVolume2itkVolume( volume );

  typedef itk::DiscreteGaussianImageFilter<itkInternalVolumeType, itkInternalVolumeType> FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput( itkVol );

  filter->SetVariance( varOfGauss );
  filter->SetMaximumKernelWidth( kernelWidth );
  
  filter->Update();

  itkInternalVolumeType::Pointer newItkVol = filter->GetOutput();
  
  cVolume* newVol = itkVolume2cVolume( newItkVol );
  
  return newVol;
}


cVolume* cVolOp::medianFilter( cVolume *volume, int radiusX, int radiusY, int radiusZ)
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not initialized yet. Exiting..."<<std::endl;
      exit(-1);
    }

  typedef itk::MedianImageFilter< itkInternalVolumeType, itkInternalVolumeType >  FilterType;

  FilterType::Pointer filter = FilterType::New();

  itkInternalVolumeType::SizeType indexRadius;
  
  indexRadius[0] = radiusX; // radius along x
  indexRadius[1] = radiusY; // radius along y
  indexRadius[2] = radiusZ; // radius along z

  filter->SetRadius( indexRadius );

  itkInternalVolumeType::Pointer itkVol = cVolume2itkVolume( volume );

  filter->SetInput( itkVol );
  filter->Update();

  cVolume *newVol = itkVolume2cVolume( filter->GetOutput() );

  return newVol;
}




cVolume *cVolOp::hardThreshold(cVolume *volume, voxelType thld, voxelType outputLow, voxelType outputHigh)
{
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not initialized yet. Exiting..."<<std::endl;
      exit(-1);
    }

  int x = volume->getSizeX(), y = volume->getSizeY(), z = volume->getSizeZ();
  int l = x*y*z;

  cVolume *binVol = new cVolume(volume);

  for (int it = 0; it < l; ++it) 
    {
      voxelType v = binVol->getVoxel(it);
      binVol->setVoxel(it, v <= thld?outputLow:outputHigh);
    }


  binVol->setSpacing(volume->getSpacingX(), volume->getSpacingY(), volume->getSpacingZ() );

  return binVol;
}





cVolume* cVolOp::gheSmooth(cVolume *volume, int iterNum)
{
  cVolume *pVol = new cVolume( volume );
  cVolume *newVol = new cVolume( volume );
  cVolume *tem;

  if (pVol->initiated() == false)
    {
      std::cerr<<"Input volume not initialized! exiting..."<<std::endl;
      exit(-1);
    }

  int X = pVol->getSizeX();
  int Y = pVol->getSizeY();
  int Z = pVol->getSizeZ();

  double spX = pVol->getSpacingX();
  double spY = pVol->getSpacingY();
  double spZ = pVol->getSpacingZ();

  double spX2 = 2*spX;
  double spY2 = 2*spY;
  double spZ2 = 2*spZ;

  double spXsq = spX*spX;
  double spYsq = spY*spY;
  double spZsq = spZ*spZ;
  double minSpSq = (spXsq<=spYsq)?spXsq:spYsq;
  minSpSq = (minSpSq<=spZsq)?minSpSq:spZsq;

  double spXY = spX*spY;
  double spXZ = spX*spY;
  double spYZ = spY*spZ;


  //  std::vector<voxelType>::iterator pixelPointer = pVol->getVoxelIterator();

  float dt = minSpSq/10; // for 3D

  //  std::cerr<<"minSpSq is: "<<minSpSq<<"\t dt is: "<<dt<<std::endl;

  for (int in = 0; in < iterNum; ++in) 
    {
      for (int iz=1; iz <= Z-2; iz++) 
        {
          for (int iy=1; iy <= Y-2; iy++) 
            {
              for (int ix=1; ix <= X-2; ix++) 
                {
                  double I = pVol->getVoxel(ix, iy, iz);
                  double I_x_minus = pVol->getVoxel(ix-1, iy, iz);
                  double I_x_plus = pVol->getVoxel(ix+1, iy, iz);
                  double I_y_minus = pVol->getVoxel(ix, iy-1, iz);
                  double I_y_plus = pVol->getVoxel(ix, iy+1, iz);
                  double I_z_minus = pVol->getVoxel(ix, iy, iz-1);
                  double I_z_plus = pVol->getVoxel(ix, iy, iz+1);

                  double I_x_minus_y_minus = pVol->getVoxel(ix-1, iy-1, iz);
                  double I_x_plus_y_plus = pVol->getVoxel(ix+1, iy+1, iz);
                  double I_x_minus_y_plus = pVol->getVoxel(ix-1, iy+1, iz);
                  double I_x_plus_y_minus = pVol->getVoxel(ix+1, iy-1, iz);

                  double I_x_minus_z_minus = pVol->getVoxel(ix-1, iy, iz-1);
                  double I_x_plus_z_plus = pVol->getVoxel(ix+1, iy, iz+1);
                  double I_x_minus_z_plus = pVol->getVoxel(ix-1, iy, iz+1);
                  double I_x_plus_z_minus = pVol->getVoxel(ix+1, iy, iz-1);

                  double I_y_minus_z_minus = pVol->getVoxel(ix, iy-1, iz-1);
                  double I_y_plus_z_plus = pVol->getVoxel(ix, iy+1, iz+1);
                  double I_y_minus_z_plus = pVol->getVoxel(ix, iy-1, iz+1);
                  double I_y_plus_z_minus = pVol->getVoxel(ix, iy+1, iz-1);

                  voxelType Ix=0, Iy=0, Iz=0, Ixx=0, Iyy=0, Izz=0, Ixy=0, Ixz=0, Iyz=0;

                  Ix = (I_x_plus - I_x_minus)/spX2;
                  Iy = (I_y_plus - I_y_minus)/spY2;
                  Iz = (I_z_plus - I_z_minus)/spZ2;
                        
                  Ixx = (I_x_plus - 2*I + I_x_minus)/spXsq;
                  Iyy = (I_y_plus - 2*I + I_y_minus)/spYsq;;
                  Izz = (I_z_plus - 2*I + I_y_minus)/spZsq;;
              
                  Ixy = 0.25*(I_x_plus_y_plus + I_x_minus_y_minus - I_x_plus_y_minus - I_x_minus_y_plus)/spXY;
                  Ixz = 0.25*(I_x_plus_z_plus + I_x_minus_z_minus - I_x_plus_z_minus - I_x_minus_z_plus)/spXZ;
                  Iyz = 0.25*(I_y_plus_z_plus + I_y_minus_z_minus - I_y_plus_z_minus - I_y_minus_z_plus)/spYZ;

                  voxelType dI = dt*((Ix*Ix*( Iyy + Izz ) + Iy*Iy*( Ixx + Izz ) + Iz*Iz*( Ixx + Iyy ) \
                                      - 2*Ix*Iy*Ixy - 2*Ix*Iz*Ixz - 2*Iy*Iz*Iyz )/(Ix*Ix + Iy*Iy + Iz*Iz + DBL_MIN));

                  newVol->setVoxel(ix, iy, iz, I+dI);
                }
            }
        }

      // deal with faces:
      // y=0, y=Y-1 faces without edges
      for (int ix = 1; ix <= X-2; ++ix)
        {
          for (int iz = 1; iz <= Z-2; ++iz)
            {
              int iy = 0;

              double a = 2*(newVol->getVoxel(ix, iy+1, iz)) - newVol->getVoxel(ix, iy+2, iz);
              newVol->setVoxel(ix, iy, iz, a);

              iy = Y-1;
              a = 2*(newVol->getVoxel(ix, iy-1, iz)) - newVol->getVoxel(ix, iy-2, iz);
              newVol->setVoxel(ix, iy, iz, a);
            }
        }

      // z=0, z=Z-1 faces without edges
      for (int ix = 1; ix <= X-2; ++ix)
        {
          for (int iy = 1; iy <= Y-2; ++iy)
            {
              int iz = 0;

              double a = 2*(newVol->getVoxel(ix, iy, iz+1)) - newVol->getVoxel(ix, iy, iz+2);
              newVol->setVoxel(ix, iy, iz, a);

              iz = Z-1;
              a = 2*(newVol->getVoxel(ix, iy, iz-1)) - newVol->getVoxel(ix, iy, iz-2);
              newVol->setVoxel(ix, iy, iz, a);
            }
        }

      // x=0, x=X-1 faces without edges
      for (int iy = 1; iy <= Y-2; ++iy)
        {
          for (int iz = 1; iz <= Z-2; ++iz)
            {
              int ix = 0;

              double a = 2*(newVol->getVoxel(ix+1, iy, iz)) - newVol->getVoxel(ix+2, iy, iz);
              newVol->setVoxel(ix, iy, iz, a);

              ix = X-1;
              a = 2*(newVol->getVoxel(ix-1, iy, iz)) - newVol->getVoxel(ix-2, iy, iz);
              newVol->setVoxel(ix, iy, iz, a);
            }
        }

      // deal with edges:
      for (int ix = 1; ix <= X-1; ++ix)
        {
          int iy = 0, iz = 0;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix, iy+1, iz) + newVol->getVoxel(ix, iy, iz+1))/2.0 );
          
          iy = Y-1; iz = 0;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix, iy-1, iz) + newVol->getVoxel(ix, iy, iz+1))/2.0 );

          iy = 0; iz = Z-1;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix, iy+1, iz) + newVol->getVoxel(ix, iy, iz-1))/2.0 );

          iy = Y-1; iz = Z-1;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix, iy-1, iz) + newVol->getVoxel(ix, iy, iz-1))/2.0 );
        }

      for (int iy = 1; iy <= Y-1; ++iy)
        {
          int ix = 0, iz = 0;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix+1, iy, iz) + newVol->getVoxel(ix, iy, iz+1))/2.0 );
          
          ix = X-1; iz = 0;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix-1, iy, iz) + newVol->getVoxel(ix, iy, iz+1))/2.0 );

          ix = 0; iz = Z-1;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix+1, iy, iz) + newVol->getVoxel(ix, iy, iz-1))/2.0 );

          ix = X-1; iz = Z-1;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix-1, iy, iz) + newVol->getVoxel(ix, iy, iz-1))/2.0 );
        }

      for (int iz = 1; iz <= Z-1; ++iz)
        {
          int ix = 0, iy = 0;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix+1, iy, iz) + newVol->getVoxel(ix+1, iy, iz))/2.0 );
          
          ix = X-1; iy = 0;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix-1, iy, iz) + newVol->getVoxel(ix, iy+1, iz))/2.0 );

          ix = 0; iy = Y-1;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix+1, iy, iz) + newVol->getVoxel(ix, iy-1, iz))/2.0 );

          ix = X-1; iy = Y-1;
          newVol->setVoxel(ix, iy, iz, (newVol->getVoxel(ix-1, iy, iz) + newVol->getVoxel(ix, iy-1, iz-1))/2.0 );
        }

      // deal with corners:
      newVol->setVoxel(0, 0, 0, (newVol->getVoxel(1, 0, 0) + newVol->getVoxel(0, 1, 0) + newVol->getVoxel(0, 0, 1))/3.0 );
      newVol->setVoxel(X-1, 0, 0, (newVol->getVoxel(X-2, 0, 0) + newVol->getVoxel(X-1, 1, 0) + newVol->getVoxel(X-1, 0, 1))/3.0 );
      newVol->setVoxel(0, Y-1, 0, (newVol->getVoxel(0, Y-2, 0) + newVol->getVoxel(1, Y-1, 0) + newVol->getVoxel(0, Y-1, 1))/3.0 );
      newVol->setVoxel(X-1, Y-1, 0, (newVol->getVoxel(X-2, Y-1, 0) + newVol->getVoxel(X-1, Y-2, 0) + newVol->getVoxel(X-1, Y-1, 1))/3.0 );

      newVol->setVoxel(0, 0, Z-1, (newVol->getVoxel(1, 0, Z-1) + newVol->getVoxel(0, 1, Z-1) + newVol->getVoxel(0, 0, Z-2))/3.0 );
      newVol->setVoxel(X-1, 0, Z-1, (newVol->getVoxel(X-2, 0, Z-1) + newVol->getVoxel(X-1, 1, Z-1) + newVol->getVoxel(X-1, 0, Z-2))/3.0 );
      newVol->setVoxel(0, Y-1, Z-1, (newVol->getVoxel(0, Y-2, Z-1) + newVol->getVoxel(1, Y-1, Z-1) + newVol->getVoxel(0, Y-1, Z-2))/3.0 );
      newVol->setVoxel(X-1, Y-1, Z-1, (newVol->getVoxel(X-2, Y-1, Z-1) + newVol->getVoxel(X-1, Y-2, Z-1) + newVol->getVoxel(X-1, Y-1, Z-2))/3.0 );


      tem = pVol;
      pVol = newVol;
      newVol = tem;
    }

  delete newVol;

  return pVol;
}


cVolume *cVolOp::dilate(cVolume *volume, int radius)
{
  // input file should be 0-1 binary image.
  if ( volume->initiated() == false ) 
    {
      std::cerr<<"Volume not initialized yet. Exiting..."<<std::endl;
      exit(-1);
    }

  cVolume *binVol = hardThreshold(volume, 1e-6, 0, 1);

  const unsigned int Dimension = 3;
  typedef itk::BinaryBallStructuringElement< pixelType, Dimension > StructuringElementType;

  typedef itk::BinaryDilateImageFilter< itkInternalVolumeType, itkInternalVolumeType, StructuringElementType >  DilateFilterType;

  DilateFilterType::Pointer  binaryDilate  = DilateFilterType::New();

  StructuringElementType  structuringElement;

  structuringElement.SetRadius( radius );  // (2*radius+1)*(2*radius+1) structuring element
  structuringElement.CreateStructuringElement();

  binaryDilate->SetKernel(  structuringElement );

  itkInternalVolumeType::Pointer itkVol = cVolume2itkVolume( binVol );

  binaryDilate->SetInput( itkVol );

  double objVal = 1.0;
  binaryDilate->SetDilateValue( objVal );
  binaryDilate->Update();

  cVolume *newVol = itkVolume2cVolume( binaryDilate->GetOutput() );

  delete binVol; binVol = 0;

  return newVol;
}
