XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Diffusion.Utilities</category>
  <title>Normalize Signal</title>
  <description>Divide out baseline image (s0) from signal.</description>
  <contributor>James Malcolm (malcolm@bwh.harvard.edu)</contributor>

  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>

    <image type="diffusion-weighted">
      <name>in_node</name> <channel>input</channel> <index>0</index>
      <label>Input diffusion signal</label>
    </image>
    <image type="diffusion-weighted">
      <name>out_node</name> <channel>output</channel> <index>1</index>
      <label>Output normalized diffusion signal</label>
    </image>

  </parameters>
</executable>
"""

from Slicer import slicer
import numpy as np

def Execute(in_node, out_node) :

    scene = slicer.MRMLScene
    in_node  = scene.GetNodeByID(in_node)
    out_node = scene.GetNodeByID(out_node)

    dwi  = in_node.GetImageData()
    dwi_ = out_node.GetImageData()
    if not dwi_ :
        dwi_ = slicer.vtkImageData() # create if necessary
        out_node.SetAndObserveImageData(dwi_)

    S = in_node.GetImageData().ToArray()
    u = in_node.GetDiffusionGradients().ToArray()
    b = in_node.GetBValues().ToArray().ravel()

    # determine non-null images
    nz = b.flat > 0
    nnz = sum(nz);

    dwi_.SetDimensions(*dwi.GetDimensions())
    dwi_.SetOrigin(0,0,0)
    dwi_.SetSpacing(1,1,1)
    dwi_.SetScalarTypeToFloat() # convert to floating point
    dwi_.SetNumberOfScalarComponents(nnz)
    dwi_.AllocateScalars()

    # divide baseline
    S_ = dwi_.ToArray()
    b0 = S[...,~nz].mean(-1) # average baselines
    S_[:] = S[...,nz] / b0[...,np.newaxis]

    out_node.SetNumberOfGradients(nnz)
    uu = out_node.GetDiffusionGradients().ToArray()
    uu[:] = u[nz,:];
    bb = out_node.GetBValues().ToArray().ravel()
    bb[:] = b[nz];

    m = slicer.vtkMatrix4x4()
    in_node.GetIJKToRASMatrix(m)
    out_node.SetIJKToRASMatrix(m)
    in_node.GetMeasurementFrameMatrix(m)
    out_node.SetMeasurementFrameMatrix(m)

    out_node.Modified()

    # Flush output HACK
    #asdf
