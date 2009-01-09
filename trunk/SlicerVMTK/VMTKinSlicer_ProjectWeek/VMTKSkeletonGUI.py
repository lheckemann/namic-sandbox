from SlicerScriptedModule import ScriptedModuleGUI
from Slicer import slicer

vtkKWScale_ScaleValueChangedEvent = 10001
vtkKWScale_ScaleValueStartChangingEvent = 10002
vtkSlicerNodeSelectorWidget_NodeSelectedEvent = 11000
vtkKWPushButton_InvokedEvent = 10000

class VMTKSkeletonGUI(ScriptedModuleGUI):

    def __init__(self):

        ScriptedModuleGUI.__init__(self)
        self.VmtkNodeSelector = slicer.vtkSlicerNodeSelectorWidget()
        self.VolumeSelector = slicer.vtkSlicerNodeSelectorWidget()
        self.OutVolumeSelector = slicer.vtkSlicerNodeSelectorWidget()

        # switch for interactive mode
        self._isInteractiveMode = 0

        # pointer to current FiducialList
        self._currentFiducialList = None
        
        #
        self.fiducialListTarget = None
        self.fiducialListSource = None

        # list of different init and evolve methods
        self.Pages = []

    
    def Destructor(self):
        pass

    ###
    ### debug prints to stdout (better than print because of flush)
    ###
    def debug(self,msg):

        debugMode = 1

        if debugMode:
            print msg
            import sys
            sys.stdout.flush()


    def RemoveMRMLNodeObservers(self):
        pass
    
    def RemoveLogicObservers(self):
        pass

    def AddGUIObservers(self):
        self.VmtkNodeSelectorSelectedTag = self.AddObserverByNumber(self.VmtkNodeSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)
        self.VolumeSelectorSelectedTag = self.AddObserverByNumber(self.VolumeSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)
        self.OutVolumeSelectorSelectedTag = self.AddObserverByNumber(self.OutVolumeSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)
        
        for page in self.Pages:
            page.AddGUIObservers(self)

        self.interactor = slicer.ApplicationGUI.GetRenderWindowInteractor()
        self.LeftButtonReleaseTag = self.interactor.AddObserver("LeftButtonReleaseEvent",self.HandleClickInRenderWindow)

        self.redSliceInteractor = slicer.ApplicationGUI.GetMainSliceGUI("Red").GetSliceViewer().GetRenderWidget().GetRenderWindowInteractor()
        self.SliceLeftButtonReleaseTag = self.redSliceInteractor.AddObserver("LeftButtonReleaseEvent",self.HandleClickInRedSliceWindow)

        self.yellowSliceInteractor = slicer.ApplicationGUI.GetMainSliceGUI("Yellow").GetSliceViewer().GetRenderWidget().GetRenderWindowInteractor()
        self.SliceLeftButtonReleaseTag = self.yellowSliceInteractor.AddObserver("LeftButtonReleaseEvent",self.HandleClickInYellowSliceWindow)

        self.greenSliceInteractor = slicer.ApplicationGUI.GetMainSliceGUI("Green").GetSliceViewer().GetRenderWidget().GetRenderWindowInteractor()
        self.SliceLeftButtonReleaseTag = self.greenSliceInteractor.AddObserver("LeftButtonReleaseEvent",self.HandleClickInGreenSliceWindow)


    def RemoveGUIObservers(self):
        self.RemoveObserver(self.VmtkNodeSelectorSelectedTag)
        self.RemoveObserver(self.VolumeSelectorSelectedTag)
        self.RemoveObserver(self.OutVolumeSelectorSelectedTag)        

        for page in self.Pages:
            page.RemoveGUIObservers(self)


    def ProcessGUIEvents(self,caller,event):
        if caller == self.VmtkNodeSelector and event == vtkSlicerNodeSelectorWidget_NodeSelectedEvent and self.VmtkNodeSelector.GetSelected():
            node = self.VmtkNodeSelector.GetSelected()
            self.GetLogic().SetAndObserveScriptedModuleNode(node)
            self.SetAndObserveScriptedModuleNode(node)
            self.UpdateGUI()
        elif caller == self.VolumeSelector and event == vtkSlicerNodeSelectorWidget_NodeSelectedEvent and self.VolumeSelector.GetSelected():
            self.UpdateMRML()
        elif caller == self.OutVolumeSelector and event == vtkSlicerNodeSelectorWidget_NodeSelectedEvent and self.OutVolumeSelector.GetSelected(): 
            self.UpdateMRML()

        for page in self.Pages:
            page.ProcessGUIEvents(caller,event,self)

    ###
    ### click handlers for the 3D render window
    ###
    def HandleClickInRenderWindow(self):

        self.debug("Clicked in 3D")

        if self._isInteractiveMode:
            coordinates = self.interactor.GetLastEventPosition()
            self.debug(coordinates[0])
            self.debug(coordinates[1])


    ###
    ### click handlers for the slices
    ###
    def HandleClickInRedSliceWindow(self):
        self.HandleClickInSliceWindow("Red",self.redSliceInteractor)

    def HandleClickInYellowSliceWindow(self):
        self.HandleClickInSliceWindow("Yellow",self.yellowSliceInteractor)

    def HandleClickInGreenSliceWindow(self):
        self.HandleClickInSliceWindow("Green",self.greenSliceInteractor)

    def HandleClickInSliceWindow(self,which,interactor):

        self.debug("Clicked on " +which + "Slice")

        if self._isInteractiveMode:
            coordinates = interactor.GetLastEventPosition()

            self.debug(coordinates[0])
            self.debug(coordinates[1])

            coordinatesRAS = self.ConvertCoordinates2RAS(which,coordinates)
            coordinatesIJK = self.ConvertCoordinates2IJK(which,coordinates)

            self.debug(coordinatesRAS)
            self.debug(coordinatesIJK)

            fiducial = self._currentFiducialList.AddFiducialWithXYZ(coordinatesRAS[0], coordinatesRAS[1], coordinatesRAS[2],0)



    ###
    ### convert coordinates to RAS
    ###
    def ConvertCoordinates2RAS(self,which,coordinates):
        # convert to RAS
        inPt = [coordinates[0], coordinates[1], 0, 1]
        matrixRAS = slicer.ApplicationGUI.GetMainSliceGUI(which).GetLogic().GetSliceNode().GetXYToRAS()
        rasPt = matrixRAS.MultiplyPoint(*inPt)
        return rasPt

    ###
    ### convert coordinates to IJK
    ###
    def ConvertCoordinates2IJK(self,which,coordinates):
        #need RAS
        rasPt = self.ConvertCoordinates2RAS(which,coordinates)
        print "A", rasPt

        #convert to IJK
        node = self.GetScriptedModuleNode()
        if node:
            inVolume = node.GetParameter('InputVolumeRef')
            if not inVolume:
                slicer.Application.ErrorMessage("No input volume found\n")
                return
            else:
                matrixIJK = slicer.vtkMatrix4x4()
                inVolume.GetRASToIJKMatrix(matrixIJK)
                ijkPt = matrixIJK.MultiplyPoint(*rasPt)
                return ijkPt
            
    ###
    ### convert coordinates to IJK
    ###
    def ConvertRAS2IJK(self,coordinates):

        rasPt = coordinates
        print "B", rasPt
        rasPt.append(1)

        #convert to IJK
        node = self.GetScriptedModuleNode()
        if node:
            inVolume = node.GetParameter('InputVolumeRef')
            if not inVolume:
                slicer.Application.ErrorMessage("No input volume found\n")
                return
            else:
                matrixIJK = slicer.vtkMatrix4x4()
                inVolume.GetRASToIJKMatrix(matrixIJK)
                self.debug(matrixIJK)
                ijkPt = matrixIJK.MultiplyPoint(*rasPt)
                return ijkPt
            


    def UpdateMRML(self):
        node = self.GetScriptedModuleNode()
        if not node:
            self.VmtkNodeSelector.SetSelectedNew("vtkMRMLScriptedModuleNode")
            self.VmtkNodeSelector.ProcessNewNodeCommand("vtkMRMLScriptedModuleNode", "VMTKParameters")
            node = self.VmtkNodeSelector.GetSelected()
            self.GetLogic().SetAndObserveScriptedModuleNode(node)
            self.SetScriptedModuleNode(node)

        if self.VolumeSelector.GetSelected():
            node.SetParameter('InputVolumeRef',self.VolumeSelector.GetSelected())

        if self.OutVolumeSelector.GetSelected():
            node.SetParameter('OutputVolumeRef',self.OutVolumeSelector.GetSelected())

        self.GetLogic().GetMRMLScene().SaveStateForUndo(node)     

    def UpdateGUI(self):
        node = self.GetScriptedModuleNode()

    def ProcessMRMLEvents(self,caller,event):
        if caller == self.GetScriptedModuleNode():
            self.UpdateGUI()

    def BuildGUI(self):
        self.GetUIPanel().AddPage("VMTKStart","VMTK Start","")
        pageWidget = self.GetUIPanel().GetPageWidget("VMTKStart")
        helpText = "Flex, dude!"
        aboutText = "This work is supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See http://www.slicer.org for details."
        self.BuildHelpAndAboutFrame(pageWidget,helpText,aboutText)

    
        self.moduleFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self.moduleFrame.SetParent(self.GetUIPanel().GetPageWidget("VMTKStart"))
        self.moduleFrame.Create()
        self.moduleFrame.SetLabelText("VMTK")
        self.moduleFrame.ExpandFrame()
        widgetName = self.moduleFrame.GetWidgetName()
        pageWidgetName = self.GetUIPanel().GetPageWidget("VMTKStart").GetWidgetName()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (widgetName,pageWidgetName))
    
        self.VmtkNodeSelector.SetNodeClass("vtkMRMLScriptedModuleNode", "ScriptedModuleName", self.GetLogic().GetModuleName(), "VMTKParameters")
        self.VmtkNodeSelector.NewNodeEnabledOn()
        self.VmtkNodeSelector.NoneEnabledOn()
        self.VmtkNodeSelector.ShowHiddenOn()
        self.VmtkNodeSelector.SetParent(self.moduleFrame.GetFrame())
        self.VmtkNodeSelector.Create()
        self.VmtkNodeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self.VmtkNodeSelector.UpdateMenu()
        self.VmtkNodeSelector.SetBorderWidth(2)
        self.VmtkNodeSelector.SetLabelText("VMTK Parameters")
        self.VmtkNodeSelector.SetBalloonHelpString("select a VMTK node from the current mrml scene.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self.VmtkNodeSelector.GetWidgetName())

        self.VolumeSelector.SetNodeClass("vtkMRMLScalarVolumeNode","","","")
        self.VolumeSelector.SetParent(self.moduleFrame.GetFrame())
        self.VolumeSelector.Create()
        self.VolumeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self.VolumeSelector.UpdateMenu()
        self.VolumeSelector.SetBorderWidth(2)
        self.VolumeSelector.SetLabelText("Input Volume:")
        self.VolumeSelector.SetBalloonHelpString("select an input volume from the current mrml scene.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self.VolumeSelector.GetWidgetName())

        self.OutVolumeSelector.SetNodeClass("vtkMRMLScalarVolumeNode","","","GADVolumeOut")
        self.OutVolumeSelector.SetNewNodeEnabled(1)
        self.OutVolumeSelector.SetParent(self.moduleFrame.GetFrame())
        self.OutVolumeSelector.Create()
        self.OutVolumeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self.OutVolumeSelector.UpdateMenu()
        self.OutVolumeSelector.SetBorderWidth(2)
        self.OutVolumeSelector.SetLabelText("Output Volume: ")
        self.OutVolumeSelector.SetBalloonHelpString("select an output volume from the current mrml scene.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self.OutVolumeSelector.GetWidgetName())

        # initialization tabs start here
        self.vmtkInitFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self.vmtkInitFrame.SetParent(self.GetUIPanel().GetPageWidget("VMTKStart"))
        self.vmtkInitFrame.Create()
        self.vmtkInitFrame.SetLabelText("Initialization")
        self.vmtkInitFrame.ExpandFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (self.vmtkInitFrame.GetWidgetName(),pageWidgetName))

        self.vmtkInitTabs = slicer.vtkKWNotebook()
        self.vmtkInitTabs.SetParent(self.vmtkInitFrame.GetFrame())
        self.vmtkInitTabs.Create()

        self.vmtkInitTabs.AddPage("Colliding Fronts","Hint for colliding fronts","")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self.vmtkInitTabs.GetWidgetName())
        id = self.vmtkInitTabs.GetFrame("Colliding Fronts")
        self.collidingFrontsPanel = VMTKCollidingFronts(id)
        self.collidingFrontsPanel.BuildGUI()
        self.Pages.append(self.collidingFrontsPanel)

        self.vmtkInitTabs.AddPage("Fast Marching","Hint for fast marching","")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self.vmtkInitTabs.GetWidgetName())
        id = self.vmtkInitTabs.GetFrame("Fast Marching")
        self.fastMarchingPanel = VMTKFastMarching(id)
        self.fastMarchingPanel.BuildGUI()
        self.Pages.append(self.fastMarchingPanel)

        self.debug(self.Pages)
        self.debug(self._isInteractiveMode)

    ###
    ### de/activate all tabs when in interactive mode
    ###
    def UnLockTabs(self,action):

        if action=="lock":
            self.vmtkInitTabs.SetEnabled(0)
        else:
            self.vmtkInitTabs.SetEnabled(1)

   
    def StartFiducialList(self,which):

        # create fiducialList
        scene = self.GetLogic().GetMRMLScene()
        #self.debug("1")
        #self.fiducialListLogic = slicer.vtkSlicerFiducialsLogic()
        #self.debug("2")
        #self.fiducialList = self.fiducialListLogic.AddFiducialList()

        self.debug(which)
        self.debug("AAAAAAAAAAAAAA")

        if which==1:
            self.fiducialListSource = scene.CreateNodeByClass("vtkMRMLFiducialListNode")
            self.fiducialListSource.SetScene(scene)
            scene.AddNode(self.fiducialListSource)
            self._currentFiducialList = self.fiducialListSource
            self.debug("Source selected")
        elif which==2:
            self.fiducialListTarget = scene.CreateNodeByClass("vtkMRMLFiducialListNode")
            self.fiducialListTarget.SetScene(scene)
            scene.AddNode(self.fiducialListTarget)
            self._currentFiducialList = self.fiducialListTarget
            self.debug("Target selected")
                    

        self.debug("start FIDLIST")
            
    def EndFiducialList(self,which):

        #scene = self.GetLogic().GetMRMLScene()
        pass

    def TearDownGUI(self):
        self.VmtkNodeSelector.SetParent(None)
        self.VmtkNodeSelector = None
        self.vmtkEvolveFrame.SetParent(None)
        self.vmtkEvolveFrame = None
        self.vmtkInitFrame.SetParent(None)
        self.vmtkInitFrame = None
        self.vmtkEvolveTabs.SetParent(None)
        self.vmtkEvolveTabs = None
        self.vmtkInitTabs.SetParent(None)
        self.vmtkInitTabs = None

        if self.GetUIPanel().GetUserInterfaceManager():
            pageWidget = self.GetUIPanel().GetPageWidget("VMTKStart")
            self.GetUIPanel().RemovePage("VMTKStart")



###
### skeleton class for each page
###
class VMTKSkeletonPage(object):

    def __init__(self,parentFrame):
        self._parentFrame = parentFrame
        # components for all tabs go here
        
    def Destructor(self):
        pass

    def BuildGUI(self):
        pass
    
    def AddGUIObservers(self,parentClass):
        pass

    def RemoveGUIObservers(self,parentClass):
        pass

    def ProcessGUIEvents(self,caller,event,parentClass):
        pass

###
### colliding fronts initialization page (derived from skeleton)
###
class VMTKCollidingFronts(VMTKSkeletonPage):

    def __init__(self,parentFrame):
        VMTKSkeletonPage.__init__(self,parentFrame)


        self._interactButton = slicer.vtkKWPushButton()
        self._interactButton.SetParent(self._parentFrame)
        self._interactButton.Create()
        self._interactButton.SetActiveBackgroundColor(0.9,0.9,0.9)
        self._interactButton.SetReliefToRaised()
        self._interactButton.SetBackgroundColor(0.9,0.9,0.9)
        self._interactButton.SetText("Activate")
        self._interactButton.SetWidth(8)
        self._interactButton.SetBalloonHelpString("Click to enter interactive mode")

    def BuildGUI(self):
        VMTKSkeletonPage.BuildGUI(self)
        slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -in %s" % (self._interactButton.GetWidgetName(), self._parentFrame.GetWidgetName()))

    def AddGUIObservers(self,parentClass):
        VMTKSkeletonPage.AddGUIObservers(self,parentClass)
        self._interactButtonTag = parentClass.AddObserverByNumber(self._interactButton,vtkKWPushButton_InvokedEvent)            

    def RemoveGUIObservers(self,parentClass):
        VMTKSkeletonPage.RemoveGUIObservers(self,parentClass)
        parentClass.RemoveObserver(self._interactButtonTag)

    def ProcessGUIEvents(self,caller,event,parentClass):
        VMTKSkeletonPage.ProcessGUIEvents(self,caller,event,parentClass)

        if caller == self._interactButton and event == vtkKWPushButton_InvokedEvent:

            if not parentClass._isInteractiveMode:

                self._interactButton.SetText("Activated!")
                self._interactButton.SetWidth(8)
                self._interactButton.SetReliefToSunken()
                self._interactButton.SetActiveBackgroundColor(0.9,0.2,0.2)
                self._interactButton.SetBackgroundColor(0.9,0.2,0.2)
                self._interactButton.SetBalloonHelpString("Click to leave interactive mode")
                parentClass._isInteractiveMode = 1
                parentClass.UnLockTabs("lock")
                parentClass.StartFiducialList(1)

            elif parentClass._isInteractiveMode:

                self._interactButton.SetText("Activate")
                self._interactButton.SetWidth(8)
                self._interactButton.SetReliefToRaised()
                self._interactButton.SetActiveBackgroundColor(0.9,0.9,0.9)
                self._interactButton.SetBackgroundColor(0.9,0.9,0.9)
                self._interactButton.SetBalloonHelpString("Click to enter interactive mode")
                parentClass._isInteractiveMode = 0
                parentClass.UnLockTabs("unlock")
                parentClass.EndFiducialList(1)

###
### fast marching initialization page (derived from skeleton)
###
class VMTKFastMarching(VMTKSkeletonPage):

    def __init__(self,parentFrame):
        VMTKSkeletonPage.__init__(self,parentFrame)


        #self._choosePointTypeSource = slicer.vtkKWRadioButton()
        #self._choosePointTypeSource.SetParent(self._parentFrame)
        #self._choosePointTypeSource.Create()
        #self._choosePointTypeSource.SetText("Source points")
        #self._choosePointTypeSource.SetValueAsInt(1)

        #self._choosePointTypeTarget = slicer.vtkKWRadioButton()
        #self._choosePointTypeTarget.SetParent(self._parentFrame)
        #self._choosePointTypeTarget.Create()
        #self._choosePointTypeTarget.SetText("Target points")
        #self._choosePointTypeTarget.SetValueAsInt(2)


        #self._choosePointTypeSource.SetVariableName(self._choosePointTypeTarget.GetVariableName())

        #self._choosePointTypeSource.SetSelectedState(1)
        #self._choosePointTypeTarget.SetSelectedState(0)

        self._choosePointType = slicer.vtkKWRadioButtonSet()
        self._choosePointType.SetParent(self._parentFrame)
        self._choosePointType.Create()
        self._choosePointType.SetBorderWidth(2)
        self._choosePointType.SetReliefToGroove()
        self.choosePointTypeSource = self._choosePointType.AddWidget(0)
        self.choosePointTypeSource.SetText("Source points")
        self.choosePointTypeSource.SetValueAsInt(1)        
        self.choosePointTypeTarget = self._choosePointType.AddWidget(1)
        self.choosePointTypeTarget.SetText("Target points")
        self.choosePointTypeTarget.SetValueAsInt(2)
        self.choosePointTypeTarget.SetVariableName(self._choosePointType.GetWidget(0).GetVariableName())

        self._choosePointType.GetWidget(0).SetSelectedState(1)        

        self._interactButton = slicer.vtkKWPushButton()
        self._interactButton.SetParent(self._parentFrame)
        self._interactButton.Create()
        self._interactButton.SetActiveBackgroundColor(0.9,0.9,0.9)
        self._interactButton.SetReliefToRaised()
        self._interactButton.SetBackgroundColor(0.9,0.9,0.9)
        self._interactButton.SetText("Choose Points")
        self._interactButton.SetWidth(15)
        self._interactButton.SetBalloonHelpString("Click to enter interactive mode")


        self._lowerThresholdEntry = slicer.vtkKWEntryWithLabel()
        self._lowerThresholdEntry.SetParent(self._parentFrame)
        self._lowerThresholdEntry.SetLabelText("Lower Threshold:")
        self._lowerThresholdEntry.Create()
        #self._lowerThresholdEntry.SetValue("100")


        self._startButton = slicer.vtkKWPushButton()
        self._startButton.SetParent(self._parentFrame)
        self._startButton.Create()
        self._startButton.SetEnabled(0)
        self._startButton.SetActiveBackgroundColor(0.9,0.9,0.9)
        self._startButton.SetReliefToRaised()
        self._startButton.SetBackgroundColor(0.9,0.9,0.9)
        self._startButton.SetText("Start!")
        self._startButton.SetWidth(8)
        self._startButton.SetBalloonHelpString("Click to start")

    def BuildGUI(self):
        VMTKSkeletonPage.BuildGUI(self)
        slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -in %s" % (self._choosePointType.GetWidgetName(), self._parentFrame.GetWidgetName()))
        #slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -in %s" % (self._choosePointTypeTarget.GetWidgetName(), self._parentFrame.GetWidgetName()))
        slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -in %s" % (self._interactButton.GetWidgetName(), self._parentFrame.GetWidgetName()))
        slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 2 -in %s" % (self._lowerThresholdEntry.GetWidgetName(), self._parentFrame.GetWidgetName()))
        slicer.TkCall("pack %s -side right -anchor nw -expand n -padx 2 -pady 2 -in %s" % (self._startButton.GetWidgetName(), self._parentFrame.GetWidgetName()))

    def AddGUIObservers(self,parentClass):
        VMTKSkeletonPage.AddGUIObservers(self,parentClass)
        self._interactButtonTag = parentClass.AddObserverByNumber(self._interactButton,vtkKWPushButton_InvokedEvent)            
        self._startButtonTag = parentClass.AddObserverByNumber(self._startButton,vtkKWPushButton_InvokedEvent)            

    def RemoveGUIObservers(self,parentClass):
        VMTKSkeletonPage.RemoveGUIObservers(self,parentClass)
        parentClass.RemoveObserver(self._interactButtonTag)
        parentClass.RemoveObserver(self._startButtonTag)

    def ProcessGUIEvents(self,caller,event,parentClass):
        VMTKSkeletonPage.ProcessGUIEvents(self,caller,event,parentClass)

        # event handler for interact button
        if caller == self._interactButton and event == vtkKWPushButton_InvokedEvent:

            if not parentClass._isInteractiveMode:

                self._interactButton.SetText("Activated!")
                self._interactButton.SetWidth(15)
                self._interactButton.SetReliefToSunken()
                self._interactButton.SetActiveBackgroundColor(0.9,0.2,0.2)
                self._interactButton.SetBackgroundColor(0.9,0.2,0.2)
                self._interactButton.SetBalloonHelpString("Click to leave interactive mode")
                parentClass._isInteractiveMode = 1
                parentClass.UnLockTabs("lock")
                self._startButton.SetEnabled(0)

                parentClass.debug(self._choosePointType.GetWidget(0).GetVariableValue())

                parentClass.StartFiducialList(self._choosePointType.GetWidget(0).GetVariableValue())

            elif parentClass._isInteractiveMode:

                self._interactButton.SetText("Choose points")
                self._interactButton.SetWidth(15)
                self._interactButton.SetReliefToRaised()
                self._interactButton.SetActiveBackgroundColor(0.9,0.9,0.9)
                self._interactButton.SetBackgroundColor(0.9,0.9,0.9)
                self._interactButton.SetBalloonHelpString("Click to enter interactive mode")
                parentClass._isInteractiveMode = 0
                parentClass.UnLockTabs("unlock")
                self._startButton.SetEnabled(1)
                parentClass.EndFiducialList(self._choosePointType.GetWidget(0).GetVariableValue())

        # event handler for apply button
        elif caller == self._startButton and event == vtkKWPushButton_InvokedEvent:

            parentClass.debug("STARTING..")

            sourceSeedIds = slicer.vtkIdList()
            targetSeedIds = slicer.vtkIdList()

            sourceSeeds = parentClass.fiducialListSource
            targetSeeds = parentClass.fiducialListTarget

            node = parentClass.GetScriptedModuleNode()
            parentClass.debug(node)
            if node:
                inVolume = node.GetParameter('InputVolumeRef')
                outVolume = node.GetParameter('OutputVolumeRef')
                if not inVolume:
                    slicer.Application.ErrorMessage("No input volume found\n")
                    return
                else:

                    if not outVolume:
                        slicer.Application.ErrorMessage("No output volume found")
                        return

                    image = inVolume.GetImageData()

                    for i in range(sourceSeeds.GetNumberOfFiducials()):
                        rasPt = sourceSeeds.GetNthFiducialXYZ(i)
                        parentClass.debug("SOURCE R A S!!!!!")
                        parentClass.debug(rasPt)
                        ijkPt = parentClass.ConvertRAS2IJK(rasPt)
                        parentClass.debug("SOURCE I J K!!!!!")
                        parentClass.debug(ijkPt)
                        sourceSeedIds.InsertNextId(image.ComputePointId(int(ijkPt[0]),int(ijkPt[1]),int(ijkPt[2])))

                    #parentClass.debug(sourceSeedIds)

                    #parentClass.debug(targetSeeds)

                    for i in range(targetSeeds.GetNumberOfFiducials()):
                        rasPt = targetSeeds.GetNthFiducialXYZ(i)
                        parentClass.debug("TARGET R A S!!!!!")
                        parentClass.debug(rasPt)
                        ijkPt = parentClass.ConvertRAS2IJK(rasPt)
                        parentClass.debug("TARGET I J K!!!!!")
                        parentClass.debug(ijkPt)
                        targetSeedIds.InsertNextId(image.ComputePointId(int(ijkPt[0]),int(ijkPt[1]),int(ijkPt[2])))

                    #parentClass.debug(targetSeedIds)

                    scalarRange = image.GetScalarRange()

                    threshold = slicer.vtkImageThreshold()
                    threshold.SetInput(image)

                    threshold.ThresholdByLower(self._lowerThresholdEntry.GetWidget().GetValue())

                    threshold.ReplaceInOff()
                    threshold.ReplaceOutOn()
                    threshold.SetOutValue(scalarRange[0] - scalarRange[1])
                    threshold.Update()

                    thresholdedImage = threshold.GetOutput()

                    shiftScale = slicer.vtkImageShiftScale()
                    shiftScale.SetInput(thresholdedImage)
                    shiftScale.SetShift(-scalarRange[0])
                    shiftScale.SetScale(1/(scalarRange[1]-scalarRange[0]))
                    shiftScale.Update()

                    speedImage = shiftScale.GetOutput()

                    fastMarching = slicer.vtkvmtkFastMarchingUpwindGradientImageFilter()
                    fastMarching.SetInput(speedImage)
                    fastMarching.SetSeeds(sourceSeedIds)
                    fastMarching.GenerateGradientImageOff()
                    fastMarching.SetTargetOffset(10.0)
                    if targetSeedIds.GetNumberOfIds() > 0:
                        fastMarching.SetTargets(targetSeedIds)
                        fastMarching.SetTargetReachedModeToOneTarget()
                    else:
                        fastMarching.SetTargetReachedModeToNoTargets()
                    fastMarching.Update()                                                            

                    #parentClass.debug(fastMarching.GetOutput())


                    subtract = slicer.vtkImageMathematics()
                    subtract.SetInput(fastMarching.GetOutput())
                    subtract.SetOperationToAddConstant()
                    subtract.SetConstantC(-fastMarching.GetTargetValue())
                    subtract.Update()

                    outVolume.SetAndObserveImageData(subtract.GetOutput())
                    matrix = slicer.vtkMatrix4x4()
                    inVolume.GetIJKToRASMatrix(matrix)
                    outVolume.SetIJKToRASMatrix(matrix)

