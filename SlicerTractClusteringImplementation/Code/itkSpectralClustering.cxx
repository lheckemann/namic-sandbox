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
#include "itkSpectralClustering.h"

// ITK objects
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkMinimumDecisionRule.h"
#include "itkExceptionObject.h"

// for debug output/visualization of features
//#include "itkImage.h"
#include <fstream>

// for random k-means init
#include <cstdlib>
#include <ctime>

namespace itk {

SpectralClustering::SpectralClustering()
{
  m_NumberOfClusters = 2;
  m_NumberOfEigenvectors = 2;
  m_EmbeddingNormalization = ROW_SUM;

  m_NormalizedWeightMatrix = MatrixType(1,1);

  m_EigenSystem = NULL;
  m_NormalizedWeightMatrixImage = NULL;
  m_EigenvectorsImage = NULL;

  m_SaveEmbeddingVectors = 0;
  m_SaveEigenvectors = 0;

  // Create the output. We use static_cast<> here because we know the default
  // output must be of type OutputClassifierDataObjectType
  OutputClassifierDataObjectType::Pointer output
    = static_cast<OutputClassifierDataObjectType*>(this->MakeOutput(0).GetPointer()); 
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
}

DataObject::Pointer
SpectralClustering
::MakeOutput(unsigned int)
{
  OutputClassifierDataObjectType::Pointer dataobject = OutputClassifierDataObjectType::New();
  OutputClassifierType::Pointer classifier = OutputClassifierType::New();
  dataobject->Set( classifier );

  return static_cast<DataObject*>(dataobject.GetPointer());
}

void
SpectralClustering
::SetNumberOfClusters(int num)
{
  if (num != m_NumberOfClusters)
    {
      m_NumberOfClusters = num;
      this->Modified();
      //m_NumberOfClustersMTime = 
    }
}

void SpectralClustering::PrintSelf(std::ostream& os, Indent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "NumberOfClusters: " << m_NumberOfClusters << "\n";
  os << indent << "NumberOfEigenvectors: " << m_NumberOfEigenvectors << "\n";
  os << indent << "m_EmbeddingNormalization: " << m_EmbeddingNormalization << "\n";
  os << indent << "m_SaveEmbeddingVectors: " << m_SaveEmbeddingVectors << "\n";

}


void SpectralClustering::ComputeClusters()
{
  // test we have input
  if (this->GetInput() == 0)
    {
      itkExceptionMacro("You must set the input (weight matrix in list sample form) before using this class.");
    }

  const InputListSampleType *input = this->GetInput()->Get();

  int numberOfItemsToCluster = input->Size();

  // Make sure the input represents a square matrix (list sample items
  // must have length equal to number of items to cluster).
  if (input->GetMeasurementVectorSize() != numberOfItemsToCluster)
    {
      itkExceptionMacro("Weight vector lengths must be equal to number of items to cluster");
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
  VectorType rowWeightSum(numberOfItemsToCluster,0);
  int idx1, idx2;
  WeightVectorType wv;
  for (idx1 = 0; idx1 < numberOfItemsToCluster; idx1++)
    {
      // find next input vector (corresponds to row of matrix)
      wv = input->GetMeasurementVector(idx1);

      for (idx2 = 0; idx2 < numberOfItemsToCluster; idx2++)
        {
          // sum the row
          rowWeightSum[idx1] += wv[idx2];
          // TEST to turn off normalization 
          // rowWeightSum[idx1] =1;

        }

      // take square root of final row sum
      rowWeightSum[idx1] = sqrt(rowWeightSum[idx1]);
      itkDebugMacro("row " << idx1 << " sum: " << rowWeightSum[idx1]);

    }

  // Initialize the VNL matrix
  m_NormalizedWeightMatrix = MatrixType(numberOfItemsToCluster,numberOfItemsToCluster);

  // Iterate over the matrix to perform normalization.
  // This step stores normalized input data (from list sample) into vnl matrix format.
  for (idx1 = 0; idx1 < numberOfItemsToCluster; idx1++)
    {
      // find next input vector (place into row of matrix)
      wv = input->GetMeasurementVector(idx1);

      for (idx2 = 0; idx2 < numberOfItemsToCluster; idx2++)
        {
          (m_NormalizedWeightMatrix)[idx1][idx2] = 
            (wv[idx2])/(rowWeightSum[idx1]*rowWeightSum[idx2]);
      
        }
    }

  // Test the matrix is finite (not NaN or Inf), otherwise can have
  // an error in eigensystem computation
  if (!m_NormalizedWeightMatrix.is_finite())
    {    
      // Exit here with an error
      itkExceptionMacro("Normalized input weight matrix not finite (Nan or Inf)");
      return;
    }


  // ----------------------------------------------------------------
  // Step 2: Compute the eigensystem of the normalized weight matrix.
  // ----------------------------------------------------------------

  // Get rid of old values
  if ( m_EigenSystem )
    {
      delete m_EigenSystem;
    }

  // Calculate eigenvectors
  m_EigenSystem = new EigenSystemType (m_NormalizedWeightMatrix);


  itkDebugMacro("Eigenvector matrix: " << m_EigenSystem->V << 
                " Eigenvalues: " << m_EigenSystem->D);

  itkDebugMacro("Eigenvalues: " << m_EigenSystem->D);


  // write to disk if requested
  std::ofstream fileEigenvectors;
  std::ofstream fileEigenvalues;
  if (m_SaveEigenvectors)
    {
      fileEigenvectors.open("eigenvectors.txt");
      fileEigenvalues.open("eigenvalues.txt");

      for (idx1 = 0; idx1 <  m_EigenSystem->V.rows(); idx1++)
        {
          
          fileEigenvalues <<   m_EigenSystem->D[idx1] << " ";

          for (idx2 = 0; idx2 <  m_EigenSystem->V.cols(); idx2++)
            {
              fileEigenvectors <<   m_EigenSystem->V[idx1][idx2] << " ";
            }

          fileEigenvectors <<  std::endl;
        }

      fileEigenvectors.close();
      fileEigenvalues.close();
    }

  // ----------------------------------------------------------------------
  // Step 3: Compute embedding vectors from rows of the eigenvector matrix.
  // ----------------------------------------------------------------------

  // Create new feature vectors using the eigenvector embedding.
  // eigenvectors are sorted with smoothest (major) last. 
  // TEST must have more than this many tracts for the code to run
  EmbedVectorType ev ( m_NumberOfEigenvectors );
  EmbedSampleType::Pointer embedding = EmbedSampleType::New();
  embedding->SetMeasurementVectorSize( m_NumberOfEigenvectors );
  
  // write to disk if requested
  std::ofstream fileEmbed;
  if (m_SaveEmbeddingVectors)
    {
      fileEmbed.open("embed.txt");
    }

  idx1=0;
  // outer loop over rows of eigenvector matrix, to
  // pick out all entries for one tract
  while (idx1 < m_EigenSystem->V.rows())
    {    
      idx2=0;

      // inner loop over columns of eigenvector matrix
      // place entries for this tract into an itk vector
      // skip first eigenvector (first column) because approx. constant.
      // use as many eigenvectors (columns) as necessary.
      // TEST embed vector length (number of evectors) needs to be specified at run time.
      // TEST put formula here
      double length = 0;
      while (idx2 < m_NumberOfEigenvectors)
        {
          // this was wrong.
          //ev[idx2]=(m_EigenSystem->V[idx1][idx2+1])/rowWeightSum[idx1];
          // This included the constant major eigenvector 
          //ev[idx2]=(m_EigenSystem->V[idx1][m_EigenSystem->V.cols()-idx2-1]);

          // This is correct.
          ev[idx2]=(m_EigenSystem->V[idx1][m_EigenSystem->V.cols()-idx2-2]);

          // Take into account user-specified normalization method
          if (m_EmbeddingNormalization == LENGTH_ONE)
            {
              // general spectral clustering normalization
              length += ev[idx2]*ev[idx2];
            }
          if (m_EmbeddingNormalization == ROW_SUM)
            {
              // corresponds to normalized cuts
              ev[idx2]=ev[idx2]/rowWeightSum[idx1];
            }
          // else don't normalize

          idx2++;
        }
      
      // If the embedding normalization is by length, normalize now
      if (m_EmbeddingNormalization == LENGTH_ONE)
        {
          length = sqrt(length);
          if (length == 0)
            {
              itkExceptionMacro("0-length embedding vector %d" << idx1);
              length = 1;
            }
          for (idx2 = 0; idx2 < m_NumberOfEigenvectors; idx2++)
            {
              ev[idx2]=ev[idx2]/length;
            }
        }

      // write to disk if requested
      if (m_SaveEmbeddingVectors)
        {
          for (idx2 = 0; idx2 < m_NumberOfEigenvectors; idx2++)
            {
              fileEmbed << ev[idx2] << " ";
            }
          fileEmbed << std::endl;
        }

      // put the embedding vector for this tract onto the sample list
      embedding->PushBack( ev );      
      idx1++;
    }


  // write to disk if requested
  if (m_SaveEmbeddingVectors)
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

  std::cout << "tree gen sample size " << treeGenerator->GetMeasurementVectorSize() << std::endl;
  try {
    treeGenerator->SetSample( embedding );
    std::cout << "tree gen sample size " << treeGenerator->GetMeasurementVectorSize() << std::endl;
  }
  catch (itk::ExceptionObject &e) {
    itkExceptionMacro("Error in setting sample for tree: " << e);
    return;
  }
  catch (...) {
    itkExceptionMacro("Error in setting sample for tree");
    return;
  }

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
    itkExceptionMacro("Error in tree generation: " << e);
    return;
  }
  catch (...) {
    itkExceptionMacro("Error in tree generation");
    return;
  }

  // Run kmeans on the new vectors (eigenvector embedding of tract info)
  // store these as that sample list with classes 
  typedef TreeGeneratorType::KdTreeType TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType> EstimatorType;
  EstimatorType::Pointer estimator = EstimatorType::New();

  // In the parameters vector, the vectors for each of the means are 
  // concatenated.  So array size is vector_length*number_of_vector_means.
  EstimatorType::ParametersType initialMeans(m_NumberOfEigenvectors*m_NumberOfClusters);


  // Now we try to choose evenly-spaced initial centroids for k-means
  // TEST though finding centroids 90 degrees apart is suggested in
  // the literature, is it helpful with our data?
  std::srand ( static_cast<unsigned>(time(0)) );
  int sampleIdx;
  // keep track of chosen initial centroids
  EmbedSampleType::Pointer centroids = EmbedSampleType::New();
  EmbedSampleType::Pointer testCentroids = EmbedSampleType::New();

  centroids->SetMeasurementVectorSize( embedding->GetMeasurementVectorSize() );
  testCentroids->SetMeasurementVectorSize( embedding->GetMeasurementVectorSize() );
  
  EmbedVectorType cent( embedding->GetMeasurementVectorSize() );

  for (idx1 = 0; idx1 < m_NumberOfClusters; idx1 ++)
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
                       idxComp < m_NumberOfEigenvectors; idxComp++)
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
  for (idx1 = 0; idx1 < m_NumberOfClusters; idx1 ++)
    {
      // find next one in final ordered centroid list
      ev = centroids->GetMeasurementVector(idx1);

      // Put the centroid into the strange format for input to 
      // the estimator
      for (idx2 = 0; idx2 < m_NumberOfEigenvectors; idx2++)
        {
          initialMeans[meanIdx] = ev[idx2];
          meanIdx++;
        }
    }  

  // Now back to standard itk k-means 
  estimator->SetParameters( initialMeans );
  itkDebugMacro("estimator params: " << estimator->GetParameters());
  estimator->SetKdTree( treeGenerator->GetOutput() );
  estimator->SetMaximumIteration( 200 );
  //estimator->SetMaximumIteration( 1000 );
  //estimator->SetMaximumIteration( 20 );
  estimator->SetCentroidPositionChangesThreshold(0.0);
  try {
    estimator->StartOptimization();
  }
  catch (itk::ExceptionObject &e) {
    itkExceptionMacro("Error in cluster estimation: " << e);
    return;
  }
  catch (...) {
    itkExceptionMacro("Error in cluster estimation");
    return;
  }

  EstimatorType::ParametersType estimatedMeans = estimator->GetParameters();
  itkDebugMacro("Final estimator params: " << estimator->GetParameters());
  for ( unsigned int i = 0 ; i < 2 ; ++i )
    {
      std::cout << "cluster[" << i << "] " << std::endl;
      std::cout << "    estimated mean : " << estimatedMeans[i] << std::endl;
      
      itkDebugMacro("cluster[" << i << "] ");
      itkDebugMacro("    estimated mean : " << estimatedMeans[i]);
    }

  // Copy the final centroids into a format we can understand
  meanIdx = 0;
  centroids->Clear();
  // loop over final centroid list
  for (idx1 = 0; idx1 < m_NumberOfClusters; idx1 ++)
    {
      // Get the centroid out of the strange format from the estimator
      for (idx2 = 0; idx2 < m_NumberOfEigenvectors; idx2++)
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
  firstComp.resize( m_NumberOfClusters );

  // first get all the first components
  for (idx1 = 0; idx1 < m_NumberOfClusters; idx1 ++)
    {
      ev = centroids->GetMeasurementVector(idx1);
      firstComp[idx1] = ev[0];
    }

  // now order the centroids to make the class label list
  std::vector< unsigned int > classLabels;
  classLabels.resize( m_NumberOfClusters );
  int label = 0;
  for (idx1 = 0; idx1 < m_NumberOfClusters; idx1 ++)
    {
      double minComp = firstComp[0];
      int minIdx = 0;
      for (idx2 = 0; idx2 < m_NumberOfClusters; idx2 ++)
        {
          if (firstComp[idx2] < minComp)
            {
              minComp = firstComp[idx2];
              minIdx = idx2;
            }
        }
      // remove the min so we find the next-largest next time
      firstComp[minIdx] = NumericTraits<double>::max();
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
  
  // 
  OutputClassifierDataObjectType *outputDataObject = this->GetOutput();
  OutputClassifierType *output = outputDataObject->Get();

  //output->DebugOn();

  output->SetDecisionRule( (itk::DecisionRuleBase::Pointer) decisionRule);
  try 
    {
      output->SetSample( embedding );
    }
  catch (itk::ExceptionObject &e) {
    itkExceptionMacro("Error in setting sample: " << e);
    return;
  }

  // need to keep this object around, increment its reference count
  // otherwise it is deleted but the classifier is still around, 
  // and we need to fully use the classifier later.
  embedding->Register();
  //embedding->DebugOn();
  output->SetNumberOfClasses( m_NumberOfClusters );
  // label according to second eigenvector ordering
  output->SetMembershipFunctionClassLabels( classLabels );


  std::vector< MembershipFunctionType::Pointer > membershipFunctions;

  MembershipFunctionType::OriginType origin( embedding->GetMeasurementVectorSize() ) ;
  int index = 0;
  for ( unsigned int i = 0 ; i < m_NumberOfClusters ; i++ ) 
    {
      membershipFunctions.push_back( MembershipFunctionType::New() );
      for ( unsigned int j = 0 ; j < origin.Size() ; j++ )
        {
          origin[j] = estimatedMeans[index++];
        }
      membershipFunctions[i]->SetOrigin( origin );

      itkDebugMacro("origin " << i << " " << membershipFunctions[i]->GetOrigin());

      output->AddMembershipFunction( membershipFunctions[i].GetPointer() );
    }

  try {
    output->Update();
  }
  catch (itk::ExceptionObject &e) {
    itkExceptionMacro("Error in update of output classifier: " << e);
    return;
  }
  catch (...) {
    itkExceptionMacro("Error in update of output classifier");
    return;
  }
  //output->GetOutput()->DebugOn();

}



// ImageData * SpectralClustering::GetNormalizedWeightMatrixImage()
// {
//   if (this->NormalizedWeightMatrixImage == NULL)
//     {
//       this->SetNormalizedWeightMatrixImage(this->ConvertVNLMatrixToImage(m_NormalizedWeightMatrix));
//     }
//   return(this->NormalizedWeightMatrixImage);
// }

// ImageData * SpectralClustering::GetEigenvectorsImage()
// {
//   if (this->EigenvectorsImage == NULL)
//     {
//       this->SetEigenvectorsImage(this->ConvertVNLMatrixToImage(&(m_EigenSystem->V)));
//     }
//   return (this->EigenvectorsImage);
// }

// ImageData *SpectralClustering::ConvertVNLMatrixToImage(InputType *matrix)
// {
//   ImageData *image = ImageData::New();

//   if (matrix != NULL)
//     {
//       int rows = matrix->rows();
//       int cols = matrix->cols();
//       image->SetDimensions(cols,rows,1);
//       image->SetScalarTypeToDouble();
//       image->AllocateScalars();
//       double *imageArray = (double *) image->GetScalarPointer();
      
//       for (int idx1 = rows-1; idx1 >= 0; idx1--)
//         {
//           for (int idx2 = 0; idx2 < cols; idx2++)
//             {
//               *imageArray = (*matrix)[idx1][idx2];
//               imageArray++;
//             }
//         }
//     }

//   return(image);
// }

} // end of namespace itk
