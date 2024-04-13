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
#include "../include/CLabel.h"

#include "../alphablend/alphablend.h"
#include "../tinyxml/tinyxml.h"

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CLabel::CLabel( void )
{
	TRECT<int> rcDefault(0, 0, 0, 0);
	this->m_vLabelBitmap	= NULL;
	this->m_vLabelDc		= NULL;
	this->m_vLabelColor		= RGB( 0, 0, 0 );
	this->m_vLabelText		= "";

	this->SetAlias( "<default_label>" );
	this->SetRect( rcDefault );
}
CLabel::~CLabel( void )
{
	this->Release();
}

//----------------------------------------------------------------------------------
//
// Creation and destruction.
//
//----------------------------------------------------------------------------------

bool CLabel::Initialize( TiXmlElement* pElement )
{
	const char* pAlias	= pElement->Attribute( "a" );
	const char* pText	= pElement->Attribute( "t" );
	if( !pAlias )
		return false;

	this->SetAlias( pAlias );
	this->m_vLabelText = pText;

	int nTempX, nTempY = 0;
	pElement->QueryIntAttribute( "x", &nTempX );
	pElement->QueryIntAttribute( "y", &nTempY );

	TRECT<int> rcLabel(nTempX, nTempY, 0, 0);
	this->SetRect( rcLabel );

	int nTempR, nTempG, nTempB = 0;
	pElement->QueryIntAttribute( "r", &nTempR );
	pElement->QueryIntAttribute( "g", &nTempG );
	pElement->QueryIntAttribute( "b", &nTempB );
	this->m_vLabelColor = RGB( nTempR, nTempG, nTempB );
	
	return true;
}

bool CLabel::Initialize( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb )
{
	this->SetAlias( strAlias );

	TRECT<int> rcLabel(ptPoint.x, ptPoint.y, 0, 0);
	this->SetRect( rcLabel );

	this->m_vLabelText	= strText;
	this->m_vLabelColor = rgb;

	return true;
}

bool CLabel::Setup( void )
{
	this->m_vLabelDc = CreateCompatibleDC(
		CPatchLib::instance()->GetScreenDc()
		);

	BITMAPINFO bmpInfo = { 0 };
	CPatchLib::instance()->GetBitmapInfo( bmpInfo );

	LPVOID lpBlankMem = NULL;
	this->m_vLabelBitmap = CreateDIBSection(
		this->m_vLabelDc,
		&bmpInfo,
		DIB_RGB_COLORS,
		&lpBlankMem,
		0, 0
		);

	SelectObject( this->m_vLabelDc, this->m_vLabelBitmap );
	SelectObject( this->m_vLabelDc, CPatchLib::instance()->GetFont() );
	SetTextColor( this->m_vLabelDc, this->m_vLabelColor );

	return true;
}

bool CLabel::Release( void )
{
	SAFE_DELETEDC( this->m_vLabelDc );
	SAFE_DELETEOBJ( this->m_vLabelBitmap );

	return true;
}

//----------------------------------------------------------------------------------
//
// Drawing.
//
//----------------------------------------------------------------------------------

bool CLabel::Draw( void )
{
	if( !this->m_vLabelDc || !this->m_vLabelBitmap )
		return false;

	if( this->m_vLabelText.length() == 0 )
		return true;

	TRECT<int> rcLabel;
	this->GetRect( rcLabel );

	PaintText(
		this->m_vLabelDc, this->m_vLabelBitmap,
		CPatchLib::instance()->GetMemoryDc(),
		CPatchLib::instance()->GetFrameDib(),
		this->m_vLabelText.c_str(),
		rcLabel.left,
		rcLabel.top
		);

	return true;
}

bool CLabel::Update( void )
{
	return true;
}

//----------------------------------------------------------------------------------
//
// IMouse overrides
//
//----------------------------------------------------------------------------------

bool CLabel::OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	return false;
}

bool CLabel::OnMouseDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	return false;
}

bool CLabel::OnMouseUp( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	return false;
}

//----------------------------------------------------------------------------------
//
// Label properties.
//
//----------------------------------------------------------------------------------

void CLabel::SetText( const std::string& strText )
{
	this->m_vLabelText = strText;
}
void CLabel::SetColor( const COLORREF rgb )
{
	this->m_vLabelColor = rgb;
}

std::string CLabel::GetText( void ) const
{
	return this->m_vLabelText;
}
COLORREF CLabel::GetColor( void ) const
{
	return this->m_vLabelColor;
}
