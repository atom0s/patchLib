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

#ifndef __PATCHLIB_CBUTTON_H_INCLUDED__
#define __PATCHLIB_CBUTTON_H_INCLUDED__

#include <Windows.h>
#include "CChild.h"

/// <summary>
/// CButton
///
/// Button object class. Inherits CChild base class.
/// </summary>
class CButton : public CChild
{
	int			m_vButtonResId;
	std::string m_vButtonCallback;

	HBITMAP		m_vButtonBitmap;
	HBITMAP		m_vButtonDib;
	HDC			m_vButtonDc;

	int			m_vButtonStateCount;
	int			m_vButtonState;
	bool		m_vButtonEnabled;
	bool		m_vIsMouseDown;
	bool		m_vIsMouseOver;

	CMDFUNCTION m_vButtonFunction; // Callback function.

public: // Construction
	CButton( void );
	~CButton( void );

	// Custom control initialization.
	bool		Initialize( const std::string& strAlias, const POINT& ptPoint, const std::string& strCommand, int nTotalStates, int nButtonResId );

public: // IChild
	bool		Initialize( TiXmlElement* pElement );
	bool		Setup( void );
	bool		Draw( void );
	bool		Update( void );
	bool		Release( void );

public: // IMouse
	bool		OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam );
	bool		OnMouseDown( HWND hWnd, WPARAM wParam, LPARAM lParam );
	bool		OnMouseUp( HWND hWnd, WPARAM wParam, LPARAM lParam );

public: // Properties
	void		SetResId( int nResId );
	void		SetCallback( const std::string& strCommand );
	void		SetHeight( int nHeight );
	void		SetStateCount( int nStateCount );
	void		SetState( int nState );
	void		SetEnabled( bool bEnabled );
	void		SetMouseDown( bool bMouseDown );
	void		SetMouseOver( bool bMouseOver );

	int			GetResId( void ) const;
	std::string GetCallback( void ) const;
	int			GetHeight( void ) const;
	int			GetStateCount( void ) const;
	int			GetState( void ) const;
	bool		IsEnabled( void ) const;
	bool		IsMouseDown( void ) const;
	bool		IsMouseOver( void ) const;
};

#endif // __PATCHLIB_CBUTTON_H_INCLUDED__