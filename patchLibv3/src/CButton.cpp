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

#include "../patchLib.h"
#include "../include/CButton.h"
#include "../include/CWindow.h"

#include "../alphablend/alphablend.h"
#include "../include/imagehelper.h"
#include "../tinyxml/tinyxml.h"

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CButton::CButton( void )
{
	TRECT<int> rcDefault	(0, 0, 0, 0);

	this->m_vButtonResId		= 0;
	this->m_vButtonCallback		= "";

	this->m_vButtonBitmap		= NULL;
	this->m_vButtonDc			= NULL;
	this->m_vButtonDib			= NULL;
	
	this->m_vButtonStateCount	= 0;
	this->m_vButtonState		= 0;
	this->m_vButtonEnabled		= true;	
	this->m_vIsMouseDown		= false;
	this->m_vIsMouseOver		= false;

	this->m_vButtonFunction		= NULL;

	this->SetAlias( "<default_button>" );
	this->SetRect( rcDefault );
}
CButton::~CButton( void )
{
	this->Release();
}

//----------------------------------------------------------------------------------
//
// Creation and destruction.
//
//----------------------------------------------------------------------------------

bool CButton::Initialize( TiXmlElement* pElement )
{
	const char* pszAlias	= pElement->Attribute( "a" );
	const char* pszCallback = pElement->Attribute( "callback" );
	
	if( !pszAlias )
		return false;

	TRECT<int> rectButton(0, 0, 0, 0);
	pElement->QueryIntAttribute( "x", reinterpret_cast< int* >( &rectButton.left ) );
	pElement->QueryIntAttribute( "y", reinterpret_cast< int* >( &rectButton.top ) );
	pElement->QueryIntAttribute( "s", &this->m_vButtonStateCount );
	pElement->QueryIntAttribute( "resId", &this->m_vButtonResId );

	if( pszCallback )
		this->m_vButtonCallback = pszCallback;
	this->m_vButtonFunction = NULL;

	this->SetAlias( pszAlias );
	this->SetRect( rectButton );

	return true;
}

bool CButton::Initialize( const std::string& strAlias, const POINT& ptPoint, const std::string& strCommand, int nTotalStates, int nButtonResId )
{
	TRECT<int> rectButton(ptPoint.x, ptPoint.y, 0, 0);

	this->m_vButtonResId		= nButtonResId;
	this->m_vButtonCallback		= strCommand;
	this->m_vButtonState		= 0;
	this->m_vButtonStateCount	= nTotalStates;
	this->m_vButtonFunction		= NULL;

	this->SetAlias( strAlias );
	this->SetRect( rectButton );

	return true;
}

bool CButton::Setup( void )
{
	// Attempt to load button image.
	SIZE sizeButton = { 0 };
	if( !LoadPngFromResource( NULL, this->m_vButtonResId, &this->m_vButtonBitmap, &sizeButton ) )
		return false;

	TRECT<int> rectButton;
	this->GetRect( rectButton );

	rectButton.right	= sizeButton.cx; // set button width
	rectButton.bottom	= sizeButton.cy; // set button height (total)
	this->SetRect( rectButton );

	this->m_vButtonDc = CreateCompatibleDC(
		CPatchLib::instance()->GetScreenDc()
		);

	SelectObject( this->m_vButtonDc, this->m_vButtonBitmap );
	BitmapToBitmap( CPatchLib::instance()->GetFrameDib(), this->m_vButtonBitmap );

	return true;
}

bool CButton::Release( void )
{
	SAFE_DELETEOBJ( this->m_vButtonBitmap );
	SAFE_DELETEOBJ( this->m_vButtonDib );
	SAFE_DELETEDC( this->m_vButtonDc );

	return true;
}

//----------------------------------------------------------------------------------
//
// Drawing.
//
//----------------------------------------------------------------------------------

bool CButton::Draw( void )
{
	// Determine current button state.
	int nButtonState = this->m_vButtonState;
	if( this->m_vButtonStateCount > 2 )
	{
		nButtonState += static_cast< int >( this->m_vIsMouseDown );
		nButtonState += static_cast< int >( this->m_vIsMouseOver );
	}
	else
	{
		nButtonState = this->m_vButtonEnabled;
	}

	TRECT<int> rectButton;
	this->GetRect( rectButton );

	AlphaBlend2(
		NULL,
		this->m_vButtonBitmap,
		NULL,
		( rectButton.bottom / this->m_vButtonStateCount ) * nButtonState,
		rectButton.right,
		( rectButton.bottom / this->m_vButtonStateCount ),
		CPatchLib::instance()->GetFrameDib(),
		rectButton.left,
		rectButton.top,
		false
		);

	return true;
}

bool CButton::Update( void )
{
	return true;
}

//----------------------------------------------------------------------------------
//
// IMouse overrides
//
//----------------------------------------------------------------------------------

bool CButton::OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );

	TRECT<int> rectButton;
	this->GetRect( rectButton );
	rectButton.bottom = rectButton.bottom / this->m_vButtonStateCount;

	if( IsOverRect( rectButton, lParam ) )
	{
		this->m_vIsMouseOver = true;
		return true;
	}

	this->m_vIsMouseOver = false;
	return false;
}

bool CButton::OnMouseDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );

	TRECT<int> rectButton;
	this->GetRect( rectButton );
	rectButton.bottom = rectButton.bottom / this->m_vButtonStateCount;

	if( IsOverRect( rectButton, lParam ) )
	{
		this->m_vIsMouseDown = true;
		this->m_vIsMouseOver = true;
		return true;
	}
	
	this->m_vIsMouseDown = false;
	this->m_vIsMouseOver = false;
	return false;
}

bool CButton::OnMouseUp( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );

	TRECT<int> rectButton;
	this->GetRect( rectButton );
	rectButton.bottom = rectButton.bottom / this->m_vButtonStateCount;

	if( IsOverRect( rectButton, lParam ) )
	{
		this->m_vIsMouseDown = false;
		this->m_vIsMouseOver = true;

		if( this->m_vButtonCallback.length() > 0 )
			CPatchLib::instance()->GetWindowObject()->CallCommand( this->m_vButtonCallback, this );

		if( this->m_vButtonStateCount == 2 )
			this->m_vButtonEnabled = !this->m_vButtonEnabled;

		return true;
	}

	this->m_vIsMouseDown = false;
	this->m_vIsMouseOver = false;
	return false;
}

//----------------------------------------------------------------------------------
//
// Button properties.
//
//----------------------------------------------------------------------------------

void CButton::SetResId( int nResId )
{
	this->m_vButtonResId = nResId;
}
void CButton::SetCallback( const std::string& strCommand )
{
	this->m_vButtonCallback = strCommand;
}
void CButton::SetHeight( int nHeight )
{
	// TODO
	// Currently this is not really needed though.
	UNREFERENCED_PARAMETER( nHeight );
}
void CButton::SetStateCount( int nStateCount )
{
	this->m_vButtonStateCount = nStateCount;
}
void CButton::SetState( int nState )
{
	this->m_vButtonState = nState;
}
void CButton::SetEnabled( bool bEnabled )
{
	this->m_vButtonEnabled = bEnabled;
}
void CButton::SetMouseDown( bool bMouseDown )
{
	this->m_vIsMouseDown = bMouseDown;
}
void CButton::SetMouseOver( bool bMouseOver )
{
	this->m_vIsMouseOver = bMouseOver;
}

int CButton::GetResId( void ) const
{
	return this->m_vButtonResId;
}
std::string CButton::GetCallback( void ) const
{
	return this->m_vButtonCallback;
}
int CButton::GetHeight( void ) const
{
	TRECT< int > rectButton;
	this->GetRect( rectButton );

	return ( rectButton.bottom / this->m_vButtonStateCount );
}
int CButton::GetStateCount( void ) const
{
	return this->m_vButtonStateCount;
}
int CButton::GetState( void ) const
{
	return this->m_vButtonState;
}
bool CButton::IsEnabled( void ) const
{
	return this->m_vButtonEnabled;
}
bool CButton::IsMouseDown( void ) const
{
	return this->m_vIsMouseDown;
}
bool CButton::IsMouseOver( void ) const
{
	return this->m_vIsMouseOver;
}