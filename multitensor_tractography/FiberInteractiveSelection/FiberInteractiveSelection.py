FilterName ="Fiber Interactive Selection"
XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>
  <category>Odyssee Modules</category>
  <title>%s</title>
  <description>Fiber Clustering "my method"</description>
  <version>1.0</version>
  <documentation-url></documentation-url>
  <license></license>
  <contributor>Demian Wassermann - INRIA EPI Odyssee</contributor>
  <parameters>
   <label>IO</label>
    <description>Input/output parameters</description>
    <geometry>
      <name>inputFiberBundle</name>
      <longflag>inputFiberBundle</longflag>
      <label>Input Fiber Bundle</label>
      <channel>input</channel>
      <description>Input bundle</description>
    </geometry>
    <geometry>
      <name>outputFiberBundle</name>
      <longflag>outputFiberBundle</longflag>
      <label>Output Fiber Bundle</label>
      <channel>output</channel>
      <description>Clustered bundle</description>
    </geometry>
    <!--<region>
      <name>positiveBox1</name>
      <longflag>positiveBox1</longflag>
      <label>Positive Box 1</label>
    </region>
    <region>
      <name>positiveBox2</name>
      <longflag>positiveBox2</longflag>
      <label>Positive Box 2</label>
    </region>
    <region>
      <name>negativeBox1</name>
      <longflag>negativeBox1</longflag>
      <label>Negative Box 1</label>
    </region>
    <region>
      <name>negativeBox2</name>
      <longflag>negativeBox2</longflag>
      <label>Negative Box 2</label>
    </region>-->
    <integer>
      <name>numberOfPositiveBoxes</name>
      <longflag>numberOfPositiveBoxes</longflag>
      <label>number of positive Boxes</label>
      <default>0</default>
      <constraints>
        <step>1</step>
        <minimum>0</minimum>
        <maximum>2</maximum>
      </constraints>
    </integer>
    <integer>
      <name>numberOfNegativeBoxes</name>
      <longflag>numberOfNegativeBoxes</longflag>
      <label>number of negative Boxes</label>
      <default>0</default>
      <constraints>
        <step>1</step>
        <minimum>0</minimum>
        <maximum>2</maximum>
      </constraints>
    </integer>
    <boolean>
      <name>showBoxes</name>
      <longflag>showBoxes</longflag>
      <label>show boxes</label>
      <default>true</default>
    </boolean>
  </parameters>
</executable>
"""%FilterName
from Slicer import slicer
from Slicer import tk

initialized = False

if 'boxes' not in locals().keys():
  boxes = {}

def Execute (inputFiberBundle = "", outputFiberBundle = "", positiveBox1 = "", positiveBox2 = "", negativeBox1 = "", negativeBox2 = "", numberOfPositiveBoxes=1, numberOfNegativeBoxes=0, showBoxes = True, commandLineModuleNode = ""):
  global initialized
  if not initialized:
    tk.tk.eval('load @LIB_LOCATION@')
    initialized = True

  print inputFiberBundle
  inputFiberBundle = slicer.MRMLScene.GetNodeByID(inputFiberBundle)
  outputFiberBundle = slicer.MRMLScene.GetNodeByID(outputFiberBundle)

  if inputFiberBundle == outputFiberBundle:
    return

  dn = outputFiberBundle.GetDisplayNode()

  if commandLineModuleNode not in boxes.keys():
    boxes[commandLineModuleNode] = FilterState(inputFiberBundle)

  boxesState = boxes[commandLineModuleNode]

  # TODO positiveBox1 does not contain the name of the ROI node but just contains the coordinates and extent of the box. But I definitely need a reference to the node so that I can register observers with it.
  # Currently the module gets called like this: FiberInteractiveSelection --inputFiberBundle vtkMRMLModelNode4 --outputFiberBundle vtkMRMLModelNode5 --positiveBox1 0,0,0,10,10,10 --numberOfPositiveBoxes 0 --numberOfNegativeBoxes 0 --showBoxes true
  print positiveBox1
  box = slicer.MRMLScene.GetNodeByID(positiveBox1)
  if box:
    boxesState.addBox(box, 1)

  while boxesState.getNumberOfPositiveBoxes() < numberOfPositiveBoxes:
    boxesState.addBoxWidget(True)

  while boxesState.getNumberOfPositiveBoxes() > numberOfPositiveBoxes:
    boxesState.removeBoxWidget(True)

  while boxesState.getNumberOfNegativeBoxes() < numberOfNegativeBoxes:
    boxesState.addBoxWidget(False)

  while boxesState.getNumberOfNegativeBoxes() > numberOfNegativeBoxes:
    boxesState.removeBoxWidget(False)

  if showBoxes:
    boxesState.showBoxWidgets()
  else:
    boxesState.hideBoxWidgets()

  outputFiberBundle.SetAndObservePolyData(boxesState.getPolyData())



class FilterState:
  _extractor = None
  _boxWidgets = []
  _inputFiberBundle = None


  def __init__(self, inputFiberBundle):
    self._inputFiberBundle = inputFiberBundle
    self._extractor = slicer.vtkLineBoxLocator()
    self._extractor.SetInput(inputFiberBundle.GetPolyData())
    inputFiberBundle.GetDisplayNode().SetVisibility(0)


  def __del_(self):
    self._extractor.Delete()
    self._boxWidgets = []


  def getPolyData(self):
    self._extractor.Update()
    return self._extractor.GetOutput()


  def getNumberOfBoxes(self):
    return len(self._boxWidgets)


  def getNumberOfPositiveBoxes(self):
    return self._extractor.GetNumberOfPositiveBoxes()


  def getNumberOfNegativeBoxes(self):
    return self._extractor.GetNumberOfNegativeBoxes()


  def addBox(self, box, positive):
    planes = slicer.vtkPlanes()
    planes = box.GetTransformedPlanes(planes)

    if positive:
      boxWidget.GetOutlineProperty().SetColor(1.,1.,1.)
      self._boxWidgets.append((box, planes, self._extractor.AddBox(planes, 1), 1))
    else:
      boxWidget.GetOutlineProperty().SetColor(1.,0.,0.)
      self._boxWidgets.append((box, planes, self._extractor.AddBox(planes, 0), 0))

    procName = self.getProcName(box)
    proc = """proc %s {} {
                %s GetPlanes %s
                %s Update
              }"""

    p1 = proc % (procName, box.GetTclName(), planes.GetTclName(), self._extractor.GetTclName())

    tk.tk.eval(p1)
    tk.tk.eval(Widget.GetTclName() + " AddObserver EndInteractionEvent " + procName)
    tk.tk.eval(Widget.GetTclName() + " AddObserver InteractionEvent " + procName)
    tk.tk.eval(Widget.GetTclName() + ' AddObserver InteractionEvent "[$::slicer3::ApplicationGUI GetActiveViewerWidget] RequestRender"')


  def addBoxWidget(self, positive = True):
    boxWidget = slicer.vtkBoxWidget()
    boxWidget.SetInteractor(slicer.ApplicationGUI.GetActiveRenderWindowInteractor())
    boxWidget.RotationEnabledOff()
    boxWidget.On()

    planes = slicer.vtkPlanes()
    boxWidget.GetPlanes(planes)
    extent = tuple(self._inputFiberBundle.GetPolyData().GetBounds())
    boxWidget.PlaceWidget(*extent)

    if positive:
      boxWidget.GetOutlineProperty().SetColor(1.,1.,1.)
      self._boxWidgets.append((boxWidget, planes, self._extractor.AddBox(planes, 1), 1))
    else:
      boxWidget.GetOutlineProperty().SetColor(1.,0.,0.)
      self._boxWidgets.append((boxWidget, planes, self._extractor.AddBox(planes, 0), 0))
  
    procName = self.getProcName(boxWidget)
    proc = """proc %s {} {
                %s GetPlanes %s
                %s Update
              }"""

    p1 = proc % (procName, boxWidget.GetTclName(), planes.GetTclName(), self._extractor.GetTclName())

    tk.tk.eval(p1)
    tk.tk.eval(boxWidget.GetTclName() + " AddObserver EndInteractionEvent " + procName)
    tk.tk.eval(boxWidget.GetTclName() + " AddObserver InteractionEvent " + procName)
    tk.tk.eval(boxWidget.GetTclName() + ' AddObserver InteractionEvent "[$::slicer3::ApplicationGUI GetActiveViewerWidget] RequestRender"')


  def removeBoxWidget(self, positive = True):
    if len(self._boxWidgets) == 0:
      return

    found = None
    for i in xrange(len(self._boxWidgets) -1, -1, -1):
      if (self._boxWidgets[i][3] == 1) == positive:
        found = i
        break
   
    if found == None:
      return

    boxWidget, planes, i, inside = self._boxWidgets[found]
    self._extractor.RemoveBox(i);

    # TODO this doesn't work somehow.
    #tk.tk.eval('unset ' + self.getProcName(boxWidget))

    #boxWidget.SetInteractor('')
    boxWidget.Off()

    self._boxWidgets = self._boxWidgets[:found] + self._boxWidgets[found + 1:]


  def hideBoxWidgets(self):
    for boxWidget, planes, i, inside in self._boxWidgets:
      boxWidget.OutlineFaceWiresOff()
      boxWidget.OutlineCursorWiresOff()
      boxWidget.HandlesOff()
      boxWidget.Off()


  def showBoxWidgets(self):
    for boxWidget, planes, i, inside in self._boxWidgets:
      boxWidget.OutlineFaceWiresOn()
      boxWidget.OutlineCursorWiresOn()
      boxWidget.HandlesOn()
      boxWidget.On()


  def getProcName(self, box):
    return 'UpdateSelection' + box.GetTclName()
