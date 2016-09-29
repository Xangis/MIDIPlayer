#ifndef _MIDIPLAYERAPP_H_
#define _MIDIPLAYERAPP_H_

/*!
 * Includes
 */
#include "wx/image.h"

/*!
 * wxKeyboardApp class declaration
 */
class MidiPlayerApp: public wxApp
{
    DECLARE_CLASS( MidiPlayerApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    MidiPlayerApp();
    ~MidiPlayerApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();
};

/*!
 * Application instance declaration 
 */
DECLARE_APP(MidiPlayerApp)

#endif
    // _WXKEYBOARDAPP_H_
