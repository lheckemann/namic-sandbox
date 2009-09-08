from Slicer import slicer
from SlicerAdvancedPageSkeleton import SlicerAdvancedPageSkeleton

vtkKWPushButton_InvokedEvent = 10000
vtkSlicerNodeSelectorWidget_NodeSelectedEvent = 11000
vtkKWScale_ScaleValueChangedEvent = 10001
vtkKWScale_ScaleValueStartChangingEvent = 10002

###
### fiducial seeding page (derived from skeleton)
###
class SlicerFiducialSeedingGUI(SlicerAdvancedPageSkeleton):

    def __init__(self,parentFrame,parentClass):
        SlicerAdvancedPageSkeleton.__init__(self,parentFrame,parentClass)

        self._firstRowFrame = slicer.vtkKWFrame()
        self._fiducialSelector = slicer.vtkSlicerNodeSelectorWidget()
        self._fiducialRegionSizeScale = slicer.vtkKWScaleWithLabel()
        self._fiducialStepSizeScale = slicer.vtkKWScaleWithLabel()

        self._startButton = slicer.vtkKWPushButton()
        self._defaultButton = slicer.vtkKWPushButton()

#       self._transformableNode = NULL
#       self._sourceFiducialList = None
#       self._targetFiducialList = None


    def Destructor(self):
        SlicerAdvancedPageSkeleton.Destructor(self)

        self._firstRowFrame.SetParent(None)        
        self._firstRowFrame = None

        self._fiducialSelector.SetParent(None)
        self._fiducialSelector = None
        self._fiducialRegionSizeScale.SetParent(None)
        self._fiducialRegionSizeScale = None
        self._fiducialStepSizeScale.SetParent(None)
        self._fiducialStepSizeScale = None

        self._startButton.SetParent(None)
        self._startButton = None
        self._defaultButton.SetParent(None)
        self._defaultButton = None

#       self._sourceFiducialList = None


    def AddGUIObservers(self):
        SlicerAdvancedPageSkeleton.AddGUIObservers(self)

        self._fiducialSelectorSelectedTag = self._parentClass.AddObserverByNumber(self._fiducialSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)

        self._fiducialRegionSizeScaleChangingTag = self._parentClass.AddObserverByNumber(self._fiducialRegionSizeScale ,vtkKWScale_ScaleValueStartChangingEvent)
        self._fiducialRegionSizeScaleChangedTag = self._parentClass.AddObserverByNumber(self._fiducialRegionSizeScale ,vtkKWScale_ScaleValueChangedEvent)

        self._fiducialStepSizeScaleChangingTag = self._parentClass.AddObserverByNumber(self._fiducialStepSizeScale ,vtkKWScale_ScaleValueStartChangingEvent)
        self._fiducialStepSizeScaleChangedTag = self._parentClass.AddObserverByNumber(self._fiducialStepSizeScale ,vtkKWScale_ScaleValueChangedEvent)

#       self._addSourcePointButtonTag = self._parentClass.AddObserverByNumber(self._addSourcePointButton,vtkKWPushButton_InvokedEvent)

        self._startButtonTag = self._parentClass.AddObserverByNumber(self._startButton,vtkKWPushButton_InvokedEvent)

        self._defaultButtonTag = self._parentClass.AddObserverByNumber(self._defaultButton,vtkKWPushButton_InvokedEvent)

    def RemoveGUIObservers(self):
        SlicerAdvancedPageSkeleton.RemoveGUIObservers(self)

        self._parentClass.RemoveObserver(self._fiducialSelectorSelectedTag)

        self._parentClass.RemoveObserver(self._fiducialRegionSizeScaleChangingTag)
        self._parentClass.RemoveObserver(self._fiducialRegionSizeScaleChangedTag)

        self._parentClass.RemoveObserver(self._fiducialStepSizeScaleChangingTag)
        self._parentClass.RemoveObserver(self._fiducialStepSizeScaleChangedTag)
#       self._parentClass.RemoveObserver(self._addSourcePointButtonTag)

        self._parentClass.RemoveObserver(self._startButtonTag)
        self._parentClass.RemoveObserver(self._defaultButtonTag)


    def UpdateMRML(self):

        node = self._parentClass.GetScriptedModuleNode()

#       if self._sourceFiducialList != None:
#           node.SetParameter('CF_sourceFiducialList',self._sourceFiducialList.GetID())
#       else:
#           node.SetParameter('CF_sourceFiducialList',"None")

        if self._fiducialSelector.GetSelected():
            node.SetParameter('InputFiducialRef',self._fiducialSelector.GetSelected().GetID())


#       if (this->FiducialSelector->GetSelected())
#       {
#         n->SetInputFiducialRef(this->FiducialSelector->GetSelected()->GetID());
#       }

        node.SetParameter('fiducialRegionSize',self._fiducialRegionSizeScale.GetWidget().GetValue()) 
        node.SetParameter('fiducialStepSize',self._fiducialStepSizeScale.GetWidget().GetValue()) 
#       if self._targetFiducialList != None:
#           node.SetParameter('CF_targetFiducialList',self._targetFiducialList.GetID())
#       else:
#           node.SetParameter('CF_targetFiducialList',"None")


    def UpdateGUI(self):

        node = self._parentClass.GetScriptedModuleNode()

        if node:

            self._parentClass.SetUpdatingOn()
            fiducialRegionSize = node.GetParameter('fiducialRegionSize')
            if fiducialRegionSize:
                self._fiducialRegionSizeScale.GetWidget().SetValue(fiducialRegionSize)
            fiducialStepSize = node.GetParameter('fiducialStepSize')
            if fiducialStepSize:
                self._fiducialStepSizeScale.GetWidget().SetValue(fiducialStepSize)
            node.SetParameter('InputFiducialRef',self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter("InputFiducialRef")))

#           s = this->GetMRMLScene()->GetNodeByID(n->GetInputFiducialRef());
#           this->FiducialSelector->SetSelected(s);
#           if (node.GetParameter("InputFiducialRef")!="None" and node.GetParameter("InputFiducialRef")):
#               self._sourceFiducialList = self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter("InputFiducialRef"))


#           if node.GetParameter('InputFiducialRef')=="None":
#               self._parentClass._helper.debug("no InputFiducialRef so far..")
#               self.UnLockInitInterface(0)
#               if self._fiducialSelector.GetSelected():
                   # update input volume to current selection
#                   self._parentClass._helper.debug("update inputROIRef to the current selection")
#                   self._updating = 1
#                   node.SetParameter('InputFiducialRef',self._fiducialSelector.GetSelected())
#                   self._state = 1 
#                   self._updating = 0
#                   self.UpdateGUIByState()

#                   self._parentClass.UpdateMRML()
#                   self._startButton.SetEnabled(1)
#                   self.UnLockInitInterface(1)
#               else:
                   # no input volume at all
                   # raise first page
#                   self._state = -1
#                   self._updating = 1
#                   node.SetParameter('raisedInitializationPageID',0)
#                   node.SetParameter('raisedEvolutionPageID',0)
#                   self._updating = 0
#                   self._startButton.SetEnabled(0)
#  
#           else:
               ### InputVolumeRef is set, test if it points to a valid node

#               if self._parentClass.GetLogic().GetMRMLScene().IsNodePresent(self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('InputFiducialRef'))):
#                   self._parentClass._helper.debug("InputFiducialRef is already associated, select this one in the GUI!")
#                   self._updating = 1
#                   self._fiducialSelector.SetSelected(node.GetParameter('InputFiducialRef'))

#               else:
#                    self._helper.debug("InputFiducialRef points to a deleted node! update.. and lock interface")
#                    self._updating = 1
#                    node.SetParameter('InpuFiducialRef',"None")
#                    self._startButton.SetEnabled(0)


#           if (node.GetParameter("CF_sourceFiducialList")!="None" and node.GetParameter("CF_sourceFiducialList")):
#               self._sourceFiducialList = self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter("CF_sourceFiducialList"))

#           if (node.GetParameter("CF_targetFiducialList")!="None" and node.GetParameter("CF_targetFiducialList")):
#               self._targetFiducialList = self._parentClass.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter("CF_targetFiducialList"))

#           self._state = node.GetParameter('CF_state')
#           self.UpdateGUIByState()


#           self._thresholdSlider.SetExtent(node.GetParameter("CF_lowerThreshold"),node.GetParameter("CF_higherThreshold"),0,100,0,100)
            self._parentClass.SetUpdatingOff()

 



    def DeleteFiducialListsFromScene(self):
        
        node = self._parentClass.GetScriptedModuleNode()

        if node:

            scene = self._parentClass.GetLogic().GetMRMLScene()

            if self._sourceFiducialList!=None:
                if scene.IsNodePresent(self._sourceFiducialList):
                    # node is in scene, now delete
                    scene.RemoveNode(self._sourceFiducialList)
                    self._sourceFiducialList = None




    def ProcessGUIEvents(self,caller,event):
        SlicerAdvancedPageSkeleton.ProcessGUIEvents(self,caller,event)

        if caller == self._startButton and event == vtkKWPushButton_InvokedEvent:
            self.Execute()
            self._parentClass.UpdateMRML()
        elif caller == self._defaultButton and event == vtkKWPushButton_InvokedEvent:
#           self.DeleteFiducialListsFromScene()
            self.Reset()
            self._parentClass.UpdateMRML()


    def ProcessMRMLEvents(self,caller,event):
#       SlicerAdvancedPageSkeleton.ProcessMRMLEvents(self,caller,event)
#       if self._updating != 1:
#           self._helper.debug("main ProcessMRMLEvents called, event: "+str(event))

#           if caller == self.GetScriptedModuleNode() and self.GetScriptedModuleNode!=None:
#               self.UpdateGUI()

        if self.UpdatingMRML == 1:
            return
#       if event == vtkMRMLSc
#       slicer.vtkMRMLTractographyFiducialSeedingNode





    def AddTransformableNodeObserver(transformableNode):
        events = slicer.vtkIntArray()
        events.InsertNextValue(vtkMRMLTransformableNode_TransformModifiedEvent)
        if (vtkMRMLFiducialListNode_SafeDownCast(n) != None):
            events.InsertNextValue(vtkMRMLFiducialListNode_FiducialModifiedEvent)
        slicer.vtkSetAndObserveMRMLNodeEventsMacro(self._transformableNode, transformableNode, events)
        events.Delete()



        
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

        if self._currentFiducialListLabel == "SP":

            # one fiducial added to source points
            # disable source point button

            #re-change the button appeareance
            self._state = 1
            self.UpdateGUIByState()

            self._parentClass.UpdateMRML()



    def BuildGUI(self):
        SlicerAdvancedPageSkeleton.BuildGUI(self)

        self._firstRowFrame.SetParent(self._parentFrame)
        self._firstRowFrame.Create()

        self._fiducialSelector.SetNodeClass("vtkMRMLFiducialListNode","","","")
        self._fiducialSelector.AddNodeClass("vtkMRMLModelNode","","","")
        self._fiducialSelector.SetNewNodeEnabled(0)
        self._fiducialSelector.NoneEnabledOn()
        self._fiducialSelector.SetShowHidden(1)
        self._fiducialSelector.SetParent(self._firstRowFrame)
        self._fiducialSelector.Create()
        self._fiducialSelector.SetMRMLScene(self._parentClass.GetLogic().GetMRMLScene())
        self._fiducialSelector.UpdateMenu()
        self._fiducialSelector.SetBorderWidth(2)
        self._fiducialSelector.SetLabelText("Select FiducialList:")
        self._fiducialSelector.SetBalloonHelpString("select a fiducial list for seeding.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._fiducialSelector.GetWidgetName())


        self._fiducialRegionSizeScale.SetParent(self._firstRowFrame)
        self._fiducialRegionSizeScale.SetLabelText("Fiducial Seeding Region Size (mm)")
        self._fiducialRegionSizeScale.Create()
#        w = self._anisotropyThresholdScale.GetScale().GetWidth()
        self._fiducialRegionSizeScale.GetWidget().SetRange(0,10)
        self._fiducialRegionSizeScale.GetWidget().SetResolution(0.5)
        self._fiducialRegionSizeScale.GetWidget().SetValue(1) 
        slicer.TkCall("pack %s -side top -anchor e -padx 2 -pady 2" % self._fiducialRegionSizeScale.GetWidgetName())

        self._fiducialStepSizeScale.SetParent(self._firstRowFrame)
        self._fiducialStepSizeScale.SetLabelText("Fiducial Seeding Step Size (mm)")
        self._fiducialStepSizeScale.Create()
#        w = self._anisotropyThresholdScale.GetScale().GetWidth()
        self._fiducialStepSizeScale.GetWidget().SetRange(0,10)
        self._fiducialStepSizeScale.GetWidget().SetResolution(0.1)
        self._fiducialStepSizeScale.GetWidget().SetValue(1) 
        slicer.TkCall("pack %s -side top -anchor e -padx 2 -pady 2" % self._fiducialStepSizeScale.GetWidgetName())

        self._startButton.SetParent(self._parentFrame)
        self._startButton.Create()
        self._startButton.SetText("Start!")
        self._startButton.SetEnabled(1)

        self._defaultButton.SetParent(self._parentFrame)
        self._defaultButton.Create()
        self._defaultButton.SetText("Default")
        self._defaultButton.SetEnabled(1)



        slicer.TkCall("pack %s -side top -expand y -padx 2 -pady 2 -in %s" % (self._firstRowFrame.GetWidgetName(), self._parentFrame.GetWidgetName()))

        slicer.TkCall("pack %s -side right -expand n -padx 2 -pady 2 -in %s" % (self._startButton.GetWidgetName(), self._parentFrame.GetWidgetName()))
        slicer.TkCall("pack %s -side right -expand n -padx 2 -pady 2 -in %s" % (self._defaultButton.GetWidgetName(), self._parentFrame.GetWidgetName()))


    def Execute(self):

        scene = self._parentClass.GetLogic().GetMRMLScene()

        self._parentClass.SetUpdatingOn()

        inVolumeNode = self._parentClass._inVolumeSelector.GetSelected()
        outFibersNode = self._parentClass._outFibersSelector.GetSelected()
        fiducialNode = self._fiducialSelector.GetSelected()
        tensorType = self._parentClass._tensorTypeMenu.GetWidget().GetValue()
        stoppingMode = self._parentClass._stoppingModeMenu.GetWidget().GetValue()
        anisotropyThreshold = self._parentClass._anisotropyThresholdScale.GetWidget().GetValue()
        stoppingCurvature = self._parentClass._stoppingCurvatureScale.GetWidget().GetValue()
        integrationStepLength = self._parentClass._integrationStepLengthScale.GetWidget().GetValue()
        minimumLength = self._parentClass._minimumLengthScale.GetWidget().GetValue()
#       fraction = self._parentClass._fractionScale.GetWidget().GetValue()
        fiducialRegionSize = self._fiducialRegionSizeScale.GetWidget().GetValue()
        fiducialStepSize = self._fiducialStepSizeScale.GetWidget().GetValue()
        fiducial = True

        myLogic = self._parentClass.GetMyLogic()
        resultContainer = myLogic.ExecuteTractography(inVolumeNode, outFibersNode, "", fiducialNode, tensorType, stoppingMode, anisotropyThreshold, stoppingCurvature, integrationStepLength, minimumLength, 1,1, fiducialRegionSize, fiducialStepSize, fiducial)
#        resultContainer = self._parentClass.G

#        resultContainer = self._parentClass.GetHelper().SetAndMergeInitVolume(resultContainer)
#        self._parentClass.GetHelper().GenerateModel(resultContainer)

        self._parentClass.SetUpdatingOff()
        self._parentClass.UpdateMRML() # save the results



    def Reset(self):
        self._fiducialRegionSizeScale.GetWidget().SetValue(1) 
        self._fiducialStepSizeScale.GetWidget().SetValue(1) 
        self._parentClass._tensorTypeMenu.GetWidget().SetValue("Two-Tensor")
        self._parentClass._stoppingModeMenu.GetWidget().SetValue("Cl1")
        self._parentClass._anisotropyThresholdScale.GetWidget().SetValue(0.1) 
        self._parentClass._stoppingCurvatureScale.GetWidget().SetValue(0.8) 
        self._parentClass._integrationStepLengthScale.GetWidget().SetValue(0.5) 
        self._parentClass._minimumLengthScale.GetWidget().SetValue(20)
#       fraction = self._parentClass._fractionScale.GetWidget().GetValue()
