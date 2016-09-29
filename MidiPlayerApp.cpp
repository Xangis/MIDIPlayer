#include "wx/wx.h"
#include "MidiPlayerApp.h"
#include "MidiPlayer.h"

/*!
 * Application instance implementation
 */

IMPLEMENT_APP( MidiPlayerApp )

/*!
 * wxKeyboardApp type definition
 */

IMPLEMENT_CLASS( MidiPlayerApp, wxApp )

/*!
 * wxKeyboardApp event table definition
 */

BEGIN_EVENT_TABLE( MidiPlayerApp, wxApp )
END_EVENT_TABLE()

/*!
 * Constructor for wxKeyboardApp
 */
MidiPlayerApp::MidiPlayerApp()
{
}

MidiPlayerApp::~MidiPlayerApp()
{
}

/*!
 * Initialisation for wxKeyboardApp
 */
bool MidiPlayerApp::OnInit()
{
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.
    wxInitAllImageHandlers();
    MidiPlayer* mainWindow = new MidiPlayer(NULL, ID_KEYBOARD_DLG );
    mainWindow->Show(true);

    return true;
}

/*!
 * Cleanup for wxKeyboardApp
 */
int MidiPlayerApp::OnExit()
{
    return wxApp::OnExit();
}

