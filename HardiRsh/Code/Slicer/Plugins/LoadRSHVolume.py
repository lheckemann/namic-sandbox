XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>HARDI Modules</category>
  <title>Load an RSH Volume</title>
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

initialized = False


def Initialize():

  os      =__import__('os')
  Slicer  =__import__('Slicer')
  slicer  = Slicer.Slicer()
  sys     =__import__('sys')
  
  #find the library File
  if os.environ.has_key('HARDI_MODULES_DIR'):
    libFile = os.path.join(os.environ['HARDI_MODULES_DIR'],'@LIB_FILENAME@')
  elif os.environ.has_key('Slicer3_MODULES_DIR'):
    libFile = os.path.join(os.environ['Slicer3_MODULES_DIR'],'@LIB_FILENAME@')
  else:
    libFile = os.path.join('@CMAKE_BINARY_DIR@','@Slicer3_INSTALL_PLUGINS_LIB_DIR@','@LIB_FILENAME@')

  if os.path.exists(libFile):
    Slicer.TkCall('load '+libFile)
  else:
    print "Can't find Shared Libary"
    sys.exit(1);

  newNodes = [\
    slicer.vtkMRMLDiffusionRSHVolumeNode() ,\
    slicer.vtkMRMLDiffusionRSHVolumeDisplayNode(),\
    slicer.vtkMRMLDiffusionRSHVolumeSliceDisplayNode(),\
    slicer.vtkMRMLDiffusionRSHDisplayPropertiesNode() ]

  for n in newNodes:
    slicer.MRMLScene.RegisterNodeClass(n)

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
  import nifti
  from numpy import rollaxis,ndindex,median
  import sys


  # load the input
  odf = nifti.NiftiImage(inputFileName)

  #setup the vtkImageData
  id = slicer.vtkImageData()

  #we got a niftiVector image act accordingly
  if (odf.raw_nimg.intent_code>=1000):
    odf_array = odf.getDataArray()[:,0,...]
  else: #we got a 4d nifti file ala odyssee.
    odf_array = odf.getDataArray()

  odf_array = rollaxis( odf_array, 0, 4 )
  id.SetDimensions( odf_array.shape[2], odf_array.shape[1], odf_array.shape[0] )
  id.SetNumberOfScalarComponents(odf_array.shape[3])

  id.SetScalarTypeToDouble()
  id.AllocateScalars()
  id.ToArray()[:,:,:,:]=odf_array[:,:,:,:]

  RASToIJK = slicer.vtkMatrix4x4()
  RASToIJK.Identity()

  qf = odf.getInverseQForm()
  for i in ndindex( qf.shape ):
    RASToIJK.SetElement( i[0], i[1], qf[i] )

  if centerImage:
    RASToIJK.SetElement( 0, 3, odf_array.shape[2]/2.)
    RASToIJK.SetElement( 1, 3, odf_array.shape[1]/2.)
    RASToIJK.SetElement( 2, 3, odf_array.shape[0]/2.)

  qbvn = slicer.vtkMRMLDiffusionRSHVolumeNode()
  qbvn.SetRASToIJKMatrix( RASToIJK )

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

  qbvdn.AddSliceGlyphDisplayNodes( qbvn );
  for i in xrange(1,4):
    qbvn.GetNthDisplayNode(i).SetVisibility(0)
    qbvn.GetNthDisplayNode(i).SetAndObserveColorNodeID(colorNodeID)


