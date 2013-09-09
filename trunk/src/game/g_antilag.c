#include "g_local.h"

void G_StoreClientPosition( gentity_t* ent ) {
	int top;

	if ( !( ent->inuse &&
			( ent->aiTeam >= 0 ) &&
			ent->r.linked &&
			( ent->health > 0 ) &&
			!( ent->client->ps.pm_flags & PMF_LIMBO ) &&
			( ent->client->ps.pm_type == PM_NORMAL )
			) ) {
		return;
	}

	ent->client->topMarker++;
	if ( ent->client->topMarker >= MAX_CLIENT_MARKERS ) {
		ent->client->topMarker = 0;
	}

	top = ent->client->topMarker;

	VectorCopy( ent->r.mins, ent->client->clientMarkers[top].mins );
	VectorCopy( ent->r.maxs, ent->client->clientMarkers[top].maxs );
	VectorCopy( ent->s.pos.trBase, ent->client->clientMarkers[top].origin );
	ent->client->clientMarkers[top].time = level.time;
}

static void G_AdjustSingleClientPosition( gentity_t* ent, int time ) {
	int i, j;

	if ( time > level.time ) {
		time = level.time;
	} // no lerping forward....

	// find a pair of markers which bound the requested time
	i = j = ent->client->topMarker;
	do {
		if ( ent->client->clientMarkers[i].time <= time ) {
			break;
		}

		j = i;
		i--;
		if ( i < 0 ) {
			i = MAX_CLIENT_MARKERS - 1;
		}
	} while ( i != ent->client->topMarker );

	if ( i == j ) { // oops, no valid stored markers
		return;
	}

	// save current position to backup
	if ( ent->client->backupMarker.time != level.time ) {
		VectorCopy( ent->r.currentOrigin, ent->client->backupMarker.origin );
		VectorCopy( ent->r.mins, ent->client->backupMarker.mins );
		VectorCopy( ent->r.maxs, ent->client->backupMarker.maxs );
		ent->client->backupMarker.time = level.time;
	}

	if ( i != ent->client->topMarker ) {
		float frac = (float)( time - ent->client->clientMarkers[i].time ) /
					 (float)( ent->client->clientMarkers[j].time - ent->client->clientMarkers[i].time );

		LerpPosition( ent->client->clientMarkers[i].origin, ent->client->clientMarkers[j].origin, frac, ent->r.currentOrigin );
		LerpPosition( ent->client->clientMarkers[i].mins, ent->client->clientMarkers[j].mins, frac, ent->r.mins );
		LerpPosition( ent->client->clientMarkers[i].maxs, ent->client->clientMarkers[j].maxs, frac, ent->r.maxs );
	} else {
		VectorCopy( ent->client->clientMarkers[j].origin, ent->r.currentOrigin );
		VectorCopy( ent->client->clientMarkers[j].mins, ent->r.mins );
		VectorCopy( ent->client->clientMarkers[j].maxs, ent->r.maxs );
	}

	trap_LinkEntity( ent );
}

static void G_ReAdjustSingleClientPosition( gentity_t* ent ) {
	if ( !ent || !ent->client ) {
		return;
	}

	// restore from backup
	if ( ent->client->backupMarker.time == level.time ) {
		VectorCopy( ent->client->backupMarker.origin, ent->r.currentOrigin );
		VectorCopy( ent->client->backupMarker.mins, ent->r.mins );
		VectorCopy( ent->client->backupMarker.maxs, ent->r.maxs );
		ent->client->backupMarker.time = 0;

		trap_LinkEntity( ent );
	}
}

void G_AdjustClientPositions( gentity_t* ent, int time, qboolean forward ) {
	int i;
	gentity_t   *list;

	for ( i = 0; i < level.numConnectedClients; i++, list++ ) {
		list = g_entities + level.sortedClients[i];
		// Gordon: ok lets test everything under the sun
		if ( list->client &&
			 list->inuse &&
			 ( list->aiTeam >= 0 ) &&
			 ( list != ent ) &&
			 list->r.linked &&
			 ( list->health > 0 ) &&
			 !( list->client->ps.pm_flags & PMF_LIMBO ) &&
			 ( list->client->ps.pm_type == PM_NORMAL )
			 ) {
			if ( forward ) {
				G_AdjustSingleClientPosition( list, time );
			} else {
				G_ReAdjustSingleClientPosition( list );
			}
		}
	}
}

void G_ResetMarkers( gentity_t* ent ) {
	int i, time;
	char buffer[ MAX_CVAR_VALUE_STRING ];
	float period;

	trap_Cvar_VariableStringBuffer( "sv_fps", buffer, sizeof( buffer ) - 1 );

	period = atoi( buffer );
	if ( !period ) {
		period = 50;
	} else {
		period = 1000.f / period;
	}

	ent->client->topMarker = MAX_CLIENT_MARKERS - 1;
	for ( i = MAX_CLIENT_MARKERS - 1, time = level.time; i >= 0; i--, time -= period ) {
		VectorCopy( ent->r.mins, ent->client->clientMarkers[i].mins );
		VectorCopy( ent->r.maxs, ent->client->clientMarkers[i].maxs );
		VectorCopy( ent->r.currentOrigin, ent->client->clientMarkers[i].origin );
		ent->client->clientMarkers[i].time = time;
	}
}

void G_AttachBodyParts( gentity_t* ent ) {
	int i;
	gentity_t   *list;

	for ( i = 0; i < level.numConnectedClients; i++, list++ ) {
		list = g_entities + level.sortedClients[i];
		// Gordon: ok lets test everything under the sun
		if ( list->inuse &&
			 ( list->aiTeam >= 0 ) &&
			 ( list != ent ) &&
			 list->r.linked &&
			 ( list->health > 0 ) &&
			 !( list->client->ps.pm_flags & PMF_LIMBO ) &&
			 ( list->client->ps.pm_type == PM_NORMAL )
			 ) {
			list->client->tempHead = G_BuildHead( list );
		} else {
			list->client->tempHead = NULL;
		}
	}
}

void G_DettachBodyParts( void ) {
	int i;
	gentity_t   *list;

	for ( i = 0; i < level.numConnectedClients; i++, list++ ) {
		list = g_entities + level.sortedClients[i];
		if ( list->client->tempHead ) {
			G_FreeEntity( list->client->tempHead );
		}
	}
}

int G_SwitchBodyPartEntity( gentity_t* ent ) {
	if ( ent->s.eType == ET_TEMPHEAD ) {
		return ent->parent - g_entities;
	}
	return ent - g_entities;
}

#define POSITION_READJUST						\
	if ( res != results->entityNum ) {				 \
		VectorSubtract( end, start, dir );			\
		VectorNormalizeFast( dir );				\
									\
		VectorMA( results->endpos, -1, dir, results->endpos );	\
		results->entityNum = res;				\
	}

// Run a trace with players in historical positions.
void G_HistoricalTrace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	int res;
	vec3_t dir;

	if ( !g_antilag.integer || !ent->client || g_gametype.integer == GT_SINGLE_PLAYER ) {
		G_AttachBodyParts( ent );

		trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );

		res = G_SwitchBodyPartEntity( &g_entities[ results->entityNum ] );
		POSITION_READJUST

		G_DettachBodyParts();
		return;
	}

	G_AdjustClientPositions( ent, ent->client->pers.cmd.serverTime, qtrue );

	G_AttachBodyParts( ent ) ;

	trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );

	res = G_SwitchBodyPartEntity( &g_entities[ results->entityNum ] );
	POSITION_READJUST

	G_DettachBodyParts();

	G_AdjustClientPositions( ent, 0, qfalse );
}

void G_HistoricalTraceBegin( gentity_t *ent ) {
	G_AdjustClientPositions( ent, ent->client->pers.cmd.serverTime, qtrue );
}

void G_HistoricalTraceEnd( gentity_t *ent ) {
	G_AdjustClientPositions( ent, 0, qfalse );
}

//bani - Run a trace without fixups (historical fixups will be done externally)
void G_Trace( gentity_t* ent, trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	int res;
	vec3_t dir;

	G_AttachBodyParts( ent );

	trap_Trace( results, start, mins, maxs, end, passEntityNum, contentmask );

	res = G_SwitchBodyPartEntity( &g_entities[ results->entityNum ] );
	POSITION_READJUST

	G_DettachBodyParts();
}
