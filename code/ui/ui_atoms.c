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

// Copyright (C) 1999-2000 Id Software, Inc.
//
/**********************************************************************
	UI_ATOMS.C

	User interface building blocks and support functions.
**********************************************************************/
#include "ui_local.h"

uiStatic_t uis;
qboolean m_entersound;              // after a frame, so caching won't disrupt the sound

void QDECL Com_DPrintf( const char *fmt, ... ) __attribute__ ( ( format ( printf, 1, 2 ) ) );

// JPW NERVE added Com_DPrintf
#define MAXPRINTMSG 4096
void QDECL Com_DPrintf( const char *fmt, ... ) {
	va_list argptr;
	char msg[MAXPRINTMSG];
	int developer;

	developer = trap_Cvar_VariableValue( "developer" );
	if ( !developer ) {
		return;
	}

	va_start( argptr,fmt );
	Q_vsnprintf( msg, sizeof( msg ), fmt, argptr );
	va_end( argptr );

	Com_Printf( "%s", msg );
}
// jpw

void QDECL Com_Error( int level, const char *error, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, error );
	Q_vsnprintf( text, sizeof ( text ), error, argptr );
	va_end( argptr );

	trap_Error( text );
}

void QDECL Com_Printf( const char *msg, ... ) {
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	Q_vsnprintf( text, sizeof ( text ), msg, argptr );
	va_end( argptr );

	trap_Print( text );
}

/*
=================
UI_ClampCvar
=================
*/
float UI_ClampCvar( float min, float max, float value ) {
	if ( value < min ) {
		return min;
	}
	if ( value > max ) {
		return max;
	}
	return value;
}

/*
=================
UI_StartDemoLoop
=================
*/
void UI_StartDemoLoop( void ) {
	trap_Cmd_ExecuteText( EXEC_APPEND, "d1\n" );
}

// TTimo: unused
/*
static void NeedCDAction( qboolean result ) {
	if ( !result ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
	}
}

static void NeedCDKeyAction( qboolean result ) {
	if ( !result ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "quit\n" );
	}
}
*/

char *UI_Argv( int arg ) {
	static char buffer[MAX_STRING_CHARS];

	trap_Argv( arg, buffer, sizeof( buffer ) );

	return buffer;
}


char *UI_Cvar_VariableString( const char *var_name ) {
	static char buffer[MAX_STRING_CHARS];

	trap_Cvar_VariableStringBuffer( var_name, buffer, sizeof( buffer ) );

	return buffer;
}

void UI_LoadBestScores( const char *map, int game ) {
}

/*
===============
UI_ClearScores
===============
*/
void UI_ClearScores( void ) {
}

static void UI_Cache_f( void ) {
	Display_CacheAll();
}

/*
=======================
UI_CalcPostGameStats
=======================
*/
static void UI_CalcPostGameStats( void ) {
}

/*
=================
UI_ConsoleCommand
=================
*/
qboolean UI_ConsoleCommand( int realTime ) {
	char    *cmd;

	uiInfo.uiDC.frameTime = realTime - uiInfo.uiDC.realTime;
	uiInfo.uiDC.realTime = realTime;

	cmd = UI_Argv( 0 );

	// ensure minimum menu data is available
	//Menu_Cache();

	if ( Q_stricmp( cmd, "ui_test" ) == 0 ) {
		UI_ShowPostGame( qtrue );
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_report" ) == 0 ) {
		UI_Report();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_load" ) == 0 ) {
		UI_Load();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "remapShader" ) == 0 ) {
		if ( trap_Argc() == 4 ) {
			char shader1[MAX_QPATH];
			char shader2[MAX_QPATH];
			char shader3[MAX_QPATH];
			Q_strncpyz( shader1, UI_Argv( 1 ), sizeof( shader1 ) );
			Q_strncpyz( shader2, UI_Argv( 2 ), sizeof( shader2 ) );
			Q_strncpyz(shader3, UI_Argv(3), sizeof(shader3));
			
			trap_R_RemapShader(shader1, shader2, shader3);
			return qtrue;
		}
	}

	if ( Q_stricmp( cmd, "postgame" ) == 0 ) {
		UI_CalcPostGameStats();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_cache" ) == 0 ) {
		UI_Cache_f();
		return qtrue;
	}

	if ( Q_stricmp( cmd, "ui_teamOrders" ) == 0 ) {
		//UI_TeamOrdersMenu_f();
		return qtrue;
	}


	if ( Q_stricmp( cmd, "ui_cdkey" ) == 0 ) {
		//UI_CDKeyMenu_f();
		return qtrue;
	}

	return qfalse;
}

/*
=================
UI_Shutdown
=================
*/
void UI_Shutdown( void ) {
}

void UI_DrawNamedPic( float x, float y, float width, float height, const char *picname ) {
	qhandle_t hShader;

	hShader = trap_R_RegisterShaderNoMip( picname );
	UI_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 1, 1, hShader );
}

void UI_DrawHandlePic( float x, float y, float w, float h, qhandle_t hShader ) {
	float s0;
	float s1;
	float t0;
	float t1;

	if ( w < 0 ) {   // flip about vertical
		w  = -w;
		s0 = 1;
		s1 = 0;
	} else {
		s0 = 0;
		s1 = 1;
	}

	if ( h < 0 ) {   // flip about horizontal
		h  = -h;
		t0 = 1;
		t1 = 0;
	} else {
		t0 = 0;
		t1 = 1;
	}

	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, h, s0, t0, s1, t1, hShader );
}

/*
================
UI_FillRect

Coordinates are 640*480 virtual values
=================
*/
void UI_FillRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	UI_AdjustFrom640( &x, &y, &width, &height );
	trap_R_DrawStretchPic( x, y, width, height, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );

	trap_R_SetColor( NULL );
}

void UI_DrawSides( float x, float y, float w, float h ) {
	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, 1, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x + w - 1, y, 1, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

void UI_DrawTopBottom( float x, float y, float w, float h ) {
	UI_AdjustFrom640( &x, &y, &w, &h );
	trap_R_DrawStretchPic( x, y, w, 1, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x, y + h - 1, w, 1, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}
/*
================
UI_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void UI_DrawRect( float x, float y, float width, float height, const float *color ) {
	trap_R_SetColor( color );

	UI_DrawTopBottom( x, y, width, height );
	UI_DrawSides( x, y, width, height );

	trap_R_SetColor( NULL );
}

void UI_SetColor( const float *rgba ) {
	trap_R_SetColor( rgba );
}

void UI_UpdateScreen( void ) {
	trap_UpdateScreen();
}


void UI_DrawTextBox( int x, int y, int width, int lines ) {
	UI_FillRect( x + BIGCHAR_WIDTH / 2, y + BIGCHAR_HEIGHT / 2, ( width + 1 ) * BIGCHAR_WIDTH, ( lines + 1 ) * BIGCHAR_HEIGHT, colorBlack );
	UI_DrawRect( x + BIGCHAR_WIDTH / 2, y + BIGCHAR_HEIGHT / 2, ( width + 1 ) * BIGCHAR_WIDTH, ( lines + 1 ) * BIGCHAR_HEIGHT, colorWhite );
}

qboolean UI_CursorInRect( int x, int y, int width, int height ) {
	if ( uiInfo.uiDC.cursorx < x ||
		 uiInfo.uiDC.cursory < y ||
		 uiInfo.uiDC.cursorx > x + width ||
		 uiInfo.uiDC.cursory > y + height ) {
		return qfalse;
	}

	return qtrue;
}
