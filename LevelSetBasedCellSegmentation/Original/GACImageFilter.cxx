#include "Functions.h"

// C:\Documents and Settings\mosaligk\Desktop\ZebraFish\build\debug>GAC.exe 0h_2_5histeq.png 0h_2_5speed.png 
// 0h_2_5seg123.png 5 1 3 3 0.03 -0.5 6 0.03
int main( int argc, char *argv[] )
{
  if(argc<11){
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " firstfile lastfile speedFileFormat1 distFileFormat2 foregroundFileFormat3 outputFileFormat";
    std::cerr << " initialDistance";
    std::cerr << " propagationScaling curvatureScaling advectionScaling maxRMSChange"  << std::endl;
    return 1;
    }
  
  const unsigned int Dimension = 3;
  const unsigned int first_file = atoi(argv[1]);
    const unsigned int last_file =  atoi(argv[2]);
    char *input_file_name1 =  argv[3];
  char *input_file_name2 =  argv[4];
  char *input_file_name3 =  argv[5];
  char *output_file_name =  argv[6];
  const double initialDistance = atof(argv[7]);
  const double propagationScaling = atof(argv[8]);
  const double curvatureScaling = atof(argv[9]);
  const double advectionScaling = atof(argv[10]);
  const double maxRMSChange = atof(argv[11]);
  const double seedValue = -initialDistance;
  int NumOfCells = 0;

  typedef float InternalPixelType;
  typedef unsigned char IOPixelType;
  typedef itk::Vector<int,Dimension> IndexVectorType;
  typedef itk::Image<IOPixelType,Dimension> IOImageType;
  typedef itk::Image<InternalPixelType,Dimension> InternalImageType;
  typedef itk::RegionOfInterestImageFilter<InternalImageType,InternalImageType> ROIFilterType;
  typedef itk::ImageRegionIterator<InternalImageType> IteratorType;
  typedef itk::Statistics::ListSample<IndexVectorType> SampleType;
  typedef itk::CastImageFilter<InternalImageType,IOImageType> FloatToCharCastFilterType;
  typedef itk::Image<IOPixelType,2> WriteImageType;
  typedef itk::ImageSeriesWriter<IOImageType, WriteImageType> SeriesWriterType;

  InternalImageType::SpacingType spacing;    
    spacing[0] = 1; spacing[1] = 1; spacing[2] = 1;
    InternalImageType::PointType origin;
    origin[0] = 0; origin[1] = 0; origin[2] = 0;

  // Read in the speed images
  InternalImageType::Pointer speed = ReadIn<InternalImageType>(first_file,last_file,input_file_name1);
  speed->SetSpacing(spacing);
    speed->SetOrigin(origin);

  // Read in the currDist images
  InternalImageType::Pointer currDist = ReadIn<InternalImageType>(first_file,last_file,input_file_name2);
  currDist->SetSpacing(spacing);
    currDist->SetOrigin(origin);

  // Read in the foreground images
  InternalImageType::Pointer remMask = ReadIn<InternalImageType>(first_file,last_file,input_file_name3);
  remMask->SetSpacing(spacing);
    remMask->SetOrigin(origin);

  InternalImageType::Pointer cumMask = InternalImageType::New();
  cumMask->SetRegions(speed->GetLargestPossibleRegion());
  cumMask->Allocate();
  cumMask->FillBuffer(0);
  cerr<<endl<<"\t-- Created cumulative mask......"  <<endl;

  // Find out seed locations and place them in a sample container
  SampleType::Pointer sample = SampleType::New();
  ImageSeeds(currDist,remMask,sample);
  cerr<<endl<<"\t-- Obtained seed locations......"  <<endl;

  InternalImageType::SizeType inputSize = speed->GetLargestPossibleRegion().GetSize();
  // Iterate over all valid seed locations
  for(int i=0;i<sample->GetTotalFrequency();i++){
    IndexVectorType seedPos = sample->GetMeasurementVector(i);
    InternalImageType::IndexType seedLoc;
    for (int j=0;j<Dimension;j++)
      seedLoc[j] = seedPos[j];

    // Check if the seed is in a novel cell location
    if (cumMask->GetPixel(seedLoc) == 0) {
      InternalImageType::RegionType::IndexType start;
      start[0] = ((seedPos[0] - 20)>0)? seedPos[0] - 20:0;
      start[1] = ((seedPos[1] - 20)>0)? seedPos[1] - 20:0;
      start[2] = ((seedPos[2] - 20)>0)? seedPos[2] - 20:0;

      InternalImageType::RegionType::IndexType end;
      end[0] = ((seedPos[0] + 20)>inputSize[0]-1)? inputSize[0]-1:(seedPos[0] + 20);
      end[1] = ((seedPos[1] + 20)>inputSize[1]-1)? inputSize[1]-1:(seedPos[1] + 20);
      end[2] = ((seedPos[2] + 20)>inputSize[2]-1)? inputSize[2]-1:(seedPos[2] + 20);

      InternalImageType::RegionType::SizeType size;
      size[0] = end[0] - start[0] + 1;
      size[1] = end[1] - start[1] + 1;
      size[2] = end[2] - start[2] + 1;

      cerr << "Seed Location: "  << seedLoc[0]  << ' ' << seedLoc[1]  << ' ' << seedLoc[2] << endl;
      cerr << "Start: "      << start[0]    << ' ' << start[1]    << ' ' << start[2] << endl;
      cerr << "End: "        << end[0]    << ' ' << end[1]    << ' ' << end[2] << endl;

      for (int j=0;j<Dimension;j++)
        seedLoc[j] = seedPos[j]-start[j];

      InternalImageType::RegionType region;
      region.SetSize(size);
      region.SetIndex(start);

      ROIFilterType::Pointer ROIfilter = ROIFilterType::New();
      ROIfilter->SetInput(speed);
      ROIfilter->SetRegionOfInterest(region);
      ROIfilter->Update();

      InternalImageType::Pointer subMask = GAC<InternalImageType>(ROIfilter->GetOutput(), seedLoc,
        seedValue,propagationScaling, curvatureScaling, advectionScaling,maxRMSChange,NumOfCells);

      IteratorType It(cumMask,region);
      IteratorType subIt(subMask,subMask->GetLargestPossibleRegion());
      for (It.GoToBegin(),subIt.GoToBegin(); !It.IsAtEnd();++It,++subIt)
        if (subIt.Get()>0)
          It.Set(subIt.Get());

      NumOfCells++;
      std::cout << NumOfCells << ' ' << std::endl;
    }
  }

  FloatToCharCastFilterType::Pointer caster = FloatToCharCastFilterType::New();
  caster->SetInput(cumMask);

  WriteOut<IOImageType>(caster->GetOutput(),first_file,last_file,output_file_name);

  return 0;
}
