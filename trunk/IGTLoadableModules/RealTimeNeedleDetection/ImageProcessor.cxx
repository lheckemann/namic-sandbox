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
  mLocalInputImage->Delete();
  mLocalOutputImage->Delete();
  mLocalTmp1->Delete();
  mLocalTmp2->Delete();
  std::cout << "ImageProcessor destructed" << std::endl;
 //TODO: delete properly 
}

//Change from little to big Endian | not used anymore
void ImageProcessor::changeEndian(unsigned short* data)
{
  *data = ((*data&0x00ff)<<8)+((*data&0xff00)>>8);
  *data = ((*data&0x0f0f)<<4)+((*data&0xf0f0)>>4);
  *data = ((*data&0x3333)<<2)+((*data&0xcccc)>>2);
  *data = ((*data&0x5555)<<1)+((*data&0xaaaa)>>1);  
}

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
  rescale->SetOutputMaximum(MAXOUTPUT); //TODO:Change this back to MAX later on
  rescale->SetInput(outputImage);
  rescale->Update();
  return rescale->GetOutput();
}

// Sets the input and initializes all the variables again
void ImageProcessor::SetImage(void* pImage, int xSize, int ySize, int scalarSize, double spacing[3], double origin[3])
{
  mScalarSize = scalarSize;  
  mWhichTmp = 0; 
  mLocalInputImage = FloatImageType::New();
  mLocalOutputImage = FloatImageType::New();
  mLocalTmp1 = FloatImageType::New();
  mLocalTmp2 = FloatImageType::New(); 
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
    
    UCharImageType::SizeType sizeFilterImage;
    UCharImageType::IndexType startFilterImage; //startIndex in ImageRegion
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
  
    UShortImageType::SizeType sizeFilterImage;
    UShortImageType::IndexType startFilterImage; //startIndex in ImageRegion
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
  {
    mLocalOutputImage = gradFilter->GetOutput();
    mWhichTmp = 0;
  }
  gradFilter->Update();
}

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
  {
    mLocalOutputImage = threshFilter->GetOutput();
    mWhichTmp = 0;
  }
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
}

// The HoughTransformation finds a line in the image, it looks for bright lines
// An inversion of colors is included, because the needle appears dark in the original image
void ImageProcessor::HoughTransformation(bool inputTmp, bool outputTmp, double* points)
{
  InverterType::Pointer inverter = InverterType::New();
  HoughFilter::Pointer houghFilter = HoughFilter::New();
  int numberOfLines    = 1;
  double avgIntensity  = 0;   //average intensity of the pixels of the needle
  double lastIntensity = MAX; //intensity of last pixel examined, used to compare with the current pixel finding the end of the needle
  int  length          = 0;   //length of the found needle in pixels
    
  if(inputTmp && (mWhichTmp == 1))
  {
    std::cout << "mLocalTmp1" << std::endl;
    inverter->SetInput(mLocalTmp1);
  }
  else if (inputTmp && (mWhichTmp == 2))
  {
    std::cout << "mLocalTmp2" << std::endl;
    inverter->SetInput(mLocalTmp2);
  }
  else
  {
    std::cout << "mLocalInput" << std::endl;
    inverter->SetInput(mLocalInputImage);
  }
  
  itk::ImageFileWriter<UCharImageType>::Pointer writer = itk::ImageFileWriter<UCharImageType>::New();
  writer->SetFileName("/projects/mrrobot/goerlitz/test/inverted.png");
  writer->SetInput(RescaleFloatToUChar(inverter->GetOutput()));
  writer->Update();
  
  houghFilter->SetInput(inverter->GetOutput()); 
  houghFilter->SetNumberOfLines(numberOfLines);
  houghFilter->SetVariance(1);   // default is 10 -> no blurring with the gaussian
  houghFilter->SetDiscRadius(5);  // default is 5
  // Don't need to do houghFilter->Update(), because it gets updated through houghFilter->GetLines
 
  //--------------------------------------------------------------------------------------------------
  //get line from HoughTransformation
  mLocalOutputImage = mLocalInputImage;
  HoughFilter::LinesListType lines = houghFilter->GetLines(numberOfLines);

  HoughFilter::LinesListType::const_iterator itLines = lines.begin();
  while(itLines != lines.end()) // TODO:useless, because I only have one line
  {  
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
            
    //--------------------------------------------------------------------------------------------------
    //draw found lines in mLocalInputImage        
    FloatImageType::IndexType localIndex;
    itk::Size<2> size = mLocalInputImage->GetLargestPossibleRegion().GetSize();
    //normalize the support vector to x = xSize  TODO: normalize to either x or y, if needle comes from the right or the bottom
    double multiplier = 0;
    multiplier = (u[0]-size[0]) / v[0]; // u[0]-multiplier*v[0] = size[0]
     
    // find the first x and y coordinates of the first point of the line in the image and use these instead of instead of a random point u on the line
    points[0] -= multiplier * v[0];
    points[1] -= multiplier * v[1];
     
    //normalize the direction vector to negative x, because the needle enters from the right side of the image
    if(v[0] > 0.0)
    {
      v[0] *= -1;
      v[1] *= -1;
    }
          
    float diag = sqrt((float)( size[0]*size[0] + size[1]*size[1] ));
    std::cout << "line: " << points[0] << "|" << points[1] << " / " << v[0] <<"|" << v[1] << " mult: " << multiplier << std::endl;
    std::cout << "diag: " << diag << std::endl;

    //  The line might be as long as the diagonal, thus that many pixels have to be computed
    //  and every iteration they get checked, if they are in the bounds of the mLocalInputImage
    for(int i=0; i<=static_cast<int>(diag); i++)
    {
      localIndex[0]=(long int)(points[0]+i*v[0]);
      localIndex[1]=(long int)(points[1]+i*v[1]);

      FloatImageType::RegionType outputRegion =  mLocalInputImage->GetLargestPossibleRegion();
      
      std::cout << i << ":" ;
      if(outputRegion.IsInside(localIndex)) // this pixel of the line is inside the boundings of mLocalInputImage
      {
        std::cout << localIndex[0] << "|" << localIndex[1];
        if(mLocalInputImage->GetPixel(localIndex) < lastIntensity + 10000)  // if pixel still belongs to needle
        {
          length++;
          lastIntensity = mLocalInputImage->GetPixel(localIndex);
          avgIntensity += mLocalInputImage->GetPixel(localIndex);
          std::cout << " pixel:" << mLocalInputImage->GetPixel(localIndex) << " total:" << avgIntensity;
        }         
        else // found the tip of the needle
        {          
          points[2] = localIndex[0];
          points[3] = localIndex[1];
          break;
        }
        std::cout << std::endl;
        mLocalInputImage->SetPixel(localIndex, 0);
      }
    }
    avgIntensity /= length;
    itLines++;
  }
  
  if(outputTmp)
  {
    if(mWhichTmp == 1)
    {
      mWhichTmp = 2;
      mLocalTmp2 = mLocalInputImage;
    }
    else
    {
      mWhichTmp = 1;
      mLocalTmp1 = mLocalInputImage;
    }
  }
  else
    mLocalOutputImage = mLocalInputImage;
    
  std::cout << "end of houghtransform" << std::endl;
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
  {
    mLocalOutputImage = cannyFilter->GetOutput();
    mWhichTmp = 0;
  }

  cannyFilter->SetVariance(5);
  cannyFilter->Update();  
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
  {
    mLocalOutputImage = binFilter->GetOutput();
    mWhichTmp = 0;
  }
  
  binFilter->SetOutsideValue(MAX);
  binFilter->SetInsideValue(0);
  binFilter->SetLowerThreshold(20000);
  binFilter->SetUpperThreshold(40000); 
  binFilter->Update();
}

void ImageProcessor::LaplacianRecursiveGaussian(bool inputTmp, bool outputTmp)
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
  {
    mLocalOutputImage = lapFilter->GetOutput();
    mWhichTmp = 0;
  }
  lapFilter->SetNormalizeAcrossScale( false );
  lapFilter->SetSigma(1);
  try
  {
    lapFilter->Update();
    }
      catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught updating the lap filter!" << std::endl;
    std::cout << err << std::endl;
  }  
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
  {
    mLocalOutputImage = sobelFilter->GetOutput();
    mWhichTmp = 0;
  }  
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
}

void ImageProcessor::SobelFilter(bool inputTmp, bool outputTmp, int direction) 
{
  FloatImageType::Pointer image; 
  if(inputTmp && (mWhichTmp == 1))
    image = mLocalTmp1;
  else if (inputTmp && (mWhichTmp == 2))
    image = mLocalTmp2;
  else
    image = mLocalInputImage;  
  int a,b; 
  if(direction == 1) // Operatordirection is x
  {
    a = 2;
    b = 1;
  }
  else if(direction == 2) // Operatordirection is y 
  {
    a = 1;
    b = 2;
  }
      
  // TODO: Comment on direction switchting, also make a const for X and Y axis
  //        Direction switching is not working yet, only x-axis
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
  FloatNeighborhoodIteratorType::OffsetType position3  = {{a-2, b }};
  FloatNeighborhoodIteratorType::OffsetType position4  = {{a-3, b }};
  FloatNeighborhoodIteratorType::OffsetType position5  = {{a-4, b }};
  FloatNeighborhoodIteratorType::OffsetType position6  = {{ a, b-1}};
  FloatNeighborhoodIteratorType::OffsetType position7  = {{a-1,b-1}};
  FloatNeighborhoodIteratorType::OffsetType position8  = {{a-3,b-1}};
  FloatNeighborhoodIteratorType::OffsetType position9  = {{a-4,b-1}};
  FloatNeighborhoodIteratorType::OffsetType position10 = {{ a ,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position11 = {{a-1,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position12 = {{a-2,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position13 = {{a-3,b-2}};
  FloatNeighborhoodIteratorType::OffsetType position14 = {{a-4,b-2}};
  
  for (it.GoToBegin(), out.GoToBegin(); !it.IsAtEnd(); ++it, ++out)
  {
    float sum = 0;
    sum = it.GetPixel(position10) - it.GetPixel(position1);
    sum += it.GetPixel(position11) - it.GetPixel(position2);
    sum += it.GetPixel(position12) - it.GetPixel(position3);
    sum += it.GetPixel(position13) - it.GetPixel(position4);
    sum += it.GetPixel(position14) - it.GetPixel(position5);
    sum += 2.0 * it.GetPixel(position6);
    sum += 2.0 * it.GetPixel(position7);
    sum += 2.0 * it.GetPixel(position8);
    sum += 2.0 * it.GetPixel(position9); 
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
  {
    mLocalOutputImage = output;
    mWhichTmp = 0;
  }  
}

// only writes grayscale integer 2D images
void ImageProcessor::Write(const char* filePath, int whichImage)
{
  itk::ImageFileWriter<UCharImageType>::Pointer
    writer = itk::ImageFileWriter<UCharImageType>::New();
  writer->SetFileName(filePath);
  switch(whichImage)
  {
    case  1: 
      writer->SetInput(RescaleFloatToUChar(mLocalInputImage));
      std::cout << "inputimage selected to write to file" << std::endl;
      break;
    case  2: 
      writer->SetInput(RescaleFloatToUChar(mLocalTmp1));
      break;
    case  3:
      writer->SetInput(RescaleFloatToUChar(mLocalTmp2));
      break;
    case  4:
      writer->SetInput(RescaleFloatToUChar(mLocalOutputImage));
      std::cout << "outputimage selected to write to file" << std::endl;
      break;
    default:
      std::cerr << "invalid selection to write to file" << std::endl;
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
}


// HoughTransform Algorithm
// Create the line
//        LineType::PointListType list; // insert two points per line
//
//        double radius = it_input.GetIndex()[0]; 
//        double teta   = ((it_input.GetIndex()[1])*2*nPI/this->GetAngleResolution())-nPI;
//        double Vx = radius*vcl_cos(teta );
//        double Vy = radius*vcl_sin(teta );
//        double norm = vcl_sqrt(Vx*Vx+Vy*Vy);
//        double VxNorm = Vx/norm;
//        double VyNorm = Vy/norm;
//
//        if((teta<=0) || (teta >= nPI / 2 ) )
//          {
//          if(teta >= nPI/2)
//            {
//            VyNorm = - VyNorm;
//            VxNorm = - VxNorm;
//            }
//
//          LinePointType p;
//          p.SetPosition(Vx,Vy);
//          list.push_back(p);
//          p.SetPosition(Vx-VyNorm*5,Vy+VxNorm*5);
//          list.push_back(p);
//          }
//        else // if teta>0
//          {
//          LinePointType p;
//          p.SetPosition(Vx,Vy);
//          list.push_back(p);
//          p.SetPosition(Vx-VyNorm*5,Vy+VxNorm*5);
//          list.push_back(p);
//          } // end if(teta>0)






