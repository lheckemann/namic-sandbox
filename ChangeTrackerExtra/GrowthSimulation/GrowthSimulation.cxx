#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkBinaryContourImageFilter.h"

#include "itkLabelObject.h"
#include "itkShapeLabelObject.h"
#include "itkShapeLabelObjectAccessors.h"
#include "itkLabelMap.h"

#include "itkShapeKeepNObjectsLabelMapFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkLabelMapToLabelImageFilter.h"

#include "itkOrientedImage.h"

#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 

#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkImageDuplicator.h"

#define CHANGING_BAND 5
#define EXP_FUNCTION 1

using namespace std;

typedef short PixelType;
typedef float DTPixelType;
const   unsigned int      Dimension = 3;
typedef itk::OrientedImage< PixelType, Dimension >    ImageType;
typedef itk::OrientedImage< DTPixelType, Dimension >   DTImageType;

typedef itk::ImageFileReader< ImageType >  ReaderType;
typedef itk::ImageFileWriter< ImageType >  WriterType;
typedef itk::ImageFileWriter< DTImageType >  DTWriterType;
typedef itk::ThresholdImageFilter<ImageType> ThresholdType;
typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> BinaryThresholdType;
typedef itk::BinaryThresholdImageFilter<DTImageType,DTImageType> DTBinaryThresholdType;
typedef itk::MinimumMaximumImageFilter<ImageType> MinMaxType;
typedef itk::MaskImageFilter<ImageType,ImageType,ImageType> MaskType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIterType;
typedef itk::SignedMaurerDistanceMapImageFilter<ImageType,DTImageType> DTType;

typedef itk::ShapeLabelObject<PixelType,3> ShapeLabelObjectType;
typedef itk::LabelMap<ShapeLabelObjectType> LabelMapType;
typedef itk::LabelImageToShapeLabelMapFilter<ImageType,LabelMapType> LabelToShapeType;

typedef itk::CovariantVector< float, Dimension >  GradientPixelType;
typedef itk::Image< GradientPixelType, Dimension > GradientImageType;
typedef itk::GradientRecursiveGaussianImageFilter<DTImageType,GradientImageType> GradFilterType;
typedef itk::ImageFileWriter<GradientImageType> DFWriterType;
typedef itk::ImageFileWriter<DTImageType> DTWriterType;

typedef itk::BinaryBallStructuringElement<PixelType, Dimension> StructuringElementType;
typedef itk::BinaryErodeImageFilter<ImageType, ImageType, StructuringElementType>  ErodeFilterType;

int main( int argc, char ** argv )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage inputICCSeg inputTumorSeg outputImagePrefix growthMagnitude [0=sin model|1=1/exp model|2=const model]" << std::endl;
    return EXIT_FAILURE;
    }

  ReaderType::Pointer inputReader = ReaderType::New();
  ReaderType::Pointer iccReader = ReaderType::New();
  ReaderType::Pointer tumorReader = ReaderType::New();
  DFWriterType::Pointer outputWriter = DFWriterType::New();
  WriterType::Pointer outputMagnWriter = WriterType::New();
  
  std::string dfFileName = std::string(argv[4])+".mhd";
  std::string dfMagnFileName = std::string(argv[4])+"_magn.mhd";

  inputReader->SetFileName( argv[1] );
  iccReader->SetFileName( argv[2] );
  tumorReader->SetFileName( argv[3] );
  outputWriter->SetFileName( dfFileName.c_str() );
  outputMagnWriter->SetFileName( dfMagnFileName.c_str() );
  float growthMagnitude = atof(argv[5]);
  int modelType = atoi(argv[6]);

  inputReader->Update();
  iccReader->Update();
  tumorReader->Update();

  ImageType::Pointer inputImage = inputReader->GetOutput();
  ImageType::Pointer iccImage = iccReader->GetOutput();
  ImageType::Pointer tumorImage = tumorReader->GetOutput();

  // Find the center of gravity for the tumor label
  LabelToShapeType::Pointer label2shape = LabelToShapeType::New();
  typedef itk::ShapeLabelObject< PixelType, 3 > ShapeLabelObjectType;
  label2shape->SetInput(tumorImage);
  label2shape->Update();

  ImageType::IndexType centroidIdx;
  typedef itk::LabelMap< ShapeLabelObjectType >   LabelMapType;
  typedef LabelMapType::LabelObjectContainerType LabelObjectContainerType;
  LabelMapType* labelMap = label2shape->GetOutput();
  ShapeLabelObjectType* labelObject = labelMap->GetNthLabelObject(1);
  ImageType::PointType centroidPt = labelObject->GetCentroid();
  tumorImage->TransformPhysicalPointToIndex(labelObject->GetCentroid(), centroidIdx);
  std::cout << "Total label objects: " << labelMap->GetNumberOfLabelObjects() << std::endl;
  std::cout << "Center of gravity for label " << labelObject->GetLabel() << ": " << labelObject->GetCentroid() << " (" 
    << centroidIdx << ")" << std::endl;

  typedef itk::BinaryContourImageFilter<ImageType,ImageType> ContourFilterType;
  ContourFilterType::Pointer contourFilter = ContourFilterType::New();
  contourFilter->SetInput(tumorImage);
  contourFilter->SetForegroundValue(labelObject->GetLabel());
  contourFilter->Update();
  ImageType::Pointer contourImage = contourFilter->GetOutput();
 
  typedef itk::ImageDuplicator<ImageType> DuplicatorType;
  DuplicatorType::Pointer dup0 = DuplicatorType::New();
  dup0->SetInputImage(tumorImage);
  dup0->Update();
  ImageType::Pointer dfMagnImage = dup0->GetOutput();
  dfMagnImage->FillBuffer(0.);

  typedef itk::ImageDuplicator<ImageType> DuplicatorType;
  DuplicatorType::Pointer dup = DuplicatorType::New();
  dup->SetInputImage(tumorImage);
  dup->Update();
  ImageType::Pointer dtInput = dup->GetOutput();

  dtInput->FillBuffer(0);
  dtInput->SetPixel(centroidIdx,1);

  DTType::Pointer dtFromCenter = DTType::New();
  dtFromCenter->SetInput(dtInput);
  dtFromCenter->SetUseImageSpacing(true);
  dtFromCenter->SetSquaredDistance(false);
  dtFromCenter->Update();

  float aveDistToBdry = 0., cnt = 0;
  itk::ImageRegionIteratorWithIndex<ImageType> bdrySearchIt(contourImage, contourImage->GetBufferedRegion());
  for(bdrySearchIt.GoToBegin();!bdrySearchIt.IsAtEnd();++bdrySearchIt){
    if(!bdrySearchIt.Get())
      continue;
    float dist2center = dtFromCenter->GetOutput()->GetPixel(bdrySearchIt.GetIndex());
    aveDistToBdry += dist2center;
    cnt++;
  }
  aveDistToBdry /= cnt;

  std::cout << "Ave boundary point is at distance " << aveDistToBdry << std::endl;

  GradFilterType::Pointer grad = GradFilterType::New();
  grad->SetInput(dtFromCenter->GetOutput());
  grad->Update();

  float distCentr2Bdry = aveDistToBdry;
  
  GradientImageType::Pointer gradImage = grad->GetOutput();

  float sinScaleConst = 3.14/(2*distCentr2Bdry);
  if(modelType==1){  // inverse exp model
    std::cout << "Using exp growth simulation model" << std::endl;
  } else if(modelType==2){
    std::cout << "Using const growth simulation model" << std::endl;
  } else {
    std::cout << "Using sin growth simulation model" << std::endl;
    std::cout << "Parameters: Magnitude = " << growthMagnitude << 
      ", Scale constant = " << sinScaleConst << std::endl;
  }

  itk::ImageRegionIteratorWithIndex<GradientImageType> it(gradImage, gradImage->GetBufferedRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> iccit(iccImage, iccImage->GetBufferedRegion());
  for(it.GoToBegin(),iccit.GoToBegin();!it.IsAtEnd();++it,++iccit){
    GradientImageType::PointType curPt;
    GradientImageType::IndexType curIdx = it.GetIndex();
    gradImage->TransformIndexToPhysicalPoint(curIdx, curPt);

    float distFromCenter = dtFromCenter->GetOutput()->GetPixel(curIdx);
    GradientImageType::PixelType curDispl = it.Get(); // gradient vector at the current pixel

    switch (modelType) {
    case 1:
      if(curDispl.GetNorm())
        curDispl *= growthMagnitude/exp(.1*distCentr2Bdry)/curDispl.GetNorm();
      else
        curDispl *= 0;
      break;
    case 2:
      curDispl *= growthMagnitude/curDispl.GetNorm();
      break;
    case 0:
      // sin model
      // use half-period of 3D sin wave centered at the tumor centroid, and 
      // scaled in such a way that the maximum displacement magnitude is at the 
      // distance from the centroid to the tumor boundary
      if(distFromCenter<2.*distCentr2Bdry && curDispl.GetNorm())
        curDispl *= growthMagnitude*sin(distFromCenter*sinScaleConst)/curDispl.GetNorm();
      else
        curDispl *= 0;
      break;
    default:
      assert(0);
    }

    if(!iccit.Get())
        curDispl *= 0.;
    it.Set(curDispl);
    dfMagnImage->SetPixel(curIdx,curDispl.GetNorm());
  }

  outputWriter->SetInput(grad->GetOutput());
  outputWriter->Update();
  
  outputMagnWriter->SetInput(dfMagnImage);
  outputMagnWriter->Update();
 
  return NULL;
}
