######################################################################
# To compile, type:
#         make -f Makefile
# Note: This makefile is to build library and test applications
#
# This makefile assumes that your build environment is already correctly
# configured. (For example, the correct version of your compiler and
# linker should be on your PATH.)
# Revision       Date                 Author
#  ------------------------------------------------------------------
#  			                         Vijay Gayee 
# SYNOPSYS:
#   ---------
######################################################################

include $(TNG_TOOLS)/make_init.txt

# If undefined in the environment default NDDSHOME to install dir
ifndef NDDSHOME
NDDSHOME := "/usr/local/rti_dds"
endif

AR=ar
ARFLAGS:= rv
ABOUT=
SOURCE_DIR = 
CC=gcc

TARGET_ARCH = x64Linux2.6gcc4.4.5

ifndef COMPILER
COMPILER = gcc
endif
COMPILER_FLAGS = -m64
ifndef LINKER
LINKER = gcc
endif
LINKER_FLAGS = -m64
SYSLIBS = -ldl -lnsl -lm -lpthread -lrt
DEFINES = -DRTI_UNIX -DRTI_LINUX -DRTI_64BIT 
ifndef DEBUG
DEBUG=0 
endif
ifeq ($(DEBUG),1)
COMPILER_FLAGS += -g -O0
LINKER_FLAGS += -g
DEBUG_SFX = d
else
DEBUG_SFX = 
endif
 
ifndef SHAREDLIB
SHAREDLIB=0
endif

ifeq ($(SHAREDLIB),1)
SHAREDLIB_SFX = 
else
SHAREDLIB_SFX = z
DEFINES += -DRTI_STATIC
endif

#DIRECTORIES = objs.dir libs.dir tests.dir
OBJ_EXT = .o
EXE_EXT =
OBJ_DIR = objs
LIB_DIR = libs
TEST_DIR = tests

#====================================================================
#  Run this only when you change IDL file , else not required
#  $(NDDSHOME)/bin/rtiddsgen IPRC_COMM_LINK.idl -example $(TARGET_ARCH) -replace -language C
#  $(NDDSHOME)/bin/rtiddsgen IPRC_COMM_LINK.idl -example x64Linux2.6gcc4.4.5 -replace -language C
#=======================================================================  
INCLUDES = -I. -I$(NDDSHOME)/include -I$(NDDSHOME)/include/ndds  -I$(TNG_SOURCE)/utils/inc 
       
LIBS = -L$(NDDSHOME)/lib/$(TARGET_ARCH) \
        -lnddsc$(SHAREDLIB_SFX)$(DEBUG_SFX) -lnddscore$(SHAREDLIB_SFX)$(DEBUG_SFX) $(SYSLIBS) \
	$(TNG_LIB)/libutils$(LIB_EXT)
       
CFLAGS += $(COMPILER_FLAGS) $(DEFINES) $(INCLUDES) 



IDLRSOURCES    = IPRC_COMM_LINK.idl
SOURCES = IPRC_COMM_LINKPlugin.c \
		IPRC_COMM_LINK.c \
		IPRC_COMM_LINKSupport.c \
                iprclink_api.c \

OBJS = $(SOURCES:.c=$(OBJ_EXT))
#%.o: %.c
IPRCLIB  = $(TNG_LIB)/libiprc.a
DIPRCLIB  = libiprc.so
TESTS = IPRC_API_PUB_TEST \
	IPRC_API_SUB_TEST

USRQOS = $(TNG_USER)/config/USER_QOS_PROFILES.xml

#DIRECTORIES = objs.dir libs.dir tests.dir
#----------------------------------------------------------------------------
# Std targets
#----------------------------------------------------------------------------
.PHONY:: dirs

default:: all

all:: gendds dirs lib $(USRQOS)

tests:: $(TESTS)

lib:: $(IPRCLIB) $(DIPRCLIB)

dirs:: $(DIRECTORIES)

gendds:: do_script

clean::
	rm -f $(OBJS) $(IPRCLIB) $(TESTS)

#-------------------------------------
# make rules
#--------------------------------------

do_script : 
	./ddsgen.sh

$(USRQOS): $(TNG_SOURCE)/dds/config/USER_QOS_PROFILES.xml
	tr -d '\r' < $(TNG_SOURCE)/dds/config/USER_QOS_PROFILES.xml > $@

$(dirs) : $(DIRECTORIES) 

$(IPRCLIB): $(OBJS) 
	$(AR) $(ARFLAGS) $(AROUT)$@ $(OBJS)

$(DIPRCLIB): $(OBJS) 
	$(CC) -shared -o $(AROUT)$@ $(OBJS)


IPRC_API_PUB_TEST: iprclink_api_wr_test.c $(IPRCLIB)
	$(CC) $(CFLAGS) -g -c iprclink_api_wr_test.c
	$(CC) -o $@ $(LDFLAGS) iprclink_api_wr_test.o $(IPRCLIB)  $(LIBS) 
	rm -f iprclink_api_wr_test.o        

IPRC_API_SUB_TEST: iprclink_api_rd_test.c $(IPRCLIB)
	$(CC) $(CFLAGS) -g -c iprclink_api_rd_test.c
	$(CC) -o $@ $(LDFLAGS) iprclink_api_rd_test.o  $(IPRCLIB) $(LIBS) 
	rm -f iprclink_api_rd_test.o        


# create those subdirectories if needed.
%.dir : 
	@echo "Checking directory $*"
	@if [ ! -d $* ]; then \
		echo "Making directory $*"; \
		mkdir -p $* ; \
	fi;
#################
-include $(TNG_TOOLS)/make_def.txt
