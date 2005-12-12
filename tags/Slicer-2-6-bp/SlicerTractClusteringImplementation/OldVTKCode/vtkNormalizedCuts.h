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
// .NAME vtkNormalizedCuts - Cluster N items using input NxN weight matrix.

// .SECTION Description
// Implementation of the Normalized Cuts algorithm for spectral clustering,
// as described in:
// Fowlkes et al., Spectral Grouping Using the Nystrom Method
// IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, 
// VOL. 26, NO. 2, FEBRUARY 2004
// The output is an itk classifier object, with labels for each of 
// the N input items.
// Depending on the type of embedding normalization chosen, performs either
// Normalized Cuts or general spectral clustering.
// The number of clusters must be specified by the user.

// .SECTION See Also
// vtkClusterTracts vtkTractShapeFeatures

#ifndef __vtkNormalizedCuts_h
#define __vtkNormalizedCuts_h

//#include "vtkDTMRIConfigure.h"
#include "vtkObject.h"
// use itk numerics lib (vnl)
#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
// ITK objects
#include "itkListSample.h"
#include "itkVector.h"
#include "itkSampleClassifier.h"

// Forward declarations to avoid including header files here.
// Goes along with use of new vtkCxxSetObjectMacro
class vtkImageData;

//class VTK_DTMRI_EXPORT vtkNormalizedCuts : public vtkObject
class  vtkNormalizedCuts : public vtkObject
{
 public:
  // Description
  // Construct
  static vtkNormalizedCuts *New();

  vtkTypeRevisionMacro(vtkNormalizedCuts,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

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

  // Description
  // Use the matrix format provided by vnl for the input similarity matrix
  //BTX
  typedef vnl_matrix<double> InputType;
  // TEST we want this specified at run time!
  const static int InternalNumberOfEigenvectors = 2;

  typedef itk::Vector< double, InternalNumberOfEigenvectors > EmbedVectorType;
  typedef itk::Statistics::ListSample< EmbedVectorType > EmbedSampleType;
  typedef itk::Statistics::SampleClassifier< EmbedSampleType > OutputClassifierType;

  //ETX

  // Description
  // Set input to this class.  This matrix should be NxN, where N
  // is the number of items we want to cluster.  Entries should be 
  // similarity values (not distances).
  void SetInputWeightMatrix(InputType* matrix)
    {
      // Don't delete this here if it is non-NULL, because the
      // class that created it is responsible for deleting it.
      // (It is not a VTK reference-counted object.)
      this->InputWeightMatrix = matrix;
    };
  
  // Description
  // Get input to this class. 
  InputType *GetInputWeightMatrix()
    {
      return this->InputWeightMatrix;
    };

  //BTX
  // Description
  // Get the classifier which is output from this class
  OutputClassifierType::Pointer GetOutputClassifier() 
    {
      return this->OutputClassifier;
    }
  //ETX

  // Description
  // Number of clusters to output
  vtkSetMacro(NumberOfClusters,int);
  vtkGetMacro(NumberOfClusters,int);

  // Description
  // Number of eigenvectors to use in the embedding
  // TEST not currently implemented due to fixed-length itk vector!
  //vtkSetMacro(NumberOfEigenvectors,int);
  vtkGetMacro(NumberOfEigenvectors,int);

  // Description
  // Get the intermediate computations of this class as images 
  // for visualization
  vtkImageData *GetNormalizedWeightMatrixImage();
  vtkImageData *GetEigenvectorsImage();

  // Description
  // Normalized cuts normalization of embedding vectors
  void SetEmbeddingNormalizationToRowSum()
    {
      this->SetEmbeddingNormalization(ROW_SUM);
    };

  // Description
  // Spectral clustering normalization of embedding vectors
  void SetEmbeddingNormalizationToLengthOne()
    {
      this->SetEmbeddingNormalization(LENGTH_ONE);
    };

  // Description
  // No normalization of embedding vectors
  void SetEmbeddingNormalizationToNone()
    {
      this->SetEmbeddingNormalization(NONE);
    };

  // Description
  // Return type of embedding normalization
  vtkGetMacro(EmbeddingNormalization,int);

  // Description
  // Make a vtk image to visualize contents of a vnl matrix
  vtkImageData *ConvertVNLMatrixToVTKImage(InputType *matrix);

  // Description
  // Write embedding vector coordinates to output file embed.txt 
  // (useful to visualize embedding vectors using external code)
  vtkSetMacro(SaveEmbeddingVectors, int);
  vtkGetMacro(SaveEmbeddingVectors, int);
  vtkBooleanMacro(SaveEmbeddingVectors, int);

 protected:
  vtkNormalizedCuts();
  ~vtkNormalizedCuts() {};

  //BTX
  InputType *InputWeightMatrix;
  vnl_symmetric_eigensystem<double> *EigenSystem;
  OutputClassifierType::Pointer OutputClassifier;
  //ETX

  int NumberOfClusters;
  int NumberOfEigenvectors;
  int EmbeddingNormalization;
  int SaveEmbeddingVectors;

  //BTX
  enum EmbeddingNormalizationType { NONE, ROW_SUM, LENGTH_ONE } ;
  //ETX
  vtkSetMacro(EmbeddingNormalization,int);

  vtkImageData *NormalizedWeightMatrixImage;
  vtkImageData *EigenvectorsImage;
  // functions for vtkCxxSetObjectMacro:
  virtual void SetNormalizedWeightMatrixImage(vtkImageData *);
  virtual void SetEigenvectorsImage(vtkImageData *);

 private:
  vtkNormalizedCuts(const vtkNormalizedCuts&); // Not implemented.
  void operator=(const vtkNormalizedCuts&); // Not implemented.
};

#endif

