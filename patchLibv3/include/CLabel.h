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

#ifndef __PATCHLIB_CLABEL_H_INCLUDED__
#define __PATCHLIB_CLABEL_H_INCLUDED__

#include <Windows.h>
#include "CChild.h"

/// <summary>
/// CLabel
///
/// Label object class. Inherits CChild base class.
/// </summary>
class CLabel : public CChild
{
	std::string m_vLabelText;
	COLORREF	m_vLabelColor;
	HBITMAP		m_vLabelBitmap;
	HDC			m_vLabelDc;

public: // Construction
	CLabel( void );
	~CLabel( void );

	// Custom control initialization.
	bool		Initialize( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb );
	
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
	void		SetText( const std::string& strText );	
	void		SetColor( const COLORREF rgb );
	
	std::string GetText( void ) const;
	COLORREF	GetColor( void ) const;
};

#endif // __PATCHLIB_CLABEL_H_INCLUDED__