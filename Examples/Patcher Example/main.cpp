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
// patchLib - Patcher Example
//
// This is an example using patchLib v3 as a patcher application. This sample
// simply demonstrates how to setup an application using patchLib that would
// be used for patching another file.
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

	// Do backup adjustments here..

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
	CButton* pButton = reinterpret_cast< CButton* >( CPatchLib::instance()->GetWindowObject()->GetChild( "Backup" ) );
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

	boost::shared_ptr< CPatchLib > patchLib = CPatchLib::instance();

	// Attach to this application instance.
	patchLib->AttachApplication( hInstance );

	// Initialize patchLib.
	if( patchLib->Initialize( IDR_TEMPLATE, NULL ) )
	{
		// Register command callbacks.
		patchLib->GetWindowObject()->RegisterCommand( "$Music", DoMusicFunction );
		patchLib->GetWindowObject()->RegisterCommand( "$Backup", DoBackupFunction );
		patchLib->GetWindowObject()->RegisterCommand( "$Patch", DoPatchFunction );
		patchLib->GetWindowObject()->RegisterCommand( "$Exit", DoExitFunction );
		
		// Initialize window.
		if( patchLib->InitializeWindow() )
		{
			// Ready.. set.. go!
			patchLib->Start();
		}
	}

	// Cleanup and return.
	patchLib->Destroy();
	return 0;
}