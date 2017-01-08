# TODO

* Fix the drag-and-drop destructor crash so we can re-enable drag and drop. This appears
  to be a bug in wxWidgets (it crashes in their pointer handling), but haven't confirmed.
* More + better encapsulation for the MIDIFile class. Some things are ugly for
  the sake of expediency.

# MIDIPlayer 2.11 Changes (January 2017)

* Fixed window redraw problem when loading a new song when one is already loaded.
* Added pause button.

# MIDIPlayer 2.1 Changes (October 2016)

* Added some error handling for loading files with incorrect track length data.
* Changed file reading to binary (why wasn't it before? WTF?)

# MIDIPLayer 2 (October 2016)

A reimplementation of MIDI playback, but with a nice detailed interface.

A new MIDI engine has been written from scratch and it no longer relies on any DirectX code.
It uses RtMidi for platform-agnostic MIDI playback.

It also shows the individual tracks of the MIDI file.

# MIDIPlayer 1

An ancient, simple Windows app for playing MIDI files. Originally released at
http://zetacentauri.com

![MIDIPlayer Screenshot](https://github.com/Xangis/MIDIPlayer/blob/master/images/midiplayer1.gif)

A pre-compiled binary is in the bin directory.

Requires DirectMusic, which is no longer supported on 64-bit Windows machines.
