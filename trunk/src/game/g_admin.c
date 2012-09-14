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
Deals with ! (if I recall right this bit is from S4NDMoD or some ET source *???*)
===========
*/
void admCmds(const char *strCMD1, char *strCMD2, char *strCMD3){
	
	int i = 0, j=0;
	int foundcolon=0;

	while(strCMD1[i] != 0)
	{
		if(!foundcolon)
		{
			if(strCMD1[i] == '!') {
				foundcolon = 1;
				strCMD2[i]=0;
			}
			else
				strCMD2[i]=strCMD1[i];
			i++;
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
	}else if (count > 1){
		trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^1!\n\"", ent->client->pers.cmd2));
	return;
	}
	
		for (i = 0; i < count; i++){

			if (!isHigher(ent, &g_entities[nums[i]])) {
				trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7has has the same or a higher level than you^1!\"", g_entities[nums[i]].client->pers.netname));
			return;
			}

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

	if (!isHigher(ent, targetclient)) {
		trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7has has the same or a higher level than you^1!\"", targetclient->client->pers.netname ));
	return;	
	} else if (targetclient->client->sess.ignored ) {
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

	if (!isHigher(ent, targetclient)) {
		trap_SendServerCommand(ent-g_entities, va("print \"Player %s ^7has has the same or a higher level than you^1!\"", targetclient->client->pers.netname ));
	return;	
	} else if (targetclient->client->sess.ignored == 0 ) {
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
	int count = 0;
	int i;
	int nums[MAX_CLIENTS];
	int damagetodo;

	damagetodo = 20; // do 20 damage

	count = ClientNumberFromNameMatch(ent->client->pers.cmd2, nums);
		if (count == 0){
			trap_SendServerCommand(ent-g_entities, va("print \"Client not on server^1!\n\""));		
		return;
		}else if (count > 1){
			trap_SendServerCommand(ent-g_entities, va("print \"To many people with %s in their name^1!\n\"", ent->client->pers.cmd2));
		return;
		} 
		
		for (i = 0; i < count; i++){			

			// If we slap a player that has 20hp or less we're gonna kil him so print MOD..
			if (ent->client->ps.stats[STAT_HEALTH] <= 20) {
				G_Damage(ent-nums[i], NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_SLAP);				
				trap_SendServerCommand(-1, va("chat \"console: %s ^7was slapped to death by Admin^1!\n\"", g_entities[nums[i]].client->pers.netname));
				player_die( ent-nums[i], ent, ent, 100000, MOD_SLAP );
			} else {
				G_Damage(ent-nums[i], NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_SLAP);
				trap_SendServerCommand(-1, va("chat \"console: %s ^7was slapped by Admin^1!\n\"", g_entities[nums[i]].client->pers.netname));
				// it's broadcasted globaly but only to near by players	
				G_AddEvent(ent, EV_GENERAL_SOUND, G_SoundIndex("sound/multiplayer/vo_revive.wav"));  // L0 - TODO: Add sound in pack...
			}	
		} 
return;
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
	damagetodo = 250; // do 20 damage


	if ((clientid < 0) || (clientid >= MAX_CLIENTS))
	{
		trap_SendServerCommand(ent-g_entities, va("print \"Invalid client number^1!\n\""));
		return;
	}

	if ((!g_entities[clientid].client) || (level.clients[clientid].pers.connected != CON_CONNECTED))
	{
		trap_SendServerCommand(ent-g_entities, va("print \"No one under this client number^1!\n\""));
		return;
	}
	
	ent = &g_entities[clientid];

	if (ent->client->ps.stats[STAT_HEALTH] > 0) {
			G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_ADMKILL);				
			trap_SendServerCommand(-1, va("chat \"console: %s ^7was killed by Admin^1!\n\"", ent->client->pers.netname));
			player_die( ent, ent, ent, 100000, MOD_ADMKILL );
	} else { 
		G_Damage(ent, NULL, NULL, NULL, NULL, damagetodo, DAMAGE_NO_PROTECTION, MOD_ADMKILL);
		trap_SendServerCommand(-1, va("chat \"console: %s ^7was slapped by Admin^1!\n\"", ent->client->pers.netname));
		// it's broadcasted globaly but only to near by players
		G_AddEvent(ent, EV_GENERAL_SOUND, G_SoundIndex("sound/multiplayer/vo_revive.wav")); // Add sound in soundpack : TODO
	}
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

	admCmds(ent->client->pers.cmd1, alt, cmd);

	if (!strcmp(cmd,"incognito"))			{ cmd_incognito(ent);	return qtrue;}
	else if (!strcmp(cmd,"ignore"))			{ cmd_ignore(ent);	return qtrue;}
	else if (!strcmp(cmd,"unignore"))		{ cmd_unignore(ent);	return qtrue;}
	else if (!strcmp(cmd,"clientignore"))	{ cmd_clientIgnore(ent);	return qtrue;}
	else if (!strcmp(cmd,"clientunignore"))	{ cmd_clientUnignore(ent);	return qtrue;}
	else if (!strcmp(cmd,"kick"))			{ cmd_kick(ent);	return qtrue;}
	else if (!strcmp(cmd,"clientkick"))		{ cmd_clientkick(ent);	return qtrue;}
//	else if (!strcmp(cmd,"slap"))			{ cmd_slap(ent);	return qtrue;} // FIXME
//	else if (!strcmp(cmd,"kill"))			{ cmd_kill(ent);	return qtrue;} // FIXME

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

	admCmds(ent->client->pers.cmd1, alt, cmd);

	if (!strcmp(cmd,"login"))			trap_SendServerCommand( ent-g_entities, va("print \"Help: Logs you in as Administrator.\n\""));
	else if (!strcmp(cmd,"@login"))		trap_SendServerCommand( ent-g_entities, va("print \"Help: Silently logs you in as Administrator.\n\""));
	else if (!strcmp(cmd,"logout"))		trap_SendServerCommand( ent-g_entities, va("print \"Help: Removes you from Administrator status.\n\""));
	else if (!strcmp(cmd,"incognito"))	trap_SendServerCommand( ent-g_entities, va("print \"Help: Toggles your Admin status from hidden to visible or other way around.\n\""));
	else return qfalse;

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
