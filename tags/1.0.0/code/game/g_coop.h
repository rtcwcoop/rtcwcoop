// g_coop.h

#define AXIS_OBJECTIVE      1
#define ALLIED_OBJECTIVE    2
#define OBJECTIVE_DESTROYED 4

void SetCoopSpawnWeapons( gclient_t *client );
void CoopInfoMessage( gentity_t *ent );
void CheckCoopStatus( void );
gentity_t *SelectRandomCoopSpawnPoint( vec3_t origin, vec3_t angles );
gentity_t *SelectRandomAntiCoopSpawnPoint( gentity_t *ent, vec3_t origin, vec3_t angles );
void Coop_DeleteStats( int clientnum );
void Coop_AddStats( gentity_t *targ, gentity_t *attacker, int dmg_ref, int mod );
