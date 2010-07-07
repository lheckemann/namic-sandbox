#ifndef shapeProjector_txx_
#define shapeProjector_txx_

#include "shapeProjector.h"



//itk
#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkSimilarity3DTransform.h"

#include "itkCenteredTransformInitializer.h"

#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkResampleImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"


#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "itkSubtractImageFilter.h"

#include "itkImageDuplicator.h"

// local
#include "cArrayOp.h"
#include "thld3.hpp"


namespace newProstate
{
  template<typename float_t>
  void shapeProjector<float_t>::init()
  {
    m_regFillInValue = 0; // for binary

    m_regTransform = transform_t::New();

    //m_regNewShape = itkImage_t::New();

    return;
  }

  template<typename float_t>
  shapeProjector<float_t>::shapeProjector()
  {
    init();

    return;
  }

  
  template<typename float_t>
  void shapeProjector<float_t>::setMeanShape(typename itkImage_t::Pointer meanShape)
  {
    m_meanShape = meanShape;

    return;
  }


  template<typename float_t>
  void shapeProjector<float_t>::setEigenShapeList( typename std::vector< typename itkImage_t::Pointer > eigenShapeList)
  {
    if (!m_meanShape)
      {
        std::cerr<<"Error: set mean shape first.\n";
        raise(SIGABRT);
      }

    m_eigenShapeList = eigenShapeList;

    // get the eigen values and normalize eigen shapes
    long n = m_eigenShapeList.size();

    m_eigenValues.resize(n);


    typedef itk::ImageRegionConstIterator<itkImage_t> itkImageRegionConstIterator_t;
    typedef itk::ImageRegionIterator<itkImage_t> itkImageRegionIterator_t;

#pragma omp parallel for
    for (long i = 0; i < n; ++i)
      {
        typename itkImage_t::Pointer thisEigenShape = m_eigenShapeList[i];

        if (thisEigenShape->GetLargestPossibleRegion().GetSize() != m_meanShape->GetLargestPossibleRegion().GetSize())
          {
            std::cerr<<"Error: size of mean shape =  "<<m_meanShape->GetLargestPossibleRegion().GetSize()<<std::endl;
            std::cerr<<"Error: size of "<<i<<"-th eigen shape = "\
                     <<thisEigenShape->GetLargestPossibleRegion().GetSize()<<std::endl;

            raise(SIGABRT);
          }

        /**
           According to imagePCAAnalysisTest2.cxx, the eigen shape in
           NOT normalized, instead, the sqrt(iEigen-th eigen value)
           has been multiplied. So, here compute the norm of each
           image, that will be sqrt(eigenValue)
        */
        itkImageRegionConstIterator_t iter(thisEigenShape, thisEigenShape->GetLargestPossibleRegion());
        double s = 0;
        for (iter.GoToBegin(); !iter.IsAtEnd(); ++iter)
          {
            typename itkImage_t::PixelType v = iter.Get();
            
            s += v*v;
          }

        double thisEigenValue = sqrt(s);
        if (thisEigenValue < 1e-6 && thisEigenValue > -1e-6)
          {
            std::cout<<"Warning: thisEigenValue = "<<thisEigenValue<<", too close to 0.\n";
          }

        m_eigenValues[i] = thisEigenValue;
        // std::cout<<sqrt(s)<<std::endl;


        // normalize
        itkImageRegionIterator_t iter1(thisEigenShape, thisEigenShape->GetLargestPossibleRegion());
        for (iter1.GoToBegin(); !iter1.IsAtEnd(); ++iter1)
          {
            iter1.Set(iter1.Get()/thisEigenValue);
          }
      }

    return;
  }

  template<typename float_t>
  void shapeProjector<float_t>::getNewShapeBin()
  {
    // get binary of new shape (sfls), for reg
    m_newShapeBin = thld3<itkImage_t, itkImage_t>(m_newShape, -1e10, 0, 1, 0);

    //m_newShapeBin = m_newShape;

    return;
  }


  template<typename float_t>
  void shapeProjector<float_t>::setNewShape( typename itkImage_t::Pointer newShape)
  {
    m_newShape = newShape;

    getNewShapeBin();

    return;
  }


  template<typename float_t>
  void shapeProjector<float_t>::regShapeToMeanShape()
  {
    /**
     * The new shape and the mean shape are both binary, regiser them
     * use MSE and similarity trans. Recored the trans
     */

    typedef transform_t::ParametersType ParametersType;

    typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

    typedef itk::MeanSquaresImageToImageMetric< itkImage_t, itkImage_t >    MetricType;

    typedef itk::LinearInterpolateImageFunction< itkImage_t, double >    InterpolatorType;

    typedef itk::ImageRegistrationMethod< itkImage_t, itkImage_t >    RegistrationType;

    typename MetricType::Pointer         metric        = MetricType::New();
    typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
    typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    typename RegistrationType::Pointer   registration  = RegistrationType::New();
  

    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetInterpolator(  interpolator  );


    transform_t::Pointer  transform = transform_t::New();
    registration->SetTransform( transform );

    //   //tst/
    //   std::cout<<"mean and new================================================================================\n";
    //   std::cout<<m_meanShape->GetLargestPossibleRegion()<<std::endl;
    //   std::cout<<m_meanShape->GetSpacing()<<std::endl;
    //   std::cout<<"================================================================================\n";
    //   std::cout<<m_newShape->GetLargestPossibleRegion()<<std::endl;
    //   std::cout<<m_newShape->GetSpacing()<<std::endl;
    //   std::cout<<"================================================================================\n";
    //   //tst//



    registration->SetFixedImage(    m_meanShape    );
    registration->SetMovingImage(   m_newShapeBin   );
    registration->SetFixedImageRegion( m_meanShape->GetBufferedRegion() );


    typedef itk::CenteredTransformInitializer< transform_t, itkImage_t, itkImage_t >  TransformInitializerType;

    typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();

    initializer->SetTransform(   transform );
    initializer->SetFixedImage(  m_meanShape );
    initializer->SetMovingImage( m_newShapeBin );
    initializer->MomentsOn(); // align moments, good for binary images
    initializer->InitializeTransform();

    registration->SetInitialTransformParameters( transform->GetParameters() );


    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

    assert(transform->GetNumberOfParameters() == 7);


    const double translationScale = 1.0 / 1000.0;

    optimizerScales[0] = 1.0; // versor.x
    optimizerScales[1] = 1.0; // versor.y
    optimizerScales[2] = 1.0; // versor.z
    optimizerScales[3] = translationScale; // translation.x
    optimizerScales[4] = translationScale; // translation.y
    optimizerScales[5] = translationScale; // translation.z
    optimizerScales[6] = 100.0; // scale

    optimizer->SetScales( optimizerScales );

    optimizer->SetMaximumStepLength( 0.0200  ); 
    optimizer->SetMinimumStepLength( 0.0001 );

    optimizer->SetNumberOfIterations( 1000 );



    try 
      { 
        registration->StartRegistration(); 
        /*
          std::cout << "Optimizer stop condition: "                       \
          << registration->GetOptimizer()->GetStopConditionDescription()  \
          << std::endl;
        */
      } 
    catch( itk::ExceptionObject & err ) 
      { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        exit(-1);
      } 
  
    OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();


    m_regTransform->SetCenter( transform->GetCenter() );
    m_regTransform->SetParameters( finalParameters );
    m_regTransform->SetFixedParameters( transform->GetFixedParameters() );

    typedef itk::ResampleImageFilter< itkImage_t, itkImage_t >    ResampleFilterType;

    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();


    //   std::cout<<"final trans = \n"<<m_regTransform<<std::endl;
    //   std::cout<<"================================================================================\n";

    resampler->SetTransform( m_regTransform );
    resampler->SetInput( m_newShapeBin );

    resampler->SetSize(    m_meanShape->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin(  m_meanShape->GetOrigin() );
    resampler->SetOutputSpacing( m_meanShape->GetSpacing() );
    resampler->SetOutputDirection( m_meanShape->GetDirection() );
    resampler->SetDefaultPixelValue( m_regFillInValue );
    resampler->Update();

    m_regNewShape = resampler->GetOutput();


    //   std::cout<<resampler->GetOutput()->GetLargestPossibleRegion()<<std::endl;
    //   std::cout<<"================================================================================\n";
    //   std::cout<<m_regNewShape->GetLargestPossibleRegion()<<std::endl;
    //   std::cout<<"================================================================================\n";



    // //tst
    // writeImage3<float>(m_regNewShape, "m_regNewShape.nrrd");

    // exit(0);
    // //tst//


    return;
  }


  template<typename float_t>
  void shapeProjector<float_t>::gogogo()
  {
    /*
      1. reg new shape to the mean shape, call the reg-ed new shape as m_regNewShape

      .... m_regNewShape should have same size/spacing etc as meanShape

      .... record the transformation T


      2. remove mean shape from the registered new shape, called a

      3. inner prod of a with each of eigen shape, then divide eigen values, to get coefs

      4. proj coefs into [-3 3], get B

      5. reconstruct to form B

      6. Use inv(T) to get back
    */

    // 1.
    std::cout<<"Register new shape to mean shape..."<<std::flush;
    regShapeToMeanShape();

    // 4, 5
    std::cout<<"Proj..."<<std::flush;
    projectToShapeSpace();
  
    // 6
    transferFilteredShapeToNewShape();


    //   // side .
    //   transferMeanShapeToNewShape();


    return;
  }


  template<typename float_t>
  void shapeProjector<float_t>::projectToShapeSpace()
  {
    /**
     * 1. subtract the mean shape
     * 2. for each eigen shape, do:
     *    inner prod with this shape, devide by eigenValue to get coef, reconstruct
     * 3. add mean back
     */
    

    // 1.
    typedef itk::SubtractImageFilter<itkImage_t, itkImage_t, itkImage_t> itkSubtractImageFilter_t;
    typename itkSubtractImageFilter_t::Pointer subtractor = itkSubtractImageFilter_t::New();

    subtractor->SetInput1(m_regNewShape);
    subtractor->SetInput2(m_meanShape);
    subtractor->Update();
    typename itkImage_t::Pointer withoutMean = subtractor->GetOutput();

    // //tst
    // writeImage3<float>(withoutMean, "withoutMean.nrrd");
    // //tst//



    // initialize recontrcted/filtered shape by duplicating mean shape
    typedef itk::ImageDuplicator< itkImage_t > itkImageDuplicator_t;
    typename itkImageDuplicator_t::Pointer duplicator = itkImageDuplicator_t::New();
    duplicator->SetInputImage(m_meanShape); 
    duplicator->Update(); 
    m_regNewShapeProjected = duplicator->GetOutput();
    

    
    // 2. 3.
    long n = m_eigenShapeList.size();

    typedef itk::ImageRegionConstIterator<itkImage_t> itkImageRegionConstIterator_t;

    for (long i = 0; i < n; ++i)
      {
        typename itkImage_t::Pointer thisEigenBase = m_eigenShapeList[i];

        // //tst
        // writeImage3<float>(thisEigenBase, "thisEigenBase.nrrd");
        // //tst//


        /** 
            project the new shape onto the i-th eigen shape
        */
        double innerProd = 0.0;

        itkImageRegionConstIterator_t iter1(thisEigenBase, thisEigenBase->GetLargestPossibleRegion());
        itkImageRegionConstIterator_t iter2(withoutMean, withoutMean->GetLargestPossibleRegion());

        for (iter1.GoToBegin(); !iter1.IsAtEnd(); ++iter1, ++iter2)
          {
            typename itkImage_t::PixelType v1 = iter1.Get();
            typename itkImage_t::PixelType v2 = iter2.Get();
            
            innerProd += v1*v2;
          }

        double coef = innerProd/m_eigenValues[i];
        std::cout<<"coef for "<<i<<"-th eigen shape is "<<coef<<std::endl;

        /**
           proj coef to [-3, 3], then multiply by the eigen value
         */
        if (coef >= 3.0)
          {
            coef = 3.0;
          }
        else if (coef <= -3.0)
          {
            coef = -3.0;
          }
        coef *= m_eigenValues[i];

        /** 
            reconstruct by coef with the i-th eigen shape
        */
        typedef itk::ImageRegionIterator<itkImage_t> itkImageRegionIterator_t;
        itkImageRegionIterator_t iter3(m_regNewShapeProjected, m_regNewShapeProjected->GetLargestPossibleRegion());
        iter1.GoToBegin();
        iter3.GoToBegin();

        for (; !iter1.IsAtEnd(); ++iter1, ++iter3)
          {
            iter3.Set( coef*iter1.Get() + iter3.Get() );
          }
      }


    // //tst
    // writeImage3<float>(m_regNewShapeProjected, "m_regNewShapeProjected.nrrd");
    // //tst//

    return;
  }

// //   template<typename float_t>
// //   itk::Similarity3DTransform< double >::Pointer
// //   shapeProjector<float_t>::getInverseTransform(transform_t::Pointer t)
// //   {
// //     /**
// //      * From:
// //      * http://www.cmake.org/pipermail/insight-users/2006-September/019484.html
// //      * 
// //      * The GetInverse method is not implemented in the
// //      * Similarity3DTransform.
// //      *
// //      * There has been some discussion on how to implement the inverse
// //      * operation in a consistent way across all the transform.
// //      * 
// //      * 
// //      * The GetInverse() method that is being called is the one of the
// //      * superclasses of the Similarity transform. That's not the method
// //      * that should be used here.
// //      *
// //      *
// //      * What you may want to do is to extract the three components of the
// //      * Similarity transform:
// //      *
// //      * 1) Scale (a Scalar) 
// //      *
// //      * 2) Rotation (a Versor = Unit Quaternion) 
// //      *
// //      * 3) Translation (a Vector)
// //      *
// //      * Invert them and then invert their sequence of application.
// //      *
// //      * You get the values with:
// //      *
// //      * scale  = similarityTransform->GetScale();
// //      * versor = similarityTransform->GetVersor();
// //      * vector = similarityTransform->GetTranslation();

// //      * The inversion is computed as

// //      * iscale = 1.0 / scale
// //      * iversor = versor.GetConjugate();
// //      * ivector = -vector;

// //      * with iscale and iversor you can compute the matrix as

// //      * matrix = iversor.GetMatrix() * iscale

// //      * Then in order to revert the order of translation you do

// //      * rivector = matrix * ivector;


// //      * If you take now these componets:


// //      * { iscale, iversor, rivector }


// //      * and "Set" them in a Similarity transform, you will have
// //      * the inversse of the original similarity transform.


// //      * Please let us know if you find any problem implementing this
// //      * method.


// //      * Thanks

// //      * Luis
// //      */

// //     transform_t::Pointer invT = transform_t::New();

// //     transform_t::ScaleType scale  = t->GetScale();
// //     transform_t::VersorType versor = t->GetVersor();
// //     transform_t::OutputVectorType vector = t->GetTranslation();


// //     // compute inverse separately
// //     transform_t::ScaleType iscale = 1.0 / scale;
// //     transform_t::VersorType iversor = versor.GetConjugate();
// //     transform_t::OutputVectorType ivector = -vector;

// //     transform_t::MatrixType matrix = iversor.GetMatrix() * iscale;

// //     transform_t::OutputVectorType rivector = matrix * ivector;
      
  
// //     invT->SetCenter( t->GetCenter() );
// //     invT->SetFixedParameters( t->GetFixedParameters() );


// //     transform_t::ParametersType newP = t->GetParameters();
// //     newP[0] = iversor.GetX();
// //     newP[1] = iversor.GetY();
// //     newP[2] = iversor.GetZ();
// //     newP[3] = rivector[0];
// //     newP[4] = rivector[1];
// //     newP[5] = rivector[2];

// //     newP[6] = iscale;

// //     invT->SetParameters(newP);

// //     return invT;  
// //   }


  template<typename float_t>
  void shapeProjector<float_t>::transferFilteredShapeToNewShape()
  {
    /**
     *AFTER* the new shape is registered to the mean shape, use the
     *inverse transformation to tranform the mean shape back to the new
     *shape space.

     precondition: m_regTransform is ready

    */

    transform_t::Pointer finalTransform_inverse = transform_t::New();
    finalTransform_inverse->SetCenter( m_regTransform->GetCenter() );
    m_regTransform->GetInverse(finalTransform_inverse);
    finalTransform_inverse->SetFixedParameters( m_regTransform->GetFixedParameters() );


    //transform_t::Pointer finalTransform_inverse = getInverseTransform(m_regTransform);

    typedef itk::ResampleImageFilter< itkImage_t, itkImage_t > ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    resampler->SetTransform( finalTransform_inverse );
    resampler->SetInput( m_regNewShapeProjected );

    resampler->SetSize(    m_newShape->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin(  m_newShape->GetOrigin() );
    resampler->SetOutputSpacing( m_newShape->GetSpacing() );
    resampler->SetOutputDirection( m_newShape->GetDirection() );
    resampler->SetDefaultPixelValue( m_regFillInValue );

    resampler->Update();


    m_regNewShapeProjectedBack = resampler->GetOutput();

    // //tst
    // writeImage3<float>(m_regNewShapeProjectedBack, "m_regNewShapeProjectedBack.nrrd");
    // //tst//
  }


//   template<typename float_t>
//   void shapeProjector<float_t>::transferMeanShapeToNewShape()
//   {
//     /**
//      *AFTER* the new shape is registered to the mean shape, use the
//      *inverse transformation to tranform the mean shape back to the new
//      *shape space.

//      precondition: m_regTransform is ready

//     */

//     transform_t::Pointer finalTransform_inverse = transform_t::New();
//     finalTransform_inverse->SetCenter( m_regTransform->GetCenter() );
//     m_regTransform->GetInverse(finalTransform_inverse);
//     finalTransform_inverse->SetFixedParameters( m_regTransform->GetFixedParameters() );


//     douher::writeImage3<double>(m_meanShape, "m_meanShape.nrrd");
//     douher::writeImage3<double>(m_regNewShape, "m_regNewShape.nrrd");

//     //tst
//     std::cout<<"mean shape ori: "<<m_meanShape->GetOrigin()<<std::endl;
//     std::cout<<"mean shape dir: \n"<<m_meanShape->GetDirection()<<std::endl;
//     std::cout<<"mean shape spc: "<<m_meanShape->GetSpacing()<<std::endl;

//     std::cout<<"----------------------------------------------------------------------\n";

//     std::cout<<"reg new shape ori: "<<m_regNewShape->GetOrigin()<<std::endl;
//     std::cout<<"reg new shape dir: \n"<<m_regNewShape->GetDirection()<<std::endl;
//     std::cout<<"reg new shape spc: "<<m_regNewShape->GetSpacing()<<std::endl;
//     //tst//


//     typedef itk::ResampleImageFilter< itkImage_t, itkImage_t > ResampleFilterType;
//     typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

//     resampler->SetTransform( finalTransform_inverse );
//     resampler->SetInput( m_meanShape );


//     resampler->SetSize(    m_newShape->GetLargestPossibleRegion().GetSize() );
//     resampler->SetOutputOrigin(  m_newShape->GetOrigin() );
//     resampler->SetOutputSpacing( m_newShape->GetSpacing() );
//     resampler->SetOutputDirection( m_newShape->GetDirection() );
//     resampler->SetDefaultPixelValue( m_regFillInValue );

//     resampler->Update();


//     //tst
//     douher::writeImage3<double>(resampler->GetOutput(), "h_meanNew.nrrd");
//     //tst//






//     ////////////////////////////////////////////////////////////////////////////////
//     typename ResampleFilterType::Pointer resampler1 = ResampleFilterType::New();

//     resampler1->SetTransform( finalTransform_inverse );
//     resampler1->SetInput( m_regNewShape );

//     resampler1->SetSize(    m_newShape->GetLargestPossibleRegion().GetSize() );
//     resampler1->SetOutputOrigin(  m_newShape->GetOrigin() );
//     resampler1->SetOutputSpacing( m_newShape->GetSpacing() );
//     resampler1->SetOutputDirection( m_newShape->GetDirection() );
//     resampler1->SetDefaultPixelValue( m_regFillInValue );

//     resampler1->Update();


//     //tst
//     douher::writeImage3<double>(resampler1->GetOutput(), "h_m_regNewShape.nrrd");
//     //tst//
//   }

}
#endif
