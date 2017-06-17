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

// cg_scoreboard -- draw the scoreboard on top of the game screen
#include "cg_local.h"
#include "../ui/ui_shared.h"

#define SCOREBOARD_WIDTH    ( 31 * BIGCHAR_WIDTH )

void CG_DrawCoopScoreboard( void ) {
	int i, place = 0, w;
	float *color;       // faded color based on cursor hint drawing
	float color2[4] = {0, 0, 0, 1};
	float color3[4] = {1, 1, 1, 1};
	float black[4] = {0, 0, 0, 1};
	clientInfo_t *ci;
	long score;
	int ping;
#ifdef MONEY
	int deaths = 0;
	float damage_ratio = 0.0f;
	int highest_score;
	float highest_damage_ratio;
	int lowest_deaths;
	float green[4] = {0, 1, 0, 1};
#endif
	const char *s;
	int msec, mins, seconds, tens;     // JPW NERVE
	qboolean maxlives = qfalse;

		#define MAX_STATS_VARS  64

	if ( cg_paused.integer ) {
		// no draw if any menu's are up  (or otherwise paused)
		return;
	}

	color = colorWhite;

	if ( !color ) {     // currently faded out, don't draw
		return;
	}

	// draw winner
	if ( cg.predictedPlayerState.pm_type == PM_INTERMISSION && cgs.gametype == GT_COOP_BATTLE ) {
		const char *s, *buf;
		int w;

		s = CG_ConfigString( CS_BATTLE_INFO );
		buf = Info_ValueForKey( s, "winner" );
#ifdef LOCALISATION
		w = CG_DrawStrlen( va( CG_TranslateString( "%s wins!" ), buf ) ) * BIGCHAR_WIDTH;
		CG_DrawBigString( 320 - ( w / 2 ), 40, va( CG_TranslateString( "%s wins !" ), buf ), 1.0F );
#else
		w = CG_DrawStrlen( va( "%s wins!", buf ) ) * BIGCHAR_WIDTH;
		CG_DrawBigString( 320 - ( w / 2 ), 40, va( "%s wins !", buf ), 1.0F );
#endif
		if ( !cg.latchVictorySound ) {
			cg.latchVictorySound = qtrue;
			trap_S_StartLocalSound( trap_S_RegisterSound( "sound/multiplayer/music/l_complete_2.wav" ), CHAN_LOCAL_SOUND );
		}
	}

	color2[3] = color[3];

	// patched for 8 players.
	// background
	color2[3] *= 0.6f;
	CG_FilledBar( 150, 104, 340, 280, color2, NULL, NULL, 1.0f, 0 );

	color2[0] = color2[1] = color2[2] = 0.3f;
	color2[3] *= 0.6f;

	// border
	CG_FilledBar( 148, 104, 2, 280, color2, NULL, NULL, 1.0f, 0 );        // left
	CG_FilledBar( 490, 104, 2, 280, color2, NULL, NULL, 1.0f, 0 );        // right
	CG_FilledBar( 148, 102, 344, 2, color2, NULL, NULL, 1.0f, 0 );        // top
	CG_FilledBar( 150, 124, 342, 2, color2, NULL, NULL, 1.0f, 0 );        // under green bar
	CG_FilledBar( 148, 384, 344, 2, color2, NULL, NULL, 1.0f, 0 );        // bot


	// text boxes
	color2[0] = color2[1] = color2[2] = 0.4f;
#ifdef LOCALISATION
	CG_DrawStringExt( 175, 130, CG_TranslateString( "Name" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#else
	CG_DrawStringExt( 175, 130, va( "Name" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#endif
#ifdef MONEY
	if ( cgs.gametype == GT_COOP_BATTLE ) {
		w = strlen( "Ratio" ) * SMALLCHAR_WIDTH;
		CG_DrawStringExt( 170 + 140 - w, 130, va( "Ratio" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;

		w = strlen( "Deaths" ) * SMALLCHAR_WIDTH;
		CG_DrawStringExt( 170 + 210 - w, 130, va( "Deaths" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
	}
#endif
	w = strlen( "Score" ) * SMALLCHAR_WIDTH;
#ifdef LOCALISATION
	CG_DrawStringExt( 170 + 255 - w, 130, CG_TranslateString( "Score" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#else
	CG_DrawStringExt( 170 + 255 - w, 130, va( "Score" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#endif
	w = strlen( "Ping" ) * SMALLCHAR_WIDTH;
#ifdef LOCALISATION
	CG_DrawStringExt( 170 + 294 - w, 130, CG_TranslateString( "Ping" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#else
	CG_DrawStringExt( 170 + 294 - w, 130, va( "Ping" ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#endif
	//if (cg.scores[0].respawnsLeft != -1)
	//        maxlives = qtrue;

	// first get highest values

#ifdef MONEY
	highest_score = 0;
	highest_damage_ratio = 0.0;
	lowest_deaths = 99;

	for ( i = 0; i < MAX_COOP_CLIENTS; i++ ) {
		float tmp = 0.0;

		if ( cg.scores[i].score > highest_score ) {
			highest_score = cg.scores[i].score;
		}

		if ( cg.scores[i].deaths < lowest_deaths ) {
			lowest_deaths = cg.scores[i].deaths;
		}

		if ( cg.scores[i].damage_received > 0 ) {
			tmp = (float)cg.scores[i].damage_given / (float)cg.scores[i].damage_received;
		} else {
			tmp = (float)cg.scores[i].damage_given;
		}

		if ( tmp > highest_damage_ratio ) {
			highest_damage_ratio = tmp;
		}
	}
#endif

	for ( i = 0; i < 8; i++ ) {
		if ( cg.clientNum == cg.scores[i].client && i < cg.numScores ) {
			CG_FilledBar( 170, 154 + ( 28 * i ), 300, 20, black, NULL, NULL, 1.0f, 0 );
		} else {
			CG_FilledBar( 170, 154 + ( 28 * i ), 300, 20, color2, NULL, NULL, 1.0f, 0 );
		}

		ci = &cgs.clientinfo[cg.scores[i].client];

		if ( i < cg.numScores ) {
			ping = cg.scores[i].ping;
			score = cg.scores[i].score;
#ifdef MONEY
			if ( cgs.gametype == GT_COOP_BATTLE ) {
				deaths = cg.scores[i].deaths;

				if ( cg.scores[i].damage_received > 0 ) {
					damage_ratio = (float)cg.scores[i].damage_given / (float)cg.scores[i].damage_received;
				} else {
					damage_ratio = (float)cg.scores[i].damage_given;
				}
			}
#endif


			place++;
			if ( ci->team == TEAM_SPECTATOR ) {
#ifdef LOCALISATION
				CG_DrawStringExt( 175, 154 + ( 28 * i ) + 1, va( CG_TranslateString( "[SPECTATOR] %s" ), ci->name ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 20 ) ;
#else
				CG_DrawStringExt( 175, 154 + ( 28 * i ) + 1, va( "[SPECTATOR] %s", ci->name ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 20 ) ;
#endif
			} else if ( ci->team == TEAM_RED ) {
				CG_DrawStringExt( 175, 154 + ( 28 * i ) + 1, va( "^1%i^7. %s", place, ci->name ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 20 ) ;
			} else {
				CG_DrawStringExt( 175, 154 + ( 28 * i ) + 1, va( "%i. %s", place, ci->name ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 20 ) ;
			}

#ifdef MONEY
			if ( cgs.gametype == GT_COOP_BATTLE ) {
				w = strlen( va( "%.2f", damage_ratio ) ) * SMALLCHAR_WIDTH;
				if ( ci->team != TEAM_SPECTATOR ) {
					if ( damage_ratio == highest_damage_ratio ) {
						CG_DrawStringExt( 170 + 140 - w, 154 + ( 28 * i ) + 1, va( "%.2f", damage_ratio ), green, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					} else {
						CG_DrawStringExt( 170 + 140 - w, 154 + ( 28 * i ) + 1, va( "%.2f", damage_ratio ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					}
				}


				w = strlen( va( "%d", deaths ) ) * SMALLCHAR_WIDTH;
				if ( ci->team != TEAM_SPECTATOR ) {
					if ( cg.scores[i].deaths == lowest_deaths ) {
						CG_DrawStringExt( 170 + 210 - w, 154 + ( 28 * i ) + 1, va( "%d", cg.scores[i].deaths ), green, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					} else {
						CG_DrawStringExt( 170 + 210 - w, 154 + ( 28 * i ) + 1, va( "%d", cg.scores[i].deaths ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					}
				}
			}
#endif


			if ( maxlives )
			{
				w = strlen( va( "%ld (%d)", score, cg.scores[i].respawnsLeft ) ) * SMALLCHAR_WIDTH;
				if ( ci->team != TEAM_SPECTATOR )
				{
#ifdef MONEY
					if ( cg.scores[i].score == highest_score )
					{
						CG_DrawStringExt( 170 + 255 - w, 154 + ( 28 * i ) + 1, va( "%d (%d)", cg.scores[i].score, cg.scores[i].respawnsLeft ), green, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					}
					else
#endif
					{
						CG_DrawStringExt( 170 + 255 - w, 154 + ( 28 * i ) + 1, va( "%d (%d)", cg.scores[i].score, cg.scores[i].respawnsLeft ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					}
				}
			}
			else
			{
				w = strlen( va( "%ld", score ) ) * SMALLCHAR_WIDTH;
				if ( ci->team != TEAM_SPECTATOR )
				{
#ifdef MONEY
					if ( cg.scores[i].score == highest_score )
					{
						CG_DrawStringExt( 170 + 255 - w, 154 + ( 28 * i ) + 1, va( "%d", cg.scores[i].score ), green, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					}
					else
#endif
					{
						CG_DrawStringExt( 170 + 255 - w, 154 + ( 28 * i ) + 1, va( "%d", cg.scores[i].score ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
					}
				}
			}

			w = strlen( va( "%d", ping ) ) * SMALLCHAR_WIDTH;
			CG_DrawStringExt( 170 + 294 - w, 154 + ( 28 * i ) + 1, va( "%d", cg.scores[i].ping ), color3, qfalse, qtrue, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
		}
	}


	// green title
	color2[0] = color2[2] = 0;
	color2[1] = 0.3f;
	CG_FilledBar( 150, 104, 340, 20, color2, NULL, NULL, 1.0f, 0 );

	color2[0] = color2[1] = color2[2] = 0.2f;

	// title
	color2[0] = color2[1] = color2[2] = 1;
	color2[3] = color[3];
	if ( cgs.gametype == GT_COOP ) {
#ifdef LOCALISATION
		w = strlen( CG_TranslateString( "Score" ) ) * SMALLCHAR_WIDTH;
		CG_DrawStringExt( 170 + ( 344 / 2 ) - ( w - 2 ), 105, CG_TranslateString( "Score" ), color3, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#else
		w = strlen( "scores" ) * SMALLCHAR_WIDTH;
		CG_DrawStringExt( 170 + ( 344 / 2 ) - ( w - 2 ), 105, va( "scores" ), color3, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;
#endif
	}



	if ( cgs.gametype == GT_COOP_SPEEDRUN ) {
		msec = ( cgs.timelimit * 60.f * 1000.f ) - ( cg.time - cgs.levelStartTime );
	} else {
		msec = ( cg.time - cgs.levelStartTime );
	}

	seconds = msec / 1000;
	mins = seconds / 60;
	seconds -= mins * 60;
	tens = seconds / 10;
	seconds -= tens * 10;

	if ( cgs.gametype == GT_COOP_SPEEDRUN )
#ifdef LOCALISATION
	{ s = va( CG_TranslateString( "Time to beat: %2.0f:%i%i" ), (float)mins, tens, seconds ); }
#else
	{ s = va( "Time to beat: %2.0f:%i%i", (float)mins, tens, seconds ); }
#endif
	else {
		s = va( "%2.0f:%i%i", (float)mins, tens, seconds );
	}

	CG_DrawStringExt( 170, 105, s, color3, qfalse, qfalse, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, 25 ) ;

	color2[0] = color2[1] = color2[2] = 1;
}


/*
=================
CG_DrawScoreboard

Draw the normal in-game scoreboard
=================
*/
qboolean CG_DrawScoreboard( void ) {
	int x = 0, y = 0, w;
	float fade;
	float   *fadeColor;
	char    *s;

	// don't draw anything if the menu or console is up
	if ( cg_paused.integer ) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	// still need to see 'mission failed' message in SP
	if ( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_DEAD ) {
		return qfalse;
	}

	if ( cgs.gametype == GT_SINGLE_PLAYER && cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		cg.deferredPlayerLoading = 0;
		return qfalse;
	}

	// don't draw scoreboard during death while warmup up
	if ( cg.warmup && !cg.showScores && cg.predictedPlayerState.pm_type != PM_INTERMISSION ) {
		return qfalse;
	}

	if ( cg_fixedAspect.integer ) {
		CG_SetScreenPlacement(PLACE_CENTER, PLACE_CENTER);
	}

	if ( cg.showScores || cg.predictedPlayerState.pm_type == PM_DEAD ||
		 cg.predictedPlayerState.pm_type == PM_INTERMISSION ) {
		fade = 1.0;
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

	if ( cgs.gametype <= GT_COOP ) {   //----(SA) modified
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
