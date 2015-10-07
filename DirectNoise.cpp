// DirectNoise.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Windows.h"
#include "Commdlg.h"
#include "stdlib.h"
#include "stdio.h"
#include "DirectNoise.h"
#define INITGUID
#include <dmusici.h>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MidiPlayer( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL done = FALSE;

IDirectMusicLoader8*		g_pLoader = NULL;
IDirectMusicPerformance8*	g_pPerformance = NULL;
IDirectMusicSegment8*		g_pSegment = NULL;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Dmusic and COM setup
	CoInitialize( NULL );

	CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC,
						IID_IDirectMusicLoader8, (void **)&g_pLoader );
	CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC,
						IID_IDirectMusicPerformance8, (void **)&g_pPerformance );
	// End Dmusic and COM setup

	// Perform application initialization:
	//if (!InitInstance (hInstance, nCmdShow)) 
	//{
	//	return FALSE;
	//}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DIRECTNOISE);

	// init audio
	g_pPerformance->InitAudio( 
	NULL,                  // IDirectMusic interface not needed.
	NULL,                  // IDirectSound interface not needed.
	NULL,                  // Window handle.
	DMUS_APATH_SHARED_STEREOPLUSREVERB,  // Default audiopath type.
	64,                    // Number of performance channels.
	DMUS_AUDIOF_ALL,       // Features on synthesizer.
	NULL                   // Audio parameters; use defaults.
	);
	// end init audio

	// Set the search directory.
	g_pLoader->SetSearchDirectory( 
		GUID_DirectMusicAllTypes,   // Types of files sought.
		L".", // Where to look. Null for default
		FALSE                       // Don't clear object data.
	); // This function has a lot of stuff I did to it, explained below

	HWND hWnd = CreateDialog(hInst, (LPCTSTR)IDD_MIDIPLAYER, NULL, (DLGPROC)MidiPlayer);
	ShowWindow( hWnd, nCmdShow );

	// Main message loop:
	while (!done && GetMessage(&msg, NULL, 0, 0)) 
	{
		if ( hWnd == 0 || !IsDialogMessage( hWnd, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	g_pPerformance->Stop(
	NULL,   // Stop all segments.
	NULL,   // Stop all segment states.
	0,      // Do it immediately.
	0       // Flags.
	);

	g_pPerformance->CloseDown();

	if( g_pLoader) g_pLoader->Release();
	if( g_pPerformance ) g_pPerformance->Release();
	if( g_pSegment) g_pSegment->Release();

	CoUninitialize();


	return 0;
}



//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK MidiPlayer( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if( LOWORD(wParam) == IDEXIT )
		{
			DestroyWindow( hDlg );
			done = true;
			return TRUE;
		}
		if( LOWORD(wParam) == IDC_STOP )
		{
			g_pPerformance->Stop( g_pSegment, NULL, 0, DMUS_SEGF_BEAT );			
			return TRUE;
		}
		else if(LOWORD( wParam) == IDC_PLAYMIDI )
		{
			int cvt;
			HRESULT error;
			char errorstring[256];
			char strFileName[MAX_PATH];
			WCHAR wstrFileName[MAX_PATH];
			HWND TextCtrl = GetDlgItem( hDlg, IDC_FILENAME );
			GetWindowText( TextCtrl, strFileName, MAX_PATH );

			// Convert our filename to WCHAR to make LoadObjectFromFile happy.
			cvt = mbstowcs( wstrFileName, strFileName, MAX_PATH );
			// Please ignore the failed attempts to get this right.
			//int cvt = mbtowc( wstrFileName, strFileName, MAX_PATH );
			//int cvt = mbtowc( wstrFileName, "F:\\Documents and Settings\\Xangis\\My Documents\\Visual Studio Projects\\MIDIPlayer\\piggy.mid", MAX_PATH );
			//cvt = swprintf( wstrFileName, L"%s", strFileName );

			if (error = (g_pLoader->LoadObjectFromFile(
				CLSID_DirectMusicSegment,   // Class identifier.
				IID_IDirectMusicSegment8,   // ID of desired interface.
				//L"F:\\Documents and Settings\\Xangis\\My Documents\\Visual Studio Projects\\MIDIPlayer\\piggy.mid",  // Filename
				wstrFileName,               // Filename.
				(LPVOID*) &g_pSegment)))    // Pointer that receives interface.
			{
				switch( error )
				{
				case E_FAIL:
					sprintf( errorstring, "MIDI file could not be loaded: E_FAIL" );
					break;
				case E_INVALIDARG:
					sprintf( errorstring, "MIDI file could not be loaded: E_INVALIDARG" );
					break;
				case E_OUTOFMEMORY:
					sprintf( errorstring, "MIDI file could not be loaded: E_OUTOFMEMORY" );
					break;
				case E_POINTER:
					sprintf( errorstring, "MIDI file could not be loaded: E_POINTER" );
					break;
				case REGDB_E_CLASSNOTREG:
					sprintf( errorstring, "MIDI file could not be loaded: REGDB_E_CLASSNOTREG" );
					break;
				case DMUS_E_LOADER_NOCLASSID:
					sprintf( errorstring, "MIDI file could not be loaded: DMUS_E_LOADER_NOCLASSID" );
					break;
				case DMUS_E_LOADER_FAILEDOPEN:
					sprintf( errorstring, "MIDI file could not be loaded: DMUS_E_LOADER_FAILEDOPEN (%S)", wstrFileName );
					break;
				case DMUS_E_LOADER_FORMATNOTSUPPORTED:
					sprintf( errorstring, "MIDI file could not be loaded: DMUS_E_LOADER_FORMATNOTSUPPORTED" );
					break;
				case DMUS_E_LOADER_FAILEDCREATE:
					sprintf( errorstring, "MIDI file could not be loaded: DMUS_E_LOADER_FAILEDCREATE" );
					break;
				default:
					sprintf( errorstring, "MIDI file could not be loaded: Unknown Error" );
					break;
				}
				MessageBox( hDlg, errorstring, strFileName, MB_OK );
				return 0;
			}

			g_pSegment->Download( g_pPerformance );

			g_pPerformance->PlaySegmentEx(
				g_pSegment,  // Segment to play.
				NULL,        // Used for songs; not implemented.
				NULL,        // For transitions. 
				0,           // Flags.
				0,           // Start time; 0 is immediate.
				NULL,        // Pointer that receives segment state.
				NULL,        // Object to stop.
				NULL         // Audiopath, if not default.
			);
		}
		else if( LOWORD( wParam) == IDC_BROWSE )
		{
			OPENFILENAME file;
			char szFile[MAX_PATH];

			ZeroMemory( &file, sizeof(file));
			file.lStructSize = sizeof(file);
			file.hwndOwner = hDlg;
			file.lpstrFile = szFile;
			file.lpstrFile[0] = '\0';
			file.nMaxFile = sizeof( szFile );
			file.lpstrFilter = "MIDI Files\0*.MID\0";
			file.nFilterIndex = 1;
			file.lpstrFileTitle = NULL;
			file.nMaxFileTitle = 0;
			file.lpstrInitialDir = NULL;
			file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
					
			if( GetOpenFileName( &file ))
			{
				HWND TextCtrl = GetDlgItem( hDlg, IDC_FILENAME );
				SetWindowText( TextCtrl, file.lpstrFile );
			}
			else if( CommDlgExtendedError() )
			{
				return TRUE;
			}

		}
		return TRUE;
		break;
	}

	return FALSE;
}
