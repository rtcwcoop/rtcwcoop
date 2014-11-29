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
* g_admins.h
*
* Functionality for server Administration in CO-OP mode.
*
* Author: Nate L0
* Date: 11.09/12
* Last edit: 07.11/12
****************************************************************************
*/
#ifdef _ADMINS

// Logs
#define ADMLOG "./COOP/adminLogins.log"
#define PASSLOG "./COOP/adminLoginAttempts.log"
#define ADMACT "./COOP/adminActions.log"
#define BYPASSLOG "./COOP/banBypass.log"

// Linked in g_cmds.c
void cmd_incognito( gentity_t *ent );
void cmd_do_logout( gentity_t *ent );
void cmd_do_login( gentity_t *ent, qboolean silent );
void cmd_getstatus( gentity_t *ent );

/* -> g_admin.c <- */
qboolean cmds_admin( char cmd[MAX_TOKEN_CHARS], gentity_t * ent );
void ParseAdmStr( const char *strInput, char *strCmd, char *strArgs );
void logEntry( char *filename, char *info );

/* -> g_cmds.c <- */
char *ConcatArgs( int start );

#endif // _ADMINS
