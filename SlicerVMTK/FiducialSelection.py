FilterName ="Python Fiducial Selection"
XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Vessel Segmentation</category>
  <title>%s</title>
  <description>
Module skeleton to select and export fiducials
</description>
  <version>1.0</version>
  <documentation-url>http://www.na-mic.org/Wiki/index.php/2009_Winter_Project_Week_Slicer_VMTK</documentation-url>
  <license>free</license>
  <contributor>Daniel Haehn (University of Heidelberg)</contributor>

  <parameters>
   <label>Step 1</label>
    <description>Create Fiducials</description>

    <integer>
      <name>numberOfFiducials</name>
      <longflag>numberOfFiducials</longflag>
      <label>Number of Fiducials</label>
      <default>5</default>
      <constraints>
        <step>1</step>
        <minimum>1</minimum>
        <maximum>20</maximum>
      </constraints>
    </integer>

    <boolean>
      <name>showFiducials</name>
      <longflag>showFiducials</longflag>
      <label>Show Fiducials</label>
      <default>true</default>
    </boolean>

  </parameters>

  <parameters>
   <label>Step 2</label>
    <description>Export Fiducials</description>

    <boolean>
      <name>exportFiducials</name>
      <longflag>exportFiducials</longflag>
      <label>Export Fiducials</label>
      <default>false</default>
    </boolean>

    <point>
      <name>fidList</name>
      <longflag>fidList</longflag>
      <label>Fiducial List</label>
      <description>Fiducial List</description>
    </point>

</parameters>

</executable>
"""%FilterName
from Slicer import slicer

def Execute (numberOfFiducials=5, showFiducials=True, exportFiducials=False, fidList=""):

    scene = slicer.MRMLScene

    if not exportFiducials:
        fiducialList = scene.CreateNodeByClass("vtkMRMLFiducialListNode")
        fiducialList.DisableModifiedEventOn()
        fiducialList.SetScene(scene)
        for i in range(0,numberOfFiducials):
            fid = fiducialList.AddFiducial()
            fiducialList.SetNthFiducialXYZ(fid, i, i, i)
            fiducialList.SetNthFiducialLabelText(fid,"Pick-a-point %d" % i)
   
        fiducialList.InvokePendingModifiedEvent()
        fiducialList.DisableModifiedEventOff()

        if showFiducials:
            fiducialList.SetAllFiducialsVisibility(1)
        if not showFiducials:
            fiducialList.SetAllFiducialsVisibility(0)

        scene.AddNode(fiducialList)

    if exportFiducials:
# doesnt work
#    print fidList
#    fiducialList = scene.GetNodeByID(fidList)
#    print fiducialList

# only works with static name? why?
        fiducialList = scene.GetNodeByID('vtkMRMLFiducialListNode1')

        for i in range(0,fiducialList.GetNumberOfFiducials()):
            print fiducialList.GetNthFiducialXYZ(i)


