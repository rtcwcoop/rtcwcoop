# Introduction #

Copy all your wolfenstein pk3 files to the main folder of this game.
You also have to create our pk3 file. Do this with create\_pk3.sh (for linux and mac) or with create\_pk3.bat for windows.

The pk3 will be placed inside the media folder, copy the pk3's of this folder into your main folder

You can start a server through the menus.
Or by using the console:

computer 1: start game with /coopmap **mapname**

computer 2: type /connect **ip**, once you see the introduction screen type /playerstart, or press the arrow
on the bottom right corner.

On windows you have to connect to the ip 127.0.0.1 if you want to connect to a local server.
On Mac OS X its 0.0.0.0 that you need to connect to.

Or just use the menus.

# Callvote #

When playing on a server you can change setting by calling a vote. Some settings need a map restart before they are effective. So don't forget to callvote for a map restart after you change certain cvars

# Console #

use shift-escape to open the console

# PK3 Files #

Copy all your wolfenstein pk3 files to the main folder of this game.

The game was developed with following pk3's:
```
mp_pak0.pk3
mp_pak1.pk3
mp_pak2.pk3
mp_pak3.pk3
mp_pak4.pk3
mp_pakmaps0.pk3
mp_pakmaps1.pk3
mp_pakmaps2.pk3
mp_pakmaps3.pk3
mp_pakmaps4.pk3
mp_pakmaps5.pk3
mp_pakmaps6.pk3
pak0.pk3
sp_pak1.pk3
sp_pak2.pk3
sp_pak3.pk3
sp_pak4.pk3
sp_pak_coop1.pk3
```

I strongly recommend using these and only these, except for custom maps. Don't use pk3's that use different player models, this will probably break the coop game.
And for server admins, use these pk3's and run a pure server. (set sv\_pure 1)

# Cvars #

## cg\_fancyfx ##
  * cg\_fancyfx 1 enables new graphical effects, currently this is only the possible greyscale stuff. This is to make sure that unsupported things on older computers don't crash the game, this cvar is off by default

## g\_spawnpoints ##
  * g\_spawnpoints 0 = autosaves your spawnpoint every 30 seconds
  * g\_spawnpoints 1 = you can save your spawn point with the command: spawnpoint, bind x spawnpoint to assign it to  a button.
  * g\_spawnpoints 2 = will result in a multiplayer style flagzone respawn

## g\_maxspawnpoints ##
  * Limits the number of user droppable spawnpoints, to make the game more difficult. If you set it to zero then there is no limit at all

## cg\_drawcrosshairnames ##
  * Set to 1 if you want to see info about your teamplayers: playername, ammo, ammoclip and health, its like in the multiplayer.
  * Set to 2 if you also want to see a bar that shows the armor, just like the health bar, but  below it

## g\_airespawn ##
  * Set to -1 to give the ai's unlimited respawns
_Reinforcements will always respawn_
  * Set to 0 to disable ai respawning
  * Set to x to give the ai's x number of respawns
_Reinforcements will respawn randomly with 1/2 chance_
  * Keep in mind that not all ai's will respawn

## g\_teleporttime ##
  * Sets the minimum time between two teleports. In the Battle gametype its fixed at 30000 (30 seconds).

## r\_greyscale ##
  * set to 1 and everything is rendered in greyscale
  * set it to a float value, eg: 0.5, then its 50 percent grey.
  * set it to 2 and only the world (not the hud and the menus) is rendered to greyscale (via rendertotexture)  when you are dead or in limbo


## cg\_drawteamoverlay ##
  * Set to 1 to see playerweapon, playername and health
  * Set to 2 to see playerweapon, playername, location and health

> The playerweapon is just one character, K (knife), P (pistol), S (submachinegun), E (explosive), R (rifle), H (heavy weapon).

> See screenshot below:

> ![http://bzzwolfsp.googlecode.com/svn/trunk/screenshots/rtcw_coop_teamoverlay.jpg](http://bzzwolfsp.googlecode.com/svn/trunk/screenshots/rtcw_coop_teamoverlay.jpg)

## g\_freeze ##
freezes freshly respawn players, they need to be activated by a stab of a knife of a friend

## g\_gameskill ##
  * In a coop game the aim\_accuracy, aim\_skill, attack\_skill and health of the aicast is raised by raising g\_gameskill ( 0 - 2 ).
  * In single player these are set in the mapname.ai script files for each aicast, the coop mode ignores these  values in the scripts.

## sv\_maxcoopclients ##
Limits the number of coop players in a game. sv\_maxclients still exists and should be set to 64. Don't lower it because the AI's also count as clients. sv\_maxcoopclients shouldn't be higher than sv\_maxclients - numberofai and its limited between 1 and 8

## sv\_dlrate ##
Limits the speed of the ingame downloads, is set to 100 (KByte/sec) by default. In theory it cannot reach more than 1MByte/sec

## g\_maxlives ##
The number of the times a player can die before he can't play anymore. Set to 0 to disable it.

## g\_sharedlives ##
If set to 1 then the g\_maxlives is a teamvalue

## cg\_drawcompass ##
1 or 0 Toggles the compass

## g\_friendlyfire ##
  * value 0: you can't hit friends
  * value 1: you can hit friends
  * value 2: you can hit friends but you loose the health instead (also score goes down)
  * value 3: you can hit friends but you loose the health instead (and also twice the score)
friends are: civilians and the other coop players

## g\_limbotime ##
When playing the Speedrun gametype, you respawn at fixed intervals, this interval is set by g\_limbotime, default value is 30000 (30 seconds).

## g\_reinforce ##
This cvar controls the amount of enemies. Values are 0, 1 or 2. The higher the number the more enemies that spawn. We handle this in the .ents, .ai and .script files

# Cmds #
## +dropweapon ##
> drop your current weapon so your coop friend can pick it up

## dropammo ##
> drop your current ammo clip

## dumploc ##
> dumploc <location name>

> Writes the target\_location entity with location name to your .ents file

## playerstart ##
> Use this if you are stuck at the loading screen.

## coopmap ##
> Starts a coop game

## coopdevmap ##
> starts a coop game with cheats enabled

## spawnpoint ##
> sets current location as the next spawnpoint. Only works if g\_autospawn is disabled on the server

## dumpaicast ##
> dumpaicast classname ainame

> where classname is one of these:

```
ai_soldier ai_american ai_zombie ai_warzombie ai_venom ai_loper ai_boss_helga ai_boss_heinrich ai_eliteguard ai_stimsoldier_dual ai_stimsoldier_rocket ai_stimsoldier_tesla ai_supersoldier ai_protosoldier ai_frogman ai_blackguard ai_partisan ai_civilian
```

> this writes for example an ai\_soldier entity to a file mapname.ents, copy the text from this file to mapname.ents to spawn a new aicast entity into the map, you can also script this new ai in the mapname.ai file

## dumpcoopspawnpoint ##
> This dumps a info\_player\_coop entity to the mapname.ents file
> This entity is used as an initial spawnpoint for coop games. If there are multiple of this one, a random entity is chosen. Make sure if a player spawns on this place he triggers the ai scripts when he starts walking

## drawspawns ##
## drawtriggers ##
> These commands draw the spawnpoints and the triggers

## teleport ##
> When using this you will be teleported to the spawnpoint of the player you are pointing at. If that player has no spawnpoint saved, then you will not teleport. If you don't point at a player, you will be teleported at a spawnpoint from a random player. This only works in coop, and not in speedrun or single player

# ents file support #

if you put a file in the maps folder with the name: mapname.ents then this file will be loaded and all the entities will be spawned into the map.

example: escape2.ents
```
{
"classname" "ai_zombie"
"origin" "-1084 604 -260"
"angle" "240"
"ainame" "nazi1337"
"spawnflags" "1" 
"skin" "zombie/boss2"
"head" "escape3"
}
```
this will spawn a zombie in the map and it will attack the german soldiers.

You can also make target\_location entities so a location shows up in the coopoverlay on the HUD:
this is an example for the map tram
```
{
"classname" "target_location"
"message" "Allied Spawn"
"origin" "3292 1940 960"
}
{
"classname" "target_location"
"message" "Generator"
"origin" "3408 -240 984"
}
{
"classname" "target_location"
"message" "Tram entrance"
"origin" "2576 208 1096"
}
```