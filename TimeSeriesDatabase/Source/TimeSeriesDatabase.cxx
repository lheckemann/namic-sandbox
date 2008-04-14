

#include <itkImage.h>

#include <itkTimeSeriesDatabase.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkMersenneTwisterRandomVariateGenerator.h>

template<class ImageType>
bool CompareImage ( typename ImageType::Pointer a, typename ImageType::Pointer b, typename ImageType::RegionType region ) {
  itk::ImageRegionIteratorWithIndex<ImageType> SliceIterator ( a, region );
  itk::ImageRegionIterator<ImageType> VolumeIterator ( b, region );
      
  // std::cout << "Comparing region:\n" << region << std::endl;
  while ( !SliceIterator.IsAtEnd() ) {
    if ( SliceIterator.Get() != VolumeIterator.Get() ) {
      std::cerr << "\nRegion is: " << region << std::endl;
      std::cerr << "Requsted Region is: " << a->GetRequestedRegion() << std::endl;
      std::cerr << "Error: not the same at index: " << SliceIterator.GetIndex() << std::endl;
      std::cerr << "Error: not the same at index: " << VolumeIterator.GetIndex() << std::endl;
      std::cerr << "Slice is: " << SliceIterator.Get() << std::endl;
      std::cerr << "Volume is: " << VolumeIterator.Get() << std::endl;
      return false;
    }
    ++SliceIterator;
    ++VolumeIterator;
  }
  return true;
}

int main ( int argc, char* argv[] ) {

  if ( argc < 3 ) {
    std::cerr << "Usage: TimeSeriesDatabase <DatabaseFile> <ArchetypeFile> [NumberOfTests]" << std::endl;
    exit ( EXIT_FAILURE );
  }
  int NumberOfTests = 1000;
  if ( argc > 3 ) {
    NumberOfTests = atoi ( argv[3] );
  }
  
  typedef itk::TimeSeriesDatabase<signed short> DBType;
  DBType::Pointer db = DBType::New();
  db->CreateFromFileArchetype ( argv[1], argv[2] );
  db->Print ( std::cout );

  std::cout << "Wrote db" << std::endl;
  db->Connect ( argv[1] );
  
  db->SetCurrentImage ( 0 );
  db->Update();
  DBType::OutputImageType::Pointer FullImage = db->GetOutput();
  FullImage->DisconnectPipeline();
  

  DBType::OutputImageType::RegionType region;
  itk::Size<3> sz;
  itk::Index<3> index;

  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer twister = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();
  twister->Initialize ( 12345 );
  itk::Size<3> OutputSize = db->GetOutputRegion().GetSize();

  for ( int idx = 0; idx < db->GetNumberOfVolumes(); idx++ ) {
    db->SetCurrentImage ( idx );

    // Test out all the slices
    int x;
    sz = db->GetOutputRegion().GetSize();
    index = db->GetOutputRegion().GetIndex();
    sz[2] = 1;
    region.SetSize ( sz );

    // Get all the axials
    for ( index[2] = 0; index[2] < OutputSize[2]; index[2]++ ) {
      region.SetIndex ( index );
      db->GetOutput()->SetRequestedRegion ( region );
      db->Modified();
      db->Update();
      if ( !CompareImage<DBType::OutputImageType> ( db->GetOutput(), FullImage, region ) ) {
        exit ( EXIT_FAILURE );
      }
    }

    sz = db->GetOutputRegion().GetSize();
    index = db->GetOutputRegion().GetIndex();
    sz[1] = 1;
    region.SetSize ( sz );

    // Get all the coronals
    for ( index[1] = 0; index[1] < OutputSize[1]; index[1]++ ) {
      region.SetIndex ( index );
      db->GetOutput()->SetRequestedRegion ( region );
      db->Modified();
      db->Update();
      if ( !CompareImage<DBType::OutputImageType> ( db->GetOutput(), FullImage, region ) ) {
        exit ( EXIT_FAILURE );
      }
    }

    sz = db->GetOutputRegion().GetSize();
    index = db->GetOutputRegion().GetIndex();
    sz[0] = 1;
    region.SetSize ( sz );

    // Get all the coronals
    for ( index[0] = 0; index[0] < OutputSize[0]; index[0]++ ) {
      region.SetIndex ( index );
      db->GetOutput()->SetRequestedRegion ( region );
      db->Modified();
      db->Update();
      if ( !CompareImage<DBType::OutputImageType> ( db->GetOutput(), FullImage, region ) ) {
        exit ( EXIT_FAILURE );
      }
    }


    for ( int TestCount = 0; TestCount < NumberOfTests; TestCount++ ) {
      index[0] = twister->GetIntegerVariate ( db->GetOutputRegion().GetSize()[0] - 2 );
      index[1] = twister->GetIntegerVariate ( db->GetOutputRegion().GetSize()[1] - 2 );
      index[2] = twister->GetIntegerVariate ( db->GetOutputRegion().GetSize()[2] - 2 );
      sz[0] = 1 + twister->GetIntegerVariate ( db->GetOutputRegion().GetSize()[0] - index[0] - 2 );
      sz[1] = 1 + twister->GetIntegerVariate ( db->GetOutputRegion().GetSize()[1] - index[1] - 2 );
      sz[2] = 1 + twister->GetIntegerVariate ( db->GetOutputRegion().GetSize()[2] - index[2] - 2 );
      
      region.SetSize ( sz );
      region.SetIndex ( index );
      db->GetOutput()->SetRequestedRegion ( region );
      db->Modified();
      db->Update();
      // Compare
      if ( !CompareImage<DBType::OutputImageType> ( db->GetOutput(), FullImage, region ) ) {
        exit ( EXIT_FAILURE );
      }
    }
  }
  std::cout << "Sucessfully completed " << NumberOfTests << " tests per volume" << std::endl;


  typedef itk::ImageFileWriter<DBType::OutputImageType> WriterType;
  WriterType::Pointer Writer = WriterType::New();
  Writer->SetFileName ( "Test.hdr" );
  db->GetOutput()->SetRequestedRegion ( db->GetOutputRegion() );
  Writer->SetInput ( db->GetOutput() );
  Writer->Update();
  db->Disconnect();

  db->Print ( std::cout );
  exit ( EXIT_SUCCESS );
}
