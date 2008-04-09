

#include <itkTimeSeriesDatabase.h>
#include <itkImageRegionConstIterator.h>
#include <itkImageRegionIterator.h>
#include <itksys/SystemTools.hxx>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itksys/SystemTools.hxx>
#include "itkArchetypeSeriesFileNames.h"
#include <fstream>

namespace itk {

  // template<class TPixel> int TimeSeriesDatabase<TPixel>::BlockSize = 16;
  template<class T> T TSD_MIN ( T a, T b ) { return a < b ? a : b; }
  template<class T> T TSD_MAX ( T a, T b ) { return a > b ? a : b; }

  
template <class TPixel>
bool TimeSeriesDatabase<TPixel>::CalculateIntersection ( Size<3> BlockIndex, 
                                                         typename OutputImageType::RegionType RequestedRegion, 
                                                         typename OutputImageType::RegionType& BlockRegion,
                                                         typename OutputImageType::RegionType& ImageRegion )
{

  // Calculate the intersection between the block at BlockIndex and the Requested Region
  bool IsFullBlock = true;
  for ( unsigned int i = 0; i < 3; i++ )
  {
    ImageRegion.SetIndex ( i, TSD_MAX ( (long unsigned int) RequestedRegion.GetIndex ( i ), TimeSeriesBlockSize * BlockIndex[i] ) );
    BlockRegion.SetIndex ( i, ImageRegion.GetIndex(i) % TimeSeriesBlockSize );

    // This is the end index
    long unsigned int Tmp = RequestedRegion.GetIndex ( i ) + RequestedRegion.GetSize ( i );
    Tmp = TSD_MIN ( (long unsigned int) Tmp, TimeSeriesBlockSize * (BlockIndex[i]+1) );
    Tmp = Tmp - ImageRegion.GetIndex(i);

    ImageRegion.SetSize ( i, Tmp );
    BlockRegion.SetSize ( i, Tmp );
    IsFullBlock = IsFullBlock & ( Tmp == TimeSeriesBlockSize );
  }
  std::cout << "Found Block Region: " << BlockRegion << std::endl;
  std::cout << "Found Image Region: " << ImageRegion << std::endl;
  if ( IsFullBlock ) {
    std::cout << "Is Full Block" << std::endl;
  } else {
    std::cout << "Not Full Block" << std::endl;
  }
    

}


  
template <class TPixel>
bool TimeSeriesDatabase<TPixel>::IsOpen () const
{
  return this->m_File.is_open();
}

template <class TPixel>
void TimeSeriesDatabase<TPixel>::Disconnect ()
{
  this->m_File.close();
}
  
template <class TPixel>
void TimeSeriesDatabase<TPixel>::Connect ( const char* filename )
{
  // If we are still open, disconnect
  if ( this->IsOpen() )
    {
    this->Disconnect();
    }
  // Open and make sure we have the correct header!
  this->m_Filename = filename;
  this->m_File.open ( this->m_Filename.c_str(), ios::in | ios::binary );
  // Read the first bits.
  char* buffer = new char[TimeSeriesVolumeBlockSize*sizeof(TPixel)];
  this->m_File.read ( buffer, TimeSeriesVolumeBlockSize * sizeof ( TPixel ) );
  // Associate it with a string
  std::string s ( buffer );
  delete[] buffer;
  std::istringstream o ( s );
  string foo;
  float version;
  o >> foo >> foo >> version;
  if ( version != 1.0 )
  {
    itkGenericExceptionMacro ( "TimeSeriesDatabase::Connect: Version string does not match.  Expecting 1.0, found " << version );
  }
  // Start reading our data
  std::string dummy;
  Size<3> sz;
  o >> dummy >> sz[0] >> sz[1] >> sz[2];
  m_OutputRegion.SetSize ( sz );
  o >> dummy >> m_OutputOrigin[0] >> m_OutputOrigin[1] >> m_OutputOrigin[2];
  o >> dummy >> m_OutputSpacing[0] >> m_OutputSpacing[1] >> m_OutputSpacing[2];
  o >> dummy;
  for ( unsigned int i = 0; i < m_OutputDirection.GetVnlMatrix().rows(); i++ )
  {
    for ( unsigned int j = 0; j < m_OutputDirection.GetVnlMatrix().cols(); j++ )
    {
      o >> m_OutputDirection[i][j];
    }
  }
  std::cout << "ImageSize: " << m_OutputRegion.GetSize() << endl;
  std::cout << "ImageOrigin: " << m_OutputOrigin << endl;
  std::cout << "ImageSpacing: " << m_OutputSpacing << endl;
  std::cout << "Direction: " << m_OutputDirection << endl;
}

  
  template <class TPixel>
streampos TimeSeriesDatabase<TPixel>::CalculatePosition ( Array<unsigned int> Position )
{
  streampos mult = 1;
  streampos p = 0;
  for ( int i = 0; i < Position.Size(); i++ ) {
    p += Position[i] * mult;
    mult = mult * TimeSeriesBlockSize;
  }
  return p;
}

template <class TPixel>
unsigned long TimeSeriesDatabase<TPixel>::CalculateIndex ( Size<3> p, int ImagePosition )
{
  // Remember that we use the first 16K as our header
  unsigned long position = 1 + p[0] 
    + p[1] * this->m_BlocksPerImage[0]
    + p[2] * this->m_BlocksPerImage[0] * this->m_BlocksPerImage[1]
    + ImagePosition * this->m_BlocksPerImage[0] * this->m_BlocksPerImage[1] * this->m_BlocksPerImage[2];
  return position;
}


template <class TPixel>
streampos TimeSeriesDatabase<TPixel>::CalculatePosition ( Size<3> p, int ImagePosition )
{
  streampos position = this->CalculateIndex ( p, ImagePosition );
  position = position * sizeof ( TPixel ) * TimeSeriesVolumeBlockSize;
  return position;
}

template <class TPixel>
streampos TimeSeriesDatabase<TPixel>::CalculatePosition ( unsigned long p )
{
  streampos position = p * sizeof ( TPixel ) * TimeSeriesVolumeBlockSize;
  return position;
}


template <class TPixel>
typename TimeSeriesDatabase<TPixel>::CacheBlock* TimeSeriesDatabase<TPixel>::GetCacheBlock ( unsigned long index )
{
  CacheBlock* Buffer = this->m_Cache.find ( index );
  if ( Buffer == 0 ) {
    // Fill it in
    CacheBlock B;
    this->m_File.seekg ( this->CalculatePosition ( index ) );
    this->m_File.read ( reinterpret_cast<char*> ( B.data ), TimeSeriesVolumeBlockSize * sizeof ( TPixel ) );
    this->m_Cache.insert ( index, B );
    Buffer = this->m_Cache.find ( index );
  }
  return Buffer;
}


template <class TPixel>
void TimeSeriesDatabase<TPixel>::GetVoxelTimeSeries ( typename OutputImageType::IndexType idx, ArrayType& array )
{
  // See if the index is inside the volume
  // and figure out which cache block we need
  Size<3> CurrentBlock;
  Size<3> Offset;
  for ( int i = 0; i < 3; i++ ) {
    if ( idx[i] < 0 || idx[i] > this->m_OutputRegion.Size[i] ) {
      throw 1;
    }
    CurrentBlock[i] = reinterpret_cast<unsigned long> ( idx[i] / (double)TimeSeriesBlockSize );
    Offset[i] = idx[i] % TimeSeriesBlockSize;
  }
  unsigned long offset = Offset[0] + Offset[1] * TimeSeriesBlockSize + Offset[2] * TimeSeriesBlockSizeP2;
  array = ArrayType ( this->m_Dimensions[3] );
  for ( int volume = 0; volume < this->m_Dimensions[3]; volume++ ) {
    CacheBlock* cache = this->GetCacheBlock ( CurrentBlock, volume );
    array[volume] = cache->data[offset];
  }
}


template <class TPixel>
void TimeSeriesDatabase<TPixel>::GenerateOutputInformation ( ) 
{
  typename OutputImageType::Pointer output = this->GetOutput();
  output->SetSpacing ( this->m_OutputSpacing );     
  output->SetOrigin ( this->m_OutputOrigin );       
  output->SetDirection ( this->m_OutputDirection ); 
  output->SetLargestPossibleRegion ( this->m_OutputRegion );
}  

template <class TPixel>
void TimeSeriesDatabase<TPixel>::GenerateData() 
{
  typename OutputImageType::Pointer output = this->GetOutput();
  typename OutputImageType::RegionType Region;
  itkDebugMacro ( << "TimeSeriesDatabase::GenerateData()  Allocating" );
  cout << "Requested region " << output->GetRequestedRegion();
  Region = output->GetRequestedRegion();
  output->SetBufferedRegion( output->GetLargestPossibleRegion() );
  output->Allocate();

  if ( !this->IsOpen() )
  {
    itkGenericExceptionMacro ( "TimeSeriesDatabase::GenerateOutputInformation: not open for reading" );
  }

  Size<3> BlockStart, BlockCount;
  for ( unsigned int i = 0; i < 3; i++ ) {
    BlockStart[i] = (int) floor ( Region.GetIndex(i) / TimeSeriesBlockSize );
    BlockCount[i] = (int) TSD_MAX ( 1.0, ceil ( (Region.GetIndex(i)+Region.GetSize(i)) / (double)TimeSeriesBlockSize ) );
  }
  std::cout << "Block Start: " << BlockStart << endl;
  std::cout << "Block Count: " << BlockCount << endl;


  Size<3> CurrentBlock;
  // Now, read our data, caching as we go, in future, make this thread safe
  Size<3> BlockSize = { TimeSeriesBlockSize, TimeSeriesBlockSize, TimeSeriesBlockSize };
  ImageRegion<3> BlockRegion;
  BlockRegion.SetSize ( BlockSize );
  // Fetch only the blocks we need
  for ( CurrentBlock[2] = BlockStart[2]; CurrentBlock[2] < BlockStart[2] + BlockCount[2]; CurrentBlock[2]++ ) {
    for ( CurrentBlock[1] = BlockStart[1]; CurrentBlock[1] < BlockStart[1] + BlockCount[1]; CurrentBlock[1]++ ) {
      for ( CurrentBlock[0] = BlockStart[0]; CurrentBlock[0] < BlockStart[0] + BlockCount[0]; CurrentBlock[0]++ ) {
        typename OutputImageType::RegionType BR, IR;
        std::cout << "For Block Index: " << CurrentBlock << std::endl;
        unsigned long index = this->CalculateIndex ( CurrentBlock, this->m_CurrentImage );
        CacheBlock* Buffer = this->GetCacheBlock ( index );
        if ( this->CalculateIntersection ( CurrentBlock, Region, BR, IR ) ) {
          // Just iterate over whole block
          // Good we can use an iterator!
          Index<3> BlockIndex = { CurrentBlock[0] * TimeSeriesBlockSize,  CurrentBlock[1] * TimeSeriesBlockSize,  CurrentBlock[2] * TimeSeriesBlockSize };
          BlockRegion.SetIndex ( BlockIndex );
          ImageRegionIterator<OutputImageType> it ( output, IR );
          it.GoToBegin();
          TPixel* ptr = Buffer->data;
          while ( !it.IsAtEnd() ) {
            it.Set ( *ptr );
            ++it;
            ++ptr;
          }
        } else {
          // cout << "The Hard way" << std::endl;
          // Now we do it the hard way...
          Index<3> ImageIndex;
          Size<3> StartIndex, EndIndex;
          for ( int i = 0; i < 3; i++ ) {
            StartIndex[i] = BR.GetIndex(i);
            EndIndex[i] = BR.GetIndex(i) + BR.GetSize(i);
          }
          std::cout << "Start Index: " << StartIndex << std::endl;
          std::cout << "End Index: " << EndIndex << std::endl;
          for ( int bz = StartIndex[2]; bz < EndIndex[2]; bz++ ) {
            ImageIndex[2] = IR.GetIndex(2) + bz;
            for ( int by = StartIndex[1]; by < EndIndex[1]; by++ ) {
              ImageIndex[1] = IR.GetIndex(1) + by;
              for ( int bx = StartIndex[0]; bx < EndIndex[0]; bx++ ) {
                // Put bx,by,bz into bx-xoff,by-yoff,bz-zoff
                ImageIndex[0] = IR.GetIndex(0) + bx;
                output->SetPixel ( ImageIndex, Buffer->data[bx + TimeSeriesBlockSize*by + TimeSeriesBlockSize*TimeSeriesBlockSize*bz] );
                }
              }
            }
          }
        }
      }
    }
  
#if 0
  for ( CurrentBlock[2] = 0; CurrentBlock[2] < this->m_BlocksPerImage[2]; CurrentBlock[2]++ ) {
    for ( CurrentBlock[1] = 0; CurrentBlock[1] < this->m_BlocksPerImage[1]; CurrentBlock[1]++ ) {
      for ( CurrentBlock[0] = 0; CurrentBlock[0] < this->m_BlocksPerImage[0]; CurrentBlock[0]++ ) {


        // Seek to the block we need
        unsigned long index = this->CalculateIndex ( CurrentBlock, this->m_CurrentImage );
        CacheBlock* Buffer = this->GetCacheBlock ( index );
        // Write into the output
        // Load up the block, and save it at the proper index
        // Is this block fully within the image?
        if ( ( ( CurrentBlock[0] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < this->m_OutputRegion.GetSize()[0] )
             && ( ( CurrentBlock[1] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < this->m_OutputRegion.GetSize()[1] )
             && ( ( CurrentBlock[2] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < this->m_OutputRegion.GetSize()[2] ) ) {
          // Good we can use an iterator!
          Index<3> BlockIndex = { CurrentBlock[0] * TimeSeriesBlockSize,  CurrentBlock[1] * TimeSeriesBlockSize,  CurrentBlock[2] * TimeSeriesBlockSize };
          BlockRegion.SetIndex ( BlockIndex );
          ImageRegionIterator<OutputImageType> it ( output, BlockRegion );
          it.GoToBegin();
          TPixel* ptr = Buffer->data;
          while ( !it.IsAtEnd() ) {
            it.Set ( *ptr );
            ++it;
            ++ptr;
          }
        } else {
          // cout << "The Hard way" << std::endl;
          // Now we do it the hard way...
          Index<3> BlockIndex;
          Size<3> StartIndex, EndIndex;
          for ( int i = 0; i < 3; i++ ) {
            StartIndex[i] = CurrentBlock[i]*TimeSeriesBlockSize;
            EndIndex[i] = TSD_MIN ( StartIndex[i] + TimeSeriesBlockSize, this->m_OutputRegion.GetSize()[i] );
          }            
          for ( int bz = StartIndex[2]; bz < EndIndex[2]; bz++ ) {
            BlockIndex[2] = bz;
            for ( int by = StartIndex[1]; by < EndIndex[1]; by++ ) {
              BlockIndex[1] = by;
              for ( int bx = StartIndex[0]; bx < EndIndex[0]; bx++ ) {
                // Put bx,by,bz into bx-xoff,by-yoff,bz-zoff
                BlockIndex[0] = bx;
                output->SetPixel ( BlockIndex, Buffer->data[bx-StartIndex[0] + TimeSeriesBlockSize*(by-StartIndex[1]) + TimeSeriesBlockSize*TimeSeriesBlockSize*(bz-StartIndex[2])]  );
              }
            }
          }
        }
      }
    }
  }
#endif  
  return;
}
  



template <class TPixel>
void TimeSeriesDatabase<TPixel>::CreateFromFileArchetype ( const char* filename, const char* archetype )
{
  std::vector<std::string> candidateFiles;
  std::string fileNameCollapsed = itksys::SystemTools::CollapseFullPath( archetype);
  if (!itksys::SystemTools::FileExists (fileNameCollapsed.c_str()))  {
      itkGenericExceptionMacro ( "TimeSeriesDatabase::CreateFromFileArchetype: Archetype file " << fileNameCollapsed.c_str() << " does not exist.");
      return;
    }
  ArchetypeSeriesFileNames::Pointer fit = itk::ArchetypeSeriesFileNames::New();
  fit->SetArchetype ( fileNameCollapsed );
  
  typedef Image<TPixel,3> ImageType;
  typedef ImageFileReader<ImageType> ReaderType;
  ImageRegion<3> region;

  // Load the first image's size
  candidateFiles = fit->GetFileNames();
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName ( fileNameCollapsed );
  reader->UpdateOutputInformation();
  region = reader->GetOutput()->GetLargestPossibleRegion();
  this->m_Dimensions[0] = region.GetSize()[0];
  this->m_Dimensions[1] = region.GetSize()[1];
  this->m_Dimensions[2] = region.GetSize()[2];
  this->m_Dimensions[3] = candidateFiles.size();

  this->m_OutputRegion = reader->GetOutput()->GetLargestPossibleRegion();
  this->m_OutputSpacing = reader->GetOutput()->GetSpacing();
  this->m_OutputOrigin = reader->GetOutput()->GetOrigin();
  this->m_OutputDirection = reader->GetOutput()->GetDirection();

  fstream db;
  std::cout << "Opening " << filename << std::endl;
  db.open ( filename, ios::out | ios::binary );
  this->m_Filename = filename;

  // Start reading and writing out the images, 16x16x16 blocks at a time.
  for ( unsigned int i = 0; i < candidateFiles.size(); i++ ) {
    reader->SetFileName ( itksys::SystemTools::CollapseFullPath ( candidateFiles[i].c_str() ) );
    try {
      reader->Update();
    } catch ( ExceptionObject& e ) {
      itkExceptionMacro ( << "Failed to read " << candidateFiles[i] << " caught " << e );
    }
    // Verify that we have the same size as expected
    region = reader->GetOutput()->GetLargestPossibleRegion();
    if ( this->m_Dimensions[0] != region.GetSize()[0]
         || this->m_Dimensions[1] != region.GetSize()[1]
         || this->m_Dimensions[2] != region.GetSize()[2] ) {
      db.close();
      itkExceptionMacro ( << " size of the data in " << candidateFiles[i] << " is (" 
                          << region.GetSize()[0] << ", "
                          << region.GetSize()[1] << ", "
                          << region.GetSize()[2] << ") "
                          << " and does not match the expectected size ("
                          << this->m_Dimensions[0] << ", "
                          << this->m_Dimensions[1] << ", "
                          << this->m_Dimensions[2] << ")" );
    }

    // Build and write our blocks
    TPixel buffer[TimeSeriesBlockSize*TimeSeriesBlockSize*TimeSeriesBlockSize];
    Size<3> BlockSize = { TimeSeriesBlockSize, TimeSeriesBlockSize, TimeSeriesBlockSize };
    ImageRegion<3> BlockRegion;
    BlockRegion.SetSize ( BlockSize );
    for ( int idx = 0; idx < 3; idx++ ) {
      this->m_BlocksPerImage[idx] = (unsigned int) ceil ( this->m_Dimensions[idx] / (float)TimeSeriesBlockSize );
    }
    Size<3> CurrentBlock;
    for ( CurrentBlock[2] = 0; CurrentBlock[2] < this->m_BlocksPerImage[2]; CurrentBlock[2]++ ) {
      for ( CurrentBlock[1] = 0; CurrentBlock[1] < this->m_BlocksPerImage[1]; CurrentBlock[1]++ ) {
        for ( CurrentBlock[0] = 0; CurrentBlock[0] < this->m_BlocksPerImage[0]; CurrentBlock[0]++ ) {
          /*
          std::cout << "Reading/Writing Block: " << CurrentBlock[0] << ", " << CurrentBlock[1] << ", " << CurrentBlock[2] << endl;
          std::cout << "Debug: " << ( ( CurrentBlock[2] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < region.GetSize()[2] ) << endl;
          std::cout << "Debug: " << CurrentBlock[2] * TimeSeriesBlockSize + TimeSeriesBlockSize << " Region size  " << region.GetSize()[2] << endl;
          */
          // Load up the block, and save it at the proper index
          // Is this block fully within the image?
          if ( ( ( CurrentBlock[0] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < region.GetSize()[0] )
               && ( ( CurrentBlock[1] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < region.GetSize()[1] )
               && ( ( CurrentBlock[2] * TimeSeriesBlockSize + TimeSeriesBlockSize ) < region.GetSize()[2] ) ) {

            // Good we can use an iterator!
            Index<3> BlockIndex = { CurrentBlock[0] * TimeSeriesBlockSize,  CurrentBlock[1] * TimeSeriesBlockSize,  CurrentBlock[2] * TimeSeriesBlockSize };
            BlockRegion.SetIndex ( BlockIndex );
            ImageRegionConstIterator<ImageType> it ( reader->GetOutput(), BlockRegion );
            it.GoToBegin();
            TPixel* ptr = buffer;
            while ( !it.IsAtEnd() ) {
              *ptr = it.Value();
              ++it;
              ++ptr;
            }
          } else {

            // cout << "The Hard way" << std::endl;
            // Now we do it the hard way...
            Index<3> BlockIndex;
            Size<3> StartIndex, EndIndex;
            for ( int i = 0; i < 3; i++ ) {
              StartIndex[i] = CurrentBlock[i]*TimeSeriesBlockSize;
              EndIndex[i] = TSD_MIN ( StartIndex[i] + TimeSeriesBlockSize, region.GetSize()[i] );
            }            
            for ( int bz = StartIndex[2]; bz < EndIndex[2]; bz++ ) {
              BlockIndex[2] = bz;
              for ( int by = StartIndex[1]; by < EndIndex[1]; by++ ) {
                BlockIndex[1] = by;
                for ( int bx = StartIndex[0]; bx < EndIndex[0]; bx++ ) {
                  // Put bx,by,bz into bx-xoff,by-yoff,bz-zoff
                  BlockIndex[0] = bx;
                  TPixel value = reader->GetOutput()->GetPixel ( BlockIndex );
                  buffer[bx-StartIndex[0] + TimeSeriesBlockSize*(by-StartIndex[1]) + TimeSeriesBlockSize*TimeSeriesBlockSize*(bz-StartIndex[2])] = value;
                  // std::cout << "Reading pixel index: " << BlockIndex << " into " << bx-StartIndex[0] << ", " << by-StartIndex[1] << ", " << bz-StartIndex[2] << endl;
                }
              }
            }
          }
          db.seekp ( this->CalculatePosition ( CurrentBlock, i ) );
          db.write ( reinterpret_cast<char*> ( buffer ), TimeSeriesBlockSize*TimeSeriesBlockSize*TimeSeriesBlockSize*sizeof(TPixel) );
        }
      }
    }
  }
  // Write the header
  db.seekp ( 0 );
  std::ostringstream b;
  b << "TimeSeriesDatabase" << endl;
  b << "Version 1.0" << endl;
  b << "ImageSize: " << m_OutputRegion.GetSize()[0] << " "<< m_OutputRegion.GetSize()[1] << " " << m_OutputRegion.GetSize()[2] << std::endl;
  b << "ImageOrigin: " << m_OutputOrigin[0] << " " << m_OutputOrigin[1] << " " << m_OutputOrigin[2] << std::endl;
  b << "ImageSpacing: " << m_OutputSpacing[0] << " " << m_OutputSpacing[1] << " " << m_OutputSpacing[2] << std::endl;
  b << "Direction: ";
  for ( unsigned int i = 0; i < 3; i++ )
  {
    for ( unsigned int j = 0; j < 3; j++ )
    {
      b << m_OutputDirection.GetVnlMatrix()[i][j] << " ";
    }
  }
  b << endl;
  // std::cout << b.str() << endl;
  db.write ( b.str().c_str(), strlen ( b.str().c_str() ) );
  db.close();
}

template <class TPixel>
TimeSeriesDatabase<TPixel>::TimeSeriesDatabase () : m_Cache ( 16384 ){
  this->m_Dimensions.SetSize ( 4 );
  this->m_BlocksPerImage.SetSize ( 4 );
}
  
template <class TPixel>
TimeSeriesDatabase<TPixel>::~TimeSeriesDatabase () {
}
  

template <class TPixel>
void
TimeSeriesDatabase<TPixel>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Dimensions: " << m_Dimensions << "\n";
  os << indent << "Database: " << m_Filename << "\n";
  os << indent << "BlocksPerImage: " << m_BlocksPerImage << "\n";
  os << indent << "OutputSpacing: " << m_OutputSpacing << "\n";
  os << indent << "OutputRegion: " << m_OutputRegion;
  os << indent << "OutputOrigin: " << m_OutputOrigin << "\n";
  os << indent << "OutputDirection: " << m_OutputDirection << "\n";
  if ( this->IsOpen() ) {
    os << indent << "Database is open." << "\n";
  } else {
    os << indent << "Database is closed." << "\n";
  }
}


}
