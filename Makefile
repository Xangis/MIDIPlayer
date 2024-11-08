# Sigmatizm makefile.  Requires wxWidgets.
# by default, wx-config from the PATH is used
WX_CONFIG := wx-config

# Main executable file
PROGRAM = installer/MidiPlayer.app/Contents/MacOS/MidiPlayer

# Directory containing library portions of code.
INCLUDEDIR2 = /usr/local/include
INCLUDEDIR3 = ../../lib/rtmidi-4.0.0
LIBDIR = /usr/local/lib

# Object files
OBJECTS = MidiPlayerApp.o MidiPlayer.o MidiTrackPanel.o $(INCLUDEDIR3)/RtMidi.o ../MidiFile/MidiFile.o ../MidiFile/MidiTrack.o

CXX = $(shell $(WX_CONFIG) --cxx -ggdb)
#CXX = $(shell $(WX_CONFIG) --cxx -O3)

.SUFFIXES:	.o .cpp

.cpp.o :
#	$(CXX) -c -O3 -D__MACOSX_CORE__ -I$(INCLUDEDIR2) -I$(INCLUDEDIR3) `$(WX_CONFIG) --cxxflags` -o $@ $<
	$(CXX) -c -g -ggdb -D__MACOSX_CORE__ -I$(INCLUDEDIR2) -I$(INCLUDEDIR3) `$(WX_CONFIG) --cxxflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) -L$(LIBDIR) `$(WX_CONFIG) --libs` -lpthread -framework CoreMidi -framework CoreAudio

clean: 
	rm -f *.o $(PROGRAM) $(INCLUDEDIR3)/*.o
