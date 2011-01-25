#include "DefRegEval.h"

#include "itkShiftScaleImageFilter.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkImagePCAShapeModelEstimator.h"
#include "itkUnaryFunctorImageFilter.h"

#include "vtksys/SystemTools.hxx"
#include "vtksys/Directory.hxx"

static const int NUMBER_OF_EIGEN_MODES=3;

typedef itk::Image <unsigned char, 3> OutputImageType;

template <class InputPixelType>
class ContourOfSignedDistanceMapFunctor
{
public: 
  InputPixelType operator()( InputPixelType input )
  {
    if( input <= 0)
      return 1;
    else
      return 0;
  }
};

//This function generates the mean shape, modes of variation, and eigenvalues, and stores them in the output directory

int main(int argc, char *argv[])
{
  std::string inputTrainingShapesDir;
  std::string outputShapeModelDir;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--inputTrainingShapesDir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrainingShapesDir, "Input directory where the aligned training shapes are stored, in metaimage (mha) file format");
  args.AddArgument("--outputShapeModelDir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputShapeModelDir, "Output shape model directory");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (inputTrainingShapesDir.empty())
  {
    std::cerr << "inputTrainingShapesDir is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (outputShapeModelDir.empty())
  {
    std::cerr << "outputShapeModelDir is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  /////////////

  //find all ".mha" files in the directory
  std::vector<std::string> fileNames;  
  vtksys::Directory d;
  if ( !d.Load(inputTrainingShapesDir.c_str()) )
  {
    std::cerr << "Cannot open files in the input directory " << inputTrainingShapesDir << std::endl;
    exit(EXIT_FAILURE);
  }
  for ( unsigned long cc = 0; cc < d.GetNumberOfFiles(); cc ++ )
  {
    std::string fname = d.GetFile(cc);
    std::string realName = inputTrainingShapesDir + "/" + fname;
    if (vtksys::SystemTools::FileIsDirectory(realName.c_str()) )
    {
      continue;
    }    
    std::string lowerCaseFname = vtksys::SystemTools::LowerCase(fname); // convert to lower case for case-insensitive matching
    if (lowerCaseFname.find(".mha")==std::string::npos)
    {
      continue;
    }
    fileNames.push_back(realName);
  } 

  if (fileNames.size()<1)
  {
    std::cout<<"ERROR: couldn't find any .mha files in "<<inputTrainingShapesDir<<std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout<<"reading aligned shapes"<<std::endl<<std::endl;

  //define the distance map generator filter
  typedef itk::SignedDanielssonDistanceMapImageFilter<InternalImageType, InternalImageType> DistanceMapFilterType;

  //define the PCA Shape Model estimator
  typedef itk::ImagePCAShapeModelEstimator<InternalImageType, InternalImageType> ShapeEstimatorType;
  ShapeEstimatorType::Pointer shapeEstimator = ShapeEstimatorType::New();
  shapeEstimator->SetNumberOfTrainingImages(fileNames.size());
  shapeEstimator->SetNumberOfPrincipalComponentsRequired(NUMBER_OF_EIGEN_MODES);

  // note that the filter has to process each training image individually
  // whereas the PCA shape estimator will process all the training maps generated
  // simultaneously

  //in a for loop read all the training images one by one
  //and generate distance map for each
  InternalImageReaderType::Pointer  imageReader  = InternalImageReaderType::New();
  for (int i=0; i<fileNames.size(); i++)
  {  
    std::cout<<"reading aligned shape: "<<fileNames.at(i)<<std::endl;
    imageReader->SetFileName(fileNames.at(i).c_str());
    DistanceMapFilterType::Pointer filterDaniel = DistanceMapFilterType::New();
    filterDaniel->SetInput(imageReader->GetOutput());
    filterDaniel->Update();
    shapeEstimator->SetInput(i, filterDaniel->GetOutput());
    std::cout<<"done reading aligned shape"<<std::endl<<std::endl;
  }

  try
  {
    shapeEstimator->Update();
  }
  catch( itk::ExceptionObject & err ) 
  { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    exit(EXIT_FAILURE);
  }

  //create directory for output    
  vtksys::SystemTools::MakeDirectory(outputShapeModelDir.c_str());

  typedef itk::ImageFileWriter< InternalImageType >  WriterType;
  WriterType::Pointer writer =  WriterType::New();
  writer->SetUseCompression(true);

  std::cout<<"outputting signed distance map of mean shape"<<std::endl;
  //output distance map of mean shape
  InternalImageType::Pointer meanShape = shapeEstimator->GetOutput(0);
  std::string meanShapeDistanceMapFile = outputShapeModelDir;
  meanShapeDistanceMapFile.append("/MeanShape_DMap.mha");
  writer->SetFileName(meanShapeDistanceMapFile.c_str());  
  writer->SetInput(meanShape);
  writer->Update();
  std::cout<<"mean shape signed distance map generation complete"<<std::endl<<std::endl;

  std::cout<<"generating contour of mean shape"<<std::endl;
  //this is still signed distance map, need a contour

#if defined(_WIN32)
  typedef itk::UnaryFunctorImageFilter<
    InternalImageType,OutputImageType,
    ContourOfSignedDistanceMapFunctor<typename InternalImageType::PixelType> > ContourExtractorType;
#else
  typedef itk::UnaryFunctorImageFilter<
    InternalImageType,OutputImageType,
    ContourOfSignedDistanceMapFunctor<InternalImageType::PixelType> > ContourExtractorType;
#endif

  ContourExtractorType::Pointer contourExtractor = ContourExtractorType::New();
  contourExtractor->SetInput(meanShape);
  contourExtractor->Update();
  OutputImageType::Pointer MeanImageContour = contourExtractor->GetOutput();
  typedef itk::ImageFileWriter< OutputImageType >  BinaryWriterType;
  BinaryWriterType::Pointer binaryWriter = BinaryWriterType::New();
  binaryWriter->SetUseCompression(true);
  std::string meanShapeFile = outputShapeModelDir;
  meanShapeFile.append("/MeanShape.mha");
  binaryWriter->SetFileName(meanShapeFile.c_str());
  binaryWriter->SetInput(MeanImageContour);
  binaryWriter->Update();
  std::cout<<"contour generation complete"<<std::endl<<std::endl;

  std::cout<<"generating modes of variation"<<std::endl<<std::endl;
  char *Suffix = new char[20]; 
  //store modes of variation
  for (int mode=1; mode<=NUMBER_OF_EIGEN_MODES; mode++)
  {  
    //form the file name
    sprintf(Suffix, "%02d.mha", mode);
    std::string variationModeFile = outputShapeModelDir;
    variationModeFile.append("/VariationMode_");
    variationModeFile.append(Suffix);
    writer->SetFileName(variationModeFile.c_str());
    writer->SetInput(shapeEstimator->GetOutput(mode));
    writer->Update();
  }
  delete [] Suffix;
  std::cout<<"generation of modes of variation complete"<<std::endl<<std::endl;

  //store eigen values
  vnl_vector<double> eigenValues(NUMBER_OF_EIGEN_MODES); 
  eigenValues = shapeEstimator->GetEigenValues();
  std::cout<<"storing eigenvalues"<<std::endl<<std::endl;
  std::string eigenValueFilePath = outputShapeModelDir;
  eigenValueFilePath.append("/eigenValues.txt");
  std::ofstream eigenValueFile(eigenValueFilePath.c_str(), std::ios_base::binary | std::ios_base::out ); // binary, to force LF line ending (otherwise file compare would fail during testing)
  if (!eigenValueFile.is_open())
  {
    std::cerr << "Fatal Error: Could not store eigenvalues" << std::endl;
    exit(EXIT_FAILURE);
  }
  for (int mode=1; mode<=NUMBER_OF_EIGEN_MODES; mode++)
  {
    eigenValueFile<<eigenValues[mode-1]<<"\n";
  }
  eigenValueFile.close();
  std::cout<<"finished storing eigenvalues"<<std::endl<<std::endl;

  return EXIT_SUCCESS;
}
