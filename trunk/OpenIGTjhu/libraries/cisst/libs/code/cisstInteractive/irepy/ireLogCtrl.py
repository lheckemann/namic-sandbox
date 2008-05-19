# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):  Peter Kazanzides
#  Created on: 2004-04-30
#  $Id: ireLogCtrl.py,v 1.9 2006/01/17 22:41:56 pkaz Exp $
#
#  The ireLogCtrl class defines the panel used for the logger.
#  Currently, it contains a header and a text control (wx.TextCtrl)
#  for the log output.  The header includes a checkbox to enable/disable
#  the logger and an IntCtrl box to allow the user to change the
#  channel level of detail (LoD).
#
#  Currently, if the IRE is not embedded (i.e., not called from a
#  C++ program), the logger functionality is disabled because the
#  ireLogger module is not available.
#
#  Future enhancements could include:
#    - logging support for cases where the IRE is not embedded (need
#      to create an importable Python module for ireLogger).
#    - the ability to change the level of detail on a class-by-class
#      basis (via the Class Registry).
#    - the ability to change the global level of detail.
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

""" ireLogCtrl

"""
import wx
import wx.lib.intctrl as intctrl

# See if the IRE is embedded in a C++ application.
# This can be determined by checking whether the ireLogger
# module can be imported, because ireLogger is created
# by the cisstInteractive (C++) library.
ireEmbedded = True
try:
    import ireLogger
    from ireLogCtrl import *
except ImportError,e:
    ireEmbedded = False


class ireLogCtrl(wx.Panel):
  
  def __init__(self, parent, id, title='',
                 style=wx.TE_READONLY | wx.HSCROLL | wx.TE_MULTILINE | wx.TE_RICH):
    wx.Panel.__init__(self, parent, id)

        self.SetBackgroundColour(wx.BLUE)

        Font = wx.Font(10, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self.Text = wx.StaticText(self, -1, title, style=wx.ALIGN_LEFT)
        self.Text.SetForegroundColour(wx.WHITE)
        self.Text.SetFont(Font)
        self.EnableBox = wx.CheckBox(self, -1, 'Enabled')
        self.EnableBox.SetForegroundColour(wx.WHITE)
        self.EnableBox.SetFont(Font)
        self.ChannelLoDText = wx.StaticText(self, -1, 'Channel LoD: ')
        self.ChannelLoDText.SetForegroundColour(wx.WHITE)
        self.ChannelLoDText.SetFont(Font)

        LoD = 5
        if ireEmbedded:
            LoD = ireLogger.GetLoD()

        # use the IntCtrl control to get the LoD.  Another option would be to use
        # wx.SpinCtrl, which would give nice up/down arrows, but does not prevent
        # the user from entering non-numeric data using the keyboard.
        # For now, we arbitrarily set the maximum LoD to 99.
        self.ChannelLoD = intctrl.IntCtrl(self, -1, LoD, min=0, max=99, limited=True)

        if not ireEmbedded:
            self.EnableBox.Enable(False)
            self.ChannelLoDText.Enable(False)
            self.ChannelLoD.Enable(False)

        self.HeaderSizer = wx.BoxSizer(wx.HORIZONTAL)
        self.HeaderSizer.Add(self.Text, 0, wx.ALIGN_LEFT)
        self.HeaderSizer.Add((0,0),3)   # Add a stretchable space
        self.HeaderSizer.Add(self.EnableBox, 0, wx.ALIGN_LEFT)
        self.HeaderSizer.Add((0,0),3)   # Add a stretchable space
        self.HeaderSizer.Add(self.ChannelLoDText, 0, wx.ALIGN_RIGHT)
        self.HeaderSizer.Add(self.ChannelLoD, 0, wx.ALIGN_LEFT)
        self.HeaderSizer.Add((0,0),1)   # Add a smaller stretchable space

        self.logText = wx.TextCtrl(self, -1, style=style)

        self.LogSizer = wx.BoxSizer(wx.VERTICAL)
        self.LogSizer.Add(self.HeaderSizer, 0, wx.EXPAND)
        self.LogSizer.Add(self.logText, 1, wx.EXPAND)
        self.SetSizer(self.LogSizer)

        self.Bind(wx.EVT_CHECKBOX, self.OnEnableBox, self.EnableBox)
        self.Bind(intctrl.EVT_INT, self.OnChannelLoD, self.ChannelLoD)

    def EnableLogger(self):
        global ireEmbedded
        if ireEmbedded:
            # Make the TextCtrl into a logger target (wx.Log) and pass
            # the LogMessage callback to the C++ software.
            self.logTarget = wx.LogTextCtrl(self.logText)
            self.logTarget.SetTimestamp(None)
            wx.Log.SetActiveTarget(self.logTarget)
            # Callback to a Python function (via interpreter)
            callback = wx.LogMessage
            # Try to be clever and pass the (wrapped) C function as the
            # callback.  If a C function is passed, the ireLogger module
            # can call it directly.  If we can't find the C function,
            # we'll use the Python callback function (wx.LogMessage).
            try:
                callback = wx._misc_.LogMessage
            except AttributeError,e:
                print "EnableLogger warning: could not find C function:"
                print e
            ireLogger.SetTextOutput(callback, True)
            # Alternate implementation (Python callback only; how to get
            # the C function callback for a class method?)
            #ireLogger.SetTextOutput(self.logText.AppendText, False)
            self.EnableBox.SetValue(True)


    def DisableLogger(self):
        global ireEmbedded
        if ireEmbedded:
            ireLogger.ClearTextOutput()
            self.EnableBox.SetValue(False)

    # Event handlers

    def OnEnableBox(self, event):
        if event.IsChecked():
            self.EnableLogger()
        else:
            self.DisableLogger()

    def OnChannelLoD(self, event):
        if ireEmbedded:
            ireLogger.SetLoD(event.GetValue())

