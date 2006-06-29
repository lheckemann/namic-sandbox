/*=auto=========================================================================

  Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkIsingMeanfieldApproximation.cxx,v $
  Date:      $Date: 2006/06/01 21:54:02 $
  Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "itkIsingMeanfieldApproximationImageFilter.h"
#include "vtkCommand.h"

vtkStandardNewMacro(vtkIsingMeanfieldApproximation);

vtkIsingMeanfieldApproximation::vtkIsingMeanfieldApproximation()
{
  this->nonactive = 0;
  this->posactive = 301;
  this->negactive = 300;
  this->logTransitionMatrix = vtkFloatArray::New();
}

vtkIsingMeanfieldApproximation::~vtkIsingMeanfieldApproximation()
{
  this->logTransitionMatrix->Delete();
}

void vtkIsingMeanfieldApproximation::SimpleExecute(vtkImageData *input, vtkImageData *output)
{

  dims[0] = x;   // X is the size[0] of the image
  dims[1] = y;   // Y is the size[1] of the image
  dims[2] = z;   // Z is the size[2] of the image

  size = x*y*z;
  int numberOfInputs;

#if (VTK_MAJOR_VERSION >= 5)
  numberOfInputs = this->GetNumberOfInputConnections(0);
#else  
  numberOfInputs = this->NumberOfInputs;
#endif
  
  // in case of anatomical label map input
  if (numberOfInputs == 3)
    {
    m_SegmentationLabelMap = SegmentationLabelsImageType::New();
    m_SegmentationLabelMap->FillBuffer(0);

    register int i, j, k;     
    for (k=0; k<z; k++)
      {
      for (j=0; j<y; j++)
        {
        for (i=0; i<x; i++)
          {
          labelValue = (short int *) (GetInput(2)->GetScalarPointer(i,j,k));
          m_SegmentationLabelMap->SetValue((k*x*y)+(j*x)+i,(int)(*labelValue));
          }
        }
      }


    /** REPLACE THIS CODE WITH THE ITK RELABEL IMAGE FILTER (AND ALSO TAKE FROM
       * IT THE ACTUAL NUMBER OF LABELS */
                        for (int j=0; j<segInput; j++)
                          {
                          for (unsigned long int i=0; i<size; i++)
                            {
                            if (m_SegmentationLabelMap->GetValue(i) == segLabel->GetValue(j))
                              {
                              m_SegmentationLabelMap->SetValue(i,j);
                              }
                            }
                          }
    /** END OF CODE TO REPLACE WITH THE RELABEL IMAGE FILTER */

    }
  else 
    {
    m_SegmentationLabelMap = vtkIntArray::New();
    m_SegmentationLabelMap->FillBuffer(0);
    }
  
  vtkIntArray *classArray = (vtkIntArray *)this->GetInput(0)->GetPointData()->GetScalars();
        
  
  numberOfPosteriorClasses = NumberOfSegmetationLabels * m_NumberOfActivationStates;

  sum = 0;
  for (int i=0; i<(numberOfPosteriorClasses*numberOfPosteriorClasses); i++)
    sum += (transitionMatrix->GetValue(i));
        
  if (sum == 0){      
    // construction of a matrix indicating the transition strength between classes      
    register int i, j, k;
    for (i=0; i<x; i++)
      for (j=0; j<y; j++)
        for (k=0; k<z; k++){
          if (i != 0){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+(j*x)+i-1);
            transitionMatrix->SetValue((index1*numberOfPosteriorClasses)+index2, (transitionMatrix->GetValue((index1*numberOfPosteriorClasses)+index2)+1));
          }
          if (i != x-1){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+(j*x)+i+1);
            transitionMatrix->SetValue((index1*numberOfPosteriorClasses)+index2, (transitionMatrix->GetValue((index1*numberOfPosteriorClasses)+index2)+1));
          }
          if (j != 0){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+((j-1)*x)+i);
            transitionMatrix->SetValue((index1*numberOfPosteriorClasses)+index2, (transitionMatrix->GetValue((index1*numberOfPosteriorClasses)+index2)+1));
          }
          if (j != y-1){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue((k*x*y)+((j+1)*x)+i);
            transitionMatrix->SetValue((index1*numberOfPosteriorClasses)+index2, (transitionMatrix->GetValue((index1*numberOfPosteriorClasses)+index2)+1));       
          }
          if (k != 0){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue(((k-1)*x*y)+(j*x)+i);
            transitionMatrix->SetValue((index1*numberOfPosteriorClasses)+index2, (transitionMatrix->GetValue((index1*numberOfPosteriorClasses)+index2)+1));
          }
         if (k != z-1){
            index1 = classArray->GetValue((k*x*y)+(j*x)+i);
            index2 = classArray->GetValue(((k+1)*x*y)+(j*x)+i);
            transitionMatrix->SetValue((index1*numberOfPosteriorClasses)+index2, (transitionMatrix->GetValue((index1*numberOfPosteriorClasses)+index2)+1));
          }
        }
        
    // neighborhoods were counted double
    for (int i=0; i<numberOfPosteriorClasses; i++)
      if (transitionMatrix->GetValue((i*numberOfPosteriorClasses)+i) != 0)
        transitionMatrix->SetValue((i*numberOfPosteriorClasses)+i, (transitionMatrix->GetValue((i*numberOfPosteriorClasses)+i))/2);
  }  
  
  // in case of existing 0 values in transition matrix, increase all by 1 to prevent log range error   
  for (int i=0; i<(numberOfPosteriorClasses*numberOfPosteriorClasses); i++)
    if (transitionMatrix->GetValue(i) == 0){
      for (int j=0; j<(numberOfPosteriorClasses*numberOfPosteriorClasses); j++)             
        transitionMatrix->SetValue(j, (transitionMatrix->GetValue(j))+1);     
      break;
    }
  
  // construction of log transition matrix
  logTransitionMatrix->SetNumberOfValues(numberOfPosteriorClasses*numberOfPosteriorClasses);
  for (int i=0; i<numberOfPosteriorClasses; i++)
    for (int j=0; j<numberOfPosteriorClasses; j++){
      logHelp = (float) log((transitionMatrix->GetValue((i*numberOfPosteriorClasses)+j))/sqrt(((activationFrequence->GetValue(i))*size)*((activationFrequence->GetValue(j))*size)));                  
      logTransitionMatrix->SetValue((i*numberOfPosteriorClasses)+j, logHelp);
    }
  
  vtkFloatArray *probGivenClassArray = (vtkFloatArray *)this->GetInput(1)->GetPointData()->GetScalars();
  vtkFloatArray *outputArray = vtkFloatArray::New();
  vtkIntArray *finalOutput = vtkIntArray::New();
  
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetSpacing(1.0,1.0,1.0);
  output->SetOrigin(0.0,0.0,0.0);
  output->AllocateScalars();

  helpArray = new float[numberOfPosteriorClasses];

  // initialization of class probability output volume
  for (int n=0; n<numberOfPosteriorClasses; n++)
    for (unsigned long int i=0; i<size; i++)
      outputArray->InsertNextValue((1.0/numberOfPosteriorClasses));
  
  // meanfield iteration     
  register int i, j, k, n;     
  for (n=0; n<iterations; n++){
    for (k=0; k<z; k++)
      for (j=0; j<y; j++)
        for (i=0; i<x; i++){
          sumHelpArray = 0.0;
          for (int l=0; l<numberOfPosteriorClasses; l++){
            eValue = 0.0;          
            if (i != 0){
              for (int s=0; s<numberOfPosteriorClasses; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+(j*x)+i-1))*(logTransitionMatrix->GetValue((l*numberOfPosteriorClasses)+s))); 
            }    
            if (i != x-1){
              for (int s=0; s<numberOfPosteriorClasses; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+(j*x)+i+1))*(logTransitionMatrix->GetValue((l*numberOfPosteriorClasses)+s)));                             
            }           
            if (j != 0){
              for (int s=0; s<numberOfPosteriorClasses; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+((j-1)*x)+i))*(logTransitionMatrix->GetValue((l*numberOfPosteriorClasses)+s))); 
            }           
            if (j != y-1){
              for (int s=0; s<numberOfPosteriorClasses; s++)
                eValue += ((outputArray->GetValue((s*size)+(k*x*y)+((j+1)*x)+i))*(logTransitionMatrix->GetValue((l*numberOfPosteriorClasses)+s)));
            }        
            if (k != 0){
              for (int s=0; s<numberOfPosteriorClasses; s++)
                eValue += ((outputArray->GetValue((s*size)+((k-1)*x*y)+(j*x)+i))*(logTransitionMatrix->GetValue((l*numberOfPosteriorClasses)+s))); 
            }    
            if (k != z-1){
              for (int s=0; s<numberOfPosteriorClasses; s++)
                eValue += ((outputArray->GetValue((s*size)+((k+1)*x*y)+(j*x)+i))*(logTransitionMatrix->GetValue((l*numberOfPosteriorClasses)+s))); 
            }        
                        
// THIS REQUIRES THE SEGMENTATION ARRAY m_SegmentationLabelMap to be contiguous labeles
// {0,1,2,....N} so we can use it as index in probGivenSegM.

            helpArray[l] =   activationFrequence->GetValue(l) 
                           * m_PriorClassProbabilityGivenSegmentation(
                                     m_SegmentationLabelMap->GetPixel(k,j,i), l )
                           * probGivenClassArray->GetValue((l*size)+(k*x*y)+(j*x)+i)
                           * exp(eValue);

            sumHelpArray += helpArray[l];
          }
          for (int l=0; l<numberOfPosteriorClasses; l++){
            outputArray->SetValue((l*size)+(k*x*y)+(j*x)+i, helpArray[l]/sumHelpArray);
          }
        }   
    UpdateProgress(n * (1.0/iterations));   
  }
  
  // creation of activation label map
  for (unsigned long int i=0; i<size; i++){
    max = 0.0;
    posMax = 0;
    for (int n=0; n<numberOfPosteriorClasses; n++){
      if ((outputArray->GetValue((n*size)+i)) > max){
        max = (outputArray->GetValue((n*size)+i)); 
        posMax = n;
      }
    }   
    if (numActivationStates == 2){
      if (posMax < (numberOfPosteriorClasses/2))
        finalOutput->InsertNextValue(nonactive);
      else
        finalOutput->InsertNextValue(posactive);
    }
    else{
      if (posMax < (numberOfPosteriorClasses/3))
        finalOutput->InsertNextValue(nonactive);
      else
        if ((posMax >= (numberOfPosteriorClasses/3)) && (posMax < 2*(numberOfPosteriorClasses/3)))
          finalOutput->InsertNextValue(posactive);
        else
          finalOutput->InsertNextValue(negactive);
    }
  }     

  output->GetPointData()->SetScalars(finalOutput);
  
#if (VTK_MAJOR_VERSION >= 5)
  numberOfInputs = this->GetNumberOfInputConnections(0);
#else  
  numberOfInputs = this->NumberOfInputs;
#endif
  if (numberOfInputs != 3)
  {
    m_SegmentationLabelMap->Delete();
  }
  delete [] helpArray;
  outputArray->Delete(); 
  finalOutput->Delete();     
}



// If the output image of a filter has different properties from the input image
// we need to explicitly define the ExecuteInformation() method
void vtkIsingMeanfieldApproximation::ExecuteInformation(vtkImageData *input, vtkImageData *output)
{
  output->SetDimensions(dims);
  output->SetScalarType(VTK_INT);
  output->SetSpacing(1.0,1.0,1.0);
  output->SetOrigin(0.0,0.0,0.0);
  output->AllocateScalars();
}
