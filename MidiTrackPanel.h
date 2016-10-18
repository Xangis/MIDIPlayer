#ifndef _MIDITRACKPANEL_H_
#define _MIDITRACKPANEL_H_

#include "wx/panel.h"
#include <list>
#include <utility>

class MidiTrackPanel : public wxPanel
{
public:
	MidiTrackPanel();
	~MidiTrackPanel();
    MidiTrackPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize &size = wxSize(620, 40), long style = 0 )
    {
        Create( parent, id, pos, size, style );
    }
    void Create( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize &size = wxSize(620, 40), long style = 0 );
	void OnPaint( wxPaintEvent& event );
	void AddMidiNote(int time, int note);
	DECLARE_EVENT_TABLE()
private:
	int _trackLength;
	std::list<std::pair< int, int > > _midiNotes;
};

#endif