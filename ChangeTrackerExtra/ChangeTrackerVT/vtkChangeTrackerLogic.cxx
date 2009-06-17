#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkChangeTrackerLogic.h"
#include "vtkChangeTracker.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLChangeTrackerNode.h"
#include "vtkImageClip.h"
#include "vtkImageChangeInformation.h"
#include "vtkImageResample.h"
#include "vtkSlicerApplication.h"
#include "vtkImageMathematics.h"
#include "vtkImageSumOverVoxels.h"
#include "vtkImageIslandFilter.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerVolumesGUI.h"
#include "vtkKWProgressGauge.h"
#include "vtkImageMedian3D.h"
#include "vtkImageAccumulate.h"
//#include "vtkSlicerApplication.h"

// CommandLineModule support
#include "vtkMRMLCommandLineModuleNode.h"
#include "vtkCommandLineModuleGUI.h"
#include "vtkCommandLineModuleLogic.h"
#include "ModuleDescription.h"
#include "itksys/DynamicLoader.hxx"
#include <assert.h>
#include "vtkMRMLLinearTransformNode.h"
#include "vtkNRRDWriter.h"
#define ERROR_NODE_VTKID 0

//----------------------------------------------------------------------------
vtkChangeTrackerLogic* vtkChangeTrackerLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = 
    vtkObjectFactory::CreateInstance("vtkChangeTrackerLogic");
  if(ret)
    {
    return (vtkChangeTrackerLogic*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkChangeTrackerLogic;
}


//----------------------------------------------------------------------------
vtkChangeTrackerLogic::vtkChangeTrackerLogic()
{
  this->ModuleName = NULL;

  this->ProgressCurrentAction = NULL;
  this->ProgressGlobalFractionCompleted = 0.0;
  this->ProgressCurrentFractionCompleted = 0.0;

  //this->DebugOn();
  this->ChangeTrackerNode = NULL; 
  // this->LocalTransform = NULL; 
  // this->GlobalTransform = NULL; 

  this->Analysis_Intensity_Mean      = 0.0;
  this->Analysis_Intensity_Variance  = 0.0;
  this->Analysis_Intensity_Threshold = 0.0;

  this->Analysis_Intensity_Scan1ByLower = NULL; 
  this->Analysis_Intensity_Scan1Range   = NULL; 
  this->Analysis_Intensity_Scan2ByLower = NULL; 
  this->Analysis_Intensity_Scan2Range   = NULL; 
  this->Analysis_Intensity_ScanSubtract = NULL; 
  this->Analysis_Intensity_ScanSubtractSmooth = NULL; 
  this->Analysis_Intensity_ROIGrowth      = NULL; 
  this->Analysis_Intensity_ROIShrink      = NULL; 
  this->Analysis_Intensity_ROIGrowthInt   = NULL;
  this->Analysis_Intensity_ROIShrinkInt   = NULL;
  this->Analysis_Intensity_ROINegativeBin = NULL;
  this->Analysis_Intensity_ROIPositiveBin = NULL;
  this->Analysis_Intensity_ROIBinCombine  = NULL;
  this->Analysis_Intensity_ROIPositiveBinReal = NULL;
  this->Analysis_Intensity_ROINegativeBinReal = NULL;
  this->Analysis_Intensity_ROIBinAdd      = NULL;
  this->Analysis_Intensity_ROIBinDisplay  = NULL;
  this->Analysis_Intensity_ROIGrowthVolume = NULL;
  this->Analysis_Intensity_ROIShrinkVolume = NULL;

  // if set to zero then SaveVolume will not do anything 
  this->SaveVolumeFlag = 0;  

  this->Scan2_RegisteredVolume = NULL;
  // AF: if you don't set initial pointers to NULL, and then try to add them
  // to the scene, ObserverManager will try to unregister and segfault!
  this->Scan2_SuperSampleRegisteredVolume = NULL;
}


//----------------------------------------------------------------------------
vtkChangeTrackerLogic::~vtkChangeTrackerLogic()
{
  vtkSetMRMLNodeMacro(this->ChangeTrackerNode, NULL);
  this->SetProgressCurrentAction(NULL);
  this->SetModuleName(NULL);

  // if (this->LocalTransform) {
  //   this->LocalTransform->Delete();
  //   this->LocalTransform = NULL;
  // }
  // 
  // if (this->GlobalTransform) {
  //   this->GlobalTransform->Delete();
  //   this->GlobalTransform = NULL;
  // }
  

  if (this->Analysis_Intensity_Scan1ByLower) {
    this->Analysis_Intensity_Scan1ByLower->Delete();
    this->Analysis_Intensity_Scan1ByLower = NULL;
  } 

  if (this->Analysis_Intensity_Scan1Range) {
    this->Analysis_Intensity_Scan1Range->Delete();
    this->Analysis_Intensity_Scan1Range = NULL;
  } 

  if (this->Analysis_Intensity_Scan2ByLower) {
    this->Analysis_Intensity_Scan2ByLower->Delete();
    this->Analysis_Intensity_Scan2ByLower = NULL;
  } 
  
  if (this->Analysis_Intensity_Scan2Range) {
    this->Analysis_Intensity_Scan2Range->Delete();
    this->Analysis_Intensity_Scan2Range = NULL;
  } 

  if (this->Analysis_Intensity_ScanSubtract) {
    this->Analysis_Intensity_ScanSubtract->Delete();
    this->Analysis_Intensity_ScanSubtract = NULL;
  }
  
  if (this->Analysis_Intensity_ScanSubtractSmooth) { 
    this->Analysis_Intensity_ScanSubtractSmooth->Delete();
    this->Analysis_Intensity_ScanSubtractSmooth= NULL;
  }

  if (this->Analysis_Intensity_ROIGrowth) {
    this->Analysis_Intensity_ROIGrowth->Delete();
    this->Analysis_Intensity_ROIGrowth= NULL;

  } 
  if (this->Analysis_Intensity_ROIShrink) {
    this->Analysis_Intensity_ROIShrink->Delete();
    this->Analysis_Intensity_ROIShrink= NULL;
  } 

  if (this->Analysis_Intensity_ROIGrowthInt) {
    this->Analysis_Intensity_ROIGrowthInt->Delete();
    this->Analysis_Intensity_ROIGrowthInt= NULL;
  }

  if (this->Analysis_Intensity_ROIShrinkInt) {
    this->Analysis_Intensity_ROIShrinkInt->Delete();
    this->Analysis_Intensity_ROIShrinkInt= NULL;
  }

  if (this->Analysis_Intensity_ROINegativeBin) {
    this->Analysis_Intensity_ROINegativeBin->Delete();
    this->Analysis_Intensity_ROINegativeBin = NULL;
  }

  if (this->Analysis_Intensity_ROIPositiveBin) {
    this->Analysis_Intensity_ROIPositiveBin->Delete();
    this->Analysis_Intensity_ROIPositiveBin = NULL;
  }

  if (this->Analysis_Intensity_ROIBinCombine) {
    this->Analysis_Intensity_ROIBinCombine->Delete();
    this->Analysis_Intensity_ROIBinCombine = NULL;
  }

  if (this->Analysis_Intensity_ROIPositiveBinReal) {
    this->Analysis_Intensity_ROIPositiveBinReal->Delete();
    this->Analysis_Intensity_ROIPositiveBinReal = NULL;
  }

  if (this->Analysis_Intensity_ROINegativeBinReal) {
    this->Analysis_Intensity_ROINegativeBinReal->Delete();
    this->Analysis_Intensity_ROINegativeBinReal = NULL;
  }

  if (this->Analysis_Intensity_ROIBinAdd) {
    this->Analysis_Intensity_ROIBinAdd->Delete();
    this->Analysis_Intensity_ROIBinAdd = NULL;
  }

  if (this->Analysis_Intensity_ROIBinDisplay) {
    this->Analysis_Intensity_ROIBinDisplay->Delete();
    this->Analysis_Intensity_ROIBinDisplay = NULL;
  }

  if (this->Analysis_Intensity_ROIShrinkVolume) {
    this->Analysis_Intensity_ROIShrinkVolume->Delete();
    this->Analysis_Intensity_ROIShrinkVolume = NULL;
  }

  if (this->Analysis_Intensity_ROIGrowthVolume) {
    this->Analysis_Intensity_ROIGrowthVolume->Delete();
    this->Analysis_Intensity_ROIGrowthVolume = NULL;
  }


}

//----------------------------------------------------------------------------
void vtkChangeTrackerLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  // !!! todo
}

// For AG
// vtkGeneralTransform* vtkChangeTrackerLogic::CreateGlobalTransform() 
// vtkTransform* vtkChangeTrackerLogic::CreateGlobalTransform() 
// {
//   this->GlobalTransform = vtkTransform::New();
//   return this->GlobalTransform;
// }
// 
// // For AG
// // vtkGeneralTransform* vtkChangeTrackerLogic::CreateLocalTransform() 
// vtkTransform* vtkChangeTrackerLogic::CreateLocalTransform() 
// {
//   this->LocalTransform = vtkTransform::New();
//   return this->LocalTransform;
// }

int vtkChangeTrackerLogic::CheckROI(vtkMRMLVolumeNode* volumeNode) {
  if (!volumeNode || !this->ChangeTrackerNode || !volumeNode->GetImageData()) {
    cout << "vtkChangeTrackerLogic::CheckROI: No Volume Defined" << endl;
    return 0;
  }

  int* dimensions = volumeNode->GetImageData()->GetDimensions();

  for (int i = 0 ; i < 3 ; i++) {
    if ((this->ChangeTrackerNode->GetROIMax(i) < 0) || (this->ChangeTrackerNode->GetROIMax(i) >= dimensions[i])) {
      // cout << "vtkChangeTrackerLogic::CheckROI: dimension["<<i<<"] = " << dimensions[i] << endl;
      return 0 ;
    }
    if ((this->ChangeTrackerNode->GetROIMin(i) < 0) || (this->ChangeTrackerNode->GetROIMin(i) >= dimensions[i])) return 0 ;
    if (this->ChangeTrackerNode->GetROIMax(i) < this->ChangeTrackerNode->GetROIMin(i)) return 0;
  }
  return 1;
}


// Give another Volume as an example - creates a volume without anything in it 
// copied from vtkMRMLScalarVolumeNode* vtkSlicerVolumesLogic::CloneVolume without deep copy
vtkMRMLScalarVolumeNode* vtkChangeTrackerLogic::CreateVolumeNode(vtkMRMLVolumeNode *volumeNode, const char *name) {
  if (!this->ChangeTrackerNode || !volumeNode ) 
    {
    return NULL;
    }

  // clone the display node
  vtkMRMLScalarVolumeDisplayNode *clonedDisplayNode = vtkMRMLScalarVolumeDisplayNode::New();
  clonedDisplayNode->CopyWithScene(volumeNode->GetDisplayNode());
  this->ChangeTrackerNode->GetScene()->AddNode(clonedDisplayNode);

  // clone the volume node
  vtkMRMLScalarVolumeNode *clonedVolumeNode = vtkMRMLScalarVolumeNode::New();
  clonedVolumeNode->CopyWithScene(volumeNode);
  clonedVolumeNode->SetAndObserveImageData(NULL);
  clonedVolumeNode->SetAndObserveStorageNodeID(NULL);
  clonedVolumeNode->SetName(name);
  clonedVolumeNode->SetAndObserveDisplayNodeID(clonedDisplayNode->GetID());
  clonedVolumeNode->SetAndObserveImageData(NULL);

  if (0) {
    // copy over the volume's data
    vtkImageData* clonedVolumeData = vtkImageData::New(); 
    clonedVolumeData->DeepCopy(volumeNode->GetImageData());
    clonedVolumeNode->SetAndObserveImageData( clonedVolumeData );
    clonedVolumeNode->SetModifiedSinceRead(1);
    clonedVolumeData->Delete();
  }

  // add the cloned volume to the scene
  this->ChangeTrackerNode->GetScene()->AddNode(clonedVolumeNode);

  // remove references
  clonedVolumeNode->Delete();
  clonedDisplayNode->Delete();

  return (clonedVolumeNode);
 
}

void vtkChangeTrackerLogic::DeleteSuperSample(int ScanNum) {
  // cout <<  "vtkChangeTrackerLogic::DeleteSuperSample " << endl;
   // Delete old attached node first 
  vtkMRMLVolumeNode* currentNode = NULL; 
  switch(ScanNum){
  case 1:
    currentNode =  vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_SuperSampleRef()));
    this->ChangeTrackerNode->SetScan1_SuperSampleRef(NULL);
    break;
  case 2:
    currentNode =  vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan2_SuperSampleRef()));
    this->ChangeTrackerNode->SetScan2_SuperSampleRef(NULL);
    break;
  case 0:
    currentNode =  vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_SuperSampleInputSegmRef()));
    this->ChangeTrackerNode->SetScan1_SuperSampleInputSegmRef(NULL);
    break;
  default:
    return;
  }
  if (currentNode) { 
    this->ChangeTrackerNode->GetScene()->RemoveNode(currentNode); 
  } 
}

double vtkChangeTrackerLogic::DefineSuperSampleSize(const double inputSpacing[3], const int ROIMin[3], const int ROIMax[3]) {
    int size = ROIMax[0] - ROIMin[0] + 1;
    double TempSpacing = double(size) * inputSpacing[0] / 100.0;
    double SuperSampleSpacing = (TempSpacing < 0.3 ?  0.3 : TempSpacing);
    
    size = ROIMax[1] - ROIMin[1] + 1;
    TempSpacing = double(size) * inputSpacing[1] / 100.0;
    if (TempSpacing > SuperSampleSpacing) { SuperSampleSpacing = TempSpacing;}

    size = ROIMax[2] - ROIMin[2] + 1;
    TempSpacing = double(size) * inputSpacing[2] / 100.0;
    if (TempSpacing > SuperSampleSpacing) { SuperSampleSpacing = TempSpacing;}
    
    return SuperSampleSpacing;
}

int vtkChangeTrackerLogic::CreateSuperSampleFct(vtkImageData *input, const int ROIMin[3], const int ROIMax[3], const double SuperSampleSpacing, vtkImageData *output, bool LinearInterpolation) {
  if (SuperSampleSpacing <= 0.0) return 1;
  // ---------------------------------
  // Just focus on region of interest
  vtkImageClip  *ROI = vtkImageClip::New();
     ROI->SetInput(input);
     ROI->SetOutputWholeExtent(ROIMin[0],ROIMax[0],ROIMin[1],ROIMax[1],ROIMin[2],ROIMax[2]); 
     ROI->ClipDataOn();   
     ROI->Update(); 

  vtkImageChangeInformation *ROIExtent = vtkImageChangeInformation::New();
     ROIExtent->SetInput(ROI->GetOutput());
     ROIExtent->SetOutputExtentStart(0,0,0); 
  ROIExtent->Update();
 
  // ---------------------------------
  // Now perform super sampling 
  vtkImageResample *ROISuperSample = vtkImageResample::New(); 
     ROISuperSample->SetDimensionality(3);
     if(LinearInterpolation)
       ROISuperSample->SetInterpolationModeToLinear();
     else
       ROISuperSample->SetInterpolationModeToNearestNeighbor();
     ROISuperSample->SetInput(ROIExtent->GetOutput());
     ROISuperSample->SetAxisOutputSpacing(0,SuperSampleSpacing);
     ROISuperSample->SetAxisOutputSpacing(1,SuperSampleSpacing);
     ROISuperSample->SetAxisOutputSpacing(2,SuperSampleSpacing);
     ROISuperSample->ReleaseDataFlagOff();
  ROISuperSample->Update();

  vtkImageCast *cast = vtkImageCast::New();
  cast->SetOutputScalarTypeToShort();
  cast->SetInput(ROISuperSample->GetOutput());
  cast->Update();

  // ---------------------------------
  // Clean up 
//  output->DeepCopy(ROISuperSample->GetOutput());
  output->DeepCopy(cast->GetOutput());

  ROISuperSample->Delete();
  ROIExtent->Delete();
  ROI->Delete();
  cast->Delete();
  return 0;
}

vtkMRMLScalarVolumeNode* vtkChangeTrackerLogic::CreateSuperSample(int ScanNum) {
  // ---------------------------------
  // Initialize Variables 
  if (!this->ChangeTrackerNode)  return NULL;

  vtkMRMLVolumeNode* volumeNode = NULL;
  bool LinearInterpolation = true;
  switch(ScanNum){
  case 0:
    volumeNode = vtkMRMLVolumeNode::SafeDownCast(
      this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_InputSegmRef()));
    LinearInterpolation = 0;
    break;
  case 2:
    volumeNode = vtkMRMLVolumeNode::SafeDownCast(
      this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan2_GlobalRef()));
    break;
  case 1:
    volumeNode = vtkMRMLVolumeNode::SafeDownCast(
      this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_Ref()));
    break;
  default: vtkErrorMacro(<< "Internal error: unknown image requested for supersampling");
           return NULL;
  }

  // make sure the input image is valid!
  if(!volumeNode->GetImageData())
    return NULL;

  if (!this->CheckROI(volumeNode)) {
    vtkErrorMacro(<<"Warning: vtkChangeTrackerLogic::CreateSuperSample: Scan " << ScanNum << " failed CheckROI");
    return NULL;
  }

  // ---------------------------------
  // Perform Super Sampling 
  int ROIMin[3] = {this->ChangeTrackerNode->GetROIMin(0), this->ChangeTrackerNode->GetROIMin(1), this->ChangeTrackerNode->GetROIMin(2)};
  int ROIMax[3] = {this->ChangeTrackerNode->GetROIMax(0), this->ChangeTrackerNode->GetROIMax(1), this->ChangeTrackerNode->GetROIMax(2)};

  double SuperSampleSpacing = -1;
  if (ScanNum == 1) {
    double *Spacing = volumeNode->GetSpacing();
    SuperSampleSpacing = this->DefineSuperSampleSize(Spacing, ROIMin, ROIMax);
    double SuperSampleVol = SuperSampleSpacing*SuperSampleSpacing*SuperSampleSpacing;
    this->ChangeTrackerNode->SetSuperSampled_Spacing(SuperSampleSpacing);    
    this->ChangeTrackerNode->SetSuperSampled_VoxelVolume(SuperSampleVol); 
    this->ChangeTrackerNode->SetSuperSampled_RatioNewOldSpacing(SuperSampleVol/(Spacing[0]*Spacing[1]*Spacing[2]));
    this->ChangeTrackerNode->SetScan1_VoxelVolume(Spacing[0]*Spacing[1]*Spacing[2]);
  } else {
    SuperSampleSpacing = this->ChangeTrackerNode->GetSuperSampled_Spacing();
  }

  vtkImageChangeInformation *ROISuperSampleInput = vtkImageChangeInformation::New();
     ROISuperSampleInput->SetInput(volumeNode->GetImageData());
     ROISuperSampleInput->SetOutputSpacing(volumeNode->GetSpacing());
  ROISuperSampleInput->Update();

  vtkImageData *ROISuperSampleOutput = vtkImageData::New();
  if (this->CreateSuperSampleFct(ROISuperSampleInput->GetOutput(), ROIMin, ROIMax, SuperSampleSpacing, ROISuperSampleOutput, LinearInterpolation)) {
    ROISuperSampleInput->Delete();
    ROISuperSampleOutput->Delete();
    return NULL;
  }

  vtkImageChangeInformation *ROISuperSampleExtent = vtkImageChangeInformation::New();
     ROISuperSampleExtent->SetInput(ROISuperSampleOutput);
     ROISuperSampleExtent->SetOutputSpacing(1,1,1);
  ROISuperSampleExtent->Update();

  // We need to copy the result  bc ChangeInformation only has a pointer to the input data 
  // - which is later deleted and therefore will cause errors 
  vtkImageData *ROISuperSampleFinal = vtkImageData::New();
     ROISuperSampleFinal->DeepCopy(ROISuperSampleExtent->GetOutput());

  // Compute new origin
  vtkMatrix4x4 *ijkToRAS=vtkMatrix4x4::New();
  volumeNode->GetIJKToRASMatrix(ijkToRAS);
  double newIJKOrigin[4] = 
    {this->ChangeTrackerNode->GetROIMin(0),this->ChangeTrackerNode->GetROIMin(1), this->ChangeTrackerNode->GetROIMin(2), 1.0 };
  double newRASOrigin[4];
  ijkToRAS->MultiplyPoint(newIJKOrigin,newRASOrigin);
  ijkToRAS->Delete();


  // ---------------------------------
  // Now return results and clean up 
  char VolumeOutputName[255];
  switch(ScanNum){
    case 2:
      sprintf(VolumeOutputName, "%s_VOI_GlobalReg_SuperSampled", this->GetInputScanName(1));
      break;
    case 1:
      sprintf(VolumeOutputName, "%s_VOI_SuperSampled", this->GetInputScanName(0));
      break;
    case 0:
      sprintf(VolumeOutputName, "Scan1Segmented_VOI_SuperSampled");
      break;
    }

  vtkMRMLScalarVolumeNode *VolumeOutputNode;
  if(LinearInterpolation)
    VolumeOutputNode = this->CreateVolumeNode(volumeNode,VolumeOutputName);
  else
    {
//    vtkSlicerApplication *application = vtkSlicerApplication::SafeDownCast(this->GetGUI()->GetApplication()); 
    vtkSlicerVolumesLogic *volumesLogic  
      = (vtkSlicerVolumesGUI::SafeDownCast(vtkSlicerApplication::GetInstance()->GetModuleGUIByName("Volumes")))->GetLogic();
    vtkMRMLScene *scene = this->ChangeTrackerNode->GetScene();
    VolumeOutputNode = volumesLogic->CreateLabelVolume(scene, volumeNode, VolumeOutputName);
    }

  VolumeOutputNode->SetAndObserveImageData(ROISuperSampleFinal);
  VolumeOutputNode->SetSpacing(SuperSampleSpacing,SuperSampleSpacing,SuperSampleSpacing);  
  VolumeOutputNode->SetOrigin(newRASOrigin[0],newRASOrigin[1],newRASOrigin[2]);

  ROISuperSampleFinal->Delete();
  ROISuperSampleExtent->Delete();
  ROISuperSampleOutput->Delete();
  ROISuperSampleInput->Delete();
  return VolumeOutputNode;
}

  // In tcl
  // set GUI  [$::slicer3::Application GetModuleGUIByName "ChangeTracker"]
  //    set NODE [$GUI  GetNode]
  //    set SCENE [$NODE GetScene]
  // set VolumeOutputNode [$SCENE GetNodeByID [$NODE GetScan1_SuperSampleRef]]
  // set VolumeOutputDisplayNode [$VolumeOutputNode GetScalarVolumeDisplayNode]
  // Important files  
  // ~/Slicer/Slicer3/Base/Logic/vtkSlicerVolumesLogic
  // ~/Slicer/Slicer3/Libs/MRML/vtkMRMLVolumeNode.h
  
  // this->ChangeTrackerNode->GetScene()->AddNode(VolumeOutputNode);
  // VolumeOutputNode->Delete();


void vtkChangeTrackerLogic::SourceAnalyzeTclScripts(vtkKWApplication *app) {
 char TCL_FILE[1024]; 
 sprintf(TCL_FILE,"%s/Tcl/ChangeTrackerFct.tcl",
         this->GetModuleShareDirectory());

 app->LoadScript(TCL_FILE); 

 sprintf(TCL_FILE,"%s/Tcl/ChangeTrackerReg.tcl",
         this->GetModuleShareDirectory());
 app->LoadScript(TCL_FILE); 

 //   cout << "SourceAnalyzeTclScripts: This is just for debugging right now" << endl;
 //   sprintf(TCL_FILE,"/home/pohl/slicer3/Slicer3-build/share/Slicer3/Modules/ChangeTracker/Tcl/ChangeTrackerFct.tcl");
 //   app->LoadScript(TCL_FILE); 
 // 
 //   sprintf(TCL_FILE,"/home/pohl/slicer3/Slicer3-build/share/Slicer3/Modules/ChangeTracker/Tcl/ChangeTrackerReg.tcl");
 //   app->LoadScript(TCL_FILE); 
 
}

void vtkChangeTrackerLogic::DeleteAnalyzeOutput(vtkSlicerApplication *app) {
   // Delete old attached node first 
  if (!ChangeTrackerNode) return;
  this->SourceAnalyzeTclScripts(app);
  app->Script("::ChangeTrackerTcl::Scan2ToScan1Registration_DeleteOutput Global");
  vtkMRMLVolumeNode* currentNode =  vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan2_SuperSampleRef()));
  if (currentNode) { 
    this->ChangeTrackerNode->GetScene()->RemoveNode(currentNode); 
    this->ChangeTrackerNode->SetScan2_SuperSampleRef(NULL);
  }
  app->Script("::ChangeTrackerTcl::HistogramNormalization_DeleteOutput"); 
  app->Script("::ChangeTrackerTcl::Scan2ToScan1Registration_DeleteOutput Local"); 
  app->Script("::ChangeTrackerTcl::IntensityThresholding_DeleteOutput 1");
  app->Script("::ChangeTrackerTcl::IntensityThresholding_DeleteOutput 2");
  app->Script("::ChangeTrackerTcl::Analysis_Intensity_DeleteOutput_GUI"); 
}

int vtkChangeTrackerLogic::AnalyzeGrowth(vtkSlicerApplication *app) {
  // This is for testing how to start a tcl script 
  //cout << "=== Start ANALYSIS ===" << endl;

  this->SourceAnalyzeTclScripts(app);
  vtkKWProgressGauge *progressBar = app->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge();
  
  int debug =  0; 
  double TimeLength = 0.55;
  if (this->ChangeTrackerNode->GetAnalysis_Intensity_Flag()) TimeLength += 0.25;
  if (this->ChangeTrackerNode->GetAnalysis_Deformable_Flag()) TimeLength += 0.60;

  if (!debug) { 
    progressBar->SetValue(5.0/TimeLength);
    if(this->ChangeTrackerNode->GetUseITK()){
      // AF: at this point, registration must have completed
      //assert(this->ChangeTrackerNode->GetScan2_RegisteredRef());
      if(DoITKRegistration(vtkSlicerApplication::GetInstance()))
        return ERR_GLOBAL_REG;
      app->Script("update");
    } else {
      // AF: keep initial registration approach for validation
      app->Script("::ChangeTrackerTcl::Scan2ToScan1Registration_GUI Global");
    }
    progressBar->SetValue(25.0/TimeLength);

    //----------------------------------------------
    // Second step -> Save the outcome
    if (!this->ChangeTrackerNode) {
      return ERR_OTHER;
    }
    this->DeleteSuperSample(2);
    vtkMRMLScalarVolumeNode *outputNode = this->CreateSuperSample(2);

    if (!outputNode) {
      return ERR_OTHER;
    } 

    this->ChangeTrackerNode->SetScan2_SuperSampleRef(outputNode->GetID());
    this->SaveVolume(app,outputNode);
    progressBar->SetValue(30.0/TimeLength);

    //----------------------------------------------
    // Kilian-Feb-08 you should first register and then normalize bc registration is not impacted by normalization 
    if(this->ChangeTrackerNode->GetUseITK()){
      // AF: do local registration. It is probably not a good style to have a
      // separate function for the similar functionality, but style is not the
      // goal.
      if(DoITKROIRegistration(vtkSlicerApplication::GetInstance()))
        return ERR_LOCAL_REG;
      app->Script("update");
    } else {
      app->Script("::ChangeTrackerTcl::Scan2ToScan1Registration_GUI Local"); 
      progressBar->SetValue(50.0/TimeLength);
    }
    std::cerr << "Before normalization" << std::endl;
    app->Script("::ChangeTrackerTcl::HistogramNormalization_GUI"); 
    progressBar->SetValue(55.0/TimeLength);
    std::cerr << "After normalization" << std::endl;

  } else {
    cout << "DEBUGGING " << endl;
    if (1) {
      if (!this->ChangeTrackerNode->GetScan2_NormedRef() || !strcmp(this->ChangeTrackerNode->GetScan2_NormedRef(),"")) { 
        char fileName[1024];
        sprintf(fileName,"%s/TG_scan2_norm.nhdr",this->ChangeTrackerNode->GetWorkingDir());
        vtkMRMLVolumeNode* tmp = this->LoadVolume(app,fileName,0,"TG_scan2_norm");
        if (tmp) {
          this->ChangeTrackerNode->SetScan2_NormedRef(tmp->GetID());
        } else {
          cout << "Error: Could not load " << fileName << endl;
          return ERR_OTHER;
        }
      }
    } else {
      if (!this->ChangeTrackerNode->GetScan2_LocalRef() || !strcmp(this->ChangeTrackerNode->GetScan2_LocalRef(),"")) { 
        char fileName[1024];
        sprintf(fileName,"%s/TG_scan2_Local.nrrd",this->ChangeTrackerNode->GetWorkingDir());
        vtkMRMLVolumeNode* tmp = this->LoadVolume(app,fileName,0,"TG_scan2_Local");
        if (tmp) {
          this->ChangeTrackerNode->SetScan2_LocalRef(tmp->GetID());
        } else {
          cout << "Error: Could not load " << fileName << endl;
          return ERR_OTHER;
        }
      }
    }
  }


  if (this->ChangeTrackerNode->GetAnalysis_Intensity_Flag()) { 

    // If input segmentation is provided, the thresholds are not set. 
    // The temporary solution will be to set thresholds based on the
    // min/max intensities in the segmented area.
    if(this->ChangeTrackerNode->GetScan1_InputSegmRef())
      SetThresholdsFromSegmentation();

    if (!atoi(app->Script("::ChangeTrackerTcl::IntensityThresholding_GUI 1"))) 
      return ERR_OTHER; 
    std::cerr << "IntensityThresholding 1" << std::endl;
    progressBar->SetValue(60.0/TimeLength);
    if (!atoi(app->Script("::ChangeTrackerTcl::IntensityThresholding_GUI 2"))) 
      return ERR_OTHER; 
    std::cerr << "Intensitythresholding 2" << std::endl;
    progressBar->SetValue(65.0/TimeLength);
    if (!atoi(app->Script("::ChangeTrackerTcl::Analysis_Intensity_GUI"))) 
      return ERR_OTHER; 
    std::cerr << "AnalysisIntensity" << std::endl;
    progressBar->SetValue(80.0/TimeLength);
   } 
  if (this->ChangeTrackerNode->GetAnalysis_Deformable_Flag()) {
    if (debug) {
      if (!this->ChangeTrackerNode->GetAnalysis_Deformable_Ref() || !strcmp(this->ChangeTrackerNode->GetAnalysis_Deformable_Ref(),"")) { 
        char fileName[1024];
        sprintf(fileName,"%s/TG_Analysis_Deformable.nhdr",this->ChangeTrackerNode->GetWorkingDir());
        vtkMRMLVolumeNode* tmp = this->LoadVolume(app,fileName,1,"TG_Analysis_Deformable");
        if (tmp) {
          this->ChangeTrackerNode->SetAnalysis_Deformable_Ref(tmp->GetID());
        } else {
         cout << "Error: Could not load " << fileName << endl;
         return ERR_OTHER;
        }
      }
    } else {
      if (!atoi(app->Script("::ChangeTrackerTcl::Analysis_Deformable_GUI"))) 
        return ERR_OTHER; 
      progressBar->SetValue(100);
    }
  }
  // cout << "=== End ANALYSIS ===" << endl;

  return 0;
}

void vtkChangeTrackerLogic::RegisterMRMLNodesWithScene() {
   vtkMRMLChangeTrackerNode* tmNode =  vtkMRMLChangeTrackerNode::New();
   this->GetMRMLScene()->RegisterNodeClass(tmNode);
   tmNode->Delete();
}


vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_Scan1ByLower() {
  if (this->Analysis_Intensity_Scan1ByLower) { this->Analysis_Intensity_Scan1ByLower->Delete(); }
  this->Analysis_Intensity_Scan1ByLower = vtkImageThreshold::New();
  return this->Analysis_Intensity_Scan1ByLower;
}

vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_Scan1Range() {
  if (this->Analysis_Intensity_Scan1Range) { this->Analysis_Intensity_Scan1Range->Delete(); }
  this->Analysis_Intensity_Scan1Range = vtkImageThreshold::New();
  return this->Analysis_Intensity_Scan1Range;
}

vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_Scan2ByLower() {
  if (this->Analysis_Intensity_Scan2ByLower) { this->Analysis_Intensity_Scan2ByLower->Delete(); }
  this->Analysis_Intensity_Scan2ByLower = vtkImageThreshold::New();
  return this->Analysis_Intensity_Scan2ByLower;
}

vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_Scan2Range() {
  if (this->Analysis_Intensity_Scan2Range) { this->Analysis_Intensity_Scan2Range->Delete(); }
  this->Analysis_Intensity_Scan2Range = vtkImageThreshold::New();
  return this->Analysis_Intensity_Scan2Range;
}


vtkImageMathematics* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ScanSubtract() {
  if (this->Analysis_Intensity_ScanSubtract) { this->Analysis_Intensity_ScanSubtract->Delete(); }
  this->Analysis_Intensity_ScanSubtract = vtkImageMathematics::New();
  return this->Analysis_Intensity_ScanSubtract;
}

vtkImageMedian3D* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ScanSubtractSmooth() {
  if (this->Analysis_Intensity_ScanSubtractSmooth) { this->Analysis_Intensity_ScanSubtractSmooth->Delete(); }
  this->Analysis_Intensity_ScanSubtractSmooth = vtkImageMedian3D::New();
  return this->Analysis_Intensity_ScanSubtractSmooth;
}

vtkImageData* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIGrowth() {
  if (this->Analysis_Intensity_ROIGrowth) { this->Analysis_Intensity_ROIGrowth->Delete(); }
  this->Analysis_Intensity_ROIGrowth = vtkImageData::New();
  return this->Analysis_Intensity_ROIGrowth;
}

vtkImageData* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIShrink() {
  if (this->Analysis_Intensity_ROIShrink) { this->Analysis_Intensity_ROIShrink->Delete(); }
  this->Analysis_Intensity_ROIShrink = vtkImageData::New();
  return this->Analysis_Intensity_ROIShrink;
}

vtkImageMathematics* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIGrowthInt() {
  if (this->Analysis_Intensity_ROIGrowthInt) { this->Analysis_Intensity_ROIGrowthInt->Delete(); }
  this->Analysis_Intensity_ROIGrowthInt = vtkImageMathematics::New();
  return this->Analysis_Intensity_ROIGrowthInt;
}

vtkImageMathematics* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIShrinkInt() {
  if (this->Analysis_Intensity_ROIShrinkInt) { this->Analysis_Intensity_ROIShrinkInt->Delete(); }
  this->Analysis_Intensity_ROIShrinkInt = vtkImageMathematics::New();
  return this->Analysis_Intensity_ROIShrinkInt;
}

vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROINegativeBin() {
  if (this->Analysis_Intensity_ROINegativeBin) { this->Analysis_Intensity_ROINegativeBin->Delete(); }
  this->Analysis_Intensity_ROINegativeBin = vtkImageThreshold::New();
  return this->Analysis_Intensity_ROINegativeBin;
}

vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIPositiveBin() {
  if (this->Analysis_Intensity_ROIPositiveBin) { this->Analysis_Intensity_ROIPositiveBin->Delete(); }
  this->Analysis_Intensity_ROIPositiveBin = vtkImageThreshold::New();
  return this->Analysis_Intensity_ROIPositiveBin;
}

vtkImageMathematics* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIBinCombine() {
  if (this->Analysis_Intensity_ROIBinCombine) { this->Analysis_Intensity_ROIBinCombine->Delete(); }
  this->Analysis_Intensity_ROIBinCombine = vtkImageMathematics::New();
  return this->Analysis_Intensity_ROIBinCombine;
}

vtkImageIslandFilter* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIPositiveBinReal() {
  if (this->Analysis_Intensity_ROIPositiveBinReal) { this->Analysis_Intensity_ROIPositiveBinReal->Delete(); }
  this->Analysis_Intensity_ROIPositiveBinReal = vtkImageIslandFilter::New();
  return this->Analysis_Intensity_ROIPositiveBinReal;
}

vtkImageIslandFilter* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROINegativeBinReal() {
  if (this->Analysis_Intensity_ROINegativeBinReal) { this->Analysis_Intensity_ROINegativeBinReal->Delete(); }
  this->Analysis_Intensity_ROINegativeBinReal = vtkImageIslandFilter::New();
  return this->Analysis_Intensity_ROINegativeBinReal;
}


vtkImageMathematics* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIBinAdd() {
  if (this->Analysis_Intensity_ROIBinAdd) { this->Analysis_Intensity_ROIBinAdd->Delete(); }
  this->Analysis_Intensity_ROIBinAdd = vtkImageMathematics::New();
  return this->Analysis_Intensity_ROIBinAdd;
}

vtkImageThreshold* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIBinDisplay() {
  if (this->Analysis_Intensity_ROIBinDisplay) { this->Analysis_Intensity_ROIBinDisplay->Delete(); }
  this->Analysis_Intensity_ROIBinDisplay = vtkImageThreshold::New();
  return this->Analysis_Intensity_ROIBinDisplay;
}

vtkImageSumOverVoxels* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIGrowthVolume() {
  if (this->Analysis_Intensity_ROIGrowthVolume) { this->Analysis_Intensity_ROIGrowthVolume->Delete(); }
  this->Analysis_Intensity_ROIGrowthVolume = vtkImageSumOverVoxels::New();
  return this->Analysis_Intensity_ROIGrowthVolume;
}
vtkImageSumOverVoxels* vtkChangeTrackerLogic::CreateAnalysis_Intensity_ROIShrinkVolume() {
  if (this->Analysis_Intensity_ROIShrinkVolume) { this->Analysis_Intensity_ROIShrinkVolume->Delete(); }
  this->Analysis_Intensity_ROIShrinkVolume = vtkImageSumOverVoxels::New();
  return this->Analysis_Intensity_ROIShrinkVolume;
}

vtkImageData*  vtkChangeTrackerLogic::GetAnalysis_Intensity_ROIBinCombine() { 
  return (this->Analysis_Intensity_ROIBinCombine ? this->Analysis_Intensity_ROIBinCombine->GetOutput() : NULL);
}

vtkImageData*  vtkChangeTrackerLogic::GetAnalysis_Intensity_ROIBinDisplay() { 
  return (this->Analysis_Intensity_ROIBinDisplay ? this->Analysis_Intensity_ROIBinDisplay->GetOutput() : NULL);
}

void vtkChangeTrackerLogic::MeassureGrowth(double &Shrinkage, double &Growth) {
   if (this->ChangeTrackerNode) {
   this->MeassureGrowth(
     static_cast<int>(this->ChangeTrackerNode->GetSegmentThresholdMin()),
     static_cast<int>(this->ChangeTrackerNode->GetSegmentThresholdMax()),
     Shrinkage, Growth); 
   }  else {
     cout << "Error: vtkChangeTrackerLogic::MeassureGrowth: No ChangeTrackerNode defined" << endl;
   }
 }

void vtkChangeTrackerLogic::MeassureGrowth(int SegmentThreshMin, int SegmentThreshMax,double &Shrinkage, double &Growth) {
   
  if (!this->Analysis_Intensity_ROINegativeBin || !this->Analysis_Intensity_ROIPositiveBin || !this->Analysis_Intensity_ROIShrinkVolume || !this->Analysis_Intensity_ROIGrowthVolume ) {
    Shrinkage = 1 ;
    Growth = -1;
    return;

  }
  int IntensityMin = SegmentThreshMin - (int) this->Analysis_Intensity_Threshold ;
  int IntensityMax = SegmentThreshMax + (int) this->Analysis_Intensity_Threshold ;

//  std::cerr << "Intensity range: " << IntensityMin << " - " << IntensityMax << std::endl;

  // Biasing results - this is a hack right now 
  // Used in in journal publication - for some reason the pipeline favors shrinkage over growth 
  // ShrinkBias > 1 => less Shrinkage; ShrinkBias < 1 => more shrinkage  
  float ShrinkBias = 1.1; 
  float GrowthBias = 1.0; 

  if (this->Analysis_Intensity_Scan1ByLower) {
    this->Analysis_Intensity_Scan1ByLower->ThresholdByUpper(IntensityMin);
    this->Analysis_Intensity_Scan1ByLower->SetOutValue(IntensityMin);
    this->Analysis_Intensity_Scan1ByLower->Update();
  }
  if ( this->Analysis_Intensity_Scan1Range ) {
    this->Analysis_Intensity_Scan1Range->ThresholdByLower(IntensityMax); 
    this->Analysis_Intensity_Scan1Range->SetOutValue(IntensityMax);
    this->Analysis_Intensity_Scan1Range->Update();
  } 

  if (this->Analysis_Intensity_Scan2ByLower) {
    this->Analysis_Intensity_Scan2ByLower->ThresholdByUpper(IntensityMin);
    this->Analysis_Intensity_Scan2ByLower->SetOutValue(IntensityMin);
    this->Analysis_Intensity_Scan2ByLower->Update(); 
  }

  if (this->Analysis_Intensity_Scan2Range) {
    this->Analysis_Intensity_Scan2Range->ThresholdByLower(IntensityMax); 
    this->Analysis_Intensity_Scan2Range->SetOutValue(IntensityMax);
    this->Analysis_Intensity_Scan2Range->Update();
  }

  
  this->Analysis_Intensity_ScanSubtract->Update();
  this->Analysis_Intensity_ScanSubtractSmooth->Update();
  this->Analysis_Intensity_ROIGrowthInt->Update();
  this->Analysis_Intensity_ROIShrinkInt->Update();

  // See Corresponding comment in ChangeTrackerFct - reduces bias towards shrinkage
  this->Analysis_Intensity_ROINegativeBin->ThresholdByLower(-ShrinkBias*this->Analysis_Intensity_Threshold); 
  this->Analysis_Intensity_ROINegativeBin->Update(); 
  this->Analysis_Intensity_ROIPositiveBin->ThresholdByUpper(GrowthBias*this->Analysis_Intensity_Threshold); 
  this->Analysis_Intensity_ROIPositiveBin->Update(); 
  this->Analysis_Intensity_ROIPositiveBinReal->Update();
  this->Analysis_Intensity_ROINegativeBinReal->Update();
  this->Analysis_Intensity_ROIBinAdd->Update();
  this->Analysis_Intensity_ROIBinDisplay->Update();
  this->Analysis_Intensity_ROIGrowthVolume->Update(); 
  this->Analysis_Intensity_ROIShrinkVolume->Update(); 

  // cout << "BLUBB: Growth " <<  this->Analysis_Intensity_ROIGrowthVolume->GetVoxelSum() << " Shrink " << this->Analysis_Intensity_ROIShrinkVolume->GetVoxelSum() << " Total " <<  this->Analysis_Intensity_ROIGrowthVolume->GetVoxelSum() +  this->Analysis_Intensity_ROIShrinkVolume->GetVoxelSum() << " Total-original " << this->Analysis_Intensity_ROITotal->GetVoxelSum() << endl;
  Shrinkage = this->Analysis_Intensity_ROIShrinkVolume->GetVoxelSum(); 
  Growth = this->Analysis_Intensity_ROIGrowthVolume->GetVoxelSum(); 

}

void vtkChangeTrackerLogic::SaveVolume(vtkSlicerApplication *app, vtkMRMLVolumeNode *volNode) {
  if (!this->SaveVolumeFlag) return;  
  this->SaveVolumeForce(app,volNode);
}


void vtkChangeTrackerLogic::SaveVolumeFileName( vtkMRMLVolumeNode *volNode, char *FileName) {
  sprintf(FileName,"%s/%s.nhdr",this->ChangeTrackerNode->GetWorkingDir(),volNode->GetName());
}

void vtkChangeTrackerLogic::SaveVolumeForce(vtkSlicerApplication *app, vtkMRMLVolumeNode *volNode) {
 // Initialize
 vtkSlicerVolumesGUI  *volumesGUI    = vtkSlicerVolumesGUI::SafeDownCast(app->GetModuleGUIByName("Volumes")); 
 if (!volumesGUI) return;
 vtkSlicerVolumesLogic *volumesLogic = volumesGUI->GetLogic();

 // Create Directory if necessary
 {
   char CMD[1024];
   sprintf(CMD,"file isdirectory %s",this->ChangeTrackerNode->GetWorkingDir()); 
   if (!atoi(app->Script(CMD))) { 
     sprintf(CMD,"file mkdir %s",this->ChangeTrackerNode->GetWorkingDir()); 
     app->Script(CMD); 
   }
 }

 {
   char fileName[1024];
   this->SaveVolumeFileName(volNode,fileName);
   if (!volumesLogic->SaveArchetypeVolume( fileName, volNode ) )  {
     cout << "Error: Could no save file " << endl;
   }
 }
}

vtkMRMLVolumeNode* vtkChangeTrackerLogic::LoadVolume(vtkSlicerApplication *app, char* fileName, int LabelMapFlag,const char* volumeName) {
   vtkSlicerVolumesGUI  *volumesGUI    = vtkSlicerVolumesGUI::SafeDownCast(app->GetModuleGUIByName("Volumes")); 
   if (!volumesGUI) return NULL;
   vtkSlicerVolumesLogic *volumesLogic = volumesGUI->GetLogic();
   // Ignore error messages - I do not know how to get around them 
   return volumesLogic->AddArchetypeVolume(fileName,volumeName,LabelMapFlag);
}

void vtkChangeTrackerLogic::VolumeWriter(const char* fileName, vtkImageData *Output) {
    vtkNRRDWriter *iwriter = vtkNRRDWriter::New(); 
    iwriter->SetInput(Output);
    iwriter->SetFileName(fileName);
    iwriter->Write();
    iwriter->Delete();
}
 



//----------------------------------------------------------------------------
void vtkChangeTrackerLogic::PrintResult(ostream& os, vtkSlicerApplication *app)
{  
  // vtkMRMLNode::PrintSelf(os,indent);
  if (!this->ChangeTrackerNode) return;
  os  << "This file was generated by vtkMrmChangeTrackerNode " << "\n";;
  os  << "Date:      " << app->Script("date") << "\n";;

  vtkMRMLVolumeNode *VolNode = vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_Ref()));
  os  << "Scan1_Ref: " <<  (VolNode && VolNode->GetStorageNode() ? VolNode->GetStorageNode()->GetFileName() : "(none)") << "\n";

  VolNode = vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan2_Ref()));
  os  << "Scan2_Ref: " <<  (VolNode && VolNode->GetStorageNode() ? VolNode->GetStorageNode()->GetFileName() : "(none)") << "\n";
  os  << "ROI:" << endl;
  os  << "  Min: " << this->ChangeTrackerNode->GetROIMin(0) << " "<< this->ChangeTrackerNode->GetROIMin(1) << " "<< this->ChangeTrackerNode->GetROIMin(2) <<"\n";
  os  << "  Max: " << this->ChangeTrackerNode->GetROIMax(0) << " "<< this->ChangeTrackerNode->GetROIMax(1) << " "<< this->ChangeTrackerNode->GetROIMax(2) <<"\n";
  os  << "Threshold: [" << this->ChangeTrackerNode->GetSegmentThresholdMin() <<", " << this->ChangeTrackerNode->GetSegmentThresholdMax() << "]\n";
  if (this->ChangeTrackerNode->GetAnalysis_Intensity_Flag()) {
    os  << "Analysis based on Intensity Pattern" << endl;
    os  << "  Sensitivity:      "<< this->ChangeTrackerNode->GetAnalysis_Intensity_Sensitivity() << "\n";
    app->Script("::ChangeTrackerTcl::Analysis_Intensity_UpdateThreshold_GUI"); 
    double Growth, Shrinkage;
    this->MeassureGrowth(Shrinkage,Growth); 
    double Total = Growth + Shrinkage;
    os  << "  Intensity Metric: \n";
    os <<  "    Shrinkage: " << floor(-Shrinkage*this->ChangeTrackerNode->GetSuperSampled_VoxelVolume()*1000)/1000.0 << "mm" << char(179) 
       << " (" << int(-Shrinkage*this->ChangeTrackerNode->GetSuperSampled_RatioNewOldSpacing()) << " Voxels)" << "\n";
    os <<  "    Growth: " << floor(Growth*this->ChangeTrackerNode->GetSuperSampled_VoxelVolume()*1000)/1000.0 << "mm" << char(179) 
       << " (" << int(Growth*this->ChangeTrackerNode->GetSuperSampled_RatioNewOldSpacing()) << " Voxels)" << "\n";
    os <<  "    Total Change: " << floor(Total*this->ChangeTrackerNode->GetSuperSampled_VoxelVolume()*1000)/1000.0 << "mm" << char(179) 
       << " (" << int(Total*this->ChangeTrackerNode->GetSuperSampled_RatioNewOldSpacing()) << " Voxels)" << "\n";
  }
  if (this->ChangeTrackerNode->GetAnalysis_Deformable_Flag()) {
    os  << "Analysis based on Deformable Map" << endl;
    os  << "  Segmentation Metric: "<<  floor(this->ChangeTrackerNode->GetAnalysis_Deformable_SegmentationGrowth()*1000)/1000.0 << "mm" << char(179) 
       << " (" << int(this->ChangeTrackerNode->GetAnalysis_Deformable_SegmentationGrowth()/this->ChangeTrackerNode->GetScan1_VoxelVolume()) << " Voxels)\n";
    os  << "  Jacobian Metric:     "<<  floor(this->ChangeTrackerNode->GetAnalysis_Deformable_JacobianGrowth()*1000)/1000.0 << "mm" << char(179) 
       << " (" << int(this->ChangeTrackerNode->GetAnalysis_Deformable_JacobianGrowth()/this->ChangeTrackerNode->GetScan1_VoxelVolume()) << " Voxels)\n";
  }
}

// works for running stuff in TCL so that you do not need to look in two windows 
void vtkChangeTrackerLogic::PrintText(char *TEXT) {
  cout << TEXT << endl;
} 

void vtkChangeTrackerLogic::DefinePreSegment(vtkImageData *INPUT, const int RANGE[2], vtkImageThreshold *OUTPUT) {
  OUTPUT->SetInValue(10);
  OUTPUT->SetOutValue(0);
  OUTPUT->SetOutputScalarTypeToShort();
  OUTPUT->SetInput(INPUT); 
  OUTPUT->ThresholdBetween(RANGE[0],RANGE[1]); 
  OUTPUT->Update();
}

void vtkChangeTrackerLogic::DefineSegment(vtkImageData *INPUT, vtkImageIslandFilter *OUTPUT) {
  OUTPUT->SetIslandMinSize(1000);
  OUTPUT->SetInput(INPUT);
  OUTPUT->SetNeighborhoodDim3D();
  // OUTPUT->SetPrintInformation(2); 
  OUTPUT->Update(); 
}

void vtkChangeTrackerLogic::LinearResample (vtkMRMLVolumeNode* inputVolumeNode, vtkMRMLVolumeNode* outputVolumeNode, vtkMRMLVolumeNode* outputVolumeGeometryNode,
                    vtkTransform* outputRASToInputRASTransform, double backgroundLevel)
{
  vtkImageData* inputImageData  = inputVolumeNode->GetImageData();
  vtkImageData* outputImageData = outputVolumeNode->GetImageData();
  vtkImageData* outputGeometryData = NULL;
  if (outputVolumeGeometryNode != NULL)
    {
    outputGeometryData = outputVolumeGeometryNode->GetImageData();
    }

  vtkImageReslice* resliceFilter = vtkImageReslice::New();

  //
  // set inputs
  resliceFilter->SetInput(inputImageData);

  //
  // set geometry
  if (outputGeometryData != NULL)
    {
    resliceFilter->SetInformationInput(outputGeometryData);
    outputVolumeNode->CopyOrientation(outputVolumeGeometryNode);
    }

  //
  // setup total transform
  // ijk of output -> RAS -> XFORM -> RAS -> ijk of input
  vtkTransform* totalTransform = vtkTransform::New();
  if (outputRASToInputRASTransform != NULL)
    {
    totalTransform->DeepCopy(outputRASToInputRASTransform);
    }

  vtkMatrix4x4* outputIJKToRAS  = vtkMatrix4x4::New();
  outputVolumeNode->GetIJKToRASMatrix(outputIJKToRAS);
  vtkMatrix4x4* inputRASToIJK = vtkMatrix4x4::New();
  inputVolumeNode->GetRASToIJKMatrix(inputRASToIJK);

  totalTransform->PreMultiply();
  totalTransform->Concatenate(outputIJKToRAS);
  totalTransform->PostMultiply();
  totalTransform->Concatenate(inputRASToIJK);
  resliceFilter->SetResliceTransform(totalTransform);

  //
  // resample the image
  resliceFilter->SetBackgroundLevel(backgroundLevel);
  resliceFilter->OptimizationOn();
  resliceFilter->SetInterpolationModeToLinear();
  resliceFilter->Update();
  outputImageData->ShallowCopy(resliceFilter->GetOutput());

  //
  // clean up
  outputIJKToRAS->Delete();
  inputRASToIJK->Delete();
  resliceFilter->Delete();
  totalTransform->Delete();
}

int vtkChangeTrackerLogic::DoITKRegistration(vtkSlicerApplication *app){

  // Init some useful references
  vtkMRMLCommandLineModuleNode *moduleNode = NULL;
  vtkCommandLineModuleLogic *moduleLogic = NULL;

  vtkMRMLScene *scene = this->ChangeTrackerNode->GetScene();
  vtkMRMLChangeTrackerNode *ctNode = this->ChangeTrackerNode;

  moduleLogic = vtkCommandLineModuleLogic::New();
  
  moduleNode = 
    static_cast<vtkMRMLCommandLineModuleNode*>(scene->CreateNodeByClass("vtkMRMLCommandLineModuleNode"));
  if(!moduleNode){
    return -2;
  }

  moduleNode->SetModuleDescription("Rigid registration");

  vtkMRMLScalarVolumeNode *outputNode;
  outputNode = static_cast<vtkMRMLScalarVolumeNode*>(scene->GetNodeByID(ctNode->GetScan2_GlobalRef()));
  if(outputNode){
    scene->RemoveNode(outputNode);
    ctNode->SetScan2_GlobalRef("");
  }

  char RegVolumeName[255];
  sprintf(RegVolumeName, "%s_GlobalReg", this->GetInputScanName(1));
  outputNode = 
    CreateVolumeNode(static_cast<vtkMRMLVolumeNode*>(scene->GetNodeByID(ctNode->GetScan1_Ref())), 
                     RegVolumeName);

  // Create output transform node
  vtkMRMLLinearTransformNode *transformNode =
    vtkMRMLLinearTransformNode::New();
  transformNode->SetName("Global_LRTransform");
  scene->AddNode(transformNode);
  transformNode->Delete();

  // Linear registration parameter setup
  moduleNode->SetParameterAsString("FixedImageFileName", ctNode->GetScan1_Ref());
  moduleNode->SetParameterAsString("MovingImageFileName", ctNode->GetScan2_Ref());
  moduleNode->SetParameterAsString("Iterations", "100,100,50,20");
  moduleNode->SetParameterAsString("ResampledImageFileName", outputNode->GetID());
  moduleNode->SetParameterAsString("OutputTransform", transformNode->GetID());

  moduleLogic->SetAndObserveMRMLScene(scene);
  moduleLogic->SetApplicationLogic(app->GetApplicationGUI()->GetApplicationLogic());
  moduleLogic->SetTemporaryDirectory(app->GetTemporaryDirectory());
  moduleLogic->LazyEvaluateModuleTarget(moduleNode);
  ModuleDescription moduleDesc = moduleNode->GetModuleDescription();
  if(moduleDesc.GetTarget() == "Unknown")
    return -4;  // failed to find registration module

  moduleLogic->ApplyAndWait(moduleNode);
  if(moduleNode->GetStatus() != vtkMRMLCommandLineModuleNode::Completed)
    return -5;

  ctNode->SetScan2_GlobalRef(outputNode->GetID());
  moduleLogic->SetAndObserveMRMLScene(NULL);
  moduleLogic->Delete();
  moduleNode->Delete();

  return 0;
}


// AF: registration of ROI
int vtkChangeTrackerLogic::DoITKROIRegistration(vtkSlicerApplication *app){
  vtkMRMLCommandLineModuleNode *moduleNode = NULL;
  vtkCommandLineModuleLogic *moduleLogic = NULL;

  vtkMRMLScene *scene = this->ChangeTrackerNode->GetScene();
  vtkMRMLChangeTrackerNode *ctNode = this->ChangeTrackerNode;

  assert(ctNode->GetScan1_SuperSampleRef());
  assert(ctNode->GetScan2_SuperSampleRef());


  moduleLogic = vtkCommandLineModuleLogic::New(); 
  moduleNode = 
    static_cast<vtkMRMLCommandLineModuleNode*>(scene->CreateNodeByClass("vtkMRMLCommandLineModuleNode"));
  if(!moduleNode){
    //std::cerr << "Cannot create Rigid registration node. Aborting." << std::endl;
    return -2;
  }

  moduleNode->SetModuleDescription("Rigid registration");

  vtkMRMLScalarVolumeNode *outputNode;
  outputNode = 
    static_cast<vtkMRMLScalarVolumeNode*>(scene->GetNodeByID(ctNode->GetScan2_LocalRef()));
  if(outputNode){
    scene->RemoveNode(outputNode);
    ctNode->SetScan2_LocalRef("");
  }

  char RegVolumeName[255];
  sprintf(RegVolumeName, "%s_VOI_LocalReg", this->GetInputScanName(1));
  outputNode = CreateVolumeNode(static_cast<vtkMRMLVolumeNode*>(scene->GetNodeByID(ctNode->GetScan1_Ref())), 
                                RegVolumeName);

  // Create output transform node
  // TODO: check whether the transform has been created, delete/reuse if yes
  vtkMRMLLinearTransformNode *transformNode =
    vtkMRMLLinearTransformNode::New();
  transformNode->SetName("ROI_LRTransform");
  scene->AddNode(transformNode);
  transformNode->Delete();

  // Linear registration parameter setup
  moduleNode->SetParameterAsString("FixedImageFileName", ctNode->GetScan1_SuperSampleRef());
  moduleNode->SetParameterAsString("MovingImageFileName", ctNode->GetScan2_SuperSampleRef());
  moduleNode->SetParameterAsString("TranslationScale", "10");
  moduleNode->SetParameterAsString("Iterations", "100,100,50,20");
  moduleNode->SetParameterAsString("ResampledImageFileName", outputNode->GetID());
  moduleNode->SetParameterAsString("OutputTransform", transformNode->GetID());

  moduleLogic->SetAndObserveMRMLScene(scene);
  moduleLogic->SetApplicationLogic(app->GetApplicationGUI()->GetApplicationLogic());
  moduleLogic->SetTemporaryDirectory(app->GetTemporaryDirectory());
  moduleLogic->LazyEvaluateModuleTarget(moduleNode);

  ModuleDescription moduleDesc = moduleNode->GetModuleDescription();
  if(moduleDesc.GetTarget() == "Unknown")
    return -4;  // failed to find registration module

  moduleLogic->ApplyAndWait(moduleNode);

  if(moduleNode->GetStatus() != vtkMRMLCommandLineModuleNode::Completed)
    return -5;

  ctNode->SetScan2_LocalRef(outputNode->GetID());
  moduleLogic->SetAndObserveMRMLScene(NULL);
  moduleLogic->Delete();
  moduleNode->Delete();

  return 0;
}

void vtkChangeTrackerLogic::ProcessMRMLEvents(vtkObject* caller, 
  unsigned long event, void *callData){
  vtkMRMLScalarVolumeNode *callerNode = 
    vtkMRMLScalarVolumeNode::SafeDownCast(caller);
  if(callerNode && callerNode == this->Scan2_RegisteredVolume){
//    this->ChangeTrackerNode->SetScan2LinearRegRef(callerNode->GetID());
    this->ChangeTrackerNode->SetScan2_RegisteredReady(true);
  }
}

char* vtkChangeTrackerLogic::GetInputScanName(int scan){
   vtkMRMLVolumeNode *volumeNode;
   if(scan == 0)
     volumeNode = vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_Ref()));
   else
     volumeNode = vtkMRMLVolumeNode::SafeDownCast(this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan2_Ref()));
   return volumeNode->GetName();
}

void vtkChangeTrackerLogic::SetThresholdsFromSegmentation(){
  vtkMRMLVolumeNode *scan1_segm = vtkMRMLVolumeNode::SafeDownCast(
    this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_SegmentRef()));
  vtkMRMLVolumeNode *scan1 = vtkMRMLVolumeNode::SafeDownCast(
    this->ChangeTrackerNode->GetScene()->GetNodeByID(this->ChangeTrackerNode->GetScan1_SuperSampleRef()));
  vtkImageData *image = scan1->GetImageData();
  vtkImageData *mask = scan1_segm->GetImageData();

  std::cerr << "Input image scalar type: " << image->GetScalarType() << std::endl;
  std::cerr << "Input mask scalar type: " << mask->GetScalarType() << std::endl;

  vtkImageMathematics* mult = vtkImageMathematics::New();
  vtkImageThreshold* thresh = vtkImageThreshold::New();
  thresh->SetInput(mask);
  thresh->ThresholdByLower(1);
  thresh->SetInValue(1);
  thresh->SetOutValue(0);
  mult->SetInput(0, image);
  mult->SetInput(1, thresh->GetOutput());
  mult->SetOperationToMultiply();

  vtkImageCast *cast = vtkImageCast::New();
  cast->SetInput(mult->GetOutput());
  cast->SetOutputScalarTypeToShort();
  

  vtkImageAccumulate* hist = vtkImageAccumulate::New();
  hist->SetInput(cast->GetOutput());
  hist->Update();

  double max = hist->GetMax()[0];
  hist->SetComponentOrigin(0.,0.,0.);
  hist->SetComponentExtent(0.,max,0.,0.,0.,0.);
  hist->SetComponentSpacing(1.,0.,0.);
  hist->IgnoreZeroOn();
  hist->Update();

  std::cerr << "Histogram min: " << hist->GetMin()[0] << std::endl;
  std::cerr << "Histogram max: " << hist->GetMax()[0] << std::endl;

  this->ChangeTrackerNode->SetSegmentThresholdMin(hist->GetMin()[0]);
  this->ChangeTrackerNode->SetSegmentThresholdMax(hist->GetMax()[0]);

  mult->Delete();
  thresh->Delete();
  hist->Delete();
  cast->Delete();
}
// AF <<<
