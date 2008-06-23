// This function includes all the ITK includes and function calls for some repetitively used ITK filters
// Simplifies ITK style pipeline programming to functional style programming
#include "Functions.h"

int main( int argc, char *argv[] )
{
  //Example: ./preprocess 1 80 ~/ZFishEye/RawData/%d.png ~/ZFishEye/GaussCorrResampled/%d.png ~/ZFishEar/SmoothResampled/%d.png
  if(argc<5){
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " firstfile lastfile inputFileFormat gaussOutputFormat smoothFormat" << std::endl;
    return 1;
    }
  
  // Identify the input arguments. The code runs only on 3D datasets.
  const unsigned int Dimension = 3;
  const unsigned int first_file = atoi(argv[1]); // the first file in the sequence
    const unsigned int last_file =  atoi(argv[2]); // the last file in the sequence
    char *input_file_name =  argv[3];
  char *output_file_name1 =  argv[4];
  char *output_file_name2 =  argv[5];

  // Routine typedefs for all the filters being used.
  typedef float InternalPixelType;
  typedef unsigned char IOPixelType;
  typedef itk::Vector<int,Dimension> IndexVectorType;
  typedef itk::Image<IOPixelType,Dimension> IOImageType;
  typedef itk::Image<InternalPixelType,Dimension> InternalImageType;
  typedef itk::NumericSeriesFileNames NameGeneratorType;
  typedef itk::ImageSeriesReader<InternalImageType> ImageSeriesReaderType;
  typedef itk::CastImageFilter<InternalImageType,IOImageType> FloatToCharCastFilterType;
  typedef itk::Image<IOPixelType,2> WriteImageType;
  typedef itk::ImageSeriesWriter<IOImageType, WriteImageType> SeriesWriterType;

  // Read in the raw data volume
  InternalImageType::Pointer input_volume = ReadIn<InternalImageType>(first_file,last_file,input_file_name);

  // Confocal images have different spacings. Set the right spacings for Gaussian kernel generation
  // and B-spline interpolation here.
  InternalImageType::SizeType size = input_volume->GetLargestPossibleRegion().GetSize();
  InternalImageType::SpacingType spacing;    
    spacing[0] = 1; spacing[1] = 1; spacing[2] = 2;
    InternalImageType::PointType origin;
    origin[0] = 0; origin[1] = 0; origin[2] = 0;

  input_volume->SetSpacing(spacing);
    input_volume->SetOrigin(origin);

  // Non-linear anisotropic diffusion filtering to strengthen edge gradients and filter small internal gradients
  // Parameters work in general. May need to verify settings for each dataset.
  InternalImageType::Pointer smooth_volume = ImageCurvatureSpeed<InternalImageType>(input_volume,2,5);
  cerr<<endl<<"\t-- Anisotropic smoothing complete......"  <<endl;

  // Perform Gaussian correlation at every voxel in the filtered volume. Gaussian kernel is adjusted for the non-linear spacing
  InternalImageType::Pointer gaussCorr = GaussCorrelation<InternalImageType>(smooth_volume);//ReadIn<InternalImageType>(first_file,last_file,output_file_name1);
  gaussCorr->SetSpacing(spacing);
    gaussCorr->SetOrigin(origin);
  cerr<<endl<<"\t-- Gaussian Correlation complete......"  <<endl;

  // Upsample the axial direction of the Gaussian correlation volume
  InternalImageType::Pointer gaussUpsample = Resample<InternalImageType>(gaussCorr);
  cerr<<endl<<"\t-- Resampling complete......"  <<endl;

  // Determine the new axial dimension as a result of sampling
  InternalImageType::SizeType nsize = gaussUpsample->GetLargestPossibleRegion().GetSize();

  // Conversion of float pixel type to unsigned char for writing out 8-bit images. Can be changed to write out 16-bit
  FloatToCharCastFilterType::Pointer caster1 = FloatToCharCastFilterType::New();
  caster1->SetInput(gaussUpsample);
  WriteOut<IOImageType>(caster1->GetOutput(),first_file,first_file+(int)nsize[2]-1,output_file_name1);

  // Upsample the axial direction of the smoothed volume
  InternalImageType::Pointer smoothUpsample = Resample<InternalImageType>(smooth_volume);

  // Write out the smoothed volume
  FloatToCharCastFilterType::Pointer caster2 = FloatToCharCastFilterType::New();
  caster2->SetInput(smoothUpsample);
  WriteOut<IOImageType>(caster2->GetOutput(),first_file,first_file+(int)nsize[2]-1,output_file_name2);

  return 0;
}

