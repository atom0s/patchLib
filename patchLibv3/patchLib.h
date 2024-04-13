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

#pragma once

#ifndef __PATCHLIB_H_INCLUDED__
#define __PATCHLIB_H_INCLUDED__

#ifdef UNICODE
#error "patchLib does not currently support Unicode! Please change your character set!"
#endif

#ifndef STRICT
#define STRICT
#endif

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN // This stops 'interface' from working.
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // WinXP minimum.
#endif

/*******************************************************************************************
 *
 * patchLib v3 Compile Configuration
 *
 *******************************************************************************************/

/////////////////////////////////////////////////////////////////////////////////////////////
//
// PATCHLIB_USE_GDIPLUS
// 
//		Required to be enabled if you wish to allow patchLib
//		to draw Win32 controls. Win32 controls require more
//		effort to draw properly onto a layered window which
//		GdiPlus makes this much easier to handle.
//
//		GdiPlus must be installed on the users machine in order
//		to use any patchLib file compiled with this feature!
//
//		Uncomment to enable.
// 
/////////////////////////////////////////////////////////////////////////////////////////////
#define PATCHLIB_USE_GDIPLUS

/////////////////////////////////////////////////////////////////////////////////////////////
//
// PATCHLIB_USE_COMMCTRL
//
//		Determines if patchLib should automatically make use
//		of the Common Control library. This is needed for certain
//		Win32 controls to properly draw.
//
//		This option is only usable while Gdiplus is enabled!
// 
/////////////////////////////////////////////////////////////////////////////////////////////
#define PATCHLIB_USE_COMMCTRL

/////////////////////////////////////////////////////////////////////////////////////////////
//
// PATCHLIB_COMMCTRL_FLAGS
//
//		Flags passed to InitCommonControlsEx API when initializing
//		the common control library. Should contain proper flags for
//		objects you wish to draw. For a list of values, see:
// 
//		http://msdn.microsoft.com/en-us/library/bb775507%28VS.85%29.aspx
//		
//		This option is only usable while Gdiplus is enabled.
//		This option is only usable while common controls are being used.
// 
/////////////////////////////////////////////////////////////////////////////////////////////
#define PATCHLIB_COMMCTRL_FLAGS ICC_STANDARD_CLASSES

/*******************************************************************************************
 * END CONFIGURATIONS
 *******************************************************************************************/

#include <Windows.h>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

#ifdef PATCHLIB_USE_GDIPLUS
	#ifdef PATCHLIB_USE_COMMCTRL
		// Common Controls
		#pragma comment( lib, "Comctl32.lib" )
		#include <CommCtrl.h>
	#endif

	// Gdiplus
	#pragma comment( lib, "GdiPlus.lib" )
	#include <GdiPlus.h>
	using namespace Gdiplus;
#endif

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

class CChild;
class CMusic;
class CWindow;

/**
 * Safety-check Definitions
 */
#define SAFE_DELETE( p )		if( p ) { delete p; p = NULL; }
#define SAFE_DELETEARRAY( p )	if( p ) { delete[] p; p = NULL; }
#define SAFE_DELETEOBJ( p )		if( p ) { DeleteObject( p ); p = NULL; }
#define SAFE_DELETEDC( p )		if( p ) { DeleteDC( p ); p = NULL; }
#define SAFE_RELEASEDC( w, p )	if( p ) { ReleaseDC( w, p ); p = NULL; }

/**
 * Window Procedure Callbacks
 */
typedef void	( __stdcall* WMCALLBACKNORETURN )	( HWND, WPARAM, LPARAM );
typedef LRESULT ( __stdcall* WMCALLBACKRETURN )		( HWND, WPARAM, LPARAM );

typedef struct _WMCALLBACKSTRUCT {
	WMCALLBACKNORETURN	lpCallbackNoReturn;
	WMCALLBACKRETURN	lpCallbackReturn;
	int					Flags;
} WMCALLBACKSTRUCT, *LPWMCALLBACKSTRUCT;


/**
 * Button Command Callbacks
 */
typedef boost::function1< bool, CChild* > CMDFUNCTION;

typedef struct _CMDCALLBACK {
	std::string		Command;
	CMDFUNCTION		Function;
} CMDCALLBACK, *LPCMDCALLBACK;

/**
 *
 * patchLib Engine
 * 
 */
class CPatchLib
{
	/**
	 * Singleton instance.
	 */
	static boost::shared_ptr< CPatchLib > m_vInstance;

	/**
	 * Window object.
	 */
	boost::shared_ptr< CWindow > m_vWindow;

	/**
	 * Application instance.
	 */
	HINSTANCE		m_vAppInstance;

	/**
	 * Drawing variables.
	 */
	BLENDFUNCTION	m_vBlendFunction;
	BITMAPINFO		m_vBitmapInfo;
	HDC				m_vDcMemory;
	HDC				m_vDcScreen;
	HBITMAP			m_vDibMain;
	LPVOID			m_vBlankMem;

	/**
	 * Frame variables.
	 */
	int				m_vFrameResId;
	HBITMAP			m_vFrameBmp;
	HBITMAP			m_vFrameDib;
	POINT			m_vFramePoint;
	SIZE			m_vFrameSize;

	/**
	 * Font variables.
	 */
	int				m_vFontResId;
	int				m_vFontHeight;
	std::string		m_vFontName;
	HFONT			m_vFont;
	LOGFONT			m_vFontObject;

	/**
	 * Music variables
	 */
	boost::shared_ptr< CMusic >	m_vMusicObject;

public: // Construction
	CPatchLib( void );
	~CPatchLib( void );

public: // Singleton instance.
	static boost::shared_ptr< CPatchLib > instance( void );

public: // Creation and destruction.
	bool		AttachApplication( HINSTANCE hInstance );
	bool		Initialize( int nTemplateResId = 0, int nFrameResId = 0 );
	bool		InitializeWindow( void );
	bool		InitializeFont( int nFontResId, const std::string& strFontName, int nFontHeight );
	bool		Destroy( void );

private: // Template initialization.
	bool		InitializeTemplate( int nTemplateResId );

public: // Drawing.
	bool		Start( void );
	bool		Draw( void );
	bool		FadeWindowIn( void );
	bool		FadeWindowOut( void );

public: // Variable exposure.

	// Window object.
	CWindow*	GetWindowObject( void ) const;

	// Music object.
	CMusic*		GetMusicObject( void ) const;

	// Drawing objects.
	void		GetBlendFunction( BLENDFUNCTION& blendFunc ) const;
	void		GetBitmapInfo( BITMAPINFO& bmpInfo ) const;
	HDC			GetMemoryDc( void ) const;
	HDC			GetScreenDc( void ) const;
	HBITMAP		GetMainDib( void ) const;

	// Frame objects.
	int			GetFrameResId( void ) const;
	HBITMAP		GetFrameBmp( void ) const;
	HBITMAP		GetFrameDib( void ) const;
	void		GetFramePoint( POINT& lpPoint ) const;
	void		GetFrameSize( SIZE& lpSize ) const;

	// Font objects.
	int			GetFontResId( void ) const;
	int			GetFontHeight( void ) const;
	std::string GetFontName( void ) const;
	HFONT		GetFont( void ) const;
	void		GetFontInfo( LOGFONT& logfont ) const;

};

#endif // __PATCHLIB_H_INCLUDED__