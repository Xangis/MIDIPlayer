#include "MidiTrackPanel.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/dcmemory.h"

BEGIN_EVENT_TABLE(MidiTrackPanel, wxPanel)
    EVT_PAINT(MidiTrackPanel::OnPaint)
END_EVENT_TABLE()

MidiTrackPanel::MidiTrackPanel()
{
    _trackLength = -1;
    _playbackTick = -1;
    _bitmapValid = false;
    _trackBitmap = NULL;
}

MidiTrackPanel::~MidiTrackPanel()
{
    if( _trackBitmap != NULL )
    {
        delete _trackBitmap;
    }
}

void MidiTrackPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
{
    _trackLength = -1;
    _playbackTick = -1;
    _bitmapValid = false;
    _trackBitmap = NULL;
    wxPanel::Create( parent, id, pos, size, style );
}

void MidiTrackPanel::SetPlaybackTick(int tick)
{
    _playbackTick = tick;
    Refresh();
}

void MidiTrackPanel::GenerateBitmap()
{
    if( _trackBitmap != NULL )
    {
        delete _trackBitmap;
    }
    _trackBitmap = new wxBitmap(this->GetSize());
    wxMemoryDC dc;
    dc.SelectObject(*_trackBitmap);
    // Draw the image if we have one, otherwise draw a default graphic.
    dc.SetPen(GetBackgroundColour());
    dc.SetBrush(GetBackgroundColour());
    dc.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    wxColour ltgrey = wxColour(192, 192, 192);
	dc.SetBrush(ltgrey);
    dc.SetPen(ltgrey);
    int width = GetSize().GetWidth();
	for( std::list<std::pair<int, int> >::iterator it = _midiNotes.begin(); it != _midiNotes.end(); it++ )
	{
        int timePos = (*it).first;
        if( _trackLength > 0 )
        {
            timePos = ((*it).first * width) / _trackLength;
        }
		dc.DrawRectangle(timePos, ((*it).second * 35)/127, 3, 3);
    }
    if( _trackTitle.length() > 0 )
    {
        wxFont font = wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);
        dc.SetTextForeground(*wxWHITE);
        dc.DrawText(_trackTitle, 2, 21);
    }
    dc.SelectObject(wxNullBitmap);
    _bitmapValid = true;
}

void MidiTrackPanel::OnPaint( wxPaintEvent& event )
{
    // Draw the image if we have one, otherwise draw a default graphic.
    wxPaintDC dc(this);
    if( !_bitmapValid || !_trackBitmap || !_trackBitmap->IsOk() )
    {
        GenerateBitmap();
    }
    dc.DrawBitmap(*_trackBitmap, 0, 0);
    wxSize size = GetSize();
    if( _playbackTick > 0 )
    {
        dc.SetPen(*wxGREY_PEN);
        int linePos = (_playbackTick * size.GetWidth()) / _trackLength;
        dc.DrawLine(linePos, 0, linePos, size.GetHeight());
    }
	event.Skip(false);
}

void MidiTrackPanel::AddMidiNote( int time, int note )
{
	_midiNotes.push_back(std::pair<int, int>(time, note));
	if( time > _trackLength )
	{
		_trackLength = time;
	}
    _bitmapValid = false;
}

void MidiTrackPanel::SetTrackTitle(const wxString& title)
{
    _trackTitle = wxString(title); 
};
