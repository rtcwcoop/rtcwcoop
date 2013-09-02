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
This is a small webserver that autostarts when a server is started.
This way clients can easily autodownload pk3 files (fast). And it requires no setup
on the server end.
its running on the tcp version of net_port
27960 TCP by default
*/

#ifdef USE_LOCAL_HEADERS
#   include "SDL_thread.h"
#else
#   include <SDL_thread.h>
#endif

#include "server.h"

#ifdef USE_HTTP_SERVER

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#ifdef WIN32
# include <winsock2.h>
# include <windows.h>
#else
# include <pwd.h>
# include <unistd.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/socket.h>
#endif

#ifdef WIN32
typedef int socklen_t;
#endif

#define BACKLOG 10

# if !defined HAVE_CLOSESOCKET
#  define closesocket close
# endif

typedef struct HTTPState_s {
	unsigned short port;
	int sock;

	struct in_addr listen_addr;

	struct sockaddr_in address;
} HTTPState_t;

HTTPState_t HTTPState;


//Constants that indicate the state of the HTTP thread.
#define HTTP_THREAD_DEAD        0   // Thread is dead or hasn't been started
#define HTTP_THREAD_INITIALISED 1   // Thread is initialised
#define HTTP_THREAD_QUITTING    2   // The thread is being killed

// Status of the http thread
static int HTTP_ThreadStatus = HTTP_THREAD_DEAD;
// Quit requested?
static qboolean HTTP_QuitRequested;

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

int HTTP_SendMSG( int socket, char *buf ) {
	int _send = send( socket, buf, strlen( buf ), 0 );
	if ( _send == -1 ) {
		Com_Printf( "HTTP ERROR: socket send failed (%s)\n", strerror( errno ) );
	}

	return _send;
}

int HTTP_ClientConnection( void *sock ) {
	char *token;
	int idPack;
	char *input;
	int len;
	int handle;
	char *downloadName;
	char *fs_game;
	char *tofree;
	char *string;
	int cnt = 0;
	int numbytes = 0;
	char buf[512];
	int socket = (int)sock;

	if ( ( numbytes = recv( socket, buf, 512 - 1, 0 ) ) == -1 ) {
		Com_Printf( "HTTP_ClientConnection: ERROR: socket recv failed (%s)\n", strerror( errno ) );
		return 0;
	}

	buf[numbytes] = '\0';
	input = va( "%s", buf );

	token = COM_Parse( &input );

	if ( strcmp( token, "GET" ) ) {
		Com_Printf( "HTTP_ClientConnection: need GET, received: '%s'\n", token );
		closesocket( socket );
		return 0;
	}

	// get path
	token = COM_Parse( &input );
	string = strdup( token );

	if ( string != NULL ) {
		tofree = string;

		while ( ( token = strsep( &string, "/" ) ) != NULL && cnt < 3 ) {
			if ( cnt == 1 ) {
				fs_game = strdup( token );
			} else if ( cnt == 2 ) {
				downloadName = strdup( token );
			}
			cnt++;
		}

		free( tofree );
	}

	len = FS_SV_FOpenFileRead( va( "%s/%s", fs_game, downloadName ), &handle );

	if ( !FS_VerifyPak( va( "%s/%s", fs_game, downloadName ) ) ) {
		Com_DPrintf( "HTTP_ClientConnection: Not on the download paklist: %s\n", downloadName );
		len = 0;
	}

	idPack = SV_FS_idPak( va( "%s/%s", fs_game, downloadName ), (char *)"main" );

	if ( idPack ) {
		Com_DPrintf( "HTTP_ClientConnection: Cannot download idPakfiles\n" );
		len = 0;
	}

	if ( len ) {
		#define CHUNK_SIZE 512

		char *file_data = malloc( CHUNK_SIZE );

		size_t nbytes = 0;

		HTTP_SendMSG( socket, "HTTP/1.1 200 OK\n" );
		HTTP_SendMSG( socket, va( "Content-Length: %d\n", len ) );
		HTTP_SendMSG( socket, "Content-Type: application/zip; charset=UTF-8\n" );
		HTTP_SendMSG( socket, "\n" );

		while ( ( nbytes = FS_Read( file_data, CHUNK_SIZE, handle ) ) > 0 ) {
			int offset = 0;
			int sent;

			while ( ( sent = send( socket, file_data + offset, nbytes, 0 ) ) > 0 || ( sent == -1 && errno == EINTR ) ) {

				if ( sent > 0 ) {
					offset += sent;
					nbytes -= sent;
				}
			}
		}

		free( file_data );

	} else {
		HTTP_SendMSG( socket, "HTTP/1.1 404 Not Found\n" );
		HTTP_SendMSG( socket, "Server: rtcwcoop" );
		HTTP_SendMSG( socket, "Content-Type: text/html; charset=UTF-8\n" );
		HTTP_SendMSG( socket, "Connection: close\n" );
		HTTP_SendMSG( socket, "\n" );
	}

	closesocket( socket );

	return 1;

}

static void HTTP_MainLoop( void ) {

	socklen_t sin_size;
	int clientsock;
	struct sockaddr_in their_addr;

	do {
		// If we must quit, send the command.
		if ( HTTP_QuitRequested && HTTP_ThreadStatus != HTTP_THREAD_QUITTING ) {
			HTTP_ThreadStatus = HTTP_THREAD_QUITTING;
		} else {
			//
			sin_size = sizeof( struct sockaddr_in );

			if ( ( clientsock = accept( HTTPState.sock, (struct sockaddr *)&their_addr, &sin_size ) ) == -1 ) {
				Com_Printf( "HTTP ERROR: socket accept failed (%s)\n", strerror( errno ) );
				continue;
			}

			SDL_CreateThread( HTTP_ClientConnection, (void *)clientsock );

		}
	} while ( HTTP_ThreadStatus != HTTP_THREAD_QUITTING );
}

static void HTTP_Thread( void ) {
	int yes = 1;
	cvar_t *port;

	port = Cvar_Get( "net_port", va( "%i", PORT_SERVER ), CVAR_LATCH );

	// set default values
	HTTPState.port = port->integer;
	HTTPState.sock = -1;

#ifdef WIN32
	WSADATA winsockdata;

	if ( WSAStartup( MAKEWORD( 1, 1 ), &winsockdata ) ) {
		Com_Printf( "HTTP ERROR: can't initialize winsock\n" );
		return;
	}
#endif

	// open the socket
	HTTPState.sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( HTTPState.sock < 0 ) {
		Com_Printf( "HTTP ERROR: socket creation failed (%s)\n", strerror( errno ) );
		return;
	}


	if ( setsockopt( HTTPState.sock, SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof( int ) ) == -1 ) {
		Com_Printf( "HTTP ERROR: setsockopt failed (%s)\n", strerror( errno ) );
		return;
	}

	// bind it to the desired port
	memset( &HTTPState.address, 0, sizeof( HTTPState.address ) );

	HTTPState.address.sin_family = AF_INET;
	HTTPState.address.sin_addr.s_addr = htonl( INADDR_ANY );
	HTTPState.address.sin_port = htons( HTTPState.port );

	if ( bind( HTTPState.sock, (struct sockaddr*)&HTTPState.address, sizeof( HTTPState.address ) ) == -1 ) {
		Com_Printf( "HTTP ERROR: socket binding failed (%s)\n", strerror( errno ) );
		return;
	}

// results in a crash: ERROR: FS_FileForHandle: NULL
//    Com_Printf("HTTP Listening on TCP port %hu\n", ntohs (HTTPState.address.sin_port));

	if ( listen( HTTPState.sock, BACKLOG ) == -1 ) {
		Com_Printf( "HTTP ERROR: socket listen failed (%s)\n", strerror( errno ) );
		return;
	}

	HTTP_MainLoop();

	closesocket( HTTPState.sock );
#ifdef WIN32
	WSACleanup();
#endif

	return;
}

SDL_Thread *HTTP_ThreadHandle = NULL;

/*
==================
HTTP_SystemThreadProc
==================
*/
static int HTTP_SystemThreadProc( void *dummy ) {
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
		HTTP_ThreadHandle = SDL_CreateThread( HTTP_SystemThreadProc, NULL );
	}
}

/*
==================
HTTP_WaitThread
==================
*/
static void HTTP_WaitThread( void ) {
	if ( HTTP_ThreadHandle != NULL ) {
		if ( HTTP_ThreadStatus != HTTP_THREAD_DEAD ) {
			//SDL_WaitThread(HTTP_ThreadHandle, NULL);
			// don't wait until its finished, just shut down
			SDL_KillThread( HTTP_ThreadHandle );
		}
		HTTP_ThreadStatus = HTTP_THREAD_DEAD;
		HTTP_ThreadHandle = NULL;
	}
}


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
	cvar_t *_sv_allowDownload = Cvar_Get( "sv_allowDownload", "1", 0 );

	if (_sv_allowDownload->integer) {
		SV_InitHTTP();
	}
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

	HTTP_QuitRequested = qtrue;
	closesocket( HTTPState.sock );
#ifdef WIN32
	WSACleanup();
#endif

}

/*
==================
SV_HTTPIsRunning
==================
*/
qboolean SV_HTTPIsRunning( void ) {
	return ( HTTP_ThreadStatus != HTTP_THREAD_DEAD );
}

#endif
