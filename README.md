# TODO

* Allow drag-and-drop of MIDI file to open it.
* Make sure we can display track labels containing newlines. Moonlight seems to have one on track 4.
* Fix track rendering. Filter - Take a Picture uses 18% because all events are redrawn every second.
  Instead we should have a pre-rendered bitmap we use to draw before drawing the indicator line.

# MIDIPLayer

A reimplementation of MIDI playback, but with a nice detailed interface.

# MIDIPlayer 1:

An ancient, simple Windows app for playing MIDI files. Originally released at
http://zetacentauri.com

![MIDIPlayer Screenshot](https://github.com/Xangis/MIDIPlayer/blob/master/images/midiplayer1.gif)

A pre-compiled binary is in the bin directory.

Requires DirectMusic, which is no longer supported on 64-bit Windows machines.
