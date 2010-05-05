XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Diffusion Weighted</category>
  <title>Extract Baseline</title>
  <description>Extract out averaged baseline image.</description>
  <contributor>James Malcolm (malcolm@bwh.harvard.edu)</contributor>

  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>

    <image type="diffusion-weighted">
      <name>dwi_node</name> <channel>input</channel> <index>0</index>
      <label>Input diffusion signal</label>
    </image>
    <image type="scalar">
      <name>b0_node</name> <channel>output</channel> <index>1</index>
      <label>Output averaged baseline</label>
    </image>

  </parameters>
</executable>
"""

from Slicer import slicer

def Execute(dwi_node, b0_node) :

    scene = slicer.MRMLScene
    dwi_node = scene.GetNodeByID(dwi_node)
    b0_node  = scene.GetNodeByID(b0_node)

    b0 = b0_node.GetImageData()
    if not b0 :
        b0 = slicer.vtkImageData() # create if necessary
        b0_node.SetAndObserveImageData(b0)

    b0.SetDimensions(*dwi_node.GetImageData().GetDimensions())
    b0.SetOrigin(0,0,0)
    b0.SetSpacing(1,1,1)
    b0.SetScalarTypeToShort() # int16
    b0.AllocateScalars()

    matrix = slicer.vtkMatrix4x4()
    dwi_node.GetIJKToRASMatrix(matrix)
    b0_node.SetIJKToRASMatrix(matrix)

    # grab diffusion weighted signal and b-values
    S = dwi_node.GetImageData().ToArray()
    b = dwi_node.GetBValues().ToArray().ravel()

    b0 = b0.ToArray()
    b0[:] = S[...,b == 0].mean(-1) # average unweighted images

    b0_node.Modified()
