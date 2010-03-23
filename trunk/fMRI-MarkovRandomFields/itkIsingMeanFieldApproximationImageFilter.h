/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkModule.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.18 $

=========================================================================auto=*/
#ifndef __itkIsingMeanfieldApproximation_h
#define __itkIsingMeanfieldApproximation_h

#include "vtkFMRIEngineConfigure.h"

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "vnl/vnl_math.h"

namespace itk
{

//
// TODO: Take spacing into account in the computation.
//

/** \class IsingMeanfieldApproximationImageFilter 
 *  \brief Computes for each voxel the posterior class probability.
 * 
 *  This class takes as inputs a fMRI statistics image and an anatomical
 *  labelled image.  and use them to compute posterior probabilities of
 *  activation states per voxel.
 * 
 *  \ingroup MRFFilters
 *
 */
class ITK_EXPORT IsingMeanfieldApproximationImageFilter : public 
                                       ImageToImageFilter<
                                            Image<float,3>
                                            VectorImage<float,3> >

{

public:

  /** Standard Self typedef */
  typedef IsingMeanfieldApproximationImageFilter Self;

  typedef ImageToImageFilter<
              Image<float,3>,
              VectorImage<float,3> >  Superclass;

  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(IsingMeanfieldApproximationImageFilter, ImageToImageFilter);
  
  /** Image related typedefs. */
  typedef Image< float,3 >            fMRIStatisticsImageType;
  typedef VectorImage< float, 3 >     PosteriorProbabilitiesImageType;
  typedef Image< unsigned short, 3 >  SegmentationLabelsImageType;


  /** Set the number of activation states expected in the functional MRI input */
  itkSetMacro( NumberOfActivationStates, unsigned int );

  /** Maximum number of iterations for the mean field update. This provides a
    * convergence criterion. */
  itkSetMacro( MaximumNumberOfIterations, unsigned int );
  
  /** Set the anatomical label map */
  void SetSegmentationLabelMap( const SetSegmentationLabelMap * labelMap );

  /** Set the anatomical label map */
  void SetfMRIStatisticsImage( const fMRIStatisticsImageType * labelMap );

  /** Type for representing the prior class probabilities given segmentation labels. */
  typedef vnl_matrix< float >  PriorClassProbabilityGivenSegmentationMatrixType;

  /** Set the prior class probabilities given segmentation labels. This matrix
   * should be provided by the user and it should have a number of rows equal to
   * the number of segmentation labels and a number of columns equals to the
   * product of number of activation states and the number of segmentation
   * labels. */
  itkSetMacro( PriorClassProbabilityGivenSegmentation, 
               PriorClassProbabilityGivenSegmentationMatrixType ); 

  /** Type for representing the pair-wise pixel potential matrix */
  typedef vnl_matrix< float >  PairwisePotentialMatrixType;

  /** Set the pair-wise pixel potential matrix. This matrix represents how
   * likely is to find two adjancent pixels with particular values of posterior
   * probabilities. This matrix is optional. If provided by the user it is used
   * directly in the computation of the Ising model, otherwise this filter
   * estimates the matrix values based on the input segmentation labeling and the
   * activation statistics. The symmetric matrix must have a number of rows equal
   * to the product of number of activation states and the number of segmentation
   * labels, which is equal to the number of posterior probabilities. */
  void SetPairwisePotentialMatrix( const PairwisePotentialMatrixType & matrix ); 

  /** Type for representing the activation frequency. */
  typedef vnl_vector< float >  ActivationFrequencyArrayType;

  /** Set the array containing the activation frequency. These frequencies
 * represents how often each one of the combinations of activation states and
 * segmentation label appears. The length of the array must be equal to the
 * product of the number of activation states and the number of segmentation
 * lables. This input MUST be provided by the user before running the filter.*/
  itkSetMacro( ActivationFrequencyArray, ActivationFrequencyArrayType ); 


private:

  unsigned int m_NumberOfActivationStates;  

  SegmentationLabelsImageType::Pointer       m_SegmentationLabelMap;
  fMRIStatisticsImageType::Pointer           m_fMRIStatisticsImage;
  PosteriorProbabilitiesImageType::Pointer   m_PosteriorProbabilitesImage;
  
  PriorClassProbabilityGivenSegmentationMatrixType m_PriorClassProbabilityGivenSegmentation;

  PairwisePotentialMatrixType                m_PairwisePotentialMatrix;
  bool                                       m_PairwisePotentialMatrixSetByUser;

  ActivationFrequencyArrayType               m_ActivationFrequencyArray;

  unsigned int                               m_MaximumNumberOfIterations;

  int nonactive;                        // value for label map representation
  int posactive;                        // value for label map representation
  int negactive;                        // value for label map representation
  int segInput;                         // number of anatomical labels
  float max;                            // maximum of posterior probability
  int posMax;                           // class of maximum of posterior probability
  int numActivationStates;              // number of activation states
  vtkFloatArray *logTransitionMatrix;   // log transition matrix  
  vtkIntArray *transitionMatrix;    // transition matrix
  short int *labelValue;                // contains a value of the anatomical label map
  int sum;                              // help variable for summation
  int index1;                    // index der transition matrix
  int index2;                           // index der transition matrix
  float logHelp;                        // help variable for log calculation
  vtkIntArray *segMArray;               // anatomical label map input
  float eValue;                         // e value in the meanfield algorithm
  float *helpArray;                     // array to buffer calculations of the meanfield iteration
  float sumHelpArray;                // sum of help array
  int numIterations;                    // number of meanfield algorithm iterations
  int dims[3];                          // array of dimensions
  unsigned long int size;               // size of the image input   
  float help;                           // help variable

protected:

  IsingMeanfieldApproximationImageFilter();
  virtual ~IsingMeanfieldApproximationImageFilter();

  void SimpleExecute(vtkImageData *input, vtkImageData *output);
  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
};

} // end namespace itk

#endif