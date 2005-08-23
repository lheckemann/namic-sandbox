//
// testITKCluster.cxx: Test code for itkSpectralClustering class.
// 
// Creates ideal sample input and runs clustering.
// Produces 3 text files:
//   inputClusters.txt   (input cluster indices)
//   inputMatrix.txt   (input similarity matrix)
//   embed.txt           (embedding vectors)
//   outputClusters.txt  (output cluster indices)
// Also outputs the normalized weight matrix as an image.
//
// (There is one bug, I can't find the image output on windows.
// Apparently it is not written to the directory where I run the file.)
//

#include <fstream>
#include <iostream>
#include "itkSpectralClustering.h"

int main(int argc, char* argv[])
{

  if(argc == 0)
    {
      std::cout << "Usage: testCluster <infile> <outfile> <# Clusters>" << std::endl;
      return 0;
    }

  // create input test data
  // -----------------------

  // number of things to cluster
  // This is hard-coded in itkSpectralClustering until we have
  // run time vector length specification.
  int numberOfItemsToCluster = 21;

  // Length of each input weight vector is the same as the number 
  // of items to cluster, since weights compare each item to every other.
  typedef itk::SpectralClustering::WeightVectorType WeightVectorType;
  int weightVectorLength = numberOfItemsToCluster;

  // now make a simulated perfect input list (matrix), block diagonal
  // within-cluster similarity = 1 and all other entries = 0;
  itk::SpectralClustering::InputListSampleDataObjectType::Pointer weightListWrapper = 
    itk::SpectralClustering::InputListSampleDataObjectType::New();

  itk::SpectralClustering::InputListSampleType::Pointer weightList = 
    itk::SpectralClustering::InputListSampleType::New();

  weightListWrapper->Set(weightList);

  // Explicitly set the length: (length of a measurement vector in the sample)
  weightList->SetMeasurementVectorSize( weightVectorLength );

  int numberOfClusters = 3;
  int numberOfEigenvectors = 2;
  int clusterSize = numberOfItemsToCluster/numberOfClusters;
  int clusterIdx = 1;
  std::ofstream fileInputClusters;
  fileInputClusters.open("inputClusters.txt");
  std::ofstream fileInputMatrix;
  fileInputMatrix.open("inputMatrix.txt");

  for (int row = 0; row < numberOfItemsToCluster; row++)
    {
      if (row == clusterIdx*clusterSize)
        clusterIdx++;

      // output cluster indices to disk
      fileInputClusters << clusterIdx << std::endl;

      // new weight vector for each row of the matrix
      WeightVectorType wv( weightVectorLength );

      for (int col = 0; col < weightVectorLength; col++)
        {
          wv[col] = 0;
          if (col < clusterIdx*clusterSize && col >= (clusterIdx-1)*clusterSize)
            {
              wv[col] = 1;
            }

          wv[col] = wv[col] + 0.1;

          // output matrix values to disk
          fileInputMatrix << wv[col] << " ";
        }

      // output matrix values to disk (newline at end of row)
      fileInputMatrix << std::endl;

      weightList->PushBack(wv);
    }
  fileInputClusters.close();
  fileInputMatrix.close();



  // Now create the cluster-er and set its input
  // -----------------------
  itk::SpectralClustering::Pointer spectralCluster = itk::SpectralClustering::New();
  spectralCluster->SetInput(weightListWrapper);
  spectralCluster->SetNumberOfClusters(numberOfClusters);
  spectralCluster->SetNumberOfEigenvectors(numberOfEigenvectors);

  // For debug/test, this outputs the embedding vectors in a file (embed.txt)
  spectralCluster->SaveEmbeddingVectorsOn();
  spectralCluster->SaveEigenvectorsOn();

  spectralCluster->DebugOn();
  spectralCluster->SetEmbeddingNormalizationToNone();

  // CHANGE to do update method
  try 
    {
    spectralCluster->ComputeClusters();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cout << e;
    }
  catch (...)
    {
      std::cout << "Unknown exception in compute clusters" << std::endl;
    }

  // Now print/save the input and output
  // -----------------------
  // print cluster numbers
  itk::SpectralClustering::OutputClassifierType::OutputType * membershipSample =  
    spectralCluster->GetOutput()->Get()->GetOutput();

  if (membershipSample == NULL)
    {
      std::cerr << "Error: clusters have not been computed." << std::endl;
      return 1;      
    }
  // Iterate over all class labels and save the info
  itk::SpectralClustering::OutputClassifierType::OutputType::ConstIterator iter = 
    membershipSample->Begin();

  std::ofstream fileOutputClusters;
  fileOutputClusters.open("outputClusters.txt");

  int idx = 0;
  while ( iter != membershipSample->End() )
    {
      std::cout <<"index = " << idx << "   class label = " << iter.GetClassLabel() << std::endl;
      fileOutputClusters << iter.GetClassLabel() << std::endl;
      idx++;
      ++iter;
    }
  fileOutputClusters.close();

  return 0;
}
