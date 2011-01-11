#include "DefRegEval.h"

#include "itkUnaryFunctorImageFilter.h" 
#include "itkShiftScaleImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "vtkSmartPointer.h"
#include "vtkMinimalStandardRandomSequence.h"

typedef itk::Image <unsigned char, 3> OutputImageType;

////////////////
const double MIN_WEIGHT_MODE_1 = -0.2;
const double MAX_WEIGHT_MODE_1 = 0.2;
const double MIN_WEIGHT_MODE_2 = -1.5;
const double MAX_WEIGHT_MODE_2 = 1.5;
const double MIN_WEIGHT_MODE_3 = -0.2;
const double MAX_WEIGHT_MODE_3 = 0.2;
///////////////


template <class InputPixelType>
class ContourOfSignedDistanceMapFunctor
{
public:
  InputPixelType operator()( InputPixelType input )
  {
    return (input<=0)?1:0;
  }
};

int main(int argc, char *argv[])
{
  std::string shapeModelDir = "C:\\OrganErrorMapRendering\\Output";
  std::string outputImageFilename = "shape-image.mha";

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--shape-model-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &shapeModelDir, "Directory path where the shape model files are stored (eigenValues.txt and *.mha)");
  args.AddArgument("--output", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImageFilename, "Generated shape image output file name");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  /////////////

  std::vector<InternalImageType::Pointer> EigenModes;
  InternalImageType::Pointer MeanShape;

  //get eigenvalues
  vnl_vector<double> eigenValues(3);

  std::string eigenValueFile = shapeModelDir;
  eigenValueFile.append("\\eigenValues.txt");

  char* line = new char[2000];
  std::ifstream eigenValueInputStream(eigenValueFile.c_str());

  eigenValueInputStream.getline(line, 2000);
  eigenValues[0] = strtod(line, NULL);

  eigenValueInputStream.getline(line, 2000);
  eigenValues[1] = strtod(line, NULL);

  eigenValueInputStream.getline(line, 2000);
  eigenValues[2] = strtod(line, NULL);

  eigenValueInputStream.close();

  //get EigenModes

  char* Suffix = new char[20];

  if(EigenModes.size() == 0)
  {
    for(int mode=0; mode<3; mode++)
    {
      //form the file name
      sprintf(Suffix, "%02d.mha", mode+1);
      std::string variationModeFile = shapeModelDir;
      variationModeFile.append("\\VariationMode_");
      variationModeFile.append(Suffix);

      InternalImageReaderType::Pointer  ImageReader  = InternalImageReaderType::New();
      ImageReader->SetFileName(variationModeFile.c_str());

      try
      {
        ImageReader->Update();
      }
      catch( itk::ExceptionObject & err ) 
      { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        exit(-1);
      }  

      EigenModes.push_back(ImageReader->GetOutput());
    }
  }

  //get mean shape
  if(!MeanShape)
  {
    InternalImageReaderType::Pointer  ImageReader  = InternalImageReaderType::New();

    std::string meanShapeFile = shapeModelDir;
    meanShapeFile.append("\\MeanShape_DMap.mha");
    ImageReader->SetFileName(meanShapeFile.c_str());

    try
    {
      ImageReader->Update();
    }
    catch( itk::ExceptionObject & err ) 
    { 
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      exit(-1);
    }

    MeanShape = InternalImageType::New();
    MeanShape = ImageReader->GetOutput();
  }

  //generate n random organ contours
  vtkSmartPointer<vtkMinimalStandardRandomSequence> random = vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();

#if defined(_WIN32)
  typedef itk::UnaryFunctorImageFilter<typename InternalImageType,OutputImageType,ContourOfSignedDistanceMapFunctor<InternalImageType::PixelType> > ContourExtractorType;
#else
  typedef itk::UnaryFunctorImageFilter<InternalImageType,OutputImageType,ContourOfSignedDistanceMapFunctor<InternalImageType::PixelType> > ContourExtractorType;
#endif

  ContourExtractorType::Pointer contourExtractor = ContourExtractorType::New();

  //randomize PCA weights
  double sigmaFactor[3];
  random->Next();
  sigmaFactor[0] = random->GetRangeValue(MIN_WEIGHT_MODE_1, MAX_WEIGHT_MODE_1);
  random->Next();
  sigmaFactor[1] = random->GetRangeValue(MIN_WEIGHT_MODE_2, MAX_WEIGHT_MODE_2);
  random->Next();
  sigmaFactor[2] = random->GetRangeValue(MIN_WEIGHT_MODE_3, MAX_WEIGHT_MODE_3);

  //force them to have two decimal places
  sigmaFactor[0] = (double)((int)(sigmaFactor[0] * 100))/(double)100;
  sigmaFactor[1] = (double)((int)(sigmaFactor[1] * 100))/(double)100;
  sigmaFactor[2] = (double)((int)(sigmaFactor[2] * 100))/(double)100;

  std::cout<<"Weight Mode 1 = "<<sigmaFactor[0]<<std::endl;
  std::cout<<"Weight Mode 2 = "<<sigmaFactor[1]<<std::endl;
  std::cout<<"Weight Mode 3 = "<<sigmaFactor[2]<<std::endl<<std::endl;

  InternalImageType::Pointer image = MeanShape;

  typedef itk::ShiftScaleImageFilter<InternalImageType, InternalImageType> DivideFilterType;
  DivideFilterType::Pointer divider = DivideFilterType::New();

  typedef itk::AddImageFilter<InternalImageType,InternalImageType,InternalImageType> AddFilterType;
  AddFilterType::Pointer addFilter = AddFilterType::New();

  typedef itk::SubtractImageFilter<InternalImageType, InternalImageType, InternalImageType> SubtractFilterType;
  SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();

  for(int mode = 0; mode < 3; mode++)
  {
    divider->SetInput(EigenModes[mode]);
    divider->SetScale(sigmaFactor[mode]*sqrt(eigenValues[mode]));
    divider->SetShift(0);
    divider->Update();
    if(sigmaFactor[mode] > 0) //is positive
    {
      addFilter->SetInput1(image);
      addFilter->SetInput2(divider->GetOutput());
      addFilter->Update();
      image = addFilter->GetOutput();
    }
    else//is negative
    {
      subtractFilter->SetInput1(image);
      subtractFilter->SetInput2(divider->GetOutput());
      subtractFilter->Update();
      image = subtractFilter->GetOutput();
    }
  }    

  divider->SetInput(image);
  divider->SetScale(-1);
  divider->SetShift(0);
  divider->Update();

  typedef itk::ImageFileWriter< InternalImageType >  FloatWriterType;
  FloatWriterType::Pointer floatWriter = FloatWriterType::New();
  floatWriter->SetFileName(outputImageFilename.c_str());
  floatWriter->SetInput(divider->GetOutput());
  floatWriter->Update();

  return EXIT_SUCCESS;;
}
