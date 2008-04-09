

#include <itkImage.h>

#include <itkTimeSeriesDatabase.h>
#include <itkImageFileWriter.h>


int main ( int argc, char* argv[] ) {

  typedef itk::TimeSeriesDatabase<signed short> DBType;
  DBType::Pointer db = DBType::New();
  db->CreateFromFileArchetype ( "TSD.tsd", "/Users/blezek/Source/TimeSeriesDatabase/Data/CTHeadAxial-01.nhdr" );
  db->Print ( std::cout );

  std::cout << "Wrote db" << std::endl;
  db->Connect ( "TSD.tsd" );
  
  db->SetCurrentImage ( 3 );
  DBType::OutputImageType::RegionType region;
  itk::Size<3> sz = { 64, 64, 1 };
  region.SetSize ( sz );
  db->GetOutput()->SetRequestedRegion ( region );
  db->Update();
  
  typedef itk::ImageFileWriter<DBType::OutputImageType> WriterType;
  WriterType::Pointer Writer = WriterType::New();
  Writer->SetFileName ( "Test.hdr" );
  Writer->SetInput ( db->GetOutput() );
  Writer->Update();
  db->Disconnect();
}
