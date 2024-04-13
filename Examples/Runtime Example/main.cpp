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
// patchLib - Runtime Example
//
// This is an example using patchLib v3 without needing a template file. This
// example demonstrates how to setup controls and use patchLib completely from
// code without embedding an xml template file.
//
// This file does not contain any actual patching code or examples!
//
// Credits to ufo-Pussy (Seek n'Destroy) for the template skin!
//******************************************************************************************

#include <Windows.h>
#include "resource.h"

// patchLib
#include "../../patchLibv3/patchLib.h"
#include "../../patchLibv3/include/CMusic.h"
#include "../../patchLibv3/include/CChild.h"
#include "../../patchLibv3/include/CWindow.h"
#include "../../patchLibv3/include/CButton.h"

POINT ptControlArray[] =
{
	// Buttons
	{ 14, 178 },	// Music
	{ 95, 178 },	// Backup
	{ 177, 178 },	// Patch
	{ 259, 178 },	// Exit

	// Labels
	{ 89, 117 },	// Application
	{ 62, 152 },	// Author
	{ 240, 152 },	// Date

	// Scrollers
	{ 362, 3 },		// Scroller

	// Urls
	{ 42, 135 },	// Url
};

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
 * DoBackupFunction
 *
 * Called when the backup button is pressed.
 */
bool __stdcall DoBackupFunction( CChild* pChild )
{
	UNREFERENCED_PARAMETER( pChild );
	return 0;
}

/**
 * DoPatchFunction
 *
 * Patches a target file.
 */
bool __stdcall DoPatchFunction( CChild* pChild )
{
	UNREFERENCED_PARAMETER( pChild );

	// Obtain Backup button.
	CButton* pButton = reinterpret_cast< CButton* >( CPatchLib::instance()->GetWindowObject()->GetChild( "btnBackup" ) );
	if( !pButton ) return 0;

	char szMessage[ 255 ] = { 0 };
	sprintf_s( szMessage, 255, "Patch status: %d", pButton->IsEnabled() );

	MessageBox( CPatchLib::instance()->GetWindowObject()->GetHwnd(), szMessage, "patchLibv3 :: Example", 0 );

	return 0;
}

/**
 * DoExitFunction
 *
 * Exits the current application when called.
 */
bool __stdcall DoExitFunction( CChild* pChild )
{
	UNREFERENCED_PARAMETER( pChild );

	SendMessage( CPatchLib::instance()->GetWindowObject()->GetHwnd(), WM_CLOSE, 0, 0 );
	return 0;
}

/**
 * WinMain
 *
 * Application entry point.
 */
int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nShowCmd );

	// Attach patchLib to application.
	boost::shared_ptr< CPatchLib > patchLib = CPatchLib::instance();
	patchLib->AttachApplication( hInstance );

	// Initialize patchLib.
	if( patchLib->Initialize( NULL, IDB_FRAME ) )
	{
		// Obtain window object.
		CWindow* pWindow = patchLib->GetWindowObject();
		if( !pWindow )
		{
			patchLib->Destroy();
			return 0;
		}

		//
		// Pre-Window Initialization
		//

		pWindow->SetClass( "pLibv3PatcherDemo" );
		pWindow->SetTitle( "patchLibv3 Patcher Demo" );
		pWindow->SetStartXPos( -1 );
		pWindow->SetStartYPos( -1 );

		patchLib->InitializeFont( IDF_FONT, "bit1", 8 );
		patchLib->GetMusicObject()->Initialize( IDR_TUNE );

		pWindow->AddButton( "btnMusic", ptControlArray[0], "$Music", 2, IDB_MUSIC );
		pWindow->AddButton( "btnBackup", ptControlArray[1], "$Backup", 2, IDB_BACKUP );
		pWindow->AddButton( "btnPatch", ptControlArray[2], "$Patch", 3, IDB_PATCH );
		pWindow->AddButton( "btnExit", ptControlArray[3], "$Exit", 3, IDB_EXIT );

		pWindow->AddLabel( "lblApplication", "Dummy Application", ptControlArray[4], RGB( 0, 0, 0 ) );
		pWindow->AddLabel( "lblAuthor", "atom0s", ptControlArray[5], RGB( 0, 0, 0 ) );
		pWindow->AddLabel( "lblDate", "00.00.0000", ptControlArray[6], RGB( 0, 0, 0 ) );

		pWindow->AddScroller( "scrMain", "This is some text to be scrolled...", ptControlArray[7], RGB( 1, 0, 0 ), 336, 0.0f, 0.0f, 0 );

		pWindow->AddUrl( "urlMain", "www.dummy-site.com", "http://www.dummy-site.com/test_no_follow_link.html", ptControlArray[8], RGB( 0, 0, 0 ), RGB( 255, 255, 255 ) );

		pWindow->RegisterCommand( "$Music", DoMusicFunction );
		pWindow->RegisterCommand( "$Backup", DoBackupFunction );
		pWindow->RegisterCommand( "$Patch", DoPatchFunction );
		pWindow->RegisterCommand( "$Exit", DoExitFunction );

		// Initialize window object.
		patchLib->InitializeWindow();

		//
		// Post-Window Initialization
		// 

		pWindow->SetTopMost( true );
		pWindow->SetCursor( IDC_CURSOR );
		pWindow->SetIcon( IDI_ICON );

		// Ready.. set.. go!
		patchLib->Start();
	}

	patchLib->Destroy();

	return 0;
}
