
#include "itkImage.h" 
#include "itkImageRegionIterator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h" 

int main( int argc, char *argv[] )
{
  // Verify the number of parameters on the command line.
  if ( argc < 15 )
    {
      std::cerr << "Missing parameters. " << std::endl;
      std::cerr << "Usage: " << std::endl;
      std::cerr << argv[0]
                << "16 label maps"
                << std::endl;
      return -1;
    }

  const int Dimension = 3; 
  typedef   short    MaskPixelType;
   
  typedef itk::Image< MaskPixelType, Dimension >   MaskType;  
  typedef itk::ImageRegionIterator< MaskType > MaskIteratorType; 
  typedef itk::ImageFileReader< MaskType >  MaskReaderType; 
  typedef itk::ImageFileWriter< MaskType >  MaskWriterType;

  MaskReaderType::Pointer ICCreader = MaskReaderType::New();
  ICCreader->SetFileName( argv[1] );
  ICCreader->Update();
  
  MaskReaderType::Pointer cerereader = MaskReaderType::New();
  cerereader->SetFileName( argv[2] );
  cerereader->Update();

  MaskReaderType::Pointer RCaudatereader = MaskReaderType::New();
  RCaudatereader->SetFileName( argv[3] );
  RCaudatereader->Update();

  MaskReaderType::Pointer LCaudatereader = MaskReaderType::New();
  LCaudatereader->SetFileName( argv[4] );
  LCaudatereader->Update();

  MaskReaderType::Pointer RPutamenreader = MaskReaderType::New();
  RPutamenreader->SetFileName( argv[5] );
  RPutamenreader->Update();

  MaskReaderType::Pointer LPutamenreader = MaskReaderType::New();
  LPutamenreader->SetFileName( argv[6] );
  LPutamenreader->Update();

  MaskReaderType::Pointer RGlobusPallidusreader = MaskReaderType::New();
  RGlobusPallidusreader->SetFileName( argv[7] );
  RGlobusPallidusreader->Update();

  MaskReaderType::Pointer LGlobusPallidusreader = MaskReaderType::New();
  LGlobusPallidusreader->SetFileName( argv[8] );
  LGlobusPallidusreader->Update();

  MaskReaderType::Pointer RThalamusreader = MaskReaderType::New();
  RThalamusreader->SetFileName( argv[9] );
  RThalamusreader->Update();

  MaskReaderType::Pointer LThalamusreader = MaskReaderType::New();
  LThalamusreader->SetFileName( argv[10] );
  LThalamusreader->Update();

  MaskReaderType::Pointer RHippocampusreader = MaskReaderType::New();
  RHippocampusreader->SetFileName( argv[11] );
  RHippocampusreader->Update();

  MaskReaderType::Pointer LHippocampusreader = MaskReaderType::New();
  LHippocampusreader->SetFileName( argv[12] );
  LHippocampusreader->Update();

  MaskReaderType::Pointer CSFreader = MaskReaderType::New();
  CSFreader->SetFileName( argv[13] );
  CSFreader->Update();

  MaskReaderType::Pointer WMreader = MaskReaderType::New();
  WMreader->SetFileName( argv[14] );
  WMreader->Update();

  MaskReaderType::Pointer BrainStemreader = MaskReaderType::New();
  BrainStemreader->SetFileName( argv[15] );
  BrainStemreader->Update();
 
  MaskReaderType::Pointer GMreader = MaskReaderType::New();
  GMreader->SetFileName( argv[16] );
  GMreader->Update();

  MaskType::Pointer outputImage = MaskType::New(); 
  outputImage->SetRegions( BrainStemreader->GetOutput()->GetRequestedRegion() ); 
  outputImage->CopyInformation( BrainStemreader->GetOutput() ); 
  outputImage->Allocate();

  MaskIteratorType ICCit(  ICCreader ->GetOutput(), ICCreader ->GetOutput()->GetRequestedRegion() );
  MaskIteratorType cereit(  cerereader->GetOutput(), cerereader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType RCaudateit(  RCaudatereader->GetOutput(), RCaudatereader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType LCaudateit(  LCaudatereader->GetOutput(), LCaudatereader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType RPutamenit(  RPutamenreader->GetOutput(), RPutamenreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType LPutamenit(  LPutamenreader->GetOutput(), LPutamenreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType RGlobusPallidusit(  RGlobusPallidusreader->GetOutput(), RGlobusPallidusreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType LGlobusPallidusit(  LGlobusPallidusreader->GetOutput(), LGlobusPallidusreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType RThalamusit(  RThalamusreader->GetOutput(), RThalamusreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType LThalamusit(  LThalamusreader->GetOutput(), LThalamusreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType RHippocampusit(  RHippocampusreader->GetOutput(), RHippocampusreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType LHippocampusit(  LHippocampusreader->GetOutput(), LHippocampusreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType CSFit(  CSFreader->GetOutput(), CSFreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType WMit(  WMreader->GetOutput(), WMreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType BrainStemit(  BrainStemreader->GetOutput(), BrainStemreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType GMit(  GMreader->GetOutput(), GMreader->GetOutput()->GetRequestedRegion() );
  MaskIteratorType outputImageit(  outputImage, outputImage->GetRequestedRegion() );

  ICCit.GoToBegin();
  cereit.GoToBegin();
  RCaudateit.GoToBegin();
  LCaudateit.GoToBegin();
  RPutamenit.GoToBegin();
  LPutamenit.GoToBegin();
  RGlobusPallidusit.GoToBegin();
  LGlobusPallidusit.GoToBegin();
  RThalamusit.GoToBegin();
  LThalamusit.GoToBegin();
  RHippocampusit.GoToBegin();
  LHippocampusit.GoToBegin();
  BrainStemit.GoToBegin();
  outputImageit.GoToBegin();
  WMit.GoToBegin();
  GMit.GoToBegin();
  for (CSFit.GoToBegin(); !CSFit.IsAtEnd(); ++CSFit)
  {
     bool flag = 0;
    // std::cerr<<GucciICCit.Get()<< ReviseSubCit.Get() <<std::endl; 
     if (ICCit.Get() == 0 ) {outputImageit.Set(0);flag =1;} //ECC
     else //Subc_structures
     {
         if (RCaudateit.Get() == 1){outputImageit.Set(13);flag = 1;}
         if (LCaudateit.Get() == 1){outputImageit.Set(14);flag = 1;}
         if (RPutamenit.Get() == 1){outputImageit.Set(15);flag = 1;}
         if (LPutamenit.Get() == 1){outputImageit.Set(16);flag = 1;}
         if (RGlobusPallidusit.Get() == 1){outputImageit.Set(17);flag = 1;}
         if (LGlobusPallidusit.Get() == 1){outputImageit.Set(18);flag = 1;}
         if (RThalamusit.Get() == 1){outputImageit.Set(11);flag = 1;}
         if (LThalamusit.Get() == 1){outputImageit.Set(12);flag = 1;}
         if (RHippocampusit.Get() == 1){outputImageit.Set(19);flag = 1;}
         if (LHippocampusit.Get() == 1){outputImageit.Set(20);flag = 1;}
      //   std::cerr<<"set"<< std::endl;
     }
     if (flag == 0)  //Cere & BS
     { 
         if (BrainStemit.Get()==1){outputImageit.Set(5);flag = 1;}
         if (cereit.Get()==1){outputImageit.Set(6);flag = 1;}
     }     
     if (flag == 0 && WMit.Get()==1) {outputImageit.Set(3);flag = 1;} //WM
     if (flag == 0 && CSFit.Get()==1) {outputImageit.Set(1);flag = 1;} //CSF
     if (flag == 0 && GMit.Get()==1) {outputImageit.Set(2);flag = 1;}//GM
     if (flag == 0 ) outputImageit.Set(1); //rest fill in CSF
     ++ICCit;
     ++cereit;
     ++RCaudateit;
     ++LCaudateit;
     ++RPutamenit;
     ++LPutamenit;
     ++RGlobusPallidusit;
     ++LGlobusPallidusit;
     ++RThalamusit;
     ++LThalamusit;
     ++RHippocampusit;
     ++LHippocampusit;
     ++BrainStemit;
     ++outputImageit;
     ++WMit;
     ++GMit;
  }

  MaskWriterType::Pointer writer = MaskWriterType::New();
  writer->SetFileName( argv[17] );
  
  writer->SetInput( outputImage );
  writer->Update();
  
  return 0;
}
