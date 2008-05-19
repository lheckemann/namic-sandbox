# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):  Chris Abidin, Andrew LaMora
#  Created on: 2004-04-30
#  $Id: ireWorkspace.py,v 1.3 2005/09/26 16:36:05 anton Exp $
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
# ireWorkspace.py
""" 
This module initializes the IRE workspace.

This module should not be directly loaded or called.
See ireMain.py
"""

import sys
import exceptions
import cPickle

class NoOutput:
  def write(self, String):
    pass

def IsPicklable(Object):
  try:
    cPickle.dump(Object, NoOutput())
  except exceptions.Exception, error: #cPickle.PicklingError, error:
    return False
  return True

#------------------------------------------------------
# SaveWorkspaceToFile
#
# Save workspace variables and statements to a file
#------------------------------------------------------
def SaveWorkspaceToFile(ObjectDictionary, File):
  Workspace = {}
  for ObjectName in ObjectDictionary:
    if IsPicklable(ObjectDictionary[ObjectName]):
      Workspace.update({ObjectName:ObjectDictionary[ObjectName]})
  cPickle.dump(Workspace,File)

#------------------------------------------------------
# LoadWorkspaceFile
#
# Load workspace variables and statements from a file
#------------------------------------------------------
def LoadWorkspaceFile(ObjectDictionary, File):
  Workspace = cPickle.load(File)
  for Variable in Workspace:
    ObjectDictionary[Variable] = Workspace[Variable]

#------------------------------------------------------
# main()
#
# Load the workspace, parse the serialized file for
# variables and load them into the variable list.

def main():
  v1 = 1234
  v2 = 'string'
  print 'v1 = ', v1, '\nv2 = ', v2
  SaveWorkspaceToFile(vars(), open('workspace','w'))
  v1 = v2 = None
  print 'v1 = ', v1, '\nv2 = ', v2
  LoadFile(open('workspace'))
  Workspace = cPickle.load(open('workspace'))
  for Variable in Workspace:
    exec(Variable + " = Workspace['" + Variable + "']")
  print 'v1 = ', v1, '\nv2 = ', v2

if __name__ == "__main__":
  sys.modules['ireWorkspace'] = sys.modules['__main__']
  main()
  
# $Log: ireWorkspace.py,v $
# Revision 1.3  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.2  2005/09/06 22:43:33  alamora
# irepy module: updated for cvs logging info, cisst license
#
# Revision 1.1  2005/09/06 22:30:03  alamora
# irepy module: revision 0.10 initial checkin
#
# Revision 1.1  2005/01/25 00:52:29  alamora
# First addition of IRE to the (practical) world
#
