// g_coop.c
/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

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

#include "g_local.h"
#include "g_coop.h"

// fretn - set weapons when player spawns in coop
// in the map dam, the player gets all the weapons, bug !
void SetCoopSpawnWeapons( gclient_t *client ) {

        int pc = client->sess.playerType;
        client->ps.powerups[PW_INVULNERABLE] = level.time + 5000; // some time to find cover

        // zero out all ammo counts
        memset( client->ps.ammo,MAX_WEAPONS,sizeof( int ) ); 

        // Communicate it to cgame
        client->ps.stats[STAT_PLAYER_CLASS] = pc;

        // Abuse teamNum to store player class as well (can't see stats for all clients in cgame)
        client->ps.teamNum = pc;

        // All players start with a knife (not OR-ing so that it clears previous weapons)
        client->ps.weapons[0] = 0; 
        client->ps.weapons[1] = 0; 

        COM_BitSet( client->ps.weapons, WP_KNIFE );
        client->ps.ammo[BG_FindAmmoForWeapon( WP_KNIFE )] = 1; 
        //client->ps.weapon = WP_KNIFE;
     
        client->ps.weaponstate = WEAPON_READY;

        // give all the players a binocular
        client->ps.stats[STAT_KEYS] |= ( 1 << INV_BINOCS );
}


void CoopInfoMessage( gentity_t *ent ) {
        char entry[1024];
        char string[1400];
        int stringlength;
        int i, j;
        gentity_t   *player;
        int cnt; 
        int h, a;

        // send the latest information on all clients
        string[0] = 0; 
        stringlength = 0; 

        for ( i = 0, cnt = 0; i < level.numConnectedClients && cnt < TEAM_MAXOVERLAY; i++ ) {
                player = g_entities + level.sortedClients[i];
                if ( player->inuse && player->client->sess.sessionTeam ==
                         ent->client->sess.sessionTeam && !(player->r.svFlags & SVF_CASTAI)) {

                        h = player->client->ps.stats[STAT_HEALTH];
                        a = player->client->ps.stats[STAT_ARMOR];
                        if ( h < 0 ) {
                                h = 0; 
                        }    

                        if ( a < 0 ) {
                                a = 0; 
                        }    
                        // what do we want: clientnum, health, weapon, current amount of ammo
                        Com_sprintf( entry, sizeof( entry ),
                                                 " %i %i %i %i %i %i %i",
                                                 level.sortedClients[i], player->client->pers.teamState.location, h, a,
                                                 player->client->ps.weapon, player->client->ps.ammoclip[BG_FindClipForWeapon( player->client->ps.weapon )], player->client->ps.ammo[BG_FindAmmoForWeapon( player->client->ps.weapon )]);
                        j = strlen( entry );
                        if ( stringlength + j > sizeof( string ) ) {
                                break;
                        }    
                        strcpy( string + stringlength, entry );
                        stringlength += j;
                        cnt++;
                }    
        }    

        trap_SendServerCommand( ent - g_entities, va( "coopinfo %i%s", cnt, string ) );
}

/*
===========
Coop_GetLocation

Report a location for the player. Uses placed nearby target_location entities
============
*/
gentity_t *Coop_GetLocation( gentity_t *ent ) {
        gentity_t       *eloc, *best;
        float bestlen, len;
        vec3_t origin;

        best = NULL;
        bestlen = 3 * 8192.0 * 8192.0;

        VectorCopy( ent->r.currentOrigin, origin );

        for ( eloc = level.locationHead; eloc; eloc = eloc->nextTrain ) {
                len = ( origin[0] - eloc->r.currentOrigin[0] ) * ( origin[0] - eloc->r.currentOrigin[0] )
                          + ( origin[1] - eloc->r.currentOrigin[1] ) * ( origin[1] - eloc->r.currentOrigin[1] )
                          + ( origin[2] - eloc->r.currentOrigin[2] ) * ( origin[2] - eloc->r.currentOrigin[2] );

                if ( len > bestlen ) {
                        continue;
                }

                if ( !trap_InPVS( origin, eloc->r.currentOrigin ) ) {
                        continue;
                }

                bestlen = len;
                best = eloc;
        }

        return best;
}

void CheckCoopStatus( void ) {
        int i;
        gentity_t *loc, *ent;

        if ( level.time - level.lastTeamLocationTime > TEAM_LOCATION_UPDATE_TIME ) {

                level.lastTeamLocationTime = level.time;

                for ( i = 0; i < g_maxclients.integer; i++ ) {
                        ent = g_entities + i;
                        if ( ent->inuse && !(ent->r.svFlags & SVF_CASTAI) ) {
                                loc = Coop_GetLocation( ent );
                                if ( loc ) {
                                        ent->client->pers.teamState.location = loc->health;
                                } else {
                                        ent->client->pers.teamState.location = 0; 
                                }
                                CoopInfoMessage( ent );
                        }
                }
        }
}

/*
================
SelectRandomCoopSpawnPoint

go to a random point that doesn't telefrag
================
*/
#define MAX_SPAWN_POINTS    128
gentity_t *SelectRandomCoopSpawnPoint( vec3_t origin, vec3_t angles ) {
        gentity_t   *spot;
        int count;
        int selection;
        gentity_t   *spots[MAX_SPAWN_POINTS];

        count = 0; 
        spot = NULL;

        while ( ( spot = G_Find( spot, FOFS( classname ), "info_player_coop" ) ) != NULL ) {
                if ( SpotWouldTelefrag( spot ) ) {
                        continue;
                }    
                spots[ count ] = spot;
                count++;
        }    

        if ( !count ) { // no spots that won't telefrag
                spot = G_Find( NULL, FOFS( classname ), "info_player_coop" );

                if (spot)
                {
                        VectorCopy( spot->s.origin, origin );
                        origin[2] += 9;
                        VectorCopy( spot->s.angles, angles );

                        return spot;
                }
                else
                {
                        return NULL;
                }
        }    

        selection = rand() % count;
        spot = spots[ selection ];

        VectorCopy( spot->s.origin, origin );
        origin[2] += 9;
        VectorCopy( spot->s.angles, angles );

        return spot;
}

