

#include <itkImage.h>

#include <itkTimeSeriesDatabase.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkMersenneTwisterRandomVariateGenerator.h>

template<class ImageType>
bool CompareImage ( typename ImageType::Pointer a, typename ImageType::Pointer b, typename ImageType::RegionType region ) {
  itk::ImageRegionIteratorWithIndex<ImageType> ait ( a, region );
  itk::ImageRegionIterator<ImageType> bit ( b, region );
      
  // std::cout << "Comparing region:\n" << region << std::endl;
  while ( !ait.IsAtEnd() ) {
    if ( ait.Get() != bit.Get() ) {
      std::cerr << "\nRegion is: " << region << std::endl;
      std::cerr << "Requsted Region is: " << a->GetRequestedRegion() << std::endl;
      std::cerr << "Error: not the same at index: " << ait.GetIndex() << std::endl;
      std::cerr << "Ground truth is: " << ait.Get() << std::endl;
      std::cerr << "But found: " << bit.Get() << std::endl;
      return false;
    }
    ++ait;
    ++bit;
  }
  return true;
}

int main ( int argc, char* argv[] ) {

  if ( argc < 3 ) {
    std::cerr << "Usage: TimeSeriesDatabase <DatabaseFile> <ArchetypeFile> [NumberOfTests]" << std::endl;
    exit ( EXIT_FAILURE );
  }

  std::fstream f ( "/tmp/Test.txt" );

  typedef itk::TimeSeriesDatabaseHelper::counted_ptr<std::fstream> fstreamPtr;

  std::vector<fstreamPtr> sv;

  sv.push_back ( fstreamPtr ( new std::fstream ( "/tmp/Test.txt", ::std::ios::out | ::std::ios::binary ) ) );

  *sv[0] << "Foo bar" << std::endl;
  sv[0]->close();
  sv.pop_back();

  int NumberOfTests = 1000;
  if ( argc > 3 ) {
    NumberOfTests = atoi ( argv[3] );
  }
  
  typedef itk::TimeSeriesDatabase<signed short> DBType;
  // Create a series of 1MiB files
  // 1048576 is 1 MiB
  DBType::CreateFromFileArchetype ( argv[1], argv[2], 1048576 );
  std::cout << "Wrote db" << std::endl;

  DBType::Pointer db = DBType::New();

  db->Connect ( argv[1] );
  
  db->SetCurrentImage ( 0 );
  db->Update();

  db->Print ( std::cout );

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

    db->GetOutput()->SetRequestedRegion ( FullImage->GetLargestPossibleRegion() );
    db->Modified();
    db->Update();
    if ( !CompareImage<DBType::OutputImageType> ( FullImage, db->GetOutput(), FullImage->GetLargestPossibleRegion() ) ) {
      std::cerr << "Failed to compare whole volume " << idx << std::endl;
      exit ( EXIT_FAILURE );
    }

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
      if ( !CompareImage<DBType::OutputImageType> ( FullImage, db->GetOutput(), region ) ) {
        std::cerr << "Failed to compare axial slice " << index[2] << " of volume " << idx << std::endl;
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
      if ( !CompareImage<DBType::OutputImageType> ( FullImage, db->GetOutput(), region ) ) {
        std::cerr << "Failed to compare coronal slice " << index[1] << " of volume " << idx << std::endl;
        exit ( EXIT_FAILURE );
      }
    }

    sz = db->GetOutputRegion().GetSize();
    index = db->GetOutputRegion().GetIndex();
    sz[0] = 1;
    region.SetSize ( sz );

    // Get all the sagittal
    for ( index[0] = 0; index[0] < OutputSize[0]; index[0]++ ) {
      region.SetIndex ( index );
      db->GetOutput()->SetRequestedRegion ( region );
      db->Modified();
      db->Update();
      if ( !CompareImage<DBType::OutputImageType> ( FullImage, db->GetOutput(), region ) ) {
        std::cerr << "Failed to compare sagittal slice " << index[0] << " of volume " << idx << std::endl;
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
      if ( !CompareImage<DBType::OutputImageType> ( FullImage, db->GetOutput(), region ) ) {
        std::cerr << "Failed to compare random region of volume " << idx << std::endl;
        exit ( EXIT_FAILURE );
      }
    }
  }
  db->Print ( std::cout );
  std::cout << "Sucessfully completed " << NumberOfTests << " tests per volume" << std::endl;


  typedef itk::ImageFileWriter<DBType::OutputImageType> WriterType;
  WriterType::Pointer Writer = WriterType::New();
  Writer->SetFileName ( "Test.hdr" );
  db->GetOutput()->SetRequestedRegion ( db->GetOutputRegion() );
  Writer->SetInput ( db->GetOutput() );
  Writer->Update();


  db->Disconnect();

  exit ( EXIT_SUCCESS );
}
