ROOT_LIBS:=$(shell root-config --libs)  #LIBS memorizes the path to ROOT's
                                        #libraries


ROOT_INCS:=$(shell root-config --cflags) #INCS memorizes the path to ROOT's
																				 # header files

PRJ_DIR := $(PWD)

X_FILES := labtnds-analysis labtnds-net-parser

LIB := UDPframebroadcast

OBJECTS_DIR := $(PRJ_DIR)/obj
OBJECTS_analysis := main_tnds-analysis.o
SOURCES_analysis := $(wildcard $(PRJ_DIR)/src/tnds-analysis/*.*pp)
OBJECTS_net_parser := main_tnds-net-parser.o
SOURCES_net_parser := $(wildcard $(PRJ_DIR)/src/tnds-net-parser/*.*pp)
OBJECTS_lib := UDPframebroadcast.o
OBJECTS := $(OBJECTS_analysis) $(OBJECTS_net_parser) $(OBJECTS_lib)

DEBUG := -g -O0 -DDEBUG

PROFILING := -pg -O0

all : $(LIB) $(X_FILES) ;

labtnds-analysis : WORKDIR = $(PRJ_DIR)/src/tnds-analysis
labtnds-analysis : $(SOURCES_analysis) ; \
	$(LINK.cpp) -Wall -o $@ $(ROOT_LIBS) -lASImage -L$(OBJECTS_DIR) $^ -I$(PRJ_DIR)/lib

labtnds-net-parser : WORKDIR = $(PRJ_DIR)/src/tnds-net-parser
labtnds-net-parser : $(SOURCES_net_parser) $(OBJECTS_DIR)/UDPframebroadcast.o ; \
	$(LINK.cpp) -Wall -o $@ -L$(OBJECTS_DIR) $^ -I$(PRJ_DIR)/lib

UDPframebroadcast : WORKDIR = $(PRJ_DIR)/lib/UDPframebroadcast
UDPframebroadcast : $(OBJECTS_DIR)/$(OBJECTS_lib) ;

$(OBJECTS_DIR)/%.o : ; \
	$(COMPILE.cpp) -Wall -o $(OBJECTS_DIR)/$*.o $(ROOT_INCS) -I$(PRJ_DIR)/lib $(WORKDIR)/$*.cpp

debug : CXXFLAGS := $(CXXFLAGS) $(DEBUG)
debug : all ;

profiles : CXXFLAGS := $(CXXFLAGS) $(PROFILING)
profiles : all ;

clean : ; \
	rm -f $(OBJECTS_DIR)/* 
	rm -f gmon.out $(X_FILES)
