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

#include "vtkCaptureBetaProbeLogic.h"

#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkCollection.h"
#include "vtkVector.h"

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlClientSocket.h"


vtkCxxRevisionMacro(vtkCaptureBetaProbeLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkCaptureBetaProbeLogic);

//---------------------------------------------------------------------------
vtkCaptureBetaProbeLogic::vtkCaptureBetaProbeLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkCaptureBetaProbeLogic::DataCallback);

  this->Offset = NULL;
}


//---------------------------------------------------------------------------
vtkCaptureBetaProbeLogic::~vtkCaptureBetaProbeLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  if (this->Offset)
    {
    this->Offset->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkCaptureBetaProbeLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkCaptureBetaProbeLogic *self = reinterpret_cast<vtkCaptureBetaProbeLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkCaptureBetaProbeLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkCaptureBetaProbeLogic::UpdateAll()
{

}

void vtkCaptureBetaProbeLogic::PivotCalibration(vtkCollection* PivotingMatrix, double AveragePcal[3])
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
 << "Before Inverse:" << std::endl 
 << MatrixDifference->GetElement(0,0) << "\t" << MatrixDifference->GetElement(0,1) << "\t" << MatrixDifference->GetElement(0,2) << std::endl
 << MatrixDifference->GetElement(1,0) << "\t" << MatrixDifference->GetElement(1,1) << "\t" << MatrixDifference->GetElement(1,2) << std::endl
 << MatrixDifference->GetElement(2,0) << "\t" << MatrixDifference->GetElement(2,1) << "\t" << MatrixDifference->GetElement(2,2) << std::endl
 << std::endl;
      



      // Translation ( [P1-P2] )
      TranslationDifference[0] = translation1[0] - translation2[0];
      TranslationDifference[1] = translation1[1] - translation2[1];
      TranslationDifference[2] = translation1[2] - translation2[2];


      // Inverse matrix ( [R1-R2]^-1 )
      vtkMatrix3x3* calculated_matrix = vtkMatrix3x3::New();
      for(int i = 0; i < 3; i++)
     {
        for(int j = 0; j < 3; j++)
       {
         calculated_matrix->SetElement(i,j,0);
       }
     }
      MatrixDifference->Invert(MatrixDifference, calculated_matrix);

            
      std::cout 
 << "After Inverse: " << std::endl
 << calculated_matrix->GetElement(0,0) << "\t" << calculated_matrix->GetElement(0,1) << "\t" << calculated_matrix->GetElement(0,2) << std::endl
 << calculated_matrix->GetElement(1,0) << "\t" << calculated_matrix->GetElement(1,1) << "\t" << calculated_matrix->GetElement(1,2) << std::endl
 << calculated_matrix->GetElement(2,0) << "\t" << calculated_matrix->GetElement(2,1) << "\t" << calculated_matrix->GetElement(2,2) << std::endl
 << std::endl;
      
      

      // Multiply Rotation and Translation ( [R1-R2]^-1 * [P1-P2] )
      calculated_matrix->MultiplyPoint(TranslationDifference, pcal);
    
      // Change sign ( Pcal = - [R1-R2]^-1 * [P1-P2] ) and add to previous results to average
      AveragePcal[0] += -pcal[0];
      AveragePcal[1] += -pcal[1];
      AveragePcal[2] += -pcal[2];

      ElementsUsed += 2;

      // std::cout << CurrentElement << "/" << ElementsUsed << std::endl;

      calculated_matrix->Delete();
      MatrixDifference->Delete();
    }

  // Average pcal
  AveragePcal[0] = AveragePcal[0] / ElementsUsed;
  AveragePcal[1] = AveragePcal[1] / ElementsUsed;
  AveragePcal[2] = AveragePcal[2] / ElementsUsed;

  // std::cout << "AvPcal: " << AveragePcal[0] << ":" << AveragePcal[1] << ":" << AveragePcal[2] << std::endl;

  /* 
  out->SetElement(0,3,pcal[0]);
  out->SetElement(1,3,pcal[1]);
  out->SetElement(2,3,pcal
  */

  rotation1->Delete();
  rotation2->Delete();
}

void vtkCaptureBetaProbeLogic::ProbeToProbeRegistration(vtkMatrix4x4* Tracker, vtkMatrix4x4* BetaProbe, vtkMatrix4x4* ProbeOffset)
{
  ProbeOffset->SetElement(0,3,Tracker->GetElement(0,3)-BetaProbe->GetElement(0,3));
  ProbeOffset->SetElement(1,3,Tracker->GetElement(1,3)-BetaProbe->GetElement(1,3));
  ProbeOffset->SetElement(2,3,Tracker->GetElement(2,3)-BetaProbe->GetElement(2,3));
}


void vtkCaptureBetaProbeLogic::ManualTipToTipCalibration(vtkMatrix4x4* Tracker, vtkMatrix4x4* BetaProbe, vtkMatrix4x4* ProbeOffset)
{
  this->Offset = vtkMatrix4x4::New();
  this->Offset->Zero();   

  this->Offset->SetElement(0,3,Tracker->GetElement(0,3)-BetaProbe->GetElement(0,3)-ProbeOffset->GetElement(0,3));
  this->Offset->SetElement(1,3,Tracker->GetElement(1,3)-BetaProbe->GetElement(1,3)-ProbeOffset->GetElement(1,3));
  this->Offset->SetElement(2,3,Tracker->GetElement(2,3)-BetaProbe->GetElement(2,3)-ProbeOffset->GetElement(2,3));

  std::cout << "Tracker: (" << Tracker->GetElement(0,3) << "," << Tracker->GetElement(1,3) << "," << Tracker->GetElement(2,3) << ")" << std::endl;

  std::cout << "Tracker: (" << BetaProbe->GetElement(0,3) << "," << BetaProbe->GetElement(1,3) << "," << BetaProbe->GetElement(2,3) << ")" << std::endl;

  std::cout << "Offset x: " << this->Offset->GetElement(0,3) << std::endl;
  std::cout << "Offset y: " << this->Offset->GetElement(1,3) << std::endl;
  std::cout << "Offset z: " << this->Offset->GetElement(2,3) << std::endl;

  
}

