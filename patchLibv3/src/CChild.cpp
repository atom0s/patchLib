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

#include "../include/CChild.h"

//----------------------------------------------------------------------------------
//
// Construction
//
//----------------------------------------------------------------------------------

CChild::CChild( void )
{
	this->m_vChildAlias			= "";
	this->m_vChildRect.bottom	= 0;
	this->m_vChildRect.left		= 0;
	this->m_vChildRect.right	= 0;
	this->m_vChildRect.top		= 0;
}
CChild::~CChild( void )
{
}

void CChild::SetAlias( const std::string& strAlias )
{
	this->m_vChildAlias = strAlias;
}
std::string CChild::GetAlias( void ) const
{
	return this->m_vChildAlias;
}

void CChild::SetRect( const TRECT<int>& rect )
{
	memcpy( &this->m_vChildRect, &rect, sizeof( TRECT<int> ) );
}
void CChild::GetRect( TRECT<int>& rect ) const
{
	memcpy( &rect, &this->m_vChildRect, sizeof( TRECT<int> ) );
}
TRECT<int> CChild::GetRect( void ) const
{
	return this->m_vChildRect;
}