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
#include "../include/CScroller.h"

#include "../alphablend/alphablend.h"
#include "../tinyxml/tinyxml.h"

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CScroller::CScroller( void )
{
	TRECT<int> rcDefault	(0, 0, 0, 0);
	this->m_vScrollerAmplitude	= 0;
	this->m_vScrollerBitmap		= NULL;
	this->m_vScrollerColor		= RGB( 0, 0, 0 );
	this->m_vScrollerDc			= NULL;
	this->m_vScrollerFrequency	= 0.0f;
	this->m_vScrollerPosition	= 0;
	this->m_vScrollerSpeed		= 0.0f;
	this->m_vScrollerText		= "";
	this->m_vScrollerWidth		= 0;

	this->SetAlias( "<default_scroller>" );
	this->SetRect( rcDefault );
}
CScroller::~CScroller( void )
{
	this->Release();
}

//----------------------------------------------------------------------------------
//
// Creation and destruction.
//
//----------------------------------------------------------------------------------

bool CScroller::Initialize( TiXmlElement* pElement )
{
	const char* pAlias	= pElement->Attribute( "a" );
	const char* pText	= pElement->Attribute( "t" );
	if( !pAlias )
		return false;

	this->SetAlias( pAlias );
	this->m_vScrollerText = pText;

	int nTempX, nTempY = 0;
	pElement->QueryIntAttribute( "x", &nTempX );
	pElement->QueryIntAttribute( "y", &nTempY );
	pElement->QueryIntAttribute( "w", &this->m_vScrollerWidth );

	TRECT<int> rcScroller(nTempX, nTempY, 0, 0);
	this->SetRect( rcScroller );

	int nTempR, nTempG, nTempB = 0;
	pElement->QueryIntAttribute( "r", &nTempR );
	pElement->QueryIntAttribute( "g", &nTempG );
	pElement->QueryIntAttribute( "b", &nTempB );
	this->m_vScrollerColor = RGB( nTempR, nTempG, nTempB );

	pElement->QueryIntAttribute( "amp", &this->m_vScrollerAmplitude );
	pElement->QueryFloatAttribute( "f", &this->m_vScrollerFrequency );
	pElement->QueryFloatAttribute( "s", &this->m_vScrollerSpeed );

	return true;
}

bool CScroller::Initialize( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb, int nWidth, float fFreq, float fSpeed, int nAmp )
{
	this->SetAlias( strAlias );

	TRECT<int> rcScroller(ptPoint.x, ptPoint.y, 0, 0);
	this->SetRect( rcScroller );

	this->m_vScrollerText		= strText;
	this->m_vScrollerColor		= rgb;
	this->m_vScrollerWidth		= nWidth;
	this->m_vScrollerAmplitude	= nAmp;
	this->m_vScrollerFrequency	= fFreq;
	this->m_vScrollerSpeed		= fSpeed;

	return true;
}

bool CScroller::Setup( void )
{
	this->m_vScrollerDc = CreateCompatibleDC(
		CPatchLib::instance()->GetScreenDc()
		);

	SelectObject( this->m_vScrollerDc, CPatchLib::instance()->GetFont() );

	CPatchLib::instance()->GetBitmapInfo( this->m_vScrollerBitmapInfo );

	TRECT<int> rectScroller;
	this->GetRect( rectScroller );
	GetTextExtentPoint( this->m_vScrollerDc, this->m_vScrollerText.c_str(), this->m_vScrollerText.length(), &this->m_vScrollerSize );
	
	rectScroller.right	= this->m_vScrollerSize.cx;
	rectScroller.bottom	= this->m_vScrollerSize.cy;
	this->SetRect( rectScroller );

	this->m_vScrollerBitmapInfo.bmiHeader.biWidth = this->m_vScrollerSize.cx;
	this->m_vScrollerBitmapInfo.bmiHeader.biWidth += this->m_vScrollerWidth * 2;
	this->m_vScrollerBitmapInfo.bmiHeader.biHeight = this->m_vScrollerSize.cy;
	this->m_vScrollerSize.cy-- ;

	LPVOID lpBlankMem = NULL;
	this->m_vScrollerBitmap = CreateDIBSection(
		this->m_vScrollerDc,
		&this->m_vScrollerBitmapInfo,
		DIB_RGB_COLORS,
		&lpBlankMem,
		0, 0
		);

	SelectObject( this->m_vScrollerDc, this->m_vScrollerBitmap );

	BITMAP bmp = { 0 };
	memset( &bmp, 0x00, sizeof( BITMAP ) );

	GetObject( this->m_vScrollerBitmap, sizeof( BITMAP ), &bmp );
	SetBkMode( this->m_vScrollerDc, OPAQUE );
	SetBkColor( this->m_vScrollerDc, 0 );
	SetTextColor( this->m_vScrollerDc, this->m_vScrollerColor );

	TextOut( this->m_vScrollerDc, this->m_vScrollerWidth, 0, this->m_vScrollerText.c_str(), this->m_vScrollerText.length() );

	BYTE* lpBits = reinterpret_cast< BYTE* >( bmp.bmBits );
	DWORD dwSize = bmp.bmWidth * bmp.bmHeight;

	while( dwSize > 0 )
	{
		if( *(DWORD*)lpBits != 0 )
			*(BYTE*)&lpBits[3] |= 0xFF;
		lpBits += 4;
		dwSize--;
	}

	this->m_vIsUpdating = false;

	return true;
}

bool CScroller::Release( void )
{
	SAFE_DELETEOBJ( this->m_vScrollerBitmap );
	SAFE_DELETEDC( this->m_vScrollerDc );

	return true;
}

//----------------------------------------------------------------------------------
//
// Drawing.
//
//----------------------------------------------------------------------------------

bool CScroller::Draw( void )
{
	TRECT<int> rectScroller;
	this->GetRect( rectScroller );

	AlphaBlendScroller(
		this->m_vScrollerWidth,
		this->m_vScrollerBitmap,
		this->m_vScrollerPosition,
		0,
		this->m_vScrollerWidth,
		this->m_vScrollerSize.cy,
		CPatchLib::instance()->GetFrameDib(),
		rectScroller.left,
		rectScroller.top,
		true,
		reinterpret_cast< DWORD_PTR >( &this->m_vScrollerFrequency ),
		reinterpret_cast< DWORD_PTR >( &this->m_vScrollerSpeed ),
		this->m_vScrollerAmplitude
		);

	return true;
}

bool CScroller::Update( void )
{
	this->m_vScrollerPosition++ ;

	BITMAP bmp = { 0 };
	GetObject( this->m_vScrollerBitmap, sizeof( BITMAP ), &bmp );

	if( ( bmp.bmWidth - this->m_vScrollerWidth ) == this->m_vScrollerPosition )
		this->m_vScrollerPosition = 0;

	return true;
}

//----------------------------------------------------------------------------------
//
// IMouse overrides
//
//----------------------------------------------------------------------------------

bool CScroller::OnMouseMove( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	return false;
}

bool CScroller::OnMouseDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	return false;
}

bool CScroller::OnMouseUp( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER( hWnd );
	UNREFERENCED_PARAMETER( wParam );
	UNREFERENCED_PARAMETER( lParam );
	return false;
}

//----------------------------------------------------------------------------------
//
// Properties
//
//----------------------------------------------------------------------------------

void CScroller::SetText( const std::string& strText )
{
	this->m_vScrollerText = strText;

	// Reset scroller object to update new text.
	this->Release();
	this->Setup();
}
void CScroller::SetColor( const COLORREF rgb )
{
	this->m_vScrollerColor = rgb;
}
void CScroller::SetWidth( const int nWidth )
{
	this->m_vScrollerWidth = nWidth;
}
void CScroller::SetFrequency( float fFreq )
{
	this->m_vScrollerFrequency = fFreq;
}
void CScroller::SetSpeed( float fSpeed )
{
	this->m_vScrollerSpeed = fSpeed;
}
void CScroller::SetAmplitude( int nAmp )
{
	this->m_vScrollerAmplitude = nAmp;
}

std::string CScroller::GetText( void ) const
{
	return this->m_vScrollerText;
}
COLORREF CScroller::GetColor( void ) const
{
	return this->m_vScrollerColor;
}
int CScroller::GetWidth( void ) const
{
	return this->m_vScrollerWidth;
}
float CScroller::GetFrequency( void ) const
{
	return this->m_vScrollerFrequency;
}
float CScroller::GetSpeed( void ) const
{
	return this->m_vScrollerSpeed;
}
int CScroller::GetAmplitude( void ) const
{
	return this->m_vScrollerAmplitude;
}
