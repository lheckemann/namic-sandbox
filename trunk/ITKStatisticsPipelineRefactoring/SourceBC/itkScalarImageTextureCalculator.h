/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkScalarImageTextureCalculator.h,v $
  Language:  C++
  Date:      $Date: 2009-03-04 19:29:54 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkScalarImageTextureCalculator_h
#define __itkScalarImageTextureCalculator_h

#include "itkScalarImageToTextureFeaturesFilter.h"

namespace itk {
namespace Statistics {

/** \class ScalarImageTextureCalculator 
 *  \brief This class computes texture descriptions from an image.
 *
 * This class computes features that summarize the texture of a given image.
 * The texture features are compute a la Haralick, and have proven to be useful
 * in image classification for biological and medical imaging.
 * This class computes the texture features of an image (optionally in a masked
 * masked region), averaged across several spatial directions so that they are 
 * invariant to rotation. 
 *
 * By default, texure features are computed for each spatial
 * direction and then averaged afterward, so it is possible to access the standard
 * deviations of the texture features. These values give a clue as to texture 
 * anisotropy. However, doing this is much more work, because it involved computing
 * one GLCM for each offset given. To compute a single GLCM for all of the offsets,
 * call FastCalculationsOn(). If this is called, then the texture standard deviations
 * will not be computed (and will be set to zero), but texture computation will
 * be much faster.
 *
 * This class is templated over the input image type.
 *
 * Template Parameters:
 * The image type, and the type of histogram frequency container. If you are using
 * a large number of bins per axis, a sparse frequency container may be advisable.
 * The default is to use a dense frequency container.
 *
 * Inputs and parameters:
 * -# An image
 * -# A mask defining the region over which texture features will be
 *    calculated. (Optional)
 * -# The pixel value that defines the "inside" of the mask. (Optional, defaults
 *    to 1 if a mask is set.)
 * -# The set of features to be calculated. These features are defined
 *    in the GreyLevelCooccurrenceMatrixTextureCoefficientsCalculator class. (Optional,
 *    defaults to {Energy, Entropy, InverseDifferenceMoment, Inertia, ClusterShade, 
 *    ClusterProminence}, as in Conners, Trivedi and Harlow.)
 * -# The number of intensity bins. (Optional, defaults to 256.)
 * -# The set of directions (offsets) to average across. (Optional, defaults to
 *    {(-1, 0), (-1, -1), (0, -1), (1, -1)} for 2D images and scales analogously for ND 
 *    images.)
 * -# The pixel intensity range over which the features will be calculated.
 *    (Optional, defaults to the full dynamic range of the pixel type.)
 * 
 * In general, the default parameter values should be sufficient.
 *
 * Outputs:
 * (1) The average value of each feature.
 * (2) The standard deviation in the values of each feature.
 * 
 * Web references:
 * http://www.cssip.uq.edu.au/meastex/www/algs/algs/algs.html
 * http://www.ucalgary.ca/~mhallbey/texture/texture_tutorial.html
 *
 * Print references:
 * Haralick, R.M., K. Shanmugam and I. Dinstein. 1973.  Textural Features for 
 * Image Classification. IEEE Transactions on Systems, Man and Cybernetics. 
 * SMC-3(6):610-620.
 *
 * Haralick, R.M. 1979. Statistical and Structural Approaches to Texture. 
 * Proceedings of the IEEE, 67:786-804.
 *
 * R.W. Conners and C.A. Harlow. A Theoretical Comaprison of Texture Algorithms. 
 * IEEE Transactions on Pattern Analysis and Machine Intelligence,  2:204-222, 1980.
 *
 * R.W. Conners, M.M. Trivedi, and C.A. Harlow. Segmentation of a High-Resolution
 * Urban Scene using Texture  Operators. Computer Vision, Graphics and Image 
 * Processing, 25:273-310,  1984.
 * 
 * \sa ScalarImageToGreyLevelCooccurrenceMatrixGenerator
 * \sa MaskedScalarImageToGreyLevelCooccurrenceMatrixGenerator
 * \sa GreyLevelCooccurrenceMatrixTextureCoefficientsCalculator
 *
 * Author: Zachary Pincus
 */
    
template< class TImageType,
          class THistogramFrequencyContainer = DenseFrequencyContainer2 >
class ScalarImageTextureCalculator : public ScalarImageToTextureFeaturesFilter<TImageType, THistogramFrequencyContainer>
{
public:
  /** Standard typedefs */
  typedef ScalarImageTextureCalculator Self;
  typedef ScalarImageToTextureFeaturesFilter<TImageType, THistogramFrequencyContainer>
                                       Superclass;
  typedef SmartPointer<Self>           Pointer;
  typedef SmartPointer<const Self>     ConstPointer;
      
  /** Run-time type information (and related methods). */
  itkTypeMacro(ScalarImageTextureCalculator, ScalarImageToTextureFeaturesFilter);
      
  /** standard New() method support */
  itkNewMacro(Self);
  
  /** Triggers the computation of the features */
  void Compute( void )
    {
      Superclass::Update();
    }
      
  /** Connects the input image for which the features are going to be
   * computed */
//  void SetInput( const ImageType * );
      
protected:
  ScalarImageTextureCalculator(){}
  virtual ~ScalarImageTextureCalculator() {}
      
private:
};
    
} // end of namespace Statistics 
} // end of namespace itk 

#endif
