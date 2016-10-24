; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{CAECD8FC-317B-4DD3-BD61-BD4C89FD3E8B}
AppName=MidiPlayer
AppVerName=MIDIPlayer 2.0
AppPublisher=Zeta Centauri
AppPublisherURL=http://zetacentauri.com
AppSupportURL=http://zetacentauri.com
AppUpdatesURL=http://zetacentauri.com
DefaultDirName={pf}\Zeta Centauri\MidiPlayer
DefaultGroupName=Zeta Centauri\MidiPlayer
LicenseFile=C:\Users\Xangis\code\MidiPlayer\installer\License.txt
OutputDir=C:\Users\Xangis\code\MidiPlayer\Installer
OutputBaseFilename=MIDIPlayer2Setup
SetupIconFile=C:\Users\Xangis\code\MidiPlayer\keyicon.ico
UninstallDisplayIcon={app}\keyicon.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Xangis\code\MidiPlayer\Release\MidiPlayer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Xangis\code\MidiPlayer\keyicon.ico"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "C:\Users\Xangis\code\MidiPlayer\installer\vcredist2010_x86.exe"; DestDir: "{app}"; Flags: ignoreversion deleteafterinstall

[Icons]
Name: "{group}\MidiPlayer"; Filename: "{app}\MidiPlayer.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:ProgramOnTheWeb,MidiPlayer}"; Filename: "http://zetacentauri.com/software_midiplayer.htm"
Name: "{commondesktop}\MidiPlayer"; Filename: "{app}\MidiPlayer.exe"; Tasks: desktopicon; WorkingDir: "{app}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\MidiPlayer"; Filename: "{app}\MidiPlayer.exe"; Tasks: quicklaunchicon; WorkingDir: "{app}"

[Registry]
Root: HKCR; Subkey: ".mid"; ValueType: string; ValueName: ""; ValueData: "MidiPlayerMIDIFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "MidiPlayerMIDIFile"; ValueType: string; ValueName: ""; ValueData: "MIDI File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "MidiPlayerMIDIFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\MidiPlayer.exe"" ""%1"""

[Run]
Filename: "{app}\vcredist2010_x86.exe"; Parameters: "/q"; WorkingDir: "{app}";  StatusMsg: "Installing Visual C++ 2010 Redistributable..."; Flags: waituntilterminated
Filename: "{app}\MidiPlayer.exe"; Description: "{cm:LaunchProgram,MidiPlayer}"; Flags: nowait postinstall skipifsilent

