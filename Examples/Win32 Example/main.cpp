/**
 *
 * This file is part of patchLib.
 * 
 * patchLib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * patchLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with patchLib.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *******************************************************************************************
 */

//******************************************************************************************
// patchLib - Win32 Example
//
// This is an example using patchLib v3 with Win32 controls.
// This file does not contain any actual patching code or examples!
//******************************************************************************************

#include <Windows.h>
#include "resource.h"

#include "../../patchLibv3/patchLib.h"
#include "../../patchLibv3/include/CMusic.h"
#include "../../patchLibv3/include/CWindow.h"
#include "../../patchLibv3/include/CScroller.h"

// Scroller Message
std::string g_vScrollerMessage = \
	"This is an example Keygen application template! Enter a name and a serial will auto-gen as you type!";

// Editbox Handles
HWND g_vNameHwnd	= NULL;
HWND g_vSerialHwnd	= NULL;

// Original Window Proc
WNDPROC g_vOrigWndProc = NULL;


/**
 * DoMusicFunction
 *
 * Toggles the music for the application.
 */
bool __stdcall DoMusicFunction( CChild* pChild )
{
	UNREFERENCED_PARAMETER( pChild );

	CPatchLib::instance()->GetMusicObject()->Pause();
	return 0;
}

/**
 * DoCopyFunction
 *
 * Called when the copy button is pressed.
 */
bool __stdcall DoCopyFunction( CChild* pChild )
{
	UNREFERENCED_PARAMETER( pChild );

	// Open clipboard and empty it.
	if( !OpenClipboard( CPatchLib::instance()->GetWindowObject()->GetHwnd() ) )
		return 0;
	EmptyClipboard();

	// Get serial text length.
	int nLength = GetWindowTextLength( g_vSerialHwnd );
	if( nLength == 0 ) return 0;

	// Create handle for clipboard copy.
	HGLOBAL hGlobal = GlobalAlloc( GMEM_MOVEABLE, ( nLength + 1 ) * sizeof( TCHAR ) );
	if( !hGlobal )
	{
		CloseClipboard();
		return 0;
	}

	// Get serial text.
	char* pszSerial = new char[ nLength + 1 ];
	GetWindowText( g_vSerialHwnd, pszSerial, nLength + 1 );

	// Copy to global memory.
	LPVOID lpLockPtr = GlobalLock( hGlobal );
	memcpy( lpLockPtr, pszSerial, nLength * sizeof( TCHAR ) );
	GlobalUnlock( hGlobal );

	// Set clipboard data.
	SetClipboardData( CF_TEXT, hGlobal );

	// Cleanup..
	CloseClipboard();
	delete pszSerial;
	
	return 0;
}

/**
 * DoExitFunction
 *
 * Called when the exit button is pressed.
 */
bool __stdcall DoExitFunction( CChild* pChild )
{
	UNREFERENCED_PARAMETER( pChild );

	SendMessage( CPatchLib::instance()->GetWindowObject()->GetHwnd(), WM_CLOSE, 0, 0 );
	return 0;
}

/**
 *
 * Serial Generation Example
 * 
 * The serials produced by this generation example are not
 * valid for any software. This is simply for show.
 *
 */
bool DoSerial( void )
{
	// Get name text size.
	int nNameLength = GetWindowTextLength( g_vNameHwnd );

	// Name must be at least 6 chars long.
	if( nNameLength < 6 )
	{
		SetWindowText( g_vSerialHwnd, "Name must be at least 6 chars long!" );
		return false;
	}

	// Generate (fake) serial code.
	char szAlphaNumeric[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	std::string strSerial = "AAAA";

	for( int x = 0; x < nNameLength; x++ )
	{
		int nRandPos = rand() % ( sizeof( szAlphaNumeric ) - 1 );
		strSerial += szAlphaNumeric[ nRandPos ];
	}

	SetWindowText( g_vSerialHwnd, strSerial.c_str() );

	return true;
}

/**
 *
 * Win32 Subclass Example (Name box window proc.)
 *
 * Handles auto-generation of serials when the edit control
 * has text written to it.
 *
 */
LRESULT CALLBACK NameEditCallback( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Pass messages through and listen for WM_CHAR..
	LRESULT lResult = CallWindowProc( g_vOrigWndProc, hWnd, uMsg, wParam, lParam );

	// Generate serial from new name.
	if( uMsg == WM_CHAR )
		DoSerial();

	return lResult;
}


/**
 *
 * WinMain - Entry point..
 *
 */
int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nShowCmd );

	boost::shared_ptr< CPatchLib > patchLib = CPatchLib::instance();

	// Attach application instance.
	patchLib->AttachApplication( hInstance );

	// Initialize patchLib.
	if( patchLib->Initialize( IDR_TEMPLATE, NULL ) )
	{
		// Set our scroller message.
		( ( CScroller*) patchLib->GetWindowObject()->GetChild( "scrollTest1" ) )->SetText( g_vScrollerMessage );

		// Register command callbacks.
		patchLib->GetWindowObject()->RegisterCommand( "$Music", DoMusicFunction );
		patchLib->GetWindowObject()->RegisterCommand( "$Copy", DoCopyFunction );
		patchLib->GetWindowObject()->RegisterCommand( "$Exit", DoExitFunction );

		// Initialize window.
		if( patchLib->InitializeWindow() )
		{
			// Create text objects.
			g_vNameHwnd		= CreateWindowEx( NULL, "EDIT", "", WS_CHILD|WS_VISIBLE|WS_BORDER, 140, 125, 400, 20, patchLib->GetWindowObject()->GetHwnd(), (HMENU)9200, hInstance, NULL );
			g_vSerialHwnd	= CreateWindowEx( NULL, "EDIT", "", WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY, 140, 150, 400, 20, patchLib->GetWindowObject()->GetHwnd(), (HMENU)9201, hInstance, NULL );

			// Subclass name edit box.
			g_vOrigWndProc = reinterpret_cast< WNDPROC >( SetWindowLongPtr( g_vNameHwnd, GWL_WNDPROC, reinterpret_cast< LONG_PTR >( NameEditCallback ) ) );

			// Set default name and generate key.
			SetWindowText( g_vNameHwnd, "atom0s" );
			DoSerial();

			// Ready.. set.. go!
			patchLib->Start();
		}
	}
	patchLib->Destroy();

	return 0;
}