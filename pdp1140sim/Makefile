
PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Detect the operating system
ifeq ($(shell uname -o), Cygwin)
    OUTPUT_DIR = bin-cygwin
    DLL_DIR = /cygdrive/c/cygwin64/bin
    DLLS = $(DLL_DIR)/cyggcc_s-seh-1.dll  $(DLL_DIR)/cygstdc++-6.dll $(DLL_DIR)/cygwin1.dll
    # Create output directory if it doesn't exist
    $(shell mkdir -p $(OUTPUT_DIR))
    # deploy DLLs
    $(shell cp -f $(DLLS) $(OUTPUT_DIR))
else
    OUTPUT_DIR = bin-linux
    # Create output directory if it doesn't exist
    $(shell mkdir -p $(OUTPUT_DIR))
endif


OBJS = $(OUTPUT_DIR)/application.o	\
	$(OUTPUT_DIR)/messages.o \
	$(OUTPUT_DIR)/tcpmessageinterface.o	\
	$(OUTPUT_DIR)/pdp11simulator.o \
	$(OUTPUT_DIR)/pdp11simulator40.o

	CFLAGS += -Wall -Wextra -g -fmax-errors=5 -I../common
#	CFLAGS += -Wall -Wextra -O2

all:	$(OUTPUT_DIR)/pdp1140sim

$(OUTPUT_DIR)/pdp1140sim:	$(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^
	$(EXTRA_CMDS)

$(OUTPUT_DIR)/messages.o:	$(PROJECT_ROOT)../common/messages.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

$(OUTPUT_DIR)/tcpmessageinterface.o:	$(PROJECT_ROOT)../common/tcpmessageinterface.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

$(OUTPUT_DIR)/pdp11simulator.o:	$(PROJECT_ROOT)../common/pdp11simulator.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

$(OUTPUT_DIR)/%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

$(OUTPUT_DIR)/%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(OUTPUT_DIR)/pdp1140sim $(OBJS) $(EXTRA_CLEAN)
