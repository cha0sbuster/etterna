#include "global.h"
/*
-----------------------------------------------------------------------------
 File: Banner.h

 Desc: The song's banner displayed in SelectSong.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
-----------------------------------------------------------------------------
*/

#include "Banner.h"
#include "PrefsManager.h"
#include "SongManager.h"
#include "RageBitmapTexture.h"
#include "ThemeManager.h"
#include "RageUtil.h"
#include "song.h"
#include "RageTextureManager.h"

RageTextureID Banner::BannerTex( RageTextureID ID )
{
	/* Song banners often have HOT PINK color keys. */
	ID.bHotPinkColorKey = true;
	ID.bDither = true;
	return ID;
}

Banner::Banner()
{
	m_bScrolling = false;
	m_fPercentScrolling = 0;

	TEXTUREMAN->CacheTexture( BannerTex(THEME->GetPathToG("Banner all music")) );
	TEXTUREMAN->CacheTexture( BannerTex(THEME->GetPathToG("Common fallback banner")) );
	TEXTUREMAN->CacheTexture( BannerTex(THEME->GetPathToG("Banner roulette")) );
	TEXTUREMAN->CacheTexture( BannerTex(THEME->GetPathToG("Banner random")) );
}

bool Banner::Load( RageTextureID ID )
{
	ID = BannerTex(ID);

	m_fPercentScrolling = 0;
	m_bScrolling = false;

	TEXTUREMAN->VolatileTexture( ID );
	return Sprite::Load( ID );
};

void Banner::Update( float fDeltaTime )
{
	Sprite::Update( fDeltaTime );

	if( m_bScrolling )
	{
        m_fPercentScrolling += fDeltaTime/2;
		m_fPercentScrolling -= (int)m_fPercentScrolling;

		const RectF *pTextureRect = m_pTexture->GetTextureCoordRect(0);
 
		float fTexCoords[8] = 
		{
			0+m_fPercentScrolling, pTextureRect->top,		// top left
			0+m_fPercentScrolling, pTextureRect->bottom,	// bottom left
			1+m_fPercentScrolling, pTextureRect->bottom,	// bottom right
			1+m_fPercentScrolling, pTextureRect->top,		// top right
		};
		Sprite::SetCustomTextureCoords( fTexCoords );
	}
}

void Banner::SetScrolling( bool bScroll, float Percent)
{
	m_bScrolling = bScroll;
	m_fPercentScrolling = Percent;

	/* Set up the texture coord rects for the current state. */
	Update(0);
}

void Banner::LoadFromSong( Song* pSong )		// NULL means no song
{
	Sprite::EnableShadow( false );

	if( pSong == NULL )					LoadFallback();
	else if( pSong->HasBanner() )		Load( pSong->GetBannerPath() );
	else								LoadFallback();

	m_bScrolling = false;
}

void Banner::LoadAllMusic()
{
	Load( THEME->GetPathToG("Banner all") );
	m_bScrolling = false;
}

void Banner::LoadFromGroup( CString sGroupName )
{
	CString sGroupBannerPath = SONGMAN->GetGroupBannerPath( sGroupName );
	if( sGroupBannerPath != "" )	Load( sGroupBannerPath );
	else							LoadFallback();
	m_bScrolling = false;
}

void Banner::LoadFromCourse( Course* pCourse )		// NULL means no course
{
	if( pCourse == NULL )						LoadFallback();
	else if( pCourse->m_sBannerPath != "" )		Load( pCourse->m_sBannerPath );
	else										LoadFallback();

	m_bScrolling = false;
}

void Banner::LoadFallback()
{
	Load( THEME->GetPathToG("Common fallback banner") );
}

void Banner::LoadRoulette()
{
	Load( THEME->GetPathToG("Banner roulette") );
	m_bScrolling = true;
}

void Banner::LoadRandom()
{
	Load( THEME->GetPathToG("Banner random") );
	m_bScrolling = true;
}

void Banner::ScaleToClipped( float fWidth, float fHeight )
{
	m_fRememberedClipWidth = fWidth;
	m_fRememberedClipHeight = fHeight;
}
