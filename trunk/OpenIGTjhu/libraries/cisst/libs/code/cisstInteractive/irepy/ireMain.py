# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):  Chris Abidin, Andrew LaMora, Peter Kazanzides
#  Created on: 2004-04-30
#  $Id: ireMain.py,v 1.25 2006/05/05 19:38:43 kapoor Exp $
#
#--- begin cisst license - do not edit ---
#
#CISST Software License Agreement(c)
#
#Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.
#
#This software ("CISST Software") is provided by The Johns Hopkins
#University on behalf of the copyright holders and
#contributors. Permission is hereby granted, without payment, to copy,
#modify, display and distribute this software and its documentation, if
#any, for research purposes only, provided that (1) the above copyright
#notice and the following four paragraphs appear on all copies of this
#software, and (2) that source code to any modifications to this
#software be made publicly available under terms no more restrictive
#than those in this License Agreement. Use of this software constitutes
#acceptance of these terms and conditions.
#
#The CISST Software has not been reviewed or approved by the Food and
#Drug Administration, and is for non-clinical, IRB-approved Research
#Use Only.
#
#IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
#ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
#DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
#EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
#THE POSSIBILITY OF SUCH DAMAGE.
#
#THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
#EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
#IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#PURPOSE, AND NON-INFRINGEMENT.
#
#THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
#PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
#
#--- end cisst license ---
#

""" ireMain.py
This module launches the Interactive Research Environment (IRE).

To launch the IRE, import this module into python and call:
>>>ireMain.launchIrePython()

To launch the IRE without the CISST Object Register, call:
>>>ireMain.launchIrePython( False )

Authors: Chris Abidin, Andrew LaMora, Peter Kazanzides
Copyright 2004-2005
ERC CISST
Johns Hopkins University
Baltimore, MD 21218
"""
###############################   SETUP  ######################################
#------------------------------------------------------
# Import standard libraries
#------------------------------------------------------
import os, imp, time, cPickle, exceptions, types
import string as String

#------------------------------------------------------
# Import what we need from the wx toolkit
#------------------------------------------------------
try:
    import wx
    import wx.py as py
except Exception, e:
    print "Could not load wxPython: ", e
    sys.exit(1)


#------------------------------------------------------
# Import IRE specific subclass modules
#------------------------------------------------------
from ireEditorNotebook import *
from ireListCtrlPanel import *
from ireLogCtrl import *
from ireWorkspace import *
from ireInputBox import *
import ireImages

# Now, see if the IRE is embedded in a C++ application.
# This can be determined by checking whether the ireLogger
# module can be imported, because ireLogger is created
# by the cisstInteractive (C++) library.
ireEmbedded = True
try:
    import ireLogger
except ImportError,e:
    ireEmbedded = False

###############################  CLASSES ######################################

#------------------------------------------------------
# ireMain
# This is the main container, it extends wx.Frame
#------------------------------------------------------
class ireMain(wx.Frame):
    
    #This is ireMain.  It loads the main window of the IRE.

    LogToDiary = False

    ObjectRegister = None
    
    Register = None
    ScopeVariables = None

    HISTORY_FILENAME = 'history'
    DIARY_FILENAME = 'diary'
    
    TYPES_TO_EXCLUDE = ['module','builtin_function_or_method','type','NoneType','classobj']

    ID_VIEW_REGISTER = wx.NewId()
    ID_VIEW_VARIABLES = wx.NewId()
    ID_VIEW_HISTORY = wx.NewId()
    ID_VIEW_LOGGER = wx.NewId()
    ID_LOADWORKSPACE = wx.NewId()
    ID_SAVEWORKSPACE = wx.NewId()
    ID_RUNINSHELL = wx.NewId()
    ID_RUNINNEWPROCESS = wx.NewId()
    ID_TRUNCATEHISTORY = wx.NewId()
    ID_LOADHISTORY = wx.NewId()
    ID_CLEARHISTORY = wx.NewId()
    ID_TESTINPUTBOX = wx.NewId()
    ID_LOAD_CISSTCOMMON = wx.NewId()
    ID_LOAD_CISSTVECTOR = wx.NewId()
    ID_LOAD_CISSTNUMERICAL = wx.NewId()
    ID_LOAD_CISSTDEVICEINTERFACE = wx.NewId()
    ID_LOAD_CISSTREALTIME = wx.NewId()

    #------------------------------------------------------
    #IRE CONSTRUCTOR
    #------------------------------------------------------
    def __init__(self, parent, id, title):

        
        # Create a frame that is 750 x 500 pixels in size (if not embedded) or
        # 750 x 700 pixels in size (if embedded).  The larger size provides more
        # room for the Register Contents and Logger Output windows, which are enabled
        # by default when the IRE is embedded in a C++ program.
        ireSize = (750, 500)
        if ireEmbedded:
           ireSize = (750, 700)
        wx.Frame.__init__(self, parent, -1, title, pos=(150, 150), size=ireSize)

        #------------------------------------------------------
        # Define and set up the menu labels
        #------------------------------------------------------
        
        menu = self.FileMenu = wx.Menu()
        menu.Append(wx.ID_NEW,    "&New \tCtrl-N",            "New file")
        menu.Append(wx.ID_OPEN,   "&Open... \tCtrl-O",        "Open file")
        menu.AppendSeparator()
        menu.Append(wx.ID_REVERT, "&Revert \tCtrl-R",         "Revert to last saved version")
        menu.Append(wx.ID_CLOSE,  "&Close \tCtrl-W",          "Close current file")
        menu.AppendSeparator()
        menu.Append(wx.ID_SAVE,   "&Save \tCtrl-S",           "Save file")
        menu.Append(wx.ID_SAVEAS, "Save &As...\tShift+Ctrl+S","Save as a different filename")
        menu.AppendSeparator()
        menu.Append(wx.ID_PRINT,  "&Print... \tCtrl+P",       "Print")
        menu.AppendSeparator()
        menu.Append(wx.ID_EXIT,   "E&xit \tAlt-X",            "Exit the IRE")
        
        menu = self.EditMenu = wx.Menu()
        menu.Append(wx.ID_UNDO,      "&Undo \tCtrl+Z",       "Undo the last action")
        menu.Append(wx.ID_REDO,      "&Redo \tCtrl+Y",       "Redo the last undone action")
        menu.AppendSeparator()
        menu.Append(wx.ID_CUT,       "Cu&t \tCtrl+X",        "Cut the selection")
        menu.Append(wx.ID_COPY,      "&Copy \tCtrl+C",       "Copy the selection")
        menu.Append(wx.ID_PASTE,     "Paste \tCtrl+V",       "Paste from clipboard")
        menu.AppendSeparator()
        menu.Append(wx.ID_SELECTALL, "Select A&ll \tCtrl+A", "Select all text")
        
        menu = self.ViewMenu = wx.Menu()
        self.MENU_VIEW_REGISTER = wx.MenuItem(menu, self.ID_VIEW_REGISTER, "Register Contents",
                                                    "View Register Contents", wx.ITEM_CHECK)
        menu.AppendItem(self.MENU_VIEW_REGISTER)
        self.MENU_VIEW_REGISTER.Check(ireEmbedded)
        self.MENU_VIEW_VARIABLES = wx.MenuItem(menu, self.ID_VIEW_VARIABLES, "Shell Variables",
                                                     "View Shell Variables", wx.ITEM_CHECK)
        menu.AppendItem(self.MENU_VIEW_VARIABLES)
        self.MENU_VIEW_VARIABLES.Check(True)
        self.MENU_VIEW_HISTORY = wx.MenuItem(menu, self.ID_VIEW_HISTORY,   "Command History",
                                                   "View Command History", wx.ITEM_CHECK)
        menu.AppendItem(self.MENU_VIEW_HISTORY)
        self.MENU_VIEW_HISTORY.Check(True)
        menu.Append(self.ID_VIEW_LOGGER,   "Logger Output",   "View C++ Logger Output",   wx.ITEM_CHECK)
        menu.Check(self.ID_VIEW_LOGGER, ireEmbedded)

        menu = self.ToolsMenu = wx.Menu()
        menu.Append(self.ID_LOADWORKSPACE, "&Load workspace... \tCtrl+1", "Load workspace from file")
        menu.Append(self.ID_SAVEWORKSPACE, "&Save workspace... \tCtrl+2", "Save workspace to file")
        menu.AppendSeparator()
        menu.Append(self.ID_RUNINSHELL, "&Run in shell \tCtrl+3", "Run script in shell")
        menu.Append(self.ID_RUNINNEWPROCESS, "Run in &new process \tCtrl+4", "Run script file in separate process")

        menu.AppendSeparator()
        menu.Append(self.ID_TRUNCATEHISTORY, "Truncate command &history... \tCtrl+5", "Archive history by line")
        menu.Append(self.ID_LOADHISTORY, "Load &command history... \tCtrl+6", "Load archived commands")
        menu.Append(self.ID_CLEARHISTORY, "C&lear command history... \tCtrl+7", "Clear your command history")
        menu.AppendSeparator()
        menu.Append(self.ID_TESTINPUTBOX, "&Test input box", "Create test input box")
        
        menu = self.ImportMenu = wx.Menu()
        menu.Append(self.ID_LOAD_CISSTCOMMON, "Import cisst&Common", "Import cisstCommon")
        menu.Append(self.ID_LOAD_CISSTVECTOR, "Import cisst&Vector", "Import cisstVector")
        menu.Append(self.ID_LOAD_CISSTNUMERICAL, "Import cisst&Numerical", "Import cisstNumerical")
        menu.Append(self.ID_LOAD_CISSTDEVICEINTERFACE, "Import cisst&DeviceInterface", "Import cisstDeviceInterface")
        menu.Append(self.ID_LOAD_CISSTREALTIME, "Import cisst&RealTime", "Import cisstRealTime")

        menu = self.HelpMenu = wx.Menu()
        menu.Append(wx.ID_ABOUT, "&About...", "About this program")

        menubar = self.MenuBar = wx.MenuBar()
        menubar.Append(self.FileMenu, "&File")
        menubar.Append(self.EditMenu, "&Edit")
        menubar.Append(self.ViewMenu, "&View")
        menubar.Append(self.ToolsMenu,"&Tools")
        menubar.Append(self.ImportMenu,"&Import")
        menubar.Append(self.HelpMenu, "&Help")
        self.SetMenuBar(menubar)

        wx.EVT_MENU(self, wx.ID_NEW, self.OnFileNew)
        wx.EVT_MENU(self, wx.ID_OPEN, self.OnFileOpen)
        wx.EVT_MENU(self, wx.ID_REVERT, self.OnFileRevert)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.OnFileClose)
        wx.EVT_MENU(self, wx.ID_SAVE, self.OnFileSave)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.OnFileSaveAs)
        #wx.EVT_MENU(self, wx.ID_NAMESPACE, self.OnFileUpdateNamespace)
        wx.EVT_MENU(self, wx.ID_PRINT, self.OnFilePrint)
        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        wx.EVT_MENU(self, wx.ID_UNDO, self.OnUndo)
        wx.EVT_MENU(self, wx.ID_REDO, self.OnRedo)
        wx.EVT_MENU(self, wx.ID_CUT, self.OnCut)
        wx.EVT_MENU(self, wx.ID_COPY, self.OnCopy)
        wx.EVT_MENU(self, wx.ID_PASTE, self.OnPaste)
        wx.EVT_MENU(self, wx.ID_SELECTALL, self.OnSelectAll)
        wx.EVT_MENU(self, self.ID_VIEW_REGISTER, self.OnViewRegister)
        wx.EVT_MENU(self, self.ID_VIEW_VARIABLES, self.OnViewVariables)
        wx.EVT_MENU(self, self.ID_VIEW_HISTORY, self.OnViewHistory)
        wx.EVT_MENU(self, self.ID_VIEW_LOGGER, self.OnViewLogger)
        wx.EVT_MENU(self, self.ID_LOADWORKSPACE, self.OnLoadWorkspace)
        wx.EVT_MENU(self, self.ID_SAVEWORKSPACE, self.OnSaveWorkspace)
        wx.EVT_MENU(self, self.ID_RUNINSHELL, self.OnRunInShell)
        wx.EVT_MENU(self, self.ID_RUNINNEWPROCESS, self.OnRunInNewProcess)
        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)
        wx.EVT_MENU(self, self.ID_TRUNCATEHISTORY, self.OnTruncateHistory)
        wx.EVT_MENU(self, self.ID_LOADHISTORY, self.OnLoadHistory)
        wx.EVT_MENU(self, self.ID_CLEARHISTORY, self.OnClearHistory)
        wx.EVT_MENU(self, self.ID_TESTINPUTBOX, self.OnTestInputBox)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTCOMMON,  self.OnImportCisstCommon)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTVECTOR,  self.OnImportCisstVector)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTNUMERICAL,  self.OnImportCisstNumerical)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTDEVICEINTERFACE,  self.OnImportCisstDeviceInterface)
        wx.EVT_MENU(self, self.ID_LOAD_CISSTREALTIME,  self.OnImportCisstRealTime)

        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_CUT, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_COPY, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_PASTE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, wx.ID_SELECTALL, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_RUNINSHELL, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_RUNINNEWPROCESS, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTCOMMON, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTVECTOR, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTNUMERICAL, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTDEVICEINTERFACE, self.OnUpdateMenu)
        wx.EVT_UPDATE_UI(self, self.ID_LOAD_CISSTREALTIME, self.OnUpdateMenu)
        #wx.EVT_UPDATE_UI(self, self.ID_TRUNCATEHISTORY, self.OnTruncateHistory)
        
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.CreateStatusBar()
        
        #------------------------------------------------------
        # Build and instantiate the toolbar
        #------------------------------------------------------

        ToolBar = self.CreateToolBar( wx.TB_HORIZONTAL | wx.NO_BORDER | wx.TB_FLAT | wx.TB_TEXT )
        # Set up the toolbar, using bitmaps from ireImages.py.  Note that an alternative is to 
        # use wx.ArtProvider.GetBitmap to get the wxWidgets default bitmaps:
        #    tsize = (16,16)
        #    new_bmp =  wx.ArtProvider.GetBitmap(wx.ART_NEW, wx.ART_TOOLBAR, tsize)
        #    open_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_OPEN, wx.ART_TOOLBAR, tsize)
        #    save_bmp = wx.ArtProvider.GetBitmap(wx.ART_FILE_SAVE, wx.ART_TOOLBAR, tsize)
        ToolBar.AddSimpleTool(wx.ID_NEW,  ireImages.getNewItemBitmap(), "New", "New file")
        ToolBar.AddSimpleTool(wx.ID_OPEN, ireImages.getOpenItemBitmap(), "Open", "Open file...")
        ToolBar.AddSimpleTool(wx.ID_SAVE, ireImages.getSaveItemBitmap(), "Save", "Save file")

        #ToolBar.AddSeparator()
        # Could add other tools here, such as ones for loading cisstCommon,
        # cisstVector, cisstNumerical, ...

        ToolBar.Realize()

  
        #*************** Subdivide the frame into multiple windows ***************************
        #

        # First, create a horizontal splitter window (MainSplitter).  The bottom pane is
        # used for the logger output.  The top pane (TopSplitter) will be split further.

        self.MainSplitter = wx.SplitterWindow(self, -1)
        self.TopSplitter = wx.SplitterWindow(self.MainSplitter, -1)
        self.LoggerPanel = ireLogCtrl(self.MainSplitter, -1, 'Logger Output', 
                              style = wx.TE_READONLY | wx.HSCROLL | wx.TE_MULTILINE | wx.TE_RICH)
        # Only show logger window if IRE is embedded.
        if ireEmbedded:
            self.MainSplitter.SplitHorizontally(self.TopSplitter, self.LoggerPanel, -100)
        else:
            self.MainSplitter.Initialize(self.TopSplitter)
            self.LoggerPanel.Show(False)
        self.MainSplitter.SetMinimumPaneSize(100)
        # Sash gravity of 1.0 means that only the TopSplitter will be resized when the entire
        # frame is resized.  Note that SetSashGravity is not available in older versions
        # of wxWidgets (before 2.5.4).
        if 'SetSashGravity' in dir(self.MainSplitter):
            self.MainSplitter.SetSashGravity(1.0)

        # Now, split the top splitter into left and right panes.  The right
        # pane is used for the shell/notebook.  The left pane (panel) will contain three
        # sub-panels for the Register Contents, Shell Variables and Command History.

        self.LeftPanel = wx.Panel(self.TopSplitter, -1)
        self.EditorNotebook = ireEditorNotebook(self.TopSplitter, { 'ireFrame':self })
        self.TopSplitter.SplitVertically(self.LeftPanel, self.EditorNotebook, 200)
        self.TopSplitter.SetMinimumPaneSize(200)

        # Finally, "split" the left panel into the three sub-panels.  This is done using wx.BoxSizer,
        # rather than wx.SplitterWindow because there were a lot of problems getting the latter to work
        # correctly on all platforms.  Note that the three sub-panels are not split evenly:  the Command
        # History is allocated 40% of the space, whereas the other two panels are each allocated 30%.

        self.RegisterContentsListCtrl = ireListCtrlPanel(self.LeftPanel, 'Register Contents', ['Identifier', 'Type'])
        self.ScopeVariablesListCtrl = ireListCtrlPanel(self.LeftPanel, 'Shell Variables',  ['Identifier', 'Type'])
        self.CommandHistoryListCtrl = ireListCtrlPanel(self.LeftPanel, 'Command History', ['Commands'], self.LoadHistoryFromFile())
        self.LeftBoxSizer = wx.BoxSizer(wx.VERTICAL)
        self.LeftBoxSizer.Add(self.RegisterContentsListCtrl, 3, wx.EXPAND)
        self.LeftBoxSizer.Add(self.ScopeVariablesListCtrl, 3, wx.EXPAND)
        self.LeftBoxSizer.Add(self.CommandHistoryListCtrl, 4, wx.EXPAND)
        self.LeftPanel.SetSizer(self.LeftBoxSizer)

        # Don't show Register Contents unless IRE is embedded (the user
        # can choose to display it later).
        self.LeftBoxSizer.Show(self.RegisterContentsListCtrl, ireEmbedded)

        #*************************** Set up Drag and Drop ***************************

        # Enable the Scope Variables list as a source for drag and drop
        # (i.e., to drag variable names to the shell in order to print their
        # current values).
        self.ScopeVariablesListCtrl.EnableDropSource()
        
        # Enable the Command History list as a source for drag and drop
        # (i.e., to drag lines from the command history list to the shell)
        self.CommandHistoryListCtrl.EnableDropSource()

        #*********************** Initialize the Command History***********************

        self.CommandHistoryListCtrl.AddIndexedItem(time.strftime("--- %a, %d %b %Y, %I:%M %p ---", time.localtime()))
        lastcmd = self.CommandHistoryListCtrl.list.GetItemCount()-1
        self.CommandHistoryListCtrl.list.Focus( lastcmd  )
        self.CommandHistoryListCtrl.list.Select( lastcmd , 1 )
        self.CommandHistoryListCtrl.Refresh()
  
        #******************** Set up callback functions for the shell ****************

        self.Shell = self.EditorNotebook.Shell
        self.Shell.SetPythonCommandHandler(self.PythonCommandEntered)
        self.Shell.SetDiaryHandlers(self.DiaryOn, self.DiaryOff)

        #********************************* Set up logger *****************************

        if ireEmbedded:
            self.LoggerPanel.EnableLogger()
    
        #******************************** Misc. other setup **************************

        # Set minimum size for entire frame to be at least twice the minimum pane size.
        # Note that (ireSize[x]-clientSize[x]) is added to account for the borders,
        # menu bar, tool bar, and status line.
        clientSize = self.GetClientSizeTuple()
        self.SetSizeHints(2*self.TopSplitter.GetMinimumPaneSize() + (ireSize[0]-clientSize[0]),
                          2*self.MainSplitter.GetMinimumPaneSize() + (ireSize[1]-clientSize[1]))

        self.CheckLists()
        
        Width = 10*(self.RegisterContentsListCtrl.list.GetColumnWidth(0) +
            self.RegisterContentsListCtrl.list.GetColumnWidth(1))/2
        self.RegisterContentsListCtrl.list.SetColumnWidth(0, Width)
        self.ScopeVariablesListCtrl.list.SetColumnWidth(0, Width)

        
  #------------------------------------------------------
  # addVariablesToRegister
  #
  # Parse the input Data for variables that are to be added
  # to the Object Register.
  # NOTE: affected by changes to the Object Register
  #------------------------------------------------------
    def AddVariablesToRegister(self, Data):
        #for VariableName in Data:
            #self.ObjectRegister.Register(VariableName, self.Shell.interp.locals[VariableName])
        self.CheckRegisterContents()

  #------------------------------------------------------
  # GetVariablesFromRegister
  #
  # Calls RegisterGet to explicitly fetch variables 
  # contained in the input argument "Data" (which is
  # itself populated by List() or used to be till anton killed it)
  #------------------------------------------------------
    def GetVariablesFromRegister(self, Data):
        for VariableName in Data:
            self.Shell.interp.locals[VariableName] = FindObject(VariableName)
        self.CheckScopeVariables()

  #------------------------------------------------------
  # The following functions handle command events
  # in the notebook shell.
  #------------------------------------------------------

    def PythonCommandEntered(self, Command):
        if len(Command.strip()) > 0:
            Lines = Command.splitlines()
            for Line in Lines:
                Line = Line.split('#', 1)[0]  # remove comment
                Line = Line.rstrip()          # remove trailing whitespace
                if Line:
                    self.CommandHistoryListCtrl.AddIndexedItem(Line)
            if self.LogToDiary:
                self.WriteToDiary(Lines)
          self.CheckLists()

    def OnImportCisstCommon(self, event):
        self.ImportCisstCommon()
    def ImportCisstCommon(self):
        print "importing cisstCommon"
    self.Shell.push("from cisstCommonPython import *")
            
    def OnImportCisstVector(self, event):
        self.ImportCisstVector()
    def ImportCisstVector(self):
        print "importing cisstVector"
    self.Shell.push("from cisstVectorPython import *")

    def OnImportCisstNumerical(self, event):
        self.ImportCisstNumerical()
    def ImportCisstNumerical(self):
        print "importing cisstNumerical"
    self.Shell.push("from cisstNumericalPython import *")

    def OnImportCisstDeviceInterface(self, event):
        self.ImportCisstDeviceInterface()
    def ImportCisstDeviceInterface(self):
        print "importing cisstDeviceInterface"
    self.Shell.push("from cisstDeviceInterfacePython import *")

    def OnImportCisstRealTime(self, event):
        self.ImportCisstRealTime()
    def ImportCisstRealTime(self):
        print "importing cisstRealTime"
    self.Shell.push("from cisstRealTimePython import *")

  #------------------------------------------------------
  # Diary Functions
  #
  # Handle enabling, recording to and fetching from the 
  # statement diary.  Currently not used.
  #------------------------------------------------------
    def WriteToDiary(self, Lines):
        DiaryFile = open(self.DIARY_FILENAME, 'a')
        for Line in Lines:
            DiaryFile.write(Line + os.linesep)
        DiaryFile.close()


    def DiaryOn(self):
        self.Shell.write('\nDiary logging enabled')
        DiaryFile = open(self.DIARY_FILENAME, 'a')
        DiaryFile.write(
            os.linesep + '# Diary started ' +
            time.strftime("%a, %d %b %Y, %H:%M:%S", time.localtime()) +
            os.linesep
        )
        DiaryFile.close()
        self.LogToDiary = True;


    def DiaryOff(self):
        self.Shell.write('\nDiary logging disabled')
        self.LogToDiary = False;


    def CheckLists(self):
        if not self.ObjectRegister:
            if sys.modules.has_key('cisstCommonPython'):
                import cisstCommonPython
                self.ObjectRegister = cisstCommonPython.cmnObjectRegister
        if self.ObjectRegister:
            self.CheckRegisterContents()
        self.CheckScopeVariables()
        self.CommandHistoryListCtrl.SortList()


    def CheckRegisterContents(self):
        CurrentRegister = String.split(self.ObjectRegister.ToString())
        if CurrentRegister != self.Register:
            self.UpdateList(self.RegisterContentsListCtrl, self.GetRegisterContents(self.TYPES_TO_EXCLUDE))
            self.Register = CurrentRegister

    def CheckScopeVariables(self):
        CurrentScopeVariables = self.Shell.interp.locals.keys()   
        if CurrentScopeVariables != self.ScopeVariables:
            self.UpdateList(self.ScopeVariablesListCtrl, self.GetScopeVariables(self.TYPES_TO_EXCLUDE))
            self.ScopeVariables = CurrentScopeVariables


    def UpdateList(self, ListCtrl, Data):
        ListCtrl.ClearItems()
        for i in Data.keys():
            ListCtrl.AddTupleItem( (i,Data[i]) )
            ListCtrl.SortList()


    def GetRegisterContents(self, TypesToExclude=[]):
        Contents = {}
        VariableName = ""
        VariableType = ""
        for tok in String.split(self.ObjectRegister.ToString(), " "):
            #if starts with lparen, is type, else is name
            if tok.find('(')==0:  #this is a variabletype
                VariableType = tok[1:tok.rfind(')')]
                if VariableType not in TypesToExclude:
                    Contents.update( {VariableName:VariableType} )
            else:   #this is a variablename
                VariableName = tok
        return Contents


    def GetScopeVariables(self, TypesToExclude=[]):
        Variables = {}
        for VariableName in self.Shell.interp.locals.keys():
            if not self.IsDecorated(VariableName):
                VariableType = self.GetVariableTypeString(VariableName)
                if VariableType not in TypesToExclude:
                  Variables.update({VariableName:VariableType})
        return Variables


    def IsDecorated(self, String):
        return String[0:2] == '__' and String[len(String)-2:len(String)] == '__'


    def GetRegisterTypeString(self, VariableName):
        return self.GetTypeString("RegisterGet('" + VariableName + "')")


    def GetVariableTypeString(self, VariableName):
        return self.GetTypeString("vars(self.Shell.interp)['locals']['" + VariableName + "']")


    def GetTypeString(self, _Str):
        exec("VariableType = str(type(" + _Str + "))")
        return VariableType[VariableType.find("'")+1:VariableType.rfind("'")]

  
  #-------------------------------------------------
  # Methods for manipulating/displaying/truncating the 
  # command history 
  #-------------------------------------------------

    def SaveHistoryToFile(self):
        cmdlist = self.CommandHistoryListCtrl.GetAllItems()
        f = open(self.HISTORY_FILENAME, 'w')
        cPickle.dump(cmdlist, f)
    
    def LoadHistoryFromFile(self, fn=HISTORY_FILENAME):
        Data = []
        if os.path.isfile(fn):
            try:
                Data = cPickle.load(open(fn))
            except exceptions.Exception, error:
                msgdlg = wx.MessageDialog(self, str(error), "Load History", wx.OK | wx.ICON_ERROR)
                msgdlg.ShowModal()
                msgdlg.Destroy()
        # Make sure command history is a list of strings.  Note that an empty
        # list is not considered to be an error.
        file_error = False
        if isinstance(Data, types.ListType):
            if len(Data) > 0 and not isinstance(Data[0], types.StringTypes):
                file_error = True   # list does not contain strings
        else:
            file_error = True       # not a list
        if file_error:
            text = "Invalid command history file: " + fn
            msgdlg = wx.MessageDialog(self, text, "Load History", wx.OK | wx.ICON_ERROR)
            msgdlg.ShowModal()
            msgdlg.Destroy()
            Data = []
        return Data
    
    def ClearHistoryFile(self):
        """Remove all command history entries from the display and file."""
        text = "Are you sure you want to clear your command history?"
        title = "Clear Command History"
        dlg = wx.MessageDialog( self, text, title, wx.OK|wx.CANCEL)
        if dlg.ShowModal() == wx.ID_OK:
            #clear the history panel
            self.CommandHistoryListCtrl.ClearItems()
            #clear the history file
            self.SaveHistoryToFile()
        dlg.Destroy()
        
            
    def QueryHistoryFile(self):
        """Append to the command history from the selected history file (*.hist)."""
        # Use os.getcwd() instead of '.' to avoid gtk assertion on Linux.
        dlg = wx.FileDialog( self, "Select a Command Archive to Load:", os.getcwd(), "", \
                             "IRE Command Histories (*.hist)|*.hist|All Files (*.*)|*.*", wx.OPEN)
        # show the dialog and process it
        if dlg.ShowModal() == wx.ID_OK:
            fn = dlg.GetFilename()
            Data = self.LoadHistoryFromFile( fn )
            for n in Data:
                self.CommandHistoryListCtrl.AddIndexedItem(n)
            self.CommandHistoryListCtrl.SortList()
        dlg.Destroy()
        
        
    def TruncateHistoryFile(self):
        """Delete specified commands from Command History.  Deleted commands can (optionally)
           be archived to a new or existing history file (*.hist)."""
        #some vars
        title = "Truncate Command History"
        archlist = []
        # fetch the command history list
        cmdlist = self.CommandHistoryListCtrl.GetAllItems()
        # query the user for a choice using the simple choice dialog
        # Archived commands can be saved to a file, but will be deleted from Command History window
        cdlg = wx.SingleChoiceDialog(self, "Archive/delete up to this line, inclusive:", \
                                     title, cmdlist, wx.OK|wx.CANCEL)
        # display dlg but exit, if user pressed cancel
        if cdlg.ShowModal() == wx.ID_CANCEL:
            cdlg.Destroy()
            return
            
        # fetch the user's choice
        choice = cdlg.GetSelection()
        choice = int(choice + 1)
        cdlg.Destroy()

        # query the user for a file name, and save it if one is entered.
        # Use os.getcwd() instead of '.' to avoid gtk assertion on Linux.
        idlg = wx.FileDialog( None, "Save/append to archive file:", os.getcwd(), "", \
                              "IRE Command Histories (*.hist)|*.hist|All Files (*.*)|*.*", wx.SAVE)
        if idlg.ShowModal() == wx.ID_OK:
            fn = idlg.GetFilename()
            # Some systems don't automatically append the .hist extension
            (name, ext) = os.path.splitext(fn)
            if not ext:
               fn += '.hist'
            # write the command file up to the selected line into the named file
            # fetch any existing list first and append to it
            try:
                archlist = cPickle.load(open(fn))
            except IOError:
                archlist = []
            n=0
            for n in range(choice):
                archlist.append( cmdlist[n] )
            cPickle.dump( archlist, open(fn, 'w'))
            text = "Your command history was archived to " + fn
            msgdlg = wx.MessageDialog(self, text, title, wx.OK | wx.ICON_INFORMATION)
            msgdlg.ShowModal()
            msgdlg.Destroy()
            
        #now delete the entries from the list
        del cmdlist[0:choice]

        #clear the panel
        self.CommandHistoryListCtrl.ClearItems()
        #add only the new items back
        for line in cmdlist:
            self.CommandHistoryListCtrl.AddIndexedItem(line)
        self.CommandHistoryListCtrl.SortList()
            
        #clobber the main history file
        self.SaveHistoryToFile()
        idlg.Destroy()
        
    
    #-------------------------------------------------
    # Event handlers for menu selections
    #-------------------------------------------------
    
    def OnCloseWindow(self, evt):
        self.SaveHistoryToFile()
        self.LoggerPanel.DisableLogger()
        #self.Close(True)
        self.Destroy()

    # Disable changes to the sash (if this handler
    # is bound to EVT_SPLITTER_SASH_POS_CHANGING)
    def OnSashChanging(self, Event):
        Event.SetSashPosition(-1)

    def OnToolClick(self, Event):
        pass

    def OnFileNew(self, event):
        self.EditorNotebook.bufferNew()

    def OnFileOpen(self, event):
        self.EditorNotebook.bufferOpen()

    def OnFileRevert(self, event):
        self.EditorNotebook.bufferRevert()

    def OnFileClose(self, event):
        self.EditorNotebook.bufferClose()

    def OnFileSave(self, event):
        self.EditorNotebook.bufferSave()

    def OnFileSaveAs(self, event):
        self.EditorNotebook.bufferSaveAs()

    def OnFileUpdateNamespace(self, event):
        self.EditorNotebook.updateNamespace()

    def OnFilePrint(self, event):
        self.EditorNotebook.bufferPrint()

    def OnExit(self, event):
        self.LoggerPanel.DisableLogger()
        self.Close(False)
        #self.Destroy()

    def OnUndo(self, event):
        wx.Window_FindFocus().Undo()

    def OnRedo(self, event):
        wx.Window_FindFocus().Redo()

    def OnCut(self, event):
        wx.Window_FindFocus().Cut()

    def OnCopy(self, event):
        wx.Window_FindFocus().Copy()

    def OnPaste(self, event):
        wx.Window_FindFocus().Paste()

    def OnSelectAll(self, event):
        wx.Window_FindFocus().SelectAll()

  #-------------------------------------------------
  # View menu handlers
  #-------------------------------------------------

    # Check the status of the left panel (Register Contents, Shell Variables,
    # and Command History).  If all are disabled, then unsplit TopSplitter so
    # that there is more room for the shell.  If any of them is enabled, make
    # sure that TopSplitter is split.
    def CheckLeftPanel(self):
        if self.MENU_VIEW_REGISTER.IsChecked() or \
           self.MENU_VIEW_VARIABLES.IsChecked() or \
           self.MENU_VIEW_HISTORY.IsChecked():
            if not self.TopSplitter.IsSplit():
                self.TopSplitter.SplitVertically(self.LeftPanel, self.EditorNotebook, 200)
                # Following needed for Mac OS X
                self.LeftPanel.Show()
        else:
            if self.TopSplitter.IsSplit():
                self.TopSplitter.Unsplit(self.LeftPanel)
  
    def OnViewRegister(self, event):
        self.LeftBoxSizer.Show(self.RegisterContentsListCtrl, event.IsChecked())
        self.CheckLeftPanel()
        self.LeftBoxSizer.Layout()

    def OnViewVariables(self, event):
        self.LeftBoxSizer.Show(self.ScopeVariablesListCtrl, event.IsChecked())
        self.CheckLeftPanel()
        self.LeftBoxSizer.Layout()
    
    def OnViewHistory(self, event):
        self.LeftBoxSizer.Show(self.CommandHistoryListCtrl, event.IsChecked())
        self.CheckLeftPanel()
        self.LeftBoxSizer.Layout()
    
    def OnViewLogger(self, event):
        if event.IsChecked():
            self.MainSplitter.SplitHorizontally(self.TopSplitter, self.LoggerPanel, -100)
            # Following needed for Mac OS X
            self.LoggerPanel.Show()
        else:
            self.MainSplitter.Unsplit(self.LoggerPanel)

  #-------------------------------------------------
  # Workspace maintenance methods
  # (also event handlers)
  #-------------------------------------------------
  
    def OnLoadWorkspace(self, event):
        Filepath = py.editor.openSingle(directory='',wildcard='IRE Workspace (*.ws)|*.ws|All Files (*.*)|*.*').path
        if Filepath:
            File = open(Filepath)
            LoadWorkspaceFile(self.Shell.interp.locals, File)
            File.close()
            self.CheckScopeVariables()

    def OnSaveWorkspace(self, event):
        Filepath = py.editor.saveSingle(directory='',wildcard='IRE Workspace (*.ws)|*.ws|All Files (*.*)|*.*').path
        if Filepath:
            # Some systems don't automatically append the .ws extension
            (name, ext) = os.path.splitext(Filepath)
            if not ext:
               Filepath += '.ws'
            File = open(Filepath,'w')
            SaveWorkspaceToFile(self.Shell.interp.locals, File)
            File.close()
  
    def OnLoadHistory(self, event):
        self.QueryHistoryFile()
      
    def OnTruncateHistory(self, event):
        self.TruncateHistoryFile()
    
    def OnClearHistory(self, event):
        self.ClearHistoryFile()

    def OnTestInputBox(self, event):        
        frame = ireInputBox(None, -1, "Test Size/Position")
        frame.Show(True)
        
    #-------------------------------------------------
  # Methods for running scripts in the main or 
  # separate threads.
  #-------------------------------------------------
  
    def OnRunInShell(self, event):
        cancel = self.EditorNotebook.bufferSave()
        if not cancel:
           self.Shell.runfile(self.EditorNotebook.buffer.doc.filepath)

    def OnRunInNewProcess(self, event):
        # Don't do this when embedded -- we may not really want to run
        # another copy of the entire program.
        if not ireEmbedded:
            cancel = self.EditorNotebook.bufferSave()
            if not cancel:
                str = os.sys.executable + ' "' + self.EditorNotebook.buffer.doc.filepath + '"'
                os.system(str)

    
  #-------------------------------------------------
  # The obligatory About
  #-------------------------------------------------
    def OnAbout(self, event):
        """Display an About window."""
        title = 'About'
        twidth = 55
        text = \
'Interactive Research Environment (IRE)'.center(twidth) + '\n' + \
'The IRE provides a Python-based command interpreter'.center(twidth) + '\n\n' + \
'Developed by the Engineering Research Center for'.center(twidth) + '\n' + \
'Computer-Integrated Surgical Systems & Technology (CISST)'.center(twidth) + '\n' + \
'http://cisst.org'.center(twidth) + '\n\n' + \
'Copyright (c) 2004-2006, The Johns Hopkins University'.center(twidth) + '\n' + \
'All Rights Reserved.\n\n'.center(twidth) + '\n\n' + \
'Based on the Py module of wxPython:\n' + \
'  Shell Revision: %s\n' % self.Shell.revision + \
'  Interpreter Revision: %s\n\n' % self.Shell.interp.revision + \
'  Platform: %s\n' % sys.platform + \
'  Python Version: %s\n' % sys.version.split()[0] + \
'  wxPython Version: %s\n' % wx.VERSION_STRING

        dialog = wx.MessageDialog(self, text, title, wx.OK | wx.ICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnUpdateMenu(self, event):
        win = wx.Window_FindFocus()
        id = event.GetId()
        event.Enable(True)
        try:
            if id == wx.ID_NEW:
                event.Enable(hasattr(self.EditorNotebook, 'bufferNew'))
            elif id == wx.ID_OPEN:
                event.Enable(hasattr(self.EditorNotebook, 'bufferOpen'))
            elif id == wx.ID_REVERT:
                event.Enable(hasattr(self.EditorNotebook, 'bufferRevert') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_CLOSE:
                event.Enable(hasattr(self.EditorNotebook, 'bufferClose') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_SAVE:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSave') and self.EditorNotebook.bufferHasChanged())
            elif id == wx.ID_SAVEAS:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSaveAs') and self.EditorNotebook.hasBuffer())
            #elif id == wx.ID_NAMESPACE:
            #    event.Enable(hasattr(self.EditorNotebook, 'updateNamespace') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_PRINT:
                event.Enable(hasattr(self.EditorNotebook, 'bufferPrint') and self.EditorNotebook.hasBuffer())
            elif id == wx.ID_UNDO:
                event.Enable(win.CanUndo())
            elif id == wx.ID_REDO:
                event.Enable(win.CanRedo())
            elif id == wx.ID_CUT:
                event.Enable(win.CanCut())
            elif id == wx.ID_COPY:
                event.Enable(win.CanCopy())
            elif id == wx.ID_PASTE:
                event.Enable(win.CanPaste())
            elif id == wx.ID_SELECTALL:
                event.Enable(hasattr(win, 'SelectAll'))
            elif id == self.ID_RUNINSHELL:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSaveAs') and self.EditorNotebook.hasBuffer())
            # Don't allow the "Run in New Thread" option when embedded
            # (otherwise, tries to start a new copy of the entire program).
            elif id == self.ID_RUNINNEWPROCESS:
                event.Enable(hasattr(self.EditorNotebook, 'bufferSaveAs') and self.EditorNotebook.hasBuffer() and not ireEmbedded)
            elif id == self.ID_LOAD_CISSTCOMMON:
                event.Enable(ModuleAvailable('cisstCommonPython'))
            elif id == self.ID_LOAD_CISSTVECTOR:
                event.Enable(ModuleAvailable('cisstVectorPython'))
            elif id == self.ID_LOAD_CISSTNUMERICAL:
                event.Enable(ModuleAvailable('cisstNumericalPython'))
            elif id == self.ID_LOAD_CISSTDEVICEINTERFACE:
                event.Enable(ModuleAvailable('cisstDeviceInterfacePython'))
            elif id == self.ID_LOAD_CISSTREALTIME:
                event.Enable(ModuleAvailable('cisstRealTimePython'))
            else:
                event.Enable(False)
        except AttributeError:
            event.Enable(False)

# ModuleAvailable:  returns true if the module 'name' is present on the path.
def ModuleAvailable(name):
    try:
       imp.find_module(name)
    except ImportError:
       return False
    return True

def launchIrePython():

    if wx.VERSION[0] <= 2 and wx.VERSION[1] < 5:
        print 'IRE requires wxPython version 2.5 or greater'
        print 'Currently installed version is ', wx.VERSION_STRING
    else:
        print "starting window "
        app = wx.PySimpleApp()
        try:
            frame = ireMain(None, -1, "CISST Interactive Research Environment")
        except Exception, e:
            print e
        frame.Show(True)
        # Redraw the left panel.  This fixes some minor display problems
        # (e.g., stray characters) on some platforms.
        frame.LeftBoxSizer.Layout()
        try:
            # Load cisstCommon by default (if IRE embedded)
            if ireEmbedded:
                frame.ImportCisstCommon()
            # Run startup string (if provided)
            if len(sys.argv) > 1 and sys.argv[1]:
                frame.Shell.push(sys.argv[1])
        except Exception, e:
            print e
        app.MainLoop()


# $Log: ireMain.py,v $
# Revision 1.25  2006/05/05 19:38:43  kapoor
# IRE: Added menus to load cisstDeviceInterface and cisstRealTime
#
# Revision 1.24  2006/02/06 20:35:11  pkaz
# IRE:  changed ireShell to explicitly use callback functions rather than overriding the Bind method
# (this was a bad implementation that broke in wxPython 2.6.2.1).  Also, did some minor display cleanup.
#
# Revision 1.23  2006/01/27 09:34:18  pkaz
# ireMain.py:  don't store comments in command history.
#
# Revision 1.22  2006/01/23 03:35:36  pkaz
# IRE:  Changed "run in new thread" to "run in new process".
# Ensure wxPython version is at least 2.5.  Improved error checking.
#
# Revision 1.21  2006/01/20 23:10:11  pkaz
# IRE:  Disable "Run in New Thread" when embedded.  Add file extensions for saved history, workspace and Python files.
#
# Revision 1.20  2006/01/20 06:08:26  pkaz
# IRE:  Better (more portable) implementation of view selections for left panel.
# Allow Object Register to be used when IRE is not embedded (when user loads cisstCommonPython).
#
# Revision 1.19  2006/01/19 06:11:17  pkaz
# IRE:  portability fixes -- handle Unicode versions; also older wxPython versions (no SetSashGravity).
#
# Revision 1.18  2006/01/17 22:41:56  pkaz
# IRE:  Added ability to directly call (wrapped) C function in wxPython DLL to display cmnLogger output.
#
# Revision 1.17  2006/01/17 07:52:25  pkaz
# IRE:  Added LoD control and enable/disable checkbox to logger panel.
# Complete rewrite of ireLogCtrl.py.
#
# Revision 1.16  2006/01/16 08:13:08  pkaz
# IRE:  Re-implemented layout of IRE panels.  Added bottom panel for logger output.
# Use wx.Sizer for register contents, variables and history (instead of wx.SplitterWindow).
# Added View menu to allow user to show/hide most panels.
#
# Revision 1.15  2006/01/15 06:03:06  pkaz
# Added ireFrame (instance of main IRE window) to shell local variables to aid debugging.
#
# Revision 1.14  2006/01/14 08:55:19  pkaz
# ireMain.py:  fixed IRE to also work in standalone mode (i.e., when not started from C++).
#
# Revision 1.13  2006/01/11 16:22:18  pkaz
# IRE: Added character string parameter to LaunchIREShell(), which gets executed when Python shell is started.
#
# Revision 1.12  2006/01/10 09:25:08  pkaz
# IRE: Added display of cmnLogger output.  Work in process.
#
# Revision 1.11  2006/01/06 21:02:32  pkaz
# IRE: Added toolbar bitmaps for new/open/save.  Bitmap images are in ireImages.py.
# This resolves item 1 in #174.
#
# Revision 1.10  2006/01/06 19:47:41  pkaz
# IRE:  Fixed column sorting for command history, register contents and scope variables.
# Added up/down arrows to indicate sort directions.
#
# Revision 1.9  2006/01/04 22:43:33  pkaz
# IRE:  improved implementation of ireListCtrlPanel and command history.
#
# Revision 1.8  2006/01/02 07:23:46  pkaz
# ireMain.py:  Added SetSizeHints to specify minimum frame (window) size;
# allow resizing of panel sashes (some limitations on Linux), improve display of About dialog on Linux.
#
# Revision 1.7  2006/01/02 04:17:44  pkaz
# IRE:  new (correct) implementation of drag and drop functionality.
# This check-in resolves item 6 in #174 (drag and drop on Linux).
#
# Revision 1.6  2005/12/29 16:37:41  pkaz
# ireMain.py: fixed display of Register Contents and Variables panels on Linux.
#
# Revision 1.5  2005/12/20 21:19:18  pkaz
# ireMain.py:  Major cleanup, addressing many issues in ticket #174.
# Added Import menu for cisst libraries.  Removed entries for cisst libraries in Help menu
# (did not fit on screen anyway).  Only create toolbar if images directory exists.
# Changed About window.  Improved robustness and other cleanup.
#
# Revision 1.4  2005/12/12 06:38:44  pkaz
# ireMain.py:  documentation changes; commented out some (apparently) dead code; fixed button to load cisstVector and added button to load cisstNumerical.
#
# Revision 1.3  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.2  2005/09/06 22:43:33  alamora
# irepy module: updated for cvs logging info, cisst license
#
# Revision 1.1  2005/09/06 22:30:03  alamora
# irepy module: revision 0.10 initial checkin
#
# Revision 1.4  2005/04/06 22:10:51  alamora
# Formatting Changes
#
# Revision 1.3  2005/02/01 22:36:44  alamora
# Corrected Variable Type name display
# Cleaned up closing code
#
# Revision 1.2  2005/01/25 07:14:25  alamora
# Register contents and Variable types are correctly displayed
#
# Revision 1.1  2005/01/25 00:52:29  alamora
# First addition of IRE to the (practical) world
#
