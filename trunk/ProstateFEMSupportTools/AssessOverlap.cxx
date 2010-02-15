#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkOrientedImage.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkImageRegionConstIterator.h"

typedef itk::OrientedImage<float,3> ImageType;
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> ThresholdType;
typedef itk::SignedMaurerDistanceMapImageFilter<ImageType,ImageType> DistType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageRegionConstIteratorWithIndex<ImageType> ConstIterType;

int main( int argc, char ** argv ){
  float label = 0;
  if( argc < 3 ){
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage1  inputImage2 [labelOfInterest]" << std::endl;
    return EXIT_FAILURE;
    }

  if(argc>3){
    label = atoi(argv[3]);
  }

  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName(argv[1]);
  reader1->Update();

  ReaderType::Pointer reader2 = ReaderType::New();
  reader2->SetFileName(argv[2]);
  reader2->Update();

  ImageType::Pointer image1 = reader1->GetOutput(), image2 = reader2->GetOutput();

  ThresholdType::Pointer thr1 = ThresholdType::New();
  thr1->SetInput(image1);
  ThresholdType::Pointer thr2 = ThresholdType::New();
  thr2->SetInput(image2);


  if(!label){
    thr1->SetLowerThreshold(1);
    thr1->SetUpperThreshold(255);
    thr2->SetLowerThreshold(1);
    thr2->SetUpperThreshold(255);
  } else {
    thr1->SetLowerThreshold(label);
    thr1->SetUpperThreshold(label);
    thr2->SetLowerThreshold(label);
    thr2->SetUpperThreshold(label);
  }
  thr1->SetInsideValue(255);
  thr2->SetInsideValue(255);

  thr1->Update();
  image1 = thr1->GetOutput();
  thr2->Update();
  image2 = thr2->GetOutput();

 
  DistType::Pointer dtf1 = DistType::New(), dtf2 = DistType::New();

  dtf1->SetInput(image1);
  dtf1->SquaredDistanceOff();
  dtf1->UseImageSpacingOn();
  dtf1->SetInput(image1);
  dtf1->Update();

  dtf2->SetInput(image2);
  dtf2->SquaredDistanceOff();
  dtf2->UseImageSpacingOn();
  dtf2->SetInput(image2);
  dtf2->Update();

  ImageType::Pointer di1 = dtf1->GetOutput(), di2 = dtf2->GetOutput();

  ConstIterType it1(image1, image1->GetBufferedRegion());
  ConstIterType it2(image2, image2->GetBufferedRegion());
  ConstIterType idt1(di1, di1->GetBufferedRegion());
  ConstIterType idt2(di2, di2->GetBufferedRegion());

  ImageType::SpacingType sp = image1->GetSpacing();
  it1.GoToBegin();it2.GoToBegin();idt1.GoToBegin();idt2.GoToBegin();

  float it1acc = 0., it2acc = 0., overlapAcc = 0., hd1 = 0., hd2 = 0.;
  for(;!it1.IsAtEnd();++it1,++it2,++idt1,++idt2){

    ImageType::IndexType idx = it1.GetIndex();
    if(it1.Get()){
      it1acc++;
      if(idt2.Get()>hd2)
        if(idx[0]>0 && idx[1]>0 && idx[2]>0)
          hd2 = idt2.Get();
    }

    if(it2.Get()){
      it2acc++;
      if(idt1.Get()>hd1)
        if(idx[0]>0 && idx[1]>0 && idx[2]>0)
          hd1 = idt1.Get();
    }

    if(it1.Get() && it2.Get())
      overlapAcc++;
    
  }
 
  float dice = 2.*overlapAcc/(it1acc+it2acc);
  std::cerr << dice << "; " << ((hd1>hd2) ? hd1:hd2) << "; " << 
    it1acc*sp[0]*sp[1]*sp[2] << "; " << it2acc*sp[0]*sp[1]*sp[2] << std::endl;

  return EXIT_SUCCESS;
}



