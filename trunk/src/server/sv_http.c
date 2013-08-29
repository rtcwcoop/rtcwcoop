/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
Copyright 2013 fretn

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (<93>RTCW SP Source Code<94>).

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
This is a small webserver (libmicrohttpd) that autostarts when a server is started.
This way clients can easily autodownload pk3 files (fast). And it requires no setup
on the server end.
*/

#include "server.h"
#include "../microhttpd/microhttpd.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifdef WIN32

#else
#include <pthread.h>
#endif

//Constants that indicate the state of the HTTP thread.
#define HTTP_THREAD_DEAD        0   // Thread is dead or hasn't been started
#define HTTP_THREAD_INITIALISED 1   // Thread is initialised
#define HTTP_THREAD_QUITTING    2   // The thread is being killed

// Function that sets the thread status when the thread dies. Since that is
// system-dependent, it can't be done in the thread's main code.

static void HTTP_SetThreadDead( void );

// Status of the http thread
static int HTTP_ThreadStatus = HTTP_THREAD_DEAD;
// Quit requested?
static qboolean HTTP_QuitRequested;

#define PAGE "<html><head><title>File not found</title></head><body>File not found</body></html>"

static ssize_t file_reader( void *cls, uint64_t pos, char *buf, size_t max ) {
	int handle = (int)cls;

	FS_Seek( handle, pos, FS_SEEK_SET );
	return FS_Read( buf, max, handle );
}

static void free_callback( void *cls ) {
	int handle = (int)cls;
	FS_FCloseFile( handle );
}

/*
================
SV_FS_idPak
================
*/
// wasnt sure if the original function of this in files.c is working correctly
// so I copied it and modified so I don't break existing things
static qboolean SV_FS_idPak( char *pak, char *base ) {
	int i;

	for ( i = 0; i < NUM_ID_PAKS; i++ ) {
		if ( !FS_FilenameCompare( pak, va( "%s/pak%d.pk3", base, i ) ) ) {
			break;
		}
		if ( !FS_FilenameCompare( pak, va( "%s/mp_pak%d.pk3",base,i ) ) ) {
			break;
		}
		if ( !FS_FilenameCompare( pak, va( "%s/sp_pak%d.pk3",base,i ) ) ) {
			break;
		}
	}
	if ( i < NUM_ID_PAKS ) {
		return qtrue;
	}
	return qfalse;
}



static int ahc_echo( void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **ptr ) {
	static int aptr;
	struct MHD_Response *response;
	int ret;
	int handle;
	int len;
	int idPack;
/*
    char *downloadName;
    char *fs_game;
    char *token;
    char *tofree;
    char *string;
    int cnt = 0;
*/

	if ( 0 != strcmp( method, MHD_HTTP_METHOD_GET ) ) {
		return MHD_NO;              // unexpected method

	}
	if ( &aptr != *ptr ) {
		// do never respond on first call
		*ptr = &aptr;
		return MHD_YES;
	}

	*ptr = NULL;
/*
    string = strdup(&url[1]);

    if (string != NULL) {
        tofree = string;

        while ((token = strsep(&string, "/")) != NULL && cnt < 2)
        {
            if (cnt == 0) {
                fs_game = strdup(token);
            } else if (cnt == 1) {
                downloadName = strdup(token);
            }
            cnt++;
        }

        free(tofree);
    }
*/

	len = FS_SV_FOpenFileRead( &url[1], &handle );

	if ( !FS_VerifyPak( &url[1] ) ) {
		Com_DPrintf( "Not on the download paklist: %s\n", &url[1] );
		len = 0;
	}

	idPack = SV_FS_idPak( (char *)&url[1], (char *)"main" );

	if ( idPack ) {
		Com_DPrintf( "Cannot download idPakfiles\n" );
		len = 0;
	}

/*
    free(fs_game);
    free(downloadName);
*/

	if ( len ) {
		response = MHD_create_response_from_callback( len, 32 * 1024, &file_reader, (FILE *)handle, &free_callback );
		if ( response == NULL ) {
			FS_FCloseFile( handle );
			return MHD_NO;
		}
		ret = MHD_queue_response( connection, MHD_HTTP_OK, response );
		MHD_destroy_response( response );
	} else {
		response = MHD_create_response_from_buffer( strlen( PAGE ), (void *) PAGE, MHD_RESPMEM_PERSISTENT );
		ret = MHD_queue_response( connection, MHD_HTTP_NOT_FOUND, response );
		MHD_destroy_response( response );
	}

	return ret;
}


static void HTTP_MainLoop( void ) {

	do {
		// If we must quit, send the command.
		if ( HTTP_QuitRequested && HTTP_ThreadStatus != HTTP_THREAD_QUITTING ) {
			HTTP_ThreadStatus = HTTP_THREAD_QUITTING;
		} else {
			//
		}
	} while ( HTTP_ThreadStatus != HTTP_THREAD_QUITTING );
}

static void HTTP_Thread( void ) {
	struct MHD_Daemon *d;
	cvar_t *port = Cvar_Get( "net_port", va( "%i", PORT_SERVER ), CVAR_LATCH );

// use the same port as net_port, but on tcp
	d = MHD_start_daemon( MHD_USE_THREAD_PER_CONNECTION | MHD_USE_DEBUG,
						  port->integer,
						  NULL, NULL, &ahc_echo, PAGE, MHD_OPTION_END );
	if ( d == NULL ) {
		Com_Printf( "Could not init http server\n" );
		HTTP_SetThreadDead();
		return;
	}

	HTTP_MainLoop();

	MHD_stop_daemon( d );

	Com_Printf( "Quitting http server\n" );
	HTTP_SetThreadDead();
}





/*
 * Caution: HTTP_SystemThreadProc(), HTTP_StartThread() and HTTP_WaitThread()
 *  have separate "VARIANTS".
 *
 * Note different prototypes for HTTP_SystemThreadProc() and completely
 * different HTTP_StartThread()/HTTP_WaitThread() implementations.
 */

#ifdef WIN32

static HANDLE HTTP_ThreadHandle = NULL;
/*
==================
HTTP_SystemThreadProc
==================
*/
static DWORD WINAPI HTTP_SystemThreadProc( LPVOID dummy ) {
	HTTP_Thread();
	return 0;
}

/*
==================
HTTP_StartThread
==================
*/
static void HTTP_StartThread( void ) {
	if ( HTTP_ThreadHandle == NULL ) {
		HTTP_ThreadHandle = CreateThread( NULL, 0, HTTP_SystemThreadProc, NULL, 0, NULL );
	}
}

/*
==================
HTTP_SetThreadDead
==================
*/
static void HTTP_SetThreadDead( void ) {
	HTTP_ThreadStatus = HTTP_THREAD_DEAD;
	HTTP_ThreadHandle = NULL;
}

/*
==================
HTTP_StartThread
==================
*/
static void HTTP_WaitThread( void ) {
	if ( HTTP_ThreadHandle != NULL ) {
		if ( HTTP_ThreadStatus != HTTP_THREAD_DEAD ) {
			WaitForSingleObject( HTTP_ThreadHandle, 10000 );
			CloseHandle( HTTP_ThreadHandle );
		}
		HTTP_ThreadHandle = NULL;
	}
}

#elif defined __linux__ || defined MACOS_X || defined __FreeBSD__

static pthread_t HTTP_ThreadHandle = (pthread_t) NULL;
/*
==================
HTTP_SystemThreadProc
==================
*/
static void *HTTP_SystemThreadProc( void *dummy ) {
	HTTP_Thread();
	return NULL;
}

/*
==================
HTTP_StartThread
==================
*/
static void HTTP_StartThread( void ) {
	if ( HTTP_ThreadHandle == (pthread_t) NULL ) {
		pthread_create( &HTTP_ThreadHandle, NULL, HTTP_SystemThreadProc, NULL );
	}
}

/*
==================
HTTP_SetThreadDead
==================
*/
static void HTTP_SetThreadDead( void ) {
	HTTP_ThreadStatus = HTTP_THREAD_DEAD;
	HTTP_ThreadHandle = (pthread_t) NULL;
}

/*
==================
HTTP_WaitThread
==================
*/
static void HTTP_WaitThread( void ) {
	if ( HTTP_ThreadHandle != (pthread_t) NULL ) {
		if ( HTTP_ThreadStatus != HTTP_THREAD_DEAD ) {
			pthread_join( HTTP_ThreadHandle, NULL );
		}
		HTTP_ThreadHandle = (pthread_t) NULL;
	}
}

#endif


/*
==================
SV_InitHTTP
==================
*/
void SV_InitHTTP( void ) {
	if ( HTTP_ThreadStatus != HTTP_THREAD_DEAD ) {
		Com_Printf( "...HTTP thread is already running\n" );
		return;
	}
	HTTP_QuitRequested = qfalse;
	HTTP_ThreadStatus  = HTTP_THREAD_INITIALISED;
	HTTP_StartThread();
}

/*
==================
SV_HTTPSetup
==================
*/

void SV_HTTPSetup( void ) {
/*    cl_HTTP_connect_at_startup = Cvar_Get("cl_HTTP_connect_at_startup", "0", CVAR_ARCHIVE);
    cl_HTTP_server             = Cvar_Get("cl_HTTP_server", "irc.freenode.net", CVAR_ARCHIVE);
    cl_HTTP_channel            = Cvar_Get("cl_HTTP_channel", "rtcwcoop", CVAR_ARCHIVE); // maybe some day..
    cl_HTTP_port               = Cvar_Get("cl_HTTP_port", "6667", CVAR_ARCHIVE);
    cl_HTTP_override_nickname  = Cvar_Get("cl_HTTP_override_nickname", "0", CVAR_ARCHIVE);
    cl_HTTP_nickname           = Cvar_Get("cl_HTTP_nickname", "", CVAR_ARCHIVE);
    cl_HTTP_kick_rejoin        = Cvar_Get("cl_HTTP_kick_rejoin", "0", CVAR_ARCHIVE);
    cl_HTTP_reconnect_delay    = Cvar_Get("cl_HTTP_reconnect_delay", "100", CVAR_ARCHIVE);
*/
	SV_InitHTTP();
}


/*
==================
SV_HTTPInitiateShutdown
==================
*/
void SV_HTTPInitiateShutdown( void ) {
	HTTP_QuitRequested = qtrue;
}

/*
==================
SV_HTTPWaitShutdown
==================
*/
void SV_HTTPWaitShutdown( void ) {
	HTTP_WaitThread();
}

/*
==================
SV_HTTPIsRunning
==================
*/
qboolean SV_HTTPIsRunning( void ) {
	return ( HTTP_ThreadStatus != HTTP_THREAD_DEAD );
}
