/**
 *
 * Deals with files (txt)
 *
 * Author: Nate L0
 * Created: 19 Mar / 2013
 * Updated:
 *
 * Most of the stuff is from old S4NDMOD source (with few adjustments)
 * so credits go to Martin and Travis ( AG3NT & S4NDM4NN ).
 *
 */
#ifdef _ADMINS

#include "g_local.h"

char *TempBannedMessage;

/*
===========
Tempbanned

Port from old S4NDMoD (time saver)

AvPMOD one is more confusing and has potential
to cause problems in current situation so I'm
re-using old S4NDMoD's one as I'm to lazy to write new one.

----
P.S. Yes..i'm to lazy to change this as well (S4NDMoD->wolfX->COOP port). :)
===========
*/
void TEMPBAN_CLIENT( gentity_t *ent, const int minsbanned ) {
	FILE    *tempbannedfile;
	time_t rawtime;
	time( &rawtime );

	tempbannedfile = fopen( "tempbanned.txt","a+" );
	fputs( va( "%i.%i.%i.%i %i\n",
			   ent->client->sess.ip[0], ent->client->sess.ip[1], ent->client->sess.ip[2], ent->client->sess.ip[3], (int)( ( rawtime - 1103760000 ) + ( minsbanned * 60 ) ) ), tempbannedfile );
	fclose( tempbannedfile );
}

void rewrite_tempbanned( void ) {
	FILE        *bannedfile;
	FILE        *tempfile;
	char tempbannedips[64];

	bannedfile = fopen( "tempbanned.txt","w+" );
	tempfile = fopen( "tempbannedtemp.txt","r+" );

	while ( fgets( tempbannedips,64,tempfile ) != NULL )
	{
		fputs( tempbannedips,bannedfile );
	}

	fclose( bannedfile );
	fclose( tempfile );
	remove( "tempbannedtemp.txt" );
}

void write_tempbannedtemp( void ) {
	FILE        *bannedfile;
	FILE        *tempfile;
	char tempbaninfo[64];
	char tempbannedip[39];
	int tempbannedsec;
	time_t rawtime;
	time( &rawtime );

	bannedfile = fopen( "tempbanned.txt","r+" );

	if ( bannedfile ) {
		tempfile = fopen( "tempbannedtemp.txt","w+" );

		if ( tempfile ) {
			while ( fgets( tempbaninfo,64,bannedfile ) != NULL )
			{
				sscanf( tempbaninfo,"%s %i",tempbannedip, &tempbannedsec );
				if ( ( tempbannedsec - ( rawtime - 1103760000 ) ) > 0 ) {
					fputs( tempbaninfo,tempfile );
				}
			}

			fclose( tempfile );
		}

		fclose( bannedfile );
		rewrite_tempbanned();
	}
}

void clean_tempbans( void ) {
	write_tempbannedtemp();
}

qboolean TempBanned( char * Clientip ) {

	FILE        *tempbannedfile;
	char tempbannedip[39];
	int tempbannedsec;
	int hours;
	int tens;
	int mins;
	int seconds;
	char tempbannedinfo[1024];
	char        *sortTime;
	char        *theTime;
	time_t rawtime;
	time( &rawtime );

	tempbannedfile = fopen( "tempbanned.txt","r" ); //Open file

	if ( tempbannedfile ) { //if there is a tembbannedfile
		while ( fgets( tempbannedinfo,1024,tempbannedfile ) != NULL ) {
			sscanf( tempbannedinfo,"%s %i", tempbannedip, &tempbannedsec );

			if  ( !Q_stricmp( Clientip, tempbannedip ) && ( tempbannedsec - ( rawtime - 1103760000 ) ) > 0 ) {
				fclose( tempbannedfile );
				seconds = ( tempbannedsec - ( rawtime - 1103760000 ) );
				mins = seconds / 60;
				hours = mins / 60;
				seconds -= mins * 60;
				tens = seconds / 10;
				seconds -= tens * 10;
				mins -= hours * 60;
				sortTime = ( hours > 0 ) ? ( ( mins > 9 ) ? va( "%i:", hours ) : va( "%i:0", hours )  ) : "";
				theTime = va( "%s%i:%i%i",sortTime, mins,tens,seconds );
				TempBannedMessage = va( "You Are ^nTemporarily Banned ^7for ^n%s", theTime );
				return qtrue;
			}
		}

		fclose( tempbannedfile );
	}

	return qfalse;
}

/*
===========
Banned (file check version)

Took this from old S4NDMoD source as it simply works (time saver).
===========
*/
void BreakIP( const char *IP, char *charip1, char* charip2, char* charip3, char* charip4 ) {
	int i = 0, a = 0, b = 0, c = 0, d = 0;
	int foundperiod = 0;

	while ( IP[i] != 0 ) {
		if ( !foundperiod ) { //havent found any periods
			if ( IP[i] == '.' ) {
				foundperiod = 1;
				charip1[a] = 0;
			} else {
				charip1[a] = IP[i];
			}
			i++;
			a++;
		} else if ( foundperiod == 1 )    { // We have found 1 period
			if ( IP[i] == '.' ) {
				foundperiod = 2;
				charip2[b] = 0;
			} else {
				charip2[b] = IP[i];
			}
			i++;
			b++;
		} else if ( foundperiod == 2 )    { // We have found 2 periods
			if ( IP[i] == '.' ) {
				foundperiod = 3;
				charip3[c] = 0;
			} else {
				charip3[c] = IP[i];
			}
			i++;
			c++;
		} else if ( foundperiod == 3 )    { // We have found 3 periods
			if ( IP[i] == '.' ) {
				foundperiod = 4;
				charip4[d] = 0;
			} else {
				charip4[d] = IP[i];
			}
			i++;
			d++;
		}
	}
}

/*
===========
Password bypass

Basically we re-use existing stuff..
===========
*/
qboolean bypassing( char *password ) {

	// check for password first
	if ( strlen( g_password.string ) ) {
		char *token, *text;
		text = g_password.string;

		while ( 1 )
		{
			token = COM_Parse( &text );
			if ( !token[0] ) {
				break;
			}

			if ( !Q_stricmp( password, token ) ) {
				return qtrue;
			} else {
				return qfalse;
			}
		}
	}
	return qfalse;
}

/*
===========
Banned (file check version)

Based on s4ndmod
===========
*/
qboolean Banned( char * Clientip, char * password ) {
	FILE * bannedfile;
	char ip1[10];
	char ip2[10];
	char ip3[10];
	char ip4[10];
	char banip1[10];
	char banip2[10];
	char banip3[10];
	char banip4[10];
	char bannedips[1024];
	Q_strncpyz( ip1, "", sizeof( ip1 ) );
	Q_strncpyz( ip2, "", sizeof( ip2 ) );
	Q_strncpyz( ip3, "", sizeof( ip3 ) );
	Q_strncpyz( ip4, "", sizeof( ip4 ) );

	BreakIP( va( "%s\n",Clientip ),ip1,ip2,ip3,ip4 );
	bannedfile = fopen( "banned.txt","r" );

	if ( bannedfile ) {
		while ( fgets( bannedips,1024,bannedfile ) != NULL ) {
			Q_strncpyz( banip1, "", sizeof( banip1 ) );
			Q_strncpyz( banip2, "", sizeof( banip2 ) );
			Q_strncpyz( banip3, "", sizeof( banip3 ) );
			Q_strncpyz( banip4, "", sizeof( banip4 ) );

			BreakIP( bannedips, banip1, banip2, banip3, banip4 );

			if ( !Q_stricmp( banip1, ip1 ) || !Q_stricmp( banip1, "*" ) ) {
				if ( !Q_stricmp( banip2, ip2 ) || !Q_stricmp( banip2, "*" ) ) {
					if ( !Q_stricmp( banip3, ip3 ) || !Q_stricmp( banip3, "*" ) ) {
						if ( !Q_stricmp( banip4, ip4 ) || !Q_stricmp( banip4, "*\n" ) ) {

							fclose( bannedfile );
							if ( bypassing( password ) ) {
								return qfalse;
							} else {
								return qtrue;
							}
						}
					}
				}
			}
		}
		fclose( bannedfile );
	}
	return qfalse;
}

/*
===========
Check banned

Checks if user is banned or tempbanned..
===========
*/
int checkBanned( char *data, char * password ) {
	if ( Banned( data, password ) ) {
		return 1;
	}
	if ( TempBanned( data ) ) {
		return 2;
	}
	return 0;
}

#endif // _ADMINS
