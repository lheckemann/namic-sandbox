#ifndef centerOfMassOfBinaryImage_txx_
#define centerOfMassOfBinaryImage_txx_


#include "centerOfMassOfBinaryImage.h"

#include "itkImageRegionConstIteratorWithIndex.h"

#include <csignal>


namespace douher
{
  /*--------------------------------------------------
    centerOfMass
  */
  template< typename T, unsigned int dim > 
  vnl_vector<double> computeCenterOfMass(typename itk::Image< T, dim >::Pointer image, double& totalMass, bool relative)
  {
    typedef itk::Image< T, dim > InputImageType;

    vnl_vector<double> centerOfMass(dim, 0.0);
    totalMass = 0.0;


    typedef itk::ImageRegionConstIteratorWithIndex< InputImageType > IteratorWithIdxType;
    IteratorWithIdxType imageIter( image, image->GetLargestPossibleRegion() );

    for (imageIter.GoToBegin(); !imageIter.IsAtEnd(); ++imageIter)
      {
        typename InputImageType::PixelType val = imageIter.Get();
        typename InputImageType::IndexType idx = imageIter.GetIndex();

        totalMass += val;
        for (long id = 0; id < static_cast<long>(dim); ++id)
          {
            centerOfMass[id] += val*idx[id];
          }
      }

    if (totalMass < vnl_math::eps)
      {
        std::cerr<<"Error: totalMass < vnl_math::eps \n";
        raise(SIGABRT);
      }


    //debug//
    //    std::cout<<"total mass: "<<totalMass<<std::endl;
    //DEBUG//

    centerOfMass /= totalMass;

    if (!relative)
      {
        return centerOfMass;
      }
    else
      {
        typename InputImageType::IndexType corner = image->GetLargestPossibleRegion().GetIndex();

        for (unsigned int i = 0; i < dim; ++i)
          {
            centerOfMass[i] -= corner[i];
          }

        return centerOfMass;
      }
  }



  /*--------------------------------------------------
    centerOfMass2
  */
  template< typename T > 
  vnl_vector<double> computeCenterOfMass2(typename itk::Image< T, 2 >::Pointer image, double& totalMass, bool relative)
  {
    return computeCenterOfMass<T, 2>(image, totalMass, relative);
  }


  /*--------------------------------------------------
    centerOfMass3
  */
  template< typename T > 
  vnl_vector<double> computeCenterOfMass3(typename itk::Image< T, 3 >::Pointer image, double& totalMass, bool relative)
  {
    return computeCenterOfMass<T, 3>(image, totalMass, relative);
  }

} //douher

#endif
