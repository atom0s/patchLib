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
#include "../include/CWindow.h"

#include "../include/CChild.h"
#include "../include/CButton.h"
#include "../include/CLabel.h"
#include "../include/CScroller.h"
#include "../include/CUrl.h"

#include "../include/imagehelper.h"
#include "../tinyxml/tinyxml.h"

#include <boost/foreach.hpp>
#include <sstream>

/**
 * LOAD_PLIB_CONTROLS
 * 
 * Simple macro definition for easily loading a specific
 * control type based on the name and class object.
 */
#define LOAD_PLIB_CONTROLS( String, Class ) \
	for( TiXmlElement* pChildElement = pElement->FirstChildElement( String ); pChildElement; pChildElement = pChildElement->NextSiblingElement( String ) ) \
	{ \
		boost::shared_ptr< Class > pObj = boost::shared_ptr< Class >( new Class ); \
		if( !pObj->Initialize( pChildElement ) || !pObj->Setup() ) \
			pObj->Release(); \
		else \
			this->m_vChildren.push_back( pObj ); \
	}

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CWindow::CWindow( void )
{
	memset( &this->m_vWndClassEx, 0x00, sizeof( WNDCLASSEX ) );

	this->m_vClass			= "pLibv3DefClass";
	this->m_vTitle			= "pLibv3DefTitle";
	this->m_vWidth			= 0;
	this->m_vHeight			= 0;
	this->m_vXPosition		= 0;
	this->m_vYPosition		= 0;
	this->m_vStartXPos		= 0;
	this->m_vStartYPos		= 0;
	this->m_vTopMost		= false;
	this->m_vHwnd			= NULL;

	this->m_vCursorResId	= 0;
	this->m_vCursor			= NULL;

	this->m_vIconResId		= 0;
	this->m_vIcon			= NULL;

#ifdef PATCHLIB_USE_GDIPLUS
	this->m_vGdiGraphics	= NULL;
	this->m_vGdiplusInput	= NULL;
	this->m_vGdiplusToken	= NULL;
#endif
}
CWindow::~CWindow( void )
{
	this->Destroy( GetModuleHandle( NULL ) );
}

//----------------------------------------------------------------------------------
//
// Creation and destruction.
//
//----------------------------------------------------------------------------------

bool CWindow::Initialize( HINSTANCE hInstance )
{
	if( this->m_vClass.length() == 0 )
		return false;

#ifdef PATCHLIB_USE_GDIPLUS
	if( GdiplusStartup( &this->m_vGdiplusToken, &this->m_vGdiplusInput, NULL ) != 0 )
		return false;
#endif

	this->m_vWndClassEx.cbClsExtra		= NULL;
	this->m_vWndClassEx.cbSize			= sizeof( WNDCLASSEX );
	this->m_vWndClassEx.hbrBackground	= reinterpret_cast< HBRUSH >( COLOR_BTNFACE + 1 );
	this->m_vWndClassEx.hCursor			= NULL;
	this->m_vWndClassEx.hIcon			= NULL;
	this->m_vWndClassEx.hIconSm			= NULL;
	this->m_vWndClassEx.hInstance		= hInstance;
	this->m_vWndClassEx.lpfnWndProc		= this->staticWndProc;
	this->m_vWndClassEx.lpszClassName	= this->m_vClass.c_str();
	this->m_vWndClassEx.lpszMenuName	= NULL;
	this->m_vWndClassEx.style			= CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNWINDOW;
	
	// Register class.
	if( !RegisterClassEx( &this->m_vWndClassEx ) )
		return false;

	// Reposition window to center if requested.
	if( this->m_vStartXPos == -1 && this->m_vStartYPos == -1 )
	{
		this->m_vStartXPos = ( GetSystemMetrics( SM_CXSCREEN ) - this->m_vWidth ) / 2;
		this->m_vStartYPos = ( GetSystemMetrics( SM_CYSCREEN ) - this->m_vHeight ) / 2;
	}

	// Attempt to create the window.
	this->m_vHwnd = CreateWindowEx(
		WS_EX_LAYERED,
		this->m_vClass.c_str(),
		this->m_vTitle.c_str(),
		WS_POPUP | WS_VISIBLE,
		this->m_vStartXPos, this->m_vStartYPos,
		this->m_vWidth, this->m_vHeight,
		HWND_DESKTOP, NULL, hInstance, this
		);

	// Validate window creation.
	if( !this->m_vHwnd )
	{
		UnregisterClass( this->m_vClass.c_str(), hInstance );
		return false;
	}

	// Set window icon.
	if( this->m_vIconResId != 0 )
		this->SetIcon( this->m_vIconResId );

	// Set window cursor.
	if( this->m_vCursorResId != 0 )
		this->SetCursor( this->m_vCursorResId );

#ifdef PATCHLIB_USE_GDIPLUS
	// Initialize Gdiplus Graphics engine.
	this->m_vGdiGraphics = new Graphics( CPatchLib::instance()->GetMemoryDc() );
	if( !this->m_vGdiGraphics )
		return false;

	// Make internal copy of frame image.
	if( !ResourceToGdiPlus( &this->m_vGdiFrame, CPatchLib::instance()->GetFrameResId(), RT_RCDATA ) )
		return false;
#endif

	return true;
}

bool CWindow::Destroy( HINSTANCE hInstance )
{
	if( this->m_vHwnd )
	{
		DestroyWindow( this->m_vHwnd );
		this->m_vHwnd = NULL;
	}

	if( this->m_vClass.length() > 0 )
		UnregisterClass( this->m_vClass.c_str(), hInstance );

#ifdef PATCHLIB_USE_GDIPLUS
	SAFE_DELETE( this->m_vGdiFrame );
	SAFE_DELETE( this->m_vGdiGraphics );

	GdiplusShutdown( this->m_vGdiplusToken );
	this->m_vGdiplusToken = NULL;
#endif

	return true;
}

bool CWindow::LoadControls( TiXmlElement* pElement )
{
	LOAD_PLIB_CONTROLS( "Button", CButton )
	LOAD_PLIB_CONTROLS( "Label", CLabel )
	LOAD_PLIB_CONTROLS( "Url", CUrl )
	LOAD_PLIB_CONTROLS( "Scroller", CScroller )

	return true;
}

//----------------------------------------------------------------------------------
//
// Drawing.
//
//----------------------------------------------------------------------------------

bool CWindow::Draw( void )
{
#ifdef PATCHLIB_USE_GDIPLUS
	//
	// Win32 Control Drawing
	// 
	// Hackish method using Gdiplus to draw Win32 controls ontop
	// of a layered window. This is not the most efficient method
	// of doing this probably, but it does work.
	// 

	// Obtain frame size.
	SIZE frameSize = { 0 };
	CPatchLib::instance()->GetFrameSize( frameSize );

	// Prepare Gdiplus image and redraw frame.
	this->m_vGdiGraphics->Clear( Color( 0, 0, 0, 0 ) );
	this->m_vGdiGraphics->SetSmoothingMode( SmoothingModeNone );
	this->m_vGdiGraphics->SetCompositingMode( CompositingModeSourceCopy );
	this->m_vGdiGraphics->SetCompositingQuality( CompositingQualityHighQuality );
	this->m_vGdiGraphics->DrawImage( this->m_vGdiFrame, NULL, NULL, frameSize.cx, frameSize.cy );	

	// Loop and draw window children.
	HWND hWndChild = GetWindow( this->m_vHwnd, GW_CHILD );
	while( hWndChild )
	{
		RECT rectChild = { 0 };
		GetWindowRect( hWndChild, &rectChild );

		// MFC Hack : Get client position.
		ScreenToClient( this->m_vHwnd, reinterpret_cast< POINT* >( &rectChild ) );
		ScreenToClient( this->m_vHwnd, reinterpret_cast< POINT* >( &rectChild ) + 1 );

		// Create memory clone of child object.
		HDC hChildDc = CreateCompatibleDC( CPatchLib::instance()->GetMemoryDc() );
		HBITMAP hChildBitmap = CreateCompatibleBitmap( CPatchLib::instance()->GetMemoryDc(), ( rectChild.right - rectChild.left ), ( rectChild.bottom - rectChild.top ) );
		HGDIOBJ hChildBmpOld = SelectObject( hChildDc, hChildBitmap );

		// Print child to memory.
		SendMessage( hWndChild, WM_PRINT, reinterpret_cast< WPARAM >( hChildDc ), static_cast< LPARAM >(
			PRF_CHECKVISIBLE | PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT
			) );

		// Draw child to frame.
		Bitmap bmpChildControl( hChildBitmap, NULL );
		this->m_vGdiGraphics->DrawImage( &bmpChildControl, rectChild.left, rectChild.top );

		// Cleanup objects.
		SelectObject( hChildDc, hChildBmpOld );
		DeleteObject( hChildBitmap );
		DeleteObject( hChildBmpOld );
		DeleteDC( hChildDc );

		// Get next child object.
		hWndChild = GetWindow( hWndChild, GW_HWNDNEXT );
	}
#endif

	// Loop each control and call draw method.
	std::for_each( this->m_vChildren.begin(), this->m_vChildren.end(), 
		boost::bind( &CChild::Draw, _1 ) 
		);

	return true;
}

bool CWindow::Update( void )
{
	// Loop each control and call update method.
	std::for_each( this->m_vChildren.begin(), this->m_vChildren.end(), 
		boost::bind( &CChild::Update, _1 ) 
		);
	return true;
}

//----------------------------------------------------------------------------------
//
// Message handlers.
//
//----------------------------------------------------------------------------------

bool CWindow::MessagePump( void )
{
	if( !this->m_vHwnd ) return false;

	static MSG uMsg = { 0 };
	while( GetMessage( &uMsg, this->m_vHwnd, 0, 0 ) )
	{
		if( uMsg.message == WM_QUIT )
		{
			while( PeekMessage( &uMsg, this->m_vHwnd, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &uMsg );
				DispatchMessage( &uMsg );
			}
			return true;
		}

		TranslateMessage( &uMsg );
		DispatchMessage( &uMsg );
	}

	return true;
}

LRESULT __stdcall CWindow::staticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Obtain user data to this window instance.
	CWindow* pWindow = reinterpret_cast< CWindow* >( GetWindowLongPtr( hWnd, GWL_USERDATA ) );
	if( pWindow ) return pWindow->WindowProc( hWnd, uMsg, wParam, lParam );

	// Set user data if not currently set.
	LPCREATESTRUCT lpStruct = reinterpret_cast< LPCREATESTRUCT >( lParam );
	if( lpStruct && lpStruct->lpCreateParams )
		SetWindowLongPtr( hWnd, GWL_USERDATA, reinterpret_cast< LONG_PTR >( lpStruct->lpCreateParams ) );

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

LRESULT __stdcall CWindow::WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Process mouse actions first.
	if( uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST )
	{
		switch( uMsg )
		{
		case WM_MOUSEMOVE:
			std::for_each( this->m_vChildren.begin(), this->m_vChildren.end(), 
				boost::bind( &IMouse::OnMouseMove, _1, hWnd, wParam, lParam ) 
				);
			return 0;

		case WM_LBUTTONDOWN:
			{
				bool bIsDownOnChild = false;
				BOOST_FOREACH( boost::shared_ptr< CChild > child, this->m_vChildren )
				{
					if( child->OnMouseDown( hWnd, wParam, lParam ) )
					{
						bIsDownOnChild = true;
						break;
					}
				}

				if( bIsDownOnChild )
					return 0;
				break;
			}

		case WM_LBUTTONUP:
			{
				bool bIsDownOnChild = false;
				BOOST_FOREACH( boost::shared_ptr< CChild > child, this->m_vChildren )
				{
					if( child->OnMouseUp( hWnd, wParam, lParam ) )
					{
						bIsDownOnChild = true;
						break;
					}
				}

				if( bIsDownOnChild )
					return 0;
				break;
			}
		}
	}

	switch( uMsg )
	{
	case WM_CREATE:
		SetTimer( hWnd, 9990, 10, 0 );
		SetTimer( hWnd, 9991, 10, 0 );
		return 0;

	case WM_CLOSE:
		KillTimer( hWnd, 9991 );
		SetTimer( hWnd, 9992, 10, 0 );
		return 0;

	case WM_DESTROY:
		KillTimer( hWnd, 9990 );
		KillTimer( hWnd, 9991 );
		KillTimer( hWnd, 9992 );
		KillTimer( hWnd, 9993 );
		PostQuitMessage( 0 );
		return 0;

	case WM_TIMER:
		switch( LOWORD( wParam ) )
		{
		case 9990: // Fade-in Timer
			CPatchLib::instance()->FadeWindowIn();
			KillTimer( hWnd, 9990 );
			SetTimer( hWnd, 9993, 20, 0 );
			return 0;
		case 9991: // Draw Timer
			CPatchLib::instance()->Draw();
			if( GetAsyncKeyState( VK_ESCAPE ) & 0x01 )
				PostMessage( hWnd, WM_CLOSE, 0, 0 );
			return 0;
		case 9992: // Fade-out Timer
			CPatchLib::instance()->FadeWindowOut();
			KillTimer( hWnd, 9992 );
			PostMessage( hWnd, WM_QUIT, 0, 0 );
			return 0;
		case 9993: // Scroller Timer
			this->Update();
			return 0;
		}
		break;
	}

	// Pass events to override functions.
	if( this->m_vWndCallbacks.size() > 0 )
	{
		std::map< UINT, WMCALLBACKSTRUCT >::iterator iter = this->m_vWndCallbacks.find( uMsg );
		if( iter != this->m_vWndCallbacks.end() )
		{
			if( iter->second.Flags == 1 )
				return iter->second.lpCallbackReturn( hWnd, wParam, lParam );
			iter->second.lpCallbackNoReturn( hWnd, wParam, lParam );
		}
	}

	// Internally handle left-mouse last for movement.
	if( uMsg == WM_LBUTTONDOWN )
		return SendMessage( hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0 );

	// Default message handler.
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

//----------------------------------------------------------------------------------
//
// Control handlers.
//
//----------------------------------------------------------------------------------

CChild* CWindow::GetChild( const std::string& strAlias )
{
	std::vector< boost::shared_ptr< CChild > >::iterator iter = std::find_if(
		this->m_vChildren.begin(), this->m_vChildren.end(),
		boost::bind( &CChild::GetAlias, _1 ) == strAlias
		);

	if( iter == this->m_vChildren.end() )
		return NULL;

	return iter->get();
}

bool CWindow::RemoveChild( const std::string& strAlias )
{
	std::vector< boost::shared_ptr< CChild > >::iterator iter = std::find_if(
		this->m_vChildren.begin(), this->m_vChildren.end(),
		boost::bind( &CChild::GetAlias, _1 ) == strAlias
		);

	if( iter == this->m_vChildren.end() )
		return false;

	iter->get()->Release();
	this->m_vChildren.erase( iter );

	return true;
}

bool CWindow::AddLabel( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb )
{
	if( this->GetChild( strAlias ) )
		return false;

	boost::shared_ptr< CLabel > pLabel = boost::shared_ptr< CLabel >( new CLabel );
	if( !pLabel->Initialize( strAlias, strText, ptPoint, rgb ) || !pLabel->Setup() )
	{
		pLabel->Release();
		return false;
	}

	this->m_vChildren.push_back( pLabel );

	return true;
}

bool CWindow::AddUrl( const std::string& strAlias, const std::string& strText, const std::string& strSite, const POINT& ptPoint, const COLORREF rgb, const COLORREF rgbOver )
{
	if( this->GetChild( strAlias ) )
		return false;

	boost::shared_ptr< CUrl > pUrl = boost::shared_ptr< CUrl >( new CUrl );
	if( !pUrl->Initialize( strAlias, strText, strSite, ptPoint, rgb, rgbOver ) || !pUrl->Setup() )
	{
		pUrl->Release();
		return false;
	}

	this->m_vChildren.push_back( pUrl );

	return true;
}

bool CWindow::AddScroller( const std::string& strAlias, const std::string& strText, const POINT& ptPoint, const COLORREF rgb, int nWidth, float fFreq, float fSpeed, int nAmp )
{
	if( this->GetChild( strAlias ) )
		return false;

	boost::shared_ptr< CScroller > pScroller = boost::shared_ptr< CScroller >( new CScroller );
	if( !pScroller->Initialize( strAlias, strText, ptPoint, rgb, nWidth, fFreq, fSpeed, nAmp ) || !pScroller->Setup() )
	{
		pScroller->Release();
		return false;
	}
	
	this->m_vChildren.push_back( pScroller );

	return true;
}

bool CWindow::AddButton( const std::string& strAlias, const POINT& ptPoint, const std::string& strCommand, int nTotalStates, int nButtonResId )
{
	if( this->GetChild( strAlias ) )
		return false;

	boost::shared_ptr< CButton > pButton = boost::shared_ptr< CButton >( new CButton );
	if( !pButton->Initialize( strAlias, ptPoint, strCommand, nTotalStates, nButtonResId ) || !pButton->Setup() )
	{
		pButton->Release();
		return false;
	}

	this->m_vChildren.push_back( pButton );

	return true;
}

//----------------------------------------------------------------------------------
//
// Window procedure overrides.
//
//----------------------------------------------------------------------------------

bool CWindow::SetMessageCallback( UINT uMsg, WMCALLBACKNORETURN pCallback )
{
	std::map< UINT, WMCALLBACKSTRUCT >::iterator iter = this->m_vWndCallbacks.find( uMsg );
	if( iter != this->m_vWndCallbacks.end() ) return false;

	this->m_vWndCallbacks[ uMsg ].lpCallbackNoReturn	= pCallback;
	this->m_vWndCallbacks[ uMsg ].lpCallbackReturn		= NULL;
	this->m_vWndCallbacks[ uMsg ].Flags					= 0;

	return true;
}

bool CWindow::SetMessageCallback( UINT uMsg, WMCALLBACKRETURN pCallback )
{
	std::map< UINT, WMCALLBACKSTRUCT >::iterator iter = this->m_vWndCallbacks.find( uMsg );
	if( iter != this->m_vWndCallbacks.end() ) return false;

	this->m_vWndCallbacks[ uMsg ].lpCallbackNoReturn	= NULL;
	this->m_vWndCallbacks[ uMsg ].lpCallbackReturn		= pCallback;
	this->m_vWndCallbacks[ uMsg ].Flags					= 1;

	return true;
}

bool CWindow::RemoveMessageCallback( UINT uMsg )
{
	std::map< UINT, WMCALLBACKSTRUCT >::iterator iter = this->m_vWndCallbacks.find( uMsg );
	if( iter == this->m_vWndCallbacks.end() ) return false;

	this->m_vWndCallbacks[ uMsg ].lpCallbackNoReturn	= NULL;
	this->m_vWndCallbacks[ uMsg ].lpCallbackReturn		= NULL;
	this->m_vWndCallbacks[ uMsg ].Flags					= 0;
	this->m_vWndCallbacks.erase( iter );

	return true;
}

//----------------------------------------------------------------------------------
//
// Command button callbacks.
//
//----------------------------------------------------------------------------------

bool CWindow::RegisterCommand( const std::string& strCommand, CMDFUNCTION pFunction )
{
	std::map< std::string, CMDCALLBACK >::iterator iter = this->m_vCmdCallbacks.find( strCommand );
	if( iter != this->m_vCmdCallbacks.end() ) return false;

	this->m_vCmdCallbacks[ strCommand ].Command		= strCommand;
	this->m_vCmdCallbacks[ strCommand ].Function	= pFunction;

	return true;
}

bool CWindow::UnregisterCommand( const std::string& strCommand )
{
	std::map< std::string, CMDCALLBACK >::iterator iter = this->m_vCmdCallbacks.find( strCommand );
	if( iter == this->m_vCmdCallbacks.end() ) return false;

	this->m_vCmdCallbacks[ strCommand ].Command		= "";
	this->m_vCmdCallbacks[ strCommand ].Function	= NULL;

	this->m_vCmdCallbacks.erase( iter );

	return true;
}

bool CWindow::CallCommand( const std::string& strCommand, CChild* pChild )
{
	std::map< std::string, CMDCALLBACK >::iterator iter = this->m_vCmdCallbacks.find( strCommand );
	if( iter == this->m_vCmdCallbacks.end() ) return false;

	if( iter->second.Function != NULL )
		( iter->second.Function )( pChild );

	return true;
}

CMDFUNCTION CWindow::GetCommandFunction( const std::string& strCommand )
{
	std::map< std::string, CMDCALLBACK >::iterator iter = this->m_vCmdCallbacks.find( strCommand );
	if( iter == this->m_vCmdCallbacks.end() ) return NULL;

	return iter->second.Function;
}

//----------------------------------------------------------------------------------
//
// Window properties.
//
//----------------------------------------------------------------------------------

HWND CWindow::GetHwnd( void ) const
{
	return this->m_vHwnd;
}

bool CWindow::SetCursor( int nCursorResId )
{
	if( !this->m_vHwnd )
	{
		this->m_vCursorResId = nCursorResId;
		return false;
	}

	// Adjust to default cursor if not set.
	this->m_vCursorResId = ( nCursorResId > 0 ) ? nCursorResId : 32512;
	this->m_vCursor = LoadImage( GetModuleHandle( NULL ), MAKEINTRESOURCE( nCursorResId ), IMAGE_CURSOR, NULL, NULL, LR_DEFAULTSIZE );

	SetClassLongPtr( this->m_vHwnd, GCLP_HCURSOR, reinterpret_cast< LONG_PTR >( this->m_vCursor ) );

	return true;
}

bool CWindow::SetIcon( int nIconResId )
{
	if( !this->m_vHwnd )
	{
		this->m_vIconResId = nIconResId;
		return false;
	}

	// Windows will use the default application icon automatically.
	if( nIconResId == 0 )
	{
		this->m_vIconResId = 0;
		this->m_vIcon = NULL;
		return true;
	}

	this->m_vIconResId = nIconResId;
	this->m_vIcon = LoadImage( GetModuleHandle( NULL ), MAKEINTRESOURCE( nIconResId ), IMAGE_ICON, NULL, NULL, LR_DEFAULTSIZE );
	if( this->m_vIcon )
	{
		SendMessage( this->m_vHwnd, WM_SETICON, ICON_SMALL, reinterpret_cast< LPARAM >( this->m_vIcon ) );
		SendMessage( this->m_vHwnd, WM_SETICON, ICON_BIG, reinterpret_cast< LPARAM >( this->m_vIcon ) );
	}

	return true;
}

void CWindow::SetClass( const std::string& strClass )
{
	if( this->m_vHwnd ) return;
	this->m_vClass = strClass;
}

void CWindow::SetTitle( const std::string& strTitle )
{
	this->m_vTitle = strTitle;
	if( this->m_vHwnd )
		SetWindowText( this->m_vHwnd, this->m_vTitle.c_str() );
}

void CWindow::SetWidth( int nWidth )
{
	this->m_vWidth = nWidth;
	
	if( this->m_vHwnd )
		SetWindowPos( this->m_vHwnd, NULL, NULL, NULL, this->m_vWidth, this->m_vHeight, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER );
}

void CWindow::SetHeight( int nHeight )
{
	this->m_vHeight = nHeight;

	if( this->m_vHwnd )
		SetWindowPos( this->m_vHwnd, NULL, NULL, NULL, this->m_vWidth, this->m_vHeight, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER );
}

void CWindow::SetXPosition( int nXPos )
{
	this->m_vXPosition = nXPos;

	if( this->m_vHwnd )
		SetWindowPos( this->m_vHwnd, NULL, this->m_vXPosition, this->m_vYPosition, NULL, NULL, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER );
}

void CWindow::SetYPosition( int nYPos )
{
	this->m_vYPosition = nYPos;

	if( this->m_vHwnd )
		SetWindowPos( this->m_vHwnd, NULL, this->m_vXPosition, this->m_vYPosition, NULL, NULL, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER );
}

void CWindow::SetStartXPos( int nXPos )
{
	this->m_vStartXPos = nXPos;
}

void CWindow::SetStartYPos( int nYPos )
{
	this->m_vStartYPos = nYPos;
}

void CWindow::SetTopMost( bool bTopMost )
{
	this->m_vTopMost = bTopMost;

	SetWindowPos( this->m_vHwnd,
		( this->m_vTopMost ) ? HWND_TOPMOST : HWND_NOTOPMOST,
		NULL, NULL, NULL, NULL,
		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
		);
}

std::string CWindow::GetClass( void ) const
{
	return this->m_vClass;
}

std::string CWindow::GetTitle( void ) const
{
	return this->m_vTitle;
}

int CWindow::GetWidth( void ) const
{
	return this->m_vWidth;
}

int CWindow::GetHeight( void ) const
{
	return this->m_vHeight;
}

int CWindow::GetXPosition( void ) const
{
	return this->m_vXPosition;
}

int CWindow::GetYPosition( void ) const
{
	return this->m_vYPosition;
}

int CWindow::GetStartXPos( void ) const
{
	return this->m_vStartXPos;
}

int CWindow::GetStartYPos( void ) const
{
	return this->m_vStartYPos;
}

bool CWindow::GetTopMost( void ) const
{
	return this->m_vTopMost;
}
