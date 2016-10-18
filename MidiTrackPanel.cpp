#include "MidiTrackPanel.h"
#include "wx/dc.h"
#include "wx/dcclient.h"

BEGIN_EVENT_TABLE(MidiTrackPanel, wxPanel)
    EVT_PAINT(MidiTrackPanel::OnPaint)
END_EVENT_TABLE()

MidiTrackPanel::MidiTrackPanel()
{
}

MidiTrackPanel::~MidiTrackPanel()
{
}

void MidiTrackPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
{
    wxPanel::Create( parent, id, pos, size, style );
}

void MidiTrackPanel::OnPaint( wxPaintEvent& event )
{
    // Draw the image if we have one, otherwise draw a default graphic.
    wxPaintDC dc(this);
	dc.SetBrush(*wxWHITE_BRUSH);
	for( std::list<std::pair<int, int> >::iterator it = _midiNotes.begin(); it != _midiNotes.end(); it++ )
	{
		dc.DrawRectangle((*it).first, ((*it).second * 35)/127, 5, 5);
	}

	event.Skip(false);
}

void MidiTrackPanel::AddMidiNote( int time, int note )
{
	_midiNotes.push_back(std::pair<int, int>(time, note));
	//if( time > _trackLength )
	//{
	//	_trackLength = time;
	//}
}
