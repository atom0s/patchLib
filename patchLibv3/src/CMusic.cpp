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

#pragma comment( lib, "WinMM.lib" )

#include "../include/CMusic.h"

#include "../ufmod/ufmod.h"
#include "../tinyxml/tinyxml.h"

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CMusic::CMusic( void )
{
	this->m_vMusicResId		= 0;
	this->m_vMusicLoaded	= false;
	this->m_vMusicPaused	= false;
}
CMusic::~CMusic( void )
{
	this->Release();
}

//----------------------------------------------------------------------------------
//
// Initialization
//
//----------------------------------------------------------------------------------

bool CMusic::Initialize( TiXmlElement* pElement )
{
	pElement->QueryIntAttribute( "resId", &this->m_vMusicResId );
	
	return true;
}

bool CMusic::Initialize( int nResId )
{
	this->m_vMusicResId = nResId;

	return true;
}

bool CMusic::Release( void )
{
	uFMOD_StopSong();
	return true;
}

//----------------------------------------------------------------------------------
//
// Handlers
// 
//----------------------------------------------------------------------------------

bool CMusic::Play( void )
{
	if( this->m_vMusicResId == 0 )
		return false;

	if( !this->m_vMusicLoaded )
	{
		this->m_vMusicLoaded = true;
		if( !uFMOD_PlaySong( MAKEINTRESOURCE( this->m_vMusicResId ), 0, XM_RESOURCE ) )
		{
			this->m_vMusicLoaded = false;
			return false;
		}
		return true;
	}

	return this->Pause();
}

bool CMusic::Pause( void )
{
	if( !this->m_vMusicLoaded )
		return false;

	// Toggle paused state.
	this->m_vMusicPaused = !this->m_vMusicPaused;
	( this->m_vMusicPaused ) ? uFMOD_Pause() : uFMOD_Resume();

	return true;
}

bool CMusic::Stop( void )
{
	if( !this->m_vMusicLoaded )
		return false;

	this->m_vMusicPaused = false;
	uFMOD_StopSong();
	return true;
}
