#include "Functions.h"

int main( int argc, char *argv[] )
{
  if(argc<11){
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " firstfile lastfile GaussInputFormat SmoothInputFormat";
    std::cerr << " GradientFormat ForegroundFormat DistFormat SpeedFormat";
    std::cerr << " alpha beta t" << std::endl;
    return 1;
    }
  
  const unsigned int Dimension = 3;
  const unsigned int first_file = atoi(argv[1]);
    const unsigned int last_file =  atoi(argv[2]);
    char *input_file_name1 =  argv[3];
  char *input_file_name2 =  argv[4];
  char *output_file_name1 =  argv[5];
  char *output_file_name2 =  argv[6];
  char *output_file_name3 =  argv[7];
  char *output_file_name4 =  argv[8];

  typedef float InternalPixelType;
  typedef unsigned char IOPixelType;
  typedef itk::Vector<int,Dimension> IndexVectorType;
  typedef itk::Image<IOPixelType,Dimension> IOImageType;
  typedef itk::Image<InternalPixelType,Dimension> InternalImageType;
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<InternalImageType,InternalImageType> GradientFilterType;
  typedef itk::SigmoidImageFilter<InternalImageType,InternalImageType> SigmoidFilterType;
  typedef itk::CastImageFilter<InternalImageType,IOImageType> FloatToCharCastFilterType;

  // filename generator instance
  InternalImageType::Pointer gaussCorr = ReadIn<InternalImageType>(first_file,last_file,input_file_name1);

  // filename generator instance
  InternalImageType::Pointer smooth_volume = ReadIn<InternalImageType>(first_file,last_file,input_file_name2);

  InternalImageType::Pointer mask = ForegroundExtraction<InternalImageType>(smooth_volume,0,20,1);
  cerr<<endl<<"\t-- Foreground extraction complete......"  <<endl;

  InternalImageType::Pointer maskCorr = AddProcedure<InternalImageType>(gaussCorr,mask,2);
  cerr<<endl<<"\t-- Removing Gaussian noise complete......"  <<endl;
  
  // Compute the gradient magnitude
  GradientFilterType::Pointer gradientMagnitude = GradientFilterType::New();
  gradientMagnitude->SetInput(smooth_volume);
  gradientMagnitude->SetSigma(0.5);
  gradientMagnitude->Update();
  cerr<<endl<<"\t-- Gradient computation complete......"  <<endl;

  // Initialize speed function filter settings
  SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
  sigmoid->SetOutputMinimum(0.0);
  sigmoid->SetOutputMaximum(255.0);
  sigmoid->SetInput(gradientMagnitude->GetOutput());
  sigmoid->SetAlpha(atof(argv[9]));
  sigmoid->SetBeta(atof(argv[10]));
  sigmoid->Update();
  cerr<<endl<<"\t-- Gradient Speed computation complete......"  <<endl;

  // Writeout the gradient images
  FloatToCharCastFilterType::Pointer caster1 = FloatToCharCastFilterType::New();
  caster1->SetInput(sigmoid->GetOutput());
  WriteOut<IOImageType>(caster1->GetOutput(),first_file,last_file,output_file_name1);

  // Writeout the foreground images
  InternalImageType::Pointer remMask = ForegroundExtraction<InternalImageType>(maskCorr,150,255,255);
  cerr<<endl<<"\t-- Final foreground extraction complete......"  <<endl;
  
  FloatToCharCastFilterType::Pointer caster2 = FloatToCharCastFilterType::New();
  caster2->SetInput(remMask);
  WriteOut<IOImageType>(caster2->GetOutput(),first_file,last_file,output_file_name2);
  
  // Writeout the distance images
  InternalImageType::Pointer currDist = DistanceMap<InternalImageType>(remMask,0);
  cerr<<endl<<"\t-- Distance map computation complete......"  <<endl;

  FloatToCharCastFilterType::Pointer caster3 = FloatToCharCastFilterType::New();
  caster3->SetInput(currDist);
  WriteOut<IOImageType>(caster3->GetOutput(),first_file,last_file,output_file_name3);

  // Writeout the speed images
  InternalImageType::Pointer speed = SpeedFun<InternalImageType>(sigmoid->GetOutput(),currDist,atof(argv[11]));
  cerr<<endl<<"\t-- Combined speed computation complete......"  <<endl;

  FloatToCharCastFilterType::Pointer caster4 = FloatToCharCastFilterType::New();
  caster4->SetInput(speed);
  WriteOut<IOImageType>(caster4->GetOutput(),first_file,last_file,output_file_name4);

  return 0;
}
