#include "wx/wx.h"
#include "wx/aboutdlg.h"
#include "wx/strconv.h"
#include "MidiPlayer.h"

IMPLEMENT_DYNAMIC_CLASS( MidiPlayer, wxDialog )

BEGIN_EVENT_TABLE( MidiPlayer, wxDialog )
    EVT_CLOSE( MidiPlayer::OnCloseWindow )
	//EVT_CHOICE( ID_MIDI_DEVICE, MidiPlayer::OnChangeMidiDevice )
	//EVT_BUTTON( ID_HELPBUTTON, MidiPlayer::OnHelp )
	EVT_BUTTON( ID_BTN_INFO, MidiPlayer::OnInfo )
	EVT_BUTTON( ID_BTN_BROWSE, MidiPlayer::OnBrowse )
	EVT_BUTTON( ID_BTN_PLAY, MidiPlayer::OnPlay )
	EVT_BUTTON( ID_BTN_STOP, MidiPlayer::OnStop )
	EVT_BUTTON( ID_BTN_EXIT, MidiPlayer::OnExit )
END_EVENT_TABLE()


MidiPlayer::MidiPlayer( )
{
}

MidiPlayer::MidiPlayer( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}


bool MidiPlayer::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	//_helpCtrl = NULL;
	_midiFile = NULL;
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
    return true;
}

void MidiPlayer::CreateControls()
{
    MidiPlayer* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("File:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer3->Add(itemStaticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxTextCtrl* _txtFilename = new wxTextCtrl(itemDialog1, ID_TXT_FILENAME, _(""), wxDefaultPosition, wxSize(360, -1));
	itemBoxSizer3->Add(_txtFilename, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxButton* _btnBrowse = new wxButton(itemDialog1, ID_BTN_BROWSE, _("Browse"));
	itemBoxSizer3->Add(_btnBrowse, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	wxButton* _btnPlay = new wxButton(itemDialog1, ID_BTN_PLAY, _("Play"));
	itemBoxSizer4->Add(_btnPlay, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxButton* _btnStop = new wxButton(itemDialog1, ID_BTN_STOP, _("Stop"));
	itemBoxSizer4->Add(_btnStop, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxButton* _btnExit = new wxButton(itemDialog1, ID_BTN_EXIT, _("Exit"));
	itemBoxSizer4->Add(_btnExit, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxButton* _btnInfo = new wxButton(itemDialog1, ID_BTN_INFO, _("Info"));
	itemBoxSizer4->Add(_btnInfo, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

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

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
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
	if( _midiFile == NULL )
	{
		_midiFile = new MidiFile();
	}
	_midiFile->Load(fname.c_str());

    event.Skip();
}

void MidiPlayer::OnPlay( wxCommandEvent& event )
{
    event.Skip();
}

void MidiPlayer::OnStop( wxCommandEvent& event )
{
    event.Skip();
}

void MidiPlayer::OnExit( wxCommandEvent& event )
{
	Close();
    event.Skip();
}
