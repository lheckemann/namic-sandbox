

#include <itkImage.h>

#include <itkTimeSeriesDatabase.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>


int main ( int argc, char* argv[] ) {

  if ( argc != 3 ) {
    std::cerr << "Usage: TimeSeriesDatabase <DatabaseFile> <ArchetypeFile>" << std::endl;
    exit ( EXIT_FAILURE );
  }
  
  
  typedef itk::TimeSeriesDatabase<signed short> DBType;
  DBType::Pointer db = DBType::New();
  db->CreateFromFileArchetype ( argv[1], argv[2] );
  db->Print ( std::cout );

  std::cout << "Wrote db" << std::endl;
  db->Connect ( argv[1] );
  
  db->SetCurrentImage ( 3 );
  db->Update();
  DBType::OutputImageType::Pointer FullImage = db->GetOutput();
  FullImage->DisconnectPipeline();
  

  DBType::OutputImageType::RegionType region;
  itk::Size<3> sz = { 64, 1, 93 };
  region.SetSize ( sz );
  itk::Index<3> index = { 0, 1, 0 };
  region.SetIndex ( index );
  db->GetOutput()->SetRequestedRegion ( region );
  db->Update();

  // Compare
  itk::ImageRegionIteratorWithIndex<DBType::OutputImageType> SliceIterator ( db->GetOutput(), region );
  itk::ImageRegionIterator<DBType::OutputImageType> VolumeIterator ( FullImage, region );

  while ( !SliceIterator.IsAtEnd() ) {
    if ( SliceIterator.Get() != VolumeIterator.Get() ) {
      std::cerr << "Error: not the same at index: " << SliceIterator.GetIndex() << std::endl;
      exit ( EXIT_FAILURE );
    }
    ++SliceIterator;
    ++VolumeIterator;
  }
  


  typedef itk::ImageFileWriter<DBType::OutputImageType> WriterType;
  WriterType::Pointer Writer = WriterType::New();
  Writer->SetFileName ( "Test.hdr" );
  Writer->SetInput ( db->GetOutput() );
  Writer->Update();
  db->Disconnect();
}
