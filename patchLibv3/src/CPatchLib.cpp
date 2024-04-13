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

#include "../include/imagehelper.h"
#include "../tinyxml/tinyxml.h"

#include "../include/CWindow.h"
#include "../include/CChild.h"
#include "../include/CButton.h"
#include "../include/CLabel.h"
#include "../include/CScroller.h"
#include "../include/CUrl.h"

#include "../include/CMusic.h"

// Initialize singleton instance.
boost::shared_ptr< CPatchLib > CPatchLib::m_vInstance = boost::shared_ptr< CPatchLib >( new CPatchLib );

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CPatchLib::CPatchLib( void )
{
	memset( &this->m_vBitmapInfo.bmiColors[0], 0x00, sizeof( RGBQUAD ) );
	memset( &this->m_vBitmapInfo.bmiHeader, 0x00, sizeof( BITMAPINFOHEADER ) );

	this->m_vAppInstance = NULL;

	this->m_vBlendFunction.AlphaFormat	= AC_SRC_ALPHA;
	this->m_vBlendFunction.BlendFlags	= 0;
	this->m_vBlendFunction.BlendOp		= 0;
	this->m_vBlendFunction.SourceConstantAlpha = 0;

	this->m_vBitmapInfo.bmiHeader.biBitCount		= 32;
	this->m_vBitmapInfo.bmiHeader.biClrImportant	= 0;
	this->m_vBitmapInfo.bmiHeader.biClrUsed			= 0;
	this->m_vBitmapInfo.bmiHeader.biCompression		= BI_RGB;
	this->m_vBitmapInfo.bmiHeader.biHeight			= 0;
	this->m_vBitmapInfo.bmiHeader.biPlanes			= 1;
	this->m_vBitmapInfo.bmiHeader.biSize			= sizeof( BITMAPINFOHEADER );
	this->m_vBitmapInfo.bmiHeader.biSizeImage		= 0;
	this->m_vBitmapInfo.bmiHeader.biWidth			= 0;
	this->m_vBitmapInfo.bmiHeader.biXPelsPerMeter	= 0;
	this->m_vBitmapInfo.bmiHeader.biYPelsPerMeter	= 0;

	this->m_vDcMemory	= NULL;
	this->m_vDcScreen	= NULL;
	this->m_vDibMain	= NULL;
	this->m_vBlankMem	= NULL;

	this->m_vFrameResId = NULL;
	this->m_vFrameBmp	= NULL;
	this->m_vFrameDib	= NULL;
	this->m_vFramePoint.x = 0;
	this->m_vFramePoint.y = 0;
	this->m_vFrameSize.cx = 0;
	this->m_vFrameSize.cy = 0;

	this->m_vFontResId	= NULL;
	this->m_vFontHeight = 0;
	this->m_vFontName	= "";
	this->m_vFont		= NULL;
	
	memset( &this->m_vFontObject, 0x00, sizeof( LOGFONT ) );
	this->m_vFontObject.lfCharSet			= DEFAULT_CHARSET;
	this->m_vFontObject.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	this->m_vFontObject.lfEscapement		= 0;
	this->m_vFontObject.lfHeight			= 8;
	this->m_vFontObject.lfItalic			= 0;
	this->m_vFontObject.lfOrientation		= 0;
	this->m_vFontObject.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	this->m_vFontObject.lfPitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
	this->m_vFontObject.lfQuality			= ANTIALIASED_QUALITY;
	this->m_vFontObject.lfStrikeOut			= 0;
	this->m_vFontObject.lfUnderline			= 0;
	this->m_vFontObject.lfWeight			= FW_DONTCARE;
	this->m_vFontObject.lfWidth				= 0;

	strcpy_s( this->m_vFontObject.lfFaceName, 32, "Courier New" );
}
CPatchLib::~CPatchLib( void )
{
}

//----------------------------------------------------------------------------------
//
// Singleton instance.
//
//----------------------------------------------------------------------------------

boost::shared_ptr< CPatchLib > CPatchLib::instance( void )
{
	if( !m_vInstance )
		m_vInstance.reset( new CPatchLib );
	return m_vInstance;
}

//----------------------------------------------------------------------------------
//
// Creation and destruction.
//
//----------------------------------------------------------------------------------

bool CPatchLib::AttachApplication( HINSTANCE hInstance )
{
	this->m_vAppInstance = hInstance;

	// Hackish fail-safe if nothing is passed.
	if( hInstance == NULL )
		this->m_vAppInstance = GetModuleHandle( NULL );

#ifdef PATCHLIB_USE_COMMCTRL
	// Initialize Common Control Library
	INITCOMMONCONTROLSEX iccex = { 0 };
	iccex.dwSize	= sizeof( INITCOMMONCONTROLSEX );
	iccex.dwICC		= PATCHLIB_COMMCTRL_FLAGS;
	InitCommonControlsEx( &iccex );
#endif

	return true;
}

bool CPatchLib::Initialize( int nTemplateResId /* = 0 */, int nFrameResId /* = 0 */ )
{
	// Invalid function call.
	if( nTemplateResId == 0 && nFrameResId == 0 )
		return false;

	//
	// Template initialization.
	//-------------------------------------------------------------

	if( nTemplateResId > 0 )
		return this->InitializeTemplate( nTemplateResId );

	//
	// Manual initialization.
	//-------------------------------------------------------------

	if( !LoadPngFromResource( NULL, nFrameResId, &this->m_vFrameBmp, &this->m_vFrameSize ) )
		return false;

	if( this->m_vFrameBmp == NULL || this->m_vFrameSize.cx == 0 || this->m_vFrameSize.cy == 0 )
		return false;

	this->m_vFrameResId = nFrameResId;

	this->m_vBitmapInfo.bmiHeader.biWidth	= this->m_vFrameSize.cx;
	this->m_vBitmapInfo.bmiHeader.biHeight	= this->m_vFrameSize.cy;

	this->m_vDcScreen	= GetDC( 0 );
	this->m_vDcMemory	= CreateCompatibleDC( this->m_vDcScreen );
	this->m_vDibMain	= CreateDIBSection( this->m_vDcScreen, &this->m_vBitmapInfo, DIB_RGB_COLORS, &this->m_vBlankMem, 0, 0 );

	this->m_vFrameDib	= CreateDIBSection( this->m_vDcMemory, &this->m_vBitmapInfo, DIB_RGB_COLORS, &this->m_vBlankMem, 0, 0 );
	SelectObject( this->m_vDcMemory, this->m_vFrameDib );
	BitmapToBitmap( this->m_vDibMain, this->m_vFrameBmp );

	// Initialize window object.
	if( !this->m_vWindow )
		this->m_vWindow.reset( new CWindow );

	this->m_vWindow->SetClass( "pLibv3DefClass" );
	this->m_vWindow->SetTitle( "pLibv3DefTitle" );
	this->m_vWindow->SetWidth( this->m_vFrameSize.cx );
	this->m_vWindow->SetHeight( this->m_vFrameSize.cy );
	this->m_vWindow->SetStartXPos( -1 );
	this->m_vWindow->SetStartYPos( -1 );

	// Initialize music object.
	if( !this->m_vMusicObject )
		this->m_vMusicObject.reset( new CMusic );

	return true;
}

bool CPatchLib::InitializeTemplate( int nTemplateResId )
{
	// Shouldn't be here if this is already 0...
	if( nTemplateResId == 0 )
		return false;

	// Prepare Xml template for reading.
	TiXmlDocument xmlDoc;
	if( !xmlDoc.LoadFromResource( nTemplateResId ) )
		return false;
	TiXmlHandle xmlHandle( &xmlDoc );

	/*------------------------------------------------------------------------
		Element: patchLib
		Purpose:
			Contains the base xml entry that holds all child nodes.
			This element is required for patchLib to continue loading
			a template file.
	------------------------------------------------------------------------*/

	TiXmlElement* pPatchLib = xmlHandle.FirstChildElement( "patchLib" ).Element();
	if( !pPatchLib ) return false;

	/*------------------------------------------------------------------------
		Element: Frame
		Purpose:
			Contains the frame information to load the base background
			image for the overall window. This must be loaded first for
			the window to properly initialize!
	------------------------------------------------------------------------*/

	TiXmlElement* pFrame = pPatchLib->FirstChildElement( "Frame" );
	if( !pFrame ) return false;

	// Obtain frame resource id.
	if( pFrame->QueryIntAttribute( "resId", &this->m_vFrameResId ) == TIXML_NO_ATTRIBUTE )
		return false;

	// Initialize frame and base patchLib variables.
	if( !this->Initialize( NULL, this->m_vFrameResId ) ) return false;

	// Ensure window was initialized.
	if( !this->m_vWindow ) return false;

	/*------------------------------------------------------------------------
		Element: Window
		Purpose:
			Contains the window information for the overall window
			used. Contains children nodes for each child object.
	------------------------------------------------------------------------*/

	TiXmlElement* pWindow = pPatchLib->FirstChildElement( "Window" );
	if( !pWindow ) return false;

	const char* pWindowClass = pWindow->Attribute( "c" );
	const char* pWindowTitle = pWindow->Attribute( "t" );

	if( pWindowClass )
		this->m_vWindow->SetClass( pWindowClass );
	if( pWindowTitle )
		this->m_vWindow->SetTitle( pWindowTitle );

	int nStartXPos, nStartYPos, nTopMost = 0;
	pWindow->QueryIntAttribute( "x", &nStartXPos );
	pWindow->QueryIntAttribute( "y", &nStartYPos );
	pWindow->QueryIntAttribute( "a", &nTopMost );

	this->m_vWindow->SetStartXPos( nStartXPos );
	this->m_vWindow->SetStartYPos( nStartYPos );
	this->m_vWindow->SetTopMost( ( nTopMost > 0 ) ? true : false );

	/*------------------------------------------------------------------------
		Element: Cursor
		Purpose:
			Contains the information for the cursor image used.
	------------------------------------------------------------------------*/

	TiXmlElement* pCursor = pPatchLib->FirstChildElement( "Cursor" );
	if( !pCursor )
		this->m_vWindow->SetCursor( 0 );
	else
	{
		int nCursorResId = 0;
		pCursor->QueryIntAttribute( "resId", &nCursorResId );
		this->m_vWindow->SetCursor( nCursorResId );
	}

	/*------------------------------------------------------------------------
		Element: Icon
		Purpose:
			Contains the information for the icon image used.
	------------------------------------------------------------------------*/

	TiXmlElement* pIcon = pPatchLib->FirstChildElement( "Icon" );
	if( !pIcon )
		this->m_vWindow->SetIcon( 0 );
	else
	{
		int nIconResId = 0;
		pIcon->QueryIntAttribute( "resId", &nIconResId );
		this->m_vWindow->SetIcon( nIconResId );
	}

	/*------------------------------------------------------------------------
		Element: Font
		Purpose:
			Contains the information for the font used.
	------------------------------------------------------------------------*/

	TiXmlElement* pFont = pPatchLib->FirstChildElement( "Font" );
	if( pFont )
	{
		this->m_vFontName = pFont->Attribute( "n" );
		pFont->QueryIntAttribute( "resId", &this->m_vFontResId );
		pFont->QueryIntAttribute( "h", &this->m_vFontHeight );
	}

	if( !this->InitializeFont( this->m_vFontResId, this->m_vFontName, this->m_vFontHeight ) )
	{
		this->m_vFontResId	= 0;
		this->m_vFontName	= "Courier New";
		this->m_vFontHeight = 10;
		this->InitializeFont( this->m_vFontResId, this->m_vFontName, this->m_vFontHeight );
	}

	/*------------------------------------------------------------------------
		Element: Music
		Purpose:
			Contains the information for the music used.
	------------------------------------------------------------------------*/

	TiXmlElement* pMusic = pPatchLib->FirstChildElement( "Music" );
	if( pMusic )
	{
		this->m_vMusicObject->Initialize( pMusic );
	}
	else
	{
		this->m_vMusicObject->Initialize( 0 );
	}

	/*------------------------------------------------------------------------
		Element: Window [Control Objects]
		Purpose:
			Loops and loads the child elements inside the window element.
			Due to edits to patchLib this should be the last called
			loading from the template!
	------------------------------------------------------------------------*/

	if( !this->m_vWindow->LoadControls( pWindow ) )
		return false;
	
	return true;
}

bool CPatchLib::InitializeWindow( void )
{
	if( !this->m_vWindow ) return false;
	
	return this->m_vWindow->Initialize( this->m_vAppInstance );
}

bool CPatchLib::InitializeFont( int nFontResId, const std::string& strFontName, int nFontHeight )
{
	this->m_vFontResId	= nFontResId;
	this->m_vFontName	= strFontName;
	this->m_vFontHeight	= nFontHeight;

	if( nFontResId != 0 )
	{
		HRSRC hResource = FindResource( this->m_vAppInstance, MAKEINTRESOURCE( nFontResId ), RT_RCDATA );
		if( !hResource ) return false;

		HGLOBAL hGlobal = LoadResource( this->m_vAppInstance, hResource );
		if( !hGlobal ) return false;

		LPVOID lpResource = LockResource( hGlobal );
		if( !lpResource ) return false;

		DWORD dwFontSize = SizeofResource( this->m_vAppInstance, hResource );
	
		DWORD dwNumFonts = NULL;
		if( !AddFontMemResourceEx( lpResource, dwFontSize, NULL, &dwNumFonts ) )
			return false;
	}

	this->m_vFontObject.lfHeight = nFontHeight;
	strcpy_s( this->m_vFontObject.lfFaceName, 32, strFontName.c_str() );

	// Adjust font size to look proper.
	this->m_vFontObject.lfHeight = -MulDiv(
		this->m_vFontObject.lfHeight,
		GetDeviceCaps( GetDC( 0 ), LOGPIXELSY ),
		72
		);

	this->m_vFont = CreateFontIndirect( &this->m_vFontObject );
	if( !this->m_vFont )
		return false;
	return true;
}

bool CPatchLib::Destroy( void )
{
	// Cleanup frame.
	SAFE_DELETEOBJ( this->m_vFrameDib );
	SAFE_DELETEOBJ( this->m_vFrameBmp );

	// Cleanup patchLib.
	SAFE_DELETEOBJ( this->m_vDibMain );
	SAFE_DELETEDC( this->m_vDcMemory );
	SAFE_DELETEDC( this->m_vDcScreen );

	// Cleanup music.
	if( this->m_vMusicObject )
		this->m_vMusicObject->Release();

	// Cleanup window.
	if( this->m_vWindow )
		this->m_vWindow->Destroy( this->m_vAppInstance );

	return true;
}

//----------------------------------------------------------------------------------
//
// Drawing.
//
//----------------------------------------------------------------------------------

bool CPatchLib::Start( void )
{
	if( !this->m_vWindow ) return false;
	if( !this->m_vWindow->GetHwnd() ) return false;

	ShowWindow( this->m_vWindow->GetHwnd(), SW_SHOWNORMAL );
	UpdateWindow( this->m_vWindow->GetHwnd() );

	if( this->m_vMusicObject )
		this->m_vMusicObject->Play();

	this->m_vWindow->MessagePump();

	return true;
}

bool CPatchLib::Draw( void )
{
	if( !this->m_vWindow ) return false;
	if( !this->m_vWindow->GetHwnd() ) return false;

	// Apply transparency to frame.
	BitmapToBitmap( this->m_vFrameDib, this->m_vDibMain );

#ifndef PATCHLIB_USE_GDIPLUS
	//
	// If Gdiplus is enabled, we do not need to call this
	// because Gdiplus will automatically apply alpha for
	// us instead.
	//
	PremultipliedAlpha( this->m_vFrameDib );
#endif

	// Draw window children.
	this->m_vWindow->Draw();

	// Update window.
	UpdateLayeredWindow(
		this->m_vWindow->GetHwnd(),
		this->m_vDcScreen,
		NULL,
		&this->m_vFrameSize,
		this->m_vDcMemory,
		&this->m_vFramePoint,
		NULL,
		&this->m_vBlendFunction,
		ULW_ALPHA
		);

	return true;
}

bool CPatchLib::FadeWindowIn( void )
{
	if( !this->m_vWindow ) return false;
	if( !this->m_vWindow->GetHwnd() ) return false;

	while( this->m_vBlendFunction.SourceConstantAlpha < 255 )
	{
		this->m_vBlendFunction.SourceConstantAlpha += 5;

		UpdateLayeredWindow(
			this->m_vWindow->GetHwnd(),
			this->m_vDcScreen,
			NULL,
			&this->m_vFrameSize,
			this->m_vDcMemory,
			&this->m_vFramePoint,
			NULL,
			&this->m_vBlendFunction,
			ULW_ALPHA
			);

		Sleep( 10 );
	}

	return true;
}

bool CPatchLib::FadeWindowOut( void )
{
	if( !this->m_vWindow ) return false;
	if( !this->m_vWindow->GetHwnd() ) return false;

	while( this->m_vBlendFunction.SourceConstantAlpha > 0 )
	{
		this->m_vBlendFunction.SourceConstantAlpha -= 5;

		UpdateLayeredWindow(
			this->m_vWindow->GetHwnd(),
			this->m_vDcScreen,
			NULL,
			&this->m_vFrameSize,
			this->m_vDcMemory,
			&this->m_vFramePoint,
			NULL,
			&this->m_vBlendFunction,
			ULW_ALPHA
			);

		Sleep( 10 );
	}

	return true;
}

//----------------------------------------------------------------------------------
//
// Variable exposure.
//
//----------------------------------------------------------------------------------

CWindow* CPatchLib::GetWindowObject( void ) const
{
	// Important! 
	// Return weak reference!
	return this->m_vWindow.get();
}

CMusic* CPatchLib::GetMusicObject( void ) const
{
	// Important!
	// Return weak reference!
	return this->m_vMusicObject.get();
}

void CPatchLib::GetBlendFunction( BLENDFUNCTION& blendFunc ) const
{
	memcpy( &blendFunc, &this->m_vBlendFunction, sizeof( BLENDFUNCTION ) );
}
void CPatchLib::GetBitmapInfo( BITMAPINFO& bmpInfo ) const
{
	memcpy( &bmpInfo.bmiColors[0], &this->m_vBitmapInfo.bmiColors[0], sizeof( RGBQUAD ) );
	memcpy( &bmpInfo.bmiHeader, &this->m_vBitmapInfo.bmiHeader, sizeof( BITMAPINFOHEADER ) );
}
HDC CPatchLib::GetMemoryDc( void ) const
{
	return this->m_vDcMemory;
}
HDC CPatchLib::GetScreenDc( void ) const
{
	return this->m_vDcScreen;
}
HBITMAP CPatchLib::GetMainDib( void ) const
{
	return this->m_vDibMain;
}

int CPatchLib::GetFrameResId( void ) const
{
	return this->m_vFrameResId;
}
HBITMAP CPatchLib::GetFrameBmp( void ) const
{
	return this->m_vFrameBmp;
}
HBITMAP CPatchLib::GetFrameDib( void ) const
{
	return this->m_vFrameDib;
}
void CPatchLib::GetFramePoint( POINT& lpPoint ) const
{
	lpPoint.x = this->m_vFramePoint.x;
	lpPoint.y = this->m_vFramePoint.y;
}
void CPatchLib::GetFrameSize( SIZE& lpSize ) const
{
	lpSize.cx = this->m_vFrameSize.cx;
	lpSize.cy = this->m_vFrameSize.cy;
}

int CPatchLib::GetFontResId( void ) const
{
	return this->m_vFontResId;
}
int CPatchLib::GetFontHeight( void ) const
{
	return this->m_vFontHeight;
}
std::string CPatchLib::GetFontName( void ) const
{
	return this->m_vFontName;
}
HFONT CPatchLib::GetFont( void ) const
{
	return this->m_vFont;
}
void CPatchLib::GetFontInfo( LOGFONT& logfont ) const
{
	memcpy( &logfont, &this->m_vFontObject, sizeof( LOGFONT ) );
}