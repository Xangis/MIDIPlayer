#include "wx/wx.h"
#include "wx/aboutdlg.h"
#include "wx/strconv.h"
#include "MidiPlayer.h"
#include "MidiTrackPanel.h"

IMPLEMENT_DYNAMIC_CLASS( MidiPlayer, wxDialog )

BEGIN_EVENT_TABLE( MidiPlayer, wxDialog )
    EVT_CLOSE( MidiPlayer::OnCloseWindow )
	EVT_CHOICE( ID_MIDI_DEVICE, MidiPlayer::OnChangeMidiDevice )
	//EVT_BUTTON( ID_HELPBUTTON, MidiPlayer::OnHelp )
	EVT_BUTTON( ID_BTN_INFO, MidiPlayer::OnInfo )
	EVT_BUTTON( ID_BTN_BROWSE, MidiPlayer::OnBrowse )
	EVT_BUTTON( ID_BTN_PLAY, MidiPlayer::OnPlay )
	EVT_BUTTON( ID_BTN_SAVE, MidiPlayer::OnSave )
	//EVT_BUTTON( ID_BTN_STOP, MidiPlayer::OnStop )
	EVT_BUTTON( ID_BTN_EXIT, MidiPlayer::OnExit )
END_EVENT_TABLE()


MidiPlayer::MidiPlayer( )
{
}

MidiPlayer::~MidiPlayer()
{
	_playing = false;
	Pause();
	AllNotesOff();
	// Give everything a chance to finish up.
	Sleep(10);
	if( _midiFile != NULL )
	{
		delete _midiFile;
	}
	if(_midiOutDevice != NULL )
	{
		_midiOutDevice->closePort();
	}
}

MidiPlayer::MidiPlayer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}


bool MidiPlayer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	//_helpCtrl = NULL;
	_device = NULL;
	_midiFile = NULL;
	_playing = false;
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    _midiOutDevice = new RtMidiOut();
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
    SetFocus();

	wxFileSystem::AddHandler(new wxZipFSHandler());
	//_helpCtrl = new wxHtmlHelpController(wxHF_CONTENTS);
#ifdef WIN32
	//wxFileName filename = wxStandardPaths::Get().GetDataDir() + _("\\proximacontrol.htb");
#else
	//wxFileName filename = wxString(_("./proximacontrol.htb"));
#endif
	//if( !_helpCtrl->AddBook(filename))
	//{
	//	wxMessageBox( _("Unable to load help file.  Please make sure that proximacontrol.htb is in the program directory." ));
	//}
	// Load icon.
	if( _icon.LoadFile(_("keyicon.ico"), wxBITMAP_TYPE_ICO ))
	{
		SetIcon(_icon);
	}

#ifdef WIN32
	// Set up timer
	QueryPerformanceFrequency( &_tickspersec );
	QueryPerformanceCounter( &_currtime );
	_lasttime = _currtime;
#endif
#ifdef linux
    clock_gettime(CLOCK_MONOTONIC, &_currtime);
#endif
#ifdef __APPLE__
	clock_get_time(_clock, &_currtime);
#endif
	Run();

    return true;
}

void MidiPlayer::CreateControls()
{
    MidiPlayer* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	_btnPlay = new wxButton(itemDialog1, ID_BTN_PLAY, _("Play"));
	itemBoxSizer4->Add(_btnPlay, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	//_btnStop = new wxButton(itemDialog1, ID_BTN_STOP, _("Stop"));
	//itemBoxSizer4->Add(_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_btnExit = new wxButton(itemDialog1, ID_BTN_EXIT, _("Exit"));
	itemBoxSizer4->Add(_btnExit, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_btnInfo = new wxButton(itemDialog1, ID_BTN_INFO, _("Info"));
	itemBoxSizer4->Add(_btnInfo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_btnSave = new wxButton(itemDialog1, ID_BTN_SAVE, _("Save"));
	itemBoxSizer4->Add(_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer25 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer25, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	// Look for MIDI output devices before creating choice box.
	wxArrayString deviceList;
    unsigned int nPorts = _midiOutDevice->getPortCount();
    std::string portName;
    std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
    for ( unsigned int i=0; i < nPorts; i++ )
    {
        try
        {
          portName = _midiOutDevice->getPortName(i);
          wxString port(portName.c_str(), wxConvUTF8, portName.length());
          deviceList.Add( port );
        }
        catch (RtMidiError &error)
        {
          error.printMessage();
        }
        std::cout << "  Output Port #" << i+1 << ": " << portName << '\n';
    }
    std::cout << '\n';

    try
    {
        _midiOutDevice->openPort(0);
    }
    catch( RtMidiError &error )
    {
        // I don't know why trying to get a std::string into a wxString is so fucking hard.
        wxString message(error.getMessage().c_str(), wxConvUTF8, error.getMessage().length());
        wxMessageBox(message, _("Error Opening MIDI Out"));
    }

    _device = new wxChoice( itemDialog1, ID_MIDI_DEVICE, wxDefaultPosition, wxDefaultSize, deviceList );
	_device->SetSelection(0);
    itemBoxSizer25->Add(_device, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("File:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtFilename = new wxTextCtrl(itemDialog1, ID_TXT_FILENAME, _(""), wxDefaultPosition, wxSize(460, -1));
	itemBoxSizer3->Add(_txtFilename, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_btnBrowse = new wxButton(itemDialog1, ID_BTN_BROWSE, _("Browse"));
	itemBoxSizer3->Add(_btnBrowse, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_trackPanelSizer = new wxBoxSizer(wxVERTICAL);
	itemBoxSizer2->Add(_trackPanelSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	_txtLoadedFile = new wxStaticText(itemDialog1, ID_TXT_LOADEDFILE, _("File:"), wxDefaultPosition, wxSize(320, -1));
	itemBoxSizer5->Add(_txtLoadedFile, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtSongLength = new wxStaticText(itemDialog1, ID_TXT_LENGTH, _("Length:"), wxDefaultPosition, wxSize(200, -1));
	itemBoxSizer5->Add(_txtSongLength, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	_txtNumEvents = new wxStaticText(itemDialog1, ID_TXT_NUMEVENTS, _("Events:"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtNumEvents, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtNumTracks = new wxStaticText(itemDialog1, ID_TXT_NUMTRACKS, _("Tracks:"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtNumTracks, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtSize = new wxStaticText(itemDialog1, ID_TXT_SIZE, _("Size:"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtSize, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtType = new wxStaticText(itemDialog1, ID_TXT_TYPE, _("Type:"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtPPQN = new wxStaticText(itemDialog1, ID_TXT_TYPE, _("PPQN:"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtPPQN, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtBPM = new wxStaticText(itemDialog1, ID_TXT_TYPE, _("BPM:"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtBPM, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    //itemBoxSizer2->Add(itemBoxSizer12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
}

/**
* Handles window close events.
*/
void MidiPlayer::OnCloseWindow( wxCloseEvent& event )
{
    Destroy();
    event.Skip();
}

/**
* Shows about box.
*/
void MidiPlayer::OnInfo( wxCommandEvent& event )
{
	// Show about box.
    wxAboutDialogInfo info;
    info.SetName(_("MidiPlayer"));
    info.SetVersion(_("2.0"));
    info.SetCopyright(_("(c) 2006-2016 Zeta Centauri"));
	info.AddDeveloper(_("Jason Champion"));
	//info.SetIcon(_icon);
	info.SetLicense(_("MidiPlayer is freeware and may be distributed freely."));
	info.SetWebSite(_("http://zetacentauri.com"));
	info.SetDescription(_("MidiPlayer uses the wxWidgets and RtMidi libraries."));

    wxAboutBox(info);

    event.Skip();
}

void MidiPlayer::OnSave( wxCommandEvent& event )
{
	wxString path = wxString::Format( _("%s%s"), "test", _txtFilename->GetLabel() );
	wxFileDialog fdialog( this, _("Save MIDI File"), path, _(""), _("MIDI Files (*.mid) |*.mid||"), wxFD_SAVE );

	wxString filename;

	if( fdialog.ShowModal() != wxID_OK )
	{
		return;
	}

	wxString fname = fdialog.GetPath();

	_midiFile->Save(fname.c_str());

    event.Skip();
}

void MidiPlayer::OnBrowse( wxCommandEvent& event )
{
	wxString path = _(".\\");
	//wxMessageBox(path);
	wxFileDialog fdialog( this, _("Load MIDI File"), path, _(""), _("MIDI Files (*.mid) |*.mid||"), wxFD_OPEN );

	wxString filename;

	if( fdialog.ShowModal() != wxID_OK )
	{
		return;
	}

	wxString value;
	wxString fname = fdialog.GetPath();
	if( _midiFile != NULL )
	{
		delete _midiFile;
	}
	_midiFile = new MidiFile();
	_midiFile->Load(fname.c_str());
	_txtFilename->SetLabel(fname);

	_txtNumEvents->SetLabel(wxString::Format(_("Events: %d"), _midiFile->GetNumEvents()));
	_txtNumTracks->SetLabel(wxString::Format(_("Tracks: %d"), _midiFile->GetNumTracks()));
	_txtLoadedFile->SetLabel(wxString::Format(_("File: %s"), fname.c_str()));
	_txtSize->SetLabel(wxString::Format(_("Size: %d"), _midiFile->GetSize()));
	_txtType->SetLabel(wxString::Format(_("Type: %d"), _midiFile->GetType()));
	_txtBPM->SetLabel(wxString::Format(_("BPM: %d"), (int)_midiFile->GetBPM()));
	int ppqn = _midiFile->GetPPQN();
	if( ppqn == 0 ) ppqn = 1;
	_txtPPQN->SetLabel(wxString::Format(_("PPQN: %d"), ppqn));

	int length = _midiFile->GetLength();
	int ticks = _midiFile->GetLengthInTicks();
	int seconds = 0;
	int minutes = 0;
	if( length >= 0 )
	{
		seconds = length % 60;
		minutes = length / 60;
	}
	_txtSongLength->SetLabel(wxString::Format(_("Length: %d:%02d (%d ticks)"), minutes, seconds, ticks));

	// Clear any existing tracks before adding new ones.
	_trackPanelSizer->Clear(true);
	for( int i = 0; i < _midiFile->GetNumTracks(); i++ )
	{
		//wxPanel* panel = new wxPanel(this, 0, 0, 620, 40);
		MidiTrackPanel* panel = new MidiTrackPanel(this, -1);
		panel->SetSize(620, 40);
		panel->SetBackgroundColour(wxColour(((i+1) * 33) % 256, ((i + 1) * 49) % 256, ((i + 1) * 65) % 256));
        panel->SetLengthInTicks(ticks);
        const unsigned char* title = _midiFile->GetTrackData(i)->GetTitle();
        if( title != NULL )
        {
            panel->SetTrackTitle(wxString(title));
        }
		MidiTrack* track = _midiFile->GetTrackData(i);
		std::list<MIDIEvent*>* events = track->GetEvents();
		for( std::list<MIDIEvent*>::iterator it = events->begin(); it != events->end(); it++ )
		{
			if( (*it)->message > 0x90 && (*it)->message < 0xA0 )
			{
				panel->AddMidiNote( (*it)->absoluteTime, (*it)->value1 );
			}
		}
		_trackPanelSizer->Add(panel);
	}
	Fit();

	event.Skip();
}

void MidiPlayer::OnPlay( wxCommandEvent& event )
{
	_mutex.Lock();
	if( !_playing )
	{

#ifdef WIN32
            QueryPerformanceCounter( &_currtime );
            QueryPerformanceCounter( &_lasttime );
#endif
#ifdef linux
            clock_gettime(CLOCK_MONOTONIC, &_currtime);
            clock_gettime(CLOCK_MONOTONIC, &_lasttime);
#endif
#ifdef __APPLE__
            clock_get_time(_clock, &_currtime);
            clock_get_time(_clock, &_lasttime);
#endif
		for( int i = 0; i < _midiFile->GetNumTracks(); i++ )
		{
			_midiFile->GetTrackData(i)->MoveToTick(0);
		}
		_playing = true;
		_numTicksElapsed = 0.0;
		_btnPlay->SetLabel(_("Stop"));
	}
	else
	{
		_playing = false;
		_btnPlay->SetLabel(_("Play"));
        AllNotesOff();
	}
	_mutex.Unlock();
    event.Skip();
}

//void MidiPlayer::OnStop( wxCommandEvent& event )
//{
//    event.Skip();
//}

void MidiPlayer::OnExit( wxCommandEvent& event )
{
	Close();
    event.Skip();
}

// Thread to play the beat - makes dialog independent of sound.
void* MidiPlayer::Entry( )
{
	// LARGE_INTEGER difference, sum;
	int counter = 0;
	static int mutateMeasure = 0;

	while( !TestDestroy())
	{
		_mutex.Lock();
		if( _playing == true && _midiFile != NULL )
		{
			// Get pulse length in milliseconds.
			double pulseLength = _midiFile->GetPulseLength() * 1000.0;

#ifdef WIN32
			QueryPerformanceCounter( &_currtime );
			double elapsedMilliseconds = ((_currtime.QuadPart - _lasttime.QuadPart) * 1000.0) / (_tickspersec.QuadPart);
			double ticks = elapsedMilliseconds / pulseLength;
#else
#ifdef linux
            clock_gettime(CLOCK_MONOTONIC, &_currtime);
#endif
#ifdef __APPLE__
			clock_get_time(_clock, &_currtime);
#endif
            double oldNanoseconds = _lasttime.tv_sec * 1000000000.0 + _lasttime.tv_nsec;
            double newNanoseconds = _currtime.tv_sec * 1000000000.0 + _currtime.tv_nsec;
			double elapsedMilliseconds = (newNanoseconds - oldNanoseconds) / 1000000.0;
			double ticks = elapsedMilliseconds / pulseLength;
#endif
			if( (int)(_numTicksElapsed + ticks) > (int)(_numTicksElapsed) )
			{
				for( int i = 0; i < _midiFile->GetNumTracks(); i++ )
				{
					// TODO: Retrieve notes from file starting at pulse X.
					std::list<MIDIEvent*> playableEvents = _midiFile->GetTrackData(i)->AdvanceToTick(_numTicksElapsed+ticks);
					for( std::list<MIDIEvent*>::iterator iter = playableEvents.begin(); iter != playableEvents.end(); iter++ )
					{
						MIDIEvent* midiEvent = *iter;
						// So far we only send note on and note off data.
						if( midiEvent->message >= 144 && midiEvent->message < 176 )
						{
							printf("MIDI Note found.\n");
							// 00 (not used), 7F (velocity), 2B (note number), 9X (note on)+channel
							SendMidiMessage( 0, midiEvent->value2, midiEvent->value1, midiEvent->message );
						}
						else if( midiEvent->message >= 192 && midiEvent->message < 208 )
						{
							SendMidiMessage( 0, 0, midiEvent->value1, midiEvent->message );
						}
						else
						{
							printf("Not sure what to do with message %d.\n", midiEvent->message );
						}
					}
					// Set our time so we know when to play the next beat.
					// By not adding the time it took us to actually play the beat we maintain timing consistency.
				}
				_lasttime = _currtime;
				_mutex.Unlock();
			}
			_mutex.Unlock();
			_numTicksElapsed += ticks;
			// TODO: When the song is finished playing, indicate that playing has stopped
			// (change the play button, etc)
			//if( _playing && _numTicksElapsed > _midiFile->GetLengthInTicks() )
			//{
			//	wxCommandEvent evt;
			//	//evt.SetId(ID_BTN_PLAY);
			//	this->GetEventHandler()->AddPendingEvent(evt);
			//	_btnPlay->GetEventHandler()->AddPendingEvent(evt);
			//}
			Sleep(1);
		}
		else
		{
			_mutex.Unlock();
			Sleep(1);
		}
	}
	return 0;
}

void MidiPlayer::SendMidiMessage(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4, bool shortmsg)
{
    std::vector<unsigned char> msg;
    msg.push_back(byte4);
    msg.push_back(byte3);
    if(!shortmsg && _midiOutDevice != NULL)
    {
      msg.push_back(byte2);
      if( byte1 != 0 )
      {
        msg.push_back(byte1);
      }
#ifndef VST
      _midiOutDevice->sendMessage(&msg);
#endif
    }
}

/**
* Turns off all notes.
*/
void MidiPlayer::AllNotesOff()
{
	// 00 (not used), 0x00, 123, 0xBX (message + channel)
	for( int i = 0; i < 16; i++ )
	{
		SendMidiMessage( 0, 0, 123, (176 + i) );
	}
}

/**
* Processes MIDI output device selection changes.
*/
void MidiPlayer::OnChangeMidiDevice( wxCommandEvent& event )
{
    AllNotesOff();
    try
    {
        // Close the old device
        std::cout << "Closing port: " << _midiOutDevice->getPortName() << std::endl;
        _midiOutDevice->closePort();
	    // Subtract 1 because the first device is -1 (MIDI Mapper).
	    int selection = _device->GetCurrentSelection( );
        std::cout << "Opening port: " << _midiOutDevice->getPortName(selection) << std::endl;
	    _midiOutDevice->openPort(selection);
    }
    catch( RtMidiError &error )
    {
        // I don't know why trying to get a std::string into a wxString is so fucking hard.
        //wxMessageBox(wxString(error.getMessage()), _("Error Opening MIDI Out"));
    }
}
