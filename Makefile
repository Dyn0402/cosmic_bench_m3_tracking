ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so

OutPutOpt     = -o

MYINCLUDE     = ~/.
MYLIB         = $(PPATH)/lib
OUTPUTDIR     = ./
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)



# Linux with egcs
CXX           = g++
CXXFLAGS      = -O2 -Wall -Wno-deprecated -fno-exceptions -fPIC $(ROOTCFLAGS) -I$(MYINCLUDE)
LD            = g++
LIBS          = $(ROOTLIBS) -lm -ldl -rdynamic
GLIBS         = $(ROOTGLIBS) -L/usr/X11R6/lib -L$(MYLIB) -lXpm -lX11 -lm -ldl -rdynamic -lpthread
LDFLAGS       =  $(GLIBS) 
SOFLAGS       = -shared

#------------------------------------------------------------------------------

exec: DataReader

clean:;		@rm -f *.o *Dict.* *dict* core

.SUFFIXES: .$(SrcSuf)

DataReader: NewDataReader.o datareader.o header.o dataline.o
	$(LD) $^ -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<







