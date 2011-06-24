/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// LiveUltrasound includes
#include "vtkSlicerLiveUltrasoundLogic.h"

// MRML includes

// VTK includes
#include <vtkNew.h>
#include "vtkSmartPointer.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerLiveUltrasoundLogic);

//----------------------------------------------------------------------------
vtkSlicerLiveUltrasoundLogic::vtkSlicerLiveUltrasoundLogic()
{
    this->LiveUltrasoundNode = NULL; 
}

//----------------------------------------------------------------------------
vtkSlicerLiveUltrasoundLogic::~vtkSlicerLiveUltrasoundLogic()
{
    vtkSetAndObserveMRMLNodeMacro(this->LiveUltrasoundNode, NULL);
    if ( this->LiveUltrasoundNode != NULL )
    {
        this->LiveUltrasoundNode->Delete(); 
        this->LiveUltrasoundNode = NULL; 
    } 
}

//----------------------------------------------------------------------------
void vtkSlicerLiveUltrasoundLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerLiveUltrasoundLogic::InitializeEventListeners()
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneClosedEvent);
  this->SetAndObserveMRMLSceneEventsInternal(this->GetMRMLScene(), events.GetPointer());
}

//----------------------------------------------------------------------------
void vtkSlicerLiveUltrasoundLogic::SetAndObserveLiveUltrasoundNode(vtkMRMLLiveUltrasoundNode *node)
{
    if ( node ) 
    {
        vtkSetAndObserveMRMLNodeMacro(this->LiveUltrasoundNode, node);
    }
}

//-----------------------------------------------------------------------------
void vtkSlicerLiveUltrasoundLogic::ProcessMRMLEvents(
  vtkObject* vtkNotUsed(caller), unsigned long vtkNotUsed(event), void * vtkNotUsed(callData))
{
}

//-----------------------------------------------------------------------------
void vtkSlicerLiveUltrasoundLogic::RegisterNodes()
{
 vtkMRMLScene* scene = this->GetMRMLScene(); 
  if (!scene)
    {
    return;
    }
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLLiveUltrasoundNode>::New()); 
}
