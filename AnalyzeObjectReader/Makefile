#*************************************************************************/
Copyright (c) 2007, Regents of the University of Iowa

All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright 
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.
    * Neither the name of The University of Iowa nor the names of its 
      contributors may be used to endorse or promote products derived from 
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*************************************************************************/
#*************************************************************************/
#*   Programmer: Hans J. Johnson
#*       E-mail: hans-johnson@uiowa.edu
#* Organization: The University of Iowa
#*     Function:
#*
#*    $Author: jsong $
#*   $Source: /antioch/home/research/cvsroot/uiig/src/uiigutils/uiigobjectmap/Makefile,v $
#* $Revision: 1.1.1.1 $
#*   $Locker:  $
#*    $State: Exp $
#*     $Date: 2006/09/13 18:33:35 $
#*************************************************************************/
##The following items should be set as environmental varibles, or as command line options
## 
## csh% setenv ARCH sgi OR   bash% export ARCH=alpha etc 
## % make -e
## OR
## % make ARCH=sgi ABI=n32 RESEARCHHOME=~/src/ResearchHome
# ARCH= [SGI HP WIN alpha ix86]           #This is the destination architecture
# ABI= ?SGI [ 32 n32 64 MP64 compatible]  #This is the compilation choices
# ABI= ?alpha [ fast compatible ]   # see RESEARCHHOME/$(ARCH)/build for options
# RESEARCHHOME = /fovea1/people/research #The RESEARCHHOME directory specifies the path to the research home directory
## NOTE: any of the varibles listed should be set properly in the included make_file stubs

##The following include specifies many predefined options you may wish to use
##Please look at the file to be sure it is using the proper flags
include $(RESEARCHHOME)/$(ARCH)/build/make_$(ARCH)_$(ABI).h

##------------------Change PROJECT----------------
##The PROJECT varible is used as the name of the project you are working on
##   -------MAKE SURE THERE ARE NO SPACES AFTER THE PROJECT NAME!!-------------
PROJECT=uiigobjectmap
##----------------End Change---------------------

##------------------Change TARGETS----------------
##The TARGETS varible specifies the name of the target to create
TARGETS=$(ARCH)/$(ABI)/lib$(PROJECT).a
##----------------End Change---------------------

##------------------Change SRCS----------------
## insert the source files to compile here ( %ls *.cc )
SRCS.cpp=objectentry.cpp  objectmap.cpp  objectutils.cpp
SRCS.cc=
SRCS.c=

##------------------End Change----------------

##------------------Change COMPILATION NEEDS----------------   
##This is to setup library and header file needs 
## --see  $(RESEARCHHOME)/$(ARCH)/build/make_$(ARCH)_$(ABI).h for some predefined items 

##CPPFLAGS are things needed for the Preprosseor (i.e. location of include files)
CPPFLAGS=$(ARCH_INCDIR)  -I./ -I$(RESEARCHHOME)/$(ARCH)/$(ABI)/include -D$(ARCH)=$(ARCH) -D$(ABI)=$(ABI) 

##LIBDIRS The locations of needed libraries 
LIBDIRS=$(ARCH_LIBSDIR)   -L./ -L$(RESEARCHHOME)/$(ARCH)/$(ABI)/lib  

##LIBS The needed libraries 
LIBS=$(ARCH_LIBS) 

##LDFLAGS are the things needed by the linker
LDFLAGS=$(LIBDIRS) $(LIBS) $(PCA_FLAGS)

##ARFLAGS are flags needed by the archiver (probably OK by default)
#ARFLAGS=

##CFLAGS are the compiler options needed by the C compiler
#CFLAGS=  $(ABI_MODEL) -D$(ARCH)=$(ARCH) -D$(ABI)=$(ABI) $(C_OPTIMIZATION) $(C_CODE_COMPATIBILITY)
#CFLAGS=  $(ABI_MODEL) $(C_OPTIMIZATION) $(C_CODE_COMPATIBILITY)
##CXXFLAGS are the compiler options needed by the C++ compiler
#CXXFLAGS=$(ABI_MODEL) -D$(ARCH)=$(ARCH) -D$(ABI)=$(ABI) $(CXX_OPTIMIZATION) $(CXX_CODE_COMPATIBILITY) 
CXXFLAGS=$(ABI_MODEL) $(CXX_OPTIMIZATION) $(CXX_CODE_COMPATIBILITY) 
##------------------End Change----------------            


##The following lines define the make dependancies
default: $(TARGETS) Makefile
	echo $(TARGETS) made

##The following line includes all the dependancy files
DEPS=$(SRCS.cc:%.cc=$(ARCH)/$(ABI)/auto_deps/%.d) $(SRCS.cpp:%.cpp=$(ARCH)/$(ABI)/auto_deps/%.d) $(SRCS.c:%.c=$(ARCH)/$(ABI)/auto_deps/%.d)
include $(DEPS)

##The following line describes the object files needed 
OBJS=$(SRCS.cc:%.cc=$(ARCH)/$(ABI)/%.o) $(SRCS.cpp:%.cpp=$(ARCH)/$(ABI)/%.o) $(SRCS.c:%.c=$(ARCH)/$(ABI)/%.o)

$(TARGETS): $(OBJS)
	$(CXX_AR) $(CXX_ARFLAGS) $@ $(OBJS) $(AUTO_TEMPLATE_OBJECTS)

## The following line includes a makefile stub that includes standard definitions
## for clean, clobber, install
include $(RESEARCHHOME)/$(ARCH)/build/make_$(ARCH)_utilities_sep.h

##Add any suffix rules specific to this project here. Very seldom needs to be done!
#%.o: %.cc
#	$(CXX) $(CFLAGS) -c $< -o $@

