XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>HARDI Modules</category>
  <title>Load an RSH Volume from a nrrd file</title>
  <description>
Load an RSH Volume
</description>
  <version>1.0</version>
  <documentation-url></documentation-url>
  <license></license>
  <contributor>Demian Wassermann + Luke Bloy</contributor>

  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>

    <file>
      <name>inputFileName</name>
      <label>Input RSH File</label>
      <channel>input</channel>
      <description>Input surface to be filtered</description>
      <index>0</index>
    </file>

    <boolean>
      <name>centerImage</name>
      <label>Center the Image?</label>
      <default>false</default>
      <channel>input</channel>
      <index>3</index>
    </boolean>
<!--
    <image>
      <name>outputVolume</name>
      <label>Output RSH Volume</label>
      <channel>output</channel>
      <index>1</index>
      <description>Output filtered surface</description>
    </image>
-->
  </parameters>
</executable>
"""

import teem as Teem
import numpy as np

typemap_numpy2nrrd = {
  np.dtype('byte'): Teem.nrrdTypeChar,
  np.dtype('ubyte'): Teem.nrrdTypeUChar,
  np.dtype('short'): Teem.nrrdTypeShort,
  np.dtype('ushort'): Teem.nrrdTypeUShort,
  np.dtype('int'): Teem.nrrdTypeInt,
  np.dtype('uint'): Teem.nrrdTypeUInt,
  np.dtype('int64'): Teem.nrrdTypeLLong,
  np.dtype('uint64'): Teem.nrrdTypeULLong,
  np.dtype('float32'): Teem.nrrdTypeFloat,
  np.dtype('float64'): Teem.nrrdTypeDouble
  }
#
endian = '<' if Teem.airMyEndian.value == 1234 else '>'
typemap_nrrd2numpy = {
  Teem.nrrdTypeChar: endian + 'i1',
  Teem.nrrdTypeUChar: endian + 'u1',
  Teem.nrrdTypeShort: endian + 'i2',
  Teem.nrrdTypeUShort: endian + 'u2',
  Teem.nrrdTypeInt: endian + 'i4',
  Teem.nrrdTypeUInt: endian + 'u4',
  Teem.nrrdTypeLLong: endian + 'i8',
  Teem.nrrdTypeULLong: endian + 'u8',
  Teem.nrrdTypeFloat: endian + 'f4',
  Teem.nrrdTypeDouble: endian + 'f8'
  }
#
def vec3Finite(vec):
  return (np.isfinite(vec[0])
    and np.isfinite(vec[1])
    and np.isfinite(vec[2]))

class NRRDvol():
  def __init__(self):
    self.nrrd = Teem.nrrdNew()
    self.gctx = Teem.gageContextNew()
  def __del__(self):
    self.nrrd = Teem.nrrdNuke(self.nrrd)
    self.gctx = Teem.gageContextNix(self.gctx)

  # allows data to be accessed by np.asarray(v)
  # http://docs.scipy.org/doc/numpy/reference/arrays.interface.html
  def _get_array_interface(self):
    nrd = self.nrrd.contents
    s = []
    for i in reversed(range(nrd.dim)):
        s.append(nrd.axis[i].size)
    r = {'shape': tuple(s),
         'typestr': typemap_nrrd2numpy[nrd.type],
         'data': (nrd.data, False),
         'version': 3}
    return r
  __array_interface__ = property(_get_array_interface)

  @staticmethod
  def load(fname):
    v = NRRDvol()
    if (Teem.nrrdLoad(v.nrrd, fname, None)):
        err = Teem.biffGetDone('nrrd')
        print "Tenvol: problem loading \'%s\'" % fname, err
        raise IOError
    # these checks are specific to the types of 3-D volumes
    # that we want to be dealing with for Project 4
    nrd = v.nrrd.contents
    if not (3 == nrd.spaceDim
      and vec3Finite(nrd.spaceOrigin)):
      print "Tenvol: didn't get 3-D space origin"
      raise IOError
    if not (vec3Finite(nrd.axis[1].spaceDirection)
            and vec3Finite(nrd.axis[2].spaceDirection)
            and vec3Finite(nrd.axis[3].spaceDirection)):
      print "Tenvol: didn't get three spatially-oriented axes"
      raise IOError

    #Determine IJKtoRAS matrix
    tmp = np.ndarray((4,4))
    tmp.fill(0)
    tmp[0:3,0] = nrd.axis[1].spaceDirection[0:3]
    tmp[0:3,1] = nrd.axis[2].spaceDirection[0:3]
    tmp[0:3,2] = nrd.axis[3].spaceDirection[0:3]
    tmp[0:3,3] = nrd.spaceOrigin[0:3]
    tmp[3,3] = 1

    #currently only holds ijktoworld (defined in the nrrd space field)
    v.ijk2ras = np.matrix(tmp)
    
    if (nrd.space == 1): #RAS
      pass
    elif (nrd.space == 2): #LAS       
      #Flip L to R
      v.ijk2ras[0:3,0] = -v.ijk2ras[0:3,0]
      v.ijk2ras[0,3]   = -v.ijk2ras[0,3]
    elif (nrd.space == 3): #LPS        
      #Flip L to R
      v.ijk2ras[0:3,0] = -v.ijk2ras[0:3,0]
      v.ijk2ras[0,3]   = -v.ijk2ras[0,3]
      #Flip P to A
      v.ijk2ras[0:3,1] = -v.ijk2ras[0:3,1]
      v.ijk2ras[1,3]   = -v.ijk2ras[1,3]
    else:        
      print "Tenvol: don't recongnize the nrrd world coordinates"
      raise IOError

    return v
    
    

initialized = False


def Initialize():

  os      =__import__('os')
  Slicer  =__import__('Slicer')
  slicer  = Slicer.Slicer()
  sys     =__import__('sys')
  
  #find the library File
  libFile = os.path.join('@CMAKE_BINARY_DIR@','@Slicer3_INSTALL_MODULES_LIB_DIR@','@LIB_FILENAME@')
  
  if ( not os.path.exists(libFile) 
     and os.environ.has_key('Slicer3_MODULES_DIR') ):
      libFile = os.path.join(os.environ['Slicer3_MODULES_DIR'],'@LIB_FILENAME@')

  if os.path.exists(libFile):
    Slicer.TkCall('load '+libFile)
  else:
    print "Can't find Shared Libary : "+libFile
    sys.exit(1);

  print "done Loading libraries"

  newNodes = [\
    slicer.vtkMRMLDiffusionRSHVolumeNode() ,\
    slicer.vtkMRMLDiffusionRSHVolumeDisplayNode(),\
    slicer.vtkMRMLDiffusionRSHVolumeSliceDisplayNode(),\
    slicer.vtkMRMLDiffusionRSHDisplayPropertiesNode() ]

  print "starting to Loading libraries"

  for n in newNodes:
    slicer.MRMLScene.RegisterNodeClass(n)

  print "done Registering Nodes"

  RSHColorTable = slicer.vtkMRMLColorTableNode()
  RSHColorTable.SetName('RSHColorScheme')
  RSHColorTable.SetTypeToRainbow()
  RSHColorTable.GetLookupTable().SetHueRange(.667,0)
  slicer.MRMLScene.AddNode(RSHColorTable)
    
def Execute(inputFileName="",  centerImage = False ):


  global initialized

  if not initialized:
    Initialize()

  print "done Initialize"

  from Slicer import slicer
  import numpy as np
  import sys


  # load the input
  rshVol = NRRDvol.load(inputFileName)

  #setup the vtkImageData
  id = slicer.vtkImageData()

  data_array = np.asarray(rshVol)
  id.SetDimensions( data_array.shape[2], data_array.shape[1], data_array.shape[0] )
  id.SetNumberOfScalarComponents(data_array.shape[3])

  id.SetScalarTypeToDouble()
  id.AllocateScalars()
  id.ToArray()[:,:,:,:]=data_array[:,:,:,:]

  #get the ijkToRAS matrix
  RAStoIJK = slicer.vtkMatrix4x4()
  RAStoIJK.Identity()

  ras2ijk_mat = rshVol.ijk2ras.getI();

  for i in np.ndindex( ras2ijk_mat.shape ):
    RAStoIJK.SetElement( i[0], i[1], ras2ijk_mat[i] )

  if centerImage:
    RAStoIJK.SetElement( 0, 3, data_array.shape[2]/2.)
    RAStoIJK.SetElement( 1, 3, data_array.shape[1]/2.)
    RAStoIJK.SetElement( 2, 3, data_array.shape[0]/2.)

  qbvn = slicer.vtkMRMLDiffusionRSHVolumeNode()
  qbvn.SetRASToIJKMatrix( RAStoIJK )

  qbvdn = slicer.vtkMRMLDiffusionRSHVolumeDisplayNode()

  slicer.MRMLScene.AddNode(qbvn)
  slicer.MRMLScene.AddNode(qbvdn)

  qbvn.SetAndObserveDisplayNodeID( qbvdn.GetID() )
  qbvdn.SetAndObserveColorNodeID("vtkMRMLColorTableNodeGrey")
  qbvn.SetAndObserveImageData( id )

  id.Modified()

  qbvdn.UpdateImageDataPipeline()
  qbvdn.AutoWindowLevelOn()

  ColorNodes = slicer.MRMLScene.GetNodesByClassByName( 'vtkMRMLColorTableNode', 'RSHColorScheme' )

  colorNodeID = 'vtkMRMLColorTableNodeRainbow'
  if ColorNodes.GetNumberOfItems()>0:
    colorNodeID = ColorNodes.GetItemAsObject(0).GetID()
  print "qbvdn.AddSliceGlyphDisplayNodes( qbvn );"
  qbvdn.AddSliceGlyphDisplayNodes( qbvn );
  print "after - qbvdn.AddSliceGlyphDisplayNodes( qbvn );"


  for i in xrange(1,4):
    qbvn.GetNthDisplayNode(i).SetVisibility(0)
    qbvn.GetNthDisplayNode(i).SetAndObserveColorNodeID(colorNodeID)
