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
#include "vtkNormalizedCuts.h"

// for vtk objects we use here
#include "vtkObjectFactory.h"

// ITK objects
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkMinimumDecisionRule.h"
#include "itkExceptionObject.h"

// for debug output/visualization of features
#include "vtkImageData.h"

// for random k-means init
#include <cstdlib>
#include <ctime>


vtkCxxRevisionMacro(vtkNormalizedCuts, "$Revision: 1.15 $");
vtkStandardNewMacro(vtkNormalizedCuts);

vtkCxxSetObjectMacro(vtkNormalizedCuts,NormalizedWeightMatrixImage, 
                     vtkImageData);
vtkCxxSetObjectMacro(vtkNormalizedCuts,EigenvectorsImage, vtkImageData);

vtkNormalizedCuts::vtkNormalizedCuts()
{
  this->NumberOfClusters = 2;
  this->NumberOfEigenvectors = this->InternalNumberOfEigenvectors;
  this->EmbeddingNormalization = ROW_SUM;

  this->NormalizedWeightMatrixImage = NULL;
  this->EigenvectorsImage = NULL;
  this->EigenSystem = NULL;

  this->SaveEmbeddingVectors = 0;
}

void vtkNormalizedCuts::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "InputWeightMatrix: " << this->InputWeightMatrix << "\n";
  os << indent << "NumberOfClusters: " << this->NumberOfClusters << "\n";
  os << indent << "NumberOfEigenvectors: " << this->NumberOfEigenvectors << "\n";

}


void vtkNormalizedCuts::ComputeClusters()
{
  // test we have input
  if (this->InputWeightMatrix == NULL)
    {
      vtkDebugMacro("You must set the InputWeightMatrix before using this class.");
      return;
    }

  // -------------------------------------------------
  // Step 1: Normalize the weight matrix.
  // -------------------------------------------------
  // Create normalized Laplacian from weight matrix.
  // See Fowlkes et al., Spectral Grouping Using the Nystro¨m Method
  // IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE, 
  // VOL. 26, NO. 2, FEBRUARY 2004
  // Normalize: D^(-1/2) W D^(-1/2) 
  // Where W = square, symmetric weight matrix.
  // And D = diagonal matrix with row sum of W on diagonal
  // Then leading eigenvectors of the above are used to partition.
  // This D^(-1/2) normalization method amounts to the following:
  // compute row sum of W as a vector, for each
  // number then take 1/sqrt of it.  Then multiply columns and rows 
  // by this.

  // first compute normalization factors by summing rows
  typedef vnl_vector<double> VectorType;
  // create vector initialized to zero
  VectorType rowWeightSum(this->InputWeightMatrix->rows(),0);
  int idx1, idx2;
  idx1=0;
  while (idx1 < this->InputWeightMatrix->rows())
    {
      idx2=0;
      while (idx2 < this->InputWeightMatrix->cols())
        {
          // sum the row
          rowWeightSum[idx1] += (*this->InputWeightMatrix)[idx1][idx2];
          // TEST to turn off normalization 
          // rowWeightSum[idx1] =1;
          idx2++;
        }
      // take square root of final row sum
      rowWeightSum[idx1] = sqrt(rowWeightSum[idx1]);
      vtkDebugMacro("row " << idx1 << " sum: " << rowWeightSum[idx1]);
      idx1++;
    }

  // Delete any old output image from previous inputs
  if (this->NormalizedWeightMatrixImage) 
    {
      this->NormalizedWeightMatrixImage->Delete();
      this->NormalizedWeightMatrixImage = NULL;
    }

  // Iterate over the matrix to perform normalization.
  idx1=0;
  while (idx1 < this->InputWeightMatrix->rows())
    {
      idx2=0;
      while (idx2 < this->InputWeightMatrix->cols())
        {
          (*this->InputWeightMatrix)[idx1][idx2] = 
            ((*this->InputWeightMatrix)[idx1][idx2])/
            (rowWeightSum[idx1]*rowWeightSum[idx2]);
      
          idx2++;
        }
      idx1++;
    }

  // Test the matrix is finite (not NaN or Inf), otherwise can have
  // an error in eigensystem computation
  if (!this->InputWeightMatrix->is_finite())
    {    
      // Exit here with an error
      vtkErrorMacro("Normalized input weight matrix not finite (Nan or Inf)");
      return;
    }

  // ----------------------------------------------------------------
  // Step 2: Compute the eigensystem of the normalized weight matrix.
  // ----------------------------------------------------------------

  // Get rid of old values
  if ( this->EigenSystem )
    {
      delete this->EigenSystem;
    }
  if (this->EigenvectorsImage) 
    {
      this->EigenvectorsImage->Delete();
      this->EigenvectorsImage = NULL;
    }

  // Calculate eigenvectors
  this->EigenSystem = 
    new vnl_symmetric_eigensystem<double> (*this->InputWeightMatrix);


  vtkDebugMacro("Eigenvector matrix: " << this->EigenSystem->V << 
                " Eigenvalues: " << this->EigenSystem->D);

  vtkDebugMacro("Eigenvalues: " << this->EigenSystem->D);


  // ----------------------------------------------------------------------
  // Step 3: Compute embedding vectors from rows of the eigenvector matrix.
  // ----------------------------------------------------------------------

  // Create new feature vectors using the eigenvector embedding.
  // eigenvectors are sorted with smoothest (major) last. 
  // TEST must have more than this many tracts for the code to run
  // TEST We need to let user specify embedding vector length!
  // TEST this is a problem, now we must specify vector length at compile time.
  EmbedVectorType ev;
  EmbedSampleType::Pointer embedding = EmbedSampleType::New();
  
  // write to disk if requested
  ofstream fileEmbed;
  if (this->SaveEmbeddingVectors)
    {
      fileEmbed.open("embed.txt");
    }

  idx1=0;
  // outer loop over rows of eigenvector matrix, to
  // pick out all entries for one tract
  while (idx1 < this->EigenSystem->V.rows())
    {    
      idx2=0;

      // inner loop over columns of eigenvector matrix
      // place entries for this tract into an itk vector
      // skip first eigenvector (first column) because approx. constant.
      // use as many eigenvectors (columns) as necessary.
      // TEST embed vector length (number of evectors) needs to be specified at run time.
      // TEST put formula here
      double length = 0;
      while (idx2 < this->InternalNumberOfEigenvectors)
        {
          // this was wrong.
          //ev[idx2]=(this->EigenSystem->V[idx1][idx2+1])/rowWeightSum[idx1];
          // This included the constant major eigenvector 
          //ev[idx2]=(this->EigenSystem->V[idx1][this->EigenSystem->V.cols()-idx2-1]);

          // This is correct.
          ev[idx2]=(this->EigenSystem->V[idx1][this->EigenSystem->V.cols()-idx2-2]);

          // Take into account user-specified normalization method
          if (this->EmbeddingNormalization == LENGTH_ONE)
            {
              // general spectral clustering normalization
              length += ev[idx2]*ev[idx2];
            }
          if (this->EmbeddingNormalization == ROW_SUM)
            {
              // corresponds to normalized cuts
              ev[idx2]=ev[idx2]/rowWeightSum[idx1];
            }
          // else don't normalize

          idx2++;
        }
      
      // If the embedding normalization is by length, normalize now
      if (this->EmbeddingNormalization == LENGTH_ONE)
        {
          length = sqrt(length);
          if (length == 0)
            {
              vtkErrorMacro("0-length embedding vector %d" << idx1);
              length = 1;
            }
          for (idx2 = 0; idx2 < this->InternalNumberOfEigenvectors; idx2++)
            {
              ev[idx2]=ev[idx2]/length;
            }
        }

      // write to disk if requested
      if (this->SaveEmbeddingVectors)
        {
          for (idx2 = 0; idx2 < this->InternalNumberOfEigenvectors; idx2++)
            {
              fileEmbed << ev[idx2] << " ";
            }
          fileEmbed << endl;
        }

      // put the embedding vector for this tract onto the sample list
      embedding->PushBack( ev );      
      idx1++;
    }

  // write to disk if requested
  if (this->SaveEmbeddingVectors)
    fileEmbed.close();


  // -------------------------------------------------
  // Step 4: Cluster embedding vectors using k-means.
  // -------------------------------------------------
  // TEST we would like functionality like matlab in order to 
  // do multiple runs of kmeans and pick the best.

  // Create a Kd tree and populate it with the embedding vectors,
  // in order to run k-means.
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator< EmbedSampleType > 
    TreeGeneratorType;
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

  treeGenerator->SetSample( embedding );
  // This is the number of items that can go into a leaf node.
  // it seems not to work with 1
  // TEST what is relation of bucket size to min cluster size?
  // TEST could calculate depending on input size
  treeGenerator->SetBucketSize( 3 );
  //treeGenerator->SetBucketSize( 16 );
  try {
    treeGenerator->Update();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in tree generation: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in tree generation");
    return;
  }

  // Run kmeans on the new vectors (eigenvector embedding of tract info)
  // store these as that sample list with classes 
  typedef TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();

  // In the parameters vector, the vectors for each of the means are 
  // concatenated.  So array size is vector_length*number_of_vector_means.
  int numberOfClusters=this->NumberOfClusters;
  EstimatorType::ParametersType initialMeans(this->InternalNumberOfEigenvectors*numberOfClusters);


  // Now we try to choose evenly-spaced initial centroids for k-means
  // TEST though finding centroids 90 degrees apart is suggested in
  // the literature, is it helpful with our data?
  std::srand ( static_cast<unsigned>(time(0)) );
  int sampleIdx;
  // keep track of chosen initial centroids
  EmbedSampleType::Pointer centroids = EmbedSampleType::New();
  EmbedSampleType::Pointer testCentroids = EmbedSampleType::New();
  EmbedVectorType cent;

  for (idx1 = 0; idx1 < numberOfClusters; idx1 ++)
    {
      // Init means with randomly selected sample member vectors.
      // index is random number between 0 and number of vectors-1.
      sampleIdx = std::rand()%embedding->Size();
      ev = embedding->GetMeasurementVector(sampleIdx);

      // To aim for evenly spread centroids in the embedding space,
      // ideally we would pick the data point that is furthest away
      // from all chosen centroids. But this would mean looping over
      // all of the embedding vectors. Instead randomly choose several
      // potential centroids, and just compare to already-chosen
      // centroids.  keep the new centroid that is furthest from the
      // already-chosen centroids.
      if (idx1 != 0)
        {
          const int numTestCentroids = 5;
          // TEST this line turns off the attempt to select good centroids
          // and reverts to original random selection only
          //const int numTestCentroids = 1;
          
          // measure how good each centroid is (want least similar to others)
          double similarity[numTestCentroids];

          // choose several centroids and measure their quality
          for (int idxChoice = 0; idxChoice < numTestCentroids; idxChoice++)
            {
              testCentroids->PushBack( ev );      
              similarity[idxChoice] = 0;
              
              // measure against centroids chosen so far
              for (int idxCentroid = 0; idxCentroid < idx1; idxCentroid++)
                {
                  cent = centroids->GetMeasurementVector(idxCentroid);

                  // dot product as similarity measure.  for this
                  // application embedding points are approximately on
                  // the surface of a sphere so we want angular
                  // separation between centroids => dot product.
                  double dot = 0;
                  for (int idxComp = 0; 
                       idxComp < this->InternalNumberOfEigenvectors; idxComp++)
                    {                  
                      dot += ev[idxComp]*cent[idxComp];
                    }
                  // we want approx 90 degrees apart, so we
                  // want dot products close to 0 => use abs of dot
                  // product, then we can do min later.
                  similarity[idxChoice] += fabs(dot);
                  
                }

              // get the next centroid to test
              sampleIdx = std::rand()%embedding->Size();
              ev = embedding->GetMeasurementVector(sampleIdx);
            }
          
          // now find the best one with the minimim similarity to others
          double minSimilarity = similarity[0];
          ev = testCentroids->GetMeasurementVector(0);
          for (int idxChoice = 1; idxChoice < numTestCentroids; idxChoice++)
            {
              if (similarity[idxChoice] < minSimilarity)
                {
                  minSimilarity = similarity[idxChoice];
                  ev = testCentroids->GetMeasurementVector(idxChoice);
                }
            }          
        }


      // save the chosen centroid for use when picking more centroids
      centroids->PushBack( ev );      

    }

  int meanIdx = 0;
  // loop over final ordered centroid list
  for (idx1 = 0; idx1 < numberOfClusters; idx1 ++)
    {
      // find next one in final ordered centroid list
      ev = centroids->GetMeasurementVector(idx1);

      // Put the centroid into the strange format for input to 
      // the estimator
      for (idx2 = 0; idx2 < this->InternalNumberOfEigenvectors; idx2++)
        {
          initialMeans[meanIdx] = ev[idx2];
          meanIdx++;
        }
    }  

  // Now back to standard itk k-means 
  estimator->SetParameters( initialMeans );
  vtkDebugMacro("estimator params: " << estimator->GetParameters());
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 200 );
  //estimator->SetMaximumIteration( 1000 );
  //estimator->SetMaximumIteration( 20 );
  estimator->SetCentroidPositionChangesThreshold(0.0);
  try {
    estimator->StartOptimization();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in cluster estimation: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in cluster estimation");
    return;
  }

  EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();
  vtkDebugMacro("Final estimator params: " << estimator->GetParameters());
  for ( unsigned int i = 0 ; i < 2 ; ++i )
    {
      std::cout << "cluster[" << i << "] " << std::endl;
      std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
      
      vtkDebugMacro("cluster[" << i << "] ");
      vtkDebugMacro("    estimated mean : " << estimatedMeans[i]);
    }

  // Copy the final centroids into a format we can understand
  meanIdx = 0;
  centroids->Clear();
  // loop over final centroid list
  for (idx1 = 0; idx1 < numberOfClusters; idx1 ++)
    {
      // Get the centroid out of the strange format from the estimator
      for (idx2 = 0; idx2 < this->InternalNumberOfEigenvectors; idx2++)
        {
          ev[idx2] = estimatedMeans[meanIdx];
          meanIdx++;
        }

      // put it onto the final centroid list
      centroids->PushBack(ev);
    }  

  // ------------------------------------------------------------------------------
  // Step 5: Sort output cluster centroids according to second eigenvector ordering
  // ------------------------------------------------------------------------------

  // Now that we have found the final centroids, order them according
  // to their first component (so output class labels can be sorted
  // according to the second eigenvector of the normalized laplacian)
  std::vector< double > firstComp;
  firstComp.resize( numberOfClusters );

  // first get all the first components
  for (idx1 = 0; idx1 < numberOfClusters; idx1 ++)
    {
      ev = centroids->GetMeasurementVector(idx1);
      firstComp[idx1] = ev[0];
    }

  // now order the centroids to make the class label list
  std::vector< unsigned int > classLabels;
  classLabels.resize( numberOfClusters );
  int label = 0;
  for (idx1 = 0; idx1 < numberOfClusters; idx1 ++)
    {
      double minComp = firstComp[0];
      int minIdx = 0;
      for (idx2 = 0; idx2 < numberOfClusters; idx2 ++)
        {
          if (firstComp[idx2] < minComp)
            {
              minComp = firstComp[idx2];
              minIdx = idx2;
            }
        }
      // remove the min so we find the next-largest next time
      firstComp[minIdx] = VTK_DOUBLE_MAX;
      // give this centroid the next class label
      classLabels[minIdx] = label;
      label++;
    }

  // -------------------------------------------------
  // Step 6: Calculate output cluster memberships.
  // -------------------------------------------------

  typedef itk::Statistics::EuclideanDistance< EmbedVectorType > 
    MembershipFunctionType;
  typedef itk::MinimumDecisionRule DecisionRuleType;
  DecisionRuleType::Pointer decisionRule = DecisionRuleType::New();
  
  // this smart pointer will deallocate anything old it may have
  // pointed to, so this line is okay.
  this->OutputClassifier = OutputClassifierType::New();

  //this->OutputClassifier->DebugOn();

  this->OutputClassifier->SetDecisionRule( (itk::DecisionRuleBase::Pointer) decisionRule);
  this->OutputClassifier->SetSample( embedding );
  // need to keep this object around, increment its reference count
  // otherwise it is deleted but the classifier is still around, 
  // and we need to fully use the classifier later.
  embedding->Register();
  //embedding->DebugOn();
  this->OutputClassifier->SetNumberOfClasses( numberOfClusters );
  // label according to second eigenvector ordering
  this->OutputClassifier->SetMembershipFunctionClassLabels( classLabels );


  std::vector< MembershipFunctionType::Pointer > membershipFunctions;
  MembershipFunctionType::OriginType origin;
  int index = 0;
  for ( unsigned int i = 0 ; i < numberOfClusters ; i++ ) 
    {
      membershipFunctions.push_back( MembershipFunctionType::New() );
      for ( unsigned int j = 0 ; j < EmbedSampleType::MeasurementVectorSize ; j++ )
        {
          origin[j] = estimatedMeans[index++];
        }
      membershipFunctions[i]->SetOrigin( origin );

      vtkDebugMacro("origin " << i << " " << membershipFunctions[i]->GetOrigin());

      this->OutputClassifier->AddMembershipFunction( membershipFunctions[i].GetPointer() );
    }

  try {
    this->OutputClassifier->Update();
  }
  catch (itk::ExceptionObject &e) {
    vtkErrorMacro("Error in update of output classifier: " << e);
    return;
  }
  catch (...) {
    vtkErrorMacro("Error in update of output classifier");
    return;
  }
  //this->OutputClassifier->GetOutput()->DebugOn();

}



vtkImageData * vtkNormalizedCuts::GetNormalizedWeightMatrixImage()
{
  if (this->NormalizedWeightMatrixImage == NULL)
    {
      this->SetNormalizedWeightMatrixImage(this->ConvertVNLMatrixToVTKImage(this->InputWeightMatrix));
    }
  return(this->NormalizedWeightMatrixImage);
}

vtkImageData * vtkNormalizedCuts::GetEigenvectorsImage()
{
  if (this->EigenvectorsImage == NULL)
    {
      this->SetEigenvectorsImage(this->ConvertVNLMatrixToVTKImage(&(this->EigenSystem->V)));
    }
  return (this->EigenvectorsImage);
}

vtkImageData *vtkNormalizedCuts::ConvertVNLMatrixToVTKImage(InputType *matrix)
{
  vtkImageData *image = vtkImageData::New();

  if (matrix != NULL)
    {
      int rows = matrix->rows();
      int cols = matrix->cols();
      image->SetDimensions(cols,rows,1);
      image->SetScalarTypeToDouble();
      image->AllocateScalars();
      double *imageArray = (double *) image->GetScalarPointer();
      
      for (int idx1 = rows-1; idx1 >= 0; idx1--)
        {
          for (int idx2 = 0; idx2 < cols; idx2++)
            {
              *imageArray = (*matrix)[idx1][idx2];
              imageArray++;
            }
        }
    }

  return(image);
}
