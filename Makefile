ROOT_LIBS:=$(shell root-config --libs)  #LIBS memorizes the path to ROOT's
                                        #libraries


ROOT_INCS:=$(shell root-config --cflags) #INCS memorizes the path to ROOT's
																				 # header files

PRJ-DIR = $(PWD)

X-FILES = tnds-analysis tnds-net-parser

LIB = UDPframebroadcast

OBJECTS-DIR = $(PRJ-DIR)/obj
OBJECTS-analysis = main_tnds-analysis.o
OBJECTS-net-parser = main_tnds-net-parser.o
OBJECTS-lib = UDPframebroadcast.o
OBJECTS = $(OBJECTS-analysis) $(OBJECTS-net-parser) $(OBJECTS-lib)

DEBUG = -g -O0 -DDEBUG

PROFILING = -pg -O0

%.o : $(WORKING-DIR)/%.cpp ; \
	$(COMPILE.cpp) $(ROOT_INCS) $(WORKING-DIR)/$< \
	mv $@ $(OBJECTS-DIR) 

all : $(LIB) $(X-FILES) ;

tnds-analysis : WORKING-DIR = $(PRJ-DIR)/src/tnds-analysis
tnds-analysis : $(OBJECTS-analysis) ; \
	$(LINK.o) -o $@ $^ $(ROOT_LIBS) -lASImage

tnds-net-parser : WORKING-DIR = $(PRJ-DIR)/src/tnds-net-parser
tnds-net-parser : $(OBJECTS-net-parser) ; \
	$(LINK.o) -o $@ $^

UDPframebroadcast : WORKING-DIR = $(PRJ-DIR)/lib/UDPframebroadcast
UDPframebroadcast : $(OBJECTS-lib) ;

debug : CXXFLAGS := $(CXXFLAGS) $(DEBUG)
debug : all ;

profiles : CXXFLAGS := $(CXXFLAGS) $(PROFILING)
profiles : all ;

clean : ; rm -f $(OBJECTS-DIR)/$(OBJECTS) gmon.out $(X-FILES)
