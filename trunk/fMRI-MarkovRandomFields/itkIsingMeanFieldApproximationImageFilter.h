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
  typedef typename TInputImage::Pointer InputImagePointer;

  /** Set the number of activation states expected in the functional MRI input */
  itkSetMacro( NumberOfActivationStates, unsigned int );

  /** Maximum number of iterations for the mean field update. This provides a
    * convergence criterion. */
  itkSetMacro( MaximumNumberOfIterations, unsigned int );
  
  void SetsegLabel(vtkIntArray *segLabelTcl) {this->segLabel = segLabelTcl;};
  void SetprobGivenSegM(vtkFloatArray *probGivenSegMTcl) {this->probGivenSegM = probGivenSegMTcl;}; 
  void SettransitionMatrix(vtkIntArray *transitionMatrixTcl) {this->transitionMatrix = transitionMatrixTcl;};
  void SetactivationFrequence(vtkFloatArray *activationFrequenceTcl) {this->activationFrequence = activationFrequenceTcl;};
  
private:

  unsigned int m_NumberOfActivationStates;  

  int nonactive;                        // value for label map representation
  int posactive;                        // value for label map representation
  int negactive;                        // value for label map representation
  int segInput;                         // number of anatomical labels
  float max;                            // maximum of posterior probability
  int posMax;                           // class of maximum of posterior probability
  int numActivationStates;              // number of activation states
  vtkFloatArray *activationFrequence;   // class frequence   
  vtkFloatArray *logTransitionMatrix;   // log transition matrix  
  vtkFloatArray *probGivenSegM;         // prob given SegM matrix [segInput][nType]
  vtkIntArray *transitionMatrix;    // transition matrix
  vtkIntArray *segLabel;                // list of segmentation labels
  short int *labelValue;                // contains a value of the anatomical label map
  int sum;                              // help variable for summation
  int index1;                    // index der transition matrix
  int index2;                           // index der transition matrix
  float logHelp;                        // help variable for log calculation
  vtkIntArray *segMArray;               // anatomical label map input
  int iterations;            // iterations of meanfield algorithm 
  float eValue;                         // e value in the meanfield algorithm
  float *helpArray;                     // array to buffer calculations of the meanfield iteration
  float sumHelpArray;                // sum of help array
  int numIterations;                    // number of meanfield algorithm iterations
  int dims[3];                          // array of dimensions
  unsigned long int size;               // size of the image input   
  float help;                           // help variable
protected:
  vtkIsingMeanfieldApproximation();
  ~vtkIsingMeanfieldApproximation();
  void SimpleExecute(vtkImageData *input, vtkImageData *output);
  void ExecuteInformation(vtkImageData *input, vtkImageData *output);
};

} // end namespace itk

#endif
