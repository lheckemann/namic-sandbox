/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

  ==========================================================================*/


#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkBetaProbeLogic.h"

#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkCollection.h"
#include "vtkVector.h"

vtkCxxRevisionMacro(vtkBetaProbeLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkBetaProbeLogic);

//---------------------------------------------------------------------------
vtkBetaProbeLogic::vtkBetaProbeLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkBetaProbeLogic::DataCallback);


  this->UDPServerNode      = NULL;
  this->CountsType         = -1;
  this->m_Threader         = itk::MultiThreader::New();
  this->ThreadID           = -1;
  this->DetectionRunning   = false;
  this->ThresholdDetection = -1;

  this->PositionTransform       = NULL;
  this->DataToMap               = NULL;
  this->ColorNode               = NULL;
  this->MappedVolume            = NULL;
  this->RASToIJKMatrix          = NULL;
  this->IJKToRASDirectionMatrix = NULL;
  this->MappingRunning          = false;
  this->MappingThreadID         = -1;
  this->TmpMatrix               = NULL;
}


//---------------------------------------------------------------------------
vtkBetaProbeLogic::~vtkBetaProbeLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  if(this->GetMappingRunning())
    {
    this->StopMapping();
    }

/*
  if(this->TmpMatrix)
  {
  this->TmpMatrix->Delete();
  }
*/
}


//---------------------------------------------------------------------------
void vtkBetaProbeLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkBetaProbeLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkBetaProbeLogic::DataCallback(vtkObject *caller,
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkBetaProbeLogic *self = reinterpret_cast<vtkBetaProbeLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkBetaProbeLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkBetaProbeLogic::UpdateAll()
{

}

void vtkBetaProbeLogic::PivotCalibration(vtkCollection* PivotingMatrix, double AveragePcal[3])
{
  int NElements = PivotingMatrix->GetNumberOfItems();
  vtkMatrix3x3* rotation1 = vtkMatrix3x3::New();
  vtkMatrix3x3* rotation2 = vtkMatrix3x3::New();
  double translation1[3];
  double translation2[3];

  double pcal[3] = {0,0,0};

  int ElementsUsed = 0;

  for(int CurrentElement = 0; CurrentElement < NElements-1; CurrentElement+=2)
    {
    vtkMatrix4x4 *matrix1 = vtkMatrix4x4::SafeDownCast(PivotingMatrix->GetItemAsObject(CurrentElement));
    vtkMatrix4x4 *matrix2 = vtkMatrix4x4::SafeDownCast(PivotingMatrix->GetItemAsObject(CurrentElement+1));

    vtkMatrix3x3* MatrixDifference = vtkMatrix3x3::New();
    double TranslationDifference[3] = {0,0,0};


    // Rotation matrix1
    rotation1->SetElement(0,0,matrix1->GetElement(0,0));
    rotation1->SetElement(0,1,matrix1->GetElement(0,1));
    rotation1->SetElement(0,2,matrix1->GetElement(0,2));

    rotation1->SetElement(1,0,matrix1->GetElement(1,0));
    rotation1->SetElement(1,1,matrix1->GetElement(1,1));
    rotation1->SetElement(1,2,matrix1->GetElement(1,2));

    rotation1->SetElement(2,0,matrix1->GetElement(2,0));
    rotation1->SetElement(2,1,matrix1->GetElement(2,1));
    rotation1->SetElement(2,2,matrix1->GetElement(2,2));

    // Rotation matrix2
    rotation2->SetElement(0,0,matrix2->GetElement(0,0));
    rotation2->SetElement(0,1,matrix2->GetElement(0,1));
    rotation2->SetElement(0,2,matrix2->GetElement(0,2));

    rotation2->SetElement(1,0,matrix2->GetElement(1,0));
    rotation2->SetElement(1,1,matrix2->GetElement(1,1));
    rotation2->SetElement(1,2,matrix2->GetElement(1,2));

    rotation2->SetElement(2,0,matrix2->GetElement(2,0));
    rotation2->SetElement(2,1,matrix2->GetElement(2,1));
    rotation2->SetElement(2,2,matrix2->GetElement(2,2));


    // Translation matrix1
    translation1[0] = matrix1->GetElement(0,3);
    translation1[1] = matrix1->GetElement(1,3);
    translation1[2] = matrix1->GetElement(2,3);

    // Translation matrix2
    translation2[0] = matrix2->GetElement(0,3);
    translation2[1] = matrix2->GetElement(1,3);
    translation2[2] = matrix2->GetElement(2,3);


    // Calculate Difference ( [R1-R2] )
    // Matrix

    MatrixDifference->SetElement(0,0,rotation1->GetElement(0,0) - rotation2->GetElement(0,0));
    MatrixDifference->SetElement(0,1,rotation1->GetElement(0,1) - rotation2->GetElement(0,1));
    MatrixDifference->SetElement(0,2,rotation1->GetElement(0,2) - rotation2->GetElement(0,2));

    MatrixDifference->SetElement(1,0,rotation1->GetElement(1,0) - rotation2->GetElement(1,0));
    MatrixDifference->SetElement(1,1,rotation1->GetElement(1,1) - rotation2->GetElement(1,1));
    MatrixDifference->SetElement(1,2,rotation1->GetElement(1,2) - rotation2->GetElement(1,2));

    MatrixDifference->SetElement(2,0,rotation1->GetElement(2,0) - rotation2->GetElement(2,0));
    MatrixDifference->SetElement(2,1,rotation1->GetElement(2,1) - rotation2->GetElement(2,1));
    MatrixDifference->SetElement(2,2,rotation1->GetElement(2,2) - rotation2->GetElement(2,2));


    std::cout
      << MatrixDifference->GetElement(0,0) << " " << MatrixDifference->GetElement(0,1) << " " << MatrixDifference->GetElement(0,2) << " "<< MatrixDifference->GetElement(0,3) << std::endl
      << MatrixDifference->GetElement(1,0) << " " << MatrixDifference->GetElement(1,1) << " " << MatrixDifference->GetElement(1,2) << " "<< MatrixDifference->GetElement(1,3) << std::endl
      << MatrixDifference->GetElement(2,0) << " " << MatrixDifference->GetElement(2,1) << " " << MatrixDifference->GetElement(2,2) << " "<< MatrixDifference->GetElement(2,3) << std::endl
      << MatrixDifference->GetElement(3,0) << " " << MatrixDifference->GetElement(3,1) << " " << MatrixDifference->GetElement(3,2) << " "<< MatrixDifference->GetElement(3,3) << std::endl
      << std::endl;




    // Translation ( [P1-P2] )
    TranslationDifference[0] = translation1[0] - translation2[0];
    TranslationDifference[1] = translation1[1] - translation2[1];
    TranslationDifference[2] = translation1[2] - translation2[2];


    // Inverse matrix ( [R1-R2]^-1 )
    vtkMatrix3x3* calculated_matrix = vtkMatrix3x3::New();
    MatrixDifference->Invert(MatrixDifference, calculated_matrix);

    // Multiply Rotation and Translation ( [R1-R2]^-1 * [P1-P2] )
    MatrixDifference->MultiplyPoint(TranslationDifference, pcal);

    // Change sign ( Pcal = - [R1-R2]^-1 * [P1-P2] ) and add to previous results to average
    pcal[0] += -pcal[0];
    pcal[1] += -pcal[1];
    pcal[2] += -pcal[2];

    std::cout << pcal[0] << "-" << pcal[1] << "-" << pcal[2] << std::endl;

    ElementsUsed += 2;

    std::cout << CurrentElement << "/" << ElementsUsed << std::endl;

    calculated_matrix->Delete();
    MatrixDifference->Delete();
    }

  // Average pcal
  AveragePcal[0] = pcal[0] / ElementsUsed;
  AveragePcal[1] = pcal[1] / ElementsUsed;
  AveragePcal[2] = pcal[2] / ElementsUsed;

  rotation1->Delete();
  rotation2->Delete();
}


void vtkBetaProbeLogic::StartTumorDetection(int threshold)
{
  if(this->GetUDPServerNode())
    {
    this->ThresholdDetection = threshold;
    this->DetectionRunning = true;
    this->ThreadID = this->m_Threader->SpawnThread(vtkBetaProbeLogic::TumorDetection,this);
    }
}

void vtkBetaProbeLogic::StopTumorDetection()
{
  if(this->ThreadID >= 0)
    {
    this->DetectionRunning = false;
    this->m_Threader->TerminateThread(this->ThreadID);
    this->ThreadID = -1;
    }
}

ITK_THREAD_RETURN_TYPE vtkBetaProbeLogic::TumorDetection(void* pInfoStruct)
{
  vtkBetaProbeLogic *BetaProbeLogic = (vtkBetaProbeLogic*) (((itk::MultiThreader::ThreadInfoStruct *)(pInfoStruct))->UserData);

  while(BetaProbeLogic->DetectionRunning)
    {
    if(BetaProbeLogic->GetUDPServerNode()->GetSmoothedCounts() >= BetaProbeLogic->ThresholdDetection)
      {
      // TODO: Modify interval: Higher count -> Smaller interval
      BetaProbeLogic->BeepFunction(500);
      }
    }

  return EXIT_SUCCESS;
}

void vtkBetaProbeLogic::BeepFunction(int interval_ms)
{
#ifdef _WIN32
  Beep(500,250);
  Sleep(interval_ms);
#elif defined(__FreeBSD__) || defined(__linux__)
  std::cout << "\a" << std::endl;
  usleep(interval_ms*1000);
#endif

}

void vtkBetaProbeLogic::StartMapping()
{
  // Start Thread
  if(this->GetUDPServerNode() &&
     this->GetPositionTransform() &&
     this->GetDataToMap() &&
     this->GetColorNode() &&
     this->GetMappedVolume() &&
     this->GetRASToIJKMatrix())
    {
    this->SetMappingRunning(true);
    this->MappingThreadID = this->m_Threader->SpawnThread(vtkBetaProbeLogic::MappingFunction, this);
    }
}

void vtkBetaProbeLogic::StopMapping()
{
  if(this->MappingThreadID >= 0)
    {
    this->SetMappingRunning(false);
    this->m_Threader->TerminateThread(this->MappingThreadID);
    this->CountsType = -1;
    this->MappingThreadID = -1;
    this->SetPositionTransform(NULL);
    this->SetDataToMap(NULL);
    this->SetColorNode(NULL);
    this->SetMappedVolume(NULL);
    this->SetRASToIJKMatrix(NULL);
    this->SetIJKToRASDirectionMatrix(NULL);
    if(!this->DetectionRunning)
      {
      this->SetUDPServerNode(NULL);
      }
    }
}

ITK_THREAD_RETURN_TYPE vtkBetaProbeLogic::MappingFunction(void* pInfoStruct)
{
  // Get context
  vtkBetaProbeLogic *BetaProbeLogic = (vtkBetaProbeLogic*) (((itk::MultiThreader::ThreadInfoStruct *)(pInfoStruct))->UserData);

  // Get nodes before loop to increase speed of loop (mapping)
  vtkImageData* imageData = BetaProbeLogic->GetMappedVolume()->GetImageData();
  vtkMRMLUDPServerNode* UDPS = BetaProbeLogic->GetUDPServerNode();
  vtkMatrix4x4* MatrixToWorld = vtkMatrix4x4::New();

  // Check everything is ready
  if(BetaProbeLogic->GetRASToIJKMatrix() &&
     BetaProbeLogic->GetIJKToRASDirectionMatrix() &&
     imageData &&
     UDPS &&
     MatrixToWorld)
    {
    int *extent = imageData->GetExtent();

    // Mapping Loop
    while(BetaProbeLogic->GetMappingRunning())
      {
      // Get probe position (with registration if any)
      MatrixToWorld->Identity();
      BetaProbeLogic->GetPositionTransform()->GetMatrixTransformToWorld(MatrixToWorld);

      // Convert from image directions (LPS, RAS, etc... to IJK)
      double PointDirection[4] = {MatrixToWorld->GetElement(0,3),
                                  MatrixToWorld->GetElement(1,3),
                                  MatrixToWorld->GetElement(2,3),
                                  1};

      double PointRAS[4];
      BetaProbeLogic->GetIJKToRASDirectionMatrix()->MultiplyPoint(PointDirection, PointRAS);

      double PointIJK[4];
      BetaProbeLogic->GetRASToIJKMatrix()->MultiplyPoint(PointRAS, PointIJK);

      // Get Smoothed counts from UDPServerNode (or Gamma for small amout of activity)
      //double SmoothedCounts = UDPS->GetSmoothedCounts();
      double Counts = 0;
      switch(BetaProbeLogic->GetCountsType()){
      case 1:  Counts = UDPS->GetBetaCounts(); break;
      case 2:  Counts = UDPS->GetGammaCounts(); break;
      case 3:  Counts = UDPS->GetSmoothedCounts(); break;
      default: Counts = UDPS->GetSmoothedCounts(); break;
      }


      // Check probe position in IJK coordinate system is in the image
      // Then map 5mm around position (Betaprobe thickness) (needed for depth ?)
      // Check current value is higher than value already mapped (if any)
      // Map value
      if((PointIJK[0] > extent[0]+1) && (PointIJK[0] < extent[1]-1))
        {
        if((PointIJK[1] > extent[2]+1) && (PointIJK[1] < extent[3]-1))
          {
          if((PointIJK[2] > extent[4]+1) && (PointIJK[2] < extent[5]-1))
            {
            for(double i=PointIJK[0]-2.5;i<PointIJK[0]+2.5;i+=0.5)
              {
              for(double j=PointIJK[1]-2.5;j<PointIJK[1]+2.5;j+=0.5)
                {
                for(double k=PointIJK[2]-2.5;k<PointIJK[2]+2.5;k+=0.5)
                  {
                  if(Counts >= imageData->GetScalarComponentAsDouble(i,j,k,0))
                    {
                    imageData->SetScalarComponentFromDouble(i,
                                                            j,
                                                            k,
                                                            0,
                                                            Counts);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }

  MatrixToWorld->Delete();
  return EXIT_SUCCESS;
}
