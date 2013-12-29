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




// this file holds commands that can be executed by the server console, but not remote clients

#include "g_local.h"


/*
==============================================================================

L0 - Replaced old banning scheme all together.

==============================================================================
*/

/*
=================
L0 - Svcmd_AddIP_f

- from S4NDMOD
=================
*/
void Svcmd_AddIP_f( void ) {
	FILE        *bannedfile;

	char arg1[MAX_STRING_TOKENS];
	trap_Argv( 1, arg1, sizeof( arg1 ) );

	bannedfile = fopen( "banned.txt","a+" );

	fputs( va( "%s\n",arg1 ),bannedfile );
	G_LogPrintf( "%s was added to the banned file\n", arg1 );

	fclose( bannedfile );
}

/*
================
L0 - Svcmd_tempban_f

- from S4NDMOD
================
*/
void Svcmd_tempban_f( void ) {
	int clientNum;
	int bannedtime;
	gentity_t   *ent;
	char arg1[MAX_STRING_TOKENS];
	char arg2[MAX_STRING_TOKENS];

	trap_Argv( 1, arg1, sizeof( arg1 ) );
	clientNum = atoi( arg1 );
	ent = &g_entities[ clientNum ];

	trap_Argv( 2, arg2, sizeof( arg2 ) );
	bannedtime = atoi( arg2 );

	TEMPBAN_CLIENT( ent,bannedtime );
}

/*
===================
Svcmd_EntityList_f
===================
*/
void    Svcmd_EntityList_f( void ) {
	int e;
	gentity_t       *check;

	check = g_entities + 1;
	for ( e = 1; e < level.num_entities ; e++, check++ ) {
		if ( !check->inuse ) {
			continue;
		}
		G_Printf( "%3i:", e );
		switch ( check->s.eType ) {
		case ET_GENERAL:
			G_Printf( "ET_GENERAL          " );
			break;
		case ET_PLAYER:
			G_Printf( "ET_PLAYER           " );
			break;
		case ET_ITEM:
			G_Printf( "ET_ITEM             " );
			break;
		case ET_MISSILE:
			G_Printf( "ET_MISSILE          " );
			break;
		case ET_MOVER:
			G_Printf( "ET_MOVER            " );
			break;
		case ET_BEAM:
			G_Printf( "ET_BEAM             " );
			break;
		case ET_PORTAL:
			G_Printf( "ET_PORTAL           " );
			break;
		case ET_SPEAKER:
			G_Printf( "ET_SPEAKER          " );
			break;
		case ET_PUSH_TRIGGER:
			G_Printf( "ET_PUSH_TRIGGER     " );
			break;
		case ET_TELEPORT_TRIGGER:
			G_Printf( "ET_TELEPORT_TRIGGER " );
			break;
		case ET_INVISIBLE:
			G_Printf( "ET_INVISIBLE        " );
			break;
		case ET_GRAPPLE:
			G_Printf( "ET_GRAPPLE          " );
			break;
		case ET_EXPLOSIVE:
			G_Printf( "ET_EXPLOSIVE        " );
			break;
		case ET_TESLA_EF:
			G_Printf( "ET_TESLA_EF         " );
			break;
		case ET_SPOTLIGHT_EF:
			G_Printf( "ET_SPOTLIGHT_EF     " );
			break;
		case ET_EFFECT3:
			G_Printf( "ET_EFFECT3          " );
			break;
		case ET_ALARMBOX:
			G_Printf( "ET_ALARMBOX          " );
			break;
		default:
			G_Printf( "%3i                 ", check->s.eType );
			break;
		}

		if ( check->classname ) {
			G_Printf( "%s", check->classname );
		}
		G_Printf( "\n" );
	}
}

gclient_t   *ClientForString( const char *s ) {
	gclient_t   *cl;
	int i;
	int idnum;

	// L0 - moved this up..
	// check for a name match
	for ( i = 0 ; i < level.maxclients ; i++ ) {
		cl = &level.clients[i];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			continue;
		}
		if ( !Q_stricmp( cl->pers.netname, s ) ) {
			return cl;
		}
	}

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients ) {
			Com_Printf( "Bad client slot: %i\n", idnum );
			return NULL;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected == CON_DISCONNECTED ) {
			G_Printf( "Client %i is not connected\n", idnum );
			return NULL;
		}
		return cl;
	}

	G_Printf( "User %s is not on the server\n", s );

	return NULL;
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
void    Svcmd_ForceTeam_f( void ) {
	gclient_t   *cl;
	char str[MAX_TOKEN_CHARS];

	// find the player
	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );
	if ( !cl ) {
		return;
	}

	// set the team
	trap_Argv( 2, str, sizeof( str ) );
	SetTeam( &g_entities[cl - level.clients], str, qfalse );
}


/*
============
L0 - Print Poll answer,,
============
*/
void Svcmd_PollPrint_f( void ) {
	trap_SendServerCommand( -1, va( "chat \"console: Poll result is ^2Yes^7!\n\"" ) );
}

/*
============
L0 - Unignore
============
*/
void Svcmd_Unignore_f( void ) {
	int clientNum;
	char buf[5];

	if ( trap_Argc() != 2 ) {
		G_Printf( "Usage: unignore <client num>\n" );
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	clientNum = atoi( buf );

	if ( ( clientNum < 0 ) || ( clientNum >= MAX_CLIENTS ) ) {
		G_Printf( "Invalid client number.\n" );
		return;
	}

	if ( ( !g_entities[clientNum].client ) || ( level.clients[clientNum].pers.connected != CON_CONNECTED ) || ( g_entities[clientNum].r.svFlags & SVF_BOT ) ) {
		G_Printf( "Client not on server.\n" );
		return;
	}

	g_entities[clientNum].client->sess.ignored = 0;
	trap_SendServerCommand( clientNum, "cp \"You have been unignored^2!\n\"2" );  //let them know they can talk again
	trap_SendServerCommand( -1, va( "chat \"console: ^7%s ^7has been unignored.\n\"",g_entities[clientNum].client->pers.netname ) );
	return;
}

/*
============
L0 - Ignore
============
*/

void Svcmd_Ignore_f() {
	int clientNum;
	char buf[5];

	if ( trap_Argc() != 2 ) {
		G_Printf( "Usage: ignore <client num>\n" );
		return;
	}

	trap_Argv( 1, buf, sizeof( buf ) );
	clientNum = atoi( buf );

	if ( ( clientNum < 0 ) || ( clientNum >= MAX_CLIENTS ) ) {
		G_Printf( "Invalid client number\n" );
		return;
	}

	if ( ( !g_entities[clientNum].client ) || ( level.clients[clientNum].pers.connected != CON_CONNECTED ) || ( g_entities[clientNum].r.svFlags & SVF_BOT ) ) {
		G_Printf( "Client not on server.\n" );
		return;
	}

	g_entities[clientNum].client->sess.ignored = 1;
	trap_SendServerCommand( clientNum, "cp \"You are now ignored^1\n\"" );
	trap_SendServerCommand( -1, va( "chat \"console: ^7%s ^7has been ignored.\n\"",g_entities[clientNum].client->pers.netname ) );
	return;
}


char    *ConcatArgs( int start );

/*
=================
ConsoleCommand

=================
*/
qboolean    ConsoleCommand( void ) {
	char cmd[MAX_TOKEN_CHARS];

	trap_Argv( 0, cmd, sizeof( cmd ) );

	// Ridah, savegame
	if ( Q_stricmp( cmd, "savegame" ) == 0 ) {

		// only for single player games
		if ( g_gametype.integer != GT_SINGLE_PLAYER ) {
			return qtrue;
		}

		// don't allow a manual savegame command while we are waiting for the game to start/exit
		if ( g_reloading.integer ) {
			return qtrue;
		}
		if ( saveGamePending ) {
			return qtrue;
		}

		trap_Argv( 1, cmd, sizeof( cmd ) );
		if ( strlen( cmd ) > 0 ) {
			// strip the extension if provided
			if ( strrchr( cmd, '.' ) ) {
				cmd[strrchr( cmd,'.' ) - cmd] = '\0';
			}
			if ( !Q_stricmp( cmd, "current" ) ) {     // beginning of map
				Com_Printf( "sorry, '%s' is a reserved savegame name.  please use another name.\n", cmd );
				return qtrue;
			}

			if ( G_SaveGame( cmd ) ) {
#ifdef LOCALISATION
				trap_SendServerCommand( -1, "cp Game Saved" );  // deletedgame
#else
				trap_SendServerCommand( -1, "cp gamesaved" );  // deletedgame
#endif
			} else {
				G_Printf( "Unable to save game.\n" );
			}

		} else {    // need a name
			G_Printf( "syntax: savegame <name>\n" );
		}

		return qtrue;
	}
	// done.

	if ( Q_stricmp( cmd, "entitylist" ) == 0 ) {
		Svcmd_EntityList_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "forceteam" ) == 0 ) {
		Svcmd_ForceTeam_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "game_memory" ) == 0 ) {
		Svcmd_GameMem_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "addbot" ) == 0 ) {
		Svcmd_AddBot_f();
		return qtrue;
	}

	// L0 - Enhancements
	// Poll
	if ( Q_stricmp( cmd, "Poll:" ) == 0 ) {
		Svcmd_PollPrint_f();
		return qtrue;
	}

	// Ban
	if ( Q_stricmp( cmd, "addip" ) == 0 ) {
		Svcmd_AddIP_f();
		return qtrue;
	}

	// Tempban
	if ( Q_stricmp( cmd, "tempban" ) == 0 ) {
		Svcmd_tempban_f();
		return qtrue;
	}

	// Ignore
	if ( Q_stricmp( cmd, "ignore" ) == 0 ) {
		Svcmd_Ignore_f();
		return qtrue;
	}
	// Unignore
	if ( Q_stricmp( cmd, "unignore" ) == 0 ) {
		Svcmd_Unignore_f();
		return qtrue;
	}
	// L0 - end

	if ( g_dedicated.integer ) {
		if ( Q_stricmp( cmd, "say" ) == 0 ) {
			trap_SendServerCommand( -1, va( "print \"server: %s\"", ConcatArgs( 1 ) ) );
			return qtrue;
		}
		// everything else will also be printed as a say command
		trap_SendServerCommand( -1, va( "print \"server: %s\"", ConcatArgs( 0 ) ) );
		return qtrue;
	}

	return qfalse;
}
