#include "../randomWalkSeg.h"

#include "../cVolOp.h"

#include <ctime>
#include <string>


//void changeSeedValue(DoubleImageType::Pointer seedImage);


int main( int argc, char * argv [] )
{
  if (argc != 5)
    {
      std::cerr<<"Usage:\n"<<argv[0]<<" inputVol inputSeedVol beta outputVol\n";
      exit(-1);
    }

  cVolOp op;

  //////
  double t1, t2;
  t1 = clock();


  typedef itk::ImageFileReader< DoubleImageType > DoubleImageFileReader;

  std::cout<<"reading volume...."<<std::flush;  
  DoubleImageFileReader::Pointer reader = DoubleImageFileReader::New();
  reader->SetFileName( argv[1] );
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
  DoubleImageType::Pointer inputImage = reader->GetOutput();
  std::cout<<"done\n";



  std::cout<<"reading seed...."<<std::flush;
  DoubleImageFileReader::Pointer seedReader = DoubleImageFileReader::New();
  seedReader->SetFileName( argv[2] );
  try 
    {
      seedReader->Update();
    }
  catch ( itk::ExceptionObject &err)   
    {   
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }
  DoubleImageType::Pointer seedImage = seedReader->GetOutput();
  //  changeSeedValue(seedImage);
  std::cout<<"done\n"<<std::flush;

  randomWalkSeg rw;

  //  rw.setVol(volSmth);
  rw.setInput(inputImage);
  rw.setSeedImage(seedImage);

  int numIter = 1000;
  rw.set_solver_num_iter(numIter);

  double solverErrTol = 1e-8;
  rw.set_solver_err_tol(solverErrTol);

  double rw_beta = atof(argv[3]);
  rw.setBeta(rw_beta);
  // std::cout<<"beta value is "<<rw_beta<<std::endl<<std::flush;

  std::cout<<"computing result...."<<std::flush;
  rw.update();
  std::cout<<"done\n"<<std::flush;

  std::cout<<"smoothing result potential volume...."<<std::flush;
  DoubleImageType::Pointer potential_image = rw.getOutputImage();

  std::cout<<"writing result volume...."<<std::flush;



  typedef itk::ImageFileWriter< DoubleImageType > DoubleImageWriterType;
  DoubleImageWriterType::Pointer writer = DoubleImageWriterType::New();
  writer->SetFileName( argv[4] );
  writer->SetInput( potential_image );
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
  std::cout<<"done\n"<<std::flush;

  t2 = clock();
  t1 = (t2 - t1)/CLOCKS_PER_SEC;
  std::cout<<"New Ellapsed time = "<<t1<<std::endl<<std::flush;
  // end timing
  ////////////////////////////////////////////////////////////


  return 0;
}


// void changeSeedValue(DoubleImageType::Pointer seedImage)
// {
//   /*
//     0 --> 1
//     1 --> 0
//     2 --> 2
//    */

//   typedef itk::ImageRegionIterator< DoubleImageType > DoubleImageIteratorType;
//   DoubleImageIteratorType seedImageIt( seedImage, seedImage->GetLargestPossibleRegion() );

//   for (seedImageIt.GoToBegin(); !seedImageIt.IsAtEnd(); ++seedImageIt)
//     {
//       double v = seedImageIt.Get();

//       if ( 0.9999 <= v && v <= 1.00001 )
//         {
//           seedImageIt.Set(0);
//         }

//       if ( -0.0001 <= v && v <= 0.00001 )
//         {
//           seedImageIt.Set(1);
//         }
//     }
// }
