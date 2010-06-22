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

# need to query input data for scalar range

# this is vtkPiecewiseFunction
set opacity [$vp GetScalarOpacity]    

# this is vtkColorTransferFunction
set color [$vp GetRGBTransferFunction] 

set gradient [$vp GetGradientOpacity]

$opacity RemoveAllPoints
$opacity AddPoint -1000 0
$opacity AddPoint 198 0
$opacity AddPoint 300 1
$opacity AddPoint 430 0

$color RemoveAllPoints
$color AddRGBPoint -1000 0 0 0
$color AddRGBPoint 300 0.3 0.9 0.1
$color AddRGBPoint 430 0.9 0 0

$gradient RemoveAllPoints
$gradient AddPoint 0 1
$gradient AddPoint 357 1

$vp SetInterpolationTypeToLinear
$vp ShadeOn
$vp SetAmbient 0.30
$vp SetDiffuse 0.60
$vp SetSpecular 0.50
$vp SetSpecularPower 40

$propertyNode SetModifiedSinceRead 1

$roiNode InsideOutOn
$roiNode VisibilityOff

#set 
$scene AddNode $propertyNode
$scene AddNode $roiNode

$parametersNode SetAndObserveVolumeNodeID "vtkMRMLScalarVolumeNode1"
$parametersNode SetAndObserveVolumePropertyNodeID [$propertyNode GetID]
$parametersNode SetAndObserveROINodeID [$roiNode GetID]
$parametersNode SetCurrentVolumeMapper 3

$scene AddNode $parametersNode

# controlling the color mapping
