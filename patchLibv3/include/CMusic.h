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

#ifndef __PATCHLIB_CMUSIC_H_INCLUDED__
#define __PATCHLIB_CMUSIC_H_INCLUDED__

#include <Windows.h>

class TiXmlElement;

/// <summary>
/// CMusic
///
/// Music object class. Uses uFMOD to play chiptune music
/// inside of patchLib.
/// </summary>
class CMusic
{
	int		m_vMusicResId;
	bool	m_vMusicLoaded;
	bool	m_vMusicPaused;
	
public: // Construction
	CMusic( void );
	~CMusic( void );

public: // Initialization
	bool	Initialize( TiXmlElement* pElement );
	bool	Initialize( int nResId );
	bool	Release( void );

public: // Handlers
	bool	Play( void );
	bool	Pause( void );
	bool	Stop( void );
};

#endif // __PATCHLIB_CMUSIC_H_INCLUDED__