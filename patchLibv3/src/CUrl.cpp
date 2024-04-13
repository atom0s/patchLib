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
#include "../include/CUrl.h"

#include "../alphablend/alphablend.h"
#include "../tinyxml/tinyxml.h"

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CUrl::CUrl( void )
{
	TRECT<int> rectDefault;
	this->m_vUrlBitmap		= NULL;
	this->m_vUrlDc			= NULL;
	this->m_vUrlColor		= RGB( 0, 0, 0 );
	this->m_vUrlColorOver	= RGB( 255, 255, 255 );
	this->m_vUrlText		= "http://www.google.com/";
	this->m_vUrlSite		= "http://www.google.com/";

	this->SetAlias( "<default_url>" );
	this->SetRect( rectDefault );
}
CUrl::~CUrl( void )
{
	this->Release();
}

//----------------------------------------------------------------------------------
//
// Creation and destruction.
//
//----------------------------------------------------------------------------------

bool CUrl::Initialize( TiXmlElement* pElement )
{
	const char* pAlias	= pElement->Attribute( "a" );
	if( !pAlias )
		return false;

	this->SetAlias( pAlias );

	int nTempX, nTempY = 0;
	pElement->QueryIntAttribute( "x", &nTempX );
	pElement->QueryIntAttribute( "y", &nTempY );

	TRECT<int> rcUrl(nTempX, nTempY, 0, 0);
	this->SetRect( rcUrl );

	int nTempR, nTempG, nTempB = 0;
	pElement->QueryIntAttribute( "r", &nTempR );
	pElement->QueryIntAttribute( "g", &nTempG );
	pElement->QueryIntAttribute( "b", &nTempB );
	this->m_vUrlColor = RGB( nTempR, nTempG, nTempB );

	nTempR, nTempG, nTempB = 0;
	pElement->QueryIntAttribute( "ro", &nTempR );
	pElement->QueryIntAttribute( "go", &nTempG );
	pElement->QueryIntAttribute( "bo", &nTempB );
	this->m_vUrlColorOver = RGB( nTempR, nTempG, nTempB );

	this->m_vUrlText = pElement->Attribute( "t" );
	this->m_vUrlSite = pElement->Attribute( "url" );

	if( this->GetAlias().length() == 0 || this->GetAlias() == "<default_url>" )
		return false;

	return true;
}

bool CUrl::Initialize( const std::string& strAlias, const std::string& strText, const std::string& strSite, const POINT& ptPoint, const COLORREF rgb, const COLORREF rgbOver )
{
	this->SetAlias( strAlias );

	TRECT<int> rcUrl(ptPoint.x, ptPoint.y, 0, 0);
	this->SetRect( rcUrl );

	this->m_vUrlText		= strText;
	this->m_vUrlSite		= strSite;
	this->m_vUrlColor		= rgb;
	this->m_vUrlColorOver	= rgbOver;

	return true;
}

bool CUrl::Setup( void )
{
	this->m_vUrlDc = CreateCompatibleDC(
		CPatchLib::instance()->GetScreenDc()
		);

	BITMAPINFO bmpInfo = { 0 };
	CPatchLib::instance()->GetBitmapInfo( bmpInfo );

	LPVOID lpBlankMem = NULL;
	this->m_vUrlBitmap = CreateDIBSection(
		this->m_vUrlDc,
		&bmpInfo,
		DIB_RGB_COLORS,
		&lpBlankMem,
		0, 0
		);

	SelectObject( this->m_vUrlDc, this->m_vUrlBitmap );
	SelectObject( this->m_vUrlDc, CPatchLib::instance()->GetFont() );
	SetTextColor( this->m_vUrlDc, this->m_vUrlColor );

	TRECT<int> rcUrl;
	SIZE sizeUrl	= { 0 };

	this->GetRect( rcUrl );
	GetTextExtentPoint( this->m_vUrlDc, this->m_vUrlText.c_str(), this->m_vUrlText.length(), &sizeUrl );
	rcUrl.right		= sizeUrl.cx;
	rcUrl.bottom	= sizeUrl.cy;
	this->SetRect( rcUrl );

	return true;
}

bool CUrl::Release( void )
{
	SAFE_DELETEDC( this->m_vUrlDc );
	SAFE_DELETEOBJ( this->m_vUrlBitmap );
	
	return true;
}

//----------------------------------------------------------------------------------
//
// Drawing.
//
//----------------------------------------------------------------------------------

bool CUrl::Draw( void )
{
	if( !this->m_vUrlBitmap || !this->m_vUrlDc )
		return false;
	if( this->m_vUrlText.length() == 0 )
		return true;

	TRECT<int> rectUrl;
	this->GetRect( rectUrl );

	PaintText(
		this->m_vUrlDc,
		this->m_vUrlBitmap,
		CPatchLib::instance()->GetMemoryDc(),
		CPatchLib::instance()->GetFrameDib(),
		this->m_vUrlText.c_str(),
		rectUrl.left,
		rectUrl.top
		);

	return true;
}

bool CUrl::Update( void )
{
	return true;
}

//----------------------------------------------------------------------------------
//
// IMouse overrides
//
//----------------------------------------------------------------------------------

bool CUrl::OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );

	TRECT<int> rectUrl;
	this->GetRect( rectUrl );

	if( IsOverRect( rectUrl, lParam ) )
	{
		SetTextColor( this->m_vUrlDc, this->m_vUrlColorOver );
		return true;
	}
	
	SetTextColor( this->m_vUrlDc, this->m_vUrlColor );
	return false;
}

bool CUrl::OnMouseDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );

	TRECT<int> rectUrl;
	this->GetRect( rectUrl );

	return IsOverRect( rectUrl, lParam );
}

bool CUrl::OnMouseUp( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( wParam );

	TRECT<int> rectUrl;
	this->GetRect( rectUrl );

	if( IsOverRect( rectUrl, lParam ) )
	{
		ShellExecute( hWnd, "open", this->m_vUrlSite.c_str(), NULL, NULL, SW_SHOWNORMAL );
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------------
//
// Url properties.
//
//----------------------------------------------------------------------------------

void CUrl::SetText( const std::string& strText )
{
	this->m_vUrlText = strText;

	if( this->m_vUrlBitmap )
	{
		TRECT<int> rectUrl;
		SIZE sizeUrl	= { 0 };

		this->GetRect( rectUrl );
		GetTextExtentPoint( this->m_vUrlDc, this->m_vUrlText.c_str(), this->m_vUrlText.length(), &sizeUrl );
		rectUrl.right		= sizeUrl.cx;
		rectUrl.bottom	= sizeUrl.cy;
		this->SetRect( rectUrl );
	}
}
void CUrl::SetSite( const std::string& strSite )
{
	this->m_vUrlSite = strSite;
}
void CUrl::SetColor( const COLORREF rgb )
{
	this->m_vUrlColor = rgb;

	if( this->m_vUrlDc )
		SetTextColor( this->m_vUrlDc, this->m_vUrlColor );
}
void CUrl::SetColorOver( const COLORREF rgb )
{
	this->m_vUrlColorOver = rgb;
}

std::string CUrl::GetText( void ) const
{
	return this->m_vUrlText;
}
std::string CUrl::GetSite( void ) const
{
	return this->m_vUrlSite;
}
COLORREF CUrl::GetColor( void ) const
{
	return this->m_vUrlColor;
}
COLORREF CUrl::GetColorOver( void ) const
{
	return this->m_vUrlColorOver;
}
