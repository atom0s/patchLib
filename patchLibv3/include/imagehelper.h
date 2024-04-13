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
#pragma warning( disable: 4505 ) // Unreferenced local function.

#ifndef __IMAGEHELPER_H_INCLUDED__
#define __IMAGEHELPER_H_INCLUDED__

#include <Windows.h>
#include "../pnglib/PNGlib.h"


/// <summary>
///	LoadPngFromResource
///
/// Loads a png image from the applications resource file. This
/// function automatically converts the png to a bitmap object
/// as well as flips the image properly.
/// 
/// Credits to:
///		- atom0s
///		- Ufo-pu55y
/// </summary>
/// <param name="hWnd"></param>
/// <param name="iPngResId"></param>
/// <param name="pBitmap"></param>
/// <param name="pSize"></param>
/// <returns></returns>
static bool LoadPngFromResource( HWND hWnd, int iPngResId, HBITMAP* pBitmap, SIZE* pSize )
{
	PNGINFO pngInfo = { 0 };
	BITMAP	bmpTemp = { 0 };

	if( !PNG_Init( &pngInfo ) )
		return false;

	if( !PNG_LoadResource( &pngInfo, GetModuleHandle( NULL ), reinterpret_cast< char* >( iPngResId ) ) )
	{
		PNG_Cleanup( &pngInfo );
		return false;
	}

	if( !PNG_Decode( &pngInfo ) )
	{
		PNG_Cleanup( &pngInfo );
		return false;
	}

	*(HBITMAP*)pBitmap = PNG_CreateBitmap( &pngInfo, hWnd, (DWORD)PNG_OUTF_AUTO, FALSE );

	pSize->cx = pngInfo.iWidth;
	pSize->cy = pngInfo.iHeight;

	PNG_Cleanup( &pngInfo );

	/**
	 * Flip image vertically to prevent upside-down images.
	 * Thanks to: Ufo-pu55y (Converted to C++ by atom0s)
	 */

	GetObject( *(HBITMAP*)pBitmap, sizeof( bmpTemp ), &bmpTemp );

	HANDLE hHeapHandle = HeapCreate( HEAP_CREATE_ENABLE_EXECUTE, ( ( bmpTemp.bmWidth * bmpTemp.bmHeight ) * 4 ), NULL );
	if( hHeapHandle == NULL )
	{
		return false;
	}

	LPVOID lpHeapAlloc = HeapAlloc( hHeapHandle, HEAP_ZERO_MEMORY, ( ( bmpTemp.bmWidth * bmpTemp.bmHeight ) * 4 ) );
	if( lpHeapAlloc == NULL )
	{
		HeapDestroy( hHeapHandle );
		return false;
	}

	BYTE* pOrg = reinterpret_cast< BYTE* >( bmpTemp.bmBits );
	BYTE* pMem = reinterpret_cast< BYTE* >( lpHeapAlloc );

	for( int x = bmpTemp.bmHeight; x > 0; x-- )
	{
		pOrg += ( bmpTemp.bmWidth * 4 ) - 4;
		for( int y = bmpTemp.bmWidth; y > 0; y-- )
		{
			memcpy( pMem, pOrg, 4 );
			pOrg -= 4;
			pMem += 4;
		}
		pOrg += ( bmpTemp.bmWidth * 4 ) + 4;
	}

	pOrg -= 4;
	pMem = reinterpret_cast< BYTE* >( lpHeapAlloc );

	for( int x = ( bmpTemp.bmWidth * bmpTemp.bmHeight ); x > 0; x-- )
	{
		memcpy( pOrg, pMem, 4 );
		pMem += 4;
		pOrg -= 4;
	}

	HeapFree( hHeapHandle, 0, lpHeapAlloc );
	HeapDestroy( hHeapHandle );

	return true;
}

/// <summary>
///	BitmapToBitmap
///
/// Copies a bitmap into another bitmap.
/// </summary>
/// <param name="hbDesc"></param>
/// <param name="hbSrc"></param>
/// <returns></returns>
static bool BitmapToBitmap( HBITMAP hbDest, HBITMAP hbSrc )
{
	BITMAP bmpDest	= { 0 };
	BITMAP bmpSrc	= { 0 };

	if( GetObject( hbDest, sizeof( BITMAP ), &bmpDest ) == 0 )
		return false;
	if( GetObject( hbSrc, sizeof( BITMAP ), &bmpSrc ) == 0 )
		return false;

	if( bmpDest.bmBits == NULL || bmpSrc.bmBits == NULL )
		return false;

	for( int x = ( bmpSrc.bmWidth * bmpSrc.bmHeight ); x >= 0; x-- )
	{
		memcpy( reinterpret_cast< DWORD* >( bmpDest.bmBits ) + x,
				reinterpret_cast< DWORD* >( bmpSrc.bmBits ) + x,
				4 );
	}

	return true;
}

/// <summary>
///	PremultipliedAlpha
///
/// Applies a premultipled alpha effect to the
/// given bitmap creating transparent effects.
/// 
/// Credits to:
///		Wine Emulation Project
/// </summary>
/// <param name="hbSrc"></param>
/// <returns></returns>
static bool PremultipliedAlpha( HBITMAP hbSrc )
{
	BITMAP bmpTemp = { 0 };

	if( GetObject( hbSrc, sizeof( BITMAP ), &bmpTemp ) == 0 )
		return false;

	if( bmpTemp.bmBitsPixel != 32 )
		return false;

	BYTE* pBmp = reinterpret_cast< BYTE* >( bmpTemp.bmBits );
	int nSize = abs( bmpTemp.bmHeight ) * bmpTemp.bmWidth;
	
	while( nSize-- > 0 )
	{
		int nAlpha = pBmp[3];
		pBmp[0] = static_cast< BYTE >( ( pBmp[0] * nAlpha ) >> 8 );
		pBmp[1] = static_cast< BYTE >( ( pBmp[1] * nAlpha ) >> 8 );
		pBmp[2] = static_cast< BYTE >( ( pBmp[2] * nAlpha ) >> 8 );

		pBmp += 4;
	}

	return true;
}


#ifdef PATCHLIB_USE_GDIPLUS
/// <summary>
/// ResourceToGdiPlus
///
/// Converts a resource image to a GdiPlus graphics image.
/// This function uses Gdiplus!
/// 
/// Credits:
///		- atom0s
///		- Joe Woodbury
/// </summary>
/// <param name="ppImage"></param>
/// <param name="nResourceId"></param>
/// <param name="lpszType"></param>
/// <returns></returns>
static bool ResourceToGdiPlus( Image** ppImage, int nResourceId, const char* lpszType )
{
	// Validate params.
	if( !ppImage || !nResourceId || !lpszType )
		return false;

	// Locate resource in current application.
	HRSRC hResource = FindResource( NULL, MAKEINTRESOURCE( nResourceId ), lpszType );
	if( !hResource ) 
		return false;

	// Load the resource.
	HGLOBAL hGlobal = LoadResource( NULL, hResource );
	if( !hGlobal ) 
		return false;

	// Obtain pointer to resource data.
	LPVOID lpResource = LockResource( hGlobal );
	if( !lpResource ) 
		return false;

	// Obtain size of resource data.
	DWORD dwResourceSize = SizeofResource( NULL, hResource );
	if( dwResourceSize == 0 )
		return false;

	// Allocate block of memory to copy resource data.
	HGLOBAL hGlobalAlloc = GlobalAlloc( GMEM_MOVEABLE, dwResourceSize + 1 );
	if( !hGlobalAlloc ) 
		return false;

	// Obtain pointer to global block.
	LPVOID lpMemAlloc = GlobalLock( hGlobalAlloc );
	if( !lpMemAlloc )
	{
		GlobalFree( hGlobalAlloc );
		return false;
	}

	// Copy and unlock.
	memcpy( lpMemAlloc, lpResource, dwResourceSize );
	GlobalUnlock( hGlobalAlloc );

	// Create Gdiplus Stream from global memory.
	IStream* pStream = NULL;
	if( FAILED( CreateStreamOnHGlobal( hGlobalAlloc, FALSE, &pStream ) ) )
	{
		GlobalFree( hGlobalAlloc );
		return false;
	}

	// Convert stream to image.
	*ppImage = new Image( pStream, NULL );

	// Cleanup objects.
	GlobalFree( hGlobalAlloc );
	pStream->Release();

	// Validate creation.
	if( !ppImage || ! *ppImage || !(*ppImage)->GetLastStatus() == Gdiplus::Ok )
	{
		if( *ppImage )
			delete *ppImage;
		*ppImage = NULL;
		return false;
	}
	return true;
}
#endif

#endif // __IMAGEHELPER_H_INCLUDED__