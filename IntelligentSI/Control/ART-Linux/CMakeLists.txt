PROJECT(slave)

SET(P_ARTLINUX 1)
SET(P_VXWORKS  0)

MACRO (USE_VXWORKS_TOOLS)
# SHELL   = /bin/sh
#TOOL   = gnu
#BOARD    = mcp820
#CFLAGS   = $(CC_OPTIM) $(CC_WARNINGS) $(CC_INCLUDE) $(CC_COMPILER) $(CC_DEFINES)
#CC_WARNINGS  = -Wall
#CC_INCLUDE = -I$(VX_INCLUDE) -I$(MY_INCLUDE) -I$(MY_INCLUDE2) -I./
#CC_DEFINES = -DCPU=$(CPU) -DCPU_FAMILY=$(CPU_FAMILY) $(CC_ETC_DEFINES)
#CC_WARNINGS_ALL = -Wall
#CC_OPTIM   = -O2 -fvolatile -fno-builtin -mlongcall
#AS   = $(GNU_BIN_DIR)/as$(CPU_FAMILY)
#LD   = $(GNU_BIN_DIR)/ld$(CPU_FAMILY)
#LD_PARTIAL_FLAGS= -X -r
#NM   = $(GNU_BIN_DIR)/nm$(CPU_FAMILY)
#MAKE            = $(GNU_BIN_DIR)/make

   SET(VX_CPU "PPC6-4")
   SET(VX_CPU_FAMILY "ppc")
   SET(VX_WIND_HOST_TYPE "sun4-solaris2")
   SET(VX_WIND_BASE "/usr/local/tornado.ppc")
   SET(VX_GNU_BIN_DIR "/usr/local/tornado.ppc/host/sun4-solaris2/bin")
   SET(VX_GNU_LIB_DIR "/usr/local/tornado.ppc/host/sun4-solaris2/lib")
   SET(VX_VX_INCLUDE "/usr/local/tornado.ppc/target/config/mcp820")
   SET(VX_MY_INCLUDE "/usr/local/tornado.ppc/target/config/all")
   SET(VX_HOME "/home/koduka")
   SET(VX_MY_INCLUDE2 ${HOME}/vxworks/CPCI/lib/include)
   SET(VX_LIB_DIR ${HOME}/vxworks/CPCI/lib/lib.ppc)
   SET(VX_LIBS ${LIB_DIR}/acp550.o ${LIB_DIR}/acp560.o ${LIB_DIR}/acp420.o ${LIB_DIR}/sockUDP.o)
   SET(CMAKE_CXX_COMPILER "${GNU_BIN_DIR}/c++${CPU_FAMILY} -B${GNU_LIB_DIR}")
   SET(CMAKE_COMPILER_IS_GNUCXX 1)
   SET(CMAKE_C_COMPILER "${GNU_BIN_DIR}/cc${CPU_FAMILY} -B${GNU_LIB_DIR}")
   SET(CMAKE_AR ${GNU_BIN_DIR}/ar${CPU_FAMILY})
   SET(CMAKE_RANLIB ${GNU_BIN_DIR}/ranlib${CPU_FAMILY})
#for compiling
#   ADD_DEFINITIONS()
ENDMACRO (USE_VXWORKS_TOOLS)

SET(Required_Packages
#list packages here
)

FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

IF (P_VXWORKS)
   #USE_VXWORKS_TOOLS()
    SET(SRCS joint.cpp main.cpp driver.cpp robot.cpp save.cpp IFmanager.cpp keyConsole.cpp PNTM_interface.cpp)
ENDIF (P_VXWORKS)


IF (P_ARTLINUX)
    #SET(SRCS joint.cpp main.cpp driver.cpp robot.cpp save.cpp
    #interface.cpp IFmanager.cpp sockUDP.cpp hrpifb.cpp
    #"/usr/local/src/hrpifb/lib/I7sIo.o" "/usr/lib/art_syscalls.o")
    SET(SRCS joint.cpp main.cpp driver.cpp robot.cpp save.cpp 
        IFmanager.cpp keyConsole.cpp PNTM_interface.cpp sockUDP.cpp 
  hrpifb.cpp frame.cpp SlicerInterface.cpp 
        "/usr/local/src/hrpifb/lib/I7sIo.o" "/usr/lib/art_syscalls.o")

    INCLUDE_DIRECTORIES("/usr/local/src/hrpifb/lib/")
    INCLUDE_DIRECTORIES("/usr/local/src/hrpifb/I7sDriver/")
    SET(Libraries
       pthread
       rt
    )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
ENDIF (P_ARTLINUX)


#INCLUDE_DIRECTORIES()
#LINK_DIRECTORIES()
#ADD_DEFINITIONS("...")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ...")





SET(CurrentExe "main")
ADD_EXECUTABLE(${CurrentExe} ${SRCS}) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

