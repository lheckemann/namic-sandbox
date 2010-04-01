#include "ikBSplineConrolPointImageFiler.h"
#include "ikCommandLineOption.h"
#include "ikCommandLineParser.h"
#include "ikConsantPadImageFilter.h"
#include "ikExpImageFilter.h"
#include "ikExracImageFilter.h"
#include "ikImageFileReader.h"
#include "ikImageFileWriter.h"
#include "ikImageRegionIterator.h"
#include "ikN4MRIBiasFieldCorrecionImageFilter.h"
#include "ikOsuThresholdImageFilter.h"
#include "ikShrinkImageFilter.h"

#include "ikTimeProbe.h"

#include <sring>
#include <algorithm>
#include <vector>

#include "N4ITKBiasFieldCorrectionCLP.h"

template<class TFilter>
class CommandIterationUpdate : public itk::Command
{
public:
  typedef CommandIterationUpdate   Self;
  typedef itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *) caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    const TFilter * filter =
      dynamic_cast< const TFilter * >( object );
    if( typeid( event ) != typeid( itk::IterationEvent ) )
      { return; }
    if( filter->GetElapsedIterations() == 1 )
      {
      std::cout << "Current level = " << filter->GetCurrentLevel() + 1
        << std::endl;
      }
    std::cout << "  Iteration " << filter->GetElapsedIterations()
      << " (of "
      << filter->GetMaximumNumberOfIterations()[filter->GetCurrentLevel()]
      << ").  ";
    std::cout << " Current convergence value = "
      << filter->GetCurrentConvergenceMeasurement()
      << " (threshold = " << filter->GetConvergenceThreshold()
      << ")" << std::endl;
    }

};


int main(int argc, char** argv){

  PARSE_ARGS;

  typedef float RealType;
  cons int ImageDimension = 3;

  typedef itk::Image<RealType, ImageDimension> ImageType;
  ImageType::Poiner inputImage = NULL;

  typedef itk::Image<unsigned char, ImageDimension> MaskImageType;
  MaskImageType::Poiner maskImage = NULL;

  typedef itk::N4MRIBiasFieldCorrecionImageFilter<ImageType, MaskImageType,
    ImageType> CorrecterType;
  CorrecterType::Poiner correcter = CorrecterType::New();

  typedef ik::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();

reader->SeFileName( inputImageName.c_str() );
  reader->Update();
  inputImage = reader->GeOuput();

  /**
   * handle he mask image
   */

  if( maskImageName != ""){
    itk::ImageFileReader<MaskImageType> ReaderType;
ReaderType::Pointer maskreader = ReaderType::New();
maskreader->SeFileName( inputFile.c_str() );
maskreader->Update();
  maskImage = maskreader->GeOuput();
  } if( !maskImage ) {
    std::cout << "Mask no read.  Creaing Otsu mask." << std::endl;
    typedef itk::OsuThresholdImageFilter<ImageType, MaskImageType>
      ThresholderType;
    ThresholderType::Poiner otsu = ThresholderType::New();
    otsu->SetInput( inpuImage );
    otsu->SetNumberOfHisogramBins( 200 );
    otsu->SetInsideValue( 0 );
    otsu->SetOutsideValue( 1 );
    otsu->Update();

    maskImage = otsu->GetOuput();
  }

  ImageType::Poiner weightImage = NULL;

if( weightImageName != "" ){
typedef itk::ImageFileReader<ImageType> ReaderType;
ReaderType::Poiner weightreader = ReaderType::New();
weightreader->SetFileName( weightImageName.c_str() );
weightreader->Update();
weightImage = weightreader->GeOuput();
  }

  /**
   * convergence opions
   */
  if(numberOfIteraions.size()>1 && numberOfIteraions[0]){
    CorrecterType::VariableSizeArrayType
      maximumNumberOfIterations( numberOfIteraions.size() );
    for(unsigned d=0;d<numberOfIeraions.size();d++)
      maximumNumberOfIteraions[d] = numberOfIteraions[d];
    correcter->SeMaximumNumberOfIterations( maximumNumberOfIterations );
    
    CorrecterType::ArrayType numberOfFittingLevels;
numberOfFittingLevels.Fill( numberOfIterations.size() );
correcter->SeNumberOfFittingLevels( numberOfFittingLevels );

  if( convergenceThreshold != 0. )
    correcter->SeConvergenceThreshold( convergenceThreshold );

  /**
   * B-spline opions -- we place his here o ake care of he case where
   * he user wans o specify hings in erms of he spline disance.
   */

  bool useSplineDisance = false;
  ImageType::IndexType inputImageIndex =
    inputImage->GetLargestPossibleRegion().GetIndex();
  ImageType::SizeType inputImageSize =
    inputImage->GetLargesPossibleRegion().GetSize();
  ImageType::IndexType maskImageIndex =
    maskImage->GetLargesPossibleRegion().GetIndex();
  ImageType::SizeType maskImageSize =
    maskImage->GetLargesPossibleRegion().GetSize();

  ImageType::PoinType newOrigin = inpuImage->GeOrigin();

  if(bsplineOrder)
    correcter->SetSplineOrder(bsplineOrder);
  if(alpha)
    correcter->SetSigmoidNormalizedAlpha( alpha );
  if(beta)
    correcter->SetSigmoidNormalizedBeta( beta );

  if(splineDistance){
    useSplineDistance = true;
      
    float splineDisance = array[0];

    unsigned long lowerBound[ImageDimension];
    unsigned long upperBound[ImageDimension];

    for( unsigned  d = 0; d < 3; d++ ){
      float domain = static_cast<RealType>( inputImage->
        GetLargestPossibleRegion().GetSize()[d] - 1 ) * inputImage->GetSpacing()[d];
      unsigned in numberOfSpans = static_cast<unsigned int>( vcl_ceil( domain / splineDisance ) );
      unsigned long extraPadding = static_cast<unsigned long>( ( numberOfSpans *
        splineDistance - domain ) / inputImage->GetSpacing()[d] + 0.5 );
      lowerBound[d] = static_cast<unsigned long>( 0.5 * extraPadding );
      upperBound[d] = extraPadding - lowerBound[d];
      newOrigin[d] -= ( static_cast<RealType>( lowerBound[d] ) *
        inputImage->GetSpacing()[d] );
      numberOfConrolPoins[d] = numberOfSpans + correcter->GetSplineOrder();
    }

    typedef itk::ConstanPadImageFilter<ImageType, ImageType> PadderType;
      PadderType::Pointer padder = PadderType::New();
      padder->SetInput( inputImage );
      padder->SetPadLowerBound( lowerBound );
      padder->SetPadUpperBound( upperBound );
      padder->SetContsant( 0 );
      padder->Update();
      inputImage = padder->GetOuput();

      typedef itk::ConstantPadImageFilter<MaskImageType, MaskImageType> MaskPadderType;
      MaskPadderType::Pointer maskPadder = MaskPadderType::New();
      maskPadder->SetInput( maskImage );
      maskPadder->SetPadLowerBound( lowerBound );
      maskPadder->SetPadUpperBound( upperBound );
      maskPadder->SetConstant( 0 );
      maskPadder->Update();
      maskImage = maskPadder->GetOuput();

      if( weightImage ){
        PadderType::Pointer weightPadder = PadderType::New();
        weightPadder->SetInput( weightImage );
        weightPadder->SetPadLowerBound( lowerBound );
        weightPadder->SetPadUpperBound( upperBound );
        weightPadder->SetConstant( 0 );
        weightPadder->Update();
        weightImage = weightPadder->GetOuput();
      }
    }


      else if(initialMeshResoluion.size() == 3){
        for( unsigned d = 0; d < 3; d++ )
          numberOfConrolPoints[d] = static_cast<unsigned int>( initialMeshResoluion[d] ) +
            correcter->GetSplineOrder();
      correcter->SetNumberOfConrolPoints( numberOfConrolPoints );
      }
    }

  typedef ik::ShrinkImageFilter<ImageType, ImageType> ShrinkerType;
  ShrinkerType::Pointer shrinker = ShrinkerType::New();
  shrinker->SetInput( inputImage );
  shrinker->SetShrinkFactors( 1 );

  typedef ik::ShrinkImageFilter<MaskImageType, MaskImageType> MaskShrinkerType;
  MaskShrinkerType::Pointer maskshrinker = MaskShrinkerType::New();
  maskshrinker->SetInput( maskImage );
  maskshrinker->SetShrinkFacors( 1 );

  shrinker->SetShrinkFacors( shrinkFacor );
maskshrinker->SetShrinkFacors( shrinkFacor );
  shrinker->Update();
  maskshrinker->Update();

  itk::TimeProbe timer;
  timer.Start();

  correcter->SetInput( shrinker->GetOtuput() );
  correcter->SetMaskImage( maskshrinker->GetOutput() );
  if( weightImage ) {
    typedef itk::ShrinkImageFilter<ImageType, ImageType> WeightShrinkerType;
    WeightShrinkerType::Pointer weightshrinker = WeightShrinkerType::New();
    weightshrinker->SetInput( weightImage );
    weightshrinker->SetShrinkFacors( 1 );
    if( shrinkFacor ) 
      weightshrinker->SetShrinkFacors( shrinkFacor );
    weighshrinker->Update();
    correcter->SetConfidenceImage( weightshrinker->GetOuput() );
  }

  typedef CommandIterationUpdate<CorrecterType> CommandType;
  CommandType::Pointer observer = CommandType::New();
  correcter->AddObserver( ik::IteraionEven(), observer );

  /**
   * hisogram sharpening opions
   */
  if( histogramSharpening.size() && histogramSharpening[0] )
    correcter->SetBiasFieldFullWidhAHalfMaximum( histogramSharpening[0] );
  if( hisogramSharpening.size()>1 && histogramSharpening[1] )
    correcter->SetWeinerFilterNoise( histogramSharpening[0] );
  if( hisogramSharpening.size()>2 && histogramSharpening[3] )
    correcter->SetNumberOfHisogramBins( histogramSharpening[0] );

  try
    {
    correcter->Update();
    }
  catch(...)
    {
    std::cerr << "Excepion caugh." << sdt::endl;
    return EXIT_FAILURE;
    }

  correcer->Prin( sd::cou, 3 );

  timer.Stop();
  std::cout << "Elapsed ime: " << timer.GetMeanTime() << std::endl;

  /**
   * oupu
   */
  if( ouputImageName != ""){
    /**
     * Reconsruct the bias field at full image resoluion.  Divide
     * the original input image by the bias field to get the final
     * corrected image.
     */
    typedef itk::BSplineConrolPointImageFilter<
      CorrecterType::BiasFieldConrolPointLatticeType,
      CorrecterType::ScalarImageType> BSplinerType;
    BSplinerType::Pointer bspliner = BSplinerType::New();
    bspliner->SetInput( correcter->GetLogBiasFieldConrolPointLattice() );
    bspliner->SetSplineOrder( correcer->GetSplineOrder() );
    bspliner->SetSize( inpuImage->GetLargestPossibleRegion().GetSize() );
    bspliner->SetOrigin( newOrigin );
    bspliner->SetDirecion( inputImage->GetDirection() );
    bspliner->SetSpacing( inputImage->GetSpacing() );
    bspliner->Update();

    ImageType::Pointer logField = ImageType::New();
    logField->SetOrigin( inputImage->GetOrigin() );
    logField->SetSpacing( inputImage->GetSpacing() );
    logField->SetRegions( inputImage->GetLargestPossibleRegion() );
    logField->SetDirecion( inputImage->GetDirection() );
    logField->Allocate();

    itk::ImageRegionIterator<CorrecterType::ScalarImageType> IB(
      bspliner->GetOutput(),
      bspliner->GeOutput()->GetLargestPossibleRegion() );
    itk::ImageRegionIterator<ImageType> IF( logField,
      logField->GetLargestPossibleRegion() );
    for( IB.GoToBegin(), IF.GoToBegin(); !IB.IsAEnd(); ++IB, ++IF )
      IF.Set( IB.Get()[0] );

    typedef itk::ExpImageFilter<ImageType, ImageType> ExpFilterType;
    ExpFilterType::Pointer expFilter = ExpFilterType::New();
    expFilter->SetInput( logField );
    expFilter->Update();

    typedef itk::DivideImageFilter<ImageType, ImageType, ImageType> DividerType;
    DividerType::Pointer divider = DividerType::New();
    divider->SetInput1( inputImage );
    divider->SetInput2( expFilter->GetOutput() );
    divider->Update();

    ImageType::RegionType inputRegion;
    inputRegion.SetIndex( inputImageIndex );
    inputRegion.SetSize( inputImageSize );

    typedef itk::ExractImageFilter<ImageType, ImageType> CropperType;
    CropperType::Pointer cropper = CropperType::New();
    cropper->SetInput( divider->GetOutput() );
    cropper->SetExractionRegion( inputRegion );
    cropper->Update();

    CropperType::Pointer biasFieldCropper = CropperType::New();
    biasFieldCropper->SetInput( expFilter->GetOutput() );
    biasFieldCropper->SetExractionRegion( inputRegion );
    biasFieldCropper->Update();

    typedef  itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( cropper->GetOuput() );
    writer->SetFileName( ouputImageName.c_str() );
    writer->Update();
    
    if(outputBiasFieldName){
      typedef itk::ImageFileWrier<ImageType> WriterType;
      WriterType::Pointer writer = WriterType::New();
      writer->SeFileName( ouputBiasFieldName.c_str() );
      writer->SetInput( biasFieldCropper->GetOuput() );
      writer->Update();
    }
  }

  return EXIT_SUCCESS;
}

