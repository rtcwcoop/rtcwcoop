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

#include "g_local.h"
#include "g_coop.h"

/*
=======================================================================

  SESSION DATA

Session data is the only data that stays persistant across level loads
and tournament restarts.
=======================================================================
*/

/*
================
G_WriteClientSessionData

Called on game shutdown
================
*/
void G_WriteClientSessionData( gclient_t *client ) {
	const char  *s;
	const char  *var;

	if ( level.fResetStats ) {
		Coop_DeleteStats( client - level.clients );
	}

#ifdef _ADMINS
	// Added Admin sessions and stuff related to it
	s = va( "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",       // DHM - Nerve
			client->sess.sessionTeam,
			client->sess.spectatorNum,
			client->sess.spectatorState,
			client->sess.spectatorClient,
			client->sess.wins,
			client->sess.losses,
			client->sess.playerType,    // DHM - Nerve
			client->sess.playerWeapon,  // DHM - Nerve
			client->sess.playerPistol,  // DHM - Nerve
			client->sess.playerItem,    // DHM - Nerve
			client->sess.playerSkin,    // DHM - Nerve
			client->sess.latchPlayerType,    // DHM - Nerve
			client->sess.admin,	    // Admins
			client->sess.ip[0],         // IP
			client->sess.ip[1],         // IP
			client->sess.ip[2],         // IP
			client->sess.ip[3],         // IP
			client->sess.incognito,     // Toggle admin presence
			client->sess.ignored        // Ignored players
			);
#else
	s = va( "%i %i %i %i %i %i %i %i %i %i %i %i %i",       // DHM - Nerve
			client->sess.sessionTeam,
			client->sess.spectatorNum,
			client->sess.spectatorState,
			client->sess.spectatorClient,
			client->sess.wins,
			client->sess.losses,
			client->sess.playerType,    // DHM - Nerve
			client->sess.playerWeapon,  // DHM - Nerve
			client->sess.playerPistol,  // DHM - Nerve
			client->sess.playerItem,    // DHM - Nerve
			client->sess.playerSkin,    // DHM - Nerve
			client->sess.latchPlayerType,    // DHM - Nerve
			client->sess.ignored        // Ignored players
			);
#endif

	var = va( "session%i", (int)(client - level.clients) );

	trap_Cvar_Set( var, s );
}

/*
================
G_ReadSessionData

Called on a reconnect
================
*/
void G_ReadSessionData( gclient_t *client ) {
	char s[MAX_STRING_CHARS];
	const char  *var;

	var = va( "session%i", (int)(client - level.clients) );
	trap_Cvar_VariableStringBuffer( var, s, sizeof( s ) );

#ifdef _ADMINS
	// Added Admin sessions and stuff related to it
	sscanf( s, "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i",       // DHM - Nerve
			(int *)&client->sess.sessionTeam,
			&client->sess.spectatorNum,
			(int *)&client->sess.spectatorState,
			&client->sess.spectatorClient,
			&client->sess.wins,
			&client->sess.losses,
			&client->sess.playerType,       // DHM - Nerve
			&client->sess.playerWeapon,     // DHM - Nerve
			&client->sess.playerPistol,     // DHM - Nerve
			&client->sess.playerItem,       // DHM - Nerve
			&client->sess.playerSkin,       // DHM - Nerve
			&client->sess.latchPlayerType,       // DHM - Nerve
			(int *)&client->sess.admin,	// Admins
			(int *)&client->sess.ip[0],	// IP
			(int *)&client->sess.ip[1],	// IP
			(int *)&client->sess.ip[2],	// IP
			(int *)&client->sess.ip[3],	// IP
			&client->sess.incognito,	// Toggle admin presence
			&client->sess.ignored		// Ignored players
			);
#else
	sscanf( s, "%i %i %i %i %i %i %i %i %i %i %i %i %i",       // DHM - Nerve
			(int *)&client->sess.sessionTeam,
			&client->sess.spectatorNum,
			(int *)&client->sess.spectatorState,
			&client->sess.spectatorClient,
			&client->sess.wins,
			&client->sess.losses,
			&client->sess.playerType,       // DHM - Nerve
			&client->sess.playerWeapon,     // DHM - Nerve
			&client->sess.playerPistol,     // DHM - Nerve
			&client->sess.playerItem,       // DHM - Nerve
			&client->sess.playerSkin,       // DHM - Nerve
			&client->sess.latchPlayerType,       // DHM - Nerve
			&client->sess.ignored		// Ignored players
			);
#endif
}


/*
================
G_InitSessionData

Called on a first-time connect
================
*/
void G_InitSessionData( gclient_t *client, char *userinfo ) {
	clientSession_t *sess;
	const char      *value;

	sess = &client->sess;

	// check for team preference, mainly for bots
	value = Info_ValueForKey( userinfo, "teampref" );

	// check for human's team preference set by start server menu
	if ( !value[0] && g_localTeamPref.string[0] /*&& client->pers.localClient*/ ) {
		value = g_localTeamPref.string;

		// clear team so it's only used once
		trap_Cvar_Set( "g_localTeamPref", "" );
	}

	// initial team determination
	if ( g_gametype.integer == GT_COOP_BATTLE ) {
 		// always spawn as spectator in team games
 		sess->sessionTeam = TEAM_SPECTATOR;
		sess->spectatorState = SPECTATOR_FREE;

		if ( value[0] || g_teamAutoJoin.integer ) {
			SetTeam( &g_entities[client - level.clients], value, qfalse );
 		}
 	} else {
		if ( value[0] == 's' ) {
		// a willing spectator, not a waiting-in-line
		sess->sessionTeam = TEAM_SPECTATOR;
		} else {
			if ( g_maxGameClients.integer > 0 &&
				level.numNonSpectatorClients >= g_maxGameClients.integer ) {
				sess->sessionTeam = TEAM_SPECTATOR;
			} else {
				sess->sessionTeam = TEAM_FREE;
			}
		}

		sess->spectatorState = SPECTATOR_FREE;
	}

	AddTournamentQueue(client);

	// DHM - Nerve
	sess->playerType = 0;
	sess->playerWeapon = 0;
	sess->playerPistol = 0;
	sess->playerItem = 0;
	sess->playerSkin = 0;
	sess->latchPlayerType = 0;
	// dhm - end
#ifdef _ADMINS
	sess->admin = ADM_NONE; // admin
	sess->ip[0] = 0;        // ip
	sess->ip[1] = 0;        // ip
	sess->ip[2] = 0;        // ip
	sess->ip[3] = 0;        // ip
	sess->incognito = 0;    // admin presence
#endif
	sess->ignored = 0;      // Ignored players

	Coop_DeleteStats( client - level.clients );

	G_WriteClientSessionData( client );
}


/*
==================
G_InitWorldSession

==================
*/
void G_InitWorldSession( void ) {
	char s[MAX_STRING_CHARS];
	int gt;

	trap_Cvar_VariableStringBuffer( "session", s, sizeof( s ) );
	gt = atoi( s );

	if ( g_gametype.integer != gt ) {
		level.fResetStats = qtrue;
	}

	// if the gametype changed since the last session, don't use any
	// client sessions
	if ( g_gametype.integer != gt ) {
		level.newSession = qtrue;
		G_Printf( "Gametype changed, clearing session data.\n" );
	}
}

/*
==================
G_WriteSessionData

==================
*/
void G_WriteSessionData( void ) {
	int i;

	trap_Cvar_Set( "session", va( "%i", g_gametype.integer ) );

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected == CON_CONNECTED ) {
			G_WriteClientSessionData( &level.clients[i] );
		} else if ( level.fResetStats ) {
			Coop_DeleteStats( level.sortedClients[i] );
		}
	}
}
