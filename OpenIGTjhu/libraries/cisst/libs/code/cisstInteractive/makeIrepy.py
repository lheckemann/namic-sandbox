#!/usr/bin/python

# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

#
# $Id: makeIrepy.py,v 1.6 2005/12/21 09:24:35 pkaz Exp $
#
# Python script file to compile Python source files (*.py) and create
# Python binary files (*.pyc).  This script file requires two
# command line arguments:
# 
#    src-dir    Directory for the input source files (*.py)
#    dest-dir   Directory for the output binary files (*.py)
#
# If an error occurs (e.g., could not delete file, create directory, etc.)
# the script exits with a non-zero return code.
#
# --- begin cisst license - do not edit ---
# 
# CISST Software License Agreement(c)
# 
# Copyright 2005 Johns Hopkins University (JHU) All Rights Reserved.
# 
# This software ("CISST Software") is provided by The Johns Hopkins
# University on behalf of the copyright holders and
# contributors. Permission is hereby granted, without payment, to copy,
# modify, display and distribute this software and its documentation, if
# any, for research purposes only, provided that (1) the above copyright
# notice and the following four paragraphs appear on all copies of this
# software, and (2) that source code to any modifications to this
# software be made publicly available under terms no more restrictive
# than those in this License Agreement. Use of this software constitutes
# acceptance of these terms and conditions.
# 
# The CISST Software has not been reviewed or approved by the Food and
# Drug Administration, and is for non-clinical, IRB-approved Research
# Use Only.
# 
# IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE TO
# ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
# EVEN IF THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE BEEN ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
# 
# THE COPYRIGHT HOLDERS AND CONTRIBUTORS SPECIFICALLY DISCLAIM ANY
# EXPRESS OR IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, AND NON-INFRINGEMENT.
# 
# THE SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS." THE COPYRIGHT HOLDERS AND CONTRIBUTORS HAVE NO OBLIGATION TO
# PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
# 
# --- end cisst license ---
#

"""makeIrepy src-dir  dest-dir

This module compiles the source files of the IRE IDE into the
appropriate destination (release) directory.
"""
import os, sys, glob, py_compile
#import shutil

def main():
    if len(sys.argv)<3:
        print "Syntax:  python %s src-dir dest-dir" % sys.argv[0]
        sys.exit(1)

    src = sys.argv[1]
    dest = sys.argv[2]

    # remove all .pyc files in destination directory
    # (this won't fail even if the directory does not exist)
    try:
       map(os.remove, glob.glob(os.path.join(dest,'*.pyc')))
    except OSError, e:
       print 'makeIrepy: could not delete ' + e.filename + ', ' + e.strerror
       sys.exit(1)

    # create the directory (if it does not already exist)
    if not os.path.isdir(dest):
        try:
           os.makedirs(dest)
        except OSError, e:
           print 'makeIrepy: could not create destination directory: ' + dest
           print e
           sys.exit(1)

    # compile the source files
    for f in glob.glob1(src,'*.py'):
        py_compile.compile(os.path.join(src,f), os.path.join(dest,f+'c'))
    
    #Now copy the images over.
    #This can be directly handled by CMake
    #shutil.copytree(os.path.join(src,'images', os.path.join(dest, 'images')))
    
if __name__ == "__main__":
    main()
    
    
#
# $Log: makeIrepy.py,v $
# Revision 1.6  2005/12/21 09:24:35  pkaz
# makeIrepy.py:  added #!/usr/bin/python for Unix systems.
#
# Revision 1.5  2005/12/20 20:05:40  pkaz
# makeIrepy.py:  improved implementation.   Only deletes *.pyc files in destination directory, rather than removing the entire directory.
#
# Revision 1.4  2005/09/26 16:36:05  anton
# cisst: Added modelines for emacs and vi (python), see #164.
#
# Revision 1.3  2005/09/06 07:19:50  alamora
# makeIrepy.py: bug fix, clobbers targ dir
#
# Revision 1.2  2005/09/06 07:05:22  alamora
# makeIrepy.py: copies images directory.  Now specific to irepy.
#
# Revision 1.1  2005/09/06 06:32:56  alamora
# makeIrepy.py: A python file to compile a generic imput directory (in this case,
# irepy, the IRE IDE) into a binary directory.  Both input and output dirs are
# specified at the command line.
#    
    
