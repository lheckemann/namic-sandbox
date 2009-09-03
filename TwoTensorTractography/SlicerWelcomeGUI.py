from SlicerAdvancedPageSkeleton import SlicerAdvancedPageSkeleton
from Slicer import slicer

vtkKWPushButton_InvokedEvent = 10000

###
### welcome page (derived from skeleton)
###
class SlicerWelcomeGUI(SlicerAdvancedPageSkeleton):

    def __init__(self,parentFrame,parentClass):
        SlicerAdvancedPageSkeleton.__init__(self,parentFrame,parentClass)

        self._welcomeMessage = slicer.vtkKWTextWithHyperlinksWithScrollbars()

    def Destructor(self):
        SlicerAdvancedPageSkeleton.Destructor(self)

        self._welcomeMessage = None

    def BuildGUI(self):
        SlicerAdvancedPageSkeleton.BuildGUI(self)


        self._welcomeMessage.SetParent(self._parentFrame)
        self._welcomeMessage.Create()
        self._welcomeMessage.GetWidget().SetWrapToWord()
        self._welcomeMessage.GetWidget().SetHeight(15)
        self._welcomeMessage.GetWidget().QuickFormattingOn()
        self._welcomeMessage.SetHorizontalScrollbarVisibility(0)
        self._welcomeMessage.SetVerticalScrollbarVisibility(1)
        self._welcomeMessage.SetText("**Tractography using TEEM** (<a>http://teem.sourceforge.net/</a>):\n\n\nThe following seeding methods exist:\n\n**Fiducial Seeding**\n\n**ROI Seeding**")
        self._welcomeMessage.GetWidget().ReadOnlyOn()


        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (self._welcomeMessage.GetWidgetName(),self._parentFrame.GetWidgetName()))



