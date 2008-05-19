# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):  Chris Abidin, Andrew LaMora
#  Created on: 2004-04-30
#  $Id: ireInputBox.py,v 1.4 2005/12/20 21:00:21 pkaz Exp $
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
#ireInputBox.py
"""
The IRE Simple Input Box

Creates a wx dialog box that prompts the user for input:

Input: Two dictionary objects, of labels and data, keyed identically.
    ireInputBox(None, -1, Demo Input, dictLabels, dictData
    ... with Demo Input in quotes of course

Note: Currently, this dialog box displays the dialog size and position and
      updates them as you move or resize the dialog box.
"""

#import wxPython GUI package
import wx

#create a class derived from wxFrame
class ireInputBox( wx.Frame ):

  def __init__ (self, parent, id, title):
      #call the base class init method to create the frame
    wx.Frame.__init__(self, parent, id, title)
    
    # Associate some events with methods of this class
    self.Bind(wx.EVT_SIZE, self.OnSize)
    self.Bind(wx.EVT_MOVE, self.OnMove)
    #Add the panel and some controls
    panel = wx.Panel(self, -1)
    label1 = wx.StaticText(panel, -1, "Size:")
    label2 = wx.StaticText(panel, -1, "Pos:")
    self.sizeCtrl = wx.TextCtrl(panel, -1, "", style = wx.TE_READONLY)
    self.posCtrl = wx.TextCtrl(panel, -1, "", style = wx.TE_READONLY)
    self.panel = panel
    
    # Use some sizers for layout of the widgets
    sizer = wx.FlexGridSizer(2, 2, 5, 5)
    sizer.Add(label1)
    sizer.Add(self.sizeCtrl)
    sizer.Add(label2)
    sizer.Add(self.posCtrl)
    
    border = wx.BoxSizer()
    border.Add(sizer, 0, wx.ALL, 15)
    panel.SetSizerAndFit(border)
    self.Fit()
        
    
  # This method is called by the System when the window is resized,
  # because of the association above.
  def OnSize(self, event):
    size = event.GetSize()
    self.sizeCtrl.SetValue("%s, %s" % (size.width, size.height))
    # tell the event system to continue looking for an event handler,
    # so the default handler will get called.
    event.Skip()

  # This method is called by the System when the window is moved,
  # because of the association above.
  def OnMove(self, event):
    pos = event.GetPosition()
    self.posCtrl.SetValue("%s, %s" % (pos.x, pos.y))



##############################################################################
# $Log: ireInputBox.py,v $
# Revision 1.4  2005/12/20 21:00:21  pkaz
# ireInputBox.py:  Minor cleanup.  This appears to have been a work in process.
#
# Revision 1.3  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.2  2005/09/06 22:43:33  alamora
# irepy module: updated for cvs logging info, cisst license
#
