from SlicerScriptedModule import ScriptedModuleGUI
from Slicer import slicer

vtkSlicerNodeSelectorWidget_NodeSelectedEvent = 11000
vtkKWPushButton_InvokedEvent = 10000

class FootPrintGUI(ScriptedModuleGUI):

    def __init__(self):
        ScriptedModuleGUI.__init__(self)
        self.vtkScriptedModuleGUI.SetCategory("FootPrint")
        self.FiducialsNodeSelector = slicer.vtkSlicerNodeSelectorWidget()
        self.TransformNodeSelector = slicer.vtkSlicerNodeSelectorWidget()
        self.ApplyButton = slicer.vtkKWPushButton()
    
    def Destructor(self):
        pass
    
    def RemoveMRMLNodeObservers(self):
        pass
    
    def RemoveLogicObservers(self):
        pass

    def AddGUIObservers(self):
        self.ApplyButtonTag = self.AddObserverByNumber(self.ApplyButton,vtkKWPushButton_InvokedEvent)

    def RemoveGUIObservers(self):
        self.RemoveObserver(self.ApplyButtonTag)

    def ProcessGUIEvents(self,caller,event):
        if caller == self.ApplyButton and event == vtkKWPushButton_InvokedEvent:
            self.Apply()

    def Status(self,text):
        slicer.ApplicationGUI.GetMainSlicerWindow().SetStatusText(text)

    def ErrorDialog(self,text):
        slicer.Application.InformationMessage(text)
        dialog = slicer.vtkKWMessageDialog()
        parent = slicer.ApplicationGUI.GetMainSlicerWindow()
        dialog.SetParent(parent)
        dialog.SetMasterWindow(parent)
        dialog.SetStyleToMessage()
        dialog.SetText(text)
        dialog.Create()
        dialog.Invoke()

    def Apply(self):

        fiducials = self.FiducialsNodeSelector.GetSelected()
        if not fiducials:
            self.ErrorDialog("No fiducials selected")
            return

        transform = self.TransformNodeSelector.GetSelected()
        if not transform:
            self.ErrorDialog("No transform selected")
            return

        self.Status("A fiducial added.")
        
        matrix = transform.GetMatrixTransformToParent()
        x = matrix.GetElement(0, 3)
        y = matrix.GetElement(1, 3)
        z = matrix.GetElement(2, 3)
        fiducials.AddFiducialWithXYZ(x, y, z, 0);

    def BuildGUI(self):
        self.GetUIPanel().AddPage("FootPrint","FootPrint","")
        pageWidget = self.GetUIPanel().GetPageWidget("FootPrint")
        helpText = "FootPrint module written by Junichi Tokuda from Brigham and Women's Hospital."
        aboutText = "This work is supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See http://www.slicer.org for details."
        self.BuildHelpAndAboutFrame(pageWidget,helpText,aboutText)
    
        moduleFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        moduleFrame.SetParent(self.GetUIPanel().GetPageWidget("FootPrint"))
        moduleFrame.Create()
        moduleFrame.SetLabelText("FootPrint")
        moduleFrame.ExpandFrame()
        widgetName = moduleFrame.GetWidgetName()
        pageWidgetName = self.GetUIPanel().GetPageWidget("FootPrint").GetWidgetName()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (widgetName,pageWidgetName))

        self.FiducialsNodeSelector.SetNodeClass("vtkMRMLFiducialListNode","","","")
        self.FiducialsNodeSelector.SetNewNodeEnabled(1)
        self.FiducialsNodeSelector.SetParent(moduleFrame.GetFrame())
        self.FiducialsNodeSelector.Create()
        self.FiducialsNodeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self.FiducialsNodeSelector.UpdateMenu()
        self.FiducialsNodeSelector.SetBorderWidth(2)
        self.FiducialsNodeSelector.SetLabelText("Fiducials: ")
        self.FiducialsNodeSelector.SetBalloonHelpString("select a fiducial list to define control points for the path.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4 -expand true -fill x" % self.FiducialsNodeSelector.GetWidgetName())

        self.TransformNodeSelector.SetNodeClass("vtkMRMLLinearTransformNode","","","")
        self.TransformNodeSelector.SetParent(moduleFrame.GetFrame())
        self.TransformNodeSelector.Create()
        self.TransformNodeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self.TransformNodeSelector.UpdateMenu()
        self.TransformNodeSelector.SetBorderWidth(2)
        self.TransformNodeSelector.SetLabelText("Transform: ")
        self.TransformNodeSelector.SetBalloonHelpString("select a camera that will fly along this path.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4 -expand true -fill x" % self.TransformNodeSelector.GetWidgetName())

        self.ApplyButton.SetParent(moduleFrame.GetFrame())
        self.ApplyButton.Create()
        self.ApplyButton.SetText("Apply")
        self.ApplyButton.SetWidth(8)
        self.ApplyButton.SetBalloonHelpString("create the path and show the fly through controller.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 10" % self.ApplyButton.GetWidgetName())

    def TearDownGUI(self):
        self.FiducialsNodeSelector.SetParent(None)
        self.FiducialsNodeSelector = None
        self.TransformNodeSelector.SetParent(None)
        self.TransformNodeSelector = None
        self.ApplyButton.SetParent(None)
        self.ApplyButton = None
   
        if self.GetUIPanel().GetUserInterfaceManager():
            pageWidget = self.GetUIPanel().GetPageWidget("FootPrint")
            self.GetUIPanel().RemovePage("FootPrint")




"""
fly a slicer camera along a path defined by a fiducial list.
Hermite spline interpolation notation follows:
http://en.wikipedia.org/wiki/Cubic_Hermite_spline

execfile('../Slicer3/Modules/fly.py'); p = path(0.5); p.pathModel(); p.fly()

execfile('../Slicer3/Modules/FootPrint/FootPrintGUI.py'); p = path(0.5); p.pathModel(); p.gui()

import FootPrint; p = FootPrint.path(0.5); p.pathModel(); p.gui()
"""


