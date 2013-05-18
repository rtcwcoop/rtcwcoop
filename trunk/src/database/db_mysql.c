/*
===========================================================================

OpenWolf GPL Source Code
Copyright (C) 2009 SlackerLinux85 <SlackerLinux85@gmail.com>
Copyright (C) 2011 Dusan Jocic <dusanjocic@msn.com>

OpenWolf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

OpenWolf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/


#include "database.h"

#ifdef SQL

// Dushan
#if defined(WIN32) || defined(WIN64)
#    include <winsock.h>
#    include <mysql.h>
#else
#ifdef USE_LOCAL_HEADERS
#    include <mysql.h>
#else
#    include <mysql/mysql.h>
#endif
#endif

extern cvar_t *db_enable;

#define MAX_QUERYS_RESULTS 100

MYSQL *connection;

typedef struct {
	MYSQL_RES *results;
	MYSQL_ROW row;
} db_MySQL_querylist_t;

db_MySQL_querylist_t querylist[ MAX_QUERYS_RESULTS ];

int OW_MySQL_GetFreeQueryID( void ) {
	int i;
 
	for( i = 0; i < MAX_QUERYS_RESULTS; i++ ) {
		if( !querylist[ i ].results ) {
			return i;
		}
	}
	return -1;
}

qboolean OW_MySQL_Init( dbinterface_t *dbi ) {
	dbi->DBConnectMaster = OW_MySQL_ConnectMaster;	
	dbi->DBStatus= OW_MySQL_DBStatus;
	dbi->DBDisconnect = OW_MySQL_Disconnect;

	dbi->DBCreateTable = OW_MySQL_CreateTable;

	dbi->RunQuery = OW_MySQL_RunQuery;
	dbi->FinishQuery = OW_MySQL_FinishQuery;

	dbi->NextRow = OW_MySQL_NextRow;
	dbi->RowCount = OW_MySQL_RowCount;

	dbi->GetFieldByID = OW_MySQL_GetFieldByID;
	dbi->GetFieldByName = OW_MySQL_GetFieldByName;
	dbi->GetFieldByID_int = OW_MySQL_GetFieldByID_int;
	dbi->GetFieldByName_int = OW_MySQL_GetFieldByName_int;
	dbi->FieldCount = OW_MySQL_FieldCount;

	dbi->CleanString = OW_MySQL_CleanString;


	return qtrue;
}

//
// MYSQL Connecting related functions
//

void OW_MySQL_ConnectMaster( void ) {
	//init mysql
	connection = mysql_init( connection );

	//attempt to connect to mysql
	if( !mysql_real_connect( 
		connection, 
		db_addressMaster->string, 
		db_usernameMaster->string, 
		db_passwordMaster->string, 
		db_databaseMaster->string, 
		0,
		NULL, 
		0) ) {
			Com_Printf( "WARNING: MySQL loading failed: %s.\n", mysql_error( connection ) );
		return;
	}

	// Dushan - moved creating databases after connecting to MySQL
	OW_MySQL_CreateTable();

	Com_Printf( "MySQL Master Server Loaded.\n" );
}


void OW_MySQL_DBStatus( void ) {
}
void OW_MySQL_Disconnect( void ) {
	int i;
	
	//clear all results
	for( i = 0; i < MAX_QUERYS_RESULTS; i++ ) {
		if( querylist[ i ].results ) {
		mysql_free_result( querylist[ i ].results );
		querylist[ i ].results = NULL;
		Com_DPrintf( "DEV: MySQL Freeing query ID %i.\n", i );
		}
	}

	if( connection ) {
		mysql_close( connection );
		connection = NULL;
		Com_Printf( "MySQL Server Unloaded.\n" );
	}
}

//
// MYSQL Query related functions
//

int OW_MySQL_RunQuery( const char *query ) {
	int queryid;

	queryid = OW_MySQL_GetFreeQueryID();

	if( queryid >= 0 ) {
		// Dushan - RunQuery should work on both MySQL connections
		if( connection ) {
			if( mysql_query( connection, query ) ) {
				// Dushan - print info if query fail on MySQL connection (master server)
				Com_Printf( "WARNING: MySQL Query failed: %s\n", mysql_error( connection ) );
				return -1;
			}			

			// Dushan - query both MySQL connections
			querylist[ queryid ].results = mysql_store_result( connection );		
			Com_DPrintf( "DEV: MySQL using query ID %i.\n", queryid );
			return queryid;
		}
	} else {
		Com_DPrintf( "DEV: MySQL Failed to obtain a query ID.\n" );
		return -1;
	}
	return -1;
}

void OW_MySQL_FinishQuery( int queryid ) {
	if( querylist[ queryid ].results ) {
		mysql_free_result( querylist[ queryid ].results );
		querylist[ queryid ].results = NULL;
		Com_DPrintf( "DEV: MySQL Freeing query ID %i.\n", queryid );
	}
}

//
// MYSQL ROW related functions
//

qboolean OW_MySQL_NextRow( int queryid ) {
	if( querylist[ queryid ].results ) {
		//get next row
		querylist[ queryid ].row = mysql_fetch_row( querylist[ queryid ].results );
		//if its not valid return false
		if( !querylist[ queryid ].row ) {
			return qfalse;
		}
		return qtrue;
	}
	return qfalse;
}

int OW_MySQL_RowCount( int queryid ) {
	if( querylist[ queryid ].results )
		return mysql_num_rows( querylist[ queryid ].results );
	return 0;
}

//
// MYSQL Field related functions
//

void OW_MySQL_GetFieldByID( int queryid, int fieldid, char *buffer, int len ) {
	if( querylist[ queryid ].row[ fieldid ] ) {
		Q_strncpyz( buffer, querylist[ queryid ].row[ fieldid ], len );
	}
	else {
		Com_Printf( "WARNING: MySQL field %i doesnt exist.\n", fieldid );
	}
}

void OW_MySQL_GetFieldByName( int queryid, const char *name, char *buffer, int len ) {
	MYSQL_FIELD *fields;
	int num_fields;
	int i;

	if( querylist[ queryid ].results ) {
		num_fields = mysql_num_fields( querylist[ queryid ].results );
		fields = mysql_fetch_fields( querylist[ queryid ].results );

		//loop through till we find the field
		for( i = 0; i < num_fields; i++ ) {
			if( !strcmp( fields[ i ].name, name ) ) {
				//found check for valid data
				if( querylist[ queryid ].row[ i ] ) {
					Q_strncpyz( buffer, querylist[ queryid ].row[ i ], len );
					return;
				}
			}
		}
		Com_Printf( "WARNING: MySQL field %s doesnt exist.\n", name );
	}
}

int OW_MySQL_GetFieldByID_int( int queryid, int fieldid ) {
	if( querylist[ queryid ].row[ fieldid ] ) {
		return atoi( querylist[ queryid ].row[ fieldid ] );
	}
	else {
		Com_Printf( "WARNING: MySQL field %i doesnt exist.\n", fieldid );
		return 0;
	}
	return 0;
}

int OW_MySQL_GetFieldByName_int( int queryid, const char *name ) {
	MYSQL_FIELD *fields;
	int num_fields;
	int i;

	if( querylist[ queryid ].results ) {
		num_fields = mysql_num_fields( querylist[ queryid ].results );
		fields = mysql_fetch_fields( querylist[ queryid ].results );

		//loop through till we find the field
		for( i = 0; i < num_fields; i++ ) {
			if( !strcmp( fields[ i ].name, name ) ) {
				//found check for valid data
				if( querylist[ queryid ].row[ i ] ) { 
					return atoi( querylist[ queryid ].row[ i ] );
				}
			}
		}
		Com_Printf( "WARNING: MySQL field %s doesnt exist.\n", name );
	}
	return 0;
}

int OW_MySQL_FieldCount( int queryid ) {
	if( querylist[ queryid ].results ) {
		return mysql_num_fields( querylist[ queryid ].results );
	}
	return 0;
}

//
// MYSQL Misc functions
//

// L0 - mysql_real_escape is wrong on so many levels...
// -> I need to look into BINDS support or smth..
void OW_MySQL_CleanString( const char *in, char *out, int len ) {
	if( connection && len > 0 && in[0] ) {
		mysql_real_escape_string( connection, out, in, len );
	}	
}

//
// MYSQL Create database
//

void OW_MySQL_CreateTable( void ) {	

	// Dushan : Write and created only info if database is connected
	if( db_enable->integer == 1 ) {

		// If it is connected to database
		if( connection ) {		

			// L0 - Reworked whole database, but has to be polished.

			//
			// Ban table structure
			//
			if ( mysql_query(connection, "CREATE TABLE IF NOT EXISTS `users_banned` (	\
										  `ban_guid` varchar(32) NOT NULL,				\
										  `ban_user` varchar(32) NOT NULL DEFAULT '',	\
										  `ban_expires` datetime NOT NULL,				\
										  `ban_reason` varchar(255) NOT NULL DEFAULT '',\
										  `ban_issued` varchar(32) NOT NULL DEFAULT '',	\
										  `ban_perm` tinyint(1) NOT NULL DEFAULT '0',	\
										  `ban_count` int(11) NOT NULL DEFAULT '0',		\
										  `ban_tempcount` int(11) NOT NULL DEFAULT '0',	\
										  UNIQUE KEY `ban_guid` (`ban_guid`)			\
										) ENGINE=InnoDB DEFAULT CHARSET=utf8;")) {		\
										Com_Printf( "WARNING: MySQL Query failed (USERS_BANNED): %s\n", mysql_error( connection ) );
										return;
			}

			//
			// Mute table structure		
			// 
			if ( mysql_query(connection, "CREATE TABLE IF NOT EXISTS `users_ignored` (	\
										  `ign_guid` varchar(32) NOT NULL,				\
										  `ign_user` varchar(32) NOT NULL DEFAULT '',	\
										  `ign_expires` datetime NOT NULL,				\
										  `ign_reason` varchar(255) NOT NULL DEFAULT '',\
										  `ign_count` int(11) NOT NULL DEFAULT '0',		\
										  UNIQUE KEY `ign_guid` (`ign_guid`)			\
										) ENGINE=InnoDB DEFAULT CHARSET=utf8;")) {
										Com_Printf( "WARNING: MySQL Query failed (USERS_IGNORED): %s\n", mysql_error( connection ) );
										return;
			}	

			//
			// Admin's table
			//
			if ( mysql_query(connection, "CREATE TABLE IF NOT EXISTS `users_admins` (	\
										  `usr_guid` varchar(32) NOT NULL,				\
										  `usr_name` varchar(32) NOT NULL DEFAULT '',	\
										  `usr_level` tinyint(1) NOT NULL,				\
										  `usr_notes` text NOT NULL,					\
										  `entry_added` datetime NOT NULL,				\
										  `admin_expires` datetime NOT NULL,			\
										  UNIQUE KEY `usr_guid` (`usr_guid`),			\
										  KEY `usr_name` (`usr_name`)					\
										) ENGINE=InnoDB DEFAULT CHARSET=utf8;")) {
										Com_Printf( "WARNING: MySQL Query failed (USERS_ADMINS): %s\n", mysql_error( connection ) );
										return;
			}

			//
			// IP Bans
			//
			if ( mysql_query(connection, "CREATE TABLE IF NOT EXISTS `users_ipbans` (								\
										  `usr_ip` varchar(39) CHARACTER SET utf8 NOT NULL,							\
										  `usr_name` varchar(32) CHARACTER SET utf8 NOT NULL DEFAULT '',			\
										  `usr_banned` datetime NOT NULL,											\
										  `bypass_password` varchar(20) CHARACTER SET utf8 NOT NULL DEFAULT '',		\
										  `users_bypassing` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT '',	\
										  KEY `usr_ip` (`usr_ip`)													\
										) ENGINE=InnoDB DEFAULT CHARSET=utf8;")) {
										Com_Printf( "WARNING: MySQL Query failed (USERS_IPBANS): %s\n", mysql_error( connection ) );
										return;
			}

			//
			// IP TempBans
			//
			if ( mysql_query(connection, "CREATE TABLE IF NOT EXISTS `users_tempbans` (		\
										  `temp_ip` varchar(39) NOT NULL,					\
										  `temp_name` varchar(32) NOT NULL,					\
										  `temp_expires` datetime NOT NULL,					\
										  `temp_reason` varchar(255) NOT NULL DEFAULT '',	\
										  UNIQUE KEY `temp_ip` (`temp_ip`)					\
										) ENGINE=InnoDB DEFAULT CHARSET=utf8;")) {
										Com_Printf( "WARNING: MySQL Query failed (USERS_TEMPBANS): %s\n", mysql_error( connection ) );
										return;
			}
		}
		Com_Printf( "-----------------------------------\n" );
	}
}

#endif // SQL
