

#include "MultiThreadMIMetricTestImplementation.h"


int main( int argc, char* argv[] )
{
  typedef MultiThreadMIMetricTestImplementation<3> TestType;

  TestType testImplementation;
  TestType::SizeType size;
  size[0] = atoi(argv[1]);
  size[1] = atoi(argv[2]);
  size[2] = atoi(argv[3]);
  testImplementation.SetImageSize( size );

  TestType::SizeType bsplineSize;
  bsplineSize[0] = atoi(argv[4]);
  bsplineSize[1] = atoi(argv[5]);
  bsplineSize[2] = atoi(argv[6]);
  testImplementation.SetBSplineGridOnImageSize( bsplineSize );
  
  testImplementation.SetNumberOfIterations( atoi( argv[7] ) );
  testImplementation.SetUseThreading( atoi( argv[8] ) > 0 );

  testImplementation.SetNumberOfSamples( size[0] * size[1] * size[2] * 0.05 );

  if (argc > 10)
    {
    testImplementation.SetTimingOutputFileName( argv[9] );
    testImplementation.SetMemoryOutputFileName( argv[10] );
    }
  else if (argc > 9)
    {
    testImplementation.SetTimingOutputFileName( argv[9] );
    testImplementation.CreateMemoryOutputFileName();
    }
  else
    {
    testImplementation.CreateTimingOutputFileName();
    testImplementation.CreateMemoryOutputFileName();
    }

  testImplementation.SetDerivativeOutputFileName( "derivative.out.txt" );

  testImplementation.RunTest();

  return EXIT_SUCCESS;
}
