#include "pointSet.h"

#include "itkImage.h"
#include "itkImageRegionIterator.h"

#include "imageToPointSet.h"

#include "vnl/vnl_random.h"

#include <csignal>

// douher
#include "arrays/cArrayOp.h"
#include "imageProcessing/imageProcessing.h"


/*============================================================*/
void imageToPointSet2D( DoubleImage2DPointerType image, unsigned long numOfPt, \
                        PointSet2DType& ptSet)
{
  ptSet.clear();

  // 1st check image > 0, and pick max one 
  typedef itk::ImageRegionIterator< DoubleImage2DType > DoubleImageIteratorType;

  DoubleImageIteratorType it( image, image->GetLargestPossibleRegion() );
  it.GoToBegin();
  
  double maxVal = it.Get();
  double minVal = it.Get();
  double norm1 = 0;

  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      double v = it.Get();

      maxVal = maxVal>v?maxVal:v;
      minVal = minVal<v?minVal:v;

      norm1 += v;
    }

  //  std::cout<<"max and min = "<<maxVal<<"\t"<<minVal<<std::endl;

  if (minVal < 0)
    {
      std::cerr<<"Image has negative values. abort. \n";
      raise(SIGABRT);
    }


  // normalize
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      double v = it.Get();

      it.Set(v/norm1);
    }

  maxVal /= norm1;

  unsigned long nx = image->GetLargestPossibleRegion().GetSize(0);
  unsigned long ny = image->GetLargestPossibleRegion().GetSize(1);

  //  std::cout<<"nx, ny = "<<nx<<"\t"<<ny<<std::endl;

  //  std::cout<<"norm1 = "<<norm1<<std::endl;
  
  double cx = nx/2.0;
  double cy = ny/2.0;

  vnl_random rg;

  VnlDoubleVector2DType onePt;

  unsigned char interpType = 0;  // nn interp

  double ptX = 0;
  double ptY = 0;
  double u = 0;

  double f = 0;

  for (unsigned long is = 0; is < numOfPt; )
    {


      ptX = rg.drand32(nx-1);
      ptY = rg.drand32(ny-1);
      u = rg.drand32();

      //      std::cout<<"u: "<<u<<std::endl;

      f = douher::offGridImageValue< double >(image, ptX, ptY, interpType);

      if (u < (f/maxVal))
        {
          ++is;
          onePt[0] = ptX - cx;
          onePt[1] = ptY - cy;

          //          std::cout<<"idx picked is "<<idx<<std::endl;

          ptSet.push_back(onePt);
          //          std::cout<<is<<std::endl;
        }
    }
}



/*============================================================*/
void imageToPointSet3D( DoubleImage3DPointerType image, unsigned long numOfPt, \
                        PointSet3DType& ptSet)
{
  ptSet.clear();

  // 1st check image > 0, and pick max one 
  typedef itk::ImageRegionIterator< DoubleImage3DType > DoubleImageIteratorType;
//   DoubleImage3DType::SpacingType spacing = image->GetSpacing();
//   std::cout<<"spacing is "<<spacing<<std::endl;

  DoubleImageIteratorType it( image, image->GetLargestPossibleRegion() );
  it.GoToBegin();
  
  double maxVal = it.Get();
  double minVal = it.Get();
  double norm1 = 0;

  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      double v = it.Get();

      maxVal = maxVal>v?maxVal:v;
      minVal = minVal<v?minVal:v;

      norm1 += v;
    }

  std::cout<<"max and min = "<<maxVal<<"\t"<<minVal<<std::endl;

  if (minVal < 0)
    {
      std::cerr<<"Image has negative values. abort. \n";
      raise(SIGABRT);
    }


  // normalize
  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
      double v = it.Get();

      it.Set(v/norm1);
    }

  maxVal /= norm1;

  unsigned long nx = image->GetLargestPossibleRegion().GetSize(0);
  unsigned long ny = image->GetLargestPossibleRegion().GetSize(1);
  unsigned long nz = image->GetLargestPossibleRegion().GetSize(2);

  //  std::cout<<"nx, ny, nz = "<<nx<<"\t"<<ny<<"\t"<<nz<<std::endl;

  //  std::cout<<"norm1 = "<<norm1<<std::endl;
  
  double cx = nx/2.0;
  double cy = ny/2.0;
  double cz = nz/2.0;

  vnl_random rg;

  VnlDoubleVector3DType onePt;

  unsigned char interpType = 0;  // nn interp

  double ptX = 0;
  double ptY = 0;
  double ptZ = 0;

  double u = 0;

  double f = 0;

  for (unsigned long is = 0; is < numOfPt; )
    {
      ptX = rg.drand32(nx-1);
      ptY = rg.drand32(ny-1);
      ptZ = rg.drand32(nz-1);

      u = rg.drand32();

      //      std::cout<<"u: "<<u<<std::endl;

      f = douher::offGridImageValue< double >(image, ptX, ptY, ptZ, interpType);

      if (u < (f/maxVal))
        {
          ++is;
//           onePt[0] = spacing[0]*(ptX - cx);
//           onePt[1] = spacing[1]*(ptY - cy);
//           onePt[2] = spacing[2]*(ptZ - cz);
          onePt[0] = ptX - cx;
          onePt[1] = ptY - cy;
          onePt[2] = ptZ - cz;

          //          std::cout<<"idx picked is "<<idx<<std::endl;

          ptSet.push_back(onePt);
          //          std::cout<<is<<std::endl;
        }
    }
}
