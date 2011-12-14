// g_coop.h

void SetCoopSpawnWeapons( gclient_t *client );
void CoopInfoMessage( gentity_t *ent );
void CheckCoopStatus( void );
gentity_t *SelectRandomCoopSpawnPoint( vec3_t origin, vec3_t angles );
