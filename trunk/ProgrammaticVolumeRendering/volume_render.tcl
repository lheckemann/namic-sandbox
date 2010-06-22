# initial setup

set scene $::slicer3::MRMLScene

# there is only one scenario node
#set scenario [vtkMRMLVolumeRenderingScenarioNode New]
set propertyNode [vtkMRMLVolumePropertyNode New]
set parametersNode [vtkMRMLVolumeRenderingParametersNode New]
set roiNode [vtkMRMLROINode New]

#$scenario SetParametersNodeID [$parameters GetID]

#$scene AddNode $scenario

# turning volume rendering on/off
set vrGUI $::slicer3::VolumeRenderingGUI
set vrLogic [$vrGUI GetLogic]

$vrLogic SetVolumeVisibility 1

# controlling the opacity mapping
set vp [$propertyNode GetVolumeProperty]
#set opacity [vtkPiecewiseFunction New]
#set color [vtkColorTransferFunction New]


#$vp SetScalarOpacity $opacity
#$vp SetRGBTransferFunction $color

#set opacityStr "2 0 0 500 1"
#[$property GetVolumePropertyNode] GetPiecewiseFunctionFromString $opacityStr 


# this is vtkPiecewiseFunction
set opacity [$vp GetScalarOpacity]    

# this is vtkColorTransferFunction
set color [$vp GetRGBTransferFunction] 

$opacity RemoveAllPoints
$opacity AddPoint 0 0
$opacity AddPoint 500 1
$opacity AddPoint 510 0

$color RemoveAllPoints
$color AddRGBPoint 0 0 0 0
$color AddRGBPoint 500 100 200 50
$color AddRGBPoint 510 100 0 0

#set 
$scene AddNode $propertyNode
$scene AddNode $roiNode

$parametersNode SetAndObserveVolumeNodeID "vtkMRMLScalarVolumeNode1"
$parametersNode SetAndObserveVolumePropertyNodeID [$propertyNode GetID]
$parametersNode SetAndObserveROINodeID [$roiNode GetID]

$scene AddNode $parametersNode

# controlling the color mapping
