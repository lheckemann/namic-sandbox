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

#include <cmath>

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
    //this->m_Threader->Delete();
    }
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

//---------------------------------------------------------------------------
void vtkBetaProbeLogic::StartTumorDetection(int threshold)
{
  if(this->GetUDPServerNode())
    {
    this->ThresholdDetection = threshold;
    this->DetectionRunning = true;
    this->ThreadID = this->m_Threader->SpawnThread(vtkBetaProbeLogic::TumorDetection,this);
    }
}

//---------------------------------------------------------------------------
void vtkBetaProbeLogic::StopTumorDetection()
{
  if(this->ThreadID >= 0)
    {
    this->DetectionRunning = false;
    this->m_Threader->TerminateThread(this->ThreadID);
    this->ThreadID = -1;
    }
}

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
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


    // Calculate BetaProbe Size (2mm radius, 0.5mm depth) in IJK Coordinates
    double BetaProbeSizeInmm[4] = {2.0, 2.0, 0.5, 0};
    double BetaProbeSizeInPixels[4];
    BetaProbeLogic->GetRASToIJKMatrix()->MultiplyPoint(BetaProbeSizeInmm, BetaProbeSizeInPixels);
    double BetaProbeHalfSizeInPixels[3] = {fabs(BetaProbeSizeInPixels[0])/2,
                                           fabs(BetaProbeSizeInPixels[1])/2,
                                           fabs(BetaProbeSizeInPixels[2])/2};

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
      double Counts = 0;
      switch(BetaProbeLogic->GetCountsType()){
      case 1:  Counts = UDPS->GetBetaCounts(); break;
      case 2:  Counts = UDPS->GetGammaCounts(); break;
      case 3:  Counts = UDPS->GetSmoothedCounts(); break;
      default: Counts = UDPS->GetSmoothedCounts(); break;
      }


      // Check probe position in IJK coordinate system is in the image
      // Check current value is higher than value already mapped (if any)
      // Map value
      if((PointIJK[0] > extent[0]+1) && (PointIJK[0] < extent[1]-1))
        {
        if((PointIJK[1] > extent[2]+1) && (PointIJK[1] < extent[3]-1))
          {
          if((PointIJK[2] > extent[4]+1) && (PointIJK[2] < extent[5]-1))
            {
            for(double i=PointIJK[0]-BetaProbeHalfSizeInPixels[0];i<PointIJK[0]+BetaProbeHalfSizeInPixels[0];i+=0.5)
              {
              for(double j=PointIJK[1]-BetaProbeHalfSizeInPixels[1];j<PointIJK[1]+BetaProbeHalfSizeInPixels[1];j+=0.5)
                {
                for(double k=PointIJK[2]-BetaProbeHalfSizeInPixels[2];k<PointIJK[2]+BetaProbeHalfSizeInPixels[2];k+=0.5)
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
