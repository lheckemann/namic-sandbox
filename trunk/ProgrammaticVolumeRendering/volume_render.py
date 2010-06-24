# initial setup
Slicer = __import__("Slicer")
slicer = Slicer.slicer
scene = slicer.MRMLScene

## there is only one scenario node
##scenario=slicer.vtkMRMLVolumeRenderingScenarioNode()
propertyNode=slicer.vtkMRMLVolumePropertyNode()
parametersNode=slicer.vtkMRMLVolumeRenderingParametersNode()
roiNode=slicer.vtkMRMLROINode()

##scenario.SetParametersNodeID(parameters.GetID())

##scene.AddNode(scenario)

## turning volume rendering on/off
slicer.VolumeRenderingGUI.GetLogic().SetVolumeVisibility(1)

## controlling the opacity mapping
vp=propertyNode.GetVolumeProperty()
##opacity=slicer.vtkPiecewiseFunction()
##color=slicer.vtkColorTransferFunction()

##vp.SetRGBTransferFunction(color)

##opacityStr("2 0 0 500 1")
##property.GetVolumePropertyNode(GetPiecewiseFunctionFromString(opacityStr))

## need to query input data for scalar range

## this is vtkPiecewiseFunction
opacity=vp.GetScalarOpacity()

## this is vtkColorTransferFunction
color=vp.GetRGBTransferFunction()

gradient=vp.GetGradientOpacity()

opacity.RemoveAllPoints()
opacity.AddPoint(-1000,0)
opacity.AddPoint(198,0)
opacity.AddPoint(300,1)
opacity.AddPoint(430,0)

color.RemoveAllPoints()
color.AddRGBPoint(-1000,0, 0, 0)
color.AddRGBPoint( 300, 0.3, 0.9, 0.1)
color.AddRGBPoint( 430, 0.9, 0, 0)

gradient.RemoveAllPoints()
gradient.AddPoint(0, 1)
gradient.AddPoint(357, 1)

vp.SetInterpolationTypeToLinear()
vp.ShadeOn()
vp.SetAmbient(0.30)
vp.SetDiffuse(0.60)
vp.SetSpecular(0.50)
vp.SetSpecularPower(40)

propertyNode.SetModifiedSinceRead(1)

roiNode.InsideOutOn()
roiNode.VisibilityOff()

##set 
scene.AddNode(propertyNode)
scene.AddNode(roiNode)

parametersNode.SetAndObserveVolumeNodeID("vtkMRMLScalarVolumeNode1")
parametersNode.SetAndObserveVolumePropertyNodeID(propertyNode.GetID())
parametersNode.SetAndObserveROINodeID(roiNode.GetID())
parametersNode.SetCurrentVolumeMapper(3)

scene.AddNode(parametersNode)

# controlling the color mapping
