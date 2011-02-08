/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "../game/q_shared.h"
#include "../qcommon/qcommon.h"
#include "sys_local.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pwd.h>
#include <libgen.h>
#include <fcntl.h>

qboolean stdinIsATTY;

// Used to determine where to store user-specific files
static char homePath[ MAX_OSPATH ] = { 0 };

/*
==================
Sys_DefaultHomePath
==================
*/
char *Sys_DefaultHomePath(void)
{
	char *p;

	if( !*homePath )
	{
		if( ( p = getenv( "HOME" ) ) != NULL )
		{
			Q_strncpyz( homePath, p, sizeof( homePath ) );
#ifdef MACOS_X
			Q_strcat( homePath, sizeof( homePath ),
					"/Library/Application Support/Wolfenstein" );
#else
			Q_strcat( homePath, sizeof( homePath ), "/.wolf" );
#endif
		}
	}

	return homePath;
}

#ifndef MACOS_X
/*
================
Sys_TempPath
================
*/
const char *Sys_TempPath( void )
{
	const char *TMPDIR = getenv( "TMPDIR" );

	if( TMPDIR == NULL || TMPDIR[ 0 ] == '\0' )
		return "/tmp";
	else
		return TMPDIR;
}
#endif

/*
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038 */
unsigned long sys_timeBase = 0;
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
     0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
     (which would affect the wrap period) */
int curtime;
int Sys_Milliseconds (void)
{
	struct timeval tp;

	gettimeofday(&tp, NULL);

	if (!sys_timeBase)
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec/1000;
	}

	curtime = (tp.tv_sec - sys_timeBase)*1000 + tp.tv_usec/1000;

	return curtime;
}

#if !id386
/*
==================
fastftol
==================
*/
long fastftol( float f )
{
	return (long)f;
}
//fretn
#endif

/*
==================
Sys_SnapVector
==================
*/
void Sys_SnapVector( float *v )
{
	v[0] = rint(v[0]);
	v[1] = rint(v[1]);
	v[2] = rint(v[2]);
}
// fretn
//#endif

/*
==================
Sys_RandomBytes
==================
*/
qboolean Sys_RandomBytes( byte *string, int len )
{
	FILE *fp;

	fp = fopen( "/dev/urandom", "r" );
	if( !fp )
		return qfalse;

	if( !fread( string, sizeof( byte ), len, fp ) )
	{
		fclose( fp );
		return qfalse;
	}

	fclose( fp );
	return qtrue;
}

/*
==================
Sys_GetCurrentUser
==================
*/
char *Sys_GetCurrentUser( void )
{
	struct passwd *p;

	if ( (p = getpwuid( getuid() )) == NULL ) {
		return "player";
	}
	return p->pw_name;
}

/*
==================
Sys_GetClipboardData
==================
*/
char *Sys_GetClipboardData(void)
{
	return NULL;
}

#define MEM_THRESHOLD 96*1024*1024

/*
==================
Sys_LowPhysicalMemory

TODO
==================
*/
qboolean Sys_LowPhysicalMemory( void )
{
	return qfalse;
}

/*
==================
Sys_Basename
==================
*/
const char *Sys_Basename( char *path )
{
	return basename( path );
}

/*
==================
Sys_Dirname
==================
*/
const char *Sys_Dirname( char *path )
{
	return dirname( path );
}

/*
==================
Sys_Mkdir
==================
*/
qboolean Sys_Mkdir( const char *path )
{
	int result = mkdir( path, 0750 );

	if( result != 0 )
		return errno == EEXIST;

	return qtrue;
}

/*
==================
Sys_Cwd
==================
*/
char *Sys_Cwd( void )
{
	static char cwd[MAX_OSPATH];

	char *result = getcwd( cwd, sizeof( cwd ) - 1 );
	if( result != cwd )
		return NULL;

	cwd[MAX_OSPATH-1] = 0;

	return cwd;
}

/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define MAX_FOUND_FILES 0x1000

/*
==================
Sys_ListFilteredFiles
==================
*/
void Sys_ListFilteredFiles( const char *basedir, char *subdirs, char *filter, char **list, int *numfiles )
{
	char          search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	char          filename[MAX_OSPATH];
	DIR           *fdir;
	struct dirent *d;
	struct stat   st;

	if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
		return;
	}

	if (strlen(subdirs)) {
		Com_sprintf( search, sizeof(search), "%s/%s", basedir, subdirs );
	}
	else {
		Com_sprintf( search, sizeof(search), "%s", basedir );
	}

	if ((fdir = opendir(search)) == NULL) {
		return;
	}

	while ((d = readdir(fdir)) != NULL) {
		Com_sprintf(filename, sizeof(filename), "%s/%s", search, d->d_name);
		if (stat(filename, &st) == -1)
			continue;

		if (st.st_mode & S_IFDIR) {
			if (Q_stricmp(d->d_name, ".") && Q_stricmp(d->d_name, "..")) {
				if (strlen(subdirs)) {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s/%s", subdirs, d->d_name);
				}
				else {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s", d->d_name);
				}
				Sys_ListFilteredFiles( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
			break;
		}
		Com_sprintf( filename, sizeof(filename), "%s/%s", subdirs, d->d_name );
		if (!Com_FilterPath( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = CopyString( filename );
		(*numfiles)++;
	}

	closedir(fdir);
}

/*
==================
Sys_ListFiles
==================
*/
char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs )
{
	struct dirent *d;
	DIR           *fdir;
	qboolean      dironly = wantsubs;
	char          search[MAX_OSPATH];
	int           nfiles;
	char          **listCopy;
	char          *list[MAX_FOUND_FILES];
	int           i;
	struct stat   st;

	int           extLen;

	if (filter) {

		nfiles = 0;
		Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );

		list[ nfiles ] = NULL;
		*numfiles = nfiles;

		if (!nfiles)
			return NULL;

		listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0 ; i < nfiles ; i++ ) {
			listCopy[i] = list[i];
		}
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension)
		extension = "";

	if ( extension[0] == '/' && extension[1] == 0 ) {
		extension = "";
		dironly = qtrue;
	}

	extLen = strlen( extension );

	// search
	nfiles = 0;

	if ((fdir = opendir(directory)) == NULL) {
		*numfiles = 0;
		return NULL;
	}

	while ((d = readdir(fdir)) != NULL) {
		Com_sprintf(search, sizeof(search), "%s/%s", directory, d->d_name);
		if (stat(search, &st) == -1)
			continue;
		if ((dironly && !(st.st_mode & S_IFDIR)) ||
			(!dironly && (st.st_mode & S_IFDIR)))
			continue;

		if (*extension) {
			if ( strlen( d->d_name ) < strlen( extension ) ||
				Q_stricmp(
					d->d_name + strlen( d->d_name ) - strlen( extension ),
					extension ) ) {
				continue; // didn't match
			}
		}

		if ( nfiles == MAX_FOUND_FILES - 1 )
			break;
		list[ nfiles ] = CopyString( d->d_name );
		nfiles++;
	}

	list[ nfiles ] = NULL;

	closedir(fdir);

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles ) {
		return NULL;
	}

	listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ ) {
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	return listCopy;
}

/*
==================
Sys_FreeFileList
==================
*/
void Sys_FreeFileList( char **list )
{
	int i;

	if ( !list ) {
		return;
	}

	for ( i = 0 ; list[i] ; i++ ) {
		Z_Free( list[i] );
	}

	Z_Free( list );
}

/*
==================
Sys_Sleep

Block execution for msec or until input is recieved.
==================
*/
void Sys_Sleep( int msec )
{
	if( msec == 0 )
		return;

	if( stdinIsATTY )
	{
		fd_set fdset;

		FD_ZERO(&fdset);
		FD_SET(STDIN_FILENO, &fdset);
		if( msec < 0 )
		{
			select(STDIN_FILENO + 1, &fdset, NULL, NULL, NULL);
		}
		else
		{
			struct timeval timeout;

			timeout.tv_sec = msec/1000;
			timeout.tv_usec = (msec%1000)*1000;
			select(STDIN_FILENO + 1, &fdset, NULL, NULL, &timeout);
		}
	}
	else
	{
		// With nothing to select() on, we can't wait indefinitely
		if( msec < 0 )
			msec = 10;

		usleep( msec * 1000 );
	}
}

/*
==============
Sys_ErrorDialog

Display an error message
==============
*/
void Sys_ErrorDialog( const char *error )
{
	char buffer[ 1024 ];
	unsigned int size;
	int f = -1;
	const char *homepath = Cvar_VariableString( "fs_homepath" );
	const char *gamedir = Cvar_VariableString( "fs_gamedir" );
	const char *fileName = "crashlog.txt";
	char *ospath = FS_BuildOSPath( homepath, gamedir, fileName );

	Sys_Print( va( "%s\n", error ) );

#ifndef DEDICATED
	Sys_Dialog( DT_ERROR, va( "%s. See \"%s\" for details.", error, ospath ), "Error" );
#endif

	// Make sure the write path for the crashlog exists...
	if( FS_CreatePath( ospath ) ) {
		Com_Printf( "ERROR: couldn't create path '%s' for crash log.\n", ospath );
		return;
	}

	// We might be crashing because we maxed out the Quake MAX_FILE_HANDLES,
	// which will come through here, so we don't want to recurse forever by
	// calling FS_FOpenFileWrite()...use the Unix system APIs instead.
	f = open( ospath, O_CREAT | O_TRUNC | O_WRONLY, 0640 );
	if( f == -1 )
	{
		Com_Printf( "ERROR: couldn't open %s\n", fileName );
		return;
	}

	// We're crashing, so we don't care much if write() or close() fails.
	while( ( size = CON_LogRead( buffer, sizeof( buffer ) ) ) > 0 ) {
		if( write( f, buffer, size ) != size ) {
			Com_Printf( "ERROR: couldn't fully write to %s\n", fileName );
			break;
		}
	}

	close( f );
}

#ifndef MACOS_X
/*
==============
Sys_ZenityCommand
==============
*/
static int Sys_ZenityCommand( dialogType_t type, const char *message, const char *title )	
{
	const char *options = "";
	char       command[ 1024 ];

	switch( type )
	{
		default:
		case DT_INFO:      options = "--info"; break;
		case DT_WARNING:   options = "--warning"; break;
		case DT_ERROR:     options = "--error"; break;
		case DT_YES_NO:    options = "--question --ok-label=\"Yes\" --cancel-label=\"No\""; break;
		case DT_OK_CANCEL: options = "--question --ok-label=\"OK\" --cancel-label=\"Cancel\""; break;
	}

	Com_sprintf( command, sizeof( command ), "zenity %s --text=\"%s\" --title=\"%s\"",
		options, message, title );

	return system( command );
}

/*
==============
Sys_KdialogCommand
==============
*/
static int Sys_KdialogCommand( dialogType_t type, const char *message, const char *title )
{
	const char *options = "";
	char       command[ 1024 ];

	switch( type )
	{
		default:
		case DT_INFO:      options = "--msgbox"; break;
		case DT_WARNING:   options = "--sorry"; break;
		case DT_ERROR:     options = "--error"; break;
		case DT_YES_NO:    options = "--warningyesno"; break;
		case DT_OK_CANCEL: options = "--warningcontinuecancel"; break;
	}

	Com_sprintf( command, sizeof( command ), "kdialog %s \"%s\" --title \"%s\"",
		options, message, title );

	return system( command );
}

/*
==============
Sys_XmessageCommand
==============
*/
static int Sys_XmessageCommand( dialogType_t type, const char *message, const char *title )
{
	const char *options = "";
	char       command[ 1024 ];

	switch( type )
	{
		default:           options = "-buttons OK"; break;
		case DT_YES_NO:    options = "-buttons Yes:0,No:1"; break;
		case DT_OK_CANCEL: options = "-buttons OK:0,Cancel:1"; break;
	}

	Com_sprintf( command, sizeof( command ), "xmessage -center %s \"%s\"",
		options, message );

	return system( command );
}

/*
==============
Sys_Dialog

Display a *nix dialog box
==============
*/
dialogResult_t Sys_Dialog( dialogType_t type, const char *message, const char *title )
{
	typedef enum
	{
		NONE = 0,
		ZENITY,
		KDIALOG,
		XMESSAGE,
		NUM_DIALOG_PROGRAMS
	} dialogCommandType_t;
	typedef int (*dialogCommandBuilder_t)( dialogType_t, const char *, const char * );

	const char              *session = getenv( "DESKTOP_SESSION" );
	qboolean                tried[ NUM_DIALOG_PROGRAMS ] = { qfalse };
	dialogCommandBuilder_t  commands[ NUM_DIALOG_PROGRAMS ] = { NULL };
	dialogCommandType_t     preferredCommandType = NONE;

	commands[ ZENITY ] = &Sys_ZenityCommand;
	commands[ KDIALOG ] = &Sys_KdialogCommand;
	commands[ XMESSAGE ] = &Sys_XmessageCommand;

	// This may not be the best way
	if( !Q_stricmp( session, "gnome" ) )
		preferredCommandType = ZENITY;
	else if( !Q_stricmp( session, "kde" ) )
		preferredCommandType = KDIALOG;

	while( 1 )
	{
		int i;
		int exitCode;

		for( i = NONE + 1; i < NUM_DIALOG_PROGRAMS; i++ )
		{
			if( preferredCommandType != NONE && preferredCommandType != i )
				continue;

			if( !tried[ i ] )
			{
				exitCode = commands[ i ]( type, message, title );

				if( exitCode >= 0 )
				{
					switch( type )
					{
						case DT_YES_NO:    return exitCode ? DR_NO : DR_YES;
						case DT_OK_CANCEL: return exitCode ? DR_CANCEL : DR_OK;
						default:           return DR_OK;
					}
				}

				tried[ i ] = qtrue;

				// The preference failed, so start again in order
				if( preferredCommandType != NONE )
				{
					preferredCommandType = NONE;
					break;
				}
			}
		}

		for( i = NONE + 1; i < NUM_DIALOG_PROGRAMS; i++ )
		{
			if( !tried[ i ] )
				continue;
		}

		break;
	}

	Com_DPrintf( S_COLOR_YELLOW "WARNING: failed to show a dialog\n" );
	return DR_OK;
}
#endif

/*
==============
Sys_GLimpSafeInit

Unix specific "safe" GL implementation initialisation
==============
*/
void Sys_GLimpSafeInit( void )
{
	// NOP
}

/*
==============
Sys_GLimpInit

Unix specific GL implementation initialisation
==============
*/
void Sys_GLimpInit( void )
{
	// NOP
}

/*
==============
Sys_PlatformInit

Unix specific initialisation
==============
*/
void Sys_PlatformInit( void )
{
	const char* term = getenv( "TERM" );

	signal( SIGHUP, Sys_SigHandler );
	signal( SIGQUIT, Sys_SigHandler );
	signal( SIGTRAP, Sys_SigHandler );
	signal( SIGIOT, Sys_SigHandler );
	signal( SIGBUS, Sys_SigHandler );

	stdinIsATTY = isatty( STDIN_FILENO ) &&
		!( term && ( !strcmp( term, "raw" ) || !strcmp( term, "dumb" ) ) );
}

/*
==============
Sys_SetEnv

set/unset environment variables (empty value removes it)
==============
*/

void Sys_SetEnv(const char *name, const char *value)
{
	if(value && *value)
		setenv(name, value, 1);
	else
		unsetenv(name);
}

/*
==============
Sys_PID
==============
*/
int Sys_PID( void )
{
	return getpid( );
}

/*
==============
Sys_PIDIsRunning
==============
*/
qboolean Sys_PIDIsRunning( int pid )
{
	return kill( pid, 0 ) == 0;
}

// fretn

// TTimo - Wolf MP specific, adding .mp. to shared objects
char* Sys_GetDLLName( const char *name ) {
#if defined __i386__
        return va( "%s.mp.i386.so", name );
#elif defined __powerpc__
        return va( "%s.mp.ppc.so", name );
#elif defined __axp__
        return va( "%s.mp.axp.so", name );
#elif defined __mips__
        return va( "%s.mp.mips.so", name );
#elif defined __ppc__
        return va( "%s.mp.ppc.so", name );
#else
#error Unknown arch
#endif
}

int Sys_GetProcessorId( void ) { 
        // TODO TTimo add better CPU identification?
        // see Sys_GetHighQualityCPU
        return CPUID_GENERIC;
}

int Sys_GetHighQualityCPU() {
        // TODO TTimo see win_shared.c IsP3 || IsAthlon
        return 0;
}

#if 0

/*
========================================================================

EVENT LOOP

========================================================================
*/

// bk000306: upped this from 64
#define MAX_QUED_EVENTS     256
#define MASK_QUED_EVENTS    ( MAX_QUED_EVENTS - 1 )

sysEvent_t eventQue[MAX_QUED_EVENTS];
// bk000306: initialize
int eventHead = 0; 
int eventTail = 0; 
byte sys_packetReceived[MAX_MSGLEN];

/*
================
Sys_QueEvent

A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr ) {
        sysEvent_t  *ev; 

        ev = &eventQue[ eventHead & MASK_QUED_EVENTS ];

        // bk000305 - was missing
        if ( eventHead - eventTail >= MAX_QUED_EVENTS ) {
                Com_Printf( "Sys_QueEvent: overflow\n" );
                // we are discarding an event, but don't leak memory
                if ( ev->evPtr ) {
                        Z_Free( ev->evPtr );
                }    
                eventTail++;
        }    

        eventHead++;

        if ( time == 0 ) {
                time = Sys_Milliseconds();
        }    

        ev->evTime = time;
        ev->evType = type;
        ev->evValue = value;
        ev->evValue2 = value2;
        ev->evPtrLength = ptrLength;
        ev->evPtr = ptr;
}

/*
================
Sys_GetEvent

================
*/
sysEvent_t Sys_GetEvent( void ) {
        sysEvent_t ev;
        char    *s;
        msg_t netmsg;
        netadr_t adr;

        // return if we have data
        if ( eventHead > eventTail ) {
                eventTail++;
                return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
        }

        // pump the message loop
        // in vga this calls KBD_Update, under X, it calls GetEvent
        Sys_SendKeyEvents();

        // check for console commands
        s = Sys_ConsoleInput();
        if ( s ) {
                char  *b;
                int len;

                len = strlen( s ) + 1;
                b = Z_Malloc( len );
                strcpy( b, s );
                Sys_QueEvent( 0, SE_CONSOLE, 0, 0, len, b );
        }

        // check for other input devices
        IN_Frame();

        // check for network packets
        MSG_Init( &netmsg, sys_packetReceived, sizeof( sys_packetReceived ) );
        if ( Sys_GetPacket( &adr, &netmsg ) ) {
                netadr_t    *buf;
                int len;

                // copy out to a seperate buffer for qeueing
                len = sizeof( netadr_t ) + netmsg.cursize;
                buf = Z_Malloc( len );
                *buf = adr;
                memcpy( buf + 1, netmsg.data, netmsg.cursize );
                Sys_QueEvent( 0, SE_PACKET, 0, 0, len, buf );
        }

        // return if we have data
        if ( eventHead > eventTail ) {
                eventTail++;
                return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
        }

        // create an empty event to return

        memset( &ev, 0, sizeof( ev ) );
        ev.evTime = Sys_Milliseconds();

        return ev;
}
#endif

void Sys_InitStreamThread( void ) {
}

void Sys_ShutdownStreamThread( void ) {
}

void Sys_BeginStreamedFile( fileHandle_t f, int readAhead ) {
}

void Sys_EndStreamedFile( fileHandle_t f ) {
}

int Sys_StreamedRead( void *buffer, int size, int count, fileHandle_t f ) {
        return FS_Read( buffer, size * count, f ); 
}

void Sys_StreamSeek( fileHandle_t f, int offset, int origin ) {
        FS_Seek( f, offset, origin );
}

// Used to determine CD Path
static char cdPath[MAX_OSPATH];


void Sys_SetDefaultCDPath( const char *path ) { 
        Q_strncpyz( cdPath, path, sizeof( cdPath ) );
}

char *Sys_DefaultCDPath( void ) { 
        return cdPath;
}

void *Sys_InitializeCriticalSection() {
        return (void *)-1;
}

void Sys_EnterCriticalSection( void *ptr ) { 
}

void Sys_LeaveCriticalSection( void *ptr ) { 
}

void Sys_BeginProfiling( void ) {
}

#define MAX_CMD 1024
static char exit_cmdline[MAX_CMD] = "";
void Sys_DoStartProcess( char *cmdline );

/*
==================
chmod OR on a file
==================
*/
void Sys_Chmod( char *file, int mode ) {
        struct stat s_buf;
        int perm;
        if ( stat( file, &s_buf ) != 0 ) {
                Com_Printf( "stat('%s')  failed: errno %d\n", file, errno );
                return;
        }    
        perm = s_buf.st_mode | mode;
        if ( chmod( file, perm ) != 0 ) {
                Com_Printf( "chmod('%s', %d) failed: errno %d\n", file, perm, errno );
        }    
        Com_DPrintf( "chmod +%d '%s'\n", mode, file );
}


/*
==================
Sys_DoStartProcess
actually forks and starts a process

UGLY HACK:
  Sys_StartProcess works with a command line only
  if this command line is actually a binary with command line parameters,
  the only way to do this on unix is to use a system() call
  but system doesn't replace the current process, which leads to a situation like:
  wolf.x86--spawned_process.x86
  in the case of auto-update for instance, this will cause write access denied on wolf.x86:
  wolf-beta/2002-March/000079.html
  we hack the implementation here, if there are no space in the command line, assume it's a straight process and use execl
  otherwise, use system ..
  The clean solution would be Sys_StartProcess and Sys_StartProcess_Args..
==================
*/
void Sys_DoStartProcess( char *cmdline ) {
        switch ( fork() )
        {    
        case - 1: 
                // main thread
                break;
        case 0:
                if ( strchr( cmdline, ' ' ) ) {
                        system( cmdline );
                } else 
                {    
                        execl( cmdline, cmdline, NULL );
                        printf( "execl failed: %s\n", strerror( errno ) ); 
                }    
                _exit( 0 ); 
                break;
        }    
}

/*
==================
Sys_StartProcess
if !doexit, start the process asap
otherwise, push it for execution at exit
(i.e. let complete shutdown of the game and freeing of resources happen)
NOTE: might even want to add a small delay?
==================
*/
void Sys_StartProcess( char *cmdline, qboolean doexit ) {

        if ( doexit ) {
                Com_DPrintf( "Sys_StartProcess %s (delaying to final exit)\n", cmdline );
                Q_strncpyz( exit_cmdline, cmdline, MAX_CMD );
                Cbuf_ExecuteText( EXEC_APPEND, "quit\n" );
                return;
        }    

        Com_DPrintf( "Sys_StartProcess %s\n", cmdline );
        Sys_DoStartProcess( cmdline );
}


/*
=================
Sys_OpenURL
=================
*/
void Sys_OpenURL( const char *url, qboolean doexit ) {
        char *basepath, *homepath, *pwdpath;
        char fname[20];
        char fn[MAX_OSPATH];
        char cmdline[MAX_CMD];

        Com_Printf( "Sys_OpenURL %s\n", url );
        // opening an URL on *nix can mean a lot of things ..
        // just spawn a script instead of deciding for the user :-)

        // do the setup before we fork
        // search for an openurl.sh script
        // search procedure taken from Sys_LoadDll
        Q_strncpyz( fname, "openurl.sh", 20 );

        pwdpath = Sys_Cwd();
        Com_sprintf( fn, MAX_OSPATH, "%s/%s", pwdpath, fname );
        if ( access( fn, X_OK ) == -1 ) {
                Com_DPrintf( "%s not found\n", fn );
                // try in home path
                homepath = Cvar_VariableString( "fs_homepath" );
                Com_sprintf( fn, MAX_OSPATH, "%s/%s", homepath, fname );
                if ( access( fn, X_OK ) == -1 ) {
                        Com_DPrintf( "%s not found\n", fn );
                        // basepath, last resort
                        basepath = Cvar_VariableString( "fs_basepath" );
                        Com_sprintf( fn, MAX_OSPATH, "%s/%s", basepath, fname );
                        if ( access( fn, X_OK ) == -1 ) {
                                Com_DPrintf( "%s not found\n", fn );
                                Com_Printf( "Can't find script '%s' to open requested URL (use +set developer 1 for more verbosity)\n", fname );
                                // we won't quit
                                return;
                        }    
                }    
        }    

        Com_DPrintf( "URL script: %s\n", fn );

        // build the command line
        Com_sprintf( cmdline, MAX_CMD, "%s '%s' &", fn, url );

        Sys_StartProcess( cmdline, doexit );

}

