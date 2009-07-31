###
### skeleton class for each page
###
class SlicerAdvancedPageSkeleton(object):

    def __init__(self,parentFrame,parentClass):
        self._parentFrame = parentFrame
        self._parentClass = parentClass
        
    def Destructor(self):
        self._parentFrame = None
        self._parentClass = None

    def BuildGUI(self):
        pass
    
    def AddGUIObservers(self):
        pass

    def RemoveGUIObservers(self):
        pass

    def ProcessGUIEvents(self,caller,event):
        pass

    def UpdateMRML(self):
        pass

    def UpdateGUI(self):
        pass

    def HandleClickInSliceWindowWithCoordinates(self, which, coordinates):
        pass

    def Execute(self):
        pass

    def Reset(self):
        pass

#    def SetMinMaxOfThresholdSlider(self):
#        pass
