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

#ifndef __PATCHLIB_CSCROLLER_H_INCLUDED__
#define __PATCHLIB_CSCROLLER_H_INCLUDED__

#include <Windows.h>
#include "CChild.h"

/// <summary>
/// CScroller
///
/// Scroller object class. Inherits CChild base class.
/// </summary>
class CScroller : public CChild
{
	std::string m_vScrollerText;
	COLORREF	m_vScrollerColor;
	BITMAPINFO	m_vScrollerBitmapInfo;
	HBITMAP		m_vScrollerBitmap;
	HDC			m_vScrollerDc;
	int			m_vScrollerWidth;
	int			m_vScrollerPosition;

	SIZE		m_vScrollerSize;

	float		m_vScrollerFrequency;
	float		m_vScrollerSpeed;
	int			m_vScrollerAmplitude;

	bool		m_vIsUpdating;

public: // Construction
	CScroller( void );
	~CScroller( void );

	// Custom control initialization.
	bool		Initialize( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb, int nWidth, float fFreq, float fSpeed, int nAmp );

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
	void		SetWidth( const int nWidth );
	void		SetFrequency( float fFreq );
	void		SetSpeed( float fSpeed );
	void		SetAmplitude( int nAmp );

	std::string GetText( void ) const;
	COLORREF	GetColor( void ) const;
	int			GetWidth( void ) const;
	float		GetFrequency( void ) const;
	float		GetSpeed( void ) const;
	int			GetAmplitude( void ) const;
};

#endif // __PATCHLIB_CSCROLLER_H_INCLUDED__