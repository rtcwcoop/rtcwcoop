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

// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"
#include "../ui/ui_shared.h"

#define SCOREBOARD_WIDTH    ( 31 * BIGCHAR_WIDTH )

// fretn
void CG_DrawCoopScoreboard( void )
{
        int i;
        float *color;   // faded color based on cursor hint drawing
        float color2[4] = {0, 0, 0, 1}; 

        #define MAX_STATS_VARS  64

        if ( cg_paused.integer ) { 
                // no draw if any menu's are up  (or otherwise paused)
                return;
        }   

        //color = CG_FadeColor( cg.cursorHintTime, cg.cursorHintFade );
        color = colorWhite;
	/*if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
		 cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );

		if ( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return qfalse;
		}
		fade = *fadeColor;
	}
*/

        if ( !color ) { // currently faded out, don't draw
                return;
        }   

        // check for fade up
        //if ( cg.time < ( cg.exitStatsTime + cg.exitStatsFade ) ) { 
        //        color[3] = (float)( cg.time - cg.exitStatsTime ) / (float)cg.exitStatsFade;
        //}   

        color2[3] = color[3];


        // background
        color2[3] *= 0.6f;
        CG_FilledBar( 150, 104, 340, 230, color2, NULL, NULL, 1.0f, 0 );

        color2[0] = color2[1] = color2[2] = 0.3f;
        color2[3] *= 0.6f;

        // border
        CG_FilledBar( 148, 104, 2, 230, color2, NULL, NULL, 1.0f, 0 );    // left
        CG_FilledBar( 490, 104, 2, 230, color2, NULL, NULL, 1.0f, 0 );    // right
        CG_FilledBar( 148, 102, 344, 2, color2, NULL, NULL, 1.0f, 0 );    // top
        CG_FilledBar( 148, 334, 344, 2, color2, NULL, NULL, 1.0f, 0 );    // bot


        // text boxes
        color2[0] = color2[1] = color2[2] = 0.4f;
        for ( i = 0; i < 5; i++ ) {
                CG_FilledBar( 170, 154 + ( 28 * i ), 300, 20, color2, NULL, NULL, 1.0f, 0 );
        }


        // green title
        color2[0] = color2[2] = 0;
        color2[1] = 0.3f;
        CG_FilledBar( 150, 104, 340, 20, color2, NULL, NULL, 1.0f, 0 );

        color2[0] = color2[1] = color2[2] = 0.2f;

        // title
        color2[0] = color2[1] = color2[2] = 1;
        color2[3] = color[3];
//      CG_Text_Paint(280, 120, 2, 0.25f, color2, va("%s", CG_translateString("end_title")), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE);
        //----(SA)      scale change per MK
        CG_Text_Paint( 280, 120, 2, 0.313f, color2, va("scores"), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE );

        color2[0] = color2[1] = color2[2] = 1;
}


/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawScoreboard( void ) {
	int x = 0, y = 0, w;     // TTimo init
	float fade;
	float   *fadeColor;
	char    *s;

	// don't draw amuthing if the menu or console is up
	if ( cg_paused.integer ) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

        // fretn
	// still need to see 'mission failed' message in SP
	if ( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_DEAD && !cg_coop.integer) {
		return qfalse;
	}

	if ( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION && !cg_coop.integer) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if ( cg.warmup && !cg.showScores ) {
		return qfalse;
	}

	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
		 cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
		fadeColor = colorWhite;
	} else {
		fadeColor = CG_FadeColor( cg.scoreFadeTime, FADE_TIME );

		if ( !fadeColor ) {
			// next time scoreboard comes up, don't print killer
			cg.deferredPlayerLoading = 0;
			cg.killerName[0] = 0;
			return qfalse;
		}
		fade = *fadeColor;
	}


	// fragged by ... line
	if ( cg.killerName[0] ) {
		s = va( "Killed by %s", cg.killerName );
		w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
		x = ( SCREEN_WIDTH - w ) / 2;
		y = 40;
		CG_DrawBigString( x, y, s, fade );
	}

	// current rank

        // fretn
	if ( cgs.gametype == GT_SINGLE_PLAYER && cg_coop.integer) {   //----(SA) modified
                /*cg.cursorHintTime = cg.time;
                cg.cursorHintFade = cg_hintFadeTime.integer;
                cg.exitStatsFade = 250;     // fade /up/ time
                if ( !cg.exitStatsTime ) {
                        cg.exitStatsTime = cg.time;
                } 
                */
                CG_DrawCoopScoreboard();
		//CG_DrawBigString( 30, 50, "COOP SCORES", 1.0 );
		//y = CG_TeamScoreboard( x, y, TEAM_FREE, fade );

		/*		s = va( "%s place with %i",
						CG_PlaceString( cg.snap->ps.persistant[PERS_RANK] + 1 ),
						cg.snap->ps.persistant[PERS_SCORE] );
				w = CG_DrawStrlen( s ) * BIGCHAR_WIDTH;
				x = ( SCREEN_WIDTH - w ) / 2;
				y = 60;
				CG_DrawBigString( x, y, s, fade );*/
        }

	// load any models that have been deferred
	if ( ++cg.deferredPlayerLoading > 1 ) {
		CG_LoadDeferredPlayers();
	}

	return qtrue;
}
