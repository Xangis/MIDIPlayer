# TODO

* Fix the drag-and-drop destructor crash so we can re-enable drag and drop.

# MIDIPLayer 2

A reimplementation of MIDI playback, but with a nice detailed interface.

A new MIDI engine has been written from scratch and it no longer relies on any DirectX code.
It uses RtMidi for platform-agnostic MIDI playback.

It also showss the individual tracks of the MIDI file.

# MIDIPlayer 1

An ancient, simple Windows app for playing MIDI files. Originally released at
http://zetacentauri.com

![MIDIPlayer Screenshot](https://github.com/Xangis/MIDIPlayer/blob/master/images/midiplayer1.gif)

A pre-compiled binary is in the bin directory.

Requires DirectMusic, which is no longer supported on 64-bit Windows machines.
