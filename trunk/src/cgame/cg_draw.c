/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (RTCW SP Source Code).  

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

// cg_draw.c -- draw all of the graphical elements during
// active (after loading) gameplay

#include "cg_local.h"
#include "../ui/ui_shared.h"

//----(SA) added to make it easier to raise/lower our statsubar by only changing one thing
#define STATUSBARHEIGHT 452
//----(SA) end

extern displayContextDef_t cgDC;

int sortedTeamPlayers[TEAM_MAXOVERLAY];
int numSortedTeamPlayers;

char systemChat[256];
char teamChat1[256];
char teamChat2[256];

////////////////////////
////////////////////////
////// new hud stuff
///////////////////////
///////////////////////

int CG_Text_Width( const char *text, int font, float scale, int limit ) {
	int count,len;
	float out;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;

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
	out = 0;
	if ( text ) {
		len = strlen( text );
		if ( limit > 0 && len > limit ) {
			len = limit;
		}
		count = 0;
		while ( s && *s && count < len ) {
			if ( Q_IsColorString( s ) ) {
				s += 2;
				continue;
			} else {
				glyph = &fnt->glyphs[(int)*s];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}
	return out * useScale;
}

int CG_Text_Height( const char *text, int font, float scale, int limit ) {
	int len, count;
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;

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
	max = 0;
	if ( text ) {
		len = strlen( text );
		if ( limit > 0 && len > limit ) {
			len = limit;
		}
		count = 0;
		while ( s && *s && count < len ) {
			if ( Q_IsColorString( s ) ) {
				s += 2;
				continue;
			} else {
				glyph = &fnt->glyphs[(int)*s];
				if ( max < glyph->height ) {
					max = glyph->height;
				}
				s++;
				count++;
			}
		}
	}
	return max * useScale;
}

void CG_Text_PaintChar( float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader ) {
	float w, h;
	w = width * scale;
	h = height * scale;
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint( float x, float y, int font, float scale, vec4_t color, const char *text, float adjust, int limit, int style ) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;
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

	color[3] *= cg_hudAlpha.value;  // (SA) adjust for cg_hudalpha

	if ( text ) {
		const char *s = text;
		trap_R_SetColor( color );
		memcpy( &newColor[0], &color[0], sizeof( vec4_t ) );
		len = strlen( text );
		if ( limit > 0 && len > limit ) {
			len = limit;
		}
		count = 0;
		while ( s && *s && count < len ) {
			glyph = &fnt->glyphs[(int)*s];
			//int yadj = Assets.textFont.glyphs[text[i]].bottom + Assets.textFont.glyphs[text[i]].top;
			//float yadj = scale * (Assets.textFont.glyphs[text[i]].imageHeight - Assets.textFont.glyphs[text[i]].height);
			if ( Q_IsColorString( s ) ) {
				memcpy( newColor, g_color_table[ColorIndex( *( s + 1 ) )], sizeof( newColor ) );
				newColor[3] = color[3];
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else {
				float yadj = useScale * glyph->top;
				if ( style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE ) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					CG_Text_PaintChar( x + ofs, y - yadj + ofs,
									   glyph->imageWidth,
									   glyph->imageHeight,
									   useScale,
									   glyph->s,
									   glyph->t,
									   glyph->s2,
									   glyph->t2,
									   glyph->glyph );
					colorBlack[3] = 1.0;
					trap_R_SetColor( newColor );
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
				// CG_DrawPic(x, y - yadj, scale * cgDC.Assets.textFont.glyphs[text[i]].imageWidth, scale * cgDC.Assets.textFont.glyphs[text[i]].imageHeight, cgDC.Assets.textFont.glyphs[text[i]].glyph);
				x += ( glyph->xSkip * useScale ) + adjust;
				s++;
				count++;
			}
		}
		trap_R_SetColor( NULL );
	}
}

/*
================
CG_Draw3DModel

================
*/
void CG_Draw3DModel( float x, float y, float w, float h, qhandle_t model, qhandle_t skin, vec3_t origin, vec3_t angles ) {
	refdef_t refdef;
	refEntity_t ent;

	if ( !cg_draw3dIcons.integer || !cg_drawIcons.integer ) {
		return;
	}

	CG_AdjustFrom640( &x, &y, &w, &h );

	memset( &refdef, 0, sizeof( refdef ) );

	memset( &ent, 0, sizeof( ent ) );
	AnglesToAxis( angles, ent.axis );
	VectorCopy( origin, ent.origin );
	ent.hModel = model;
	ent.customSkin = skin;
	ent.renderfx = RF_NOSHADOW;     // no stencil shadows

	refdef.rdflags = RDF_NOWORLDMODEL;

	AxisClear( refdef.viewaxis );

	refdef.fov_x = 30;
	refdef.fov_y = 30;

	refdef.x = x;
	refdef.y = y;
	refdef.width = w;
	refdef.height = h;

	refdef.time = cg.time;

	refdef.rdflags |= RDF_DRAWSKYBOX;
	if ( !cg_skybox.integer ) {
		refdef.rdflags &= ~RDF_DRAWSKYBOX;
	}

	trap_R_ClearScene();
	trap_R_AddRefEntityToScene( &ent );
	trap_R_RenderScene( &refdef );
}

/*
================
CG_DrawHead

Used for both the status bar and the scoreboard
================
*/
void CG_DrawHead( float x, float y, float w, float h, int clientNum, vec3_t headAngles ) {
	clipHandle_t cm;
	clientInfo_t    *ci;
	float len;
	vec3_t origin;
	vec3_t mins, maxs;

	ci = &cgs.clientinfo[ clientNum ];

	if ( cg_draw3dIcons.integer ) {
		cm = ci->headModel;
		if ( !cm ) {
			return;
		}

		// offset the origin y and z to center the head
		trap_R_ModelBounds( cm, mins, maxs );

		origin[2] = -0.5 * ( mins[2] + maxs[2] );
		origin[1] = 0.5 * ( mins[1] + maxs[1] );

		// calculate distance so the head nearly fills the box
		// assume heads are taller than wide
		len = 0.7 * ( maxs[2] - mins[2] );
		origin[0] = len / 0.268;    // len / tan( fov/2 )

		// allow per-model tweaking
		VectorAdd( origin, ci->modelInfo->headOffset, origin );

		CG_Draw3DModel( x, y, w, h, ci->headModel, ci->headSkin, origin, headAngles );
//	} else if ( cg_drawIcons.integer ) {
//		CG_DrawPic( x, y, w, h, ci->modelIcon );
	}

	// if they are deferred, draw a cross out
	if ( ci->deferred ) {
		CG_DrawPic( x, y, w, h, cgs.media.deferShader );
	}
}

/*
================
CG_DrawFlagModel

Used for both the status bar and the scoreboard
================
*/
void CG_DrawFlagModel( float x, float y, float w, float h, int team ) {
	qhandle_t cm;
	float len;
	vec3_t origin, angles;
	vec3_t mins, maxs;

	VectorClear( angles );

	cm = cgs.media.redFlagModel;

	// offset the origin y and z to center the flag
	trap_R_ModelBounds( cm, mins, maxs );

	origin[2] = -0.5 * ( mins[2] + maxs[2] );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );

	// calculate distance so the flag nearly fills the box
	// assume heads are taller than wide
	len = 0.5 * ( maxs[2] - mins[2] );
	origin[0] = len / 0.268;    // len / tan( fov/2 )

	angles[YAW] = 60 * sin( cg.time / 2000.0 );;

	CG_Draw3DModel( x, y, w, h,
					team == TEAM_RED ? cgs.media.redFlagModel : cgs.media.blueFlagModel,
					0, origin, angles );
}


/*
==============
CG_DrawKeyModel
==============
*/
void CG_DrawKeyModel( int keynum, float x, float y, float w, float h, int fadetime ) {
	qhandle_t cm;
	float len;
	vec3_t origin, angles;
	vec3_t mins, maxs;

	VectorClear( angles );

	cm = cg_items[keynum].models[0];

	// offset the origin y and z to center the model
	trap_R_ModelBounds( cm, mins, maxs );

	origin[2] = -0.5 * ( mins[2] + maxs[2] );
	origin[1] = 0.5 * ( mins[1] + maxs[1] );

//	len = 0.5 * ( maxs[2] - mins[2] );
	len = 0.75 * ( maxs[2] - mins[2] );
	origin[0] = len / 0.268;    // len / tan( fov/2 )

	angles[YAW] = 30 * sin( cg.time / 2000.0 );;

	CG_Draw3DModel( x, y, w, h, cg_items[keynum].models[0], 0, origin, angles );
}

/*
================
CG_DrawTeamBackground

================
*/
void CG_DrawTeamBackground( int x, int y, int w, int h, float alpha, int team ) {
	vec4_t hcolor;

	hcolor[3] = alpha;
	if ( team == TEAM_RED ) {
		hcolor[0] = 1;
		hcolor[1] = 0;
		hcolor[2] = 0;
	} else if ( team == TEAM_BLUE ) {
		hcolor[0] = 0;
		hcolor[1] = 0;
		hcolor[2] = 1;
	} else {
		return;
	}
	trap_R_SetColor( hcolor );
	CG_DrawPic( x, y, w, h, cgs.media.teamStatusBar );
	trap_R_SetColor( NULL );
}

//////////////////////
////// end new hud stuff
//////////////////////

/*
===========================================================================================

  UPPER RIGHT CORNER

===========================================================================================
*/

#define UPPERRIGHT_X 500
/*
==================
CG_DrawSnapshot
==================
*/
static float CG_DrawSnapshot( float y ) {
	char        *s;
	int w;

	s = va( "time:%i snap:%i cmd:%i", cg.snap->serverTime,
			cg.latestSnapshotNum, cgs.serverCommandSequence );
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

	CG_DrawBigString( UPPERRIGHT_X - w, y + 2, s, 1.0F );

	return y + BIGCHAR_HEIGHT + 4;
}

/*
==================
CG_DrawFPS
==================
*/
#define FPS_FRAMES  4
static float CG_DrawFPS( float y ) {
	char        *s;
	int w;
	static int previousTimes[FPS_FRAMES];
	static int index;
	int i, total;
	int fps;
	static int previous;
	int t, frameTime;

	// don't use serverTime, because that will be drifting to
	// correct for internet lag changes, timescales, timedemos, etc
	t = trap_Milliseconds();
	frameTime = t - previous;
	previous = t;

	previousTimes[index % FPS_FRAMES] = frameTime;
	index++;
	if ( index > FPS_FRAMES ) {
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		fps = 1000 * FPS_FRAMES / total;

		s = va( "%ifps", fps );
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

		CG_DrawBigString( UPPERRIGHT_X - w, y + 2, s, 1.0F );
	}

	return y + BIGCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawTimer
=================
*/
static float CG_DrawTimer( float y ) {
	char        *s;
	int w;
	int mins, seconds, tens;
	int msec;

	msec = cg.time - cgs.levelStartTime;
	// -NERVE - SMF

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	s = va( "%i:%i%i", mins, tens, seconds );
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;

	CG_DrawBigString( UPPERRIGHT_X - w, y + 2, s, 1.0F );

	return y + BIGCHAR_HEIGHT + 4;
}

/*
=================
CG_DrawCoopOverlay
=================
*/

int maxCharsBeforeOverlay;

#define TEAM_OVERLAY_MAXNAME_WIDTH  16
#define TEAM_OVERLAY_MAXLOCATION_WIDTH  20

static float CG_DrawCoopOverlay( float y ) {
        int x, w, h, xx;
        int i, len; 
        const char *p;
        vec4_t hcolor, bgcolor;
        int pwidth, lwidth;
        int plyrs;
        char st[16];
        clientInfo_t *ci; 
        // NERVE - SMF
        char classType[2] = { 0, 0 }; 
        int val; 
        vec4_t deathcolor, damagecolor;      // JPW NERVE
        float       *pcolor;
        // -NERVE - SMF

        deathcolor[0] = 1; 
        deathcolor[1] = 0; 
        deathcolor[2] = 0; 
        deathcolor[3] = cg_hudAlpha.value;
        damagecolor[0] = 1; 
        damagecolor[1] = 1; 
        damagecolor[2] = 0; 
        damagecolor[3] = cg_hudAlpha.value;
        maxCharsBeforeOverlay = 80;

        if ( !cg_drawTeamOverlay.integer ) {
                return y;
        }

        plyrs = 0;

        // max player name width
        pwidth = 0;
        for ( i = 0; i < numSortedTeamPlayers; i++ ) {
                ci = cgs.clientinfo + sortedTeamPlayers[i];
                if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM] ) {
                        plyrs++;
                        len = CG_DrawStrlen( ci->name );
                        if ( len > pwidth ) {
                                pwidth = len;
                        }
                }
        }

        if ( !plyrs ) {
                return y;
        }

        if ( pwidth > TEAM_OVERLAY_MAXNAME_WIDTH ) {
                pwidth = TEAM_OVERLAY_MAXNAME_WIDTH;
        }

#if 1 
        // max location name width
        lwidth = 0;
        if ( cg_drawTeamOverlay.integer > 1 ) {
                for ( i = 0; i < numSortedTeamPlayers; i++ ) {
                        ci = cgs.clientinfo + sortedTeamPlayers[i];
                        if ( ci->infoValid &&
                                 ci->team == cg.snap->ps.persistant[PERS_TEAM] &&
                                 CG_ConfigString( CS_LOCATIONS + ci->location ) ) {
                                len = CG_DrawStrlen( CG_ConfigString( CS_LOCATIONS + ci->location ) );
                                if ( len > lwidth ) {
                                        lwidth = len;
                                }
                        }
                }
        }
#else
        // max location name width
        lwidth = 0;
        for ( i = 1; i < MAX_LOCATIONS; i++ ) {
                p = CG_ConfigString( CS_LOCATIONS + i );
                if ( p && *p ) {
                        len = CG_DrawStrlen( p );
                        if ( len > lwidth ) {
                                lwidth = len;
                        }
                }
        }
#endif

        if ( lwidth > TEAM_OVERLAY_MAXLOCATION_WIDTH ) {
                lwidth = TEAM_OVERLAY_MAXLOCATION_WIDTH;
        }

        if ( cg_drawTeamOverlay.integer > 1 ) {
                w = ( pwidth + lwidth + 3 + 7 ) * TINYCHAR_WIDTH; // JPW NERVE was +4+7
        } else {
                w = ( pwidth + lwidth + 8 ) * TINYCHAR_WIDTH; // JPW NERVE was +4+7

        }
        x = 640 - w - 4; // JPW was -32
        h = plyrs * TINYCHAR_HEIGHT;

        // DHM - Nerve :: Set the max characters that can be printed before the left edge
        maxCharsBeforeOverlay = ( x / TINYCHAR_WIDTH ) - 1;

        hcolor[0] = 0.25f;
        hcolor[1] = 0.25f;
        hcolor[2] = 0.5f;
        hcolor[3] = 0.25f * cg_hudAlpha.value;

        CG_FillRect( x,y,w,h,hcolor );
        VectorSet( hcolor, 0.4f, 0.4f, 0.4f );
        hcolor[3] = cg_hudAlpha.value;
        CG_DrawRect( x - 1, y, w + 2, h + 2, 1, hcolor );


        for ( i = 0; i < numSortedTeamPlayers; i++ ) {
                ci = cgs.clientinfo + sortedTeamPlayers[i];
                if ( ci->infoValid && ci->team == cg.snap->ps.persistant[PERS_TEAM] ) {

                        // NERVE - SMF
                        // determine class type
                        //val = cg_entities[ ci->clientNum ].currentState.teamNum;
                        val = ci->curWeapon;
                        
                        if ( val == WP_KNIFE ) {
                                classType[0] = 'K'; // knife
                        } else if ( val == WP_LUGER || val == WP_COLT || val == WP_AKIMBO || val == WP_SILENCER ) {
                                classType[0] = 'P'; // pistol
                        } else if ( val == WP_THOMPSON || val == WP_MP40 || val == WP_STEN ) {
                                classType[0] = 'S'; // smg
                        } else if ( val == WP_GRENADE_LAUNCHER || val == WP_GRENADE_PINEAPPLE || val == WP_DYNAMITE ) {
                                classType[0] = 'E'; // explosive
                        } else if ( val == WP_MAUSER || val == WP_GARAND || val == WP_SNIPERRIFLE || val == WP_SNOOPERSCOPE || val == WP_FG42SCOPE || val == WP_FG42 || val == WP_SNIPER ) {
                                classType[0] = 'R'; // rifle
                        } else if ( val == WP_PANZERFAUST || val == WP_VENOM || val == WP_FLAMETHROWER || val == WP_TESLA ) {
                                classType[0] = 'H'; // heavy weapon
                        } else {
                                classType[0] = 'X'; // ERROR !
                        }

                        Com_sprintf( st, sizeof( st ), "%s", classType );

                        xx = x + TINYCHAR_WIDTH;


                        hcolor[0] = hcolor[1] = 1.0;
                        hcolor[2] = 0.0;
                        hcolor[3] = cg_hudAlpha.value;

                        CG_DrawStringExt( xx, y,
                                                          st, hcolor, qtrue, qfalse,
                                                          TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 1 );

                        hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
                        hcolor[3] = cg_hudAlpha.value;

                        xx = x + 3 * TINYCHAR_WIDTH;

                        // JPW NERVE
                        if ( ci->health > 80 ) {
                                pcolor = hcolor;
                        } else if ( ci->health > 0 ) {
                                pcolor = damagecolor;
                        } else {
                                pcolor = deathcolor;
                        }
                        // jpw

                        // if that guy killed someone flash his name green
                        if (ci->lastteamkilltime + 5000 >= cg.time) {
                                bgcolor[0] = 1.0;
                                bgcolor[1] = 0.0;
                                bgcolor[2] = 0.0;
                                bgcolor[3] = cg_hudAlpha.value;
                                CG_FillRect( xx, y+1, CG_DrawStrlen(ci->name) * TINYCHAR_WIDTH, TINYCHAR_HEIGHT-1, bgcolor );
                        } else if (ci->lastkilltime + 1000 >= cg.time) {
                                pcolor[0] = 0.0;
                                pcolor[1] = 1.0;
                                pcolor[2] = 0.0;
                        }                        


                        CG_DrawStringExt( xx, y,
                                                          ci->name, pcolor, qtrue, qfalse,
                                                          TINYCHAR_WIDTH, TINYCHAR_HEIGHT, TEAM_OVERLAY_MAXNAME_WIDTH );

                        if ( lwidth ) {
                                p = CG_ConfigString( CS_LOCATIONS + ci->location );
                                if ( !p || !*p ) {
                                        p = "unknown";
                                }
                                //p = CG_TranslateString( p );
                                len = CG_DrawStrlen( p );
                                if ( len > lwidth ) {
                                        len = lwidth;
                                }

                                xx = x + TINYCHAR_WIDTH * 5 + TINYCHAR_WIDTH * pwidth +
                                         ( ( lwidth / 2 - len / 2 ) * TINYCHAR_WIDTH );
                                CG_DrawStringExt( xx, y,
                                                                  p, hcolor, qfalse, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT,
                                                                  TEAM_OVERLAY_MAXLOCATION_WIDTH );
                        }

                        Com_sprintf( st, sizeof( st ), "%3i", ci->health ); // JPW NERVE pulled class stuff since it's at top now

                        if ( cg_drawTeamOverlay.integer > 1 ) {
                                xx = x + TINYCHAR_WIDTH * 6 + TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;
                        } else {
                                xx = x + TINYCHAR_WIDTH * 4 + TINYCHAR_WIDTH * pwidth + TINYCHAR_WIDTH * lwidth;
                        }

                        CG_DrawStringExt( xx, y,
                                                          st, pcolor, qfalse, qfalse,
                                                          TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 3 );

                        y += TINYCHAR_HEIGHT;
                }
        }

        return y;
}

static void CG_DrawTimeLeft( void ) {

        const char *s;
        int msec, mins, seconds, tens;
        float color[4] = {1, 1, 1, 1}; 


        if ( cgs.gametype != GT_COOP_SPEEDRUN)
                return;

        msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );

        seconds = msec / 1000;
        mins = seconds / 60; 
        seconds -= mins * 60; 
        tens = seconds / 10;  
        seconds -= tens * 10; 

        s = va( "%2.0f:%i%i", (float)mins, tens, seconds );

        CG_DrawStringExt( 5, 105, s, color, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
}

/*
=====================
CG_DrawUpperRight

=====================
*/
static void CG_DrawUpperRight( void ) {
	float y;

	y = 0;

        if ( cgs.gametype <= GT_COOP ) {
		y = CG_DrawCoopOverlay( y );
        }
	if ( cg_drawSnapshot.integer ) {
		y = CG_DrawSnapshot( y );
	}
	if ( cg_drawFPS.integer ) {
		y = CG_DrawFPS( y );
	}
	if ( cg_drawTimer.integer ) {
		y = CG_DrawTimer( y );
	}
// (SA) disabling drawattacker for the time being
//	if ( cg_drawAttacker.integer ) {
//		y = CG_DrawAttacker( y );
//	}
//----(SA)	end
}

/*
===========================================================================================

  LOWER RIGHT CORNER

===========================================================================================
*/

/*
=================
CG_DrawTeamInfo
=================
*/
static void CG_DrawTeamInfo( void ) {
        int w, h;
        int i, len; 
        vec4_t hcolor;
        int chatHeight;
        float alphapercent;

#define CHATLOC_Y 385 // bottom end
#define CHATLOC_X 0

        if ( cg_teamChatHeight.integer < TEAMCHAT_HEIGHT ) {
                chatHeight = cg_teamChatHeight.integer;
        } else {
                chatHeight = TEAMCHAT_HEIGHT;
        }    
        if ( chatHeight <= 0 ) {
                return; // disabled

        }    
        if ( cgs.teamLastChatPos != cgs.teamChatPos ) {
                if ( cg.time - cgs.teamChatMsgTimes[cgs.teamLastChatPos % chatHeight] > cg_teamChatTime.integer ) {
                        cgs.teamLastChatPos++;
                }    

                h = ( cgs.teamChatPos - cgs.teamLastChatPos ) * TINYCHAR_HEIGHT;

                w = 0; 

                for ( i = cgs.teamLastChatPos; i < cgs.teamChatPos; i++ ) {
                        len = CG_DrawStrlen( cgs.teamChatMsgs[i % chatHeight] );
                        if ( len > w ) {
                                w = len; 
                        }    
                }    
                w *= TINYCHAR_WIDTH;
                w += TINYCHAR_WIDTH * 2; 
// JPW NERVE rewritten to support first pass at fading chat messages
                for ( i = cgs.teamChatPos - 1; i >= cgs.teamLastChatPos; i-- ) {
                        alphapercent = 1.0f - ( cg.time - cgs.teamChatMsgTimes[i % chatHeight] ) / (float)( cg_teamChatTime.integer );
                        if ( alphapercent > 1.0f ) {
                                alphapercent = 1.0f;
                        } else if ( alphapercent < 0.f ) {
                                alphapercent = 0.f;
                        }

                        if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED ) {
                                hcolor[0] = 1;
                                hcolor[1] = 0;
                                hcolor[2] = 0;
//                      hcolor[3] = 0.33;
                        } else if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE ) {
                                hcolor[0] = 0;
                                hcolor[1] = 0;
                                hcolor[2] = 1;
//                      hcolor[3] = 0.33;
                        } else {
                                hcolor[0] = 0;
                                hcolor[1] = 1;
                                hcolor[2] = 0;
//                      hcolor[3] = 0.33;
                        }

                        hcolor[3] = 0.33f * alphapercent;

                        trap_R_SetColor( hcolor );
                        CG_DrawPic( CHATLOC_X, CHATLOC_Y - ( cgs.teamChatPos - i ) * TINYCHAR_HEIGHT, 640, TINYCHAR_HEIGHT, cgs.media.teamStatusBar );

                        hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
                        hcolor[3] = alphapercent;
                        trap_R_SetColor( hcolor );

                        CG_DrawStringExt( CHATLOC_X + TINYCHAR_WIDTH,
                                                          CHATLOC_Y - ( cgs.teamChatPos - i ) * TINYCHAR_HEIGHT,
                                                          cgs.teamChatMsgs[i % chatHeight], hcolor, qfalse, qfalse,
                                                          TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 0 );
//                      CG_DrawSmallString( CHATLOC_X + SMALLCHAR_WIDTH,
//                              CHATLOC_Y - (cgs.teamChatPos - i)*SMALLCHAR_HEIGHT,
//                              cgs.teamChatMsgs[i % TEAMCHAT_HEIGHT], 1.0F );
                }
// jpw
        }
}


//----(SA)	modified
/*
===================
CG_DrawPickupItem
===================
*/
static void CG_DrawPickupItem( void ) {
	int value;
	float   *fadeColor;
	char pickupText[256];
	float color[4];
        int w = 0;

	value = cg.itemPickup;
	if ( value ) {
		fadeColor = CG_FadeColor( cg.itemPickupTime, 3000 );
		if ( fadeColor ) {
			CG_RegisterItemVisuals( value );

//----(SA) commented out
//			trap_R_SetColor( fadeColor );
//			CG_DrawPic( 8, 380, ICON_SIZE, ICON_SIZE, cg_items[ value ].icon );
//----(SA) end

			//----(SA)	so we don't pick up all sorts of items and have it print "0 <itemname>"
			if ( bg_itemlist[ value ].giType == IT_AMMO || bg_itemlist[ value ].giType == IT_HEALTH || bg_itemlist[value].giType == IT_POWERUP ) {
				if ( bg_itemlist[ value ].world_model[2] ) {   // this is a multi-stage item
					// FIXME: print the correct amount for multi-stage
					Com_sprintf( pickupText, sizeof( pickupText ), "%s", cgs.itemPrintNames[ value ] );
				} else {
					// cs: removing print of the hardcoded values for pickups since there is a dropammo command with an arbitrary amount.
					//if ( bg_itemlist[ value ].gameskillnumber[cg_gameSkill.integer] > 1 ) {
					//	Com_sprintf( pickupText, sizeof( pickupText ), "%i  %s", bg_itemlist[ value ].gameskillnumber[cg_gameSkill.integer], cgs.itemPrintNames[ value ] );
					//} else {
						Com_sprintf( pickupText, sizeof( pickupText ), "%s", cgs.itemPrintNames[value] );
					//}
				}
			} else {
				Com_sprintf( pickupText, sizeof( pickupText ), "%s", cgs.itemPrintNames[value] );
			}

			//----(SA)	trying smaller text
			color[0] = color[1] = color[2] = 1.0;
			color[3] = fadeColor[0];
			//CG_DrawStringExt2( ICON_SIZE + 16, 398, pickupText, color, qfalse, qtrue, 10, 10, 0 );
                        w = CG_DrawStrlen( pickupText ) * 10;
#ifdef LOCALISATION
			CG_DrawStringExt2( 320-(w/2), 398, CG_TranslateString(pickupText), color, qfalse, qtrue, 10, 10, 0 );
#else
			CG_DrawStringExt2( 320-(w/2), 398, pickupText, color, qfalse, qtrue, 10, 10, 0 );
#endif
			//CG_Text_Paint(ICON_SIZE + 16, 398, 2, 0.3f, color, pickupText, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);


			trap_R_SetColor( NULL );
		}
	}
}
//----(SA)	end

/*
=================
CG_DrawNotify
=================
*/
#define NOTIFYLOC_Y 42 // bottom end
#define NOTIFYLOC_X 0

static void CG_DrawNotify( void ) {
        int w, h;
        int i, len; 
        vec4_t hcolor;
        int chatHeight;
        float alphapercent;
        char var[MAX_TOKEN_CHARS];
        float notifytime = 1.0f;

        trap_Cvar_VariableStringBuffer( "con_notifytime", var, sizeof( var ) ); 
        notifytime = atof( var ) * 1000;

        if ( notifytime <= 100.f ) {
                notifytime = 100.0f;
        }    

        chatHeight = NOTIFY_HEIGHT;

        if ( cgs.notifyLastPos != cgs.notifyPos ) {
                if ( cg.time - cgs.notifyMsgTimes[cgs.notifyLastPos % chatHeight] > notifytime ) {
                        cgs.notifyLastPos++;
                }    

                h = ( cgs.notifyPos - cgs.notifyLastPos ) * TINYCHAR_HEIGHT;

                w = 0; 

                for ( i = cgs.notifyLastPos; i < cgs.notifyPos; i++ ) {
                        len = CG_DrawStrlen( cgs.notifyMsgs[i % chatHeight] );
                        if ( len > w ) {
                                w = len; 
                        }    
                }    
                w *= TINYCHAR_WIDTH;
                w += TINYCHAR_WIDTH * 2;

                if ( maxCharsBeforeOverlay <= 0 ) {
                        maxCharsBeforeOverlay = 80;
                }

                for ( i = cgs.notifyPos - 1; i >= cgs.notifyLastPos; i-- ) {
                        alphapercent = 1.0f - ( ( cg.time - cgs.notifyMsgTimes[i % chatHeight] ) / notifytime );
                        if ( alphapercent > 0.5f ) {
                                alphapercent = 1.0f;
                        } else {
                                alphapercent *= 2;
                        }

                        if ( alphapercent < 0.f ) {
                                alphapercent = 0.f;
                        }

                        hcolor[0] = hcolor[1] = hcolor[2] = 1.0;
                        hcolor[3] = alphapercent;
                        trap_R_SetColor( hcolor );

                        CG_DrawStringExt( NOTIFYLOC_X + TINYCHAR_WIDTH,
                                                          NOTIFYLOC_Y - ( cgs.notifyPos - i ) * TINYCHAR_HEIGHT,
                                                          cgs.notifyMsgs[i % chatHeight], hcolor, qfalse, qfalse,
                                                          TINYCHAR_WIDTH, TINYCHAR_HEIGHT, maxCharsBeforeOverlay );
                }
        }
}



/*
===================
CG_DrawHoldableItem
===================
*/
void CG_DrawHoldableItem_old( void ) {
	int value;
	gitem_t *item;

	if ( !cg.holdableSelect ) {
		return;
	}

	item    = BG_FindItemForHoldable( cg.holdableSelect );

	if ( !item ) {
		return;
	}

	value   = cg.predictedPlayerState.holdable[cg.holdableSelect];

	if ( value ) {

		trap_R_SetColor( NULL );

		CG_RegisterItemVisuals( item - bg_itemlist );

		if ( cg.holdableSelect == HI_WINE ) {
			if ( value > 3 ) {
				value = 3;  // 3 stages to icon, just draw full if beyond 'full'

			}
			//----(SA)	trying smaller text
			//----(SA)	and off to the right side of the HUD
//			CG_DrawPic( 100, (SCREEN_HEIGHT-ICON_SIZE)-8, ICON_SIZE/2, ICON_SIZE, cg_items[item - bg_itemlist].icons[2-(value-1)] );
			CG_DrawPic( 606, 366, 24, 48, cg_items[item - bg_itemlist].icons[2 - ( value - 1 )] );

		} else {
//			CG_DrawPic( 100, (SCREEN_HEIGHT-ICON_SIZE)-8, ICON_SIZE/2, ICON_SIZE, cg_items[item - bg_itemlist].icons[0] );
			CG_DrawPic( 606, 366, 24, 48, cg_items[item - bg_itemlist].icons[0] );

		}

		// draw the selection box so it's not just floating in space
		CG_DrawPic( 606 - 4, 366 - 4, 32, 56, cgs.media.selectShader );
	}
}
/*
		if(cg.holdableSelect == HI_WINE) {
			if(value > 3)
				value = 3;	// 3 stages to icon, just draw full if beyond 'full'

			CG_DrawPic( 598 + 16, 366, 16, 32, cg_items[item - bg_itemlist].icons[2-(value-1)] );
			CG_DrawPic( (598 + 16)-4, 366-4, 24, 40, cgs.media.selectShader );

		} else {
			CG_DrawPic( 598, 366, 32, 32, cg_items[item - bg_itemlist].icons[0] );
			CG_DrawPic( 598-4, 366-4, 40, 40, cgs.media.selectShader );
		}
*/

/*
===================
CG_DrawReward
===================
*/
static void CG_DrawReward( void ) {
	float   *color;
	int i;
	float x, y;

	if ( !cg_drawRewards.integer ) {
		return;
	}
	color = CG_FadeColor( cg.rewardTime, REWARD_TIME );
	if ( !color ) {
		return;
	}

	trap_R_SetColor( color );
	y = 56;
	x = 320 - cg.rewardCount * ICON_SIZE / 2;
	for ( i = 0 ; i < cg.rewardCount ; i++ ) {
		CG_DrawPic( x, y, ICON_SIZE - 4, ICON_SIZE - 4, cg.rewardShader );
		x += ICON_SIZE;
	}
	trap_R_SetColor( NULL );
}


/*
===============================================================================

LAGOMETER

===============================================================================
*/

#define LAG_SAMPLES     128


typedef struct {
	int frameSamples[LAG_SAMPLES];
	int frameCount;
	int snapshotFlags[LAG_SAMPLES];
	int snapshotSamples[LAG_SAMPLES];
	int snapshotCount;
} lagometer_t;

lagometer_t lagometer;

/*
==============
CG_AddLagometerFrameInfo

Adds the current interpolate / extrapolate bar for this frame
==============
*/
void CG_AddLagometerFrameInfo( void ) {
	int offset;

	offset = cg.time - cg.latestSnapshotTime;
	lagometer.frameSamples[ lagometer.frameCount & ( LAG_SAMPLES - 1 ) ] = offset;
	lagometer.frameCount++;
}

/*
==============
CG_AddLagometerSnapshotInfo

Each time a snapshot is received, log its ping time and
the number of snapshots that were dropped before it.

Pass NULL for a dropped packet.
==============
*/
void CG_AddLagometerSnapshotInfo( snapshot_t *snap ) {
	// dropped packet
	if ( !snap ) {
		lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = -1;
		lagometer.snapshotCount++;
		return;
	}

	// add this snapshot's info
	lagometer.snapshotSamples[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = snap->ping;
	lagometer.snapshotFlags[ lagometer.snapshotCount & ( LAG_SAMPLES - 1 ) ] = snap->snapFlags;
	lagometer.snapshotCount++;
}

/*
==============
CG_DrawDisconnect

Should we draw something differnet for long lag vs no packets?
==============
*/
static void CG_DrawDisconnect( void ) {
	float x, y;
	int cmdNum;
	usercmd_t cmd;
	const char      *s;
	int w;          // bk010215 - FIXME char message[1024];

        if (cg.snap->ps.pm_flags & PMF_FOLLOW)
                return;

	// draw the phone jack if we are completely past our buffers
	cmdNum = trap_GetCurrentCmdNumber() - CMD_BACKUP + 1;
	trap_GetUserCmd( cmdNum, &cmd );
	if ( cmd.serverTime <= cg.snap->ps.commandTime
		 || cmd.serverTime > cg.time ) { // special check for map_restart // bk 0102165 - FIXME
		return;
	}

	// also add text in center of screen
	s = "Connection Interrupted"; // bk 010215 - FIXME
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w / 2, 100, s, 1.0F );

	// blink the icon
	if ( ( cg.time >> 9 ) & 1 ) {
		return;
	}

	x = 640 - 48;
	y = 480 - 48;

	CG_DrawPic( x, y, 48, 48, trap_R_RegisterShader( "gfx/2d/net.tga" ) );
}


#define MAX_LAGOMETER_PING  900
#define MAX_LAGOMETER_RANGE 300

/*
==============
CG_DrawLagometer
==============
*/
static void CG_DrawLagometer( void ) {
	int a, x, y, i;
	float v;
	float ax, ay, aw, ah, mid, range;
	int color;
	float vscale;

	if ( !cg_lagometer.integer || cgs.localServer ) {
//	if(0) {
		CG_DrawDisconnect();
		return;
	}

	//
	// draw the graph
	//
	x = 640 - 48;
	y = 480 - 48;

	trap_R_SetColor( NULL );
	CG_DrawPic( x, y, 48, 48, cgs.media.lagometerShader );

	ax = x;
	ay = y;
	aw = 48;
	ah = 48;
	CG_AdjustFrom640( &ax, &ay, &aw, &ah );

	color = -1;
	range = ah / 3;
	mid = ay + range;

	vscale = range / MAX_LAGOMETER_RANGE;

	// draw the frame interpoalte / extrapolate graph
	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.frameCount - 1 - a ) & ( LAG_SAMPLES - 1 );
		v = lagometer.frameSamples[i];
		v *= vscale;
		if ( v > 0 ) {
			if ( color != 1 ) {
				color = 1;
				trap_R_SetColor( g_color_table[ColorIndex( COLOR_YELLOW )] );
			}
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 2 ) {
				color = 2;
				trap_R_SetColor( g_color_table[ColorIndex( COLOR_BLUE )] );
			}
			v = -v;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, mid, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	// draw the snapshot latency / drop graph
	range = ah / 2;
	vscale = range / MAX_LAGOMETER_PING;

	for ( a = 0 ; a < aw ; a++ ) {
		i = ( lagometer.snapshotCount - 1 - a ) & ( LAG_SAMPLES - 1 );
		v = lagometer.snapshotSamples[i];
		if ( v > 0 ) {
			if ( lagometer.snapshotFlags[i] & SNAPFLAG_RATE_DELAYED ) {
				if ( color != 5 ) {
					color = 5;  // YELLOW for rate delay
					trap_R_SetColor( g_color_table[ColorIndex( COLOR_YELLOW )] );
				}
			} else {
				if ( color != 3 ) {
					color = 3;
					trap_R_SetColor( g_color_table[ColorIndex( COLOR_GREEN )] );
				}
			}
			v = v * vscale;
			if ( v > range ) {
				v = range;
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - v, 1, v, 0, 0, 0, 0, cgs.media.whiteShader );
		} else if ( v < 0 ) {
			if ( color != 4 ) {
				color = 4;      // RED for dropped snapshots
				trap_R_SetColor( g_color_table[ColorIndex( COLOR_RED )] );
			}
			trap_R_DrawStretchPic( ax + aw - a, ay + ah - range, 1, range, 0, 0, 0, 0, cgs.media.whiteShader );
		}
	}

	trap_R_SetColor( NULL );

	if ( cg_nopredict.integer || cg_synchronousClients.integer ) {
		CG_DrawBigString( ax, ay, "snc", 1.0 );
	}

	CG_DrawDisconnect();
}

void CG_DrawFreeze(void) {
        int w, y, x;
        vec4_t color;
        char *linebuffer = va("You are frozen");

        if (!(cg.snap->ps.eFlags & EF_FROZEN))
                return;

        y = 320;

        color[0] = 1;
        color[1] = 1;
        color[2] = 1;
        color[3] = 1;

        w = BIGCHAR_WIDTH * CG_DrawStrlen( linebuffer );
 
        x = ( SCREEN_WIDTH - w ) / 2;
 
        CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue, BIGCHAR_WIDTH, BIGCHAR_WIDTH, 0 );
}


/*
===============================================================================

CENTER PRINTING

===============================================================================
*/

/*
==============
CG_CenterPrint

Called for important messages that should stay in the center of the screen
for a few moments
==============
*/
void CG_CenterPrint( const char *str, int y, int charWidth ) {
	unsigned char   *s;

//----(SA)	added translation lookup
#ifdef LOCALISATION
	Q_strncpyz( (char *)cg.centerPrint, CG_TranslateString( (char*)str ), sizeof( cg.centerPrint ) );
#else
	Q_strncpyz( (char *)cg.centerPrint, CG_translateString( (char*)str ), sizeof( cg.centerPrint ) );
#endif
//----(SA)	end


	cg.centerPrintTime = cg.time;
	cg.centerPrintY = y;
	cg.centerPrintCharWidth = charWidth;

	// count the number of lines for centering
	cg.centerPrintLines = 1;
	s = cg.centerPrint;
	while ( *s ) {
		if ( *s == '\n' ) {
			cg.centerPrintLines++;
		}
		if ( !Q_strncmp( (char *)s, "\\n", 1 ) ) {
			cg.centerPrintLines++;
			s++;
		}
		s++;
	}
}


/*
===================
CG_DrawCenterString
===================
*/
static void CG_DrawCenterString( void ) {
	char    *start;
	int l;
	int x, y, w;
	float   *color;

	if ( !cg.centerPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.centerPrintTime, 1000 * cg_centertime.value );
	if ( !color ) {
		return;
	}

	trap_R_SetColor( color );

	start = (char *)cg.centerPrint;

	y = cg.centerPrintY - cg.centerPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < 40; l++ ) {
			if ( !start[l] || start[l] == '\n' || !Q_strncmp( &start[l], "\\n", 1 ) ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.centerPrintCharWidth * CG_DrawStrlen( linebuffer );

		x = ( SCREEN_WIDTH - w ) / 2;

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue, cg.centerPrintCharWidth, (int)( cg.centerPrintCharWidth * 1.5 ), 0 );
//		CG_Text_Paint(x, y, 2, 0.3f, color, linebuffer, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);

//		y += cg.centerPrintCharWidth * 1.5;
		y += cg.centerPrintCharWidth * 2;

//		while ( *start && ( *start != '\n' ) && !Q_strncmp(start, "\\n", 1) ) {
		while ( *start && ( *start != '\n' ) ) {
			if ( !Q_strncmp( start, "\\n", 1 ) ) {
				start++;
				break;
			}
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}



/*
================================================================================

CROSSHAIRS

================================================================================
*/


/*
==============
CG_DrawWeapReticle
==============
*/
static void CG_DrawWeapReticle( void ) {
	int weap;
	vec4_t color = {0, 0, 0, 1};
	vec4_t snoopercolor = {0.7, .8, 0.7, 0};    // greenish
	float snooperBrightness;
	float x = 80, y, w = 240, h = 240;

	CG_AdjustFrom640( &x, &y, &w, &h );

	weap = cg.weaponSelect;


	if ( weap == WP_SNIPERRIFLE ) {


		// sides
		CG_FillRect( 0, 0, 80, 480, color );
		CG_FillRect( 560, 0, 80, 480, color );

		// center
		if ( cgs.media.reticleShaderSimpleQ ) {
			trap_R_DrawStretchPic( x, 0, w, h, 0, 0, 1, 1, cgs.media.reticleShaderSimpleQ );    // tl
			trap_R_DrawStretchPic( x + w, 0, w, h, 1, 0, 0, 1, cgs.media.reticleShaderSimpleQ );  // tr
			trap_R_DrawStretchPic( x, h, w, h, 0, 1, 1, 0, cgs.media.reticleShaderSimpleQ );    // bl
			trap_R_DrawStretchPic( x + w, h, w, h, 1, 1, 0, 0, cgs.media.reticleShaderSimpleQ );  // br
		}

		// hairs
		CG_FillRect( 84, 239, 177, 2, color );   // left
		CG_FillRect( 320, 242, 1, 58, color );   // center top
		CG_FillRect( 319, 300, 2, 178, color );  // center bot
		CG_FillRect( 380, 239, 177, 2, color );  // right
	} else if ( weap == WP_SNOOPERSCOPE ) {
		// sides
		CG_FillRect( 0, 0, 80, 480, color );
		CG_FillRect( 560, 0, 80, 480, color );

		// center

//----(SA)	added
		// DM didn't like how bright it gets
		snooperBrightness = Com_Clamp( 0.0f, 1.0f, cg_reticleBrightness.value );
		snoopercolor[0] *= snooperBrightness;
		snoopercolor[1] *= snooperBrightness;
		snoopercolor[2] *= snooperBrightness;
		trap_R_SetColor( snoopercolor );
//----(SA)	end

		if ( cgs.media.snooperShaderSimple ) {
			CG_DrawPic( 80, 0, 480, 480, cgs.media.snooperShaderSimple );
		}

		// hairs

		CG_FillRect( 310, 120, 20, 1, color );   //					-----
		CG_FillRect( 300, 160, 40, 1, color );   //				-------------
		CG_FillRect( 310, 200, 20, 1, color );   //					-----

		CG_FillRect( 140, 239, 360, 1, color );  // horiz ---------------------------

		CG_FillRect( 310, 280, 20, 1, color );   //					-----
		CG_FillRect( 300, 320, 40, 1, color );   //				-------------
		CG_FillRect( 310, 360, 20, 1, color );   //					-----



		CG_FillRect( 400, 220, 1, 40, color );   // l

		CG_FillRect( 319, 60, 1, 360, color );   // vert

		CG_FillRect( 240, 220, 1, 40, color );   // r
	} else if ( weap == WP_FG42SCOPE ) {
		// sides
		CG_FillRect( 0, 0, 80, 480, color );
		CG_FillRect( 560, 0, 80, 480, color );

		// center
		if ( cgs.media.reticleShaderSimpleQ ) {
			trap_R_DrawStretchPic( x,   0, w, h, 0, 0, 1, 1, cgs.media.reticleShaderSimpleQ );  // tl
			trap_R_DrawStretchPic( x + w, 0, w, h, 1, 0, 0, 1, cgs.media.reticleShaderSimpleQ );  // tr
			trap_R_DrawStretchPic( x,   h, w, h, 0, 1, 1, 0, cgs.media.reticleShaderSimpleQ );  // bl
			trap_R_DrawStretchPic( x + w, h, w, h, 1, 1, 0, 0, cgs.media.reticleShaderSimpleQ );  // br
		}

		// hairs
		CG_FillRect( 84, 239, 150, 3, color );   // left
		CG_FillRect( 234, 240, 173, 1, color );  // horiz center
		CG_FillRect( 407, 239, 150, 3, color );  // right


		CG_FillRect( 319, 2,   3, 151, color );  // top center top
		CG_FillRect( 320, 153, 1, 114, color );  // top center bot

		CG_FillRect( 320, 241, 1, 87, color );   // bot center top
		CG_FillRect( 319, 327, 3, 151, color );  // bot center bot
	}
}


//----(SA)	removed (9/8/2001)

/*
==============
CG_DrawBinocReticle
==============
*/
static void CG_DrawBinocReticle( void ) {
	// an alternative.  This gives nice sharp lines at the expense of a few extra polys
	vec4_t color = {0, 0, 0, 1};
	float x, y, w = 320, h = 240;

	if ( cgs.media.binocShaderSimpleQ ) {
		CG_AdjustFrom640( &x, &y, &w, &h );
		trap_R_DrawStretchPic( 0, 0, w, h, 0, 0, 1, 1, cgs.media.binocShaderSimpleQ );  // tl
		trap_R_DrawStretchPic( w, 0, w, h, 1, 0, 0, 1, cgs.media.binocShaderSimpleQ );  // tr
		trap_R_DrawStretchPic( 0, h, w, h, 0, 1, 1, 0, cgs.media.binocShaderSimpleQ );  // bl
		trap_R_DrawStretchPic( w, h, w, h, 1, 1, 0, 0, cgs.media.binocShaderSimpleQ );  // br
	}

	CG_FillRect( 146, 239, 348, 1, color );

	CG_FillRect( 188, 234, 1, 13, color );   // ll
	CG_FillRect( 234, 226, 1, 29, color );   // l
	CG_FillRect( 274, 234, 1, 13, color );   // lr
	CG_FillRect( 320, 213, 1, 55, color );   // center
	CG_FillRect( 360, 234, 1, 13, color );   // rl
	CG_FillRect( 406, 226, 1, 29, color );   // r
	CG_FillRect( 452, 234, 1, 13, color );   // rr
}

void CG_FinishWeaponChange( int lastweap, int newweap ); // JPW NERVE


/*
=================
CG_DrawCrosshair
=================
*/
static void CG_DrawCrosshair( void ) {
	float w, h;
	qhandle_t hShader;
	float f;
	float x, y;
	int weapnum;                // DHM - Nerve
	vec4_t hcolor = {1, 1, 1, 0};
	qboolean friendInSights = qfalse;

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if ( cg_crosshairHealth.integer ) {
		CG_ColorForHealth( hcolor );
	}

	hcolor[3] = cg_crosshairAlpha.value;    //----(SA)	added


	// on mg42
	if ( cg.snap->ps.eFlags & EF_MG42_ACTIVE ) {
		hcolor[0] = hcolor[1] = hcolor[2] = 0.0f;
		hcolor[3] = 0.6f;
		// option 1
//		CG_FillRect (300, 240, 40, 2, hcolor);	// horizontal
//		CG_FillRect (319, 242, 2, 16, hcolor);	// vertical

		// option 2
		CG_FillRect( 305, 240, 30, 2, hcolor );  // horizontal
		CG_FillRect( 314, 256, 12, 2, hcolor );  // horizontal2
		CG_FillRect( 319, 242, 2, 32, hcolor );  // vertical

		return;
	}

	friendInSights = (qboolean)( cg.snap->ps.serverCursorHint == HINT_PLYR_FRIEND );  //----(SA)	added

	weapnum = cg.weaponSelect;

	switch ( weapnum ) {

		// weapons that get no reticle
	case WP_NONE:       // no weapon, no crosshair
	case WP_GARAND:
		if ( cg.zoomedBinoc ) {
			CG_DrawBinocReticle();
		}
		return;
		break;

		// special reticle for weapon
	case WP_KNIFE:		
		if ( cg.zoomedBinoc ) {
			CG_DrawBinocReticle();
			return;
		// L0 - Crosshair shouldn't be shown at all when holding a knife.
		} else {
			return;
		}
		/*

		// no crosshair when looking at exits
		if ( cg.snap->ps.serverCursorHint >= HINT_EXIT && cg.snap->ps.serverCursorHint <= HINT_NOEXIT_FAR ) {
			return;
		}

		if ( !friendInSights ) {
			if ( !cg.snap->ps.leanf ) {     // no crosshair while leaning
				CG_FillRect( 319, 239, 2, 2, hcolor );      // dot
			}
			return;
		}
		break;
		*/
	case WP_SNIPERRIFLE:
	case WP_SNOOPERSCOPE:
	case WP_FG42SCOPE:
		CG_DrawWeapReticle();
		return;

	default:
		break;
	}

	// using binoculars
	if ( cg.zoomedBinoc ) {
		CG_DrawBinocReticle();
		return;
	}


	// mauser only gets crosshair if you don't have the scope (I don't like this, but it's a test)
	if ( cg.weaponSelect == WP_MAUSER ) {
		if ( COM_BitCheck( cg.predictedPlayerState.weapons, WP_SNIPERRIFLE ) ) {
			return;
		}
	}


	if ( !cg_drawCrosshair.integer ) {  //----(SA)	moved down so it doesn't keep the scoped weaps from drawing reticles
		return;
	}

	// no crosshair while leaning
	if ( cg.snap->ps.leanf ) {
		return;
	}

	// no crosshair when looking at exits
	if ( cg.snap->ps.serverCursorHint >= HINT_EXIT && cg.snap->ps.serverCursorHint <= HINT_NOEXIT_FAR ) {
		return;
	}

	if ( cg_paused.integer ) {
		// no draw if any menu's are up	 (or otherwise paused)
		return;
	}

	// set color based on health
	if ( cg_crosshairHealth.integer ) {
		trap_R_SetColor( hcolor );
	} else {
		trap_R_SetColor( NULL );
	}

	w = h = cg_crosshairSize.value;
/*
	// pulse the size of the crosshair when picking up items
	f = cg.time - cg.itemPickupBlendTime;
	if ( f > 0 && f < ITEM_BLOB_TIME ) {
		f /= ITEM_BLOB_TIME;
		w *= ( 1 + f );
		h *= ( 1 + f );
	}
*/
	// L0 - crosshair pulsing..
	if (!cg_solidCrosshair.integer) {
		// RF, crosshair size represents aim spread	
		f = (float)cg.snap->ps.aimSpreadScale / 255.0;
		w *= ( 1 + f*2.0 );
		h *= ( 1 + f*2.0 );
	} // end

	x = cg_crosshairX.integer;
	y = cg_crosshairY.integer;
	CG_AdjustFrom640( &x, &y, &w, &h );

//----(SA)	modified
	if ( friendInSights ) {
		hShader = cgs.media.crosshairFriendly;
	} else {
		hShader = cgs.media.crosshairShader[ cg_drawCrosshair.integer % NUM_CROSSHAIRS ];
	}
//----(SA)	end

	// NERVE - SMF - modified, fixes crosshair offset in shifted/scaled 3d views
	// (SA) also breaks scaled view...
	trap_R_DrawStretchPic(  x + cg.refdef.x + 0.5 * ( cg.refdef.width - w ),
							y + cg.refdef.y + 0.5 * ( cg.refdef.height - h ),
							w, h, 0, 0, 1, 1, hShader );
}



/*
=================
CG_ScanForCrosshairEntity
=================
*/
static void CG_ScanForCrosshairEntity( void ) {
	trace_t trace;
//	gentity_t	*traceEnt;
	vec3_t start, end;
	int content;

	// DHM - Nerve :: We want this in multiplayer
	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		return; //----(SA)	don't use any scanning at the moment.

	}
	VectorCopy( cg.refdef.vieworg, start );
	VectorMA( start, 4096, cg.refdef.viewaxis[0], end );    //----(SA)	changed from 8192

	CG_Trace( &trace, start, vec3_origin, vec3_origin, end,
			  cg.snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_BODY | CONTENTS_ITEM );

//----(SA)	allow targets that aren't clients
//	if ( trace.entityNum >= MAX_CLIENTS ) {
//		return;
//	}

//	traceEnt = &g_entities[trace.entityNum];


	// if the player is in fog, don't show it
	content = trap_CM_PointContents( trace.endpos, 0 );
	if ( content & CONTENTS_FOG ) {
		return;
	}

	// if the player is invisible, don't show it
	if ( cg_entities[ trace.entityNum ].currentState.powerups & ( 1 << PW_INVIS ) ) {
		return;
	}

	// update the fade timer
	cg.crosshairClientNum = trace.entityNum;
	cg.crosshairClientTime = cg.time;
}



/*
==============
CG_DrawDynamiteStatus
==============
*/
static void CG_DrawDynamiteStatus( void ) {
	float color[4];
	char        *name;
	int timeleft;
	float w;

	if ( cg.snap->ps.weapon != WP_DYNAMITE ) {
		return;
	}

	if ( cg.snap->ps.grenadeTimeLeft <= 0 ) {
		return;
	}

	timeleft = cg.snap->ps.grenadeTimeLeft;

//	color = g_color_table[ColorIndex(COLOR_RED)];
	color[0] = color[3] = 1.0f;

	// fade red as it pulses past seconds
	color[1] = color[2] = 1.0f - ( (float)( timeleft % 1000 ) * 0.001f );

	if ( timeleft < 300 ) {        // fade up the text
		color[3] = (float)timeleft / 300.0f;
	}

	trap_R_SetColor( color );

	timeleft *= 5;
	timeleft -= ( timeleft % 5000 );
	timeleft += 5000;
	timeleft /= 1000;

	name = va( "Timer: %d", timeleft );
	w = CG_DrawStrlen( name ) * BIGCHAR_WIDTH;

	color[3] *= cg_hudAlpha.value;
	CG_DrawBigStringColor( 320 - w / 2, 170, name, color );

	trap_R_SetColor( NULL );
}



/*
==============
CG_CheckForCursorHints
==============
*/
void CG_CheckForCursorHints( void ) {

	if ( cg.renderingThirdPerson ) {
		return;
	}

	if ( cg.snap->ps.serverCursorHint != HINT_NONE ) { // let the client remember what was last looked at (for fading out)
		cg.cursorHintTime = cg.time;
		cg.cursorHintFade = cg_hintFadeTime.integer;    // fade out time
		cg.cursorHintIcon = cg.snap->ps.serverCursorHint;
		cg.cursorHintValue = cg.snap->ps.serverCursorHintVal;
	}

	// (SA) (8/14/01) removed all the client-side stuff.  don't think it's really necessary anymore
}


/*
=====================
CG_DrawCrosshairNames
=====================
*/

static void CG_DrawCoopCrosshairNames( void ) {
        float       *color;
        char        *name;
        float w;
        // NERVE - SMF
        const char  *s;
        int playerHealth, val; 
        vec4_t c;
        float barFrac;
        int armor;
        // -NERVE - SMF
        centity_t *cent;

        if ( cg_drawCrosshair.integer < 0 ) {
                return;
        }    
        if ( !cg_drawCrosshairNames.integer ) {
                return;
        }    
        if ( cg.renderingThirdPerson ) {
                return;
        }    

        // Ridah
        if ( cg_gameType.integer == GT_SINGLE_PLAYER ) {
                return;
        }    
        // done.

        // scan the known entities to see if the crosshair is sighted on one
        CG_ScanForCrosshairEntity();

        // draw the name of the player being looked at
        color = CG_FadeColor( cg.crosshairClientTime, 1000 );

        if ( !color ) {
                trap_R_SetColor( NULL );
                return;
        }

        // NERVE - SMF
        if ( cg.crosshairClientNum > MAX_CLIENTS ) {
                return;
        }

        // we only want to see players on our team
        if ( cgs.clientinfo[cg.snap->ps.clientNum].team != TEAM_SPECTATOR
                 && cgs.clientinfo[ cg.crosshairClientNum ].team != cgs.clientinfo[cg.snap->ps.clientNum].team ) {
                return;
        }

        cent = &cg_entities[ cg.crosshairClientNum ];

        // fretn: todo make this look nice
        if (cent->currentState.eFlags & EF_FROZEN) {
                s = va("Player is frozen.");
                w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
                CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );

                s = va("Stab him with knife to respawn him.");
                w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;
                CG_DrawSmallStringColor( 320 - w / 2, 170 + SMALLCHAR_HEIGHT + 5, s, color );
                return;
        }

        playerHealth = cgs.clientinfo[ cg.crosshairClientNum ].health;
        
        if (playerHealth <= 0)
                return;

        val = cgs.clientinfo[ cg.crosshairClientNum ].curWeapon;
        name = cgs.clientinfo[ cg.crosshairClientNum ].name;
        armor = cgs.clientinfo[ cg.crosshairClientNum ].armor;

        //s = va( "[%s] %s", CG_TranslateString( playerClass ), name );
        //s = va( "[%s] %s", playerClass, name );
        if (val == WP_KNIFE)
                s = va( "%s", name );
        else if (val == WP_GRENADE_LAUNCHER || val == WP_GRENADE_PINEAPPLE || val == WP_DYNAMITE || val == WP_FLAMETHROWER || val == WP_TESLA)
                s = va( "[%d] %s", cgs.clientinfo[ cg.crosshairClientNum ].ammoClip, name );
        else
                s = va( "[%d/%d] %s", cgs.clientinfo[ cg.crosshairClientNum ].ammoClip, cgs.clientinfo[ cg.crosshairClientNum ].ammo, name );


        //s = va( "[%d]%s",armor , name );
        if ( !s ) {
                return;
        }
        w = CG_DrawStrlen( s ) * SMALLCHAR_WIDTH;

        // draw the name and class
        CG_DrawSmallStringColor( 320 - w / 2, 170, s, color );

        // health bar
        barFrac = (float)playerHealth / 100;

        if ( barFrac > 1.0 ) {
                barFrac = 1.0;
        } else if ( barFrac < 0 ) {
                barFrac = 0;
        }

        c[0] = 1.0f;
        c[1] = c[2] = barFrac;
        c[3] = 0.25 + barFrac * 0.5 * color[3];

        CG_FilledBar( 320 - w / 2, 190, 110, 10, c, NULL, NULL, barFrac, 16 );


        // armor bar
        if (cg_drawCrosshairNames.integer == 2) {
                barFrac = (float)armor / 100;

                if ( barFrac > 1.0 ) {
                        barFrac = 1.0;
                } else if ( barFrac < 0 ) {
                        barFrac = 0;
                }

                c[0] = 1.0f;
                c[1] = c[2] = barFrac;
                c[3] = 0.25 + barFrac * 0.5 * color[3];

                CG_FilledBar( 320 - w / 2, 205, 110, 10, c, NULL, NULL, barFrac, 16 );
        }

        trap_R_SetColor( NULL );
}

static void CG_DrawCrosshairNames( void ) {
	float       *color;
	vec4_t teamColor;           // NERVE - SMF
	char        *name;
	float w;

	if ( !cg_drawCrosshair.integer ) {
		return;
	}
	if ( !cg_drawCrosshairNames.integer ) {
		return;
	}
	if ( cg.renderingThirdPerson ) {
		return;
	}

	// Ridah
	if ( cg_gameType.integer == GT_SINGLE_PLAYER ) {
		return;
	}
	// done.

	// scan the known entities to see if the crosshair is sighted on one
	CG_ScanForCrosshairEntity();

	// draw the name of the player being looked at
	color = CG_FadeColor( cg.crosshairClientTime, 1000 );

	if ( !color ) {
		trap_R_SetColor( NULL );
		return;
	}

	// NERVE - SMF - use fade alpha but color text according to teams
	teamColor[3] = color[3];

	// NERVE - SMF - no longer identify opposing side, so just use green now
//	if ( cgs.clientinfo[ cg.crosshairClientNum ].team != cgs.clientinfo[ cg.clientNum ].team )
//		VectorSet( teamColor, 0.7608, 0.1250, 0.0859 );			// LIGHT-RED
//	else
	VectorSet( teamColor, 0.1250, 0.7608, 0.0859 );             // LIGHT-GREEN

	trap_R_SetColor( teamColor );
	// -NERVE - SMF

	name = cgs.clientinfo[ cg.crosshairClientNum ].name;
	w = CG_DrawStrlen( va( "%s", name ) ) * BIGCHAR_WIDTH;
//	CG_DrawBigString( 320 - w / 2, 170, name, color[3] * 0.5 );

	// NERVE - SMF
	if ( strlen( name ) ) {
		if ( ( cgs.clientinfo[ cg.crosshairClientNum ].team == TEAM_RED ) &&
			 ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_RED ) ) { // JPW NERVE -- only show same team info so people can't pan-search
			CG_DrawBigStringColor( 320 - w / 2, 170, va( "Axis: %s", name ), teamColor );
		} else if ( ( cgs.clientinfo[ cg.crosshairClientNum ].team == TEAM_BLUE ) &&
					( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_BLUE ) ) { // JPW NERVE -- so's we can't find snipers for free
			CG_DrawBigStringColor( 320 - w / 2, 170, va( "Ally: %s", name ), teamColor );
		} else {
			CG_DrawBigStringColor( 320 - w / 2, 170, va( "%s", name ), teamColor );
                }
	}
	// -NERVE - SMF

	trap_R_SetColor( NULL );
}



//==============================================================================

/*
=================
CG_DrawSpectator
=================
*/
static void CG_DrawSpectator( void ) {
	CG_DrawBigString( 320 - 9 * 8, 440, "SPECTATOR", 1.0F );
}

/*
=================
CG_DrawVote

L0 - DrawVote looks ugly as hell..I've port it from mp.

NOTE: Vote sounds are set in game logic (c_cmds.c /callvote)..
=================
*/
char* BindingFromName( const char *cvar );
static void CG_DrawVote( void ) {
	char *s;
	int sec;
	char str1[32], str2[32];
	float color[4] = { 1, 1, 0, 1 };

	if ( !cgs.voteTime ) {
		return;
	}

	Q_strncpyz( str1, BindingFromName( "vote yes" ), 32 );
	if ( !Q_stricmp( str1, "???" ) ) {
		Q_strncpyz( str1, "vote yes", 32 );
	}
	Q_strncpyz( str2, BindingFromName( "vote no" ), 32 );
	if ( !Q_stricmp( str2, "???" ) ) {
		Q_strncpyz( str2, "vote no", 32 );
	}

	// play a talk beep whenever it is modified
	if ( cgs.voteModified ) {
		cgs.voteModified = qfalse;	
	}

	sec = ( VOTE_TIME - ( cg.time - cgs.voteTime ) ) / 1000;
	if ( sec < 0 ) {
		sec = 0;
	}

	if ( !( cg.snap->ps.eFlags & EF_VOTED ) ) {
// L0 - This has to get removed eventually as it'a mess...
#ifdef LOCALISATION
		s = va( CG_TranslateString( "VOTE(%i):%s" ), sec, cgs.voteString );
#else
		s = va( "VOTE(%i):%s", sec, cgs.voteString );
#endif	
		// Should we push this down?
		CG_DrawStringExt( 8, 200, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 60 );

#ifdef LOCALISATION
		s = va( CG_TranslateString( "YES(%s):%i, NO(%s):%i" ), str1, cgs.voteYes, str2, cgs.voteNo );
#else
		s = va( "YES(%s):%i, NO(%s):%i", str1, cgs.voteYes, str2, cgs.voteNo );
#endif
		CG_DrawStringExt( 8, 214, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 60 );
	} else {

#ifdef LOCALISATION
		s = va( CG_TranslateString( "Y:%i, N:%i" ), cgs.voteYes, cgs.voteNo );
#else
		s = va( "Y:%i, N:%i", cgs.voteYes, cgs.voteNo );
#endif
		CG_DrawStringExt( 8, 214, s, color, qtrue, qfalse, TINYCHAR_WIDTH, TINYCHAR_HEIGHT, 20 );
	}
}

/*
=================
CG_DrawIntermission
=================
*/
static void CG_DrawIntermission( void ) {
	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		CG_DrawCenterString();
		return;
	}

	cg.scoreFadeTime = cg.time;
	CG_DrawScoreboard();
}

// NERVE - SMF
/*
=================
CG_ActivateLimboMenu
=================
*/
// TTimo: unused
/*
static void CG_ActivateLimboMenu( void ) {
	static qboolean latch = qfalse;
	qboolean test;
	char buf[32];

	if ( cgs.gametype != GT_WOLF )
		return;

	// should we open the limbo menu
	test = cg.snap->ps.pm_flags & PMF_LIMBO || cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR;

	if ( test && !latch ) {
		trap_SendConsoleCommand( "startLimboMode\n" );
		trap_SendConsoleCommand( "OpenLimboMenu\n" );
		latch = qtrue;
	}
	else if ( !test && latch ) {
		trap_SendConsoleCommand( "stopLimboMode\n" );
		trap_SendConsoleCommand( "CloseLimboMenu\n" );
		latch = qfalse;
	}

	// set the limbo state
	trap_Cvar_VariableStringBuffer( "ui_limboMode", buf, sizeof( buf ) );

	if ( atoi( buf ) )
		cg.limboMenu = qtrue;
	else
		cg.limboMenu = qfalse;
}
*/
// -NERVE - SMF

/*
=================
CG_DrawFollow
=================
*/
static qboolean CG_DrawFollow( void ) {
	float x;
	vec4_t color;
	const char  *name;
	char deploytime[128];        // JPW NERVE

	if ( !( cg.snap->ps.pm_flags & PMF_FOLLOW ) ) {
		return qfalse;
	}
	color[0] = 1;
	color[1] = 1;
	color[2] = 1;
	color[3] = 1;

// JPW NERVE -- if in limbo, show different follow message
	if ( cg.snap->ps.pm_flags & PMF_LIMBO ) {
//		CG_Printf("following %s\n",cgs.clientinfo[ cg.snap->ps.clientNum ].name);
		color[1] = 0;
		color[2] = 0;
		if ( cgs.clientinfo[cg.snap->ps.clientNum].team == TEAM_RED ) {
			//sprintf( deploytime,"Deploying in %d seconds", (int)( (float)( cg_redlimbotime.integer - ( cg.time % cg_redlimbotime.integer ) ) * 0.001f ) );
		} else {
			//sprintf( deploytime,"Deploying in %d seconds", (int)( (float)( cg_bluelimbotime.integer - ( cg.time % cg_bluelimbotime.integer ) ) * 0.001f ) );
		}

		if ( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] ) {
			if (cg_limbotime.integer > 0) {
				sprintf( deploytime,"Deploying in %d seconds", (int)( (float)( cg_limbotime.integer - ( cg.time % cg_limbotime.integer ) ) * 0.001f ) );
				x = 0.5 * ( 640 - BIGCHAR_WIDTH * strlen( deploytime ) ); //CG_DrawStrlen( deploytime ) );
				CG_DrawStringExt( x, 24, deploytime, color, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
			}
		} else {
			sprintf( deploytime,"No lives left" );
			x = 0.5 * ( 640 - BIGCHAR_WIDTH * strlen( deploytime ) ); //CG_DrawStrlen( deploytime ) );
			CG_DrawStringExt( x, 24, deploytime, color, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
		}

		if ( cg.snap->ps.persistant[PERS_RESPAWNS_LEFT] ) {
			sprintf( deploytime,"(Following %s)",cgs.clientinfo[ cg.snap->ps.clientNum ].name );
			x = 0.5 * ( 640 - BIGCHAR_WIDTH * strlen( deploytime ) ); //CG_DrawStrlen( deploytime ) );
			CG_DrawStringExt( x, 48, deploytime, color, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
		}

	} else {
// jpw	
		// L0 - fixed coloring- switched from giant to big char - offset it down a little so name doesn't look nested with first line.
		CG_DrawBigString( 320 - 9 * 8, 24, "^?following^7", 1.0F );

		name = cgs.clientinfo[ cg.snap->ps.clientNum ].name;

		x = 0.5 * ( 640 - BIGCHAR_WIDTH * CG_DrawStrlen( name ) );

		CG_DrawStringExt( x, 60, name, color, qtrue, qtrue, BIGCHAR_WIDTH, BIGCHAR_HEIGHT, 0 );
	} // JPW NERVE
	return qtrue;
}



/*
=================
CG_DrawAmmoWarning
=================
*/
static void CG_DrawAmmoWarning( void ) {
	const char  *s;
	int w;

//----(SA)	forcing return for now
//			if we have messages to show here, comment back in
	return;


	if ( cg_drawAmmoWarning.integer == 0 ) {
		return;
	}

	if ( !cg.lowAmmoWarning ) {
		return;
	}

	if ( cg.lowAmmoWarning == 2 ) {
		s = "OUT OF AMMO";
	} else {
		s = "LOW AMMO WARNING";
	}
	w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
	CG_DrawBigString( 320 - w / 2, 64, s, 1.0F );
}

/*
=================
L0 - Countdown audio

Atm commented out as It doesn't work..
=================
*/
/*
static void countDownAnnouncer(int cnt) {

	if (!cg_announcer.integer) 
		return;

	// Argh...ugly!
	if (cnt == 5)
		trap_S_StartLocalSound( cgs.media.count5Sound , CHAN_ANNOUNCER ); // Count 5
	else if (cnt == 4)
		trap_S_StartLocalSound( cgs.media.count4Sound , CHAN_ANNOUNCER ); // Count 4
	else if (cnt == 3)
		trap_S_StartLocalSound( cgs.media.count3Sound , CHAN_ANNOUNCER ); // Count 3
	else if (cnt == 2)
		trap_S_StartLocalSound( cgs.media.count2Sound , CHAN_ANNOUNCER ); // Count 2
	else if (cnt == 1)
		trap_S_StartLocalSound( cgs.media.count5Sound , CHAN_ANNOUNCER ); // Count 1
return;
}
*/

/*
=================
CG_DrawWarmup
=================
*/
static void CG_DrawWarmup( void ) {
	int w;
	int sec;
	int cw;
	const char  *s;

	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		return;     // (SA) don't bother with this stuff in sp
	}

	sec = cg.warmup;
	if ( !sec ) {
		return;
	}

	if ( sec < 0 ) {
		if ( cgs.gametype == GT_COOP_BATTLE )
			s = "Waiting for 1 player";
        else
            return;		

        // s = "Waiting for players";
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		CG_DrawBigString( 320 - w / 2, 40, s, 1.0F );
#ifdef MONEY
                s = "buy items with /buy and /quickbuy";
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
                s = "buy items with ^2/buy^7 and ^2/quickbuy^7";
		CG_DrawBigString( 320 - w / 2, 80, s, 1.0F );
#endif
		cg.warmupCount = 0;
		return;
	}


	sec = ( sec - cg.time ) / 1000;

	if ( sec < 0 ) {
		sec = 0;
	}	
		
		// L0 - Don't show if announcer is enabled!
		if ( sec == 0 && !cg_announcer.integer) {
	        s = va( "^2FIGHT!");			
        } else {
	        s = va( "Game starts in: ^2%i", sec + 1 );	        
			// countDownAnnouncer(sec); // L0 - Needs fixing / FIXME
        }	 

        cw = 16;

	w = CG_DrawStrlen( s );
	CG_DrawStringExt( 320 - w * cw / 2, 120, s, colorWhite,
					  qfalse, qtrue, cw, (int)( cw * 1.5 ), 0 );
}

//==================================================================================

/*
=================
CG_DrawFlashFade
=================
*/
static void CG_DrawFlashFade( void ) {
	static int lastTime;
	int elapsed, time;
	vec4_t col;

	if ( cgs.scrFadeStartTime + cgs.scrFadeDuration < cg.time ) {
		cgs.scrFadeAlphaCurrent = cgs.scrFadeAlpha;
	} else if ( cgs.scrFadeAlphaCurrent != cgs.scrFadeAlpha ) {
		elapsed = ( time = trap_Milliseconds() ) - lastTime;  // we need to use trap_Milliseconds() here since the cg.time gets modified upon reloading
		lastTime = time;
		if ( elapsed < 500 && elapsed > 0 ) {
			if ( cgs.scrFadeAlphaCurrent > cgs.scrFadeAlpha ) {
				cgs.scrFadeAlphaCurrent -= ( (float)elapsed / (float)cgs.scrFadeDuration );
				if ( cgs.scrFadeAlphaCurrent < cgs.scrFadeAlpha ) {
					cgs.scrFadeAlphaCurrent = cgs.scrFadeAlpha;
				}
			} else {
				cgs.scrFadeAlphaCurrent += ( (float)elapsed / (float)cgs.scrFadeDuration );
				if ( cgs.scrFadeAlphaCurrent > cgs.scrFadeAlpha ) {
					cgs.scrFadeAlphaCurrent = cgs.scrFadeAlpha;
				}
			}
		}
	}
	// now draw the fade
	if ( cgs.scrFadeAlphaCurrent > 0.0 ) {
//		CG_Printf("fade: %f\n", cgs.scrFadeAlphaCurrent);
		VectorClear( col );
		col[3] = cgs.scrFadeAlphaCurrent;
//		CG_FillRect( -10, -10, 650, 490, col );
		CG_FillRect( 0, 0, 640, 480, col ); // why do a bunch of these extend outside 640x480?
	}
}



/*
==============
CG_DrawFlashZoomTransition
	hide the snap transition from regular view to/from zoomed

  FIXME: TODO: use cg_fade?
==============
*/
static void CG_DrawFlashZoomTransition( void ) {
	vec4_t color;
	float frac;
	int fadeTime;

	if ( !cg.snap ) {
		return;
	}

	if ( cg.snap->ps.eFlags & EF_MG42_ACTIVE ) {   // don't draw when on mg_42
		// keep the timer fresh so when you remove yourself from the mg42, it'll fade
		cg.zoomTime = cg.time;
		return;
	}

	if ( cgs.gametype <= GT_SINGLE_PLAYER ) { // JPW NERVE
		if ( cg.zoomedScope ) {
			fadeTime = cg.zoomedScope;  //----(SA)
		} else {
			fadeTime = 300;
		}
	}
	// jpw

	frac = cg.time - cg.zoomTime;

	if ( frac < fadeTime ) {
		frac = frac / (float)fadeTime;

		if ( cg.weaponSelect == WP_SNOOPERSCOPE ) {
//			Vector4Set( color, 0.7f, 0.3f, 0.7f, 1.0f - frac );
//			Vector4Set( color, 1, 0.5, 1, 1.0f - frac );
//			Vector4Set( color, 0.5f, 0.3f, 0.5f, 1.0f - frac );
			Vector4Set( color, 0.7f, 0.6f, 0.7f, 1.0f - frac );
		} else {
			Vector4Set( color, 0, 0, 0, 1.0f - frac );
		}

		CG_FillRect( -10, -10, 650, 490, color );
	}
}



/*
=================
CG_DrawFlashDamage
=================
*/
static void CG_DrawFlashDamage( void ) {
	vec4_t col;
	float redFlash;

	if ( !cg.snap ) {
		return;
	}

// L0 - Blood blending
	if (cg_bloodBlend.integer)
		return;
// end

	if ( cg.v_dmg_time > cg.time ) {
		redFlash = fabs( cg.v_dmg_pitch * ( ( cg.v_dmg_time - cg.time ) / DAMAGE_TIME ) );

		// blend the entire screen red
		if ( redFlash > 5 ) {
			redFlash = 5;
		}

		VectorSet( col, 0.2, 0, 0 );
		col[3] =  0.7 * ( redFlash / 5.0 );

		CG_FillRect( -10, -10, 650, 490, col );
	}
}


/*
=================
CG_DrawFlashFire
=================
*/
static void CG_DrawFlashFire( void ) {
	vec4_t col = {1,1,1,1};
	float alpha, max, f;

	if ( !cg.snap ) {
		return;
	}

	if ( cg_thirdPerson.integer ) {
		return;
	}

	if ( cg.cameraMode ) { // don't draw flames on camera screen.  will still do damage though, so not a potential cheat
		return;
	}

	if ( !cg.snap->ps.onFireStart ) {
		cg.v_noFireTime = cg.time;
		return;
	}

	alpha = (float)( ( FIRE_FLASH_TIME - 1000 ) - ( cg.time - cg.snap->ps.onFireStart ) ) / ( FIRE_FLASH_TIME - 1000 );
	if ( alpha > 0 ) {
		if ( alpha >= 1.0 ) {
			alpha = 1.0;
		}

		// fade in?
		f = (float)( cg.time - cg.v_noFireTime ) / FIRE_FLASH_FADEIN_TIME;
		if ( f >= 0.0 && f < 1.0 ) {
			alpha = f;
		}

		max = 0.5 + 0.5 * sin( (float)( ( cg.time / 10 ) % 1000 ) / 1000.0 );
		if ( alpha > max ) {
			alpha = max;
		}
		col[0] = alpha;
		col[1] = alpha;
		col[2] = alpha;
		col[3] = alpha;
		trap_R_SetColor( col );
		CG_DrawPic( -10, -10, 650, 490, cgs.media.viewFlashFire[( cg.time / 50 ) % 16] );
		trap_R_SetColor( NULL );

		trap_S_AddLoopingSound( cg.snap->ps.clientNum, cg.snap->ps.origin, vec3_origin, cgs.media.flameSound, (int)( 255.0 * alpha ) );
		trap_S_AddLoopingSound( cg.snap->ps.clientNum, cg.snap->ps.origin, vec3_origin, cgs.media.flameCrackSound, (int)( 255.0 * alpha ) );
	} else {
		cg.v_noFireTime = cg.time;
	}
}

/*
=================
CG_DrawFlashLightning
=================
*/
static void CG_DrawFlashLightning( void ) {
	//vec4_t		col={1,1,1,1}; // TTimo: unused
	float alpha;
	centity_t *cent;
	qhandle_t shader;

	if ( !cg.snap ) {
		return;
	}

	if ( cg_thirdPerson.integer ) {
		return;
	}

	cent = &cg_entities[cg.snap->ps.clientNum];

	if ( !cent->pe.teslaDamagedTime || ( cent->pe.teslaDamagedTime > cg.time ) ) {
		return;
	}

	alpha = 1.0 - (float)( cg.time - cent->pe.teslaDamagedTime ) / LIGHTNING_FLASH_TIME;
	if ( alpha > 0 ) {
		if ( alpha >= 1.0 ) {
			alpha = 1.0;
		}

		if ( ( cg.time / 50 ) % ( 2 + ( cg.time % 2 ) ) == 0 ) {
			shader = cgs.media.viewTeslaAltDamageEffectShader;
		} else {
			shader = cgs.media.viewTeslaDamageEffectShader;
		}

		CG_DrawPic( -10, -10, 650, 490, shader );
	}
}



/*
==============
CG_DrawFlashBlendBehindHUD
	screen flash stuff drawn first (on top of world, behind HUD)
==============
*/
static void CG_DrawFlashBlendBehindHUD( void ) {
	CG_DrawFlashZoomTransition();
}


/*
=================
CG_DrawFlashBlend
	screen flash stuff drawn last (on top of everything)
=================
*/
static void CG_DrawFlashBlend( void ) {
	CG_DrawFlashLightning();
	CG_DrawFlashFire();
	CG_DrawFlashDamage();
	CG_DrawFlashFade();
}

// NERVE - SMF
/*
=================
CG_DrawObjectiveInfo
=================
*/
#define OID_LEFT    10
#define OID_TOP     65

void CG_ObjectivePrint( const char *str, int charWidth, int team ) {
	char    *s;

	Q_strncpyz( cg.oidPrint, str, sizeof( cg.oidPrint ) );

	cg.oidPrintTime = cg.time;
	cg.oidPrintY = OID_TOP;
	cg.oidPrintCharWidth = charWidth;

	// count the number of lines for oiding
	cg.oidPrintLines = 1;
	s = cg.oidPrint;
	while ( *s ) {
		if ( *s == '\n' ) {
			cg.oidPrintLines++;
		}
		s++;
	}
}

static void CG_DrawObjectiveInfo( void ) {
	char    *start;
	int l;
	int x, y, w;
	int x1, y1, x2, y2;
	float   *color;
	vec4_t backColor = { 0.2f, 0.2f, 0.2f, 1.f };

	if ( !cg.oidPrintTime ) {
		return;
	}

	color = CG_FadeColor( cg.oidPrintTime, 1000 * 5 );
	if ( !color ) {
		return;
	}

	trap_R_SetColor( color );

	start = cg.oidPrint;

	y = cg.oidPrintY - cg.oidPrintLines * BIGCHAR_HEIGHT / 2;

	x1 = OID_LEFT - 2;
	y1 = y - 2;
	x2 = 0;

	// first just find the bounding rect
	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < 40; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.oidPrintCharWidth * CG_DrawStrlen( linebuffer );
		if ( x1 + w > x2 ) {
			x2 = x1 + w;
		}

		x = OID_LEFT;

		y += cg.oidPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	x2 = x2 + 4;
	y2 = y - cg.oidPrintCharWidth * 1.5 + 4;

	backColor[3] = color[3];
	CG_FillRect( x1, y1, x2 - x1, y2 - y1, backColor );

	VectorSet( backColor, 0, 0, 0 );
	CG_DrawRect( x1, y1, x2 - x1, y2 - y1, 1, backColor );

	// do the actual drawing
	start = cg.oidPrint;
	y = cg.oidPrintY - cg.oidPrintLines * BIGCHAR_HEIGHT / 2;

	while ( 1 ) {
		char linebuffer[1024];

		for ( l = 0; l < 40; l++ ) {
			if ( !start[l] || start[l] == '\n' ) {
				break;
			}
			linebuffer[l] = start[l];
		}
		linebuffer[l] = 0;

		w = cg.oidPrintCharWidth * CG_DrawStrlen( linebuffer );
		if ( x1 + w > x2 ) {
			x2 = x1 + w;
		}

		x = OID_LEFT;

		CG_DrawStringExt( x, y, linebuffer, color, qfalse, qtrue,
						  cg.oidPrintCharWidth, (int)( cg.oidPrintCharWidth * 1.5 ), 0 );

		y += cg.oidPrintCharWidth * 1.5;

		while ( *start && ( *start != '\n' ) ) {
			start++;
		}
		if ( !*start ) {
			break;
		}
		start++;
	}

	trap_R_SetColor( NULL );
}
// -NERVE - SMF

//==================================================================================


void CG_DrawTimedMenus() {
	if ( cg.voiceTime ) {
		int t = cg.time - cg.voiceTime;
		if ( t > 2500 ) {
			Menus_CloseByName( "voiceMenu" );
			trap_Cvar_Set( "cl_conXOffset", "0" );
			cg.voiceTime = 0;
		}
	}
}


/*
=================
CG_Fade
=================
*/
void CG_Fade( int r, int g, int b, int a, int time, int duration ) {

	// incorporate this into the current fade scheme

	cgs.scrFadeAlpha = (float)a / 255.0f;
	cgs.scrFadeStartTime = time;
	cgs.scrFadeDuration = duration;

	if ( cgs.scrFadeStartTime + cgs.scrFadeDuration <= cg.time ) {
		cgs.scrFadeAlphaCurrent = cgs.scrFadeAlpha;
	}


	return;


	if ( time <= 0 ) {  // do instantly
		cg.fadeRate = 1;
		cg.fadeTime = cg.time - 1;  // set cg.fadeTime behind cg.time so it will start out 'done'
	} else {
		cg.fadeRate = 1.0f / time;
		cg.fadeTime = cg.time + time;
	}

	cg.fadeColor2[ 0 ] = ( float )r / 255.0f;
	cg.fadeColor2[ 1 ] = ( float )g / 255.0f;
	cg.fadeColor2[ 2 ] = ( float )b / 255.0f;
	cg.fadeColor2[ 3 ] = ( float )a / 255.0f;
}


/*
===============
CG_DrawGameScreenFade
===============
*/
static void CG_DrawGameScreenFade( void ) {
	vec4_t col;

	if ( cg.viewFade <= 0.0 ) {
		return;
	}

	if ( !cg.snap ) {
		return;
	}

	VectorClear( col );
	col[3] = cg.viewFade;
	CG_FillRect( 0, 0, 640, 480, col );
}

/*
=================
CG_ScreenFade
=================
*/
static void CG_ScreenFade( void ) {
	int msec;
	int i;
	float t, invt;
	vec4_t color;

	// Ridah, fade the screen (in-game)
	CG_DrawGameScreenFade();

	if ( !cg.fadeRate ) {
		return;
	}

	msec = cg.fadeTime - cg.time;
	if ( msec <= 0 ) {
		cg.fadeColor1[ 0 ] = cg.fadeColor2[ 0 ];
		cg.fadeColor1[ 1 ] = cg.fadeColor2[ 1 ];
		cg.fadeColor1[ 2 ] = cg.fadeColor2[ 2 ];
		cg.fadeColor1[ 3 ] = cg.fadeColor2[ 3 ];

		if ( !cg.fadeColor1[ 3 ] ) {
			cg.fadeRate = 0;
			return;
		}

		CG_FillRect( 0, 0, 640, 480, cg.fadeColor1 );

	} else {
		t = ( float )msec * cg.fadeRate;
		invt = 1.0f - t;

		for ( i = 0; i < 4; i++ ) {
			color[ i ] = cg.fadeColor1[ i ] * t + cg.fadeColor2[ i ] * invt;
		}

		if ( color[ 3 ] ) {
			CG_FillRect( 0, 0, 640, 480, color );
		}
	}
}



/*
=================
CG_Draw2D
=================
*/
static void CG_Draw2D( void ) {

	// if we are taking a levelshot for the menu, don't draw anything
	if ( cg.levelShot ) {
		return;
	}

	if ( cg.cameraMode ) { //----(SA)	no 2d when in camera view
		CG_DrawFlashBlend();    // (for fades)
		return;
	}

	if ( cg_draw2D.integer == 0 ) {
		return;
	}

	CG_ScreenFade();


	if ( cg.snap->ps.pm_type == PM_INTERMISSION ) {
		CG_DrawIntermission();
		return;
	}

	CG_DrawFlashBlendBehindHUD();

	if ( cgs.localServer ) {
	    CG_DrawOnScreenText();
	}

	if ( cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR ) {
		CG_DrawSpectator();
		CG_DrawCrosshair();
		CG_DrawCrosshairNames();
                if ( cgs.gametype != GT_SINGLE_PLAYER )
                        CG_DrawTeamInfo();
	} else {
		// don't draw any status if dead
		if ( cg.snap->ps.stats[STAT_HEALTH] > 0 ) {

			CG_DrawCrosshair();

			if ( cg_drawStatus.integer ) {
				Menu_PaintAll();
				CG_DrawTimedMenus();
			}

                        if ( cgs.gametype != GT_SINGLE_PLAYER )
                                CG_DrawTeamInfo();

//			CG_DrawStatusBar();
			CG_DrawAmmoWarning();
			CG_DrawDynamiteStatus();
			CG_DrawCoopCrosshairNames();
			CG_DrawWeaponSelect();
			CG_DrawHoldableSelect();
			CG_DrawPickupItem();
			CG_DrawReward();
		}
	}

	CG_DrawVote();

	CG_DrawLagometer();

	if ( !cg_paused.integer ) {
		CG_DrawUpperRight();
	}

        CG_DrawTimeLeft();

//	CG_DrawLowerRight();
	if ( !CG_DrawFollow() ) {
		CG_DrawWarmup();
	}

	// don't draw center string if scoreboard is up
	if ( !CG_DrawScoreboard() ) {
                CG_DrawNotify();

		CG_DrawCenterString();

		CG_DrawObjectiveInfo();     // NERVE - SMF
	}

	// L0 - announcer
	CG_DrawAnnouncer();

	// Ridah, draw flash blends now
	CG_DrawFlashBlend();

    // fretn
    CG_DrawFreeze();
}

/*
====================
CG_StartShakeCamera
====================
*/
void CG_StartShakeCamera( float p, int duration, vec3_t src, float radius ) {
	int i;

	// find a free shake slot
	for ( i = 0; i < MAX_CAMERA_SHAKE; i++ ) {
		if ( cg.cameraShake[i].time > cg.time || cg.cameraShake[i].time + cg.cameraShake[i].length <= cg.time ) {
			break;
		}
	}

	if ( i == MAX_CAMERA_SHAKE ) {
		return; // no free slots

	}
	cg.cameraShake[i].scale = p;

	cg.cameraShake[i].length = duration;
	cg.cameraShake[i].time = cg.time;
	VectorCopy( src, cg.cameraShake[i].src );
	cg.cameraShake[i].radius = radius;
}

/*
====================
CG_CalcShakeCamera
====================
*/
void CG_CalcShakeCamera() {
	float val, scale, dist, x, sx;
	float bx = 0.0f; // TTimo: init
	int i;

	// build the scale
	scale = 0.0f;
	sx = (float)cg.time / 600.0; // x * (float)(cg.cameraShake[i].length) / 600.0;
	for ( i = 0; i < MAX_CAMERA_SHAKE; i++ ) {
		if ( cg.cameraShake[i].time <= cg.time && cg.cameraShake[i].time + cg.cameraShake[i].length > cg.time ) {
			dist = Distance( cg.cameraShake[i].src, cg.refdef.vieworg );
			// fade with distance
			val = cg.cameraShake[i].scale * ( 1.0f - ( dist / cg.cameraShake[i].radius ) );
			// fade with time
			x = 1.0f - ( ( cg.time - cg.cameraShake[i].time ) / cg.cameraShake[i].length );
			val *= x;
			// overwrite global scale if larger
			if ( val > scale ) {
				scale = val;
				bx = x;
			}
		}
	}

	// check the current rumble status
	if ( cg.rumbleScale > scale ) {
		scale = cg.rumbleScale;
		bx = cg.rumbleScale;
	}

	if ( scale <= 0.0f ) {
		cg.cameraShakePhase = crandom() * M_PI; // randomize the phase
		return;
	}

	if ( scale > 1.0f ) {
		scale = 1.0f;
	}

	// up/down
	val = sin( M_PI * 8 * sx + cg.cameraShakePhase ) * bx * 18.0f * scale;
	cg.cameraShakeAngles[0] = val;
	//cg.refdefViewAngles[0] += val;

	// left/right
	val = sin( M_PI * 15 * sx + cg.cameraShakePhase ) * bx * 16.0f * scale;
	cg.cameraShakeAngles[1] = val;
	//cg.refdefViewAngles[1] += val;

	// roll
	val = sin( M_PI * 12 * sx + cg.cameraShakePhase ) * bx * 10.0f * scale;
	cg.cameraShakeAngles[2] = val;
	//cg.refdefViewAngles[2] += val;
}

/*
====================
CG_ApplyShakeCamera
====================
*/
void CG_ApplyShakeCamera() {
	VectorAdd( cg.refdefViewAngles, cg.cameraShakeAngles, cg.refdefViewAngles );
	AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
}

// NERVE - SMF
void CG_CoopStartShakeCamera( float p ) {
        cg.cameraShakeScale = p; 

        cg.cameraShakeLength = 1000 * ( p * p ); 
        cg.cameraShakeTime = cg.time + cg.cameraShakeLength;
        cg.cameraShakePhase = crandom() * M_PI; // start chain in random dir
}

void CG_ShakeCamera() {
        float x, val; 

        if ( cg.time > cg.cameraShakeTime ) {
                cg.cameraShakeScale = 0; // JPW NERVE all pending explosions resolved, so reset shakescale
                return;
        }    

        // JPW NERVE starts at 1, approaches 0 over time
        x = ( cg.cameraShakeTime - cg.time ) / cg.cameraShakeLength;

        // up/down
        val = sin( M_PI * 8 * x + cg.cameraShakePhase ) * x * 18.0f * cg.cameraShakeScale;
        cg.refdefViewAngles[0] += val;

        // left/right
        val = sin( M_PI * 15 * x + cg.cameraShakePhase ) * x * 16.0f * cg.cameraShakeScale;
        cg.refdefViewAngles[1] += val;

        AnglesToAxis( cg.refdefViewAngles, cg.refdef.viewaxis );
}
// -NERVE - SMF



/*
=====================
CG_DrawActive

Perform all drawing needed to completely fill the screen
=====================
*/
void CG_DrawActive( stereoFrame_t stereoView ) {
	float separation;
	vec3_t baseOrg;

	// optionally draw the info screen instead
	if ( !cg.snap ) {
		CG_DrawInformation();
		return;
	}

	// if they are waiting at the mission stats screen, show the stats
	if ( cg_gameType.integer <= GT_SINGLE_PLAYER ) {
		if ( strlen( cg_missionStats.string ) > 1 ) {
			trap_Cvar_Set( "com_expectedhunkusage", "-2" );
			CG_DrawInformation();
			return;
		}
	}

	switch ( stereoView ) {
	case STEREO_CENTER:
		separation = 0;
		break;
	case STEREO_LEFT:
		separation = -cg_stereoSeparation.value / 2;
		break;
	case STEREO_RIGHT:
		separation = cg_stereoSeparation.value / 2;
		break;
	default:
		separation = 0;
		CG_Error( "CG_DrawActive: Undefined stereoView" );
	}


	// clear around the rendered view if sized down
//	CG_TileClear();	// (SA) moved down

	// offset vieworg appropriately if we're doing stereo separation
	VectorCopy( cg.refdef.vieworg, baseOrg );
	if ( separation != 0 ) {
		VectorMA( cg.refdef.vieworg, -separation, cg.refdef.viewaxis[1], cg.refdef.vieworg );
	}

	cg.refdef.glfog.registered = 0; // make sure it doesn't use fog from another scene
/*
	// NERVE - SMF - activate limbo menu and draw small 3d window
	CG_ActivateLimboMenu();

	if ( cg.limboMenu ) {
		float x, y, w, h;
		x = LIMBO_3D_X;
		y = LIMBO_3D_Y;
		w = LIMBO_3D_W;
		h = LIMBO_3D_H;

		cg.refdef.width = 0;
		CG_AdjustFrom640( &x, &y, &w, &h );

		cg.refdef.x = x;
		cg.refdef.y = y;
		cg.refdef.width = w;
		cg.refdef.height = h;
	}
	// -NERVE - SMF
*/
	cg.refdef.rdflags |= RDF_DRAWSKYBOX;
	if ( !cg_skybox.integer ) {
		cg.refdef.rdflags &= ~RDF_DRAWSKYBOX;
	}

	trap_R_RenderScene( &cg.refdef );

        if ( cg.snap->ps.pm_flags & PMF_LIMBO || cg.snap->ps.stats[STAT_HEALTH] <= 0 || cg.snap->ps.eFlags & EF_FROZEN) {
                static int texid = 0; 
                
                if ( !texid ) {
                        texid = trap_R_GetTextureId( "textures/rtt.tga" );
                }    

                trap_R_RenderToTexture( texid, 0, 0, cg.refdef.width, cg.refdef.height);
                trap_R_ClearScene();
                trap_R_DrawStretchPic( 0, 0, cg.refdef.width, cg.refdef.height, 0, 1, 1, 0, cgs.media.rtt);
        }

	// restore original viewpoint if running stereo
	if ( separation != 0 ) {
		VectorCopy( baseOrg, cg.refdef.vieworg );
	}

	// clear around the rendered view if sized down
	CG_TileClear();     //----(SA)	moved to 2d section to avoid 2d/3d fog-state problems

	// draw status bar and other floating elements
	CG_Draw2D();
}


// cs: omnibot 3d text
#define COLOR_NULL      '*' // probably should move to q_shared.h

int CG_Text_Width_Ext( const char *text, float scale, int limit, fontInfo_t* font ) {
	glyphInfo_t *glyph;
	const char *s = text;
	float out, useScale = scale * font->glyphScale;
	
	out = 0;
	if( text ) {
		int len = strlen( text );
		int count = 0;

		if (limit > 0 && len > limit) {
			len = limit;
		}

		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}

	return out * useScale;
}

int CG_Text_Height_Ext( const char *text, float scale, int limit, fontInfo_t* font ) {
	float max;
	glyphInfo_t *glyph;
	float useScale;
	const char *s = text;

	useScale = scale * font->glyphScale;
	max = 0;
	if (text) {
		int len = strlen( text );
		int count = 0;

		if (limit > 0 && len > limit) {
			len = limit;
		}

		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];

				if (max < glyph->height) {
					max = glyph->height;
				}

				s++;
				count++;
			}
		}
	}
	return max * useScale;
}

void CG_Text_PaintChar_Ext(float x, float y, float w, float h, float scalex, float scaley, float s, float t, float s2, float t2, qhandle_t hShader) {
	w *= scalex;
	h *= scaley;
	CG_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s, t, s2, t2, hShader );
}

void CG_Text_Paint_Ext( float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font ) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;

	scalex *= font->glyphScale;
	scaley *= font->glyphScale;

	if (text) {
		const char *s = text;
		trap_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[(unsigned char)*s];
			if ( Q_IsColorString( s ) ) {
				if( *(s+1) == COLOR_NULL ) {
					memcpy( newColor, color, sizeof(newColor) );
				} else {
					memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
					newColor[3] = color[3];
				}
				trap_R_SetColor( newColor );
				s += 2;
				continue;
			} else {
				float yadj = scaley * glyph->top;
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					trap_R_SetColor( colorBlack );
					CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex) + ofs, y - yadj + ofs, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
					colorBlack[3] = 1.0;
					trap_R_SetColor( newColor );
				}
				CG_Text_PaintChar_Ext(x + (/*glyph->pitch **/ scalex), y - yadj, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
				x += (glyph->xSkip * scalex) + adjust;
				s++;
				count++;
			}
		}
		trap_R_SetColor( NULL );
	}
}

#define MAX_WORLDTEXT 128
#define MAX_TEXTLENGTH 512 // fix for 3d waypoint text
#define MAX_RENDERDIST 2500

typedef struct onsText_s 
{
	struct onsText_s *next;
	int			endtime;
	vec4_t			color;
	char		text[MAX_TEXTLENGTH];
	vec3_t		origin;
} onsText_t;

static onsText_t WorldText[MAX_WORLDTEXT];
static onsText_t * freeworldtext;			// List of world text
static onsText_t * activeworldtext;			// List of world text

void CG_InitWorldText( void ) {
	int i;

	memset( &WorldText, 0, sizeof(WorldText) );
	for( i = 0; i < MAX_WORLDTEXT - 1; i++ ) {
		WorldText[i].next = &WorldText[i+1];
	}

	freeworldtext = &WorldText[0];
	activeworldtext = NULL;
}

void CG_ClearWorldText( void ) {
    int i;

    for( i = 0; i < MAX_WORLDTEXT - 1; i++ ) {
	WorldText[i].endtime = cg.time;
    }

    activeworldtext = NULL;
}

/*
================
CG_WorldToScreen
================
*/
qboolean CG_WorldToScreen(vec3_t point, float *x, float *y)
{
	vec3_t          trans;
	float           xc, yc;
	float           px, py;
	float           z;

	px = tan(cg.refdef.fov_x * M_PI / 360.0);
	py = tan(cg.refdef.fov_y * M_PI / 360.0);

	VectorSubtract(point, cg.refdef.vieworg, trans);

	xc = 640.0f / 2.0f;
	yc = 480.0f / 2.0f;

	z = DotProduct(trans, cg.refdef.viewaxis[0]);
	if(z <= 0.001f)
		return qfalse;

	if(x)
		*x = xc - DotProduct(trans, cg.refdef.viewaxis[1]) * xc / (z * px);

	if(y)
		*y = yc - DotProduct(trans, cg.refdef.viewaxis[2]) * yc / (z * py);

	return qtrue;
}

qboolean CG_AddOnScreenText( const char *text, vec3_t origin, vec4_t color, float duration )
{
	onsText_t *worldtext = freeworldtext;
	if (!worldtext) return qfalse;

	freeworldtext = worldtext->next;
	worldtext->next=activeworldtext;
	activeworldtext=worldtext;

	VectorCopy(origin, worldtext->origin);
	worldtext->endtime = cg.time + (int)((float)duration * 1000.f);
	VectorCopy4(color, worldtext->color);
	Q_strncpyz(worldtext->text,text,MAX_TEXTLENGTH);
	return qtrue;
}

void CG_DrawOnScreenText(void) {
	onsText_t *worldtext;
	onsText_t * * whereworldtext;
	const float fTxtScale = 0.25f;
	float x,y;

	/* Render/Move the world text */
	worldtext = activeworldtext;
	whereworldtext=&activeworldtext;

	while( worldtext ) 
	{
		/* Check for expiration */
		if(worldtext->endtime < cg.time) 
		{
			/* Clear up this world text */
			*whereworldtext=worldtext->next;
			worldtext->next=freeworldtext;
			freeworldtext=worldtext;
			worldtext=*whereworldtext;
			continue;
		}
		
		if( CG_WorldToScreen(worldtext->origin, &x, &y) && PointVisible(worldtext->origin) && DistanceSquared(cg.refdef.vieworg, worldtext->origin) < MAX_RENDERDIST * MAX_RENDERDIST )
		{
			const char *tokens = "\n";
			const char *tok = 0;
			char temp[1024];
			int heightOffset = 0;
			fontInfo_t *font = &cgDC.Assets.bigFont;			
				
			Q_strncpyz(temp,worldtext->text,1024);
			tok = strtok(temp,tokens);
			while(tok)
			{
				const int width = CG_Text_Width_Ext(tok,fTxtScale,0, font);
				const int height = CG_Text_Height_Ext(tok,fTxtScale,0, font);

				CG_Text_Paint_Ext(
					x - (width * 0.5), 
					y + heightOffset,
					fTxtScale,
					fTxtScale,
					worldtext->color, 
					tok, 
					0, 0, 
					ITEM_TEXTSTYLE_NORMAL,
					font);

				heightOffset += height*1.5;
				tok = strtok(NULL,tokens);
			}
		}		

		trap_R_SetColor(NULL);

		whereworldtext=&worldtext->next;
		worldtext = worldtext->next;
	}
}

// L0 - Ported from ET (NQ i think)
void CG_DrawAnnouncer( void )
{
	int		x, y, w, h;
	float	scale, fade;
	vec4_t	color;


	if(  cg.centerPrintAnnouncerTime <= cg.time )
		return;

	fade = (float)(cg.centerPrintAnnouncerTime - cg.time)/cg.centerPrintAnnouncerDuration;

	color[0] = cg.centerPrintAnnouncerColor[0];
	color[1] = cg.centerPrintAnnouncerColor[1];
	color[2] = cg.centerPrintAnnouncerColor[2];

	switch( cg.centerPrintAnnouncerMode ){
		default:
		case ANNOUNCER_NORMAL:
			color[3] = fade;
			break;
		case ANNOUNCER_SINE:
			color[3] = sin( M_PI * fade );
			break;
		case ANNOUNCER_INVERSE_SINE:
			color[3] = 1-sin( M_PI * fade );
			break;
		case ANNOUNCER_TAN:
			color[3] = tan( M_PI * fade );
	}

	scale = (1.1f - color[3]) * cg.centerPrintAnnouncerScale;

	h = CG_Text_Height_Ext( cg.centerPrintAnnouncer, scale, 0, &cgDC.Assets.textFont );

	y = (SCREEN_HEIGHT - h) / 2;

	w = CG_Text_Width_Ext(cg.centerPrintAnnouncer, scale, 0, &cgDC.Assets.textFont );

	x = ( SCREEN_WIDTH - w ) / 2;

	CG_Text_Paint_Ext( x, y, scale, scale , color, cg.centerPrintAnnouncer, 0, 0, 0, &cgDC.Assets.textFont );
}

void CG_AddAnnouncer(char *text, sfxHandle_t sound, float scale, int duration, float r, float g, float b, int mode)
{

//	if ( !cg_announcer.integer )
//		return;

	if ( sound )
		trap_S_StartLocalSound( sound, CHAN_ANNOUNCER );

	if ( text ){
		cg.centerPrintAnnouncer = text;
		cg.centerPrintAnnouncerTime = cg.time + duration;
		cg.centerPrintAnnouncerScale = scale;
		cg.centerPrintAnnouncerDuration = duration;
		cg.centerPrintAnnouncerColor[0] = r;
		cg.centerPrintAnnouncerColor[1] = g;
		cg.centerPrintAnnouncerColor[2] = b;
		cg.centerPrintAnnouncerMode = mode;
	}


}

