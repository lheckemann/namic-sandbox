# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
#  Author(s):  Andrew LaMora
#  Created on: 2004-04-30
#  $Id: __init__.py,v 1.4 2005/12/21 08:21:10 pkaz Exp $
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

#------------------------------------------
# Import the IRE libraries
#------------------------------------------
import ireMain

import threading

class IreThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        ireMain.launchIrePython()


#------------------------------------------
# function:  launch
#
# command-line command to launch the ire
#------------------------------------------
def launch():
  ireMain.launchIrePython()


#------------------------------------------
# Global statements
#------------------------------------------

#from pkgutil import extend_path
#__path__ = extend_path(__path__, __name__)

print "Welcome to the IRE!"
print "Use irepy.launch() to launch the IRE GUI"
#ireMain.launchIrePython()


##############################################################################
# $Log: __init__.py,v $
# Revision 1.4  2005/12/21 08:21:10  pkaz
# IRE: added support for running the IRE in a separate thread (uses Python threading module).
#
# Revision 1.3  2005/12/20 21:01:23  pkaz
# __init__.py:  removed some dead code.
#
# Revision 1.2  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.1  2005/09/06 22:43:08  alamora
# irepy module: init revision 0.1.0 checkin
#
# Revision 1.2  2005/02/01 22:15:00  alamora
# IRE now launches explicitly from irepy.launch()
# ireSimpleDialogs no longer loaded into def ns by default
#
# Revision 1.1  2005/01/25 00:52:29  alamora
# First addition of IRE to the (practical) world
#
