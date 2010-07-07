#ifndef shapeProjector_h_
#define shapeProjector_h_

/**
  Given:

  1. mean shape, represented as (average of) binary

  2. pca bases in all bands, (shape represented as (average of) binary)

  3. a new shape, in sfls

  Do:

  1. convert new shape to binary, to aid registration

  2. register new shape to mean shape

  3. proj to eigen shapes (normalize them first, not sure if they come as normalized)

  4. recostruct, get the filtered shape

  5. use inverse transform of the registration transform to map the the same size/pose of the newShape


  20100405 create
  20100704 last modified

  Yi Gao
*/



#include <vector>


// itk
#include "itkImage.h"
#include "itkSimilarity3DTransform.h"
#include "vnl/vnl_vector.h"


// boost
#include "boost/shared_ptr.hpp"


namespace newProstate
{
  template<typename float_t>
  class shapeProjector
  {
  public:
    typedef shapeProjector< float_t > Self;
    typedef boost::shared_ptr< Self > Pointer;


    typedef itk::Image< float_t, 3 > itkImage_t;


    shapeProjector();

    ~shapeProjector() {}

    /* New */
    static Pointer New() { return Pointer(new Self); }


    // fn
    void setNewShape( typename itkImage_t::Pointer newShape);

    void setMeanShape( typename itkImage_t::Pointer meanShape);
    void setEigenShapeList( typename std::vector< typename itkImage_t::Pointer > eigenShapeList);

    void gogogo();


    // data

    // mean shape
    typename itkImage_t::Pointer m_meanShape; // (averaged) binary shapes
    typename std::vector< typename itkImage_t::Pointer > m_eigenShapeList; // eigen shapes. 

    typename std::vector< double > m_eigenValues; // eigen values. 

    // new shape
    typename itkImage_t::Pointer m_newShape; // in sfls
    typename itkImage_t::Pointer m_newShapeBin; // in bin

    // reg new shape to mean shape, same size as MEAN shape
    typename itkImage_t::Pointer m_regNewShape; // in binary

    // reconstructed shape, same size as MEAN shape
    typename itkImage_t::Pointer m_regNewShapeProjected;

    // reconstructed shape, transform back, same size as NEW shape
    typename itkImage_t::Pointer m_regNewShapeProjectedBack; // in bin


  protected:

    /**
     * init this class
     */
    void init();


    /**
     * Store transformation that transform new shape to the mean shape
     */
    typedef itk::Similarity3DTransform< double > transform_t;
    transform_t::Pointer m_regTransform;

    transform_t::Pointer getInverseTransform(transform_t::Pointer t);

    /**/
    void getNewShapeBin();


    /**
     * Register the new shape m_newShape to the mean shape m_meanShape,
     * result in m_regNewShape
     */
    void regShapeToMeanShape();


    /**
     * Just a test function. Transform the means shape, using the
     * inv(m_regTransform), as a rough estimate. To see the pipeline is
     * working.
     */
    void transferMeanShapeToNewShape();


    /**
     * For each band in the bandStructure of the registered new shape,
     * subtract mean band and then project the residule to the bases
     * learned in the PCA step.
     */
    void projectToShapeSpace();


    /**
     * Transform the reconstructed/filtered shape, using the
     * inv(m_regTransform).
     */
    void transferFilteredShapeToNewShape();


    // /**
    //  * Binary to SFLS
    //  */
    // void bin2SFLS();


    /**
     * fillin value for registration. In this case is 0 because of binary
     */
    float_t m_regFillInValue;



  };
} // namespace

#include "shapeProjector.txx"



#endif
