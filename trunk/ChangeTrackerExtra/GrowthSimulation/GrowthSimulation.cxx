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

using namespace std;

typedef float      PixelType;
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
typedef itk::GradientRecursiveGaussianImageFilter<ImageType,GradientImageType> GradFilterType;
typedef itk::ImageFileWriter<GradientImageType> DFWriterType;

typedef itk::BinaryBallStructuringElement<PixelType, Dimension> StructuringElementType;
typedef itk::BinaryErodeImageFilter<ImageType, ImageType, StructuringElementType>  ErodeFilterType;

int main( int argc, char ** argv )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage inputICCSeg inputTumorSeg outputImage growthMagnitude" << std::endl;
    return EXIT_FAILURE;
    }

  ReaderType::Pointer inputReader = ReaderType::New();
  ReaderType::Pointer iccReader = ReaderType::New();
  ReaderType::Pointer tumorReader = ReaderType::New();
  DFWriterType::Pointer outputWriter = DFWriterType::New();
  
  inputReader->SetFileName( argv[1] );
  iccReader->SetFileName( argv[2] );
  tumorReader->SetFileName( argv[3] );
  outputWriter->SetFileName( argv[4] );
  float growthMagnitude = atof(argv[5]);

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

  typedef itk::LabelMap< ShapeLabelObjectType >   LabelMapType;
  typedef LabelMapType::LabelObjectContainerType LabelObjectContainerType;
  LabelMapType* labelMap = label2shape->GetOutput();
  ShapeLabelObjectType* labelObject = labelMap->GetNthLabelObject(1);
  ImageType::IndexType centroidIdx;
  ImageType::PointType centroidPt = labelObject->GetCentroid();
  tumorImage->TransformPhysicalPointToIndex(labelObject->GetCentroid(), centroidIdx);
  std::cout << "Total label objects: " << labelMap->GetNumberOfLabelObjects() << std::endl;
  std::cout << "Center of gravity for label " << labelObject->GetLabel() << ": " << labelObject->GetCentroid() << " (" 
    << centroidIdx << ")" << std::endl;
 
  typedef itk::ImageDuplicator<ImageType> DuplicatorType;
  DuplicatorType::Pointer dup = DuplicatorType::New();
  dup->SetInputImage(tumorImage);
  dup->Update();
  ImageType::Pointer dtInput = dup->GetOutput();
  dtInput->FillBuffer(0);
  dtInput->SetPixel(centroidIdx,1);

  DTType::Pointer dt = DTType::New();
  dt->SetInput(dtInput);
  dt->SetSquaredDistance(false);
  dt->Update();

  GradFilterType::Pointer grad = GradFilterType::New();
  grad->SetInput(dt->GetOutput());
  grad->Update();

  
  GradientImageType::Pointer gradImage = grad->GetOutput();

  itk::ImageRegionIteratorWithIndex<GradientImageType> it(gradImage, gradImage->GetBufferedRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> iccit(iccImage, iccImage->GetBufferedRegion());
  for(it.GoToBegin(),iccit.GoToBegin();!it.IsAtEnd();++it,++iccit){
    GradientPixelType displ;
    GradientImageType::PointType curPt;
    GradientImageType::IndexType curIdx = it.GetIndex();
    gradImage->TransformIndexToPhysicalPoint(curIdx, curPt);
    displ[0] = curPt[0]-centroidPt[0];
    displ[1] = curPt[1]-centroidPt[1];
    displ[2] = curPt[2]-centroidPt[2];
    GradientImageType::PixelType curDispl = it.Get();
    if(displ.GetNorm()<31.45 && curDispl.GetNorm())
      curDispl *= growthMagnitude*sin(.1*displ.GetNorm())/curDispl.GetNorm();
    else
      curDispl *= 0;
    if(!iccit.Get())
      curDispl *= 0.;
    it.Set(curDispl);
  }

  outputWriter->SetInput(grad->GetOutput());
  outputWriter->Update();
  
  
 
  return NULL;
}
