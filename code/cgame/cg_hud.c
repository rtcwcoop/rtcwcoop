/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (RTCW SP Source Code).

RTCW SP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW SP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW SP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW SP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW SP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/



#include "cg_local.h"
#include "../ui/ui_shared.h"

extern displayContextDef_t cgDC;

// set in CG_ParseTeamInfo

int drawTeamOverlayModificationCount = -1;

static void CG_DrawPlayerArmorValue( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	char num[16];
	int value;
	playerState_t   *ps;

	ps = &cg.snap->ps;

	value = ps->stats[STAT_ARMOR];

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_LEFT, PLACE_BOTTOM);
	}

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, font, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
	}
}

/*
==============
weapIconDrawSize
==============
*/
static int weapIconDrawSize( int weap ) {
	switch ( weap ) {

	// weapons to not draw
	case WP_KNIFE:
		return 0;

	// weapons with 'wide' icons
	case WP_THOMPSON:
	case WP_MP40:
	case WP_STEN:
	case WP_MAUSER:
	case WP_GARAND:
	case WP_VENOM:
	case WP_TESLA:
	case WP_PANZERFAUST:
	case WP_FLAMETHROWER:
	case WP_FG42:
	case WP_FG42SCOPE:
	case WP_SNOOPERSCOPE:
	case WP_SNIPERRIFLE:
		return 2;
	}

	return 1;
}


/*
==============
CG_DrawPlayerWeaponIcon
==============
*/
static void CG_DrawPlayerWeaponIcon( rectDef_t *rect, qboolean drawHighlighted, int align ) {
	int size;
	int realweap;                   // DHM - Nerve
	qhandle_t icon;
	float scale,halfScale;

	if ( !cg_drawIcons.integer ) {
		return;
	}

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_RIGHT, PLACE_BOTTOM);
	}

	// DHM - Nerve :: special case for WP_CLASS_SPECIAL

	realweap = cg.predictedPlayerState.weapon;

	size = weapIconDrawSize( realweap );

	if ( !size ) {
		return;
	}

	if ( drawHighlighted ) {
		icon = cg_weapons[ realweap ].weaponIcon[1];
	} else {
		icon = cg_weapons[ realweap ].weaponIcon[0];
	}



	// pulsing grenade icon to help the player 'count' in their head
	if ( cg.predictedPlayerState.grenadeTimeLeft ) {   // grenades and dynamite set this

		// these time differently
		if ( realweap == WP_DYNAMITE ) {
			if ( ( ( cg.grenLastTime ) % 1000 ) > ( ( cg.predictedPlayerState.grenadeTimeLeft ) % 1000 ) ) {
				trap_S_StartLocalSound( cgs.media.grenadePulseSound4, CHAN_LOCAL_SOUND );
			}
		} else {
			if ( ( ( cg.grenLastTime ) % 1000 ) < ( ( cg.predictedPlayerState.grenadeTimeLeft ) % 1000 ) ) {
				switch ( cg.predictedPlayerState.grenadeTimeLeft / 1000 ) {
				case 3:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound4, CHAN_LOCAL_SOUND );
					break;
				case 2:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound3, CHAN_LOCAL_SOUND );
					break;
				case 1:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound2, CHAN_LOCAL_SOUND );
					break;
				case 0:
					trap_S_StartLocalSound( cgs.media.grenadePulseSound1, CHAN_LOCAL_SOUND );
					break;
				}
			}
		}

		scale = (float)( ( cg.predictedPlayerState.grenadeTimeLeft ) % 1000 ) / 100.0f;
		halfScale = scale * 0.5f;

		cg.grenLastTime = cg.predictedPlayerState.grenadeTimeLeft;
	} else {
		scale = halfScale = 0;
	}


	if ( icon ) {
		float x, y, w, h;

		if ( size == 1 ) { // draw half width to match the icon asset

			// start at left
			x = rect->x - halfScale;
			y = rect->y - halfScale;
			w = rect->w / 2 + scale;
			h = rect->h + scale;

			switch ( align ) {
			case ITEM_ALIGN_CENTER:
				x += rect->w / 4;
				break;
			case ITEM_ALIGN_RIGHT:
				x += rect->w / 2;
				break;
			case ITEM_ALIGN_LEFT:
			default:
				break;
			}

		} else {
			x = rect->x - halfScale;
			y = rect->y - halfScale;
			w = rect->w + scale;
			h = rect->h + scale;
		}


		CG_DrawPic( x, y, w, h, icon );
	}
}


/*
==============
CG_DrawPlayerAmmoIcon
==============
*/
static void CG_DrawPlayerAmmoIcon( rectDef_t *rect, qboolean draw2D ) {
	centity_t   *cent;
	vec3_t angles;
	vec3_t origin;

	cent = &cg_entities[cg.snap->ps.clientNum];

	// TTimo: gcc: suggests () around && within ||
	if ( draw2D || ( !cg_draw3dIcons.integer && cg_drawIcons.integer ) ) {
		qhandle_t icon;
		icon = cg_weapons[ cg.predictedPlayerState.weapon ].ammoIcon;
		if ( icon ) {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, icon );
		}
	} else if ( cg_draw3dIcons.integer ) {
		if ( cent->currentState.weapon && cg_weapons[ cent->currentState.weapon ].ammoModel ) {
			VectorClear( angles );
			origin[0] = 70;
			origin[1] = 0;
			origin[2] = 0;
			angles[YAW] = 90 + 20 * sin( cg.time / 1000.0 );
			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cg_weapons[ cent->currentState.weapon ].ammoModel, 0, origin, angles );
		}
	}
}

#define CURSORHINT_SCALE    10

/*
==============
CG_DrawCursorHints

  cg_cursorHints.integer ==
    0:	no hints
    1:	sin size pulse
    2:	one way size pulse
    3:	alpha pulse
    4+:	static image

==============
*/

static void CG_DrawCursorhint( rectDef_t *rect ) {
	float       *color;
	qhandle_t icon, icon2 = 0;
	float scale, halfscale;
	qboolean redbar = qfalse;

	if ( !cg_cursorHints.integer ) {
		return;
	}

	CG_CheckForCursorHints();

	icon = cgs.media.hintShaders[cg.cursorHintIcon];

	if ( cg_fixedAspect.integer ) {
		CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);
	}

	switch ( cg.cursorHintIcon ) {
	case HINT_NONE:
	case HINT_FORCENONE:
		icon = 0;
		break;
	//case HINT_KNIFE:
	//        icon = cgs.media.hintShaders[HINT_KNIFE];
	//        break;
	case HINT_BREAKABLE_DYNAMITE:
		icon = cgs.media.hintShaders[HINT_BREAKABLE];
		break;
	case HINT_CHAIR:
		// only show 'pickupable' if you're not armed, or are armed with a single handed weapon
		if ( cg.predictedPlayerState.weapon && !( WEAPS_ONE_HANDED & ( 1 << ( cg.predictedPlayerState.weapon ) ) ) ) { // (SA) this was backwards
			icon = cgs.media.hintShaders[HINT_NOACTIVATE];
		}
		break;

	case HINT_PLAYER:
		icon = cgs.media.hintShaders[HINT_NOACTIVATE];
		break;

	case HINT_PLYR_FRIEND:
		break;

	case HINT_NOEXIT_FAR:
		redbar = qtrue;     // draw the status bar in red to show that you can't exit yet
	case HINT_EXIT_FAR:
		break;

	case HINT_NOEXIT:
		redbar = qtrue;     // draw the status bar in red to show that you can't exit yet
	case HINT_EXIT:
		cg.exitStatsFade = 250;     // fade /up/ time
		if ( !cg.exitStatsTime ) {
			cg.exitStatsTime = cg.time;
		}
		CG_DrawExitStats();
		break;

	default:
		break;
	}

	if ( !icon ) {
		return;
	}

	// color
	color = CG_FadeColor( cg.cursorHintTime, cg.cursorHintFade );
	if ( !color ) {
		trap_R_SetColor( NULL );
		cg.exitStatsTime = 0;   // exit stats will fade up next time they're hit
		cg.cursorHintIcon = HINT_NONE;  // clear the hint
		return;
	}

	if ( cg_cursorHints.integer == 3 ) {
		color[3] *= 0.5 + 0.5 * sin( (float)cg.time / 150.0 );
	}

	// size
	if ( cg_cursorHints.integer >= 3 ) {   // no size pulsing
		scale = halfscale = 0;
	} else {
		if ( cg_cursorHints.integer == 2 ) {
			scale = (float)( ( cg.cursorHintTime ) % 1000 ) / 100.0f; // one way size pulse
		} else {
			scale = CURSORHINT_SCALE * ( 0.5 + 0.5 * sin( (float)cg.time / 150.0 ) ); // sin pulse

		}
		halfscale = scale * 0.5f;
	}

	// set color and draw the hint
	trap_R_SetColor( color );
	CG_DrawPic( rect->x - halfscale, rect->y - halfscale, rect->w + scale, rect->h + scale, icon );

	if ( icon2 ) {
		CG_DrawPic( rect->x - halfscale, rect->y - halfscale, rect->w + scale, rect->h + scale, icon2 );
	}

	trap_R_SetColor( NULL );

	// draw status bar under the cursor hint
	if ( cg.cursorHintValue && ( !( cg.time - cg.cursorHintTime ) ) ) {    // don't fade bar out w/ hint icon
		if ( redbar ) {
			Vector4Set( color, 1, 0, 0, 0.5f );
		} else {
			Vector4Set( color, 0, 0, 1, 0.5f );
		}
		CG_FilledBar( rect->x, rect->y + rect->h + 4, rect->w, 8, color, NULL, NULL, (float)cg.cursorHintValue / 255.0f, 0 );
	}

}

/*
==============
CG_DrawMessageIcon
==============
*/
//----(SA)	added

static void CG_DrawMessageIcon( rectDef_t *rect ) {
	int icon;

	if ( !cg_youGotMail.integer ) {
		return;
	}

	if ( cg_youGotMail.integer == 2 ) {
		icon = cgs.media.youGotObjectiveShader;
	} else {
		icon = cgs.media.youGotMailShader;
	}

	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, icon );
}



/*
==============
CG_DrawPlayerAmmoValue
    0 - ammo
    1 - clip
==============
*/
static void CG_DrawPlayerAmmoValue( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle, int type ) {
	char num[16];
	int value, value2 = 0;
	centity_t   *cent;
	playerState_t   *ps;
	int weap;
	qboolean special = qfalse;

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	weap = cent->currentState.weapon;

	if ( !weap ) {
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOADING && type != 0 ) {
		return;
	}

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_RIGHT, PLACE_BOTTOM);
	}

	switch ( weap ) {      // some weapons don't draw ammo count text
	//case WP_KNIFE:
	case WP_CLASS_SPECIAL:              // DHM - Nerve
		return;

	case WP_KNIFE:
		if ( type == 1 ) {  // don't draw reserve value, just clip (since these weapons have all their ammo in the clip)
			return;
		}
		break;
	case WP_AKIMBO:
		special = qtrue;
		break;

	case WP_GRENADE_LAUNCHER:
	case WP_GRENADE_PINEAPPLE:
	case WP_DYNAMITE:
	case WP_TESLA:
	case WP_FLAMETHROWER:
		if ( type == 0 ) {  // don't draw reserve value, just clip (since these weapons have all their ammo in the clip)
			return;
		}
		break;

	default:
		break;
	}


	if ( type == 0 ) { // ammo
		value = cg.snap->ps.ammo[BG_FindAmmoForWeapon( weap )];
	} else {        // clip
		value = ps->ammoclip[BG_FindClipForWeapon( weap )];
		if ( special ) {
			value2 = value;
			if ( weapAlts[weap] ) {
				value = ps->ammoclip[weapAlts[weap]];
			}
//				value2 = ps->ammoclip[weapAlts[weap]];
		}
	}

	if ( value > -1 ) {
		if ( shader ) {
			trap_R_SetColor( color );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
			trap_R_SetColor( NULL );
		} else {
			Com_sprintf( num, sizeof( num ), "%i", value );
			value = CG_Text_Width( num, font, scale, 0 );
			// Moved this up a little so it's not on top of the weapon heat bar
			if ( type == 0 ) {
				CG_Text_Paint( rect->x + ( rect->w - value ) / 2, - 15 + rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
			} else {
				CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
			}

//			if(special) {	// draw '0' for akimbo guns
			if ( value2 || ( special && type == 1 ) ) {
				Com_sprintf( num, sizeof( num ), "%i /", value2 );
				value = CG_Text_Width( num, font, scale, 0 );
				CG_Text_Paint( -30 + rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
			}
		}
	}
}

static void CG_DrawPlayerHead( rectDef_t *rect, qboolean draw2D ) {
	vec3_t angles;
	float size, stretch;
	float frac;
	float x = rect->x;

	VectorClear( angles );

	if ( cg.damageTime && cg.time - cg.damageTime < DAMAGE_TIME ) {
		frac = (float)( cg.time - cg.damageTime ) / DAMAGE_TIME;
		size = rect->w * 1.25 * ( 1.5 - frac * 0.5 );

		stretch = size - rect->w * 1.25;
		// kick in the direction of damage
		x -= stretch * 0.5 + cg.damageX * stretch * 0.5;

		cg.headStartYaw = 180 + cg.damageX * 45;

		cg.headEndYaw = 180 + 20 * cos( crandom() * M_PI );
		cg.headEndPitch = 5 * cos( crandom() * M_PI );

		cg.headStartTime = cg.time;
		cg.headEndTime = cg.time + 100 + random() * 2000;
	} else {
		if ( cg.time >= cg.headEndTime ) {
			// select a new head angle
			cg.headStartYaw = cg.headEndYaw;
			cg.headStartPitch = cg.headEndPitch;
			cg.headStartTime = cg.headEndTime;
			cg.headEndTime = cg.time + 100 + random() * 2000;

			cg.headEndYaw = 180 + 20 * cos( crandom() * M_PI );
			cg.headEndPitch = 5 * cos( crandom() * M_PI );
		}
	}

	// if the server was frozen for a while we may have a bad head start time
	if ( cg.headStartTime > cg.time ) {
		cg.headStartTime = cg.time;
	}

	frac = ( cg.time - cg.headStartTime ) / (float)( cg.headEndTime - cg.headStartTime );
	frac = frac * frac * ( 3 - 2 * frac );
	angles[YAW] = cg.headStartYaw + ( cg.headEndYaw - cg.headStartYaw ) * frac;
	angles[PITCH] = cg.headStartPitch + ( cg.headEndPitch - cg.headStartPitch ) * frac;

	CG_DrawHead( x, rect->y, rect->w, rect->h, cg.snap->ps.clientNum, angles );
}

static void CG_DrawPlayerLocation( rectDef_t *rect, int font, float scale, vec4_t color, int textStyle  ) {
	clientInfo_t *ci = &cgs.clientinfo[cg.snap->ps.clientNum];
	if ( ci ) {
		const char *p = CG_ConfigString( CS_LOCATIONS + ci->location );
		if ( !p || !*p ) {
			p = "unknown";
		}
		CG_Text_Paint( rect->x, rect->y + rect->h, font, scale, color, p, 0, 0, textStyle );
	}
}

static void CG_DrawPlayerScore( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	char num[16];
	int value = cg.snap->ps.persistant[PERS_SCORE];

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, font, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
	}
}


static void CG_DrawHoldableItem( rectDef_t *rect, int font, float scale, qboolean draw2D ) {
	int value;
	gitem_t *item;

	item    = BG_FindItemForHoldable( cg.holdableSelect );

	if ( !item ) {
		return;
	}

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_RIGHT, PLACE_CENTER);
	}

	value   = cg.predictedPlayerState.holdable[cg.holdableSelect];

	if ( value ) {
		CG_RegisterItemVisuals( item - bg_itemlist );

		if ( cg.holdableSelect == HI_WINE ) {
			if ( value > 3 ) {
				value = 3;  // 3 stages to icon, just draw full if beyond 'full'
			}
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[item - bg_itemlist].icons[2 - ( value - 1 )] );
		} else {
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[item - bg_itemlist].icons[0] );
		}
	}
}

static void CG_DrawPlayerItem( rectDef_t *rect, int font, float scale, qboolean draw2D ) {
	int value;
	vec3_t origin, angles;

	value = cg.snap->ps.stats[STAT_HOLDABLE_ITEM];

	if ( value ) {
		CG_RegisterItemVisuals( value );

		if ( qtrue ) {
			CG_RegisterItemVisuals( value );
			CG_DrawPic( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].icons[0] );
		} else {
			VectorClear( angles );
			origin[0] = 90;
			origin[1] = 0;
			origin[2] = -10;
			angles[YAW] = ( cg.time & 2047 ) * 360 / 2048.0;
			CG_Draw3DModel( rect->x, rect->y, rect->w, rect->h, cg_items[ value ].models[0], 0, origin, angles );
		}
	}
}

static void CG_DrawPlayerSpawnpoints( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerState_t   *ps;
	int value;
	char num[16];

	ps = &cg.snap->ps;

	value = ps->persistant[PERS_SPAWNPOINTS_LEFT] + 1;

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, font, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
	}

}

static void CG_DrawPlayerLives( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerState_t   *ps;
	int value;
	char num[16];

	ps = &cg.snap->ps;

	value = ps->persistant[PERS_RESPAWNS_LEFT] + 1;

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, font, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
	}
}


static void CG_DrawPlayerHealth( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	playerState_t   *ps;
	int value;
	char num[16];

	ps = &cg.snap->ps;

	value = ps->stats[STAT_HEALTH];

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_LEFT, PLACE_BOTTOM);
	}

	if ( shader ) {
		trap_R_SetColor( color );
		CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );
		trap_R_SetColor( NULL );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", value );
		value = CG_Text_Width( num, font, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
	}
}


static void CG_DrawRedScore( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int value;
	char num[16];
	if ( cgs.scores1 == SCORE_NOT_PRESENT ) {
		Com_sprintf( num, sizeof( num ), "-" );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", cgs.scores1 );
	}
	value = CG_Text_Width( num, font, scale, 0 );
	CG_Text_Paint( rect->x + rect->w - value, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
}

static void CG_DrawBlueScore( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int value;
	char num[16];

	if ( cgs.scores2 == SCORE_NOT_PRESENT ) {
		Com_sprintf( num, sizeof( num ), "-" );
	} else {
		Com_sprintf( num, sizeof( num ), "%i", cgs.scores2 );
	}
	value = CG_Text_Width( num, font, scale, 0 );
	CG_Text_Paint( rect->x + rect->w - value, rect->y + rect->h, font, scale, color, num, 0, 0, textStyle );
}

static void CG_DrawTeamColor( rectDef_t *rect, vec4_t color ) {
	CG_DrawTeamBackground( rect->x, rect->y, rect->w, rect->h, color[3], cg.snap->ps.persistant[PERS_TEAM] );
}

float CG_GetValue( int ownerDraw, int type ) {
	centity_t   *cent;
	playerState_t   *ps;

	cent = &cg_entities[cg.snap->ps.clientNum];
	ps = &cg.snap->ps;

	switch ( ownerDraw ) {

	case CG_PLAYER_ARMOR_VALUE:
		return ps->stats[STAT_ARMOR];
		break;
	case CG_PLAYER_AMMO_VALUE:
		if ( cent->currentState.weapon ) {
			if ( type == RANGETYPE_RELATIVE ) {
				int weap = BG_FindAmmoForWeapon( cent->currentState.weapon );
				return (float)ps->ammo[weap] / (float)ammoTable[weap].maxammo;
			} else {
				return ps->ammo[BG_FindAmmoForWeapon( cent->currentState.weapon )];
			}
		}
		break;
	case CG_PLAYER_AMMOCLIP_VALUE:
		if ( cent->currentState.weapon ) {
			if ( type == RANGETYPE_RELATIVE ) {
				return (float)ps->ammoclip[BG_FindClipForWeapon( cent->currentState.weapon )] / (float)ammoTable[cent->currentState.weapon].maxclip;
			} else {
				return ps->ammoclip[BG_FindClipForWeapon( cent->currentState.weapon )];
			}
		}
		break;
	case CG_PLAYER_SCORE:
		return cg.snap->ps.persistant[PERS_SCORE];
		break;
	case CG_PLAYER_LIVES:
		return ps->persistant[PERS_RESPAWNS_LEFT];
		break;
	case CG_PLAYER_SPAWNPOINTS:
		return ps->persistant[PERS_SPAWNPOINTS_LEFT];
		break;
	case CG_PLAYER_HEALTH:
		return ps->stats[STAT_HEALTH];
		break;
	case CG_RED_SCORE:
		return cgs.scores1;
		break;
	case CG_BLUE_SCORE:
		return cgs.scores2;
		break;
	case CG_PLAYER_WEAPON_STABILITY:    //----(SA)	added
		return ps->aimSpreadScale;
		break;

#define BONUSTIME 60000.0f
#define SPRINTTIME 20000.0f

	case CG_STAMINA:    //----(SA)	added
		if ( type == RANGETYPE_RELATIVE ) {
			return (float)cg.snap->ps.sprintTime / SPRINTTIME;
		} else {
			return cg.snap->ps.sprintTime;
		}
		break;
	default:
		break;
	}

	return -1;
}

// THINKABOUTME: should these be exclusive or inclusive..
//
qboolean CG_OwnerDrawVisible( int flags ) {

//----(SA)	added
	if ( flags & CG_SHOW_NOT_V_BINOC ) {      // if looking through binocs
		if ( cg.zoomedBinoc ) {
			return qfalse;
		}
	}

	if ( flags & CG_SHOW_NOT_V_SNIPER ) {     // if looking through sniper scope
		if ( cg.weaponSelect == WP_SNIPERRIFLE ) {
			return qfalse;
		}
	}

	if ( flags & CG_SHOW_NOT_V_SNOOPER ) {        // if looking through snooper scope
		if ( cg.weaponSelect == WP_SNOOPERSCOPE ) {
			return qfalse;
		}
	}

	if ( flags & CG_SHOW_NOT_V_FGSCOPE ) {        // if looking through fg42 scope
		if ( cg.weaponSelect == WP_FG42SCOPE ) {
			return qfalse;
		}
	}

//----(SA)	end
	if ( flags & CG_SHOW_HEALTHCRITICAL ) {
		if ( cg.snap->ps.stats[STAT_HEALTH] < 25 ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_HEALTHOK ) {
		if ( cg.snap->ps.stats[STAT_HEALTH] > 25 ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_SINGLEPLAYER ) {
		if ( cgs.gametype == GT_SINGLE_PLAYER ) {
			return qtrue;
		}
	}

	if ( flags & CG_SHOW_DURINGINCOMINGVOICE ) {
	}

//----(SA)	added
	if ( flags & CG_SHOW_NOT_V_CLEAR ) {
		// if /not/ looking through binocs,snooper or sniper
		if ( !cg.zoomedBinoc && !( cg.weaponSelect == WP_SNIPERRIFLE ) && !( cg.weaponSelect == WP_SNOOPERSCOPE ) && !( cg.weaponSelect == WP_FG42SCOPE ) ) {
			return qfalse;
		}
	}

	if ( flags & ( CG_SHOW_NOT_V_BINOC | CG_SHOW_NOT_V_SNIPER | CG_SHOW_NOT_V_SNOOPER | CG_SHOW_NOT_V_FGSCOPE ) ) {
		// setting any of these does not necessarily disable drawing in regular view
		// CG_SHOW_NOT_V_CLEAR must also be set to hide for reg view
		if ( !( flags & CG_SHOW_NOT_V_CLEAR ) ) {
			return qtrue;
		}
	}

//----(SA)	end


	return qfalse;
}




static void CG_DrawAreaSystemChat( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, font, scale, color, systemChat, 0, 0, 0 );
}

static void CG_DrawAreaTeamChat( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, font, scale, color,teamChat1, 0, 0, 0 );
}

static void CG_DrawAreaChat( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, font, scale, color, teamChat2, 0, 0, 0 );
}

static const char *CG_GetKillerText( void ) {
	const unsigned char *s = (unsigned char *)"";
	if ( cg.killerName[0] ) {
		s = (unsigned char *)va( "Fragged by %s", cg.killerName );
	}
	return (const char*)s;
}


static void CG_DrawKiller( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	// fragged by ... line
	if ( cg.killerName[0] ) {
		int x = rect->x + rect->w / 2;
		CG_Text_Paint( x - CG_Text_Width( CG_GetKillerText(),font, scale, 0 ) / 2, rect->y + rect->h, font, scale, color, CG_GetKillerText(), 0, 0, textStyle );
	}

}


static void CG_DrawCapFragLimit( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	int limit = cgs.fraglimit;
	CG_Text_Paint( rect->x, rect->y, font, scale, color, va( "%2i", limit ),0, 0, textStyle );
}

static void CG_Draw1stPlace( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	if ( cgs.scores1 != SCORE_NOT_PRESENT ) {
		CG_Text_Paint( rect->x, rect->y, font, scale, color, va( "%2i", cgs.scores1 ),0, 0, textStyle );
	}
}

static void CG_Draw2ndPlace( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	if ( cgs.scores2 != SCORE_NOT_PRESENT ) {
		CG_Text_Paint( rect->x, rect->y, font, scale, color, va( "%2i", cgs.scores2 ),0, 0, textStyle );
	}
}

static const char *CG_GetGameStatusText( void ) {
	const unsigned char *s = (unsigned char*)"";
	if ( cg.snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
		s = (unsigned char *)va( "%s place with %i",CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),cg.snap->ps.persistant[PERS_SCORE] );
	}
	return (char *)s;
}

static void CG_DrawGameStatus( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, font, scale, color, CG_GetGameStatusText(), 0, 0, textStyle );
}

static const char *CG_GameTypeString( void ) {
	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		return "Single Player";
	} else if ( cgs.gametype == GT_COOP ) {
		return "Cooperative";
	} else if ( cgs.gametype == GT_COOP_SPEEDRUN ) {
		return "Speedrun";
	} else if ( cgs.gametype == GT_COOP_BATTLE ) {
		return "Battle";
	} else {
		return "";
	}
}

int CG_OwnerDrawWidth( int ownerDraw, int font, float scale ) {
	switch ( ownerDraw ) {
	case CG_GAME_TYPE:
		return CG_Text_Width( CG_GameTypeString(), font, scale, 0 );
	case CG_GAME_STATUS:
		return CG_Text_Width( CG_GetGameStatusText(), font, scale, 0 );
		break;
	case CG_KILLER:
		return CG_Text_Width( CG_GetKillerText(), font, scale, 0 );
		break;
	}
	return 0;
}

static void CG_DrawGameType( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	CG_Text_Paint( rect->x, rect->y + rect->h, font, scale, color, CG_GameTypeString(), 0, 0, textStyle );
}

static void CG_Text_Paint_Limit( float *maxX, float x, float y, int font, float scale, vec4_t color, const char* text, float adjust, int limit ) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;
	if ( text ) {
		const char *s = text;
		float max = *maxX;
		float useScale;

		fontInfo_t *fnt = &cgDC.Assets.textFont;
		if ( font == UI_FONT_DEFAULT ) {
			if ( scale <= cg_smallFont.value ) {
				fnt = &cgDC.Assets.smallFont;
			} else if ( scale > cg_bigFont.value ) {
				fnt = &cgDC.Assets.bigFont;
			}
		} else if ( font == UI_FONT_BIG ) {
			fnt = &cgDC.Assets.bigFont;
		} else if ( font == UI_FONT_SMALL ) {
			fnt = &cgDC.Assets.smallFont;
		} else if ( font == UI_FONT_HANDWRITING ) {
			fnt = &cgDC.Assets.handwritingFont;
		}

		useScale = scale * fnt->glyphScale;
		trap_R_SetColor( color );
		len = strlen( text );
		if ( limit > 0 && len > limit ) {
			len = limit;
		}
		count = 0;
		while ( s && *s && count < len ) {
			glyph = &fnt->glyphs[*s & 255];
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex( *( s + 1 ) )], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else {
				float yadj = useScale * glyph->top;
				if ( CG_Text_Width( (char *)s, font, useScale, 1 ) + x > max ) {
					*maxX = 0;
					break;
				}
				CG_Text_PaintChar( x, y - yadj,
								   glyph->imageWidth,
								   glyph->imageHeight,
								   useScale,
								   glyph->s,
								   glyph->t,
								   glyph->s2,
								   glyph->t2,
								   glyph->glyph );
				x += ( glyph->xSkip * useScale ) + adjust;
				*maxX = x;
				count++;
				s++;
			}
		}
		trap_R_SetColor( NULL );
	}

}



#define PIC_WIDTH 12

static void CG_DrawTeamSpectators( rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader ) {
	if ( cg.spectatorLen ) {
		float maxX;

		if ( cg.spectatorWidth == -1 ) {
			cg.spectatorWidth = 0;
			cg.spectatorPaintX = rect->x + 1;
			cg.spectatorPaintX2 = -1;
		}

		if ( cg.spectatorOffset > cg.spectatorLen ) {
			cg.spectatorOffset = 0;
			cg.spectatorPaintX = rect->x + 1;
			cg.spectatorPaintX2 = -1;
		}

		if ( cg.time > cg.spectatorTime ) {
			cg.spectatorTime = cg.time + 10;
			if ( cg.spectatorPaintX <= rect->x + 2 ) {
				if ( cg.spectatorOffset < cg.spectatorLen ) {
					cg.spectatorPaintX += CG_Text_Width( &cg.spectatorList[cg.spectatorOffset], font, scale, 1 ) - 1;
					cg.spectatorOffset++;
				} else {
					cg.spectatorOffset = 0;
					if ( cg.spectatorPaintX2 >= 0 ) {
						cg.spectatorPaintX = cg.spectatorPaintX2;
					} else {
						cg.spectatorPaintX = rect->x + rect->w - 2;
					}
					cg.spectatorPaintX2 = -1;
				}
			} else {
				cg.spectatorPaintX--;
				if ( cg.spectatorPaintX2 >= 0 ) {
					cg.spectatorPaintX2--;
				}
			}
		}

		maxX = rect->x + rect->w - 2;
		CG_Text_Paint_Limit( &maxX, cg.spectatorPaintX, rect->y + rect->h - 3, UI_FONT_DEFAULT, scale, color, &cg.spectatorList[cg.spectatorOffset], 0, 0 );
		if ( cg.spectatorPaintX2 >= 0 ) {
			float maxX2 = rect->x + rect->w - 2;
			CG_Text_Paint_Limit( &maxX2, cg.spectatorPaintX2, rect->y + rect->h - 3, UI_FONT_DEFAULT, scale, color, cg.spectatorList, 0, cg.spectatorOffset );
		}
		if ( cg.spectatorOffset && maxX > 0 ) {
			// if we have an offset ( we are skipping the first part of the string ) and we fit the string
			if ( cg.spectatorPaintX2 == -1 ) {
				cg.spectatorPaintX2 = rect->x + rect->w - 2;
			}
		} else {
			cg.spectatorPaintX2 = -1;
		}

	}
}



static void CG_DrawMedal( int ownerDraw, rectDef_t *rect, int font, float scale, vec4_t color, qhandle_t shader ) {
	score_t *score = &cg.scores[cg.selectedScore];
	float value = 0;
	char *text = NULL;
	color[3] = 0.25;

	switch ( ownerDraw ) {
	case CG_ACCURACY:
		value = score->accuracy;
		break;
	case CG_ASSISTS:
		value = score->assistCount;
		break;
	case CG_DEFEND:
		value = score->defendCount;
		break;
	case CG_EXCELLENT:
		value = score->excellentCount;
		break;
	case CG_IMPRESSIVE:
		value = score->impressiveCount;
		break;
	case CG_PERFECT:
		value = score->perfect;
		break;
	case CG_GAUNTLET:
		value = score->guantletCount;
		break;
	case CG_CAPTURES:
		value = score->captures;
		break;
	}

	if ( value > 0 ) {
		if ( ownerDraw != CG_PERFECT ) {
			if ( ownerDraw == CG_ACCURACY ) {
				text = va( "%i%%", (int)value );
				if ( value > 50 ) {
					color[3] = 1.0;
				}
			} else {
				text = va( "%i", (int)value );
				color[3] = 1.0;
			}
		} else {
			if ( value ) {
				color[3] = 1.0;
			}
			text = "Wow";
		}
	}

	trap_R_SetColor( color );
	CG_DrawPic( rect->x, rect->y, rect->w, rect->h, shader );

	if ( text ) {
		color[3] = 1.0;
		value = CG_Text_Width( text, font, scale, 0 );
		CG_Text_Paint( rect->x + ( rect->w - value ) / 2, rect->y + rect->h + 10, font, scale, color, text, 0, 0, 0 );
	}
	trap_R_SetColor( NULL );

}


/*
==============
CG_DrawWeapStability
    draw a bar showing current stability level (0-255), max at current weapon/ability, and 'perfect' reference mark

    probably only drawn for scoped weapons
==============
*/
static void CG_DrawWeapStability( rectDef_t *rect, vec4_t color, int align ) {
	vec4_t goodColor = {0, 1, 0, 0.5f}, badColor = {1, 0, 0, 0.5f};

	if ( !cg_drawSpreadScale.integer ) {
		return;
	}

	if ( cg_drawSpreadScale.integer == 1 && !( cg.weaponSelect == WP_SNOOPERSCOPE || cg.weaponSelect == WP_SNIPERRIFLE || cg.weaponSelect == WP_FG42SCOPE ) ) {
		// cg_drawSpreadScale of '1' means only draw for scoped weapons, '2' means draw all the time (for debugging)
		return;
	}

	if ( !( cg.snap->ps.aimSpreadScale ) ) {
		return;
	}

	CG_FilledBar( rect->x, rect->y, rect->w, rect->h, goodColor, badColor, NULL, (float)cg.snap->ps.aimSpreadScale / 255.0f, 2 | 4 | 256 ); // flags (BAR_CENTER|BAR_VERT|BAR_LERP_COLOR)
}


/*
==============
CG_DrawWeapHeat
==============
*/
static void CG_DrawWeapHeat( rectDef_t *rect, int align ) {
	vec4_t color = {1, 0, 0, 0.2f}, color2 = {1, 0, 0, 0.5f};
	int flags = 0;

	if ( !( cg.snap->ps.curWeapHeat ) ) {
		return;
	}

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_RIGHT, PLACE_BOTTOM);
	}

	if ( align != HUD_HORIZONTAL ) {
		flags |= 4;   // BAR_VERT

	}
	flags |= 1;       // BAR_LEFT			- this is hardcoded now, but will be decided by the menu script
	flags |= 16;      // BAR_BG			- draw the filled contrast box
//	flags|=32;		// BAR_BGSPACING_X0Y5	- different style

	flags |= 256;     // BAR_COLOR_LERP
	CG_FilledBar( rect->x, rect->y, rect->w, rect->h, color, color2, NULL, (float)cg.snap->ps.curWeapHeat / 255.0f, flags );
}


/*
==============
CG_DrawFatigue
==============
*/

static void CG_DrawFatigue( rectDef_t *rect, vec4_t color, int align ) {
	//	vec4_t	color = {0, 1, 0, 1}, color2 = {1, 0, 0, 1};
	vec4_t colorBonus = {1, 1, 0, 0.45f};   // yellow (a little more solid for the 'bonus' stamina)
	int flags = 0;

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_LEFT, PLACE_BOTTOM);
	}

	if ( align != HUD_HORIZONTAL ) {
		flags |= 4;   // BAR_VERT
		flags |= 1;   // BAR_LEFT (left, when vertical means grow 'up')
	}

	CG_FilledBar( rect->x, rect->y, rect->w, rect->h, color, NULL, NULL, (float)cg.snap->ps.sprintTime / SPRINTTIME, flags );

	// fill in the left side of the bar with the counter for the nofatigue powerup
	if ( cg.snap->ps.powerups[PW_NOFATIGUE] ) {
		CG_FilledBar( rect->x, rect->y, rect->w / 2, rect->h, colorBonus, NULL, NULL, cg.snap->ps.powerups[PW_NOFATIGUE] / BONUSTIME, flags );
	}
}

/*
=================
CG_DrawCompassIcon

NERVE - SMF
=================
*/
static void CG_DrawCompassIcon( int x, int y, int w, int h, vec3_t origin, vec3_t dest, qhandle_t shader ) {
	float angle, pi2 = M_PI * 2;
	vec3_t v1, angles;
	float len;

	VectorCopy( dest, v1 );
	VectorSubtract( origin, v1, v1 );
	len = VectorLength( v1 );
	VectorNormalize( v1 );
	vectoangles( v1, angles );

	if ( v1[0] == 0 && v1[1] == 0 && v1[2] == 0 ) {
		return;
	}

	angles[YAW] = AngleSubtract( cg.snap->ps.viewangles[YAW], angles[YAW] );

	angle = ( ( angles[YAW] + 180.f ) / 360.f - ( 0.50 / 2.f ) ) * pi2;

	w /= 2;
	h /= 2;

	x += w;
	y += h;

	w = sqrt( ( w * w ) + ( h * h ) ) / 3.f * 2.f * 0.9f;

	x = x + ( cos( angle ) * w );
	y = y + ( sin( angle ) * w );

	len = 1 - min( 1.f, len / 2000.f );

	CG_DrawPic( x - ( 14 * len + 4 ) / 2, y - ( 14 * len + 4 ) / 2, 14 * len + 4, 14 * len + 4, shader );
}


/*
=================
CG_DrawCompass

NERVE - SMF
=================
*/
static void CG_DrawCompass( void ) {
	float basex = 290, basey = 420;
	float basew = 60, baseh = 60;
	vec4_t hcolor;
	float angle;
	int i;

	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		return;
	}

	if ( !cg_drawcompass.integer ) {
		return;
	}

	if ( cg.snap->ps.pm_flags & PMF_LIMBO || cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		return;
	}

	if ( cg_fixedAspect.integer == 2 ) {
		CG_SetScreenPlacement(PLACE_CENTER, PLACE_BOTTOM);
	}

	angle = ( cg.snap->ps.viewangles[YAW] + 180.f ) / 360.f - ( 0.25 / 2.f );

	VectorSet( hcolor, 1.f,1.f,1.f );
	hcolor[3] = cg_hudAlpha.value;
	trap_R_SetColor( hcolor );

	CG_DrawRotatedPic( basex, basey, basew, baseh, trap_R_RegisterShader( "gfx/2d/compass2.tga" ), angle );
	CG_DrawPic( basex, basey, basew, baseh, trap_R_RegisterShader( "gfx/2d/compass.tga" ) );

	// draw voice chats
	for ( i = 0; i < MAX_CLIENTS; i++ ) {
		centity_t *cent = &cg_entities[i];

		if ( cg.snap->ps.clientNum == i || !cgs.clientinfo[i].infoValid || cent->currentState.teamNum != cg.snap->ps.teamNum ) {
			continue;
		}


		CG_DrawCompassIcon( basex, basey, basew, baseh, cg.snap->ps.origin, cent->lerpOrigin, trap_R_RegisterShader( "sprites/destroy.tga" ) );
	}
}
// -NERVE - SMF



/*
==============
CG_OwnerDraw
==============
*/
void CG_OwnerDraw( float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, int font, float scale, vec4_t color, qhandle_t shader, int textStyle ) {
	rectDef_t rect;

	if ( cg_drawStatus.integer == 0 ) {
		return;
	}

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	switch ( ownerDraw ) {
	case CG_PLAYER_COMPASS:
		CG_DrawCompass();
		break;
	case CG_PLAYER_WEAPON_ICON2D:
		CG_DrawPlayerWeaponIcon( &rect, ownerDrawFlags & CG_SHOW_HIGHLIGHTED, align );
		break;
	case CG_PLAYER_ARMOR_ICON:
		break;
	case CG_PLAYER_ARMOR_ICON2D:
		break;
	case CG_PLAYER_ARMOR_VALUE:
		CG_DrawPlayerArmorValue( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_PLAYER_AMMO_ICON:
		CG_DrawPlayerAmmoIcon( &rect, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_AMMO_ICON2D:
		CG_DrawPlayerAmmoIcon( &rect, qtrue );
		break;
	case CG_PLAYER_AMMO_VALUE:
		CG_DrawPlayerAmmoValue( &rect, font, scale, color, shader, textStyle, 0 );
		break;
	case CG_CURSORHINT:
		CG_DrawCursorhint( &rect );
		break;
	//----(SA)	added
	case CG_NEWMESSAGE:
		CG_DrawMessageIcon( &rect );
		break;
	//----(SA)	end
	case CG_PLAYER_AMMOCLIP_VALUE:
		CG_DrawPlayerAmmoValue( &rect, font, scale, color, shader, textStyle, 1 );
		break;
	case CG_PLAYER_WEAPON_HEAT:
		CG_DrawWeapHeat( &rect, align );
		break;
	case CG_PLAYER_WEAPON_STABILITY:
		CG_DrawWeapStability( &rect, color, align );
		break;
	case CG_STAMINA:
		CG_DrawFatigue( &rect, color, align );
		break;
	case CG_PLAYER_HEAD:
		CG_DrawPlayerHead( &rect, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_HOLDABLE:
		CG_DrawHoldableItem( &rect, font, scale, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_ITEM:
		CG_DrawPlayerItem( &rect, font, scale, ownerDrawFlags & CG_SHOW_2DONLY );
		break;
	case CG_PLAYER_SCORE:
		CG_DrawPlayerScore( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_PLAYER_LIVES:
		CG_DrawPlayerLives( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_PLAYER_SPAWNPOINTS:
		CG_DrawPlayerSpawnpoints( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_PLAYER_HEALTH:
		CG_DrawPlayerHealth( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_RED_SCORE:
		CG_DrawRedScore( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_BLUE_SCORE:
		CG_DrawBlueScore( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_RED_NAME:
		break;
	case CG_BLUE_NAME:
		break;
	case CG_BLUE_FLAGHEAD:
		break;
	case CG_BLUE_FLAGSTATUS:
		break;
	case CG_BLUE_FLAGNAME:
		break;
	case CG_RED_FLAGHEAD:
		break;
	case CG_RED_FLAGSTATUS:
		break;
	case CG_RED_FLAGNAME:
		break;
	case CG_HARVESTER_SKULLS:
		break;
	case CG_HARVESTER_SKULLS2D:
		break;
	case CG_ONEFLAG_STATUS:
		break;
	case CG_PLAYER_LOCATION:
		CG_DrawPlayerLocation( &rect, font, scale, color, textStyle );
		break;
	case CG_TEAM_COLOR:
		CG_DrawTeamColor( &rect, color );
		break;
	case CG_CTF_POWERUP:
		break;
	case CG_PLAYER_STATUS:
		break;
	case CG_PLAYER_HASFLAG:
		break;
	case CG_PLAYER_HASFLAG2D:
		break;
	case CG_AREA_SYSTEMCHAT:
		CG_DrawAreaSystemChat( &rect, font, scale, color, shader );
		break;
	case CG_AREA_TEAMCHAT:
		CG_DrawAreaTeamChat( &rect, font, scale, color, shader );
		break;
	case CG_AREA_CHAT:
		CG_DrawAreaChat( &rect, font, scale, color, shader );
		break;
	case CG_GAME_TYPE:
		CG_DrawGameType( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_GAME_STATUS:
		CG_DrawGameStatus( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_KILLER:
		CG_DrawKiller( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_ACCURACY:
	case CG_ASSISTS:
	case CG_DEFEND:
	case CG_EXCELLENT:
	case CG_IMPRESSIVE:
	case CG_PERFECT:
	case CG_GAUNTLET:
	case CG_CAPTURES:
		CG_DrawMedal( ownerDraw, &rect, font, scale, color, shader );
		break;
	case CG_SPECTATORS:
		CG_DrawTeamSpectators( &rect, font, scale, color, shader );
		break;
	case CG_TEAMINFO:
		break;
	case CG_CAPFRAGLIMIT:
		CG_DrawCapFragLimit( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_1STPLACE:
		CG_Draw1stPlace( &rect, font, scale, color, shader, textStyle );
		break;
	case CG_2NDPLACE:
		CG_Draw2ndPlace( &rect, font, scale, color, shader, textStyle );
		break;
	default:
		break;
	}
}

void CG_MouseEvent( int x, int y ) {
	int n;

	if ( ( cg.predictedPlayerState.pm_type == PM_NORMAL || cg.predictedPlayerState.pm_type == PM_SPECTATOR ) && cg.showScores == qfalse ) {
		trap_Key_SetCatcher( 0 );
		return;
	}

	cgs.cursorX += x;
	if ( cgs.cursorX < 0 ) {
		cgs.cursorX = 0;
	} else if ( cgs.cursorX > 640 ) {
		cgs.cursorX = 640;
	}

	cgs.cursorY += y;
	if ( cgs.cursorY < 0 ) {
		cgs.cursorY = 0;
	} else if ( cgs.cursorY > 480 ) {
		cgs.cursorY = 480;
	}

	n = Display_CursorType( cgs.cursorX, cgs.cursorY );
	cgs.activeCursor = 0;
	if ( n == CURSOR_ARROW ) {
		cgs.activeCursor = cgs.media.selectCursor;
	} else if ( n == CURSOR_SIZER ) {
		cgs.activeCursor = cgs.media.sizeCursor;
	}

	if ( cgs.capturedItem ) {
		Display_MouseMove( cgs.capturedItem, x, y );
	} else {
		Display_MouseMove( NULL, cgs.cursorX, cgs.cursorY );
	}

}

/*
==================
CG_EventHandling
==================
 type 0 - no event handling
      1 - team menu
      2 - hud editor

*/
void CG_EventHandling( int type ) {
	cgs.eventHandling = type;
	if ( type == CGAME_EVENT_NONE ) { // hide menus

	} else if ( type == CGAME_EVENT_TEAMMENU ) {

	} else if ( type == CGAME_EVENT_SCOREBOARD ) {

	}

}



void CG_KeyEvent( int key, qboolean down ) {

	if ( !down ) {
		return;
	}

	if ( cg.predictedPlayerState.pm_type == PM_NORMAL || ( cg.predictedPlayerState.pm_type == PM_SPECTATOR && cg.showScores == qfalse ) ) {
		CG_EventHandling( CGAME_EVENT_NONE );
		trap_Key_SetCatcher( 0 );
		return;
	}

	Display_HandleKey( key, down, cgs.cursorX, cgs.cursorY );

	if ( cgs.capturedItem ) {
		cgs.capturedItem = NULL;
	}   else {
		if ( key == K_MOUSE2 && down ) {
			cgs.capturedItem = Display_CaptureItem( cgs.cursorX, cgs.cursorY );
		}
	}
}

void CG_RunMenuScript( char **args ) {

}


void CG_GetTeamColor( vec4_t *color ) {
	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
		( *color )[0] = 1;
		( *color )[3] = .25f;
		( *color )[1] = ( *color )[2] = 0;
	} else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
		( *color )[0] = ( *color )[1] = 0;
		( *color )[2] = 1;
		( *color )[3] = .25f;
	} else {
		( *color )[0] = ( *color )[2] = 0;
		( *color )[1] = .17f;
		( *color )[3] = .25f;
	}
}
