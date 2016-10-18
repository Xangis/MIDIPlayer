#ifndef _MIDIPLAYER_H_
#define _MIDIPLAYER_H_

/*!
 * Includes
 */
#include "wx/filesys.h"
#include "wx/fs_zip.h"
#include "wx/html/helpctrl.h"
#include "wx/stdpaths.h"
#include "RtMidi.h"
#include "../MidiFile/MidiFile.h"

// High-performance timer.
#ifdef linux
#include <time.h>
#endif
#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

/*!
 * Control identifiers
 */
#define ID_KEYBOARD_DLG 10000
#define SYMBOL_WXKEYBOARD_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxWANTS_CHARS|wxMINIMIZE_BOX
#define SYMBOL_WXKEYBOARD_TITLE _("MIDIPlayer")
#define SYMBOL_WXKEYBOARD_IDNAME ID_KEYBOARD_DLG
#define SYMBOL_WXKEYBOARD_SIZE wxSize(400, 300)
#define SYMBOL_WXKEYBOARD_POSITION wxDefaultPosition
#define ID_TXT_FILENAME 10001
#define ID_BTN_BROWSE 10002
#define ID_BTN_PLAY 10003
#define ID_BTN_STOP 10004
#define ID_BTN_EXIT 10005
#define ID_BTN_INFO 10006
#define ID_TXT_NUMEVENTS 10007
#define ID_TXT_NUMTRACKS 10008
#define ID_TXT_LENGTH 10009
#define ID_TXT_LOADEDFILE 10010
#define ID_MIDI_DEVICE 10011
#define ID_TXT_SIZE 10012
#define ID_TXT_TYPE 10013
#define ID_MIDI_DEVICE 10014

/*!
 * Compatibility
 */
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxKeyboard class declaration
 */
class MidiPlayer: public wxDialog, public wxThread
{
    DECLARE_DYNAMIC_CLASS( MidiPlayer )
    DECLARE_EVENT_TABLE()
public:
    /// Constructors
    MidiPlayer( );
    MidiPlayer( wxWindow* parent, wxWindowID id = SYMBOL_WXKEYBOARD_IDNAME, const wxString& caption = SYMBOL_WXKEYBOARD_TITLE, const wxPoint& pos = SYMBOL_WXKEYBOARD_POSITION, const wxSize& size = SYMBOL_WXKEYBOARD_SIZE, long style = SYMBOL_WXKEYBOARD_STYLE );
	virtual ~MidiPlayer();
    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXKEYBOARD_IDNAME, const wxString& caption = SYMBOL_WXKEYBOARD_TITLE, const wxPoint& pos = SYMBOL_WXKEYBOARD_POSITION, const wxSize& size = SYMBOL_WXKEYBOARD_SIZE, long style = SYMBOL_WXKEYBOARD_STYLE );
    void CreateControls();
    void OnCloseWindow( wxCloseEvent& event );
	void OnInfo( wxCommandEvent& event );
	void OnBrowse( wxCommandEvent& event );
	void OnPlay( wxCommandEvent& event );
	//void OnStop( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void *Entry();
	void SelectMidiOutputDevice(int number);
	void SelectMidiOutputChannel(int number);
	void OnChangeMidiDevice( wxCommandEvent& event );
	void AllNotesOff();
    void SendMidiMessage( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4, bool shortmsg = false );
private:
	wxMutex _mutex;
	wxIcon _icon;
	MidiFile* _midiFile;
	wxButton* _btnPlay;
	wxButton* _btnExit;
	wxButton* _btnBrowse;
	//wxButton* _btnStop;
	wxButton* _btnInfo;
	wxChoice* _device;
	wxTextCtrl* _txtFilename;
	wxStaticText* _txtNumEvents;
	wxStaticText* _txtNumTracks;
	wxStaticText* _txtLoadedFile;
	wxStaticText* _txtSongLength;
	wxStaticText* _txtSize;
	wxStaticText* _txtType;
	wxStaticText* _txtPPQN;
	wxStaticText* _txtBPM;
	std::list<wxPanel*> _trackPanels;
	wxSizer* _trackPanelSizer;
	bool _playing;		// Are we playing?
    int _outputChannel;
    int _midiOutputDeviceNumber;
	double _numTicksElapsed;
    RtMidiOut* _midiOutDevice;
#ifdef WIN32
	LARGE_INTEGER _currtime;
	LARGE_INTEGER _lasttime;
	LARGE_INTEGER _tickspersec;
#endif
#ifdef linux
    struct timespec _currtime;
    struct timespec _lasttime;
#endif
#ifdef __APPLE__
    clock_serv_t _clock;
    mach_timespec_t _currtime;
    mach_timespec_t _lasttime;
#endif
};

#endif
