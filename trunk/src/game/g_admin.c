/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (?RTCW SP Source Code?).  

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

/*
****************************************************************************
* g_admins.c
* 
* Functionality for server Administration in CO-OP mode.
* 
* Author: Nate L0
* Date: 11.09/12
* Last edit: 
****************************************************************************
*/
#ifdef _ADMINS
#include "g_local.h"

/*
===========
Login
===========
*/
void cmd_do_login (gentity_t *ent, qboolean silent) { 
	char str[MAX_TOKEN_CHARS];
	qboolean error;
	char *tag;

	error = qfalse;
	trap_Argv( 1, str, sizeof( str ) );

	// Make sure user is not already logged in.
	if (!ent->client->sess.admin == ADM_NONE ) 
	{
		trap_SendServerCommand( ent-g_entities, va("print \"You are already logged in^1!\n\""));			
	return;	
	}	
	// Prevent bogus logins	
	if (( !Q_stricmp( str, "\0")) || ( !Q_stricmp( str, "")) || ( !Q_stricmp( str, "\"")) || ( !Q_stricmp( str, "none")) ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Incorrect password^1!\n\""));		
	return;
	}

		// Else let's see if there's a password match.
		if ( (Q_stricmp(str, a1_pass.string) == 0) || (Q_stricmp(str, a2_pass.string) == 0) || (Q_stricmp(str, a3_pass.string) == 0) ) {
			
			// Always start with lower level as if owner screws it up and sets the same passes for more levels, the lowest is the safest bet.
			if (Q_stricmp(str, a1_pass.string) == 0) {
				ent->client->sess.admin = ADM_MEM;
			tag = a1_tag.string;
			} else if (Q_stricmp(str, a2_pass.string) == 0) {
				ent->client->sess.admin = ADM_MED;
			tag = a2_tag.string;
			} else if (Q_stricmp(str, a3_pass.string) == 0) {
				ent->client->sess.admin = ADM_FULL;
			tag = a3_tag.string;
			} else {
				error = qtrue;
			} 
				// Something went to hell..
				if (error == qtrue) {
					// User shouldn't be anything but regular so make sure..
					ent->client->sess.admin = ADM_NONE;
						trap_SendServerCommand( ent-g_entities, va("print \"Error has occured while trying to log you in^1!\n\""));		
				return;
				}

				// We came so far so go with it..
				if (silent) {
					trap_SendServerCommand( ent-g_entities, va("print \"Silent Login successful^2!\n\""));		
					ent->client->sess.incognito = 1; // Hide them
				} else {
					trap_SendServerCommand( -1, va("chat \"console: %s ^7has logged in as %s^7!\n\"", ent->client->pers.netname, tag));		
				}
		return;
		// No match..
		} else {
			trap_SendServerCommand( ent-g_entities, va("print \"Incorrect password^1!\n\""));		
		return;
		}
}

/*
===========
Logout
===========
*/
void cmd_do_logout(gentity_t *ent) {
	// If user is not logged in do nothing
	if (ent->client->sess.admin == ADM_NONE) {
		return;
	} else {
		// Admin is hidden so don't print 
		if (ent->client->sess.incognito) 
			trap_SendServerCommand( ent-g_entities, va("print \"You have successfully logged out^2!\n\""));	
		else
			trap_SendServerCommand( -1, va("chat \"console: %s ^7has logged out!\n\"", ent->client->pers.netname));

		// Log them out now
		ent->client->sess.admin = ADM_NONE;
		// Set incognito to visible..
		ent->client->sess.incognito = 0;
	return;
	}
}

/*********************************** FUNCTIONALITY ************************************/

/*
===========
Get client number from name
===========
*/
int ClientNumberFromNameMatch(char *name, int *matches){
	int i, textLen;
	char nm[32];
	char c;
	int index = 0;

	Q_strncpyz(nm, name, sizeof(nm));	
	Q_CleanStr(nm);						
	textLen = strlen(nm);
	c = *nm;							

	for (i = 0; i < level.maxclients; i++)	
	{
		int j, len;
		char playerName[32];

		if ((!g_entities[i].client) || (g_entities[i].client->pers.connected != CON_CONNECTED) || ( g_entities[i].r.svFlags & SVF_BOT ) ) // Don't count AI ...
			continue;
		
		Q_strncpyz(playerName, g_entities[i].client->pers.netname, sizeof(playerName));	
		Q_CleanStr(playerName);		
		len = strlen(playerName);

		for (j = 0; j < len; j++)	
		{
			if (tolower(c) == tolower(playerName[j]))	
			{			
				if (!Q_stricmpn(nm, playerName+j, textLen))	
				{
					matches[index] = i;		
					index++;				
					break;					
				}
			}
		}
	}
return index;	
}

/*
==================
ClientNumberFromString - Basically copy-paste from g_cmds.c - with a tweak for bots as I'm to lazy to check if it would break anything there..
==================
*/
int ClientNumberFromString2( gentity_t *to, char *s ) {
	gclient_t   *cl;
	int idnum;
	char s2[MAX_STRING_CHARS];
	char n2[MAX_STRING_CHARS];

	// numeric values are just slot numbers
	if ( s[0] >= '0' && s[0] <= '9' ) {
		idnum = atoi( s );
		if ( idnum < 0 || idnum >= level.maxclients || g_entities[idnum].r.svFlags & SVF_BOT  ) { // Ignore players only not AI...
			trap_SendServerCommand( to - g_entities, va( "print \"Bad client slot: %i\n\"", idnum ) );
			return -1;
		}

		cl = &level.clients[idnum];
		if ( cl->pers.connected != CON_CONNECTED ) {
			trap_SendServerCommand( to - g_entities, va( "print \"Client %i is not active\n\"", idnum ) );
			return -1;
		}
		return idnum;
	}

	// check for a name match
	SanitizeString( s, s2 );
	for ( idnum = 0,cl = level.clients ; idnum < level.maxclients ; idnum++,cl++ ) {
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		SanitizeString( cl->pers.netname, n2 );
		if ( !strcmp( n2, s2 ) ) {
			return idnum;
		}
	}

	trap_SendServerCommand( to - g_entities, va( "print \"User %s is not on the server\n\"", s ) );
	return -1;
}


/*
===========
Deals with ! & ?
===========
*/
void admCmds(const char *strCMD1, char *strCMD2, char *strCMD3, qboolean cmd){
	
	int i = 0, j=0;
	int foundcolon=0;
	
	while(strCMD1[i] != 0)
	{
		if(!foundcolon)
		{	
			if (cmd) {
				if(strCMD1[i] == '?') {
					foundcolon = 1;
					strCMD2[i]=0;
				}
				else
					strCMD2[i]=strCMD1[i];
				i++;
			} else {
				if(strCMD1[i] == '!') {
					foundcolon = 1;
					strCMD2[i]=0;
				}
				else
					strCMD2[i]=strCMD1[i];
				i++;
			}
		}
		else
		{
			strCMD3[j++]=strCMD1[i++];
		}
	}
	if(!foundcolon)
		strCMD2[i]=0;
	strCMD3[j]=0;
}

/*
===========
Parse string (if I recall right this bit is from S4NDMoD)
===========
*/
void ParseAdmStr(const char *strInput, char *strCmd, char *strArgs)
{
	int i = 0, j=0;
	int foundspace=0;

	while(strInput[i] != 0){
		if(!foundspace){
			if(strInput[i] == ' '){
				foundspace = 1;
				strCmd[i]=0;
			}else
				strCmd[i]=strInput[i];
			i++;
		}else{ 
			strArgs[j++]=strInput[i++];
		}
	}
	if(!foundspace)
		strCmd[i]=0;

strArgs[j]=0;
}

/*
===========
Determine if target has a higher level
===========
*/
qboolean isHigher(gentity_t *ent, gentity_t *targ) {

	if (ent->client->sess.admin > targ->client->sess.admin)
		return qtrue;
	else 
		return qfalse;
}

/*
===========
Determine if admin level allows command
===========
*/
qboolean canUse(gentity_t *ent) {
	char *permission="";
	char *token, *parse;
	char alt[128];
	char cmd[128];

	switch (ent->client->sess.admin) {
		case ADM_NONE:// So linux stops complaining..
			return qfalse;
		break;
		case ADM_MEM:
			permission = a1_cmds.string;
		break;
		case ADM_MED:
			permission = a2_cmds.string;
		break;
		case ADM_FULL:
			if (a3_allowAll.integer)
				return qtrue;
			else
				permission = a3_cmds.string;
		break;			
	}

	admCmds(ent->client->pers.cmd1, alt, cmd, qfalse);
			
	if (strlen(permission)) {
		parse = permission;
		while (1) {
			token = COM_Parse(&parse);
			if (!token || !token[0])		
				break;

			if (!Q_stricmp(cmd, token))			
				return qtrue;		
		}
		return qfalse;
	}
return qfalse;
}

/*
===========
Can't use command msg..
===========
*/
void cantUse(gentity_t *ent) {
	char alt[128];
	char cmd[128];

	admCmds(ent->client->pers.cmd1, alt, cmd, qfalse);

	trap_SendServerCommand( ent-g_entities, va("print \"Command ^1%s ^7is not allowed for your level^1!\n\"", cmd));	
return;
}

/*
==================
Ported from et: NQ
DecolorString

Remove color characters
==================
*/
void DecolorString( char *in, char *out)
{
	while(*in) {
		if(*in == 27 || *in == '^') {
			in++;		// skip color code
			if(*in) in++;
			continue;
		}
		*out++ = *in++;
	}
	*out = 0;
}

/*********************************** COMMANDS ************************************/

/*
===========
Toggle incognito
===========
*/
void cmd_incognito(gentity_t *ent) {
	if (ent->client->sess.admin == ADM_NONE)
		return;

	if (ent->client->sess.incognito == 0) {
		ent->client->sess.incognito = 1;
			trap_SendServerCommand( ent-g_entities, va("cp \"You are now incognito^2!\n\"2"));	
	return;
	} else {
		ent->client->sess.incognito = 0;
			trap_SendServerCommand( ent-g_entities, va("cp \"Your status is now set to visible^2!\n\"2"));	
	return;
	}
}

/*
===========
Ignore user
===========
*/
void cmd_ignore(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
		
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
		if (count == 0){
			trap_SendServerCommand(ent-g_entities, va("print \"Client not on server^1!\n\""));
		return;
	} else if (count > 1) {
		trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^1!\n\"", ent->client->pers.cmd2));
	return;
	}
	
		for (i = 0; i < count; i++){
			/*
			if (!isHigher(ent, &g_entities[nums[i]])) {
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7has has the same or a higher level than you^1!\"", g_entities[nums[i]].client->pers.netname));
			return;
			}*/

			if (g_entities[nums[i]].client->sess.ignored){
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7is already ignored^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
			}  else
				g_entities[nums[i]].client->sess.ignored = 1;
				trap_SendServerCommand(-1, va("chat \"console: Admin has ignored player %s^1!\n\"", g_entities[nums[i]].client->pers.netname));
			}
		
return;
}

/*
===========
UnIgnore user
===========
*/
void cmd_unignore(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
		
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
		if (count == 0){
			trap_SendServerCommand(ent-g_entities, va("print \"Client not on server^1!\n\""));
		return;
	}else if (count > 1){
		trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^11\n\"", ent->client->pers.cmd2));
	return;
	}
	
		for (i = 0; i < count; i++){	
			/*
			if (!isHigher(ent, &g_entities[nums[i]])) {
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7has has the same or a higher level than you^1!\"", g_entities[nums[i]].client->pers.netname));
			return;
			}
			*/

			if (!g_entities[nums[i]].client->sess.ignored){
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7is already Unignored^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
			}  else
				g_entities[nums[i]].client->sess.ignored = 0;
				trap_SendServerCommand(-1, va("chat \"console: Admin has Unignored player %s^1!\n\"", g_entities[nums[i]].client->pers.netname));
			}
		
return;
}

/*
===========
Ignore user based upon client number
===========
*/
void cmd_clientIgnore( gentity_t *ent )
{
	int	player_id;
	gentity_t	*target_ent;
	gclient_t	*client;
	gentity_t	*targetclient;	 


	player_id = ClientNumberFromString2( ent, ent->client->pers.cmd2 );
	if ( player_id == -1 ) { // 
		return;
	} 

	// Make sure we're not ignoring bots.. (It can happen..)
	if ( player_id > 8) // XXX: Keep an eye on this since if slots increase we'll need to upscale this..
		return;

	targetclient = g_entities + player_id; 
	target_ent = g_entities + player_id;
	client = target_ent->client;

	if (targetclient->client->sess.ignored ) {
		trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7is already ignored^1!\"", targetclient->client->pers.netname ));
	return;
	} 

	targetclient->client->sess.ignored = 1;
	trap_SendServerCommand(-1, va("chat \"console: Admin has ignored player %s^1!\"", targetclient->client->pers.netname ));
return;
}

/*
===========
UnIgnore user based upon client number
===========
*/
void cmd_clientUnignore( gentity_t *ent )
{
	int	player_id;
	gentity_t	*target_ent;
	gclient_t	*client;
	gentity_t	*targetclient;	 


	player_id = ClientNumberFromString2( ent, ent->client->pers.cmd2 );
	if ( player_id == -1 ) { // 
		return;
	}

	targetclient = g_entities + player_id; 
	target_ent = g_entities + player_id;
	client = target_ent->client;

	if (targetclient->client->sess.ignored == 0 ) {
		trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7is already unignored^1!\"", targetclient->client->pers.netname ));
	return;
	}

	targetclient->client->sess.ignored = 0;
	trap_SendServerCommand(-1, va("chat \"console: Admin has unignored player %s^1!\"", targetclient->client->pers.netname ));
}

/*
===========
Kick player + optional <msg> (NOT PRINTED ATM DUE ERROR MESSAGE BOX...FIXME)
===========
*/
void cmd_kick(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
		if (count == 0){
			trap_SendServerCommand(ent-g_entities, va("print \"Client not on server^1!\n\""));		
		return;
		}else if (count > 1){
			trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^1!\n\"", ent->client->pers.cmd2));
		return;
		} 
		
		for (i = 0; i < count; i++){
			/*
			if (!isHigher(ent, &g_entities[nums[i]])) {
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7has has the same or a higher level than you^1!\"", g_entities[nums[i]].client->pers.netname));
			return;
			}
			*/

			trap_DropClient( nums[i], va( "^3kicked by ^3admin. ^7%s", ent->client->pers.cmd3) );
			trap_SendServerCommand(-1, va("chat \"console: Admin (%s^7) has kicked player %s^1! ^3%s\n\"", ent->client->pers.netname, g_entities[nums[i]].client->pers.netname,ent->client->pers.cmd3 ));
		}
return;
}

/*
===========
Kick player based upon clientnumber + optional <msg> (NOT PRINTED ATM DUE ERROR MESSAGE BOX...FIXME)
===========
*/
void cmd_clientkick( gentity_t *ent) {
	int	player_id;
	gentity_t	*target_ent;
	gclient_t	*client;
	gentity_t	*targetclient;	

	player_id = ClientNumberFromString2( ent, ent->client->pers.cmd2 );
	if ( player_id == -1 ) { 
		return;
	}

	targetclient = g_entities + player_id; 
	target_ent = g_entities + player_id;
	client = target_ent->client;

	//kick the client
	trap_DropClient( player_id, va( "^3kicked by ^3admin. ^7%s", ent->client->pers.cmd3));
	trap_SendServerCommand(-1, va("chat \"console: Admin (%s^7) has kicked player %s^1 %s\n\"", ent->client->pers.netname, targetclient->client->pers.netname, ent->client->pers.cmd3 ));

return;
}

/*
===========
Slap player
===========
*/
void cmd_slap(gentity_t *ent)
{
	int clientid;
	int damagetodo;

	clientid = atoi(ent->client->pers.cmd2);
	damagetodo = 20; // do 20 damage

	if ((clientid < 0) || (clientid >= MAX_CLIENTS) )
	{
		trap_SendServerCommand(ent-g_entities, va("print \"Invalid client number^1!\n\""));
		return;
	}

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED) || g_entities[clientid].r.svFlags & SVF_BOT)
	{
		trap_SendServerCommand(ent-g_entities, va("print \"Invalid client number^1!\n\""));
		return;
	}
	
	ent = &g_entities[clientid];

	if (ent->client->ps.stats[STAT_HEALTH] <= 20) {
		G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_SLAP);				
		trap_SendServerCommand(-1, va("chat \"console: %s ^7was slapped to death by Admin^1!\n\"", ent->client->pers.netname));
		player_die( ent, ent, ent, 100000, MOD_SLAP );
	return;
	} else {
		G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_SLAP);
		trap_SendServerCommand(-1, va("chat \"console: %s ^7was slapped by Admin^1!\n\"", ent->client->pers.netname));
		// it's broadcasted globaly but only to near by players	
		G_AddEvent(ent, EV_GENERAL_SOUND, G_SoundIndex("sound/multiplayer/vo_revive.wav")); // L0 - TODO: Add sound in pack...
	return;
	}		
}

/*
===========
Kill player
===========
*/
void cmd_kill(gentity_t *ent)
{
int clientid;
	int damagetodo;

	clientid = atoi(ent->client->pers.cmd2);
	damagetodo = 250; // Kill the player on spot


	if ((clientid < 0) || (clientid >= MAX_CLIENTS))
	{
		trap_SendServerCommand(ent-g_entities, va("print \"Invalid client number^1!\n\""));
		return;
	}

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED) || g_entities[clientid].r.svFlags & SVF_BOT)
	{
		trap_SendServerCommand(ent-g_entities, va("print \"Invalid client number^1!\n\""));
		return;
	}

	if (!g_entities[clientid].client->ps.stats[STAT_HEALTH] > 0) {
		trap_SendServerCommand(ent-g_entities, va("print \"Player is already dead^1!\n\""));
	return;
	}
	
	ent = &g_entities[clientid];
	
	G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_ADMKILL);				
	trap_SendServerCommand(-1, va("chat \"console: %s ^7was killed by Admin^1!\n\"", ent->client->pers.netname));
	player_die( ent, ent, ent, 100000, MOD_ADMKILL );
return;	
}

/*
===========
Lock or Unlock game
===========
*/
void cmd_gamelocked(gentity_t *ent, qboolean unlock) {

	// Deals with unlocking
	if (unlock) {
		if (!g_gamelocked.integer) {
			trap_SendServerCommand(ent-g_entities, va("print \"Joining is already enabled^1!\n\""));				
		} else {
			trap_SendServerCommand(ent-g_entities, va("chat \"console: Admin has enabled joining^2!\n\""));
			trap_Cvar_Set( "g_gamelocked", "0" );
		}
	return;	
	// Deals with locking
	} else {
		if (g_gamelocked.integer) {
			trap_SendServerCommand(ent-g_entities, va("print \"Joining is already disabled^1!\n\""));			
		} else {
			trap_SendServerCommand(ent-g_entities, va("chat \"console: Admin has disabled joining^1!\n\""));
			trap_Cvar_Set( "g_gamelocked", "1" );
		}
	return;
	}
}

/*
===========
Force user to spectators
===========
*/
void cmd_specs(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
		
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
		if (count == 0){
			trap_SendServerCommand(ent-g_entities, va("print \"Client not on server^1!\n\""));
		return;
	}else if (count > 1){
		trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^11\n\"", ent->client->pers.cmd2));
	return;
	}		
		for (i = 0; i < count; i++){			

			if (g_entities[nums[i]].client->sess.sessionTeam == TEAM_SPECTATOR){
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7is already a spectator^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
			}  else
				SetTeam( &g_entities[nums[i]], "spectator" );
				trap_SendServerCommand(-1, va("chat \"console: Admin has forced player %s ^7to spectators^1!\n\"", g_entities[nums[i]].client->pers.netname));
			}		
return;
}

/*
===========
Force user to game
===========
*/
void cmd_force(gentity_t *ent) {
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
		
	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
		if (count == 0){
			trap_SendServerCommand(ent-g_entities, va("print \"Client not on server^1!\n\""));
		return;
	}else if (count > 1){
		trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^11\n\"", ent->client->pers.cmd2));
	return;
	}		
		for (i = 0; i < count; i++){			

			if (g_entities[nums[i]].client->sess.sessionTeam == TEAM_RED){
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7is already playing^1!\n\"", g_entities[nums[i]].client->pers.netname));
			return;
			}  else
				SetTeam( &g_entities[nums[i]], "red" );
				trap_SendServerCommand(-1, va("chat \"console: Admin has forced player %s ^7into game^1!\n\"", g_entities[nums[i]].client->pers.netname));
			}		
return;
}

/*
===========
Getstatus

Prints IP's and later on GUIDs if we'll decide to go that way..
===========
*/
extern int trap_RealTime ( qtime_t * qtime );
const char *cMonths[12] = {
"Jan", "Feb", "Mar", "Apr", "May", "Jun",
"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void cmd_getstatus(gentity_t *ent) {
	int	j;
	// uptime
	int secs = level.time / 1000;
	int mins = (secs / 60) % 60;
	int hours = (secs / 3600) % 24;
	int days = (secs / (3600 * 24));
	qtime_t ct;
	trap_RealTime(&ct);	

	trap_SendServerCommand( ent-g_entities, va("print \"\n^7Server: %s    ^7%02d:%02d:%02d ^2(^7%02d %s %d^2)\n\"", sv_hostname.string, ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday, cMonths[ct.tm_mon], 1900+ct.tm_year ));
	trap_SendServerCommand( ent-g_entities, va("print \"^2------------------------------------------------------------\n\""));	
	trap_SendServerCommand( ent-g_entities, va("print \"^7Slot : Team : Name       : ^2IP              ^7: Status \n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"^2------------------------------------------------------------\n\""));

	for ( j = 0; j <= (MAX_CLIENTS-1); j++) {

		if ( g_entities[j].client ) {

			// Don't count bots...
			if (g_entities[j].r.svFlags & SVF_BOT) {
				continue;
			} // end
			
			// player is connecting
			if ( g_entities[j].client->pers.connected == CON_CONNECTING ) {
				trap_SendServerCommand( ent-g_entities, va("print \"%3d  : >><< : %-10s : ^2>>Connecting<<  ^7:\n\"", j, g_entities[j].client->pers.netname ));
			}
			
			// player is connected && not a AI (bot)
			if ( g_entities[j].client->pers.connected == CON_CONNECTED ) { 	
			
				char *team, *slot, *ip, *tag, *sortTag, *extra;
				char n1[MAX_NETNAME]; 
				char n2[MAX_NETNAME];

				DecolorString(g_entities[j].client->pers.netname, n1);
				SanitizeString(n1, n2); 
				Q_CleanStr(n2);	
				n2[10] = 0;	
					
					// Sort it :C
					slot = va("%3d", j);
					team = (g_entities[j].client->sess.sessionTeam == TEAM_SPECTATOR) ? "^2SPEC^7" : "COOP";
					ip = ( ent->client->sess.admin == ADM_NONE ) ?
						va("%i.%i.*.*", g_entities[j].client->sess.ip[0], g_entities[j].client->sess.ip[1] ) :
						va("%i.%i.%i.%i", g_entities[j].client->sess.ip[0], g_entities[j].client->sess.ip[1], g_entities[j].client->sess.ip[2], g_entities[j].client->sess.ip[3]  );
					switch (g_entities[j].client->sess.admin) {
						case ADM_NONE:
							sortTag = "";
						break;
						case ADM_MEM:
							sortTag = (g_entities[j].client->sess.incognito) ? va("%s ^7(Hidden)", a1_tag.string) : va("%s", a1_tag.string);
						break;
						case ADM_MED:
							sortTag = (g_entities[j].client->sess.incognito) ? va("%s ^7(Hidden)", a2_tag.string) : va("%s", a2_tag.string);
						break;
						case ADM_FULL:
							sortTag = (g_entities[j].client->sess.incognito) ? va("%s ^7(Hidden)", a3_tag.string) : va("%s", a3_tag.string);
						break;
					}					
					// Sort Admin tags..
					extra = (g_entities[j].client->sess.admin == ADM_NONE && g_entities[j].client->sess.ignored) ? "^1Ignored" : "";
					if (ent->client->sess.admin == ADM_NONE) {
						tag = (g_entities[j].client->sess.admin != ADM_NONE && !g_entities[j].client->sess.incognito) ? sortTag : extra;
					} else if (ent->client->sess.admin != ADM_NONE) {
						tag = (g_entities[j].client->sess.admin == ADM_NONE && g_entities[j].client->sess.ignored) ? "^1Ignored" : sortTag;
					}		

						// Print it now
						trap_SendServerCommand( ent-g_entities, va("print \"%-4s : %s : %-10s : ^2%-15s ^7: %-15s \"", 
							slot, 
							team, 
							n2,
							ip,
							tag						
						));	
			
				trap_SendServerCommand( ent-g_entities, va("print \"\n\""));
			}
		}
	}	
	trap_SendServerCommand( ent-g_entities, va("print \"^2------------------------------------------------------------\n\""));
			
			trap_SendServerCommand( ent-g_entities, va("print \"^7Uptime: ^2%d ^7day%s ^2%d ^7hours ^2%d ^7minutes\n\"", days, (days != 1 ? "s" : ""), hours, mins));
			if (g_gamelocked.integer) 
				trap_SendServerCommand( ent-g_entities, va("print \"Game status: ^1Locked! \n\""));		
			trap_SendServerCommand( ent-g_entities, va("print \"\n\""));

			return;
}

/*
===========
List commands
===========
*/
void cmd_listCmds(gentity_t *ent) {
	char *cmds;
	
	if (!adm_help.integer) {
		trap_SendServerCommand( ent-g_entities, va("print \"Admin commands list is disabled on this server^1!\n\""));
	return;
	}

	cmds = "ignore unignore clientignore clientunignore kick clientkick slap kill specs coop";

	if (ent->client->sess.admin == ADM_MEM)
		trap_SendServerCommand( ent-g_entities, va("print \"\n^2Available commands are:^7\n%s\n^2Use ? for help with command. E.g. ?incognito.\n\"", a1_cmds.string));

	else if (ent->client->sess.admin == ADM_MED)
		trap_SendServerCommand( ent-g_entities, va("print \"\n^2Available commands are:^7\n%s\n^2Use ? for help with command. E.g. ?incognito.\n\"", a2_cmds.string));
	else if (ent->client->sess.admin == ADM_FULL && !a3_allowAll.integer)
		trap_SendServerCommand( ent-g_entities, va("print \"\n^2Available commands are:^7\n%s\n^2Use ? for help with command. E.g. ?incognito.\n\"", a3_cmds.string));
	else if (ent->client->sess.admin == ADM_FULL && a3_allowAll.integer)
		trap_SendServerCommand( ent-g_entities, va("print \"\n^2Available commands are:^7\n%s\n^2Additinal server commands:^7\n%s\n^2Use ? for help with command. E.g. ?incognito.\n\"", cmds, a3_cmds.string));


return;
}

/*********************************** INTERACTIONS ************************************/
/*
===========
Admin commands
===========
*/
qboolean do_cmds(gentity_t *ent) { 
	char alt[128];
	char cmd[128];

	admCmds(ent->client->pers.cmd1, alt, cmd, qfalse);

	if (!strcmp(cmd,"incognito"))			{ if (canUse(ent)) cmd_incognito(ent); else cantUse(ent);	return qtrue;}
	else if (!strcmp(cmd,"list_cmds"))		{ cmd_listCmds(ent);	return qtrue;}
	else if (!strcmp(cmd,"ignore"))			{ if (canUse(ent)) cmd_ignore(ent);	else cantUse(ent); return qtrue;}
	else if (!strcmp(cmd,"unignore"))		{ if (canUse(ent)) cmd_unignore(ent); else cantUse(ent); return qtrue;}
	else if (!strcmp(cmd,"clientignore"))	{ if (canUse(ent)) cmd_clientIgnore(ent); else cantUse(ent); return qtrue;}
	else if (!strcmp(cmd,"clientunignore"))	{ if (canUse(ent)) cmd_clientUnignore(ent); else cantUse(ent); return qtrue;}
	else if (!strcmp(cmd,"kick"))			{ if (canUse(ent)) cmd_kick(ent); else cantUse(ent); return qtrue;}
	else if (!strcmp(cmd,"clientkick"))		{ if (canUse(ent)) cmd_clientkick(ent);	else cantUse(ent); return qtrue;}
	else if (!strcmp(cmd,"slap"))			{ if (canUse(ent)) cmd_slap(ent); else cantUse(ent); return qtrue;} 
	else if (!strcmp(cmd,"kill"))			{ if (canUse(ent)) cmd_kill(ent); else cantUse(ent); return qtrue;} 
//	else if (!strcmp(cmd,"lock"))			{ if (canUse(ent)) cmd_gamelocked(ent, qfalse); else cantUse(ent); return qtrue;}  // Leaving this out for the moment as client would need to start as SPEC upon connecting and dunno if this is general idea in the project?
//	else if (!strcmp(cmd,"unlock"))			{ if (canUse(ent)) cmd_gamelocked(ent, qtrue); else cantUse(ent); return qtrue;}   //  - || -
	else if (!strcmp(cmd,"specs"))			{ if (canUse(ent)) cmd_specs(ent); else cantUse(ent); return qtrue;} // NOTE: Set team will need to be patched with "forced" if and when lock and unlock will be set back..
	else if (!strcmp(cmd,"coop"))			{ if (canUse(ent)) cmd_force(ent); else cantUse(ent); return qtrue;} // - || -

else { trap_SendServerCommand(ent-g_entities, va("print \"Command ^1%s ^7was not found^1!\n\"", cmd)); return qfalse; }
}

/*
===========
Admin help
===========
*/
qboolean do_help(gentity_t *ent) {	
	char alt[128];
	char cmd[128];
	char *help;

	admCmds(ent->client->pers.cmd1, alt, cmd, qtrue);

	if (!strcmp(cmd,"login"))				help = "Logs you in as Administrator.";
	else if (!strcmp(cmd,"@login"))			help = "Silently logs you in as Administrator.";
	else if (!strcmp(cmd,"logout"))			help = "Removes you from Administrator status.";
	else if (!strcmp(cmd,"incognito"))		help = "Toggles your Admin status from hidden to visible or other way around.";
	else if (!strcmp(cmd,"getstatus"))		help = "Shows basic info of all connected players.";
	else if (!strcmp(cmd,"list_cmds"))		help = "Shows all available commands for your level.";
	else if (!strcmp(cmd,"ignore"))			help = "Takes player's ability to chat, use vsay or callvotes. Uses unique part of name!";
	else if (!strcmp(cmd,"unignore"))		help = "Restores player's ability to chat, use vsay or call votes. Uses unique part of name!";
	else if (!strcmp(cmd,"clientignore"))	help = "Takes player's ability to chat, callvotes or use vsay . Uses client slot!";
	else if (!strcmp(cmd,"clientunignore"))	help = "Restores player's ability to chat, callvotes or use vsay. Uses client slot!";
	else if (!strcmp(cmd,"kick"))			help = "Kicks player from server. Uses unique part of name! Optionally you can add message.";
	else if (!strcmp(cmd,"clientkick"))		help = "Kicks player from server. Uses client slot number! Optionally you can add message.";
	else if (!strcmp(cmd,"slap"))			help = "Slaps player and takes 20hp. Uses client slot number!";
	else if (!strcmp(cmd,"kill"))			help = "Kills player on spot. Uses client slot number!";
//	else if (!strcmp(cmd,"lock"))			help = "Locks the game so users can't join and can only spectate - Admin can still force users to team by using !force command.";
//	else if (!strcmp(cmd,"unlock"))			help = "Unlocks the game so everyone can play.";
	else if (!strcmp(cmd,"specs"))			help = "Moves player to spectators. Uses unique part of name!";
	else if (!strcmp(cmd,"coop"))			help = "Moves player to game. Uses unique part of name!";

	else return qfalse;

	trap_SendServerCommand( ent-g_entities, va("print \"^2Help: ^7%s\n\"", help));

return qtrue;
}

/*
===========
Commands
===========
*/
qboolean cmds_admin(char cmd[MAX_TOKEN_CHARS], gentity_t *ent) {	

	// We're dealing with command
	if ( Q_stricmp( cmd, "!" ) == 0 ) {
		return do_cmds(ent);
	} 
	// We're dealing with help
	else if ( Q_stricmp( cmd, "?" ) == 0 ) { 
		return do_help(ent);
	}

return qfalse;
}


#endif
