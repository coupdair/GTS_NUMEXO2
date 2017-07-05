TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord

# Install xxxSupport.dbd into <top>/dbd
#DBD += xxxSupport.dbd

# Build an IOC support library
#LIBRARY_IOC += gtsSupport

# Compile and add the code to the support library
#gtsSupport_SRCS += xxxRecord.c
#gtsSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
# This is required for Windows DLL builds.
gtsSupport_SRCS += dbSubExample.c
#gtsSupport_SRCS += gtsHello.c
#gtsSupport_SRCS += initTrace.c

gtsSupport_LIBS += $(EPICS_BASE_IOC_LIBS)


# Build the IOC application
PROD_IOC = gts

# gts.dbd will be created and installed
DBD += gts.dbd

# gts.dbd will include these files:
#gts_DBD += base.dbd
#gts_DBD += xxxSupport.dbd
gts_DBD += dbSubExample.dbd
#gts_DBD += gtsHello.dbd
#gts_DBD += initTrace.dbd

# gts_registerRecordDeviceDriver.cpp derives from gts.dbd
gts_SRCS += gts_registerRecordDeviceDriver.cpp

# Build the main IOC entry point where needed
gts_SRCS_DEFAULT += gtsMain.cpp
gts_SRCS_vxWorks += -nil-

# Link in the code from our support library
#gts_LIBS += gtsSupport

# To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file
ifneq ($(SNCSEQ),)
    # Build sncExample into gtsSupport
    sncExample_SNCFLAGS += +r
    gts_DBD += sncExample.dbd
    # A .stt sequence program is *not* pre-processed:
    gtsSupport_SRCS += sncExample.stt
    gtsSupport_LIBS += seq pv
    gts_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    # A .st sequence program *is* pre-processed:
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

# Finally link IOC to the EPICS Base libraries
gts_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

#WiP: in linking, it needs to:
#add /home/coudert/GTS_server/GTS_NUMEXO2/Linux-ppc440/gtsMisc.o

