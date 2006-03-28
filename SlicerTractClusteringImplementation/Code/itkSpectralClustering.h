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
#ifndef __itkSpectralClustering_h
#define __itkSpectralClustering_h


// ITK objects
#include "itkAffinityClustering.h"

#include "itkListSample.h"
#include "itkVector.h"
#include "itkSampleClassifier.h"
#include "itkImage.h"
#include "itkDataObjectDecorator.h"

// vnl numerics lib 
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

namespace itk {

/** \class SpectralClustering
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


class  SpectralClustering : public AffinityClustering
{
 public:
  /** Standard class typedefs */
  typedef SpectralClustering     Self;
  typedef AffinityClustering      Superclass ;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkTypeMacro(SpectralClustering, AffinityClustering);
  itkNewMacro(Self);

  typedef Image<float,2>   ImageType;


  // Description
  // Compute the output. 
  // This code does 6 main steps.
  // 1. Normalize the AffinityMatrix (weight matrix).
  // 2. Compute the eigensystem of the normalized weight matrix.
  // 3. Compute embedding vectors from rows of the eigenvector matrix.
  // 4. Cluster embedding vectors using k-means.
  // 5. Sort output cluster centroids according to second eigenvector ordering.
  // 6. Calculate output cluster memberships.
  // Note there is one embedding vector per row of the weight matrix (per
  // data point), so embedding vector j corresponds to input row j.
  // 
  void GenerateData();

  /** Number of eigenvectors to use in the embedding */
  itkSetMacro(NumberOfEigenvectors,unsigned int); 
  itkGetMacro(NumberOfEigenvectors,unsigned int);

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
 //ImageType *ConvertVNLMatrixToVTKImage(InputMatrixType *matrix);

  /** Write embedding vector coordinates to output file embed.txt 
      (useful to visualize embedding vectors using external code) */
  itkSetMacro(SaveEmbeddingVectors, bool);
  itkGetMacro(SaveEmbeddingVectors, bool);
  itkBooleanMacro(SaveEmbeddingVectors);

 /** Write eigenvectors output file eigenvectors.txt */
  itkSetMacro(SaveEigenvectors, bool);
  itkGetMacro(SaveEigenvectors, bool);
  itkBooleanMacro(SaveEigenvectors);

 protected:
  SpectralClustering();
  ~SpectralClustering();

  void PrintSelf(std::ostream& os, Indent indent) const;

  typedef vnl_matrix<double> MatrixType;
  MatrixType m_NormalizedWeightMatrix;

  typedef vnl_symmetric_eigensystem<double> EigenSystemType;
  EigenSystemType *m_EigenSystem;

  unsigned int m_NumberOfEigenvectors;
  int m_EmbeddingNormalization;
  bool m_SaveEmbeddingVectors;
  bool m_SaveEigenvectors;

  enum EmbeddingNormalizationType { NONE, ROW_SUM, LENGTH_ONE } ;

  itkSetMacro(EmbeddingNormalization,int);

  ImageType::Pointer m_NormalizedWeightMatrixImage;
  ImageType::Pointer m_EigenvectorsImage;

 //virtual void SetNormalizedWeightMatrixImage(const ImageType *);
 //virtual void SetEigenvectorsImage(const ImageType *);

 private:

   SpectralClustering(const SpectralClustering&); // Not implemented.
   void operator=(const SpectralClustering&); // Not implemented.

   unsigned long m_NumberOfEigenVectorsMTime;
};

}

#endif

