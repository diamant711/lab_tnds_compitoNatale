ROOT_LIBS:=$(shell root-config --libs)  #LIBS memorizes the path to ROOT's
                                        #libraries


ROOT_INCS:=$(shell root-config --cflags) #INCS memorizes the path to ROOT's
																				 # header files

PRJ_DIR := $(PWD)

X_FILES := labtnds-analysis labtnds-net-parser

LIB := UDPframebroadcast

OBJECTS_DIR := $(PRJ_DIR)/obj
OBJECTS_analysis := main_tnds-analysis.o
OBJECTS_net_parser := main_tnds-net-parser.o
OBJECTS_lib := UDPframebroadcast.o
OBJECTS := $(OBJECTS_analysis) $(OBJECTS_net_parser) $(OBJECTS_lib)

DEBUG := -g -O0 -DDEBUG

PROFILING := -pg -O0

all : $(LIB) $(X_FILES) ;

labtnds-analysis : WORKDIR = $(PRJ_DIR)/src/tnds-analysis
labtnds-analysis : $(OBJECTS_DIR)/$(OBJECTS_analysis) ; \
	$(LINK.cpp) -o $@ $(ROOT_LIBS) -lASImage -L$(OBJECTS_DIR) $^

labtnds-net-parser : WORKDIR = $(PRJ_DIR)/src/tnds-net-parser
labtnds-net-parser : $(OBJECTS_DIR)/$(OBJECTS_net_parser) $(OBJECTS_DIR)/UDPframebroadcast.o ; \
	$(LINK.cpp) -o $@ -L$(OBJECTS_DIR) $^

UDPframebroadcast : WORKDIR = $(PRJ_DIR)/lib/UDPframebroadcast
UDPframebroadcast : $(OBJECTS_DIR)/$(OBJECTS_lib) ;

$(OBJECTS_DIR)/%.o : ; \
	$(COMPILE.cpp) -o $(OBJECTS_DIR)/$*.o $(ROOT_INCS) -I$(PRJ_DIR)/lib $(WORKDIR)/$*.cpp

debug : CXXFLAGS = $(CXXFLAGS) $(DEBUG)
debug : all ;

profiles : CXXFLAGS = $(CXXFLAGS) $(PROFILING)
profiles : all ;

clean : ; \
	rm -f $(OBJECTS_DIR)/* 
	rm -f gmon.out $(X_FILES)
