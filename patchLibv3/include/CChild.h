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

#ifndef __PATCHLIB_CCHILD_H_INCLUDED__
#define __PATCHLIB_CCHILD_H_INCLUDED__

#include <Windows.h>
#include <string>

class TiXmlElement;

/// <summary>
/// TRECT
///
/// Templated RECT class.
/// </summary>
template< typename T >
class TRECT
{
public:
	T left;
	T top;
	T right;
	T bottom;

	TRECT( void ) : left( 0 ), top( 0 ), right( 0 ), bottom( 0 )
	{
	}

	TRECT( T x1, T y1, T x2, T y2 ) : left( x1 ), top( y1 ), right( x2 ), bottom( y2 )
	{
	}
};

/// <summary>
/// IsOverRect
///
/// Determines if the given mouse position is currently
/// over the given rect.
/// </summary>
/// <param name="pRect"></param>
/// <param name="lParam"></param>
/// <returns></returns>
inline static bool IsOverRect( const TRECT<int>& pRect, LPARAM lParam )
{
	/**
	 * An alternative to this is to use 'IntersectRect'
	 * but there seems to be issues with it randomly.
	 */

	if( ( pRect.left <= LOWORD( lParam ) ) && ( ( pRect.right + pRect.left ) >= LOWORD( lParam ) ) )
	{
		if( ( pRect.top <= HIWORD( lParam ) ) && ( ( pRect.bottom + pRect.top ) >= HIWORD( lParam ) ) )
			return true;
	}
	return false;
}

/// <summary>
/// IMouse
///
///	Mouse event interface. Allows child objects to handle mouse
///	events raised from the parent window.
/// </summary>
interface IMouse
{
public:
	virtual bool OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam ) = 0;
	virtual bool OnMouseDown( HWND hWnd, WPARAM wParam, LPARAM lParam ) = 0;
	virtual bool OnMouseUp( HWND hWnd, WPARAM wParam, LPARAM lParam ) = 0;
};

/// <summary>
/// IChild
///	
///	Base child interface. Main interface inherited by child objects to
///	define the required functions that are internally called while using
///	controls with patchLib.
/// </summary>
interface IChild
{
public:
	virtual bool Initialize( TiXmlElement* pElement ) = 0;
	virtual bool Setup( void )		= 0;
	virtual bool Draw( void )		= 0;
	virtual bool Update( void )		= 0;
	virtual bool Release( void )	= 0;
};

/// <summary>
/// CChild
///
/// Child object base class. Base class that exposes common required properties
/// of a child object. Inherits IMouse and IChild to expose further required
/// functions.
/// </summary>
class CChild : public IMouse, public IChild
{
	std::string m_vChildAlias;
	TRECT<int>	m_vChildRect; // Left/top are x/y

public: // Construction
	CChild( void );
	~CChild( void );
	
public: // Properties
	void		SetAlias( const std::string& strAlias );
	void		SetRect( const TRECT<int>& rect );

	std::string GetAlias( void ) const;
	void		GetRect( TRECT<int>& rect ) const;
	TRECT<int>	GetRect( void ) const;
};

#endif // __PATCHLIB_CCHILD_H_INCLUDED__