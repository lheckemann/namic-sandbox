/*=auto=========================================================================

(c) Copyright 2005 Massachusetts Institute of Technology (MIT) All Rights Reserved.

This software ("3D Slicer") is provided by The Brigham and Women's 
Hospital, Inc. on behalf of the copyright holders and contributors.
Permission is hereby granted, without payment, to copy, modify, display 
and distribute this software and its documentation, if any, for  
research purposes only, provided that (1) the above copyright notice and 
the following four paragraphs appear on all copies of this software, and 
(2) that source code to any modifications to this software be made 
publicly available under terms no more restrictive than those in this 
License Agreement. Use of this software constitutes acceptance of these 
terms and conditions.

3D Slicer Software has not been reviewed or approved by the Food and 
Drug Administration, and is for non-clinical, IRB-approved Research Use 
Only.  In no event shall data or images generated through the use of 3D 
Slicer Software be used in the provision of patient care.

IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO 
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY EXPRESS 
OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO 
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


=========================================================================auto=*/
#ifndef __itkNormalizedCuts_h
#define __itkNormalizedCuts_h


// ITK objects
#include "itkListSample.h"
#include "itkVector.h"
#include "itkSampleClassifier.h"
#include "itkProcessObjects.h"
#include "itkSymmetricEigenAnalysis.h"

namespace itk {

/** \class NormalizedCuts
 *
 * \brief Normalized Cuts Clustering Algorithm.
 *
 * Implementation of the Normalized Cuts algorithm for spectral clustering,
 * as described in:
 * Fowlkes et al., Spectral Grouping Using the Nystrom Method
 * IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, 
 * VOL. 26, NO. 2, FEBRUARY 2004
 * The output is an itk classifier object, with labels for each of 
 * the N input items.
 * Depending on the type of embedding normalization chosen, performs either
 * Normalized Cuts or general spectral clustering.
 * The number of clusters must be specified by the user.
 *
 */


class  NormalizedCuts : public ProcessObject
{
 public:
  /** Standard class typedefs */
  typedef NormalizedCuts     Self;
  typedef ProcessObject      Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkTypeMacro(NormalizedCuts, ProcessObject);
  itkNewMacro(Self);

  typedef itk::Image<float,3>   ImageType;


  // Description
  // Compute the output.  Call this after setting the InputWeightMatrix.
  // This code does 6 main steps.
  // 1. Normalize the weight matrix.
  // 2. Compute the eigensystem of the normalized weight matrix.
  // 3. Compute embedding vectors from rows of the eigenvector matrix.
  // 4. Cluster embedding vectors using k-means.
  // 5. Sort output cluster centroids according to second eigenvector ordering.
  // 6. Calculate output cluster memberships.
  // Note there is one embedding vector per row of the weight matrix (per
  // data point), so embedding vector j corresponds to input row j.
  // 
  void ComputeClusters();

  // TEST we want this specified at run time!
  const static int InternalNumberOfEigenvectors = 2;

  /** Use the Matrix for the input similarity matrix */
  typedef itk::Matrix<double,
                InternalNumberOfEigenvectors,
                InternalNumberOfEigenvectors> InputMatrixType;

  typedef itk::Vector<double,
                InternalNumberOfEigenvectors> EigenVectorType;

  typedef itk::Vector< double, InternalNumberOfEigenvectors > EmbedVectorType;
  typedef itk::Statistics::ListSample< EmbedVectorType > EmbedSampleType;
  typedef itk::Statistics::SampleClassifier< EmbedSampleType > OutputClassifierType;


  /**
   * Set input to this class.  This matrix should be NxN, where N
   * is the number of items we want to cluster.  Entries should be 
   * similarity values (not distances).
   */
  itkSetConstMacro( InputWeightMatrix, InputMatrixType );
  itkGetConstMacro( InputWeightMatrix, InputMatrixType );
  

  /**
   * Description
   * Get the classifier which is output from this class
   */
  OutputClassifierType::Pointer GetOutputClassifier() 
    {
      return this->OutputClassifier;
    }

  /** Number of clusters to output */
  itkSetMacro(NumberOfClusters,int);
  itkGetMacro(NumberOfClusters,int);

  /** Number of eigenvectors to use in the embedding */
  // TEST not currently implemented due to fixed-length itk vector!
  // itkSetMacro(NumberOfEigenvectors,int); 
  itkGetMacro(NumberOfEigenvectors,int);

  /** Get the intermediate computations of this class as images 
      for visualization */
  const ImageType * GetNormalizedWeightMatrixImage() const;
  const ImageType * GetEigenvectorsImage() const;

  /** Normalized cuts normalization of embedding vectors */
  void SetEmbeddingNormalizationToRowSum()
    {
      this->SetEmbeddingNormalization(ROW_SUM);
    };

  /** Spectral clustering normalization of embedding vectors */
  void SetEmbeddingNormalizationToLengthOne()
    {
      this->SetEmbeddingNormalization(LENGTH_ONE);
    };

  /** No normalization of embedding vectors */
  void SetEmbeddingNormalizationToNone()
    {
      this->SetEmbeddingNormalization(NONE);
    };

  /** Return type of embedding normalization */
  itkGetMacro(EmbeddingNormalization,int);

  /** Make an itk image to visualize contents of a vnl matrix */
  ImageType *ConvertVNLMatrixToVTKImage(InputMatrixType *matrix);

  /** Write embedding vector coordinates to output file embed.txt 
      (useful to visualize embedding vectors using external code) */
  itkSetMacro(SaveEmbeddingVectors, int);
  itkGetMacro(SaveEmbeddingVectors, int);
  itkBooleanMacro(SaveEmbeddingVectors);

 protected:
  NormalizedCuts();
  ~NormalizedCuts() {};

  void PrintSelf(ostream& os, Indent indent);

  InputMatrixType *InputWeightMatrix;
  SymmetricEigenAnalysis<InputMatrixType,> *EigenSystem;
  OutputClassifierType::Pointer OutputClassifier;

  int NumberOfClusters;
  int NumberOfEigenvectors;
  int EmbeddingNormalization;
  int SaveEmbeddingVectors;

  enum EmbeddingNormalizationType { NONE, ROW_SUM, LENGTH_ONE } ;

  itkSetMacro(EmbeddingNormalization,int);

  ImageType::Pointer NormalizedWeightMatrixImage;
  ImageType::Pointer EigenvectorsImage;

  virtual void SetNormalizedWeightMatrixImage(const ImageType *);
  virtual void SetEigenvectorsImage(const ImageType *);

 private:

   NormalizedCuts(const NormalizedCuts&); // Not implemented.
   void operator=(const NormalizedCuts&); // Not implemented.
};

}

#endif

