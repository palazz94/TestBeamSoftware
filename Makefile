UNAME    = $(shell uname)
EXE      = reco
 
VPATH  = .:./interface
vpath %.h ./interface

CSUF   = cc
HSUF   = h
DICTC  = Dict.$(CSUF)
DICTH  = $(patsubst %.$(CSUF),%.h,$(DICTC))

SRCS   = src/argvparser.cc src/Utility.cc src/tBeamBase.cc src/trawTupleBase.cc src/Reconstruction.cc  src/ReconstructionFromRaw.cc src/recoMain.cc
OBJS   = $(patsubst %.$(CSUF), %.o, $(SRCS))

LDFLAGS  = -g
SOFLAGS  = -shared 
CXXFLAGS = -I./interface -I./  

CXX       = g++
CXXFLAGS += -g -std=c++11
#-Wall -Wno-deprecated -std=c++11


HDRS_DICT = interface/LinkDef.h

bin: $(EXE) 
all: 
	gmake cint 
	gmake bin 
cint: $(DICTC) 

$(DICTC): $(HDRS_DICT)
	@echo "Generating dictionary $(DICTC) and $(DICTH) ..."
	rootcint -f $@ -c $(CXXFLAGS) $^ 
	perl -pi -e 's#interface/##' $(DICTH) 
	perl -pi -e 's/#include <math.h>/#include <math.h>\n#include <map>/'  $(DICTH)
	mv $(DICTC) src/
	mv $(DICTH) interface/

$(EXE): $(OBJS) src/Dict.o
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS) `root-config --libs`

# Create object files
%.o : %.$(CSUF)
	$(CXX) $(CXXFLAGS) `root-config --cflags` -o $@ -c $<

# makedepend
depend: $(SRCS:.$(CSUF)=.$(CSUF).dep)
	@cat $(notdir $^) > Makefile.dep
	@-rm -f $(notdir $^) $(patsubst %,%.bak,$(notdir $^))
%.dep:
	@touch $(notdir $@)
	rmkdepend -f$(notdir $@) -- $(CXXFLAGS) `root-config --cflags` -- $*
include Makefile.dep

# Clean 
.PHONY   : clean 
clean : 
	@-rm $(OBJS) $(EXE) interface/$(DICTH) src/$(DICTC) src/Dict.o
