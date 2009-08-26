#include "itkFindCenterOfBrainFilter.h"
#include <itkNumericTraits.h>
#include <itkImageMomentsCalculator.h>

namespace itk
{

template <class TInputImage>
FindCenterOfBrainFilter<TInputImage>
::FindCenterOfBrainFilter() :
  m_Maximize(true),
  m_Axis(2),
  m_OtsuPercentileThreshold(0.001),
  m_ClosingSize(7),
  m_HeadSizeLimit(1000),
  m_BackgroundValue(NumericTraits<typename ImageType::PixelType>::Zero)
{
  m_CenterOfBrain[0] =
    m_CenterOfBrain[1] =
    m_CenterOfBrain[2] = 0.0;
}

template <class TInputImage>
FindCenterOfBrainFilter<TInputImage>
::~FindCenterOfBrainFilter()
{
}

template <class TInputImage>
void
FindCenterOfBrainFilter<TInputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
}

template <class TInputImage>
void
FindCenterOfBrainFilter<TInputImage>
::AllocateOutputs()
{
  // Pass the input through as the output
  InputImagePointer image =
    const_cast< TInputImage * >( this->GetInput() );
  this->GraftOutput( image );

}

template <class TInputImage>
void
FindCenterOfBrainFilter<TInputImage>
::GenerateData()
{
  double SI_CenterBasedOnTopOfHead=0; //This is the return value for the estimated SI location of the center of the brain.

  //////////////////////////////////////////////////////////////////////////
  typename LFFMaskFilterType::Pointer LFF = LFFMaskFilterType::New();
  LFF->SetInput(this->GetInput());
  LFF->SetOtsuPercentileThreshold(this->m_OtsuPercentileThreshold);
  LFF->SetClosingSize(this->m_ClosingSize);
  LFF->Update();
  InputImagePointer foreground = LFF->GetOutput();

  //  foreground will initially hold just a tissue outline (a so-called integer mask),
  //  then it will code for the integer distance to the top of physical space, only within the outline,
  //  then we will convert the zeros and distance codes to input image signal and the assigned background value
  //  in conformity with the assigned volume-distance thresholding from a histogram.

  //////////////////////////////////////////////////////////////////////////
  //  This will find maximum Superior/Inferior location of the corner voxels.
  double extremum;
  {
  SizeType volOrigSize = this->GetInput()->GetLargestPossibleRegion().GetSize();
  IndexType U,V;
  PointType limitU, limitV;
  V.Fill(0);
  this->GetInput()->TransformIndexToPhysicalPoint(V, limitV);
  for (unsigned int i=0; i<volOrigSize[0]; i+=volOrigSize[0]-1)
    {
    U[0] = i;
    for (unsigned int j=0; j<volOrigSize[1]; j+=volOrigSize[1]-1)
      {
      U[1] = j;
      for (unsigned int k=0; k<volOrigSize[2]; k+=volOrigSize[2]-1)
        {
        U[2] = k;
        this->GetInput()->TransformIndexToPhysicalPoint(U, limitU);
        if (this->m_Maximize ? limitU[this->m_Axis] > limitV[this->m_Axis] : limitU[this->m_Axis] < limitV[this->m_Axis])
          {
          limitV = limitU;
          V = U;
          }
        }
      }
    }
  extremum = limitV[this->m_Axis];
  //  Now extremum is the level of the highest subspace plane in the voxel array.
  }
  std::cout << "extremum = " << extremum << std::endl;

  //////////////////////////////////////////////////////////////////////////
  //  This will produce ForegroundLevel representing where to threshold the head from the neck.
  double ForegroundLevel = 1;
  {
  ImageIteratorType ItPixel( foreground, foreground->GetLargestPossibleRegion() );

  PointType PixelPhysicalPoint;
  PixelPhysicalPoint.Fill(0.0);

  ItPixel.Begin();
  for (; !ItPixel.IsAtEnd(); ++ItPixel)
    {
    if (ItPixel.Get() != 0)
      {
      this->GetInput()->TransformIndexToPhysicalPoint( ItPixel.GetIndex(), PixelPhysicalPoint);
      ItPixel.Set( static_cast<PixelType>(vnl_math_rnd(vnl_math_abs(extremum - PixelPhysicalPoint[this->m_Axis]))) );
      }
    // else, leave the foreground coded zero, not some positive distance from the top.
    }
  // Now foreground holds both the silouette information and the distance-from-extermum information.
  }
#if 0
  return true;
#endif

  //////////////////////////////////////////////////////////////////////////
  //  This will populate a histogram to make an increasing volume distribution.
  {
  typedef typename Statistics::Histogram<double> HistogramType;

  double maxval = 0;
  double minval = vcl_numeric_limits<double>::max();
  ImageIteratorType imIter( foreground, foreground->GetLargestPossibleRegion() );
  while(! imIter.IsAtEnd() )
    {
    const double curr_val=imIter.Value();
    if(curr_val > 1 ) //Need to find min that is greater than zero.
      {
      maxval=(curr_val > maxval)?curr_val:maxval;
      minval=(curr_val < minval)?curr_val:minval;
      }
    ++imIter;
    }

  const SpacingType origSpacing =this->GetInput()->GetSpacing();
  const double voxelSize = origSpacing[0] * origSpacing[1] * origSpacing[2]*0.001;  // Cubic CC.
  std::cout << "voxelSize = " << voxelSize << " cubic CC"  << std::endl;
  std::cout << "MIN MAX " << minval << "  " << maxval << std::endl;

  //Make sure that bin width is smaller than the minimum voxel width.  A single layer cannot have more than one row of voxels accumulated in a single bin.
  int numBins =  (int) ((maxval - minval)/(vnl_math_min(origSpacing[0],vnl_math_min(origSpacing[1],origSpacing[2]))));

  // Histogram computation
#ifdef ITK_USE_REVIEW_STATISTICS
  HistogramType::SizeType size(1);
#else
  HistogramType::SizeType size;
#endif
  size[0] = numBins;
  std::cout << "Histo values " << minval <<" ...  " << maxval << " -> " << numBins << std::endl ;

#if 0
  itk::ImageFileWriter< ImageType>::Pointer dbgWriter=itk::ImageFileWriter< ImageType>::New();
  dbgWriter->SetFileName("distmap.nii.gz");
  dbgWriter->SetInput(foreground);
  dbgWriter->Update();
#endif

#ifdef ITK_USE_REVIEW_STATISTICS
  HistogramType::MeasurementVectorType minValVector(1), maxValVector(1);
#else
  HistogramType::MeasurementVectorType minValVector, maxValVector;
#endif
  minValVector[0] = minval;
  maxValVector[0] = maxval;

  HistogramType::Pointer histogram = HistogramType::New();
#ifdef ITK_USE_REVIEW_STATISTICS
  histogram->SetMeasurementVectorSize( 1 );
#endif
  histogram->Initialize( size, minValVector, maxValVector );

  // put each image pixel into the histogram
#ifdef ITK_USE_REVIEW_STATISTICS
  HistogramType::MeasurementVectorType measurement(1);
#else
  HistogramType::MeasurementVectorType measurement;
#endif
  ImageIteratorType iter (foreground, foreground->GetLargestPossibleRegion());
  iter.Begin();
  while ( !iter.IsAtEnd() )
    {
    const float value = iter.Get();
    measurement[0] = value;
    histogram->IncreaseFrequency( measurement , 1 );

    ++iter;
    }

  //////////////////////////////////////////////////////////////////////////
  //  This will use the histogram to find the desired ForegroundLevel.

  //   TInputImage::RegionType imageRegion = foreground->GetLargestPossibleRegion();
  //   int numVoxels = imageRegion.GetSize(0) * imageRegion.GetSize(1) * imageRegion.GetSize(2);
  //  std::cout << "m_HeadSizeLimit = " << m_HeadSizeLimit << " CCs" << std::endl;
  double DesiredVolumeToIncludeBeforeClipping = vcl_numeric_limits<double>::max();  // m_HeadSizeLimit is initialized to the smallest possible adult head in cubic cm.

#ifdef ITK_USE_REVIEW_STATISTICS
  HistogramType::Iterator histoIter = histogram->Begin();
#else
  HistogramType::Iterator histoIter;
#endif
  HistogramType::IndexType index;
  HistogramType::InstanceIdentifier instance;

  double CummulativeVolume = 0;    //  We need to skip over the zero bin.
  bool exitLoop = false;

  histoIter = histogram->Begin();
  ++histoIter;//Skip the zero bins.

  instance =  histoIter.GetInstanceIdentifier();
  index =  histogram->GetIndex(instance);
  maxValVector = histogram->GetHistogramMaxFromIndex(index);
  double SupInf_thickness=0;
  double RLbyAP_area=0;
  {
  PointType physOrigin;
  {
  IndexType origin;
  origin[0]=0;
  origin[1]=0;
  origin[2]=0;
  this->GetInput()->TransformIndexToPhysicalPoint(origin,physOrigin);
  }
  PointType physOriginPlusOne;
  ContinuousIndex<double,3> originPlusOne;
  originPlusOne[0]=this->GetInput()->GetSpacing()[0];
  originPlusOne[1]=this->GetInput()->GetSpacing()[1];
  originPlusOne[2]=this->GetInput()->GetSpacing()[2];
  this->GetInput()->TransformContinuousIndexToPhysicalPoint(originPlusOne,physOriginPlusOne);
  //std::cout << "physOrigin         " << physOrigin        << std::endl;
  //std::cout << "physOriginPlusOne  " << physOriginPlusOne << std::endl;
  const double RL_thickness=vnl_math_abs(physOrigin[0]-physOriginPlusOne[0])*0.1;
  const double AP_thickness=vnl_math_abs(physOrigin[1]-physOriginPlusOne[1])*0.1;
  SupInf_thickness         =vnl_math_abs(physOrigin[2]-physOriginPlusOne[2])*0.1; //Convert to cm
  //std::cout << "TEST RL:  " << RL_thickness << " AP " << AP_thickness << std::endl;
  RLbyAP_area=RL_thickness*AP_thickness; //Convert to cm^2
  if(RLbyAP_area < 1e-5 || SupInf_thickness < 1e-5 )
    {
    //std::cout << "  " << SupInf_thickness << std::endl;
    //std::cout << "  " << RL_thickness << std::endl;
    //std::cout << "  " << AP_thickness << std::endl;
    //std::cout << "  " << vnl_math_abs(physOrigin[1]-physOriginPlusOne[1]) << std::endl;

    std::cout << "ERROR:  Can not have zero area, or zero thickness." << std::endl;
    std::cout << this->GetInput() << std::endl;
    exit(-1);
    }
  }

  double topOfHeadDistFromExtremeSI = -1;
  double CurrentDistanceFromTopOfHead=0;
  double largestAreaRadius=0;
  double MaxCrossSectionalArea=0.0;
  std::cout << "zero bin count to be skipped = " << histoIter.GetFrequency() << std::endl;
  for (; (histoIter != histogram->End() && ! exitLoop ) ; ++histoIter)
    {
    instance =  histoIter.GetInstanceIdentifier();
    index =  histogram->GetIndex(instance);
    maxValVector = histogram->GetHistogramMaxFromIndex(index);
    minValVector = histogram->GetHistogramMinFromIndex(index);
    if(histoIter.GetFrequency() < 50 )
      {
      continue;
      }
    const double CurrentCrossSectionalArea=histoIter.GetFrequency()*RLbyAP_area;
    if(topOfHeadDistFromExtremeSI < 0 && CurrentCrossSectionalArea > 10.0 )
      {
      topOfHeadDistFromExtremeSI=maxValVector[0];
      }
    CurrentDistanceFromTopOfHead = (maxValVector[0]-topOfHeadDistFromExtremeSI);
    ForegroundLevel=maxValVector[0];

    if(
       (CurrentDistanceFromTopOfHead > 70.0 ) //Require at least 70mm from top  of head before considering stoping.
       &&
       (
        (CurrentCrossSectionalArea > MaxCrossSectionalArea )
        && (
            ( MaxCrossSectionalArea < 10 )
            || (CurrentCrossSectionalArea < MaxCrossSectionalArea*1.20 )  //Sometimes histogram bins are filled with 2 slices, and that needs to be avoided.
            )
        )
       )
      {
      MaxCrossSectionalArea = CurrentCrossSectionalArea;
      const double estimated_radius=vcl_sqrt(MaxCrossSectionalArea/vnl_math::pi); //Estimate the radis of a circle filling this much space
      //Now compute 1.5 times the size of a sphere with this estimated radius.
      const double ScaleFactor=1.1;// Add 10% for safety  //5+(MaxCrossSectionalArea-200)/100; //Larger brains need more scaling
      const double CurentVolumeBasedOnArea=ScaleFactor*(1.33333333333333333*vnl_math::pi*estimated_radius*estimated_radius*estimated_radius);
      DesiredVolumeToIncludeBeforeClipping=CurentVolumeBasedOnArea;
      //std::cout << "TESTING:  Radius: " << estimated_radius << " DesiredVolume " << DesiredVolumeToIncludeBeforeClipping << std::endl;
      }
    const double CurrentCrossSectionalVolume=histoIter.GetFrequency()*voxelSize;
    CummulativeVolume += CurrentCrossSectionalVolume;
    largestAreaRadius=vcl_pow(0.75*vnl_math::one_over_pi*CummulativeVolume,0.33333333333333333); //Assuming Sphere, what is radius.
#if 0
    std::cout << "HISTOGRAM: " << index << "," << histoIter.GetFrequency() << "," << minValVector[0] << "," << maxValVector[0] << std::endl ;
    // std::cout << " SupInf_thickness " << SupInf_thickness;
    // std::cout << " RLbyAP_area " << RLbyAP_area;
    std::cout << "ForegroundLevel = " << ForegroundLevel;
    std::cout << " CUMULATIVEVolume: " << CummulativeVolume;
    std::cout << " MAXCrossSectonalArea: " << MaxCrossSectionalArea << "  CurrentCrossSectionalArea: " << CurrentCrossSectionalArea<< " DesiredVolumeToIncludeBeforeClipping: " << DesiredVolumeToIncludeBeforeClipping << std::endl;
#endif
    if (  (CurrentDistanceFromTopOfHead > 100.0)  // Can not stop before 100 mm from top of head are reached.
          && (CummulativeVolume >= DesiredVolumeToIncludeBeforeClipping)   // Maximum sustainable volume based on max area of any slice.
          )
      {
      std::cout << "VOLUME CRITERIA MET, so exiting. " << CummulativeVolume << " >= " << DesiredVolumeToIncludeBeforeClipping  << std::endl;
      exitLoop = true;
      }
#if 0 //This just does not work in many cases.
    if (( ( CurrentCrossSectionalArea  > MaxCrossSectionalArea*.25 ) && ( CurrentCrossSectionalArea < 0.65* MaxCrossSectionalArea ) )  //If a constriction of 75% max occurs, then assume that the neck area has been entered.
        )
      {
      std::cout << "NECK CONSTRICTION CRITERIA MET, so exiting." << std::endl;
      exitLoop = true;
      }
#endif
    // Now ForegroundLevel represents where to threshold the head from the neck.
    }
  //NOTE:  1 radius was based on some empircal work done by Hans on 100's of data sets.
  SI_CenterBasedOnTopOfHead=extremum-(topOfHeadDistFromExtremeSI+largestAreaRadius*10.0);
  std::cout << "ForegroundLevel = " << ForegroundLevel << " topOfHeadDistFromExtremeSI " << topOfHeadDistFromExtremeSI 
            << " Y_Location_from_Top_of_Head: = "  << SI_CenterBasedOnTopOfHead <<  std::endl;
  }


  //////////////////////////////////////////////////////////////////////////
  //  This will convert the foreground code image with the rule, foreach voxel,
  //  if not in tissue outline or distance map is greater than T, set the result image voxel to Background;
  //  otherwise set the result image voxel to the source image pixel value.
  {
  ImageIteratorType ClippedImagePixel( foreground, foreground->GetLargestPossibleRegion() );
  ImageConstIteratorType OriginalImagePixel( this->GetInput(), this->GetInput()->GetLargestPossibleRegion() );

  ClippedImagePixel.Begin();
  for (; !ClippedImagePixel.IsAtEnd(); ++ClippedImagePixel)
    {
    if (ClippedImagePixel.Get() != 0)
      {
      if (ClippedImagePixel.Get() <= ForegroundLevel)
        {
        OriginalImagePixel.SetIndex( ClippedImagePixel.GetIndex() );
        ClippedImagePixel.Set( OriginalImagePixel.Get() );
        }
      else
        {
        ClippedImagePixel.Set( this->m_BackgroundValue );
        }
      }
    else
      {
      ClippedImagePixel.Set( this->m_BackgroundValue );
      }
    }
  // Now foreground holds the clipped image.
  }


  //////////////////////////////////////////////////////////////////////////
  //  This will use the clipped foreground image to get the head center of mass.
  //  PointType this->m_CenterOfBrain;
  {
  typedef typename itk::ImageMomentsCalculator<ImageType> MomentsCalculatorType;
  typename MomentsCalculatorType::Pointer moments = MomentsCalculatorType::New();
  moments->SetImage(foreground);
  moments->Compute();
  typename PointType::VectorType tempCenterOfMass = moments->GetCenterOfGravity();
  for( unsigned int q=0;q<ImageType::ImageDimension;q++ )
    {
    this->m_CenterOfBrain[q]=tempCenterOfMass[q];
    }
  }

  //////////////////////////////////////////////////////////////////////////
  //  This will splice the center of mass SI coordinate with the correct estimate.
  this->m_CenterOfBrain[2] = SI_CenterBasedOnTopOfHead;
  
}

}
