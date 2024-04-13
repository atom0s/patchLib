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

#ifndef __PATCHLIB_CURL_H_INCLUDED__
#define __PATCHLIB_CURL_H_INCLUDED__

#include <Windows.h>
#include "CChild.h"

/// <summary>
/// CUrl
///
/// Url object class. Inherits CChild base class.
/// </summary>
class CUrl : public CChild
{
	std::string		m_vUrlText;
	std::string		m_vUrlSite;
	COLORREF		m_vUrlColor;
	COLORREF		m_vUrlColorOver;
	HBITMAP			m_vUrlBitmap;
	HDC				m_vUrlDc;

public: // Construction
	CUrl( void );
	~CUrl( void );

	// Custom control initialization.
	bool		Initialize( const std::string& strAlias, const std::string& strText, const std::string& strSite, const POINT& ptPoint, const COLORREF rgb, const COLORREF rgbOver );

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
	void		SetSite( const std::string& strSite );
	void		SetColor( const COLORREF rgb );
	void		SetColorOver( const COLORREF rgb );

	std::string GetText( void ) const;
	std::string GetSite( void ) const;
	COLORREF	GetColor( void ) const;
	COLORREF	GetColorOver( void ) const;
};

#endif // __PATCHLIB_CURL_H_INCLUDED__