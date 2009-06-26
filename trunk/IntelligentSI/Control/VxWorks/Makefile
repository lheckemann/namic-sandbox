SHELL       = /bin/sh
TOOL        = gnu
TARGET_DIR  =
VENDOR      = Motorola
BOARD       = mcp820
CPU             = PPC604
CPU_FAMILY      = ppc
WIND_BASE       = /usr/local/tornado.ppc
WIND_HOST_TYPE  = sun4-solaris2

#GNU_BIN_DIR    = $(WIND_BASE)/host/$(WIND_HOST_TYPE)/bin
#GNU_LIB_DIR    = $(WIND_BASE)/host/$(WIND_HOST_TYPE)/lib/gcc-lib
#VX_INCLUDE = $(WIND_BASE)/target/config/mcp820
#MY_INCLUDE = $(WIND_BASE)/target/config/all
GNU_BIN_DIR = /usr/local/tornado.ppc/host/sun4-solaris2/bin
GNU_LIB_DIR = /usr/local/tornado.ppc/host/sun4-solaris2/lib
VX_INCLUDE  = /usr/local/tornado.ppc/target/config/mcp820
MY_INCLUDE  = /usr/local/tornado.ppc/target/config/all

HOME        = /home/koduka
MY_INCLUDE2 = $(HOME)/vxworks/CPCI/lib/include
LIB_DIR     = $(HOME)/vxworks/CPCI/lib/lib.ppc
LIBS        = $(LIB_DIR)/OpenIGTLink.o $(LIB_DIR)/UDPSock.o $(LIB_DIR)/TCPSock.o \
                  $(LIB_DIR)/igtl_header.o $(LIB_DIR)/igtl_util.o

CFLAGS      = $(CC_OPTIM) $(CC_WARNINGS) $(CC_INCLUDE) $(CC_COMPILER) \
          $(CC_DEFINES)

CC_WARNINGS = -Wall
CC_INCLUDE  = -I$(VX_INCLUDE) -I$(MY_INCLUDE) -I$(MY_INCLUDE2) -I./
CC_DEFINES  = -DCPU=$(CPU) -DCPU_FAMILY=$(CPU_FAMILY) $(CC_ETC_DEFINES)
CC_ETC_DEFINES  =
CC_WARNINGS_ALL = -Wall
CC_OPTIM    = -O2 -fvolatile -fno-builtin -mlongcall

AR      = $(GNU_BIN_DIR)/ar$(CPU_FAMILY)
AS      = $(GNU_BIN_DIR)/as$(CPU_FAMILY)
CC      = $(GNU_BIN_DIR)/cc$(CPU_FAMILY) -B$(GNU_LIB_DIR)/
CXX     = $(GNU_BIN_DIR)/c++$(CPU_FAMILY) -B$(GNU_LIB_DIR)/
LD      = $(GNU_BIN_DIR)/ld$(CPU_FAMILY)
LD_PARTIAL_FLAGS= -X -r
NM      = $(GNU_BIN_DIR)/nm$(CPU_FAMILY)
RANLIB      = $(GNU_BIN_DIR)/ranlib$(CPU_FAMILY)
MAKE            = $(GNU_BIN_DIR)/make

.SUFFIXES:  .cpp

# end of environment

SRCS    = main.cpp Device.cpp BoardBase.cpp occo.cpp\
      Driver.cpp InterfaceManager.cpp InterfaceBase.cpp\
      Joint.cpp Robot.cpp InterfaceSlicer.cpp InterfaceMasterR.cpp\
      ConsoleW.cpp FrameInverse.cpp ACP420.cpp ACP560.cpp FrameBase.cpp\
      save24.cpp fReader.cpp FrameForward.cpp

OBJS    = $(SRCS:.cpp=.o)

DEPEND  = makedepend

all: main

.cpp.o :
    @ $(RM) $@
    $(CC) $(CFLAGS) -c $<

depend:
    $(DEPEND) -- $(CFLAGS) -- $(SRCS)

main:   $(OBJS)
    $(LD) $(LD_PARTIAL_FLAGS) -o tmp.o $(OBJS) $(LIBS)
    $(NM) tmp.o | munch > ctdt.c
    $(MAKE) CC_COMPILER="-traditional" ctdt.o
    $(LD) $(LD_PARTIAL_FLAGS) -o $@ tmp.o ctdt.o
    $(RM) ctdt.c ctdt.o tmp.o

clean:
    rm -f $(OBJS)
    rm -f main

# DO NOT DELETE
