#include "Functions.h"

int main(int argc, char *argv[])
{
  if(argc < 6)
  {
    std::cerr << "Usage: " << argv[0];
    std::cerr << " firstfile lastfile InputFileFormat Length RegionSize SampleSize" << std::endl;  
    return EXIT_FAILURE;
  }

  const unsigned int Dimension = 3;
  const unsigned int first_file = atoi(argv[1]);
    const unsigned int last_file =  atoi(argv[2]);
  char *input_file_name =  argv[3];
  int L = atoi(argv[4]);
  int REGIONSIZE = atoi(argv[5]);
  int SAMPLESIZE = atoi(argv[6]);
  int NumOfMat=2;
  Npcf _Npcf(L,REGIONSIZE,SAMPLESIZE,NumOfMat);

  typedef float InternalPixelType;
  typedef itk::Image<InternalPixelType,Dimension> InternalImageType;
  typedef itk::RegionOfInterestImageFilter<InternalImageType,InternalImageType > ROIFilterType;
  typedef itk::Vector<int,Dimension> IndexVectorType;
  typedef itk::Vector<float,Dimension> NormalVectorType;
  typedef itk::Statistics::ListSample<IndexVectorType> SampleType;
  typedef itk::Statistics::ListSample<NormalVectorType> NormalSampleType;

  InternalImageType::SpacingType spacing;    
    spacing[0] = 1; spacing[1] = 1; spacing[2] = 1;
    InternalImageType::PointType origin;
    origin[0] = 0; origin[1] = 0; origin[2] = 0;

  // Read in the foreground images
  InternalImageType::Pointer input = ReadIn<InternalImageType>(first_file,last_file,input_file_name);
  input->SetSpacing(spacing);
    input->SetOrigin(origin);

  InternalImageType::Pointer inputImage = ForegroundExtraction<InternalImageType>(input,0,0,1);
  InternalImageType::SizeType inputSize = inputImage->GetLargestPossibleRegion().GetSize();

  ifstream infile;
  ofstream outfile;
  infile.open ("seedsNormal.xyzn",ios::in);
  outfile.open ("2pcf.txt",ios::in);

  // Find out seed locations and place them in a sample container
  SampleType::Pointer coord = SampleType::New();
  NormalSampleType::Pointer normal = NormalSampleType::New();
  int temp[3];
  float norm[3];
  while (infile >> temp[0] >> temp[1] >> temp[2]>>norm[0]>>norm[1]>>norm[2]){
    SampleType::MeasurementVectorType mv1;
    NormalSampleType::MeasurementVectorType mv2;
    for(int i=0;i<3;i++){
      mv1[i] = temp[i];
      mv2[i] = norm[i];
    }
    coord->PushBack(mv1);
    normal->PushBack(mv2);
  }
  infile.close();

  // Define a sub-image region to run the NPC windowing on
  InternalImageType::IndexType start;
  start[0] = REGIONSIZE/2;
  start[1] = REGIONSIZE/2;
  start[2] = REGIONSIZE/2;
  InternalImageType::IndexType end;
  end[0] = inputSize[0] - REGIONSIZE/2;
  end[1] = inputSize[1] - REGIONSIZE/2;
  end[2] = inputSize[2] - REGIONSIZE/2;

  //Generate window regions
  ROIFilterType::Pointer ROIfilter = ROIFilterType::New();
  ROIfilter->SetInput(inputImage);

  //Set window region size and start locations 
  InternalImageType::RegionType::SizeType WindowSize;
  WindowSize[0] = REGIONSIZE;
  WindowSize[1] = REGIONSIZE;
  WindowSize[2] = REGIONSIZE;
  InternalImageType::RegionType WindowRegion;
  WindowRegion.SetSize(WindowSize);

  for (int i=0;i<coord->GetTotalFrequency();i++){
    IndexVectorType seedPos = coord->GetMeasurementVector(i);
    NormalVectorType dc = normal->GetMeasurementVector(i);

    float n[3];
    InternalImageType::IndexType idx;
    for (int j=0;j<Dimension;j++){
      idx[j] = seedPos[j];
      n[i] = dc[i];
    }

    if ((idx[0]>=start[0])&&(idx[0]<end[0])&&(idx[1]>=start[1])&&(idx[1]<end[1])&&(idx[2]>=start[2])&&(idx[2]<end[2])){
      InternalImageType::IndexType WindowStart;
      WindowStart[0] = idx[0]-REGIONSIZE/2;
      WindowStart[1] = idx[1]-REGIONSIZE/2;
      WindowStart[2] = idx[2]-REGIONSIZE/2;

      WindowRegion.SetIndex(WindowStart);

      ROIfilter->SetRegionOfInterest(WindowRegion);
      ROIfilter->Update();
      InternalImageType::Pointer windowImage = ROIfilter->GetOutput();
      
      TwoPCF _TwoPCF(idx[0],idx[1],idx[2],_Npcf.getNumOfMat());
      TwoPointCorrelationEvaluation<InternalImageType>(windowImage,n,_TwoPCF,_Npcf);
      _TwoPCF.normalizeEval(SAMPLESIZE,NumOfMat);
      outfile << idx << ' ' << (int)_TwoPCF.getEval(0,0) << ' ' << (int)_TwoPCF.getEval(0,1) << ' ' << (int)_TwoPCF.getEval(1,0) << ' ' << (int)_TwoPCF.getEval(1,1) << std::endl;
    }else{
      outfile << idx << ' ' << 0 << ' ' << 0 << ' ' << 0 << ' ' << 0 << std::endl;
    }
  }
  outfile.close();

  return 0;
}

