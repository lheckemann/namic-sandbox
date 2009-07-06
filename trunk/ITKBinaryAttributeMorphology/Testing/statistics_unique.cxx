#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSimpleFilterWatcher.h"

#include "itkLabelObject.h"
#include "itkLabelMap.h"
#include "itkBinaryImageToStatisticsLabelMapFilter.h"
#include "itkObjectByObjectLabelMapFilter.h"
#include "itkStatisticsUniqueLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"

#include "itkBinaryDilateImageFilter.h"
#include "itkFlatStructuringElement.h"


int main(int argc, char * argv[])
{

  if( argc != 4 )
    {
    std::cerr << "usage: " << argv[0] << " input output reverse" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  typedef itk::Image< unsigned char, dim > ImageType;

  typedef itk::StatisticsLabelObject< unsigned char, dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryImageToStatisticsLabelMapFilter< ImageType, ImageType, LabelMapType> I2LType;
  I2LType::Pointer i2l = I2LType::New();
  i2l->SetInput( reader->GetOutput() );
  i2l->SetFeatureImage( reader->GetOutput() );
  i2l->SetFullyConnected( true );

  typedef itk::FlatStructuringElement< dim > KernelType;
  typedef itk::BinaryDilateImageFilter< ImageType, ImageType, KernelType > CastType;
  CastType::Pointer cast = CastType::New();
  KernelType::SizeType rad;
  rad.Fill( 0 );
  rad[0] = 2;
  cast->SetKernel( KernelType::Ball( rad ) );
  
  typedef itk::ObjectByObjectLabelMapFilter< LabelMapType, LabelMapType, CastType > OIType;
  OIType::Pointer oi = OIType::New();
  oi->SetInput( i2l->GetOutput() );
  oi->SetFilter( cast );
  oi->SetPadSize( rad );
//  oi->SetKeepLabels( false );
//  oi->SetBinaryInternalOutput( false );  
//  oi->Update();
//  oi->GetOutput()->PrintLabelObjects();

  typedef itk::StatisticsUniqueLabelMapFilter< LabelMapType > UniqueType;
  UniqueType::Pointer unique = UniqueType::New();
  unique->SetInput( oi->GetOutput() );
  unique->SetReverseOrdering( atoi(argv[3]) );
  unique->SetAttribute( "Sum" );
  itk::SimpleFilterWatcher watcher(unique, "filter");
  
  typedef itk::LabelMapToLabelImageFilter< LabelMapType, ImageType> L2IType;
  L2IType::Pointer l2i = L2IType::New();
  l2i->SetInput( unique->GetOutput() );

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( l2i->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();

  return 0;
}

