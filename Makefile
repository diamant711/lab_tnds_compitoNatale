ROOT_LIBS:=$(shell root-config --libs)  #LIBS memorizes the path to ROOT's
                                        #libraries


ROOT_INCS:=$(shell root-config --cflags) #INCS memorizes the path to ROOT's
																				 # header files

PRJ_DIR := $(PWD)

X_FILES := tnds_analysis tnds_net_parser

LIB := UDPframebroadcast

OBJECTS_DIR := $(PRJ_DIR)/obj
OBJECTS_analysis := main_tnds_analysis.o
OBJECTS_net_parser := main_tnds_net_parser.o
OBJECTS_lib := UDPframebroadcast.o
OBJECTS := $(OBJECTS_analysis) $(OBJECTS_net_parser) $(OBJECTS_lib)

DEBUG := -g -O0 -DDEBUG

PROFILING := -pg -O0

all : $(LIB) $(X_FILES) ;

tnds_analysis : WORKDIR = $(PRJ_DIR)/src/tnds_analysis
tnds_analysis : $(OBJECTS_DIR)/$(OBJECTS_analysis) $(OBJECTS_DIR)/UDPframebroadcast.o ; \
	$(LINK.o) -o $@ $^ $(ROOT_LIBS) -lASImage

tnds_net_parser : WORKDIR = $(PRJ_DIR)/src/tnds_net_parser
tnds_net_parser : $(OBJECTS_DIR)/$(OBJECTS_net_parser) ; \
	$(LINK.o) -o $@ $^

UDPframebroadcast : WORKDIR = $(PRJ_DIR)/lib/UDPframebroadcast
UDPframebroadcast : $(OBJECTS_DIR)/$(OBJECTS_lib) ;

$(OBJECTS_DIR)/%.o : ; \
	$(COMPILE.cpp) -o $(OBJECTS_DIR)/$*.o $(ROOT_INCS) $(WORKDIR)/$*.cpp

debug : CXXFLAGS = $(CXXFLAGS) $(DEBUG)
debug : all ;

profiles : CXXFLAGS = $(CXXFLAGS) $(PROFILING)
profiles : all ;

clean : ; \
	rm -f $(OBJECTS_DIR)/* 
	rm -f gmon.out $(X_FILES)
