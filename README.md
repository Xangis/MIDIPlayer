# MidiPlayer

MIDIPlayer is a free MIDI file player for Windows, Linux, and OSX with detailed 
file information display including individual track details (including title 
and note data).

It was originally released on http://zetacentauri.com and was downloaded more than
20,000 times before being open-sourced.

![MIDIPlayer 2 Screenshot](https://github.com/Xangis/MIDIPlayer/blob/master/images/MIDIPlayer2.png)

A prebuilt Windows installer is available in the installer directory:

https://github.com/Xangis/MIDIPlayer/blob/master/installer/MIDIPlayer2.11Setup.exe

It is licensed under the terms of the MIT license.

# Building

The UI for this application is written using wxWidgets and you'll need that in order
to build. You can get it here:

http://wxwidgets.org/

Sound playback requires RtMidi, available here:

http://www.music.mcgill.ca/~gary/rtmidi/

### Windows

Use the Visual Studio project. You'll probably have to set the wxWidgets include and
library paths in order to build because they'll be set to whatever was on the last
dev machine that was used.

The Visual Studio project is for Visual Studio 2013. If you have a newer version,
it should auto-convert.

### Linux

To get rtmidi and wxWidgets, you can install then using your package manager:

sudo apt-get install librtmidi-dev libwxgtk3.0-dev

Build the app by running "Make -f Makefile.linux". You may need to edit some path
settings if it doesn't see your rtmidi libraries.

You're probably better off downloading RtMidi from their website rather than using the
version available in your Linux distribution's repositories.

### OSX

Building is similar to linux, but just run "make". You'll have to have the command-line
compiler installed for that to work.

Once the program is built, run "OSXLibraryPaths.sh" in the installer directory to
set paths inside the application bundle. You'll have to run "OSXCopyLibs.sh" the first
time to copy the libraries in, though.

Ths OSX build has had the least amount of testing and may need some improvements to
be easier to build.

Building wxWidgets won't work out of the box. You'll need to specify the SDK version, like so:

./configure --with-macosx-version-min=10.11

# Changelog

### MIDIPlayer 2.12 Changes (April? 2020)

* Fixed track note visualization because it was flipped on the Y axis (low notes were shown high).
[TODO] * Updated RtMIDI from 2.1.0 to 4.0.0.

### MIDIPlayer 2.11 Changes (January 2017)

* Fixed window redraw problem when loading a new song when one is already loaded.
* Added pause button.

### MIDIPlayer 2.1 Changes (October 2016)

* Added some error handling for loading files with incorrect track length data.
* Changed file reading to binary (why wasn't it before? WTF?)

### MIDIPlayer 2 (October 2016)

A reimplementation of MIDI playback, but with a nice detailed interface.

A new MIDI engine has been written from scratch and it no longer relies on any DirectX code.
It uses RtMidi for platform-agnostic MIDI playback.

It also shows the individual tracks of the MIDI file.

### MIDIPlayer 1

An ancient, simple Windows app for playing MIDI files. Originally released at
http://zetacentauri.com

![MIDIPlayer Screenshot](https://github.com/Xangis/MIDIPlayer/blob/master/images/midiplayer1.gif)

A pre-compiled binary is in the bin directory.

Requires DirectMusic, which is no longer supported on 64-bit Windows machines.

## TODO

* Fix the drag-and-drop destructor crash so we can re-enable drag and drop. This appears
  to be a bug in wxWidgets (it crashes in their pointer handling), but haven't confirmed.
* More + better encapsulation for the MIDIFile class. Some things are ugly for
  the sake of expediency.

# Development Status

I consider this program to be feature complete, but I still actively maintain it.

I am willing to accept pull requests if you have improvements to contribute.
