#include "cg_local.h"


//////////////////////////////////////////////////////////////////////////

const int OMNIBOT_LINES_INCREASE_SIZE = 512;
const int OMNIBOT_LINES_INITIAL_SIZE = 4096;

//////////////////////////////////////////////////////////////////////////

typedef enum
{
	DBG_LINE,
	DBG_RADIUS,
	DBG_AABB,
	DBG_POLYGON,
} DebugShapeType;

typedef struct
{
	vec3_t start, end;
	unsigned char r,g,b,a;
} UdpDebugLineMessage;

typedef struct
{
	vec3_t pos;
	float radius;
	unsigned char r,g,b,a;
} UdpDebugRadiusMessage;

typedef struct
{
	vec3_t origin;
	vec3_t mins, maxs;
	unsigned char r,g,b,a;
	int side;
} UdpDebugAABBMessage;

typedef struct
{
	vec3_t verts[64];
	int numverts;
	unsigned char r,g,b,a;
} UdpDebugPolygonMessage;

typedef struct
{
	union
	{
		UdpDebugLineMessage line;
		UdpDebugRadiusMessage radius;
		UdpDebugAABBMessage aabb;
		UdpDebugPolygonMessage poly;
	} info;

	int expiretime;
	int debugtype;
} UdpDebugLines_t;

//////////////////////////////////////////////////////////////////////////

typedef struct
{
	UdpDebugLines_t     *m_pDebugLines;
	int m_NumDebugLines;
	int m_MaxDebugLines;
} LineList;

void CG_DrawDebugLine( UdpDebugLineMessage *_lineinfo );
void CG_DrawDebugRadius( UdpDebugRadiusMessage *_lineinfo );
void CG_DrawDebugAABB( UdpDebugAABBMessage *_aabb );
void CG_DrawDebugPolygon( UdpDebugPolygonMessage *_poly );

// Utility functions to manage the line list.
void AddToLineList( LineList *_list, const UdpDebugLines_t *_line );
void ClearLineList( LineList *_list, qboolean _freememory );
void InitLineList( LineList *_list, int _initialsize );

int g_LastRenderTime = 0;
LineList g_DebugLines = { 0,0,0 };

//////////////////////////////////////////////////////////////////////////

void OmnibotDisableDrawing( void ) {
	ClearLineList( &g_DebugLines, qtrue );
}

//////////////////////////////////////////////////////////////////////////

void OmnibotRenderDebugLines( void ) {
	int i;

	for ( i = 0; i < g_DebugLines.m_NumDebugLines; ++i )
	{
		if ( g_DebugLines.m_pDebugLines[i].expiretime >= cg.time ) {
			switch ( g_DebugLines.m_pDebugLines[i].debugtype )
			{
			case DBG_LINE:
				CG_DrawDebugLine( &g_DebugLines.m_pDebugLines[i].info.line );
				break;
			case DBG_RADIUS:
				CG_DrawDebugRadius( &g_DebugLines.m_pDebugLines[i].info.radius );
				break;
			case DBG_AABB:
				CG_DrawDebugAABB( &g_DebugLines.m_pDebugLines[i].info.aabb );
				break;
			case DBG_POLYGON:
				CG_DrawDebugPolygon( &g_DebugLines.m_pDebugLines[i].info.poly );
				break;
			}

			g_LastRenderTime = cg.time + 10000;
		} else {
			// swap with the last and reduce the num
			g_DebugLines.m_pDebugLines[i] = g_DebugLines.m_pDebugLines[g_DebugLines.m_NumDebugLines - 1];
			--g_DebugLines.m_NumDebugLines;
		}
	}

	// After a timeout, free the memory.
	if ( g_LastRenderTime < cg.time && g_DebugLines.m_pDebugLines ) {
		ClearLineList( &g_DebugLines, qtrue );
	}
}

void CG_DrawDebugLine( UdpDebugLineMessage *_lineinfo ) {
	vec3_t forward, right;
	polyVert_t verts[4];
	vec3_t line;

	float fLineWidth = 2.0f;

	//////////////////////////////////////////////////////////////////////////
	// Check the distance.
	{
		const float fRenderDistanceSq = 2048.f * 2048.f;
		float fLen1, fLen2;
		vec3_t toline1, toline2;
		VectorSubtract( cg_entities[0].currentState.pos.trBase, _lineinfo->start, toline1 );
		VectorSubtract( cg_entities[0].currentState.pos.trBase, _lineinfo->end, toline2 );
		fLen1 = VectorLengthSquared( toline1 );
		fLen2 = VectorLengthSquared( toline2 );
		if ( fLen1 > fRenderDistanceSq &&
			 fLen2 > fRenderDistanceSq ) {
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	VectorSubtract( _lineinfo->end, _lineinfo->start, forward );

	line[0] = DotProduct( forward, cg.refdef.viewaxis[1] );
	line[1] = DotProduct( forward, cg.refdef.viewaxis[2] );

	VectorScale( cg.refdef.viewaxis[1], line[1], right );
	VectorMA( right, -line[0], cg.refdef.viewaxis[2], right );
	VectorNormalize( right );

	//////////////////////////////////////////////////////////////////////////
	// Why can't this POS draw black?
	{
		static int iFixBlack = 0;
		if ( iFixBlack ) {
			if ( _lineinfo->r == 0 && _lineinfo->g == 0 && _lineinfo->b == 0 ) {
				_lineinfo->r = 65;
				_lineinfo->g = 65;
				_lineinfo->b = 65;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	VectorMA( _lineinfo->end, fLineWidth, right, verts[0].xyz );
	verts[0].st[0] = 1;
	verts[0].st[1] = 1;
	verts[0].modulate[0] = _lineinfo->r;
	verts[0].modulate[1] = _lineinfo->g;
	verts[0].modulate[2] = _lineinfo->b;
	verts[0].modulate[3] = _lineinfo->a;

	VectorMA( _lineinfo->end, -fLineWidth, right, verts[1].xyz );
	verts[1].st[0] = 1;
	verts[1].st[1] = 0;
	verts[1].modulate[0] = _lineinfo->r;
	verts[1].modulate[1] = _lineinfo->g;
	verts[1].modulate[2] = _lineinfo->b;
	verts[1].modulate[3] = _lineinfo->a;

	VectorMA( _lineinfo->start, -fLineWidth, right, verts[2].xyz );
	verts[2].st[0] = 0;
	verts[2].st[1] = 0;
	verts[2].modulate[0] = _lineinfo->r;
	verts[2].modulate[1] = _lineinfo->g;
	verts[2].modulate[2] = _lineinfo->b;
	verts[2].modulate[3] = _lineinfo->a;

	VectorMA( _lineinfo->start, fLineWidth, right, verts[3].xyz );
	verts[3].st[0] = 0;
	verts[3].st[1] = 1;
	verts[3].modulate[0] = _lineinfo->r;
	verts[3].modulate[1] = _lineinfo->g;
	verts[3].modulate[2] = _lineinfo->b;
	verts[3].modulate[3] = _lineinfo->a;

	trap_R_AddPolyToScene( cgs.media.railCoreShader, 4, verts );
}

void CG_DrawDebugRadius( UdpDebugRadiusMessage *_lineinfo ) {
	UdpDebugLineMessage lne;
	const int iNumSteps = 8;
	float fStepSize = 180.0f / (float)iNumSteps;
	int i;

	if ( !cgs.media.railCoreShader ) {
		cgs.media.railCoreShader = trap_R_RegisterShader( "railCore" );
	}

	lne.r = _lineinfo->r;
	lne.g = _lineinfo->g;
	lne.b = _lineinfo->b;
	lne.a = _lineinfo->a;

	VectorCopy( _lineinfo->pos, lne.start );
	VectorCopy( _lineinfo->pos, lne.end );
	lne.start[1] += _lineinfo->radius;
	lne.end[1] -= _lineinfo->radius;

	for ( i = 0; i < iNumSteps; ++i )
	{
		CG_DrawDebugLine( &lne );

		RotatePointAroundVertex( lne.start, 0.0f, 0.0f, fStepSize, _lineinfo->pos );
		RotatePointAroundVertex( lne.end, 0.0f, 0.0f, fStepSize, _lineinfo->pos );
	}
}

void CG_DrawDebugAABB( UdpDebugAABBMessage *_aabbinfo ) {
	UdpDebugLineMessage l;
	vec3_t vVertex[8];

	vVertex[0][0] = _aabbinfo->origin[0] + _aabbinfo->mins[0];
	vVertex[0][1] = _aabbinfo->origin[1] + _aabbinfo->mins[1];
	vVertex[0][2] = _aabbinfo->origin[2] + _aabbinfo->mins[2];

	vVertex[1][0] = _aabbinfo->origin[0] + _aabbinfo->maxs[0];
	vVertex[1][1] = _aabbinfo->origin[1] + _aabbinfo->mins[1];
	vVertex[1][2] = _aabbinfo->origin[2] + _aabbinfo->mins[2];

	vVertex[2][0] = _aabbinfo->origin[0] + _aabbinfo->maxs[0];
	vVertex[2][1] = _aabbinfo->origin[1] + _aabbinfo->maxs[1];
	vVertex[2][2] = _aabbinfo->origin[2] +  _aabbinfo->mins[2];

	vVertex[3][0] = _aabbinfo->origin[0] + _aabbinfo->mins[0];
	vVertex[3][1] = _aabbinfo->origin[1] + _aabbinfo->maxs[1];
	vVertex[3][2] = _aabbinfo->origin[2] + _aabbinfo->mins[2];

	vVertex[4][0] = _aabbinfo->origin[0] + _aabbinfo->mins[0];
	vVertex[4][1] = _aabbinfo->origin[1] + _aabbinfo->mins[1];
	vVertex[4][2] = _aabbinfo->origin[2] + _aabbinfo->maxs[2];

	vVertex[5][0] = _aabbinfo->origin[0] + _aabbinfo->maxs[0];
	vVertex[5][1] = _aabbinfo->origin[1] + _aabbinfo->mins[1];
	vVertex[5][2] = _aabbinfo->origin[2] + _aabbinfo->maxs[2];

	vVertex[6][0] = _aabbinfo->origin[0] + _aabbinfo->maxs[0];
	vVertex[6][1] = _aabbinfo->origin[1] + _aabbinfo->maxs[1];
	vVertex[6][2] = _aabbinfo->origin[2] + _aabbinfo->maxs[2];

	vVertex[7][0] = _aabbinfo->origin[0] + _aabbinfo->mins[0];
	vVertex[7][1] = _aabbinfo->origin[1] + _aabbinfo->maxs[1];
	vVertex[7][2] = _aabbinfo->origin[2] + _aabbinfo->maxs[2];

	l.r = _aabbinfo->r;
	l.g = _aabbinfo->g;
	l.b = _aabbinfo->b;
	l.a = _aabbinfo->a;

	// Top
	if ( _aabbinfo->side == 4 || _aabbinfo->side == 6 ) {
		{
			VectorCopy( vVertex[4], l.start );
			VectorCopy( vVertex[5], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[5], l.start );
			VectorCopy( vVertex[6], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[6], l.start );
			VectorCopy( vVertex[7], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[7], l.start );
			VectorCopy( vVertex[4], l.end );
			CG_DrawDebugLine( &l );
		}
	}

	// Bottom
	if ( _aabbinfo->side == 5 || _aabbinfo->side == 6 ) {
		{
			VectorCopy( vVertex[0], l.start );
			VectorCopy( vVertex[1], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[1], l.start );
			VectorCopy( vVertex[2], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[2], l.start );
			VectorCopy( vVertex[3], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[3], l.start );
			VectorCopy( vVertex[0], l.end );
			CG_DrawDebugLine( &l );
		}
	}

	// Sides
	if ( _aabbinfo->side == 6 ) {
		{
			VectorCopy( vVertex[4], l.start );
			VectorCopy( vVertex[0], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[5], l.start );
			VectorCopy( vVertex[1], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[6], l.start );
			VectorCopy( vVertex[2], l.end );
			CG_DrawDebugLine( &l );
		}
		{
			VectorCopy( vVertex[7], l.start );
			VectorCopy( vVertex[3], l.end );
			CG_DrawDebugLine( &l );
		}
	}
}

void CG_DrawDebugPolygon( UdpDebugPolygonMessage *_polyinfo ) {
	int i = 0;
	polyVert_t verts[65];

	if ( !_polyinfo || _polyinfo->numverts < 1 ) {
		return;
	}

	for (; i < _polyinfo->numverts; ++i )
	{
		verts[i].xyz[0] = _polyinfo->verts[_polyinfo->numverts - i - 1][0];
		verts[i].xyz[1] = _polyinfo->verts[_polyinfo->numverts - i - 1][1];
		verts[i].xyz[2] = _polyinfo->verts[_polyinfo->numverts - i - 1][2];

		verts[i].st[0] = 0;
		verts[i].st[1] = 0;
		verts[i].modulate[0] = _polyinfo->r;
		verts[i].modulate[1] = _polyinfo->g;
		verts[i].modulate[2] = _polyinfo->b;
		verts[i].modulate[3] = _polyinfo->a;
	}
	verts[i].xyz[0] = _polyinfo->verts[_polyinfo->numverts - 1][0];
	verts[i].xyz[1] = _polyinfo->verts[_polyinfo->numverts - 1][1];
	verts[i].xyz[2] = _polyinfo->verts[_polyinfo->numverts - 1][2];

	for (; i < _polyinfo->numverts; ++i )
	{
		verts[i].xyz[0] = _polyinfo->verts[i][0];
		verts[i].xyz[1] = _polyinfo->verts[i][1];
		verts[i].xyz[2] = _polyinfo->verts[i][2];

		verts[i].st[0] = 0;
		verts[i].st[1] = 0;
		verts[i].modulate[0] = _polyinfo->r;
		verts[i].modulate[1] = _polyinfo->g;
		verts[i].modulate[2] = _polyinfo->b;
		verts[i].modulate[3] = _polyinfo->a;
	}
	verts[i].xyz[0] = _polyinfo->verts[0][0];
	verts[i].xyz[1] = _polyinfo->verts[0][1];
	verts[i].xyz[2] = _polyinfo->verts[0][2];

	trap_R_AddPolyToScene( cgs.media.whiteShader, i, verts );
}

//////////////////////////////////////////////////////////////////////////

void SetupLineList( void ) {
	InitLineList( &g_DebugLines, 1000 );
}

void InitLineList( LineList *_list, int _initialsize ) {
	if ( _list->m_pDebugLines ) {
		free( _list->m_pDebugLines );
		_list->m_pDebugLines = 0;
	}

	_list->m_NumDebugLines = 0;
	_list->m_MaxDebugLines = _initialsize;
	_list->m_pDebugLines = (UdpDebugLines_t*)calloc( _list->m_MaxDebugLines, sizeof( UdpDebugLines_t ) );
}

void AddToLineList( LineList *_list, const UdpDebugLines_t *_line ) {
	if ( !_list || !_list->m_pDebugLines ) {
		InitLineList( &g_DebugLines, 1000 );
		//return;
	}

	if ( _list->m_NumDebugLines >= _list->m_MaxDebugLines ) {
		// We've gone over, so we need to reallocate.
		int iNewBufferSize = _list->m_MaxDebugLines + OMNIBOT_LINES_INCREASE_SIZE;
		UdpDebugLines_t *pNewBuffer = (UdpDebugLines_t*)calloc( iNewBufferSize, sizeof( UdpDebugLines_t ) );

		// Copy the old buffer to the new.
		memcpy( pNewBuffer, _list->m_pDebugLines, sizeof( UdpDebugLines_t ) * _list->m_MaxDebugLines );

		// Free the old buffer.
		free( _list->m_pDebugLines );

		// Update the pointer to the new buffer.
		_list->m_pDebugLines = pNewBuffer;

		// Save the new size.
		_list->m_MaxDebugLines = iNewBufferSize;
	}

	// Add it to the list.
	_list->m_pDebugLines[_list->m_NumDebugLines] = *_line;
	++_list->m_NumDebugLines;
}

void ClearLineList( LineList *_list, qboolean _freememory ) {
	if ( _freememory == qfalse ) {
		_list->m_NumDebugLines = 0;
	} else
	{
		free( _list->m_pDebugLines );
		_list->m_pDebugLines = 0;
		_list->m_NumDebugLines = 0;
		_list->m_MaxDebugLines = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// Interprocess callback functions

void DrawDebugLine( float *_start, float *_end, int _duration, vec4_t _color ) {
	int i;
	UdpDebugLines_t lne;

	lne.debugtype = DBG_LINE;
	lne.expiretime = cg.time + _duration;

	for ( i = 0; i < 3; ++i )
	{
		lne.info.line.start[i] = _start[i];
		lne.info.line.end[i] = _end[i];
	}

	lne.info.line.r = _color[0] * 255;
	lne.info.line.g = _color[1] * 255;
	lne.info.line.b = _color[2] * 255;
	lne.info.line.a = _color[3] * 255;

	AddToLineList( &g_DebugLines, &lne );
}

void DrawDebugRadius( float *_start, float _radius, int _duration, vec4_t _color ) {
	int i;
	UdpDebugLines_t lne;

	lne.debugtype = DBG_RADIUS;
	lne.expiretime = cg.time + _duration;

	for ( i = 0; i < 3; ++i )
		lne.info.radius.pos[i] = _start[i];
	lne.info.radius.radius = _radius;

	lne.info.radius.r = _color[0] * 255;
	lne.info.radius.g = _color[1] * 255;
	lne.info.radius.b = _color[2] * 255;
	lne.info.radius.a = _color[3] * 255;

	AddToLineList( &g_DebugLines, &lne );
}

void DrawDebugAABB( vec3_t origin, float *mins, float *_maxs, int _duration, vec4_t _color, int _side ) {
	int i;
	UdpDebugLines_t lne;

	lne.debugtype = DBG_AABB;
	lne.expiretime = cg.time + _duration;

	for ( i = 0; i < 3; ++i )
	{
		lne.info.aabb.mins[i] = mins[i];
		lne.info.aabb.maxs[i] = _maxs[i];
	}

	lne.info.aabb.r = _color[0] * 255;
	lne.info.aabb.g = _color[1] * 255;
	lne.info.aabb.b = _color[2] * 255;
	lne.info.aabb.a = _color[3] * 255;
	lne.info.aabb.side = _side;

	VectorCopy( origin, lne.info.aabb.origin );

	AddToLineList( &g_DebugLines, &lne );
}

void DrawDebugPolygon( vec3_t *verts, int _numverts, int _duration, vec4_t _color ) {
	int i;
	UdpDebugLines_t lne;

	lne.debugtype = DBG_POLYGON;
	lne.expiretime = cg.time + _duration;

	lne.info.poly.numverts = _numverts;
	for ( i = 0; i < _numverts; ++i )
	{
		lne.info.poly.verts[i][0] = verts[i][0];
		lne.info.poly.verts[i][1] = verts[i][1];
		lne.info.poly.verts[i][2] = verts[i][2];
	}

	lne.info.poly.r = _color[0] * 255;
	lne.info.poly.g = _color[1] * 255;
	lne.info.poly.b = _color[2] * 255;
	lne.info.poly.a = _color[3] * 255;

	AddToLineList( &g_DebugLines, &lne );
}

int cg_LastScreenMessageTime = 0;             // ensiform's fix for fpinfo render

// ensiform's updated func to fix fpinfo
void DrawDebugText( float *_start, const char *_msg, int _duration, vec4_t _color ) {
	if ( _start && !VectorCompare( _start, vec3_origin ) ) {
		vec3_t v3;
		VectorCopy( _start,v3 );
		CG_AddOnScreenText( _msg,v3,_color,(float)_duration / 1000.f );
	} else
	{
		if ( cg_LastScreenMessageTime != cg.time ) {
			trap_R_SetColor( _color );
			CPri( _msg );
			cg_LastScreenMessageTime = cg.time;
			trap_R_SetColor( NULL );
		}
	}
}
