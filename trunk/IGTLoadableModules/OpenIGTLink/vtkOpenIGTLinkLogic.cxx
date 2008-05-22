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

#include "vtkOpenIGTLinkLogic.h"

#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerColorLogic.h"

#include "vtkMultiThreader.h"

#include "vtkIGTLConnector.h"
#include "vtkIGTLCircularBuffer.h"

#include "igtl_image.h"
#include "igtl_transform.h"

vtkCxxRevisionMacro(vtkOpenIGTLinkLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkOpenIGTLinkLogic);

//---------------------------------------------------------------------------
vtkOpenIGTLinkLogic::vtkOpenIGTLinkLogic()
{

  this->SliceDriver0 = vtkOpenIGTLinkLogic::SLICE_DRIVER_USER;
  this->SliceDriver1 = vtkOpenIGTLinkLogic::SLICE_DRIVER_USER;
  this->SliceDriver2 = vtkOpenIGTLinkLogic::SLICE_DRIVER_USER;


  // If the following code doesn't work, slice nodes should be obtained from application GUI
  this->SliceNode0 = NULL;
  this->SliceNode1 = NULL;
  this->SliceNode2 = NULL;

  /*
    this->SliceNode0 = this->GetApplication()->GetApplicationGUI()->GetMainSliceLogic0()->GetSliceNode();
    this->SliceNode1 = this->GetApplication()->GetApplicationGUI()->GetMainSliceLogic1()->GetSliceNode();
    this->SliceNode2 = this->GetApplication()->GetApplicationGUI()->GetMainSliceLogic2()->GetSliceNode();
  */
  /*
    this->Logic0 = appGUI->GetMainSliceGUI0()->GetLogic();
    this->Logic1 = appGUI->GetMainSliceGUI1()->GetLogic();
    this->Logic2 = appGUI->GetMainSliceGUI2()->GetLogic();
  */
                                         
  this->NeedRealtimeImageUpdate0 = 0;
  this->NeedRealtimeImageUpdate1 = 0;
  this->NeedRealtimeImageUpdate2 = 0;

  this->ImagingControl = 0;

#ifdef USE_NAVITRACK
  this->OpenTrackerStream   = vtkOpenIGTLinkDataStream::New();
  this->RealtimeVolumeNode = NULL;
#endif

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkOpenIGTLinkLogic::DataCallback);

#ifdef USE_NAVITRACK
  this->OpenTrackerStream->AddObserver(vtkCommand::ModifiedEvent, this->DataCallbackCommand);
#endif 

  this->ConnectorList.clear();
  this->ConnectorPrevStateList.clear();

}


//---------------------------------------------------------------------------
vtkOpenIGTLinkLogic::~vtkOpenIGTLinkLogic()
{

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

#ifdef USE_NAVITRACK
  if (this->OpenTrackerStream)
    {
    this->OpenTrackerStream->RemoveObservers( vtkCommand::ModifiedEvent, this->DataCallbackCommand );
    this->OpenTrackerStream->Delete();
    }
#endif

}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);

  os << indent << "vtkOpenIGTLinkLogic:             " << this->GetClassName() << "\n";

}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkOpenIGTLinkLogic *self = reinterpret_cast<vtkOpenIGTLinkLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkOpenIGTLinkLogic DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::UpdateAll()
{

}


//---------------------------------------------------------------------------
int vtkOpenIGTLinkLogic::CheckConnectorsStatusUpdates()
{

  //----------------------------------------------------------------
  // Find state change in the connectors

  int nCon = GetNumberOfConnectors();
  int updated = 0;

  for (int i = 0; i < nCon; i ++)
    {
    if (this->ConnectorPrevStateList[i] != this->ConnectorList[i]->GetState())
      {
      updated = 1;
      this->ConnectorPrevStateList[i] = this->ConnectorList[i]->GetState();
      }
    }

  return updated;

}



//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::AddConnector()
{
  vtkIGTLConnector* connector = vtkIGTLConnector::New();
  connector->SetName("connector");
  this->ConnectorList.push_back(connector);
  this->ConnectorPrevStateList.push_back(-1);

  //vtkErrorMacro("Number of Connectors: " << this->ConnectorList.size());
}

//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::DeleteConnector(int id)
{
  if (id >= 0 && id < this->ConnectorList.size())
    {
    this->ConnectorList[id]->Stop();
    this->ConnectorList[id]->Delete();
    this->ConnectorList.erase(this->ConnectorList.begin() + id);
    this->ConnectorPrevStateList.erase(this->ConnectorPrevStateList.begin() + id);
    }
}

//---------------------------------------------------------------------------
int vtkOpenIGTLinkLogic::GetNumberOfConnectors()
{
  return this->ConnectorList.size();
}

vtkIGTLConnector* vtkOpenIGTLinkLogic::GetConnector(int id)
{
  if (id >= 0 && id < GetNumberOfConnectors())
    {
    return this->ConnectorList[id];
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
vtkMRMLVolumeNode* vtkOpenIGTLinkLogic::AddVolumeNode(const char* volumeNodeName)
{

  //vtkErrorMacro("AddVolumeNode(): called.");

  vtkMRMLVolumeNode *volumeNode = NULL;

  if (volumeNode == NULL)  // if real-time volume node has not been created
    {

    //vtkMRMLVolumeDisplayNode *displayNode = NULL;
    vtkMRMLScalarVolumeDisplayNode *displayNode = NULL;
    vtkMRMLScalarVolumeNode *scalarNode = vtkMRMLScalarVolumeNode::New();
    vtkImageData* image = vtkImageData::New();

    float fov = 300.0;
    image->SetDimensions(256, 256, 1);
    image->SetExtent(0, 255, 0, 255, 0, 0 );
    image->SetSpacing( fov/256, fov/256, 10 );
    image->SetOrigin( -fov/2, -fov/2, -0.0 );
    image->SetScalarTypeToShort();
    image->AllocateScalars();
        
    short* dest = (short*) image->GetScalarPointer();
    if (dest)
      {
      memset(dest, 0x00, 256*256*sizeof(short));
      image->Update();
      }
        
    /*
      vtkSlicerSliceLayerLogic *reslice = vtkSlicerSliceLayerLogic::New();
      reslice->SetUseReslice(0);
    */
    scalarNode->SetAndObserveImageData(image);

        
    /* Based on the code in vtkSlicerVolumeLogic::AddHeaderVolume() */
    //displayNode = vtkMRMLVolumeDisplayNode::New();
    displayNode = vtkMRMLScalarVolumeDisplayNode::New();
    scalarNode->SetLabelMap(0);
    volumeNode = scalarNode;
        
    if (volumeNode != NULL)
      {
      volumeNode->SetName(volumeNodeName);
      this->GetMRMLScene()->SaveStateForUndo();
            
      vtkDebugMacro("Setting scene info");
      volumeNode->SetScene(this->GetMRMLScene());
      displayNode->SetScene(this->GetMRMLScene());
            
            
      double range[2];
      vtkDebugMacro("Set basic display info");
      volumeNode->GetImageData()->GetScalarRange(range);
      range[0] = 0.0;
      range[1] = 256.0;
      displayNode->SetLowerThreshold(range[0]);
      displayNode->SetUpperThreshold(range[1]);
      displayNode->SetWindow(range[1] - range[0]);
      displayNode->SetLevel(0.5 * (range[1] - range[0]) );
            
      vtkDebugMacro("Adding node..");
      this->GetMRMLScene()->AddNode(displayNode);
            
      //displayNode->SetDefaultColorMap();
      vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
      displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
      //colorLogic->Delete();
            
      volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());
            
      vtkDebugMacro("Name vol node "<<volumeNode->GetClassName());
      vtkDebugMacro("Display node "<<displayNode->GetClassName());
            
      this->GetMRMLScene()->AddNode(volumeNode);
      vtkDebugMacro("Node added to scene");
      }

    //scalarNode->Delete();
    /*
      if (displayNode)
      {
      displayNode->Delete();
      }
    */

    }
  return volumeNode;
}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::ImportFromCircularBuffers()
{
  ConnectorListType::iterator iter;

  for (iter = this->ConnectorList.begin(); iter != this->ConnectorList.end(); iter ++)
    {
    vtkIGTLConnector::NameListType nameList;
    (*iter)->GetUpdatedBuffersList(nameList);
    vtkIGTLConnector::NameListType::iterator nameIter;
    for (nameIter = nameList.begin(); nameIter != nameList.end(); nameIter ++)
      {
      //vtkErrorMacro("vtkOpenIGTLinkLogic::ImportFromCircularBuffers(): Import Image from : " << *nameIter);
      vtkIGTLCircularBuffer* buffer = (*iter)->GetCircularBuffer(*nameIter);
      buffer->StartPull();
      const char* type = buffer->PullDeviceType();
      //vtkErrorMacro("vtkOpenIGTLinkLogic::ImportFromCircularBuffers(): TYPE = " << type);
      if (strcmp(type, "IMAGE") == 0)
        {
        UpdateMRMLScalarVolumeNode((*nameIter).c_str(), buffer->PullSize(), buffer->PullData());
        }
      else if (strcmp(type, "TRANSFORM") == 0)
        {
        UpdateMRMLLinearTransformNode((*nameIter).c_str(), buffer->PullSize(), buffer->PullData());
        }
      buffer->EndPull();
      }
      
    }
}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::UpdateMRMLScalarVolumeNode(const char* nodeName, int size, unsigned char* data)
{
  //vtkErrorMacro("vtkIGTLConnector::UpdateMRMLScalarVolumeNode()  is called  ");

  if (size < IGTL_IMAGE_HEADER_SIZE)
    {
    //vtkErrorMacro ("Illegal data size" << size << " / " << IGTL_IMAGE_HEADER_SIZE << "\n");
    return;
    }

  igtl_image_header* imgheader = (igtl_image_header*)data;
  unsigned char*     imgdata   = data + IGTL_IMAGE_HEADER_SIZE;

  igtl_image_convert_byte_order(imgheader);
  //vtkErrorMacro("image format version = " << imgheader->version);

  unsigned char imgType = imgheader->data_type;
  unsigned char scalarType = imgheader->scalar_type;

  //vtkErrorMacro("scalar type = " << (int)scalarType);
  //vtkErrorMacro("image type = " << (int)imgType);

  //vtkErrorMacro("size[0] =  " << imgheader->size[0] << ", "
  //              << "size[1] =  " << imgheader->size[1] << ", "
  //              << "size[2] =  " << imgheader->size[2] << ", ");

  //vtkErrorMacro("subvol_size[0] =  " << imgheader->subvol_size[0] << ", "
  //              << "subvol_size[1] =  " << imgheader->subvol_size[1] << ", "
  //              << "subvol_size[2] =  " << imgheader->subvol_size[2] << ", ");

  float tx = imgheader->matrix[0];
  float ty = imgheader->matrix[1];
  float tz = imgheader->matrix[2];
  float sx = imgheader->matrix[3];
  float sy = imgheader->matrix[4];
  float sz = imgheader->matrix[5];
  float nx = imgheader->matrix[6];
  float ny = imgheader->matrix[7];
  float nz = imgheader->matrix[8];
  float px = imgheader->matrix[9];
  float py = imgheader->matrix[10];
  float pz = imgheader->matrix[11];

  //vtkErrorMacro("matrix = ");
  //vtkErrorMacro( << tx << ", " << ty << ", " << tz);
  //vtkErrorMacro( << sx << ", " << sy << ", " << sz);
  //vtkErrorMacro( << nx << ", " << ny << ", " << nz);
  //vtkErrorMacro( << px << ", " << py << ", " << pz);

  vtkMRMLScalarVolumeNode* volumeNode;
  vtkImageData* imageData;

  vtkMRMLScene* scene = this->GetApplicationLogic()->GetMRMLScene();
  vtkCollection* collection = scene->GetNodesByName(nodeName);

  if (collection->GetNumberOfItems() == 0)
    {
    volumeNode = vtkMRMLScalarVolumeNode::New();
    volumeNode->SetName(nodeName);
    volumeNode->SetDescription("Received by OpenIGTLink");

    imageData = vtkImageData::New();

    imageData->SetDimensions(imgheader->size[0], imgheader->size[1], imgheader->size[2]);
    imageData->SetNumberOfScalarComponents(1);
      
    // Scalar type
    //  TBD: Long might not be 32-bit in some platform.
    switch (imgheader->scalar_type)
      {
      case IGTL_IMAGE_STYPE_TYPE_INT8:
        imageData->SetScalarTypeToChar();
        break;
      case IGTL_IMAGE_STYPE_TYPE_UINT8:
        imageData->SetScalarTypeToUnsignedChar();
        break;
      case IGTL_IMAGE_STYPE_TYPE_INT16:
        imageData->SetScalarTypeToShort();
        break;
      case IGTL_IMAGE_STYPE_TYPE_UINT16:
        imageData->SetScalarTypeToUnsignedShort();
        break;
      case IGTL_IMAGE_STYPE_TYPE_INT32:
        imageData->SetScalarTypeToUnsignedLong();
        break;
      case IGTL_IMAGE_STYPE_TYPE_UINT32:
        imageData->SetScalarTypeToUnsignedLong();
        break;
      case IGTL_IMAGE_STYPE_TYPE_FLOAT32:
        imageData->SetScalarTypeToFloat();
        break;
      case IGTL_IMAGE_STYPE_TYPE_FLOAT64:
        imageData->SetScalarTypeToDouble();
        break;
      default:
        //vtkErrorMacro ("Invalid Scalar Type\n");
        break;
      }

    imageData->AllocateScalars();
    volumeNode->SetAndObserveImageData(imageData);
    imageData->Delete();

    scene->AddNode(volumeNode);
    this->GetApplicationLogic()->GetSelectionNode()->SetReferenceActiveVolumeID(volumeNode->GetID());
    this->GetApplicationLogic()->PropagateVolumeSelection();
      
    }
  else
    {
    vtkCollection* collection = scene->GetNodesByName(nodeName);
    volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(collection->GetItemAsObject(0));
    }

  // Get vtk image from MRML node
  imageData = volumeNode->GetImageData();

  
  // TODO:
  // It should be checked here if the dimension of vtkImageData
  // and arrived data is same.

  int bytes = igtl_image_get_data_size(imgheader);

  //vtkErrorMacro("IGTL image size = " << bytes);
  if (imgheader->size[0] == imgheader->subvol_size[0] &&
      imgheader->size[1] == imgheader->subvol_size[1] &&
      imgheader->size[2] == imgheader->subvol_size[2] )
    {
    // In case that volume size == sub-volume size,
    // image is read directly to the memory area of vtkImageData
    // for better performance. 
    memcpy(imageData->GetScalarPointer(), data, bytes);

    }
  else
    {
    // In case of volume size != sub-volume size,
    // image is loaded into ImageReadBuffer, then copied to
    // the memory area of vtkImageData.

    // Check scalar size
    int scalarSize;
    switch (imgheader->scalar_type)
      {
      case IGTL_IMAGE_STYPE_TYPE_INT8:
      case IGTL_IMAGE_STYPE_TYPE_UINT8:
        scalarSize = 1;
        break;
      case IGTL_IMAGE_STYPE_TYPE_INT16:
      case IGTL_IMAGE_STYPE_TYPE_UINT16:
        scalarSize = 2;
        break;
      case IGTL_IMAGE_STYPE_TYPE_INT32:
      case IGTL_IMAGE_STYPE_TYPE_UINT32:
      case IGTL_IMAGE_STYPE_TYPE_FLOAT32:
        scalarSize = 4;
        break;
      case IGTL_IMAGE_STYPE_TYPE_FLOAT64:
        scalarSize = 8;
        break;
      default:
        scalarSize = 0;
        //vtkErrorMacro ("Invalid Scalar Type\n");
        break;
      }
        
    char* imgPtr = (char*) imageData->GetScalarPointer();
    char* bufPtr = (char*) data;
    int sizei = imgheader->size[0];
    int sizej = imgheader->size[1];
    int sizek = imgheader->size[2];
    int subsizei = imgheader->subvol_size[0];

    int bg_i = imgheader->subvol_offset[0];
    int ed_i = bg_i + imgheader->subvol_size[0];
    int bg_j = imgheader->subvol_offset[1];
    int ed_j = bg_j + imgheader->subvol_size[1];
    int bg_k = imgheader->subvol_offset[2];
    int ed_k = bg_k + imgheader->subvol_size[2];
      
    for (int k = bg_k; k < ed_k; k ++)
      {
      for (int j = bg_j; j < ed_j; j ++)
        {
        memcpy(&imgPtr[(sizei*sizej*k + sizei*j + bg_i)*scalarSize],
               bufPtr, subsizei*scalarSize);
        bufPtr += subsizei*scalarSize;
        }
      }
    }

  // normalize
  float psi = sqrt(tx*tx + ty*ty + tz*tz);
  float psj = sqrt(sx*sx + sy*sy + sz*sz);
  float psk = sqrt(nx*nx + ny*ny + nz*nz);

  tx = tx / psi;
  ty = ty / psi;
  tz = tz / psi;
  sx = sx / psj;
  sy = sy / psj;
  sz = sz / psj;
  nx = nx / psk;
  ny = ny / psk;
  nz = nz / psk;


  float hfovi = psi * imgheader->size[0] / 2.0;
  float hfovj = psj * imgheader->size[1] / 2.0;
  float hfovk = psk * imgheader->size[2] / 2.0;

  float cx = tx * hfovi + sx * hfovj + nx * hfovk;
  float cy = ty * hfovi + sy * hfovj + ny * hfovk;
  float cz = tz * hfovi + sz * hfovj + nz * hfovk;

  px = px - cx;
  py = py - cy;
  pz = pz - cz;


  // set volume orientation
  vtkMatrix4x4* rtimgTransform = vtkMatrix4x4::New();
  rtimgTransform->Identity();
  rtimgTransform->SetElement(0, 0, tx);
  rtimgTransform->SetElement(1, 0, ty);
  rtimgTransform->SetElement(2, 0, tz);
  
  rtimgTransform->SetElement(0, 1, sx);
  rtimgTransform->SetElement(1, 1, sy);
  rtimgTransform->SetElement(2, 1, sz);
  
  rtimgTransform->SetElement(0, 2, nx);
  rtimgTransform->SetElement(1, 2, ny);
  rtimgTransform->SetElement(2, 2, nz);

  rtimgTransform->SetElement(0, 3, px);
  rtimgTransform->SetElement(1, 3, py);
  rtimgTransform->SetElement(2, 3, pz);


  rtimgTransform->Invert();
  volumeNode->SetRASToIJKMatrix(rtimgTransform);

//  if (lps) { // LPS coordinate
//    vtkMatrix4x4* lpsToRas = vtkMatrix4x4::New();
//    lpsToRas->Identity();
//    lpsToRas->SetElement(0, 0, -1);
//    lpsToRas->SetElement(1, 1, -1);
//    lpsToRas->Multiply4x4(lpsToRas, rtimgTransform, rtimgTransform);
//    lpsToRas->Delete();
//  }


  px = px + cx;
  py = py + cy;
  pz = pz + cz;


  //volumeNode->SetAndObserveImageData(imageData);
  volumeNode->Modified();
  vtkMRMLSliceNode* slnode = 
    vtkMRMLSliceNode::SafeDownCast(scene->GetNodeByID("vtkMRMLSliceNode1"));
  slnode->SetSliceToRASByNTP(nx, ny, nz, tx, ty, tz, px, py, pz, 0);

}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::UpdateMRMLLinearTransformNode(const char* nodeName, int size, unsigned char* data)
{
  //vtkErrorMacro("vtkIGTLConnector::UpdateMRMLLinearTransformNode()  is called  ");

  if (size != IGTL_TRANSFORM_SIZE)
    {
    //vtkErrorMacro ("Illegal data size" << size << "/" << IGTL_TRANSFORM_SIZE << "\n");
    return;
    }

  float* matrix = reinterpret_cast<float*>(data);
  igtl_transform_convert_byte_order(matrix);

  vtkMRMLLinearTransformNode* transformNode;
  vtkMRMLScene* scene = this->GetApplicationLogic()->GetMRMLScene();
  vtkCollection* collection = scene->GetNodesByName(nodeName);

  if (collection->GetNumberOfItems() == 0)
    {
    transformNode = vtkMRMLLinearTransformNode::New();
    transformNode->SetName(nodeName);
    transformNode->SetDescription("Received by OpenIGTLink");
    
    vtkMatrix4x4* transform = vtkMatrix4x4::New();
    transform->Identity();
    //transformNode->SetAndObserveImageData(transform);
    transformNode->ApplyTransform(transform);
    transform->Delete();
    
    scene->AddNode(transformNode);
    }
  else
    {
    vtkCollection* collection = scene->GetNodesByName(nodeName);
    transformNode = vtkMRMLLinearTransformNode::SafeDownCast(collection->GetItemAsObject(0));
    }
  
  float tx = matrix[0];
  float ty = matrix[1];
  float tz = matrix[2];
  float sx = matrix[3];
  float sy = matrix[4];
  float sz = matrix[5];
  float nx = matrix[6];
  float ny = matrix[7];
  float nz = matrix[8];
  float px = matrix[9];
  float py = matrix[10];
  float pz = matrix[11];

  //vtkErrorMacro("matrix = ");
  //vtkErrorMacro( << tx << ", " << ty << ", " << tz);
  //vtkErrorMacro( << sx << ", " << sy << ", " << sz);
  //vtkErrorMacro( << nx << ", " << ny << ", " << nz);
  //vtkErrorMacro( << px << ", " << py << ", " << pz);
  
  // set volume orientation
  vtkMatrix4x4* transform = vtkMatrix4x4::New();
  vtkMatrix4x4* transformToParent = transformNode->GetMatrixTransformToParent();
  transform->Identity();
  transform->SetElement(0, 0, tx);
  transform->SetElement(1, 0, ty);
  transform->SetElement(2, 0, tz);

  transform->SetElement(0, 1, sx);
  transform->SetElement(1, 1, sy);
  transform->SetElement(2, 1, sz);

  transform->SetElement(0, 2, nx);
  transform->SetElement(1, 2, ny);
  transform->SetElement(2, 2, nz);

  transform->SetElement(0, 3, px);
  transform->SetElement(1, 3, py);
  transform->SetElement(2, 3, pz);

  transformToParent->DeepCopy(transform);
  transform->Delete();

}


//---------------------------------------------------------------------------
void vtkOpenIGTLinkLogic::ProcCommand(const char* nodeName, int size, unsigned char* data)
{
}







