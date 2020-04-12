#ifndef _MIDITRACKPANEL_H_
#define _MIDITRACKPANEL_H_

#include "wx/panel.h"
#include <list>
#include <utility>

#define DEFAULT_WIDTH 636

class MidiTrackPanel : public wxPanel
{
public:
	MidiTrackPanel();
	~MidiTrackPanel();
    MidiTrackPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize &size = wxSize(DEFAULT_WIDTH, 40), long style = 0 )
    {
        Create( parent, id, pos, size, style );
    }
    void Create( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize &size = wxSize(DEFAULT_WIDTH, 40), long style = 0 );
	void OnPaint( wxPaintEvent& event );
	void AddMidiNote(int time, int note);
    void SetLengthInTicks(int length) { _trackLength = length; };
    void SetTrackTitle(const wxString& title);
    void SetPlaybackTick(int tick);
    void GenerateBitmap();
	unsigned int CalculateNotePosition(unsigned int windowHeight, unsigned int noteNumber);
	DECLARE_EVENT_TABLE()
private:
    wxBitmap* _trackBitmap;
    bool _bitmapValid;
	int _trackLength;
    int _playbackTick;
	std::list<std::pair< int, int > > _midiNotes;
    wxString _trackTitle;
};

#endif
