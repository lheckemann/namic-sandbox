#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
{
  std::cout << "ImageProcessor created" << std::endl;
  mLocalInputImage = FloatImageType::New();
  mLocalOutputImage = FloatImageType::New();
  mLocalTmp1 = FloatImageType::New();
  mLocalTmp2 = FloatImageType::New(); 
  mScalarSize = 0;
  mWhichTmp = 0;
} 

ImageProcessor::~ImageProcessor()
{
  if(mLocalInputImage)
    mLocalInputImage->Delete();
  if(mLocalOutputImage)
    mLocalOutputImage->Delete();
  if(mLocalTmp1)
    mLocalTmp1->Delete();
  if(mLocalTmp2)
    mLocalTmp2->Delete();
  std::cout << "ImageProcessor destructed" << std::endl;
}

//TODO:Steve Do those Smartpointers of the filters get deleted after returning from the function?
ImageProcessor::FloatImageType::Pointer ImageProcessor::RescaleUCharToFloat(UCharImageType::Pointer inputImage)
{
  RescaleUCharToFloatFilter::Pointer rescale = RescaleUCharToFloatFilter::New();  
  rescale->SetOutputMinimum(0.0);
  rescale->SetOutputMaximum(MAX);
  rescale->SetInput(inputImage);
  rescale->Update();
  return rescale->GetOutput();
}
ImageProcessor::FloatImageType::Pointer ImageProcessor::RescaleUShortToFloat(UShortImageType::Pointer inputImage)
{
  RescaleUShortToFloatFilter::Pointer rescale = RescaleUShortToFloatFilter::New();  
  rescale->SetOutputMinimum(0.0);
  rescale->SetOutputMaximum(MAX);
  rescale->SetInput(inputImage);
  rescale->Update();
  return rescale->GetOutput();
}

ImageProcessor::UCharImageType::Pointer ImageProcessor::RescaleFloatToUChar(FloatImageType::Pointer outputImage)
{
  RescaleToUCharFilter::Pointer rescale = RescaleToUCharFilter::New();  
  rescale->SetOutputMinimum(  0);
  rescale->SetOutputMaximum(255);
  rescale->SetInput(outputImage);
  rescale->Update();
  return rescale->GetOutput();
}

ImageProcessor::UShortImageType::Pointer ImageProcessor::RescaleFloatToUShort(FloatImageType::Pointer outputImage)
{
  RescaleToUShortFilter::Pointer rescale = RescaleToUShortFilter::New();  
  rescale->SetOutputMinimum(  0);
  rescale->SetOutputMaximum(MAXOUTPUT); //TODO:Change this back to MAX later on or make MAXOUTPUT the maximum of the inputimage
  rescale->SetInput(outputImage);
  rescale->Update();
  return rescale->GetOutput();
}

// Sets the input and initializes all the variables again
void ImageProcessor::SetImage(void* pImage, int xSize, int ySize, int scalarSize, double spacing[3], double origin[3])
{
  mScalarSize = scalarSize;  
  mWhichTmp = 0; 
  mLocalInputImage->FillBuffer(0);
  mLocalOutputImage->FillBuffer(0);
  mLocalTmp1->FillBuffer(0);
  mLocalTmp2->FillBuffer(0); 
  double spacingFilterImage[2];
  spacingFilterImage[0] = spacing[0];
  spacingFilterImage[1] = spacing[1];
  double originFilterImage[2];
  originFilterImage[0] = origin[0];
  originFilterImage[1] = origin[1];
  
  if(scalarSize == 1)
  {
    // Hard-copies pImage to the image buffer of UCharImageType    
    UCharImageType::Pointer image = ImageProcessor::UCharImageType::New();
    
    UCharImageType::SizeType   sizeFilterImage;
    UCharImageType::IndexType  startFilterImage; //startIndex in ImageRegion
    UCharImageType::RegionType regionFilterImage;
    sizeFilterImage[0] = xSize;
    sizeFilterImage[1] = ySize;
    startFilterImage[0] = 0;
    startFilterImage[1] = 0;
    regionFilterImage.SetSize(sizeFilterImage);
    regionFilterImage.SetIndex(startFilterImage);
    
    image->SetRegions(regionFilterImage);
    image->Allocate();
    image->SetSpacing(spacingFilterImage);
    image->SetOrigin(originFilterImage);
   
    // Hard-copy the image buffer byte by byte
    UCharIteratorType it(image, regionFilterImage);
    it.GoToBegin();
    const unsigned char* data = (unsigned char*) pImage; 
    while(! it.IsAtEnd())
    {
      it.Set(*data);
      ++it;
      ++data;
    }
    mLocalInputImage = RescaleUCharToFloat(image);
  }
  else if(scalarSize == 2)
  {
    UShortImageType::Pointer image = ImageProcessor::UShortImageType::New();
  
    UShortImageType::SizeType   sizeFilterImage;
    UShortImageType::IndexType  startFilterImage; //startIndex in ImageRegion
    UShortImageType::RegionType regionFilterImage;
    sizeFilterImage[0] = xSize;
    sizeFilterImage[1] = ySize;
    startFilterImage[0] = 0;
    startFilterImage[1] = 0;    
    regionFilterImage.SetSize(sizeFilterImage);
    regionFilterImage.SetIndex(startFilterImage);
    
    image->SetRegions(regionFilterImage);
    image->Allocate();
    image->SetSpacing(spacingFilterImage);
    image->SetOrigin(originFilterImage);
   
    // Hard-copy the image buffer 2bytes at a time
    UShortIteratorType it(image, regionFilterImage);
    it.GoToBegin();
    const unsigned short* data = (unsigned short*) pImage;
    while(! it.IsAtEnd())
    {    
      it.Set(*data);
      ++it;
      ++data;
    }
    mLocalInputImage = RescaleUShortToFloat(image);    
  }
  else if(scalarSize == 4)
    std::cerr << "SetImage did not succeed! This scalar size is not supported yet." << std::endl;
  else 
  {
    std::cerr << "SetImage did not succeed! Unsupported scalar size." << std::endl;
  }
}

void ImageProcessor::GetImage(void* pImage)
{
  if(mScalarSize == 1)
  {
    unsigned char* pImageChar = (unsigned char*) pImage;
    UCharImageType::Pointer image;
    image = RescaleFloatToUChar(mLocalOutputImage);
    UCharImageType::RegionType region;
    region = image->GetBufferedRegion();
    UCharIteratorType it(image, region);
    unsigned int i = 0;
    
    it.GoToBegin();
    while(! it.IsAtEnd())
    {
      pImageChar[i] = it.Get();
      ++i;
      ++it;
    }
  }
  else if(mScalarSize == 2)
  {
    unsigned short* pImageShort = (unsigned short*) pImage;
    UShortImageType::Pointer image;
    image = RescaleFloatToUShort(mLocalOutputImage);
    UShortImageType::RegionType region;
    region = image->GetBufferedRegion();
    UShortIteratorType it(image, region);
    unsigned int i = 0;
    
    it.GoToBegin();
    while(! it.IsAtEnd())
    {
      pImageShort[i] = it.Get();
      ++i;
      ++it;
    }
  }
  else if(mScalarSize == 4)
    std::cerr << "Error! Could not get image! Not yet supported scalar size!" << std::endl; // program should never get here!
  else
    std::cerr << "Error! Could not get image! Unsupported scalar size!" << std::endl; // program should never get here!
}

void ImageProcessor::GradientMagnitude(bool inputTmp, bool outputTmp)
{
  itk::GradientMagnitudeImageFilter<FloatImageType, FloatImageType>::Pointer
    gradFilter =  itk::GradientMagnitudeImageFilter<FloatImageType, FloatImageType>::New();
  if(inputTmp && (mWhichTmp == 1))
    gradFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    gradFilter->SetInput(mLocalTmp2);
  else
    gradFilter->SetInput(mLocalInputImage);
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = gradFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = gradFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = gradFilter->GetOutput();
  gradFilter->Update();
  return;
}

//If threshold gets used with -1 and -2 as second parameter it works like ThresholdBelow and ThresholdAbove with respect to the outside value and the first threshold
void ImageProcessor::Threshold(bool inputTmp, bool outputTmp, int outsideValue, int firstThresh, int secondThresh)
{ 
  itk::ThresholdImageFilter<FloatImageType>::Pointer
    threshFilter = itk::ThresholdImageFilter<FloatImageType>::New();
  if(inputTmp && (mWhichTmp == 1))
    threshFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    threshFilter->SetInput(mLocalTmp2);
  else
    threshFilter->SetInput(mLocalInputImage);
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = threshFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = threshFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = threshFilter->GetOutput();
    
  threshFilter->SetOutsideValue(outsideValue);
  if(secondThresh >= 0)
    threshFilter->ThresholdOutside(firstThresh,secondThresh);
  else if(secondThresh == -1)
    threshFilter->ThresholdBelow(firstThresh);
  else if(secondThresh == -2)
    threshFilter->ThresholdAbove(firstThresh);
  else
    std::cerr << "Error! ImageProcessor::Threshold was not used with the right parameters!" << std::endl;
  threshFilter->Update();
  return;
}

void ImageProcessor::Invert(bool inputTmp, bool outputTmp)
{
  InverterType::Pointer inverter = InverterType::New();
  if(inputTmp && (mWhichTmp == 1))
    inverter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    inverter->SetInput(mLocalTmp2);
  else
    inverter->SetInput(mLocalInputImage);
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = inverter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = inverter->GetOutput();
    }
  }
  else
    mLocalOutputImage = inverter->GetOutput();
  
  try
  {
    inverter->Update();
    }
      catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught updating the thinning filter!" << std::endl;
    std::cout << err << std::endl;
  }  
  return;
}

// The HoughTransformation finds a line in the image | it finds bright lines
// An inversion of colors is included, because the needle appears dark in the original image
// It assumes the needle is entering from the right side of the image pointing to the left
//     inputTmp:                Input image is coming from localTmp or localInputImg
//     points:                  Array that contains 2 points of the needle transform (x1,y1,x2,y2)
//                              points[0],points[1] is the point of the needle entering the image | points[2],points[3] is the end of the needle
//     intensityThresh:         intensity until which a point is considered belonging to the needle
//     needleEnteringDirection: Entering direction of the needle in the image set in SetImage -> either ENTERINGRIGHT or ENTERINGBOTTOM
void ImageProcessor::HoughTransformation(bool inputTmp, double* points, double intensityThresh, int needleEnteringDirection) 
{
  InverterType::Pointer inverter = InverterType::New();
  HoughFilter::Pointer houghFilter = HoughFilter::New();
  double initialIntensity = -1; //average intensity of the first 6 pixels of the needle
  double avgIntensity  = -1;  //"average" intensity of the needle pixels
  double lastIntensity = MAX; //intensity of last pixel examined | used to compare with the current pixel finding the end of the needle
  int  length          = 0;   //length of the found needle in pixels
    
  if(inputTmp && (mWhichTmp == 1))
    houghFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    houghFilter->SetInput(mLocalTmp2);
  else
    houghFilter->SetInput(mLocalInputImage);
  
  houghFilter->SetNumberOfLines(1);
  houghFilter->SetVariance(1);   // default is 10 -> 1 means no blurring with the gaussian
  houghFilter->SetDiscRadius(5);  // default is 5 (this is the size of the surrounding disc in houghspace that gets taken out when a line is found. Since I only need one line, I do not need this)
  // Don't need to do houghFilter->Update(), because it gets updated through houghFilter->GetLines
 
  //--------------------------------------------------------------------------------------------------
  // Get line from HoughTransformation
  //TODO: really hard copy the image! | right now it is just a reference copy -> mLocalInputImage changes, too
  mLocalOutputImage = mLocalInputImage; // Output image = input image with line drawn into it   //TODO: change the used image to mLocalInputImage, because I do not draw the line in the image anymore
  HoughFilter::LinesListType lines = houghFilter->GetLines(1);
  HoughFilter::LinesListType::const_iterator itLines = lines.begin();

  typedef HoughFilter::LineType::PointListType  PointListType;  
  PointListType                   pointsList = (*itLines)->GetPoints();
  PointListType::const_iterator   itPoints = pointsList.begin();
  
  double u[2]; // first point of the line
  u[0] = (*itPoints).GetPosition()[0];
  u[1] = (*itPoints).GetPosition()[1];
  itPoints++;
  double v[2]; // direction vector of the line
  v[0] = u[0]-(*itPoints).GetPosition()[0];
  v[1] = u[1]-(*itPoints).GetPosition()[1];
  double norm = sqrt(v[0]*v[0]+v[1]*v[1]);
  v[0] /= norm;
  v[1] /= norm; 
  std::cout << " | | u=" << u[0] << "|" << u[1] << " v=" << v[0] << "|" << v[1] << std::endl;   
          
  //--------------------------------------------------------------------------------------------------
  //follow the dark line in the original by comparing intensities
  //TODO: This simply follows the straight line found by the hough transform -> make this follow a curved line by following the darkest spot
  FloatImageType::IndexType localIndex;
  FloatImageType::IndexType offsetIndex; // for checking adjacent pixels 
  itk::Size<2> size = mLocalOutputImage->GetLargestPossibleRegion().GetSize();
  std::cout << " | | size: " << size[0] << "|" << size[1] << std::endl;
  double multiplier = 0;
  
  if(needleEnteringDirection == ENTERINGRIGHT)
  {
    //normalize the support vector to x = xSize | v[0] = size[0]  
    multiplier = (u[0]-size[0]) / v[0]; // ==u[0]-m*v[0]=size[0] 
  }
  else if(needleEnteringDirection == ENTERINGBOTTOM)
  {
    //normalize the support vector to y = ySize | v[1] = size[1] -> in ITKimages the origin is in the upper left corner  
    multiplier = (u[1]-size[1]) / v[1]; // ==u[1]-m*v[1]=size[1]
  }
  else //Code should never get here!!!
  {
    std::cerr << "wrong argument for needleEnteringDirection! Abort hough transformation!" << std::endl;
    return;    
  }
    
  // find the x and y coordinates of the first point of the line in the image and use these instead of a random point u on the line
  u[0] -= multiplier * v[0];
  u[1] -= multiplier * v[1];  
  
  //normalize the direction vector to negative, because the needle enters from right or bottom and in the ITK coordinate system these are the highest values
  if(multiplier < 0.0) 
  {
    v[0] *= -1;
    v[1] *= -1;
  }  
        
  float diag = sqrt((float)( size[0]*size[0] + size[1]*size[1] ));
  std::cout << " | | line: " << u[0] << "|" << u[1] << " / " << v[0] <<"|" << v[1] << " mult: " << multiplier << std::endl;
  std::cout << " | | diag: " << diag << std::endl;

  //  The line might be as long as the diagonal, thus that many pixels have to be computed
  //  and every iteration they get checked, if they are in the bounds of the mLocalOutputImage
  for(int i=0; i<=static_cast<int>(diag); i++)
  {
    
    offsetIndex[0] = localIndex[0]=(long int)(u[0]+i*v[0])+1;
    offsetIndex[1] = localIndex[1]=(long int)(u[1]+i*v[1]);
//    localIndex[0]=(long int)(u[0]+i*v[0])+1; // offset of 1 for the line, because it is detected a little too high for NeedleEntering == BOTTOM
//    localIndex[1]=(long int)(u[1]+i*v[1])+1; // offset of 1 for the line, because it is detected a little too high for NeedleEntering == Left

    FloatImageType::RegionType outputRegion =  mLocalOutputImage->GetLargestPossibleRegion(); //TODO: take out of for-loop!?
    
    bool pixelIsNeedle = false;    
    if(outputRegion.IsInside(localIndex)) // this pixel of the line is inside the boundings of mLocalOutputImage
    {
      if(length == 0) // first point of the line in the image region
      {
        points[0] = (u[0]+i*v[0]); // X-coordinate of the first point of the needle in the image
        points[1] = (u[1]+i*v[1]); // Y-coordinate of the first point of the needle in the image
        pixelIsNeedle = true;
        //TODO:use initial intensity or take it out again! It does not get used yet
        initialIntensity = 0;
        for(int j = -1; j < 2; ++j)
          for(int k = -1; k < 1; ++k)
          {
            if(needleEnteringDirection == ENTERINGRIGHT)
            {
              offsetIndex[0] = localIndex[0]+k;
              offsetIndex[1] = localIndex[1]+j;
            }
            else if(needleEnteringDirection == ENTERINGBOTTOM)
            {
              offsetIndex[0] = localIndex[0]+j;
              offsetIndex[1] = localIndex[1]+k;
            }
            initialIntensity += mLocalOutputImage->GetPixel(offsetIndex);
            std::cout << " | | " << offsetIndex[0] << "&" << offsetIndex[1] << "->";
            std::cout << mLocalOutputImage->GetPixel(offsetIndex) << std::endl;
          }
        initialIntensity /= 6;
        std::cout << " | | initial intensity:" << initialIntensity << std::endl;
      }
      else
      {        
        for (int j = -1; j < 2; ++j) //TODO: maybe use the initial intensity
        {
          if(needleEnteringDirection == ENTERINGRIGHT) 
            offsetIndex[1] = localIndex[1]+j; //Y-1, Y, Y+1
          else if(needleEnteringDirection == ENTERINGBOTTOM) 
            offsetIndex[0] = localIndex[0]+j;    //X-1, X, X+1    
            
          std::cout << " | | " << offsetIndex[0] << "&" << offsetIndex[1] << "->" << mLocalOutputImage->GetPixel(offsetIndex) << "|";

          if(abs((int) (mLocalOutputImage->GetPixel(offsetIndex) - avgIntensity)) <= intensityThresh) 
            pixelIsNeedle = true;            
        }          
        std::cout << std::endl;
      }
      
      if(!pixelIsNeedle) // if supposedly does not belong to the needle anymore
      {        
        //maybe it is the bright part that belongs to the lower part of the artifact -> then there follows a really dark part
        float brightIntens = mLocalOutputImage->GetPixel(localIndex);
        std::cout << " | | " << "brightIntens" << brightIntens << std::endl; 
        for (int j = 2; j < 4; ++j) 
        {
          offsetIndex[0] = localIndex[0];
          offsetIndex[1] = localIndex[1];
          if(needleEnteringDirection == ENTERINGRIGHT) 
            offsetIndex[0] = localIndex[0]-j; //X-2, X-3
          else if(needleEnteringDirection == ENTERINGBOTTOM) 
            offsetIndex[1] = localIndex[1]-j;    //Y-2, Y-3    
            
          std::cout << " | | " << offsetIndex[0] << "&" << offsetIndex[1] << "->" << mLocalOutputImage->GetPixel(offsetIndex) << std::endl;

          if(mLocalOutputImage->GetPixel(offsetIndex) <= brightIntens/3)
          {
            //pixelIsNeedle = true;
            i += 8;  // jump over the dark artifact
            length += 8;
            break;
          }         
        } 
      }  
      else  // if pixel still belongs to needle
      {
        length++;
        lastIntensity = mLocalOutputImage->GetPixel(localIndex);
        avgIntensity += mLocalOutputImage->GetPixel(localIndex);
        if(length>1) // do not devide by two for the first one
          avgIntensity /= 2;
      }  
      
      if(!pixelIsNeedle) // found the end of the needle       
      {  
        std::cout << " | | found the end of the needle" << std::endl;
        points[2] = (u[0]+i*v[0]);  // X-coordinate of the needle tip
        points[3] = (u[1]+i*v[1]);  // Y-coordinate of the needle tip
        break;
      }
      //mLocalOutputImage->SetPixel(localIndex, 0); makes the pixel black that is supposed to belong to the needle
    }
    else if(length > 0) // needle exists and needle tip is outside of theimage region
    {  
      std::cout << " | | needle tip outside" << std::endl;
      points[2] = (u[0]+i*v[0]);  // X-coordinate of the end of the needle
      points[3] = (u[1]+i*v[1]);  // Y-coordinate of the end of the needle
      break;
    }
  }
//  if(length != 0)
//    avgIntensity /= length; 
  std::cout << " | | end of houghtransform - avgIntensity: " << avgIntensity << std::endl;
  return;
}
   
void ImageProcessor::CannyEdgeDetection(bool inputTmp, bool outputTmp)
{
  typedef itk::CannyEdgeDetectionImageFilter<FloatImageType, FloatImageType> CannyFilter;
  CannyFilter::Pointer cannyFilter = CannyFilter::New();
  if(inputTmp && (mWhichTmp == 1))
    cannyFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    cannyFilter->SetInput(mLocalTmp2);
  else
    cannyFilter->SetInput(mLocalInputImage);  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = cannyFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = cannyFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = cannyFilter->GetOutput();

  cannyFilter->SetVariance(5);
  cannyFilter->Update(); 
  return; 
}

void ImageProcessor::BinaryThreshold(bool inputTmp, bool outputTmp)
{    
  itk::BinaryThresholdImageFilter<FloatImageType, FloatImageType>::Pointer
    binFilter = itk::BinaryThresholdImageFilter<FloatImageType, FloatImageType>::New();
  if(inputTmp && (mWhichTmp == 1))
    binFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    binFilter->SetInput(mLocalTmp2);
  else
    binFilter->SetInput(mLocalInputImage);  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = binFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = binFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = binFilter->GetOutput();
  
  binFilter->SetOutsideValue(MAX);
  binFilter->SetInsideValue(0);
  binFilter->SetLowerThreshold(20000);
  binFilter->SetUpperThreshold(40000); 
  binFilter->Update();
  return;
}

void ImageProcessor::LaplacianRecursiveGaussian(double gaussVariance, bool inputTmp, bool outputTmp)
{
  itk::LaplacianRecursiveGaussianImageFilter<FloatImageType, FloatImageType>::Pointer lapFilter;
  lapFilter = itk::LaplacianRecursiveGaussianImageFilter<FloatImageType, FloatImageType>::New();
  if(inputTmp && (mWhichTmp == 1))
    lapFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    lapFilter->SetInput(mLocalTmp2);
  else
    lapFilter->SetInput(mLocalInputImage);  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = lapFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = lapFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = lapFilter->GetOutput();

  lapFilter->SetNormalizeAcrossScale(false);
  lapFilter->SetSigma((int) gaussVariance);
  try
  {
    lapFilter->Update();
    }
      catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught updating the lap filter!" << std::endl;
    std::cout << err << std::endl;
  }  
  return;
}

void ImageProcessor::SobelEdgeDetection(bool inputTmp, bool outputTmp)
{
  itk::SobelEdgeDetectionImageFilter<FloatImageType, FloatImageType>::Pointer sobelFilter;
  sobelFilter = itk::SobelEdgeDetectionImageFilter<FloatImageType, FloatImageType>::New();
  if(inputTmp && (mWhichTmp == 1))
    sobelFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    sobelFilter->SetInput(mLocalTmp2);
  else
    sobelFilter->SetInput(mLocalInputImage);  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = sobelFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = sobelFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = sobelFilter->GetOutput();

  //TODO: are there any input parameters of the sobel filter that can be set?  
  try
  {
    sobelFilter->Update();
    }
      catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught updating the sobel filter!" << std::endl;
    std::cout << err << std::endl;
  }  
  return;
}

void ImageProcessor::SobelFilter(bool inputTmp, bool outputTmp, int direction)    // Direction switching is not working yet, only x-axis
{
  FloatImageType::Pointer image; 
  if(inputTmp && (mWhichTmp == 1))
    image = mLocalTmp1;
  else if (inputTmp && (mWhichTmp == 2))
    image = mLocalTmp2;
  else
    image = mLocalInputImage;  
  int a = 2;
  int b = 1;

  FloatImageType::SizeType size;
  size[0] = a;         // x-Dimension: 5 Pixel, 2 into each direction from the center
  size[1] = b;        // y-Dimension: 3 Pixel, 1 into each direction from the center
  
  FloatNeighborhoodIteratorType it(size, image, image->GetRequestedRegion() );

  FloatImageType::Pointer output = FloatImageType::New();
  output->SetRegions(image->GetRequestedRegion());
  output->Allocate();
  FloatIteratorType out(output, image->GetRequestedRegion());
    
  FloatNeighborhoodIteratorType::OffsetType position1  = {{ a , b }};
  FloatNeighborhoodIteratorType::OffsetType position2  = {{a-1, b }};
  //FloatNeighborhoodIteratorType::OffsetType position3  = {{a-2, b }};
  FloatNeighborhoodIteratorType::OffsetType position4  = {{a-3, b }};
  FloatNeighborhoodIteratorType::OffsetType position5  = {{a-4, b }};
  FloatNeighborhoodIteratorType::OffsetType position6  = {{ a, b-1}};
  FloatNeighborhoodIteratorType::OffsetType position7  = {{a-1,b-1}};
  FloatNeighborhoodIteratorType::OffsetType position8  = {{a-3,b-1}};
  FloatNeighborhoodIteratorType::OffsetType position9  = {{a-4,b-1}};
  FloatNeighborhoodIteratorType::OffsetType position10 = {{ a ,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position11 = {{a-1,b-2}};
  //FloatNeighborhoodIteratorType::OffsetType position12 = {{a-2,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position13 = {{a-3,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position14 = {{a-4,b-2}};
  
  for (it.GoToBegin(), out.GoToBegin(); !it.IsAtEnd(); ++it, ++out)
  {
    float sum = 0;
    sum =  it.GetPixel(position10) + it.GetPixel(position1);
    sum += it.GetPixel(position11) + it.GetPixel(position2);
    //sum += it.GetPixel(position12) + it.GetPixel(position3);
    sum += it.GetPixel(position13) + it.GetPixel(position4);
    sum += it.GetPixel(position14) + it.GetPixel(position5);
    sum -= 2.0 * it.GetPixel(position6);
    sum -= 2.0 * it.GetPixel(position7);
    sum -= 2.0 * it.GetPixel(position8);
    sum -= 2.0 * it.GetPixel(position9); 
    if(sum < 0)
      sum = 0;
    out.Set(sum);
  }
  
  // TODO: Rescale from float to float
  output = RescaleUShortToFloat(RescaleFloatToUShort(output));
  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = output;
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = output;
    }
  }
  else
    mLocalOutputImage = output;
  return;
}

void ImageProcessor::DilateAndErode(bool inputTmp, bool outputTmp, int erode, int dilate) //dilates and erodes bright spots in the image
{                                                                  
  DilateFilterType::Pointer grayscaleDilate = DilateFilterType::New();  
  ErodeFilterType::Pointer grayscaleErode = ErodeFilterType::New();
  
  if(inputTmp && (mWhichTmp == 1))
    grayscaleDilate->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    grayscaleDilate->SetInput(mLocalTmp2);
  else
    grayscaleDilate->SetInput(mLocalInputImage);  
  
  StructuringElementType structuringElementDilate;
  StructuringElementType structuringElementErode;  
  structuringElementDilate.SetRadius(dilate); // if dilate==2 -> 5x5 structuring
  structuringElementErode.SetRadius(erode); // if erode==3 -> 7x7 structuring
  structuringElementDilate.CreateStructuringElement();
  structuringElementErode.CreateStructuringElement();
  
  grayscaleDilate->SetKernel(structuringElementDilate);
  grayscaleErode->SetKernel(structuringElementErode);
     
  grayscaleErode->SetInput(grayscaleDilate->GetOutput() );
  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = grayscaleErode->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = grayscaleErode->GetOutput();
    }
  }
  else
    mLocalOutputImage = grayscaleErode->GetOutput();

  try
  {
    grayscaleErode->Update();
    }
      catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught updating the eroding filter!" << std::endl;
    std::cout << err << std::endl;
  }  
  return;
}

void ImageProcessor::BinaryThinning(bool inputTmp, bool outputTmp)
{
  itk::BinaryThinningImageFilter<FloatImageType, FloatImageType>::Pointer thinFilter;
  thinFilter = itk::BinaryThinningImageFilter<FloatImageType, FloatImageType>::New();
  if(inputTmp && (mWhichTmp == 1))
    thinFilter->SetInput(mLocalTmp1);
  else if (inputTmp && (mWhichTmp == 2))
    thinFilter->SetInput(mLocalTmp2);
  else
    thinFilter->SetInput(mLocalInputImage);     

  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = thinFilter->GetOutput();
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = thinFilter->GetOutput();
    }
  }
  else
    mLocalOutputImage = thinFilter->GetOutput();
  
  try
  {
    thinFilter->Update();
    }
      catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught updating the thinning filter!" << std::endl;
    std::cout << err << std::endl;
  }  
  return;
}

// only writes grayscale integer 2D images
void ImageProcessor::Write(const char* filePath, int whichImage)
{
  itk::ImageFileWriter<UCharImageType>::Pointer
    writer = itk::ImageFileWriter<UCharImageType>::New();
  writer->SetFileName(filePath);
  switch(whichImage)
  {
    case INPUT: 
      writer->SetInput(RescaleFloatToUChar(mLocalInputImage));
      std::cout << "inputimage selected to write to file   ";
      break;
    case TMP:
      std::cout << "Tmp selected to write to file   ";
      if(mWhichTmp == 1) 
        writer->SetInput(RescaleFloatToUChar(mLocalTmp1));
      else if(mWhichTmp == 2)
        writer->SetInput(RescaleFloatToUChar(mLocalTmp2));
      else // this should never happen!
        std::cerr << "ERROR! invalid temporary file! No image written to file" << std::endl;
      break;
    case  OUTPUT:
      writer->SetInput(RescaleFloatToUChar(mLocalOutputImage));
      std::cout << "outputimage selected to write to file   ";
      break;
    default:
      std::cerr << "!!invalid selection to write to file!!" << std::endl;
  } // end switch
  
  try
  {
    writer->Update();
    std::cout << "wrote image to file" << std::endl;
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }
  return;
}

