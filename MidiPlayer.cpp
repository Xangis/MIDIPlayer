#include "wx/wx.h"
#include "wx/aboutdlg.h"
#include "wx/strconv.h"
#include "MidiPlayer.h"
#include "MidiTrackPanel.h"

IMPLEMENT_DYNAMIC_CLASS( MidiPlayer, wxDialog )

BEGIN_EVENT_TABLE( MidiPlayer, wxDialog )
    EVT_CLOSE( MidiPlayer::OnCloseWindow )
	EVT_CHOICE( ID_MIDI_DEVICE, MidiPlayer::OnChangeMidiDevice )
	EVT_BUTTON( ID_BTN_INFO, MidiPlayer::OnInfo )
    EVT_BUTTON( ID_BTN_PAUSE, MidiPlayer::OnPause )
	EVT_BUTTON( ID_BTN_BROWSE, MidiPlayer::OnBrowse )
	EVT_BUTTON( ID_BTN_PLAY, MidiPlayer::OnPlay )
	//EVT_BUTTON( ID_BTN_SAVE, MidiPlayer::OnSave )
	EVT_BUTTON( ID_BTN_STOP, MidiPlayer::OnStop )
	EVT_BUTTON( ID_BTN_TIME, MidiPlayer::OnTime )
END_EVENT_TABLE()


MidiPlayer::MidiPlayer( )
{
}

MidiPlayer::~MidiPlayer()
{
    // Disable drag and drop. If we don't do this, the program crashes.
    //SetDefaultAction(wxDragNone);
    //SetDropTarget(NULL);

    _playing = false;
    _paused = false;
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
#ifdef __APPLE__
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &_clock);
#endif
	//_helpCtrl = NULL;
	_device = NULL;
	_midiFile = NULL;
	_playing = false;
    _paused = false;
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

#ifndef __APPLE__
        wxString filepath = _("keyicon.ico");
#else
        wxString filepath = wxString::Format(_("%s//%s"), wxStandardPaths::Get().GetResourcesDir(), _("keyicon.ico"));
#endif
        if( _icon.LoadFile(filepath, wxBITMAP_TYPE_ICO ))
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

    // Enable drag and drop.
    //SetDropTarget(this);
    //SetDefaultAction(wxDragCopy);

    return true;
}

void MidiPlayer::CreateControls()
{
    MidiPlayer* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

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

    wxStaticText* itemStaticText39 = new wxStaticText( itemDialog1, wxID_STATIC, _("MIDI Device:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer25->Add(itemStaticText39, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

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

	//_btnSave = new wxButton(itemDialog1, ID_BTN_SAVE, _("Save"));
	//itemBoxSizer3->Add(_btnSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	//_txtTimeElapsed = new wxStaticText(itemDialog1, ID_TXT_TIME_ELAPSED, _("Time: 0:00.000"), wxDefaultPosition, wxSize(120, -1));
	_txtTimeElapsed = new wxStaticText(itemDialog1, ID_TXT_TIME_ELAPSED, _("Time: 0:00"), wxDefaultPosition, wxSize(106, -1));
	itemBoxSizer5->Add(_txtTimeElapsed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtSongLength = new wxStaticText(itemDialog1, ID_TXT_LENGTH, _("Length: 0:00"), wxDefaultPosition, wxSize(190, -1));
	itemBoxSizer5->Add(_txtSongLength, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_btnPlay = new wxButton(itemDialog1, ID_BTN_PLAY, _("Play"));
	itemBoxSizer5->Add(_btnPlay, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_btnPause = new wxButton(itemDialog1, ID_BTN_PAUSE, _("Pause"));
	itemBoxSizer5->Add(_btnPause, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    _btnPause->Enable(false);

	_btnInfo = new wxButton(itemDialog1, ID_BTN_INFO, _("Info"));
	itemBoxSizer5->Add(_btnInfo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_trackPanelSizer = new wxBoxSizer(wxVERTICAL);
	itemBoxSizer2->Add(_trackPanelSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    
    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer6, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	_txtNumEvents = new wxStaticText(itemDialog1, ID_TXT_NUMEVENTS, _("Events: 0"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtNumEvents, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtNumTracks = new wxStaticText(itemDialog1, ID_TXT_NUMTRACKS, _("Tracks: 0"), wxDefaultPosition, wxSize(86, -1));
	itemBoxSizer6->Add(_txtNumTracks, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtSize = new wxStaticText(itemDialog1, ID_TXT_SIZE, _("Size: 0"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtSize, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtType = new wxStaticText(itemDialog1, ID_TXT_TYPE, _("Type: 0"), wxDefaultPosition, wxSize(82, -1));
	itemBoxSizer6->Add(_txtType, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtPPQN = new wxStaticText(itemDialog1, ID_TXT_TYPE, _("PPQN: 0"), wxDefaultPosition, wxSize(100, -1));
	itemBoxSizer6->Add(_txtPPQN, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	_txtBPM = new wxStaticText(itemDialog1, ID_TXT_TYPE, _("BPM: 0"), wxDefaultPosition, wxSize(96, -1));
	itemBoxSizer6->Add(_txtBPM, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

/**
* Handles window close events.
*/
void MidiPlayer::OnCloseWindow( wxCloseEvent& event )
{
    Destroy();
    event.Skip();
}

void MidiPlayer::OnPause( wxCommandEvent& event )
{
    if( _paused )
    {
        _btnPause->SetLabel(_("Pause"));
        _paused = false;
    }
    else
    {
        _btnPause->SetLabel(_("Unpause"));
        _paused = true;
        AllNotesOff();
    }
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
    info.SetVersion(_("2.11"));
    info.SetCopyright(_("(c) 2006-2017 Zeta Centauri"));
	info.AddDeveloper(_("Jason Champion"));
	//info.SetIcon(_icon);
	info.SetLicense(_("MidiPlayer is freeware and may be distributed freely."));
	info.SetWebSite(_("http://zetacentauri.com"));
	info.SetDescription(_("MidiPlayer uses the wxWidgets and RtMidi libraries."));

    wxAboutBox(info);

    event.Skip();
}

//void MidiPlayer::OnSave( wxCommandEvent& event )
//{
//	wxString path = wxString::Format( _("%s%s"), "test", _txtFilename->GetLabel() );
//	wxFileDialog fdialog( this, _("Save MIDI File"), path, _(""), _("MIDI Files (*.mid) |*.mid||"), wxFD_SAVE );
//
//	wxString filename;
//
//	if( fdialog.ShowModal() != wxID_OK )
//	{
//		return;
//	}
//
//	wxString fname = fdialog.GetPath();
//
//	_midiFile->Save(fname.c_str());
//
//    event.Skip();
//}

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
    LoadFile(fname);
    event.Skip();
}

void MidiPlayer::LoadFile(const wxString& filename)
{
    if( _midiFile != NULL )
	{
		delete _midiFile;
	}
	_midiFile = new MidiFile();
	_midiFile->Load(filename.c_str());
	_txtFilename->SetLabel(filename);

	_txtNumEvents->SetLabel(wxString::Format(_("Events: %d"), _midiFile->GetNumEvents()));
	_txtNumTracks->SetLabel(wxString::Format(_("Tracks: %d"), _midiFile->GetNumTracks()));
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

    // Clear existing tracks.
	_trackPanelSizer->Clear(true);
    _trackPanelSizer->Show(false);
	for( int i = 0; i < _midiFile->GetNumTracks(); i++ )
	{
		//wxPanel* panel = new wxPanel(this, 0, 0, 620, 40);
		MidiTrackPanel* panel = new MidiTrackPanel(this, -1);
		panel->SetSize(620, 40);
        int colorIterator = i;
        if( colorIterator >= 4 ) { colorIterator++; } // Skip this color, it's ugly.
		panel->SetBackgroundColour(wxColour(((colorIterator+1) * 33) % 128, ((colorIterator + 1) * 43) % 224, ((colorIterator + 1) * 65) % 224));
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
			if( (*it)->message >= 0x90 && (*it)->message < 0xA0 )
			{
				panel->AddMidiNote( (*it)->absoluteTime, (*it)->value1 );
			}
		}
		_trackPanelSizer->Add(panel);
	}
    _trackPanelSizer->Show(true);
	Fit();
    Layout();
}

void MidiPlayer::OnPlay( wxCommandEvent& event )
{
	_mutex.Lock();
	if( !_playing )
	{
        if( _midiFile != NULL )
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
		    _numTicksElapsed = 0.0;
            _numMsecElapsed = 0.0;
            //_txtTimeElapsed->SetLabel("Time: 0:00.000");
            _txtTimeElapsed->SetLabel("Time: 0:00");
		    _playing = true;
		    _btnPlay->SetLabel(_("Stop"));
            _btnPause->Enable(true);
        }
	}
	else
	{
		_playing = false;
        _paused = false;
		_btnPlay->SetLabel(_("Play"));
        _btnPause->Enable(false);
        AllNotesOff();
	}
	_mutex.Unlock();
    event.Skip();
}

/**
* Stops playback. There's not a button for this, instead this exists to handle stop
* events posted by the playback thread reaching the end of the song.
*/
void MidiPlayer::OnStop( wxCommandEvent& event )
{
	_mutex.Lock();
	_playing = false;
    _paused = false;
	_btnPlay->SetLabel(_("Play"));
	_mutex.Unlock();
    AllNotesOff();
}

/**
* Fake command event that lets us know we need to update the display time.
*/
void MidiPlayer::OnTime( wxCommandEvent& event )
{
    _mutex.Lock();
    double time = _numMsecElapsed;
    double tick = _numTicksElapsed;
    _mutex.Unlock();
    int seconds = time / 1000.0;
    int milliseconds = (int)time % 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    ///_txtTimeElapsed->SetLabel(wxString::Format(_("Time: %d:%02d.%03d"), minutes, seconds, milliseconds));
    _txtTimeElapsed->SetLabel(wxString::Format(_("Time: %d:%02d"), minutes, seconds));
    if( _midiFile != NULL )
    {
        wxSizerItemList list = _trackPanelSizer->GetChildren();
        for( unsigned int i = 0; i < list.size(); i++ )
        {
            wxWindow* window = list[i]->GetWindow();
            MidiTrackPanel* panel = dynamic_cast<MidiTrackPanel*>(window);
            if( panel != NULL )
            {
                panel->SetPlaybackTick(tick);
            }
        }
    }
}

//void MidiPlayer::OnExit( wxCommandEvent& event )
//{
//	Close();
//    event.Skip();
//}

// Thread to play the beat - makes dialog independent of sound.
void* MidiPlayer::Entry( )
{
	// LARGE_INTEGER difference, sum;
	int counter = 0;
	static int mutateMeasure = 0;

	while( !TestDestroy())
	{
		_mutex.Lock();
        if( _paused == true )
        {
            // Keep our counter ticking so we don't jump a bunch of events when unpausing.
            QueryPerformanceCounter( &_lasttime );
			_mutex.Unlock();
			Sleep(1);
        }
		else if( _playing == true && _midiFile != NULL )
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
						if( midiEvent->message >= 128 && midiEvent->message < 224 )
						{
							// 00 (not used), 7F (velocity), 2B (note number), 9X (note on)+channel
							SendMidiMessage( 0, midiEvent->value2, midiEvent->value1, midiEvent->message );
						}
                        else if( midiEvent->message == 0xFF && midiEvent->value1 == 0x51 ) // Tempo change message.
                        {
                            int tempo = midiEvent->lval;
                            double calculatedTempo = 60000000.0 / tempo;
                            printf("Tempo change to %f.", calculatedTempo);
                            _midiFile->SetBPM(calculatedTempo);
                        }
						else
						{
							printf("Not sure what to do with message %d.\n", midiEvent->message );
						}
					}
				}
				// Only update the time when we've processed events (increased the number of ticks).
				_lasttime = _currtime;
			    _numTicksElapsed += ticks;
                _numMsecElapsed += elapsedMilliseconds;
                // Send a time update event once per second.
                if( (int)((_numMsecElapsed - elapsedMilliseconds) / 1000.0) < (int)(_numMsecElapsed / 1000.0)) 
                {
                    wxCommandEvent* evt = new wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_BTN_TIME);
                    QueueEvent(evt);
                }
			}
            // When the song is finished playing, auto-stop.
			if( _playing && _numTicksElapsed > _midiFile->GetLengthInTicks() )
            {
                wxCommandEvent* evt = new wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_BTN_STOP);
                QueueEvent(evt);
            }
            _mutex.Unlock();
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

bool MidiPlayer::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
{
    if( filenames.size() > 0 )
    {
        wxCommandEvent evt = wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_BTN_STOP);
        OnStop(evt);
        LoadFile(filenames[0]);
        wxCommandEvent evt2 = wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_BTN_PLAY);
        OnPlay(evt2);
        return true;
    }
    return false;
}
