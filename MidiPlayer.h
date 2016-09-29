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
#define ID_MIDI_DEVICE 10011

/*!
 * Compatibility
 */
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * wxKeyboard class declaration
 */
class MidiPlayer: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( MidiPlayer )
    DECLARE_EVENT_TABLE()
public:
    /// Constructors
    MidiPlayer( );
    MidiPlayer( wxWindow* parent, wxWindowID id = SYMBOL_WXKEYBOARD_IDNAME, const wxString& caption = SYMBOL_WXKEYBOARD_TITLE, const wxPoint& pos = SYMBOL_WXKEYBOARD_POSITION, const wxSize& size = SYMBOL_WXKEYBOARD_SIZE, long style = SYMBOL_WXKEYBOARD_STYLE );
	virtual ~MidiPlayer() { };
    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXKEYBOARD_IDNAME, const wxString& caption = SYMBOL_WXKEYBOARD_TITLE, const wxPoint& pos = SYMBOL_WXKEYBOARD_POSITION, const wxSize& size = SYMBOL_WXKEYBOARD_SIZE, long style = SYMBOL_WXKEYBOARD_STYLE );
    void CreateControls();
    void OnCloseWindow( wxCloseEvent& event );
	void OnInfo( wxCommandEvent& event );
	void OnBrowse( wxCommandEvent& event );
	void OnPlay( wxCommandEvent& event );
	void OnStop( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
private:
	wxIcon _icon;
	MidiFile* _midiFile;
    RtMidiOut* _midiOutDevice;
	wxButton* _btnPlay;
	wxButton* _btnExit;
	wxButton* _btnBrowse;
	wxButton* _btnStop;
	wxButton* _btnInfo;
	wxTextCtrl* _txtFilename;
};

#endif