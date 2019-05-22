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

// cg_info.c -- display information while data is being loading

#include "cg_local.h"
#include "../ui/ui_shared.h"


/*
======================
CG_LoadingString

======================
*/
void CG_LoadingString( const char *s ) {
	Q_strncpyz( cg.infoScreenText, s, sizeof( cg.infoScreenText ) );

	if ( s && s[0] != 0 ) {
		CG_Printf( "LOADING... %s\n",s );   //----(SA)	added so you can see from the console what's going on

	}
}

typedef struct {
	char    *label;
	int YOfs;
	int labelX;
	int labelFlags;
	vec4_t labelColor;

	char    *format;
	int formatX;
	int formatFlags;
	vec4_t formatColor;

	int numVars;
} statsItem_t;

// this defines the layout of the mission stats
// NOTE: these must match the stats sent in AICast_ScriptAction_ChangeLevel()
static statsItem_t statsItems[] = {
#ifdef LOCALISATION
	{ "Time",        134,    214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%02i:%02i:%02i",    348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  3 },
	{ "Objectives",  28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%i/%i",         348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  2 },
	{ "Secret Areas", 28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%i/%i",         348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  2 },
	{ "Treasures",    28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {0.62f, 0.56f,  0.0f,   1.0f},  "%i/%i",         348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  2 },
	{ "Attempts",    28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%i",                348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  1 },
#else
	{ "end_time",        134,    214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%02i:%02i:%02i",    348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  3 },
	{ "end_objectives",  28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%i/%i",         348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  2 },
	{ "end_secrets", 28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%i/%i",         348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  2 },
	{ "end_treasure",    28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {0.62f, 0.56f,  0.0f,   1.0f},  "%i/%i",         348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  2 },
	{ "end_attempts",    28,     214,        ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  "%i",                348,    ITEM_TEXTSTYLE_SHADOWEDMORE,    {1.0f,  1.0f,   1.0f,   1.0f},  1 },
#endif

	{ NULL }
};


/*
==============
CG_DrawExitStats
	pretty much what the game should draw when you're at the exit
	This is not the final deal, but represents the kind of thing
	that will be there
==============
*/

void CG_DrawExitStats( void ) {
	int i, y, v, j;
	float *color;   // faded color based on cursor hint drawing
	float color2[4] = {0, 0, 0, 1};
	const char *str;
	char *mstats, *token;
	clientInfo_t *ci;

	#define MAX_STATS_VARS  64
	int vars[MAX_STATS_VARS];
	char *formatStr = NULL; // TTimo: init
	int varIndex = 0;     // TTimo: init

	if ( cg_paused.integer ) {
		// no draw if any menu's are up	 (or otherwise paused)
		return;
	}

	if ( cg.showScores ) {
		return;
	}

	color = CG_FadeColor( cg.cursorHintTime, cg.cursorHintFade );

	if ( !color ) { // currently faded out, don't draw
		return;
	}

	// check for fade up
	if ( cg.time < ( cg.exitStatsTime + cg.exitStatsFade ) ) {
		color[3] = (float)( cg.time - cg.exitStatsTime ) / (float)cg.exitStatsFade;
	}

	color2[3] = color[3];

	// parse it
	str = CG_ConfigString( CS_MISSIONSTATS );

	if ( !str || !str[0] ) {
		return;
	}

	// background
	color2[3] *= 0.6f;
	CG_FilledBar( 150, 84, 340, 270, color2, NULL, NULL, 1.0f, 0 );

	color2[0] = color2[1] = color2[2] = 0.3f;
	color2[3] *= 0.6f;

	// border
	CG_FilledBar( 148, 84, 2, 270, color2, NULL, NULL, 1.0f, 0 );    // left
	CG_FilledBar( 490, 84, 2, 270, color2, NULL, NULL, 1.0f, 0 );    // right
	CG_FilledBar( 148, 82, 344, 2, color2, NULL, NULL, 1.0f, 0 );    // top
	CG_FilledBar( 148, 354, 344, 2, color2, NULL, NULL, 1.0f, 0 );    // bot


	// text boxes
	color2[0] = color2[1] = color2[2] = 0.4f;
	for ( i = 0; i < 8; i++ ) {
		CG_FilledBar( 170, 119 + ( 28 * i ), 300, 20, color2, NULL, NULL, 1.0f, 0 );
	}


	// green title
	color2[0] = color2[2] = 0;
	color2[1] = 0.3f;
	CG_FilledBar( 150, 84, 340, 20, color2, NULL, NULL, 1.0f, 0 );

	color2[0] = color2[1] = color2[2] = 0.2f;

	// title
	color2[0] = color2[1] = color2[2] = 1;
	color2[3] = color[3];
	//----(SA)	scale change per MK

#ifdef LOCALISATION
	CG_Text_Paint( 270, 100, 2, 0.313f, color2, va( "%s", CG_translateString( "Mission Stats" ) ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
#else
	CG_Text_Paint( 270, 100, 2, 0.313f, color2, va( "%s", CG_translateString( "end_title" ) ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
#endif
	color2[0] = color2[1] = color2[2] = 1;
	if ( cg.cursorHintIcon == HINT_NOEXIT ) {
#ifdef LOCALISATION
		CG_Text_Paint( 260, 65, 2, 0.225f, color2, va( "%s", CG_translateString( "Exit not yet available" ) ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
#else
		CG_Text_Paint( 260, 65, 2, 0.225f, color2, va( "%s", CG_translateString( "end_noexit" ) ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
#endif
	} else {
#ifdef LOCALISATION
		CG_Text_Paint( 250, 65, 2, 0.225f, color2, va( "%s", CG_translateString( "Proceed forward to exit..." ) ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
#else
		CG_Text_Paint( 250, 65, 2, 0.225f, color2, va( "%s", CG_translateString( "end_exit" ) ), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
#endif
	}

	mstats = (char*)str + 2;    // add offset for 's='
	for ( i = 0; mstats[i]; i++ ) {
		if ( mstats[i] == ',' ) {
			mstats[i] = ' ';
		}
	}
	for ( i = 0, y = 0, v = 0; statsItems[i].label; i++ ) {
		y += statsItems[i].YOfs;

		VectorCopy4( statsItems[i].labelColor, color2 );
		color2[3] = statsItems[i].formatColor[3] = color[3];    // set proper alpha


		if ( statsItems[i].numVars ) {
			varIndex = v;
			for ( j = 0; j < statsItems[i].numVars; j++ ) {
				token = COM_Parse( &mstats );
				if ( !token[0] ) {
					CG_Error( "error parsing mission stats\n" );
					return;
				}

				vars[v++] = atoi( token );
			}

			// build the formatStr
			switch ( statsItems[i].numVars ) {
			case 1:
				formatStr = va( statsItems[i].format, vars[varIndex] );
				break;
			case 2:
				formatStr = va( statsItems[i].format, vars[varIndex], vars[varIndex + 1] );
				break;
			case 3:
				formatStr = va( statsItems[i].format, vars[varIndex], vars[varIndex + 1], vars[varIndex + 2] );
				break;
			case 4:
				formatStr = va( statsItems[i].format, vars[varIndex], vars[varIndex + 1], vars[varIndex + 2], vars[varIndex + 3] );
				break;
			}

			CG_Text_Paint( statsItems[i].formatX, y, 2, 0.3, statsItems[i].formatColor, formatStr, 0, 0, statsItems[i].formatFlags );
		}

		if ( i == 1 ) {  // 'objectives'
			if ( vars[varIndex] < vars[varIndex + 1] ) { // missing objectives, draw in red
				color2[0] = 1;
				color2[1] = color2[2] = 0;
			}
		}

		if ( i == 3 ) {  // 'treasure'
			if ( vars[varIndex] < vars[varIndex + 1] || !vars[varIndex + 1] ) {    // missing treasure, only draw in white (gold when you got em all)  (unless there's no gold available, then 0/0 shows white)
				color2[0] = color2[1] = color2[2] = 1;  // white
			}
		}
#ifdef LOCALISATION
		CG_Text_Paint( statsItems[i].labelX, y, 2, 0.3, color2, va( "%s:", CG_TranslateString( statsItems[i].label ) ), 0, 0, statsItems[i].labelFlags );
#else
		CG_Text_Paint( statsItems[i].labelX, y, 2, 0.3, color2, va( "%s:", CG_translateString( statsItems[i].label ) ), 0, 0, statsItems[i].labelFlags );
#endif
	}
	COM_Parse( &mstats );

	ci = &cgs.clientinfo[cg.scores[cg.destroyer].client];
	CG_Text_Paint( 214, y + 28, 2, 0.3, statsItems[0].formatColor, "Destructor", 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
	CG_Text_Paint( 348, y + 28, 2, 0.3, statsItems[0].formatColor, va( "%s", ci->name ), 0, 14, ITEM_TEXTSTYLE_SHADOWEDMORE );

	ci = &cgs.clientinfo[cg.scores[cg.airbourne].client];
	CG_Text_Paint( 214, y + ( 2 * 28 ), 2, 0.3, statsItems[1].formatColor, "Airbourne ranger:", 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
	CG_Text_Paint( 348, y + ( 2 * 28 ), 2, 0.3, statsItems[1].formatColor, va( "%s", ci->name ), 0, 14, ITEM_TEXTSTYLE_SHADOWEDMORE );

	ci = &cgs.clientinfo[cg.scores[cg.pickupmaster].client];
	CG_Text_Paint( 214, y + ( 3 * 28 ), 2, 0.3, statsItems[2].formatColor, "Pickup artist:", 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );
	CG_Text_Paint( 348, y + ( 3 * 28 ), 2, 0.3, statsItems[2].formatColor, va( "%s", ci->name ), 0, 14, ITEM_TEXTSTYLE_SHADOWEDMORE );
}

/*
====================
CG_DrawInformation

Draw all the status / pacifier stuff during level loading
====================
*/
void CG_DrawInformation( void ) {
	const char  *s;
	const char  *info;
	qhandle_t levelshot = 0; // TTimo: init
	static int callCount = 0;
	float percentDone;

	int expectedHunk;
	char hunkBuf[MAX_QPATH];

	vec2_t xy = { 200, 448 };
	vec2_t wh = { 240, 10 };

	if ( cg.snap ) {
		return;     // we are in the world, no need to draw information
	}

	if ( callCount ) {    // reject recursive calls
		return;
	}

	callCount++;

	info = CG_ConfigString( CS_SERVERINFO );

	trap_Cvar_VariableStringBuffer( "com_expectedhunkusage", hunkBuf, MAX_QPATH );
	expectedHunk = atoi( hunkBuf );

	s = Info_ValueForKey( info, "mapname" );

	if ( s && s[0] != 0 ) {  // there is often no 's'
		levelshot = trap_R_RegisterShaderNoMip( va( "levelshots/%s.tga", s ) );
	}

	if ( !levelshot ) {
		levelshot = trap_R_RegisterShaderNoMip( "levelshots/unknownmap.jpg" );
	}
	trap_R_SetColor( NULL );

	// Pillarboxes
	if ( cg_fixedAspect.integer ) {
		if ( cgs.glconfig.vidWidth * 480.0 > cgs.glconfig.vidHeight * 640.0 ) {
			vec4_t col = { 0, 0, 0, 1 };
			float pillar = 0.5 * ( ( cgs.glconfig.vidWidth - ( cgs.screenXScale * 640.0 ) ) / cgs.screenXScale );

			CG_SetScreenPlacement( PLACE_LEFT, PLACE_CENTER );
			CG_FillRect( 0, 0, pillar + 1, 480, col );
			CG_SetScreenPlacement( PLACE_RIGHT, PLACE_CENTER );
			CG_FillRect( 640 - pillar, 0, pillar + 1, 480, col );
			CG_SetScreenPlacement( PLACE_CENTER, PLACE_CENTER );
		}
	}

	CG_DrawPic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, levelshot );

	// Ridah, in single player, cheats disabled, don't show unnecessary information
	// fretn - only in single player
	if ( cgs.gametype == GT_SINGLE_PLAYER ) {
		trap_UI_Popup( "briefing" );

		//trap_UpdateScreen();
		callCount--;
		return;
	}
	
	// show the server motd
	CG_DrawMotd();

	// show the percent complete bar
	if ( expectedHunk >= 0 ) {
		percentDone = (float)( cg_hunkUsed.integer + cg_soundAdjust.integer ) / (float)( expectedHunk );
		if ( percentDone > 0.97 ) { // never actually show 100%, since we are not in the game yet
			percentDone = 0.97;
		}
		CG_HorizontalPercentBar( xy[0], xy[1], wh[0], wh[1], percentDone );

	}

	callCount--;
}

