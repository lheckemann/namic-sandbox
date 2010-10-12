XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Diffusion Weighted</category>
  <title>General Anisotropy</title>
  <description>Compute generalized anisotropy from diffusion images: rms(signal).</description>
  <contributor>James Malcolm (malcolm@bwh.harvard.edu)</contributor>

  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>

    <image type="diffusion-weighted">
      <name>dwi_node</name> <channel>input</channel> <index>0</index>
      <label>Input diffusion signal</label>
    </image>
    <image type="scalar">
      <name>ga_node</name> <channel>output</channel> <index>1</index>
      <label>Output generalized anisotropy</label>
    </image>

  </parameters>
</executable>
"""

from Slicer import slicer
import numpy as np

def Execute(dwi_node, ga_node) :
    scene = slicer.MRMLScene
    dwi_node = scene.GetNodeByID(dwi_node)
    ga_node  = scene.GetNodeByID(ga_node)

    ga = ga_node.GetImageData()
    if not ga :
        ga = slicer.vtkImageData() # create if necessary
        ga_node.SetAndObserveImageData(ga)

    ga.SetDimensions(*dwi_node.GetImageData().GetDimensions())
    ga.SetOrigin(0,0,0)
    ga.SetSpacing(1,1,1)
    ga.SetScalarTypeToFloat()   # GA is inside [0,1]
    ga.AllocateScalars()

    m = slicer.vtkMatrix4x4()
    dwi_node.GetIJKToRASMatrix(m)
    ga_node.SetIJKToRASMatrix(m)

    # calculate E{X} and E{X^2}
    S  = dwi_node.GetImageData().ToArray().astype('float')
    mu = S.mean(-1)
    mu_sq = np.square(S).mean(-1)

    ga = ga.ToArray()
    ga[:] = np.sqrt(mu_sq - np.square(mu)) / np.sqrt(mu_sq) # rms

    ga_node.Modified()

#     print ga[28,72,72]
#     asdf
