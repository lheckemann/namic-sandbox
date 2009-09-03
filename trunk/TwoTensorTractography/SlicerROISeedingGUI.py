from SlicerAdvancedPageSkeleton import SlicerAdvancedPageSkeleton
from Slicer import slicer

vtkSlicerNodeSelectorWidget_NodeSelectedEvent = 11000
vtkKWPushButton_InvokedEvent = 10000
#vtkKWExtent_EndChangeEvent = 10002
vtkKWEntry_EntryValueChangedEvent = 10000

###
### ROI seeding page (derived from skeleton)
###
class SlicerROISeedingGUI(SlicerAdvancedPageSkeleton):

    def __init__(self,parentFrame,parentClass):
        SlicerAdvancedPageSkeleton.__init__(self,parentFrame,parentClass)

        self._firstRowFrame = slicer.vtkKWFrame()
        self._inROISelector = slicer.vtkSlicerNodeSelectorWidget()
        self._numberOfSeedpointsPerVoxelEntry = slicer.vtkKWEntryWithLabel()
        self._roiLabelEntry = slicer.vtkKWEntryWithLabel()
#       self._addSourcePointButton = slicer.vtkKWPushButton()
#       self._addTargetPointButton = slicer.vtkKWPushButton()
#       self._secondRowFrame = slicer.vtkKWFrame()
#       self._thresholdFrame = slicer.vtkKWFrameWithLabel()
#       self._thresholdSlider = slicer.vtkKWExtent()
        self._startButton = slicer.vtkKWPushButton()
        self._defaultButton = slicer.vtkKWPushButton()

#       self._sourceFiducialList = None
#       self._targetFiducialList = None


#       self._state = 0

    def Destructor(self):
        SlicerAdvancedPageSkeleton.Destructor(self)

        self._firstRowFrame.SetParent(None)        
        self._firstRowFrame = None
        self._inROISelector.SetParent(None)
        self._inROISelector = None
        self._numberOfSeedpointsPerVoxelEntry.SetParent(None)
        self._numberOfSeedpointsPerVoxelEntry = None
        self._roiLabelEntry.SetParent(None)
        self._roiLabelEntry = None
#       self._addSourcePointButton.SetParent(None)
#       self._addSourcePointButton = None
#       self._addTargetPointButton.SetParent(None)
#       self._addTargetPointButton = None
#       self._secondRowFrame.SetParent(None)
#       self._secondRowFrame = None
#       self._thresholdFrame.SetParent(None)
#       self._thresholdFrame = None
#       self._thresholdSlider.SetParent(None)
#       self._thresholdSlider = None
        self._startButton.SetParent(None)
        self._startButton = None
        self._defaultButton.SetParent(None)
        self._defaultButton = None

#       self._sourceFiducialList = None
#       self._targetFiducialList = None
#       self._state = None

    def AddGUIObservers(self):
        SlicerAdvancedPageSkeleton.AddGUIObservers(self)

#       self._addSourcePointButtonTag = self._parentClass.AddObserverByNumber(self._addSourcePointButton,vtkKWPushButton_InvokedEvent)

#       self._addTargetPointButtonTag = self._parentClass.AddObserverByNumber(self._addTargetPointButton,vtkKWPushButton_InvokedEvent)

#       self._thresholdSliderTag = self._parentClass.AddObserverByNumber(self._thresholdSlider, vtkKWExtent_EndChangeEvent)
        self._inROISelectorSelectedTag = self._parentClass.AddObserverByNumber(self._inROISelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)

        self._numberOfSeedpointsPerVoxelEntryTag = self._parentClass.AddObserverByNumber(self._numberOfSeedpointsPerVoxelEntry ,vtkKWEntry_EntryValueChangedEvent)

        self._roiLabelEntryTag = self._parentClass.AddObserverByNumber(self._roiLabelEntry ,vtkKWEntry_EntryValueChangedEvent)

        self._startButtonTag = self._parentClass.AddObserverByNumber(self._startButton,vtkKWPushButton_InvokedEvent)

        self._defaultButtonTag = self._parentClass.AddObserverByNumber(self._defaultButton,vtkKWPushButton_InvokedEvent)

    def RemoveGUIObservers(self):
        SlicerAdvancedPageSkeleton.RemoveGUIObservers(self)

#       self._parentClass.RemoveObserver(self._addSourcePointButtonTag)

#       self._parentClass.RemoveObserver(self._addTargetPointButtonTag)

        self._parentClass.RemoveObserver(self._inROISelectorSelectedTag)

        self._parentClass.RemoveObserver(self._numberOfSeedpointsPerVoxelEntryTag)

        self._parentClass.RemoveObserver(self._roiLabelEntryTag)

        self._parentClass.RemoveObserver(self._startButtonTag)
        self._parentClass.RemoveObserver(self._defaultButtonTag)


    def UpdateMRML(self):

        node = self._parentClass.GetScriptedModuleNode()

        if self._inROISelector.GetSelected():
            node.SetParameter('InputROIRef',self._inROISelector.GetSelected())
        else:
            node.SetParameter('InputROIRef',"None")

        node.SetParameter('numberOfSeedpointsPerVoxel',self._numberOfSeedpointsPerVoxelEntry.GetWidget().GetValue())
        node.SetParameter('roiLabel',self._roiLabelEntry.GetWidget().GetValue())

#       if input volume and output fibers not selected, lock init interface
#       if node.GetParameter('InputROIRef')=="None":# or node.GetParameter('OutputFibersRef')=="None":
#           self._startButton.SetEnabled(0)
#       else:
#           self._startButton.SetEnabled(1)
#           self._infoLabel.SetText("Start tractography!..")

#       if self._sourceFiducialList != None:
#           node.SetParameter('CF_sourceFiducialList',self._sourceFiducialList.GetID())
#       else:
#           node.SetParameter('CF_sourceFiducialList',"None")

#       if self._targetFiducialList != None:
#           node.SetParameter('CF_targetFiducialList',self._targetFiducialList.GetID())
#       else:
#           node.SetParameter('CF_targetFiducialList',"None")

#       extentValues = self._thresholdSlider.GetExtent()
#       node.SetParameter('CF_lowerThreshold', extentValues[0])
#       node.SetParameter('CF_higherThreshold', extentValues[1])
#       node.SetParameter('RS_state', self._state)

    def UpdateGUI(self):

        node = self._parentClass.GetScriptedModuleNode()

        if node:
            numberOfSeedpointsPerVoxel = node.GetParameter('numberOfSeedpointsPerVoxel')
            if numberOfSeedpointsPerVoxel:
                self._numberOfSeedpointsPerVoxelEntry.GetWidget().SetValue(numberOfSeedpointsPerVoxel)
            roiLabel = node.GetParameter('roiLabel')
            if roiLabel:
                self._roiLabelEntry.GetWidget().SetValue(roiLabel)

            if node.GetParameter('InputROIRef')=="None":
                self._parentClass._helper.debug("no inputROIRef so far..")
#               self.UnLockInitInterface(0)
                if self._inROISelector.GetSelected():
                    # update input volume to current selection
                    self._parentClass._helper.debug("update inputROIRef to the current selection")
#                   self._updating = 1
                    node.SetParameter('InputROIRef',self._inROISelector.GetSelected())
#                   self._state = 1 
#                   self._updating = 0
#                   self.UpdateGUIByState()

                    self._parentClass.UpdateMRML()
                    self._startButton.SetEnabled(1)
#                   self.UnLockInitInterface(1)
                else:
                    # no input volume at all
                    # raise first page
#                    self._state = -1
#                    self._updating = 1
#                    node.SetParameter('raisedInitializationPageID',0)
#                    node.SetParameter('raisedEvolutionPageID',0)
#                    self._updating = 0
                     self._startButton.SetEnabled(0)
#  
            else:
                ### InputVolumeRef is set, test if it points to a valid node
                if self._parentClass.GetLogic().GetMRMLScene().IsNodePresent(node.GetParameter('InputROIRef')):
                    self._parentClass._helper.debug("inputROIRef is already associated, select this one in the GUI!")
#                   self._updating = 1
                    self._inROISelector.SetSelected(node.GetParameter('InputROIRef'))
#                   self.UnLockInitInterface(1)
                    # update view of slice and 3d
                    slicer.ApplicationLogic.GetSelectionNode().SetReferenceActiveVolumeID(self._inROISelector.GetSelected().GetID())
                    slicer.ApplicationLogic.PropagateVolumeSelection()
#                   self._updating = 0
#                   self._state = 1 
#                   self.UpdateGUIByState()    
                    self._startButton.SetEnabled(1)
                    self._parentClass.UpdateMRML()
                else:
                    self._helper.debug("inputROIRef points to a deleted node! update.. and lock interface")
#                    self._updating = 1
                    node.SetParameter('InputROIRef',"None")
                    self._startButton.SetEnabled(0)
#                    node.SetParameter('raisedInitializationPageID',0)
#                    node.SetParameter('raisedEvolutionPageID',0)
#                    self._updating = 0
#                    self._state = -1
#                    self.UnLockInitInterface(0) 
             
#        self._sourceFiducialList = None
#        self._targetFiducialList = None

#        if (node.GetParameter("CF_sourceFiducialList")!="None" and node.GetParameter("CF_sourceFiducialList")):
#            self._sourceFiducialList = self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter("CF_sourceFiducialList"))

#        if (node.GetParameter("CF_targetFiducialList")!="None" and node.GetParameter("CF_targetFiducialList")):
#            self._targetFiducialList = self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter("CF_targetFiducialList"))

#        self._state = node.GetParameter('RS_state')
#        self.UpdateGUIByState()

        self._parentClass.SetUpdatingOn()
#        self._thresholdSlider.SetExtent(node.GetParameter("CF_lowerThreshold"),node.GetParameter("CF_higherThreshold"),0,100,0,100)
        self._parentClass.SetUpdatingOff()



    # belongs to UpdateGUI
#    def UpdateGUIByState(self):

#        if self._state == 0:
            # initial state, no button pressed, everything removed
            
#            self._addSourcePointButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#            self._addSourcePointButton.SetReliefToRaised()
#            self._addSourcePointButton.SetBackgroundColor(0.9,0.9,0.9)
#            self._addSourcePointButton.SetForegroundColor(0.2,0.6,0.2)
#            self._addSourcePointButton.SetText("1. Add Source Point")
#            self._addSourcePointButton.SetWidth(15)
#            self._addSourcePointButton.SetEnabled(1)
#            self._addSourcePointButton.SetBalloonHelpString("Click to add new source point")

#            self._addTargetPointButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#            self._addTargetPointButton.SetReliefToRaised()
#            self._addTargetPointButton.SetBackgroundColor(0.9,0.9,0.9)
#            self._addTargetPointButton.SetForegroundColor(0.2,0.2,0.6)
#            self._addTargetPointButton.SetText("2. Add Target Point")
#            self._addTargetPointButton.SetWidth(15)
#            self._addTargetPointButton.SetBalloonHelpString("Click to add new target point")
#            self._addTargetPointButton.SetEnabled(0)

#            self._parentClass.SetUpdatingOn()
#            self._thresholdFrame.AllowFrameToCollapseOff()
#            self._thresholdFrame.SetLabelText("Threshold of Gray Values")
#            self._thresholdFrame.SetReliefToSunken()     

#            inVolumeNode = self._parentClass._inVolumeSelector.GetSelected()

#            if inROINode:

                # update min and max
#                imageMaxValue = round(inROINode.GetImageData().ToArray().max(),0)
#                imageMinValue = round(inROINode.GetImageData().ToArray().min(),0)
#                self._parentClass.SetUpdatingOn()
#                self._thresholdSlider.SetExtentRange(imageMinValue, imageMaxValue, 0, 100, 0, 100)
#                self._thresholdSlider.SetExtent(imageMinValue, imageMaxValue, 0, 100, 0, 100)
#                self._parentClass.SetUpdatingOff()

#            else:

#                self._parentClass.SetUpdatingOn()
#                self._thresholdSlider.SetExtentRange(0, 100, 0, 100, 0, 100)
#                self._thresholdSlider.SetExtent(0, 100, 0, 100, 0, 100)
#                self._parentClass.SetUpdatingOff()

#            self._thresholdSlider.SetReliefToSunken()        
#            self._thresholdSlider.ZExtentVisibilityOff()
#            self._thresholdSlider.YExtentVisibilityOff()
#            self._thresholdSlider.SetEnabled(1)
#            self._thresholdSlider.GetXRange().SetLabelText("Gray Values of Vessels")
            #self._thresholdSlider.SetEndCommand(self._parentClass.vtkScriptedModuleGUI, "Invoke CF_ExtentUpdate")
#            self._parentClass.SetUpdatingOff()

#            self._startButton.SetEnabled(0)
#            self._startButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#            self._startButton.SetReliefToRaised()
#            self._startButton.SetBackgroundColor(0.9,0.9,0.9)
#            self._startButton.SetText("Start!")
#            self._startButton.SetWidth(8)
#            self._startButton.SetHeight(2)
#            self._startButton.SetBalloonHelpString("Click to start")

#            self._resetButton.SetEnabled(1)
#            self._resetButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#            self._resetButton.SetReliefToRaised()
#            self._resetButton.SetBackgroundColor(0.9,0.9,0.9)
#            self._resetButton.SetText("Cancel")
#            self._resetButton.SetWidth(8)
#            self._resetButton.SetHeight(2)
#            self._resetButton.SetBalloonHelpString("Click to reset")


#        elif self._state == 1:

#            self._state = 0
#            self.UpdateGUIByState()
#            self._state = 1

            # source button pressed before
#            self._addSourcePointButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#            self._addSourcePointButton.SetReliefToRaised()
#            self._addSourcePointButton.SetBackgroundColor(0.9,0.9,0.9)
#            self._addSourcePointButton.SetForegroundColor(0.2,0.6,0.2)
#            self._addSourcePointButton.SetText("Source point added!")
#            self._addSourcePointButton.SetWidth(15)
#            self._addSourcePointButton.SetBalloonHelpString("No other source point needed")
#            self._addSourcePointButton.SetEnabled(0)
#            self._addTargetPointButton.SetEnabled(1)
#            self._startButton.SetEnabled(1)
#            self._resetButton.SetEnabled(1)
#            self._parentClass.SetUpdatingOn()
#            self._thresholdSlider.SetEnabled(1)
#            self._parentClass.SetUpdatingOff()

#        elif self._state == 2:

#            self._state = 1
#            self.UpdateGUIByState()
#            self._state = 2

            # target button pressed before
#            self._addTargetPointButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#            self._addTargetPointButton.SetReliefToRaised()
#            self._addTargetPointButton.SetBackgroundColor(0.9,0.9,0.9)
#            self._addTargetPointButton.SetForegroundColor(0.2,0.2,0.6)
#            self._addTargetPointButton.SetActiveForegroundColor(0.1,0.1,0.1)
#            self._addTargetPointButton.SetText("Target point added!")
#            self._addTargetPointButton.SetWidth(15)
#            self._addTargetPointButton.SetBalloonHelpString("No other target point needed")
#            self._addTargetPointButton.SetEnabled(0)
#            self._addSourcePointButton.SetEnabled(0)
#            self._startButton.SetEnabled(1)
##            self._resetButton.SetEnabled(1)
#            self._parentClass.SetUpdatingOn()
#            self._thresholdSlider.SetEnabled(1)
#            self._parentClass.SetUpdatingOff()

    #
    # if which == 1, delete only source
    # if which == 2, delete only target
    # else delete both
    #
#    def DeleteFiducialListsFromScene(self,which):
        
#        node = self._parentClass.GetScriptedModuleNode()

#        if node:

#            scene = self._parentClass.GetLogic().GetMRMLScene()

#            if (which!=2):

#                if self._sourceFiducialList!=None:
#                    if scene.IsNodePresent(self._sourceFiducialList):
                        # node is in scene, now delete
#                        scene.RemoveNode(self._sourceFiducialList)
#                        self._sourceFiducialList = None

#            if (which!=1):

#                if self._targetFiducialList!=None:
#                    if scene.IsNodePresent(self._targetFiducialList):
                        # node is in scene, now delete
#                        scene.RemoveNode(self._targetFiducialList)
#                        self._targetFiducialList = None



    def ProcessGUIEvents(self,caller,event):
        SlicerAdvancedPageSkeleton.ProcessGUIEvents(self,caller,event)
     
#       if caller == self._addSourcePointButton and event == vtkKWPushButton_InvokedEvent:
#           if self._parentClass.GetHelper().GetIsInteractiveMode() == 0:
#               self.InitAddSourcePoint()
#           elif self._parentClass.GetHelper().GetIsInteractiveMode() == 1:
#               self.TeardownAddSourcePoint()

#       elif caller == self._addTargetPointButton and event == vtkKWPushButton_InvokedEvent:
#           if self._parentClass.GetHelper().GetIsInteractiveMode() == 0:
#               self.InitAddTargetPoint()
#           elif self._parentClass.GetHelper().GetIsInteractiveMode() == 1:
#               self.TeardownAddTargetPoint()

        if caller == self._startButton and event == vtkKWPushButton_InvokedEvent:
            self.Execute()
            self._parentClass.UpdateMRML()
        elif caller == self._defaultButton and event == vtkKWPushButton_InvokedEvent:
#           self.DeleteFiducialListsFromScene(0)
            self.Reset()
            self._parentClass.UpdateMRML()
#       elif caller == self._thresholdSlider and event == vtkKWExtent_EndChangeEvent:
#           self._parentClass.UpdateMRML()

#    def InitAddSourcePoint(self):

#       scene = self._parentClass.GetLogic().GetMRMLScene()

#       if self._sourceFiducialList == None:
            
            # no list created yet
#           self._sourceFiducialList = slicer.vtkMRMLFiducialListNode()
#           self._sourceFiducialList.SetName("VMTK Colliding Fronts Source Points")
#           self._sourceFiducialList.SetScene(scene)
#           self._sourceFiducialList.SetColor(0.2,0.6,0.2)
#           self._sourceFiducialList.SetGlyphTypeFromString("Circle2D")
#           self._sourceFiducialList.SetLocked(1)
#           scene.AddNode(self._sourceFiducialList)

        #change the button appeareance
#       self._addSourcePointButton.SetActiveBackgroundColor(0.2,0.6,0.2)
#       self._addSourcePointButton.SetReliefToSunken()
#       self._addSourcePointButton.SetBackgroundColor(0.2,0.6,0.2)
#       self._addSourcePointButton.SetForegroundColor(0.2,0.2,0.2)
#       self._addSourcePointButton.SetText("Click into slice")
#       self._addSourcePointButton.SetWidth(15)

#       self._parentClass.UnLockInitInterface(0)
#       self._addTargetPointButton.SetEnabled(0)
#       self._thresholdSlider.SetEnabled(0)
#       self._startButton.SetEnabled(0)
#       self._resetButton.SetEnabled(0)

#       self._currentFiducialList = self._sourceFiducialList
#       self._currentFiducialListLabel = "SP"
#       self._parentClass.GetHelper().SetIsInteractiveMode(1,self)

#   def TeardownAddSourcePoint(self):

#       self._currentFiducialList = None
#       self._currentFiducialListLabel = None
#       self._parentClass.GetHelper().SetIsInteractiveMode(0,None)

#       self.DeleteFiducialListsFromScene(1)

#       self._parentClass.UnLockInitInterface(1)    
#       self._state = 0
#       self.UpdateGUIByState()

#   def InitAddTargetPoint(self):
#       scene = self._parentClass.GetLogic().GetMRMLScene()

#       if self._targetFiducialList == None:
            
            # no list created yet
#           self._targetFiducialList = slicer.vtkMRMLFiducialListNode()
#           self._targetFiducialList.SetName("VMTK Colliding Fronts Target Points")
#           self._targetFiducialList.SetScene(scene)
#           self._targetFiducialList.SetColor(0.2,0.2,0.6)
#           self._targetFiducialList.SetGlyphTypeFromString("Circle2D")
#           self._targetFiducialList.SetLocked(1)
#           scene.AddNode(self._targetFiducialList)

        #change the button appeareance
#       self._addTargetPointButton.SetActiveBackgroundColor(0.2,0.2,0.6)
#       self._addTargetPointButton.SetReliefToSunken()
#       self._addTargetPointButton.SetBackgroundColor(0.2,0.2,0.6)
#       self._addTargetPointButton.SetForegroundColor(0.7,0.7,0.7)
#       self._addTargetPointButton.SetActiveForegroundColor(0.7,0.7,0.7)
#       self._addTargetPointButton.SetText("Click into slice")
#       self._addTargetPointButton.SetWidth(15)

#       self._parentClass.UnLockInitInterface(0)
#       self._addSourcePointButton.SetEnabled(0)
#       self._thresholdSlider.SetEnabled(0)
#       self._startButton.SetEnabled(0)
#       self._resetButton.SetEnabled(0)

#       self._currentFiducialList = self._targetFiducialList
#       self._currentFiducialListLabel = "TP"
#       self._parentClass.GetHelper().SetIsInteractiveMode(1,self)

#   def TeardownAddTargetPoint(self):

#       self._currentFiducialList = None
#       self._currentFiducialListLabel = None
#       self._parentClass.GetHelper().SetIsInteractiveMode(0,None)

#       self.DeleteFiducialListsFromScene(2)

#       self._parentClass.UnLockInitInterface(1)
#       self._state = 1
#       self.UpdateGUIByState()

        
    #
    # x,y
    # which is the Name of the SliceWindow (Red,Yellow,Green)
    #
    def HandleClickInSliceWindowWithCoordinates(self, which, coordinates):
        SlicerAdvancedPageSkeleton.HandleClickInSliceWindowWithCoordinates(self, which, coordinates)

        coordinatesRAS = self._parentClass.GetHelper().ConvertCoordinates2RAS(which,coordinates)
        fiducial = self._currentFiducialList.AddFiducialWithXYZ(coordinatesRAS[0], coordinatesRAS[1], coordinatesRAS[2],0)

        self._currentFiducialList.SetNthFiducialLabelText(fiducial, self._currentFiducialListLabel+str(fiducial))
        self._parentClass.GetHelper().debug("Fiducial Added! Only one needed!")

        self._parentClass.GetHelper().SetIsInteractiveMode(0,None)

#       if self._currentFiducialListLabel == "SP":

            # one fiducial added to source points
            # disable source point button

            #re-change the button appeareance
#           self._state = 1
#           self.UpdateGUIByState()

#           self._parentClass.UpdateMRML()

#       elif self._currentFiducialListLabel == "TP":

            # one fiducial added to target points
            # disable target point button

            #re-change the button appeareance
#           self._state = 2
#           self.UpdateGUIByState()

#           self._parentClass.UpdateMRML()

    def BuildGUI(self):
        SlicerAdvancedPageSkeleton.BuildGUI(self)

        self._firstRowFrame.SetParent(self._parentFrame)
        self._firstRowFrame.Create()

        self._inROISelector.SetNodeClass("vtkMRMLScalarVolumeNode","","","")
        self._inROISelector.SetParent(self._firstRowFrame)
        self._inROISelector.Create()
        self._inROISelector.SetMRMLScene(self._parentClass.GetLogic().GetMRMLScene())
        self._inROISelector.UpdateMenu()
        self._inROISelector.SetBorderWidth(2)
        self._inROISelector.SetLabelText("Select ROI:")
        self._inROISelector.SetBalloonHelpString("Select an roi label map from the current mrml scene.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._inROISelector.GetWidgetName())

        self._numberOfSeedpointsPerVoxelEntry.SetParent(self._firstRowFrame)
        self._numberOfSeedpointsPerVoxelEntry.SetLabelText("Number of seed points per voxel")
        self._numberOfSeedpointsPerVoxelEntry.Create()
#       w = self._numberOfSeedpointsPerVoxelEntry.GetScale().GetWidth()
        self._numberOfSeedpointsPerVoxelEntry.GetWidget().SetValueAsInt(1) 
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._numberOfSeedpointsPerVoxelEntry.GetWidgetName())
    
        self._roiLabelEntry.SetParent(self._firstRowFrame)
        self._roiLabelEntry.SetLabelText("ROI label")
        self._roiLabelEntry.Create()
#       self.roiLabelEntry.GetScale().SetWidth(w)
        self._roiLabelEntry.GetWidget().SetValueAsInt(1)
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._roiLabelEntry.GetWidgetName())
   
#       self._startButton.SetParent(self._firstRowFrame)
#       self._startButton.Create()
#       self._startButton.SetText("Start")
#       self._startButton.SetWidth(8)
#       slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._startButton.GetWidgetName())

#       self._resetButton.SetParent(self._firstRowFrame)
#       self._resetButton.Create()
#       self._resetButton.SetText("Reset")
#       self._resetButton.SetWidth(8)
#       slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._resetButton.GetWidgetName())


#       self._startButton.SetEnabled(1)
#       self._startButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#       self._startButton.SetReliefToRaised()
#       self._startButton.SetBackgroundColor(0.9,0.9,0.9)
#       self._startButton.SetText("Start!")
#       self._startButton.SetWidth(8)
#       self._startButton.SetHeight(2)
#       self._startButton.SetBalloonHelpString("Click to start")

#       self._resetButton.SetEnabled(1)
#       self._resetButton.SetActiveBackgroundColor(0.9,0.9,0.9)
#       self._resetButton.SetReliefToRaised()
#       self._resetButton.SetBackgroundColor(0.9,0.9,0.9)
#       self._resetButton.SetText("Cancel")
#       self._resetButton.SetWidth(8)
#       self._resetButton.SetHeight(2)
#       self._resetButton.SetBalloonHelpString("Click to reset") 

#       self._addSourcePointButton.SetParent(self._firstRowFrame)
#       self._addSourcePointButton.Create()

#       self._addTargetPointButton.SetParent(self._firstRowFrame)
#       self._addTargetPointButton.Create()

#       self._secondRowFrame.SetParent(self._parentFrame)
#       self._secondRowFrame.Create()

#       self._thresholdFrame.SetParent(self._parentFrame)
#       self._thresholdFrame.Create()

#       self._thresholdSlider.SetParent(self._thresholdFrame.GetFrame())
#       self._thresholdSlider.Create()

        self._startButton.SetParent(self._parentFrame)
        self._startButton.Create()
        self._startButton.SetText("Start!")
        self._startButton.SetEnabled(1)

        self._defaultButton.SetParent(self._parentFrame)
        self._defaultButton.Create()
        self._defaultButton.SetText("Default")
        self._defaultButton.SetEnabled(1)

#       self.UpdateGUIByState()


        slicer.TkCall("pack %s -side top -expand y -padx 2 -pady 2 -in %s" % (self._firstRowFrame.GetWidgetName(), self._parentFrame.GetWidgetName()))
#       slicer.TkCall("pack %s -side top -expand y -padx 2 -pady 2 -in %s" % (self._secondRowFrame.GetWidgetName(), self._parentFrame.GetWidgetName()))

#       slicer.TkCall("pack %s -side top -expand n -padx 2 -pady 2" % (self._addSourcePointButton.GetWidgetName()))
#       slicer.TkCall("pack %s -side top -expand n -padx 2 -pady 2" % (self._addTargetPointButton.GetWidgetName()))
#       slicer.TkCall("pack %s -side left -expand y -padx 2 -pady 2 -in %s" % (self._thresholdFrame.GetWidgetName(), self._secondRowFrame.GetWidgetName()))
#       slicer.TkCall("pack %s -side top -expand n -padx 2 -pady 2" % (self._thresholdSlider.GetWidgetName()))

        slicer.TkCall("pack %s -side right -expand n -padx 2 -pady 2 -in %s" % (self._startButton.GetWidgetName(), self._parentFrame.GetWidgetName()))
        slicer.TkCall("pack %s -side right -expand n -padx 2 -pady 2 -in %s" % (self._defaultButton.GetWidgetName(), self._parentFrame.GetWidgetName()))


    def Execute(self):
        print "Execute"
        scene = self._parentClass.GetLogic().GetMRMLScene()

        self._parentClass.SetUpdatingOn()


#       extentValues = self._thresholdSlider.GetExtent()
        inVolumeNode = self._parentClass._inVolumeSelector.GetSelected()
        outFibersNode = self._parentClass._outFibersSelector.GetSelected()
        inROINode = self._inROISelector.GetSelected()
        tensorType = self._parentClass._tensorTypeMenu.GetWidget().GetValue()
        stoppingMode = self._parentClass._stoppingModeMenu.GetWidget().GetValue()
        anisotropyThreshold = self._parentClass._anisotropyThresholdScale.GetWidget().GetValue()
        stoppingCurvature = self._parentClass._stoppingCurvatureScale.GetWidget().GetValue()
        integrationStepLength = self._parentClass._integrationStepLengthScale.GetWidget().GetValue()
        minimumLength = self._parentClass._minimumLengthScale.GetWidget().GetValue()
#       fraction = self._parentClass._fractionScale.GetWidget().GetValue()
        roiLabel = self._roiLabelEntry.GetWidget().GetValue()
        numberOfSeedpointsPerVoxel = self._numberOfSeedpointsPerVoxelEntry.GetWidget().GetValue()
#       lowerThreshold = extentValues[0]
#       higherThreshold = extentValues[1]
#       sourceSeedsNode = self._sourceFiducialList
#       targetSeedsNode = self._targetFiducialList
        fiducial = False

        myLogic = self._parentClass.GetMyLogic()
        resultContainer = myLogic.ExecuteTractography(inVolumeNode, outFibersNode, inROINode, "", tensorType, stoppingMode, anisotropyThreshold, stoppingCurvature, integrationStepLength, minimumLength, roiLabel, numberOfSeedpointsPerVoxel, 1, 1, fiducial)
#       resultContainer = self._parentClass.GetHelper().SetAndMergeInitVolume(resultContainer)
#       self._parentClass.GetHelper().GenerateModel(resultContainer)

        self._parentClass.SetUpdatingOff()
#       self._parentClass._state = 1
#       self._parentClass.UpdateGUIByState()
        self._parentClass.UpdateMRML() # save the results



    def Reset(self):
        self._numberOfSeedpointsPerVoxelEntry.GetWidget().SetValueAsInt(1) 
        self._roiLabelEntry.GetWidget().SetValueAsInt(1)
        self._parentClass._tensorTypeMenu.GetWidget().SetValue("Two-Tensor")
        self._parentClass._stoppingModeMenu.GetWidget().SetValue("Cl1")
        self._parentClass._anisotropyThresholdScale.GetWidget().SetValue(0.1) 
        self._parentClass._stoppingCurvatureScale.GetWidget().SetValue(0.8) 
        self._parentClass._integrationStepLengthScale.GetWidget().SetValue(0.5) 
        self._parentClass._minimumLengthScale.GetWidget().SetValue(20)
#       fraction = self._parentClass._fractionScale.GetWidget().GetValue()
