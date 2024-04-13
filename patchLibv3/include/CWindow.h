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

#ifndef __PATCHLIB_CWINDOW_H_INCLUDED__
#define __PATCHLIB_CWINDOW_H_INCLUDED__

#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

class CChild;
class TiXmlElement;

/// <summary>
/// CWindow
///
/// Window object class. Contains the main window information and
/// handles drawing controls and other objects.
/// </summary>
class CWindow
{
	WNDCLASSEX		m_vWndClassEx;
	std::string		m_vClass;
	std::string		m_vTitle;
	int				m_vWidth;
	int				m_vHeight;
	int				m_vXPosition;
	int				m_vYPosition;
	int				m_vStartXPos;
	int				m_vStartYPos;
	bool			m_vTopMost;
	HWND			m_vHwnd;

	/**
	 * Cursor variables.
	 */
	int			m_vCursorResId;
	HANDLE		m_vCursor;

	/**
	 * Icon variables.
	 */
	int			m_vIconResId;
	HANDLE		m_vIcon;

	/**
	 * Child objects.
	 */
	std::vector< boost::shared_ptr< CChild > > m_vChildren;

	/**
	 * Window procedure overrides.
	 */
	std::map< UINT, WMCALLBACKSTRUCT > m_vWndCallbacks;

	/**
	 * Command button callbacks.
	 */
	std::map< std::string, CMDCALLBACK > m_vCmdCallbacks;

#ifdef PATCHLIB_USE_GDIPLUS
	/**
	 * GdiPlus variables.
	 */
	ULONG_PTR			m_vGdiplusToken;
	GdiplusStartupInput m_vGdiplusInput;
	Graphics*			m_vGdiGraphics;
	Image*				m_vGdiFrame; // Frame image copy.
#endif

public: // Construction
	CWindow( void );
	~CWindow( void );

public: // Initialization
	bool		Initialize( HINSTANCE hInstance );
	bool		Destroy( HINSTANCE hInstance );
	bool		LoadControls( TiXmlElement* pElement );

public: // Drawing
	bool		Draw( void );
	bool		Update( void );

public: // Message handling.
	bool		MessagePump( void );

	LRESULT __stdcall WindowProc( HWND, UINT, WPARAM, LPARAM );
	static LRESULT __stdcall staticWndProc( HWND, UINT, WPARAM, LPARAM );

public: // Controls.
	CChild*		GetChild( const std::string& strAlias );
	bool		RemoveChild( const std::string& strAlias );

	bool		AddLabel( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb );
	bool		AddUrl( const std::string& strAlias, const std::string& strText, const std::string& strSite, const POINT& ptPoint, const COLORREF rgb, const COLORREF rgbOver );
	bool		AddScroller( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb, int nWidth, float fFreq, float fSpeed, int nAmp );
	bool		AddButton( const std::string& strAlias, const POINT& ptPoint, const std::string& strCommand, int nTotalStates, int nButtonResId );

public: // Window procedure overrides.
	bool		SetMessageCallback( UINT uMsg, WMCALLBACKNORETURN pCallback );
	bool		SetMessageCallback( UINT uMsg, WMCALLBACKRETURN pCallback );
	bool		RemoveMessageCallback( UINT uMsg );

public: // Command button callbacks.
	bool		RegisterCommand( const std::string& strCommand, CMDFUNCTION pFunction );
	bool		UnregisterCommand( const std::string& strCommand );
	bool		CallCommand( const std::string& strCommand, CChild* pChild );
	CMDFUNCTION GetCommandFunction( const std::string& strCommand );

public: // Window handle.
	HWND		GetHwnd( void ) const;

public: // Cursor and icon.
	bool		SetCursor( int nCursorResId );
	bool		SetIcon( int nIconResId );

public: // Properties
	void		SetClass( const std::string& strClass );
	void		SetTitle( const std::string& strTitle );
	void		SetWidth( int nWidth );
	void		SetHeight( int nHeight );
	void		SetXPosition( int nXPos );
	void		SetYPosition( int nYPos );
	void		SetStartXPos( int nXPos );
	void		SetStartYPos( int nYPos );
	void		SetTopMost( bool bTopMost );

	std::string GetClass( void ) const;
	std::string GetTitle( void ) const;
	int			GetWidth( void ) const;
	int			GetHeight( void ) const;
	int			GetXPosition( void ) const;
	int			GetYPosition( void ) const;
	int			GetStartXPos( void ) const;
	int			GetStartYPos( void ) const;
	bool		GetTopMost( void ) const;
};


#endif // __PATCHLIB_CWINDOW_H_INCLUDED__