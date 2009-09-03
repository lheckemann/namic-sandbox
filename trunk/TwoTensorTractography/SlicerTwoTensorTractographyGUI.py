from SlicerScriptedModule import ScriptedModuleGUI
from Slicer import slicer
import time

from SlicerTwoTensorTractographyLogic import SlicerTwoTensorTractographyLogic
from SlicerTwoTensorTractographyGUIHelper import SlicerTwoTensorTractographyGUIHelper
from SlicerWelcomeGUI import SlicerWelcomeGUI
from SlicerROISeedingGUI import SlicerROISeedingGUI
from SlicerFiducialSeedingGUI import SlicerFiducialSeedingGUI

vtkSlicerNodeSelectorWidget_NodeSelectedEvent = 11000
vtkKWPushButton_InvokedEvent = 10000
vtkKWScale_ScaleValueChangedEvent = 10001
vtkKWScale_ScaleValueStartChangingEvent = 10002
#vtkSlicerNodeSelectorWidget_NewNodeEvent = 11001
vtkKWCheckButton_SelectedStateChangedEvent = 10000


class SlicerTwoTensorTractographyGUI(ScriptedModuleGUI):


    def __init__(self):

        ScriptedModuleGUI.__init__(self)

        self._moduleNodeSelector = slicer.vtkSlicerNodeSelectorWidget()
        self._inVolumeSelector = slicer.vtkSlicerNodeSelectorWidget()
        self._outFibersSelector = slicer.vtkSlicerNodeSelectorWidget()

        self._tensorTypeMenu = slicer.vtkKWMenuButtonWithLabel()
        self._stoppingModeMenu = slicer.vtkKWMenuButtonWithLabel()
        self._anisotropyThresholdScale = slicer.vtkKWScaleWithLabel()
        self._stoppingCurvatureScale = slicer.vtkKWScaleWithLabel()
        self._integrationStepLengthScale = slicer.vtkKWScaleWithLabel()
        self._minimumLengthScale = slicer.vtkKWScaleWithLabel()
#       self._fractionScale = slicer.vtkKWScaleWithLabel()

        # upper most frame
        # create a vtkSlicerModuleCollapsibleFrame widget for each logical section in the GUI
        self._topFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        # seeding frame with tabs for each seeding method
        self._seedingMethodsFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        # notebook style for representing the different seeding methods
        self._seedingMethodsTabs = slicer.vtkKWNotebook()

        # info label
        self._infoLabel = slicer.vtkKWLabel()

        self._helper = SlicerTwoTensorTractographyGUIHelper(self)
        self._logic = SlicerTwoTensorTractographyLogic(self)

        self._outInitVolume = None
        self._outInitVolumeLast = None
        self._outInitFibers = None
        self._outInitFibersLast = None
        self._outModel = None
        self._outModelDisplay = None

        self._initPages = []

        self._updating = 0

    def Destructor(self):

        self._helper.debug("main Destructor called")

        for page in self._initPages:
            page.Destructor()

        self._moduleNodeSelector.SetParent(None)
        self._moduleNodeSelector = None
        self._inVolumeSelector.SetParent(None)
        self._inVolumeSelector = None
        self._outFibersSelector.SetParent(None)
        self._outFibersSelector = None

        self._tensorTypeMenu.SetParent(None)
        self._tensorTypeMenu = None
        self._stoppingModeMenu.SetParent(None)
        self._stoppingModeMenu = None
        self._anisotropyThresholdScale.SetParent(None)
        self._anisotropyThresholdScale = None
        self._stoppingCurvatureScale.SetParent(None)
        self._stoppingCurvatureScale = None
        self._integrationStepLengthScale.SetParent(None)
        self._integrationStepLengthScale = None
        self._minimumLengthScale.SetParent(None)
        self._minimumLengthScale = None
#       self._fractionScale.SetParent(None)
#       self._fractionScale = None

        self._topFrame.SetParent(None)
        self._topFrame = None
        self._seedingMethodsFrame.SetParent(None)
        self._seedingMethodsFrame = None
        self._seedingMethodsTabs.SetParent(None)
        self._seedingMethodsTabs = None

        self._infoLabel.SetParent(None)
        self._infoLabel = None

        self._logic = None
        self._helper = None

        self._initPages = None

        self._updating = None


    def RemoveMRMLNodeObservers(self):
        pass
    
    def RemoveLogicObservers(self):
        pass


    def AddGUIObservers(self):

        self._moduleNodeSelectorSelectedTag = self.AddObserverByNumber(self._moduleNodeSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)
        self._inVolumeSelectorSelectedTag = self.AddObserverByNumber(self._inVolumeSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)
        self._outFibersSelectorSelectedTag = self.AddObserverByNumber(self._outFibersSelector,vtkSlicerNodeSelectorWidget_NodeSelectedEvent)

        self._anisotropyThresholdScaleChangingTag = self.AddObserverByNumber(self._anisotropyThresholdScale ,vtkKWScale_ScaleValueStartChangingEvent)
        self._anisotropyThresholdScaleChangedTag = self.AddObserverByNumber(self._anisotropyThresholdScale ,vtkKWScale_ScaleValueChangedEvent)

        self._stoppingCurvatureScaleChangingTag = self.AddObserverByNumber(self._stoppingCurvatureScale ,vtkKWScale_ScaleValueStartChangingEvent)
        self._stoppingCurvatureScaleChangedTag = self.AddObserverByNumber(self._stoppingCurvatureScale ,vtkKWScale_ScaleValueChangedEvent)

        self._integrationStepLengthScaleChangingTag = self.AddObserverByNumber(self._integrationStepLengthScale ,vtkKWScale_ScaleValueStartChangingEvent)
        self._integrationStepLengthScaleChangedTag = self.AddObserverByNumber(self._integrationStepLengthScale ,vtkKWScale_ScaleValueChangedEvent)

        self._minimumLengthScaleChangingTag = self.AddObserverByNumber(self._minimumLengthScale ,vtkKWScale_ScaleValueStartChangingEvent)
        self._minimumLengthScaleChangedTag = self.AddObserverByNumber(self._minimumLengthScale ,vtkKWScale_ScaleValueChangedEvent)

#       self._fractionScaleChangingTag = self.AddObserverByNumber(self._fractionScale ,vtkKWScale_ScaleValueStartChangingEvent)
#       self._fractionScaleChangedTag = self.AddObserverByNumber(self._fractionScale ,vtkKWScale_ScaleValueChangedEvent)


        # observers for interaction
        self._renderWindowInteractor = slicer.ApplicationGUI.GetRenderWindowInteractor()
        self._renderWindowLeftButtonReleaseTag = self._renderWindowInteractor.AddObserver("LeftButtonReleaseEvent",self._helper.HandleClickInRenderWindow)

        self._redSliceInteractor = slicer.ApplicationGUI.GetMainSliceGUI("Red").GetSliceViewer().GetRenderWidget().GetRenderWindowInteractor()
        self._redSliceLeftButtonReleaseTag = self._redSliceInteractor.AddObserver("LeftButtonReleaseEvent",self._helper.HandleClickInRedSliceWindow)

        self._yellowSliceInteractor = slicer.ApplicationGUI.GetMainSliceGUI("Yellow").GetSliceViewer().GetRenderWidget().GetRenderWindowInteractor()
        self._yellowSliceLeftButtonReleaseTag = self._yellowSliceInteractor.AddObserver("LeftButtonReleaseEvent",self._helper.HandleClickInYellowSliceWindow)

        self._greenSliceInteractor = slicer.ApplicationGUI.GetMainSliceGUI("Green").GetSliceViewer().GetRenderWidget().GetRenderWindowInteractor()
        self._greenSliceLeftButtonReleaseTag = self._greenSliceInteractor.AddObserver("LeftButtonReleaseEvent",self._helper.HandleClickInGreenSliceWindow)

        self._helper.RegisterInteractors(self._renderWindowInteractor, self._redSliceInteractor, self._yellowSliceInteractor, self._greenSliceInteractor)



        # add observers for seeding method tabs
        for page in self._initPages:
            page.AddGUIObservers()


    def ProcessClickOnInitTabs(self):

        if self._updating != 1:

            self.UpdateMRML()

    def RemoveGUIObservers(self):

        self.RemoveObserver(self._moduleNodeSelectorSelectedTag)
        self.RemoveObserver(self._inVolumeSelectorSelectedTag)
        self.RemoveObserver(self._outFibersSelectorSelectedTag)
#       self.RemoveObserver(self._tensorTypeMenuTag)
#       self.RemoveObserver(self._stoppingModeMenuTag)
        self.RemoveObserver(self._anisotropyThresholdScaleChangingTag)
        self.RemoveObserver(self._anisotropyThresholdScaleChangedTag)

        self.RemoveObserver(self._stoppingCurvatureScaleChangingTag)
        self.RemoveObserver(self._stoppingCurvatureScaleChangedTag)

        self.RemoveObserver(self._integrationStepLengthScaleChangingTag)
        self.RemoveObserver(self._integrationStepLengthScaleChangedTag)

        self.RemoveObserver(self._minimumLengthScaleChangingTag)
        self.RemoveObserver(self._minimumLengthScaleChangedTag)

#       self.RemoveObserver(self._fractionScaleChangingTag)
#       self.RemoveObserver(self._fractionScaleChangedTag)

        self._renderWindowInteractor.RemoveObserver(self._renderWindowLeftButtonReleaseTag)
        self._redSliceInteractor.RemoveObserver(self._redSliceLeftButtonReleaseTag)
        self._yellowSliceInteractor.RemoveObserver(self._yellowSliceLeftButtonReleaseTag)
        self._greenSliceInteractor.RemoveObserver(self._greenSliceLeftButtonReleaseTag)

        for page in self._initPages:
            page.RemoveGUIObservers()


    def ProcessGUIEvents(self,caller,event):

        ### process events added to GUI and update MRML node

        if self._updating != 1:

            self._helper.debug("main ProcessGUIEvents called")
            
            if caller == self._inVolumeSelector and event == vtkSlicerNodeSelectorWidget_NodeSelectedEvent and self._inVolumeSelector.GetSelected():
                self._helper.debug("inVolume node changed, calling main UpdateMRML")
                self.UpdateMRML() # update reference to volume
#                for page in self._initPages:
#                    page.Reset()
                self.UpdateMRML() # update extent values
                self.UpdateGUI()

            elif caller == self._outFibersSelector and event == vtkSlicerNodeSelectorWidget_NodeSelectedEvent and self._outFibersSelector.GetSelected():
                self._helper.debug("outFibers node changed, calling main UpdateMRML")
                self.UpdateMRML() # update reference to volume
#                for page in self._initPages:
#                    page.Reset()
                self.UpdateMRML() # update extent values
                self.UpdateGUI()

            elif caller == self._moduleNodeSelector and event == vtkSlicerNodeSelectorWidget_NodeSelectedEvent and self._moduleNodeSelector.GetSelected():
                self._helper.debug("parameter node changed, calling main UpdateGUI")
                node = self._moduleNodeSelector.GetSelected()
                self.GetLogic().SetAndObserveScriptedModuleNode(node)
                self.SetAndObserveScriptedModuleNode(node)
                self.UpdateGUI() ### scripted module node has changed, update the GUI with the values stored to the node


            for page in self._initPages:
                page.ProcessGUIEvents(caller,event)



    def UpdateMRML(self):

        ### create the node or update all parameters from gui

        self._helper.debug("main UpdateMRML called")

        if self._updating != 1:

            node = self.GetScriptedModuleNode()
            self._updating = 1
            if not node or not self._moduleNodeSelector.GetSelected():
                self._helper.debug("no node associated to this scriptedmodule, creating new..")
                self._moduleNodeSelector.SetSelectedNew("vtkMRMLScriptedModuleNode")
                self._moduleNodeSelector.ProcessNewNodeCommand("vtkMRMLScriptedModuleNode", "TractographyParameters")
                node = self._moduleNodeSelector.GetSelected()
                # set an observe new node
                self.GetLogic().SetAndObserveScriptedModuleNode(node)
                self.SetScriptedModuleNode(node)
                self._helper.debug("new node created!")

            # save node parameters for Undo
            self.GetLogic().GetMRMLScene().SaveStateForUndo(node)


            if self._inVolumeSelector.GetSelected():
                node.SetParameter('InputVolumeRef',self._inVolumeSelector.GetSelected())
            else:
                node.SetParameter('InputVolumeRef',"None")

            if self._outFibersSelector.GetSelected():
                node.SetParameter('OutputFibersRef',self._inVolumeSelector.GetSelected())
            else:
                node.SetParameter('OutputFiberseRef',"None")

#           if input volume and output fibers not selected, lock init interface
 #           if node.GetParameter('InputVolumeRef')=="None":# or node.GetParameter('OutputFibersRef')=="None":
 #               self.UnLockInitInterface(0)
 #           else:
 #               self.UnLockInitInterface(1)
 #              self._infoLabel.SetText("Choose seeding method!..")

            if self._outInitVolume != None:
                node.SetParameter('OutputInitVolumeRef', self._outInitVolume.GetID())
            else:
                node.SetParameter('OutputInitVolumeRef', "None")

            if self._outInitVolumeLast != None:
                node.SetParameter('OutputInitVolumeLastRef', self._outInitVolumeLast.GetID())
            else:
                node.SetParameter('OutputInitVolumeLastRef', "None")


            if self._outModel != None:
                node.SetParameter('OutputModelRef', self._outModel.GetID())
            else:
                node.SetParameter('OutputModelRef', "None")

            if self._outModelDisplay != None:
                node.SetParameter('OutputModelDisplayRef', self._outModelDisplay.GetID())
            else:
                node.SetParameter('OutputModelDisplayRef', "None")


            node.SetParameter('anisotropyThreshold',self._anisotropyThresholdScale.GetWidget().GetValue()) 
            node.SetParameter('stoppingCurvature',self._stoppingCurvatureScale.GetWidget().GetValue()) 
            node.SetParameter('integrationStepLength',self._integrationStepLengthScale.GetWidget().GetValue()) 
            node.SetParameter('anisotropyThreshold',self._anisotropyThresholdScale.GetWidget().GetValue()) 
            node.SetParameter('minimumLength',self._minimumLengthScale.GetWidget().GetValue()) 
#            node.SetParameter('fraction',self._fractionScale.GetWidget().GetValue())

            if self._tensorTypeMenu.GetWidget().GetValue() == "Two-Tensor":
                node.SetParameter('tensorType',0)
            else:
                node.SetParameter('tensorType',1)

            if self._stoppingModeMenu.GetWidget().GetValue() == "FA":
                node.SetParameter('stoppingMode',0)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Cl1":
                node.SetParameter('stoppingMode',1)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Cp1":
                node.SetParameter('stoppingMode',2)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Ca1":
                node.SetParameter('stoppingMode',3)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Clpmin1":
                node.SetParameter('stoppingMode',4)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Cl2":
                node.SetParameter('stoppingMode',5)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Cp2":
                node.SetParameter('stoppingMode',6)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Ca2":
                node.SetParameter('stoppingMode',7)
            elif self._stoppingModeMenu.GetWidget().GetValue() == "Clpmin2":
                node.SetParameter('stoppingMode',8)


            ### save the current flow state
#            node.SetParameter('state',self._state)

            ### save the raised tab to the node
            node.SetParameter('raisedInitializationPageID', str(self._seedingMethodsTabs.GetRaisedPageId()))

            ### all needed values set, this is now a valid Parameters node
#            node.SetParameter('isValidNode',1)

            for page in self._initPages:
                page.UpdateMRML()

            node.RequestParameterList()
            self._helper.debug("parameterlist which was just set in main UpdateMRML " + node.GetParameterList())

            # save node parameters for Undo 
#            self.GetLogic().GetMRMLScene().SaveStateForUndo(node)

            self._updating = 0

            self._helper.debug("main UpdateMRML end")

        else:
            self._helper.debug("blocked call of UpdateMRML because updating is in progress")


    def UpdateGUI(self):


        if self._updating != 1:
            self._helper.debug("main UpdateGUI called")

            node = self.GetScriptedModuleNode()
            if node:

                tensorType = node.GetParameter('tensorType')
                if tensorType == 0:
                    self._tensorTypeMenu.GetWidget().SetValue("Two-Tensor")
                else:
                    self._tensorTypeMenu.GetWidget().SetValue("One-Tensor")

                stoppingMode = node.GetParameter('stoppingMode')
                if stoppingMode == 0:
                    self._stoppingModeMenu.GetWidget().SetValue("FA")
                elif stoppingMode == 1:
                    self._stoppingModeMenu.GetWidget().SetValue("Cl1")
                elif stoppingMode == 2:
                    self._stoppingModeMenu.GetWidget().SetValue("Cp1")
                elif stoppingMode == 3:
                    self._stoppingModeMenu.GetWidget().SetValue("Ca1")
                elif stoppingMode == 4:
                    self._stoppingModeMenu.GetWidget().SetValue("Clpmin1")
                elif stoppingMode == 5:
                    self._stoppingModeMenu.GetWidget().SetValue("Cl2")
                elif stoppingMode == 6:
                    self._stoppingModeMenu.GetWidget().SetValue("Cp2")
                elif stoppingMode == 7:
                    self._stoppingModeMenu.GetWidget().SetValue("Ca2")
                elif stoppingMode == 8:
                    self._stoppingModeMenu.GetWidget().SetValue("Clpmin2")

                anisotropyThreshold = node.GetParameter('anisotropyThreshold')
                if anisotropyThreshold:
                    self._anisotropyThresholdScale.GetWidget().SetValue(anisotropyThreshold)
                stoppingCurvature = node.GetParameter('stoppingCurvature')
                if stoppingCurvature:
                    self._stoppingCurvatureScale.GetWidget().SetValue(stoppingCurvature)
                integrationStepLength = node.GetParameter('integrationStepLength')
                if integrationStepLength:
                    self._integrationStepLengthScale.GetWidget().SetValue(integrationStepLength)
                minimumLength = node.GetParameter('minimumLength')
                if minimumLength:
                    self._minimumLengthScale.GetWidget().SetValue(minimumLength)
#                fraction = node.GetParameter('fraction')
#                if fraction:
#                    self._fractionScale.GetWidget().SetValue(fraction)

                
#                if node.GetParameter('isValidNode'):
                    ### old node
                    ### set the values of the node into the GUI

                    
                self._helper.debug("setting the MRML parameters to the GUI")

                node.RequestParameterList()
                self._helper.debug("parameterlist which was just read in main UpdateGUI " + node.GetParameterList())
                    

                # select the connected inputVolume
                if node.GetParameter('InputVolumeRef')=="None":# or node.GetParameter('OutputFibersRef')=="None":
                    self._helper.debug("no inputVolumeRef so far..")
#                    self.UnLockInitInterface(0)
                    if self._inVolumeSelector.GetSelected():# or self._outFibersSelector.GetSelected():
                        # update input volume to current selection
                        self._helper.debug("update inputVolumeRef to the current selection")
                        self._updating = 1
                        node.SetParameter('InputVolumeRef',self._inVolumeSelector.GetSelected())
                        self._updating = 0
#                        self.UnLockInitInterface(1)
                    else:
                        # no input volume at all
                        # raise first pages
                        self._updating = 1
                        node.SetParameter('raisedInitializationPageID',0)
                        self._updating = 0
#todo:baue outputVolume ein
                
                    
                if node.GetParameter('OutputInitVolumeRef')=="None" or not node.GetParameter('OutputInitVolumeRef'):
                    self._outInitVolume = None
                else:
                    if self.GetLogic().GetMRMLScene().IsNodePresent(self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputInitVolumeRef'))):
                        self._outInitVolume = self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputInitVolumeRef'))

                if node.GetParameter('OutputInitVolumeLastRef')=="None" or not node.GetParameter('OutputInitVolumeLastRef'):
                    self._outInitVolumeLast = None
                else:
                    if self.GetLogic().GetMRMLScene().IsNodePresent(self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputInitVolumeLastRef'))):
                        self._outInitVolumeLast = self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputInitVolumeLastRef'))


                if node.GetParameter('OutputModelRef')=="None" or not node.GetParameter('OutputModelRef'):
                    self._outModel = None
                else:
                    if self.GetLogic().GetMRMLScene().IsNodePresent(self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputModelRef'))):
                        self._outModel = self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputModelRef'))

                if node.GetParameter('OutputModelDisplayRef')=="None" or not node.GetParameter('OutputModelDisplayRef'):
                    self._outModelDisplay = None
                else:
                    if self.GetLogic().GetMRMLScene().IsNodePresent(self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputModelDisplayRef'))):
                        self._outModelDisplay = self.GetLogic().GetMRMLScene().GetNodeByID(node.GetParameter('OutputModelDisplayRef'))


                    #// set GUI widgest from parameter node
                    #vtkMRMLNode *s = this->GetMRMLScene()->GetNodeByID(n->GetInputVolumeRef());
                    #this->VolumeSelector->SetSelected(s);

                    #s = this->GetMRMLScene()->GetNodeByID(n->GetInputFiducialRef());
                   # this->FiducialSelector->SetSelected(s);

                    #vtkMRMLFiducialListNode *fiducialListNode = vtkMRMLFiducialListNode::SafeDownCast(s);
                    #vtkMRMLModelNode *modelNode = vtkMRMLModelNode::SafeDownCast(s);

                #raise the correct seeding tab                
                self._updating = 1
                self._seedingMethodsTabs.RaisePage(node.GetParameter('raisedInitializationPageID'))
                self._updating = 0

                for page in self._initPages:
                    page.UpdateGUI()


                ### parameter Node changed, reset interactive mode
                self._helper.SetIsInteractiveMode(0,None)


            self._helper.debug("main UpdateGUI end")



    def ProcessMRMLEvents(self,caller,event):
        if self._updating != 1:
            self._helper.debug("main ProcessMRMLEvents called, event: "+str(event))

            if caller == self.GetScriptedModuleNode() and self.GetScriptedModuleNode!=None:
                self.UpdateGUI()

    def BuildGUI(self):

        self._helper.debug("main BuildGUI called")

        # add a single new Page in its UIPanel 
        self.GetUIPanel().AddPage("Tractography","Tractography","")
        self._tractographyPage = self.GetUIPanel().GetPageWidget("Tractography")
        helpText = "Tractography in 3D Slicer"
        aboutText = "This work is supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See http://www.slicer.org for details."
        self._helpAboutFrame = self.BuildHelpAndAboutFrame(self._tractographyPage,helpText,aboutText)

        # parent the frame to the same UIpanel page
        self._topFrame.SetParent(self._tractographyPage)
        self._topFrame.Create()
        self._topFrame.SetLabelText("Tractography Parameters")
        self._topFrame.ExpandFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (self._topFrame.GetWidgetName(),self._tractographyPage.GetWidgetName()))
    
        self._moduleNodeSelector.SetNodeClass("vtkMRMLScriptedModuleNode", "ScriptedModuleName", self.GetLogic().GetModuleName(), "TractographyParameters")
        self._moduleNodeSelector.NewNodeEnabledOn()
        self._moduleNodeSelector.NoneEnabledOn()
        self._moduleNodeSelector.ShowHiddenOn()
        self._moduleNodeSelector.SetParent(self._topFrame.GetFrame())
        self._moduleNodeSelector.Create()
        self._moduleNodeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self._moduleNodeSelector.UpdateMenu()
        self._moduleNodeSelector.SetBorderWidth(2)
        self._moduleNodeSelector.SetLabelText("Module Parameters:")
        self._moduleNodeSelector.SetBalloonHelpString("Select a tractography node from the current mrml scene.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._moduleNodeSelector.GetWidgetName())

        self._inVolumeSelector.SetNodeClass("vtkMRMLDiffusionWeightedVolumeNode","","","")
        self._inVolumeSelector.SetParent(self._topFrame.GetFrame())
        self._inVolumeSelector.Create()
        self._inVolumeSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self._inVolumeSelector.UpdateMenu()
        self._inVolumeSelector.SetBorderWidth(2)
        self._inVolumeSelector.SetLabelText("Select DWI Volume:")
        self._inVolumeSelector.SetBalloonHelpString("Select an input DWI volume from the current mrml scene.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._inVolumeSelector.GetWidgetName())

        self._outFibersSelector.SetNodeClass("vtkMRMLFiberBundleNode","","","")
        self._outFibersSelector.NewNodeEnabledOn()
        self._outFibersSelector.SetParent(self._topFrame.GetFrame())
        self._outFibersSelector.Create()
        self._outFibersSelector.SetMRMLScene(self.GetLogic().GetMRMLScene())
        self._outFibersSelector.UpdateMenu()
        self._outFibersSelector.SetBorderWidth(2)
        self._outFibersSelector.SetLabelText("Output FiberBundleNode:")
        self._outFibersSelector.SetBalloonHelpString("Select an output fiber bundle node.")
        slicer.TkCall("pack %s -side top -anchor e -padx 20 -pady 4" % self._outFibersSelector.GetWidgetName())

        self._tensorTypeMenu.SetParent(self._topFrame.GetFrame())
        self._tensorTypeMenu.SetLabelText("Type of Tractography")
        self._tensorTypeMenu.Create()
        self._tensorTypeMenu.SetWidth(20)       
        self._tensorTypeMenu.SetLabelWidth(18)
        self._tensorTypeMenu.GetWidget().GetMenu().AddRadioButton("Two-Tensor")
        self._tensorTypeMenu.GetWidget().GetMenu().AddRadioButton("One-Tensor")
        self._tensorTypeMenu.GetWidget().SetValue("Two-Tensor")
        slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 4" % self._tensorTypeMenu.GetWidgetName())

        self._stoppingModeMenu.SetParent(self._topFrame.GetFrame())
        self._stoppingModeMenu.SetLabelText("Stopping Mode")
        self._stoppingModeMenu.Create()
        self._stoppingModeMenu.SetWidth(20)     
        self._stoppingModeMenu.SetLabelWidth(12)
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("FA")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Cl1")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Cp1")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Ca1")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Clpmin1")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Cl2")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Cp2")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Ca2")
        self._stoppingModeMenu.GetWidget().GetMenu().AddRadioButton ("Clpmin2")
        self._stoppingModeMenu.GetWidget().SetValue("Cl1")
        slicer.TkCall("pack %s -side top -anchor nw -expand n -padx 2 -pady 2" % self._stoppingModeMenu.GetWidgetName())

        self._anisotropyThresholdScale.SetParent(self._topFrame.GetFrame())
        self._anisotropyThresholdScale.SetLabelText("Stopping Value")
        self._anisotropyThresholdScale.Create()
#        w = self._anisotropyThresholdScale.GetScale().GetWidth()
        self._anisotropyThresholdScale.GetWidget().SetRange(0,1)
        self._anisotropyThresholdScale.GetWidget().SetResolution(0.01)
        self._anisotropyThresholdScale.GetWidget().SetValue(0.1) 
        slicer.TkCall("pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2" % self._anisotropyThresholdScale.GetWidgetName())

        self._stoppingCurvatureScale.SetParent(self._topFrame.GetFrame())
        self._stoppingCurvatureScale.SetLabelText("Stopping Track Curvature")
        self._stoppingCurvatureScale.Create()
#        self._stoppingCurvatureScale.GetScale().SetWidth(w)
        self._stoppingCurvatureScale.GetWidget().SetRange(0,6)
        self._stoppingCurvatureScale.GetWidget().SetResolution(0.1)
        self._stoppingCurvatureScale.GetWidget().SetValue(0.8) 
        slicer.TkCall("pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2" % self._stoppingCurvatureScale.GetWidgetName())

        self._integrationStepLengthScale.SetParent(self._topFrame.GetFrame())
        self._integrationStepLengthScale.SetLabelText("Integration Step Length")
        self._integrationStepLengthScale.Create()
#        self._integrationStepLengthScale.GetScale().SetWidth(w)
        self._integrationStepLengthScale.GetWidget().SetRange(0,1)
        self._integrationStepLengthScale.GetWidget().SetResolution(0.1)
        self._integrationStepLengthScale.GetWidget().SetValue(0.5) 
        slicer.TkCall("pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2" % self._integrationStepLengthScale.GetWidgetName())

        self._minimumLengthScale.SetParent(self._topFrame.GetFrame())
        self._minimumLengthScale.SetLabelText("Minimum Path Length")
        self._minimumLengthScale.Create()
#        self._minimumLengthScale.GetScale().SetWidth(w)
        self._minimumLengthScale.GetWidget().SetRange(0,200)
        self._minimumLengthScale.GetWidget().SetResolution(1)
        self._minimumLengthScale.GetWidget().SetValue(20) 
        slicer.TkCall("pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2" % self._minimumLengthScale.GetWidgetName())

#        self._fractionScale.SetParent(self._topFrame.GetFrame())
#        self._fractionScale.SetLabelText("Fraction")
#        self._fractionScale.Create()
#        self._fractionScale.GetScale().SetWidth(w)
#        self._fractionScale.GetWidget().SetRange(0,1)
#        self._fractionScale.GetWidget().SetResolution(0.1)
#        self._fractionScale.GetWidget().SetValue(0.1) 
#        slicer.TkCall("pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 2" % self._fractionScale.GetWidgetName())


#        self._infoLabel.SetParent(self._tractographyPage)
#        self._infoLabel.Create()
#        self._infoLabel.SetText("DWI volume needed!..")
#        self._infoLabel.SetImageToPredefinedIcon(20023)
#        self._infoLabel.SetCompoundModeToLeft()
#        self._infoLabel.SetPadX(2)
#        slicer.TkCall("pack %s -side top -fill x -expand n -padx 2 -pady 2 -in %s" % (self._infoLabel.GetWidgetName(),self._tractographyPage.GetWidgetName()))

        # Seeding method tabs start here
        # parent the frame to the same UIpanel page
        self._seedingMethodsFrame.SetParent(self._tractographyPage)
        self._seedingMethodsFrame.Create()
        self._seedingMethodsFrame.SetLabelText("Seeding Method")
        self._seedingMethodsFrame.ExpandFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (self._seedingMethodsFrame.GetWidgetName(),self._tractographyPage.GetWidgetName()))


        self._seedingMethodsTabs.SetParent(self._seedingMethodsFrame.GetFrame())
        self._seedingMethodsTabs.Create()
        self._seedingMethodsTabs.AddObserver(2088,self.ProcessClickOnInitTabs)

        # To create a notebook style GUI, a new Page must be added to the UIPanel for every tab in the notebook: 
        self._seedingMethodsTabs.AddPage("Welcome","An overview of the seeding methods","")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._seedingMethodsTabs.GetWidgetName())
        id = self._seedingMethodsTabs.GetFrame("Welcome")
        self._welcomePanel = SlicerWelcomeGUI(id,self)
        self._welcomePanel.BuildGUI()
        self._initPages.append(self._welcomePanel)

        self._seedingMethodsTabs.AddPage("Fiducial Seeding","Fiducial Seeding","")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._seedingMethodsTabs.GetWidgetName())
        id = self._seedingMethodsTabs.GetFrame("Fiducial Seeding")
        self._fiducialSeedingPanel = SlicerFiducialSeedingGUI(id,self)
        self._fiducialSeedingPanel.BuildGUI()
        self._initPages.append(self._fiducialSeedingPanel)

        self._seedingMethodsTabs.AddPage("ROI Seeding","ROI Seeding","")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._seedingMethodsTabs.GetWidgetName())
        id = self._seedingMethodsTabs.GetFrame("ROI Seeding")
        self._ROISeedingPanel = SlicerROISeedingGUI(id,self)
        self._ROISeedingPanel.BuildGUI()
        self._initPages.append(self._ROISeedingPanel)



    def TearDownGUI(self):

        self._helper.debug("main TearDownGUI called")

        if self.GetUIPanel().GetUserInterfaceManager():
            self.GetUIPanel().RemovePage("Tractography")

    def GetHelper(self):
        return self._helper

    def GetMyLogic(self):
        return self._logic

    def UnLockInitInterface(self,action):

        self._seedingMethodsTabs.SetEnabled(action)


    def SetUpdatingOn(self):
        self._updating = 1

    def SetUpdatingOff(self):
        self._updating = 0

    def ChangeInfoLabel(self,text):
        self._infoLabel.SetText(text)
        self._infoLabel.SetImageToPredefinedIcon(20023)
        self._infoLabel.SetCompoundModeToLeft()


