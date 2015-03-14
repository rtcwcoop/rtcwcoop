Up to date with RTCW Coop mod version 1.0.0!

We have made custom maps scene more appealing after 0.9.5! We have HTTP downloads and Campaign system so playing your map is now easier!



# Introduction #

Aim of this tutorial is to be up-to-date tutorial and list of features that can be used to get a map running for RTCW Coop from RTCW SP.

If this tutorial seems to be too wide and big for you, check at this very [quick summary tutorial](http://www.rtcwcoop.com/forum/viewtopic.php?id=77).

This has been done in Windows 7 environment. Users of other OS' must figure the correct folders.

I can't be held responsible for anything. Any feedback, mistakes, questions, problems or such can be pointed out on our [official mod forum](http://www.rtcwcoop.com/forum/).

Good luck and have fun from

-SSF-Sage



# Map Conversion Tutorial #

## 1. Introduction to RTCW Coop map converting ##


### 1.1. I want custom! ###

So you want to play a custom map in RTCW COOP mod? Fear not, it is fairly simple and fast. It will help tons if you know understand basics of map scripting. But even if you know nothing about scripting/mapping it is possible to convert a custom map with a bit of luck, patience, by selecting a well created map and following this tutorial.

If you are not really good scripter, pick up a simple small map at first. Make sure it runs perfectly in SP. Select one that has **no** cutscenes. Cutscene scripting needs to be removed and skipped. It's not always easy.

When you create a custom map. Remember to test it many times before releasing it. Also do test it multiple times with your friends on a dedicated server. Many bugs you will encounter **only** happen when there is more than 1 player on the server.


### 1.2. Remember to respect ###

It is recommended and friendly that you contact the original author of the map. If you can't contact the author, it is on your own risk. If you don't get contact and still want to proceed, it is recommended that you do not alter gameplay, looks or atmosphere too much. Also do remember to give full credit to the hard work by original author!

### 1.3. Difference between Coop and SP ###

While you are reading this, you probably have basic understanding of the difference between Coop and SP. If not go to a server and try it out.

Basicly things are almost same. Same BSP files can be used. BSP file is the heart of the map. Generally SP maps and Coop maps are same, commands and file work the same. However as we are playing in SP maps in MP environment we need to make some modifications to the scripts.


Most of the time all we need to do is edit the .AI file that has the AI scripting in it. Then the .script file that has the brush scripting. Also we need to add .ents file to add new entities, such as spawnpoins. Most of the other file edits are mostly cosmetical.

In a coop game, the following attributes are ignored: aim\_accuracy, aim\_skill, attack\_skill and health these are set based on the value of g\_gameskill.

In Coop 1.0.0. we added the coopmain folder. So now we have two separate folders: main/ and coopmain/. You should put all the original RTCW files into main/ folder, and all the new coop files into coopmain/

_With this update you can copy your RTCWCoop mod into your old RTCW game if you want to. Handy when you are running low on HD storage. Most of us will probably want to do it the old way with a standalone installation._

### 1.4. Folder where we store all the edited files ###

This is the coopmain folder where all screenshots you take goes. Where your config is stored. Where game creates .ents file. This folder can be used to place mapping files for easy testing. Do mind this affects all installations!

In RTCW COOP 1.0.0 the folder locates in:

C://Documents and settings/User/documents/RtcwCOOP/coopmain/

_Once you are ready with the map, you must pack it into a pk3! More on this later._

### 1.5. Scripting in short words ###

**Basic syntax of scripts**

Every .AI and .SCRIPT file script has the same basic syntax:


|name|Ainame or scriptname of entity|
|:---|:-----------------------------|
|{ |Starts the entity|
|spawn|Entity spawned|
|{ |Start spawn event|
|  |Command here|
|} |End spawn|
|trigger me|name of trigger|
|{ |Start trigger|
|  |Command here|
|} |End trigger|
|} |End entity|


```
name
{
	spawn
	{
		//I just spawned, give me command here
	}

	trigger me
	{
		//Something executed me, do something
	}
}
```

**Adding comments in the script:**

```
//Comment here
```


**Triggering something from the spawn**

Do it like this. Always add wait before you trigger something from the spawn, or you will make the map crash!

```
name
{
	spawn
	{
		wait 50  //If you trigger from spawn, ADD THIS!!!

		trigger name me  //trigger EntityName TriggerName
	}

	trigger me
	{
		//Print on the console that i got executed
		Print Hey who is calling?
	}
}
```

### 1.6 Scripting tutorials ###

**Few tutorials to help figuring out scripting:**

[Almost complete list of RTCW script commands with explanations](http://bzzwolfsp.googlecode.com/svn/wiki/mapping/scripting_definitions.zip)

_You must download, unzip it and open it in Web browser._


[Nice tutorial on scripting a basic AI with some actions](http://www.surfacegroup.org/tutorials/rtcw/ai_kim/)

[Rtcw script editor, if you are bored of notepad/notepad++ or such basic editor](http://rtcw.pro/en/downloads/rtcw-script-editor/)

### 1.7. Helpful commands ###

**Cheats**

In Coop we can use all regular cheats from SP. To enable cheats you must start your map with /coopdevmap mapname. Some useful cheats:

```
/god - makes you invincible
/notarget - makes ai not see you
/give all - gives all guns and items
/give ammo - gives ammo
/give health - gives health
/noclip - you can fly through the wall
/r_showtris 1 or 2 - allows you to see through walls
/aicast_debug 1 - allows you to see what .AI scripts are running
/script_debug 1 - allows you to see what .script file commands run
/where - tells your cordinates
```

**Also Coop adds a few new ones:**

```
/drawspawns - allows you to see all the coop spawnpoints
/drawtriggers - shows the ingame triggers
/dumploc <location name> - Add location info. We have not used this so far.
```

**Protip:**

/drawtrigger is specially awsome tool to figure out how the map works technically. Triggers activate most game events and execute trigger actions in .ai script. Combination of drawtriggers and aicast\_debug makes it easy to figure out when to add reinforcements and such.


## 2. START: Getting able to shoot and run ##

I just want to frag a couple AIs in a custom SP map with my friends!

### 2.1. Adding the map ###

It is simple. Download the original pk3 file. Place it in your game installation folder inside coopmain folder. Just like you are used to.

### 2.2. Starting map ###

Load RTCW Coop. Type /coopdevmap mymapname into console. Doing this allows us to play the map in developer mode. We can use cheats. Check the list of useful commands to learn more.

**ATTENTION! TEST THE MAP! IT MUST LOAD! IF IT DOES NOT, THE MAP HAS FAULT OR IS INCOMPATIBLE! FIX IT OR DITCH IT! DO NOT WASTE TIME ON COOP STUFF BEFORE IT RUNS!**

The map should start now. Many things don't work, but you should be able to run around. If the map don't start it has faults. Try another map, debug it or ask for help.


### 2.3. Let Coop read map scripts ###

  * 1) Open the custom pk3 file

  * 2) Go to folder maps inside the pk3 file

  * 3) Copy the files called mymap.ai and mymap.script

  * 4) Paste these two files into C://Documents and settings/User/documents/RtcwCOOP/coopmain/maps folder

_Most likely you don't have maps folder yet. Just add it like any other folder._

  * 5) Rename the files. You should now have mymap.coop.ai and mymap.coop.script files

Now the AI should have guns, be able to run and perform.

If the map has no script file, just ignore it for now. We will add one later.


### 2.4. Adding first set of spawns ###

  * 1) Join Allied team. Go to where you want your first spawns.

  * 2) Type /dumpcoopspawnpoint into console, this will create first spawnpoint

  * 3) Repeat previous 8 times. We need 8 spawnpoints.

  * 4) Close game to save it.

  * 5) Reload game and test it. Use /kill and see if you can spawn in 8 different locations.

**Something went wrong? I have to remove one!**

  * 1) Go to C://Documents and settings/User/documents/RtcwCOOP/coopmain/maps/ folder

  * 2) Open file mapname.ents in notepad

  * 3) Look for the entity:

```
{
	"classname" "coop_spawnpoint"
	"spawnflags" "3"
	"origin" "2159 -6951 69"
	"angle" "-83"
}
```

_These entities are listed in the order you added them._

_You can also check the location by reading the "origin" key. Go to your location ingame and use command /where and select the one closest._


Great. Now we also learned where the .ents file is located! This is all basicly we need to play the map.

**Additional info:**

Full list of spawnflags we have:
```
0 = disabled 
1 = enabled 
2 = allied and disabled           //Important
3 = allied and enabled            //Important
4 = axis and disabled
5 = axis and enabled              //Important
6 = axis and allied and disabled
7 = axis and allied and enabled
```

_You probably only need to know 2, 3 and 5_

### 2.5. Make sure you have .rcd files ###

.rcd files are files that includes bot reachability data. They are created automatically by the game first time you play your map.

Normally the mapper includes it into their pk3 inside maps folder, but sometimes they want to save on download size and they leave it out. Then they let every player create them theirselves.

So if following files show up into your ...documents/RtcwCOOP/coopmain/maps folder:

```
mapname_b0.rcd
mapname_b1.rcd
```

**Make sure to include them into your COOP addon pk3 we do in the next chapter! Otherwise it may cause problems when playing the map in your server!**

### 2.6. Make sure the map do not interfare with COOP MOD ###

  * 1) Rename the original pk3 to have "a" prefix in front of it. Like this:

```
a_mymap.pk3
```

_This causes the pk3 load before any mod files and not replace any similar files._

  * 2) Open up the original pk3 REMOVE the following folders:

```
 - UI
 - text
```

_These are useless for now. Content in these must be redone, if needed. We will return to these folders later on._

  * 3) If your map is inside a subfolder eg. _main/capuzzo/_

> Move the pk3 from a subfolder to be directly inside main/

```
Move pk3 from:
main/project/
To:
coopmain/
```

  * 4) Now the map will interfare with COOP mod. Check the original game that you have not altered anything. If you have, go through your pk3 and remove such content!

**Otherwise you will ruin the mod for each server that has your map!**

### 2.7. Packing up to play with your friends ###


  * 1) Create a new pk3 file called a\_mymap\_coop\_pk3

_It is recommended to create a new pk3 for the new/edited files only. Then it is faster to download new versions of your map modification._


  * 2) Create a folder called maps inside the pk3

  * 3) Paste the .ai ; .script and .ents file inside the maps folder

_You might want to add more files later on of course._

_If your maps folder include mapname\_b0.rcd and mapname\_b1.rcd files, include them too (check previous chapter)!!!_

  * 4) Add the renamed SP map and the small Coop modification pk3 into the coopmain folder of your server.

  * 5) Enjoy! You are ready to play. It is not perfect, but you can shoot, AI can shoot. Yay! :)


## 3. Making the map work properly ##

Just running and shooting is not enough for you? You want the map to play well, be able to win a map, finish objectives, hear music? Let's continue.

If we leave the scripts as they are now, it probably work somewhat when you play alone on local. But when a friend or friends join you, weird things start to happen.

### 3.1. Adding MP script support ###

Normally in SP the game starts from player scriptblock from spawn/playerstart events. Now problems come when a new player joins, because the spawn event gets triggered again. It will cause some objectives or other features to reset or re-run. So we need to change this behavior.


#### 3.1.1. First we need to add a script\_multiplayer entity. ####

Now we will create a new entity we can use to start the round, handle objectives and any other things that might break the map if more than 1 player is on the server.

**You must have one and _only one_ script\_multiplayer in your map for the scripts to work properly.**

  * 1) Open your .ents file.

  * 2) Add the following on top of everything in the file:

```
{
	"classname" "script_multiplayer"
	"scriptname" "game_manager"
}
```

  * 3) Close and save .ents file.


#### 3.1.2. Then we create the script for the script\_multiplayer entity ####

  * 1) Open mymapname.coop.script file in notepad

_If the map don't have one. Copy an existing script file. Open it in notepad, empty it. And then rename it._

  * 2) Add the following on top of the .script file:

```
game_manager
{
	spawn
	{
		trigger player map_start
	}
}
```

  * 3) Save and close .script file


#### 3.1.3. Next we need the map to start MP style ####

In other words we will add that trigger map\_start we just triggered.

  * 1) Open mymapname.coop.ai file and it looks like this:

```
player
{
   spawn
   {
   	....
   }

   playerstart
   {
   	....
   }

	//and so on.......

```

  * 2) Add that trigger map\_start we triggered from game\_manager's spawn. It will look like this:

```
player
{
   spawn
   {
   	....
   }

   playerstart
   {
   	....
   }

   trigger map_start
   {
   	//<- Add scripts here later
   }

//and so on.......
```

trigger map\_start will have all things that should only run once during the map startup. We will look closer to this in the next chapter.

  * 3) **Do test now!**

If the map script has basic bugs you will know it now. If the map will not start anymore, check the Problem solving section or ask at the forums!

_Most probably if some spawn actions have no wait you will crash with a script error._

### 3.2. Finding out where each map start script command goes to ###

Now we need to look deeper into what start up commands we have and where we need to place them.

#### 3.2.1. Short explanation ####

**Regular SP**

In regular SP game when player has loaded the map and is waiting on the briefing menu the spawn event gets executed. It starts menu time music and such.

When the player presses the arrow to actually start the game, playerstart event is executed.

**Coop**

Spawn event is executed when the player joins a team and it is executed for every player on the server when a new player joins. We need to give the new joiner guns and start music, but don't want all other players resetting their music and guns. Commands we place in Spawn are all edited for Coop, so they only affect the new comer.

In coop we have replaced playerstart with trigger map\_start, because we don't want every new player "restart" the map. All regular commands are moved into map\_start. So if you want to specify objectives, trigger actions and such at map start, place it here.

**More detailed explanations:**

**spawn event**

```
spawn
{

}
```

spawn event will run everytime a new player joins. We must only have following commands here:

  * Giving the player ammo/items/guns or anything related to this
_These will not affect other players._

_If your map has not any weapon script, but it gets weapons in SP, you must add them manually. There is a section that mention more of this. Every Coop map should have weapons added to it separately._

  * Add music. **MUST BE mu\_start sound/...**  !!!

_Otherwise the music will not start for all players. mu\_start command is edited in Coop! It will not restart music for other players. Read more in music section._

Anything else will be elsewhere, or the command will be executed again everytime a new player joins.

**playerstart event**

```
playerstart
{

}
```

**playerstart event must be emptied for Coop mod!** However leave it empty like this or you may get an error.

**trigger map\_start**

```
trigger map_start
{

}
```

trigger map\_start will hold everything we want to have executed when the map starts. But not executed again when a new player joins. Eg.

  * Accums
  * Objective scripting
  * Episode info
  * Triggering map start scripts eg. make something happen
  * Number of secrets
  * Random respawing
  * Anything else you want at map start


#### 3.2.2. Fixing start commands in our example map ####

This is our starting point in the map christmas\_map:

```
player
{
   spawn
   {
   	accum 1 set 0
   	objectivesneeded 1
   	abort_if_loadgame
   	mu_start sound/music/l_briefing_1
   	print Mission Objective: Capture the German Camp
   }

   playerstart
   {
   	giveweapon weapon_knife
   	giveweapon weapon_luger
   	setammo ammo_9mm 100
   	giveweapon weapon_mp40
   	setammo ammo_grenades 3
   	setammo ammo_dynamite 1
   	selectweapon weapon_mp40
   	mu_fade 0 100
   	mu_queue sound/music/music1
   	wait 7000
   	mu_stop 0 100
   }

   trigger map_start     //NOTE WE ADDED THIS IN PREVIOUS CHAPTER!
   {

   }

//and so on.......

```

  * 1) Look at spawn event. Remove if there are such commands as:

```
spawn
{
	...
	suggestweapon
	abort_if_loadgame
	mu_start sound/...
}
```

_Our example map has only abort\_if\_loadgame and mu\_start_

  * 2) Now copy the accum, any objective, episode and such commands to trigger map\_start. It should look like this now:

_If there are more commands try determing what it does. If you need it or not._

```
player
{
   spawn
   {

   }

   playerstart
   {
   	giveweapon weapon_knife
   	giveweapon weapon_luger
   	setammo ammo_9mm 100
	giveweapon weapon_mp40
	setammo ammo_grenades 3
   	setammo ammo_dynamite 1
	selectweapon weapon_mp40
	mu_fade 0 100
	mu_queue sound/music/music1
	wait 7000
	mu_stop 0 100
   }

   trigger map_start
   {
   	accum 1 set 0
   	objectivesneeded 1
   	print Mission Objective: Capture the German Camp
   }

//and so on.......

```

  * 3) Now we must empty playerstart. It no longer is supported. Leave the empty event thought or you might get an error. Only ammo and items and music should be moved to spawn event. Optional ones to the map\_start. So it should look like following:


```
player
{
   spawn
   {
   	giveweapon weapon_knife
   	giveweapon weapon_luger
   	setammo ammo_9mm 100
   	giveweapon weapon_mp40
   	setammo ammo_grenades 3
   	setammo ammo_dynamite 1
   	selectweapon weapon_mp40
   	mu_fade 0 100
   	mu_queue sound/music/music1
   	wait 7000
   	mu_stop 0 100
   }

   playerstart
   {

   }

   trigger map_start
   {
   	accum 1 set 0
   	objectivesneeded 1
   	print Mission Objective: Capture the German Camp
   }

//and so on.......
```

  * 4) Now rename mu\_queue to mu\_start and remove all other mu_commands from spawn event._

_The particular example map has different idea on music. Check music scripting section, if you want something different than this. Cosmetics.._

The final result should look like this:

```
player
{
   spawn
   {
   	giveweapon weapon_knife
   	giveweapon weapon_luger
   	setammo ammo_9mm 100
   	giveweapon weapon_mp40
   	setammo ammo_grenades 3
   	setammo ammo_dynamite 1
   	selectweapon weapon_mp40

   	mu_start sound/music/music1
   }

   playerstart
   {

   }

   trigger map_start
   {
   	accum 1 set 0
   	objectivesneeded 1
   	print Mission Objective: Capture the German Camp
   }

//and so on.......

```

**TEST YOUR MAP NOW**

_If it does not load, check that if you have a missing { or } or typos. Or go to Problem Solving._

### 3.3. Make the objectives work properly ###

Best solution would be to move all accums and objective stuff to the game\_manager in .script file. But this might not always be a must.

I have a lazy solution for you, that I have used sometimes.. So far it have worked for me. If you want to be extra safe then look at problem solving section.

Most common reason the objectives fail to work is because of accums. The next chapter might be enough to fix the objectives in some cases.

**Make sure to test all objectives with atleast 2 players.** _Also try it atleast twice in row. First you finish the objectives, then your fellow player should finish them._

### 3.4. Make the accums work properly ###

**Accum commands do not work properly in player scriptblock!!!**

Reason for this is that multiple players share the scriptblock, but accum command is only read by the one who activated it.

The solution to this is to turn all accum commands inside player scriptblock to globalaccums.

**Repeat: Only inside player scriptblock!**

Difference between accum and globalaccum:
```
accum =  Only one scriptblock or one client can see this. These work normally in any other scriptblock than player.

globalaccum = All scriptblocks and clients can see this.

```


#### 3.4.1 Fixing accums inside player scriptblock ####

  * 1) Search the ai script with keyword globalaccum, if nothing shows up, good. If there are already globalaccum, check that the numbers don't overlap.

_The number that matter in this case is the first after accum command:_

globalaccum **0** set 0 //Proper values for this are 0-9

  * 2) Select and copy player script block into another txt file

_If there are only few triggers, do it manually._

  * 3) Use replace tool to replace accum with globalaccum

  * 4) Paste it back to the .ai script.


Definition of player script block with example:

```
player //Starts up with this
{
   spawn
   {
   	...

   	globalaccum 5 set 0   //Used to be accum 5 set 0  
   }
   

//Lots of events and triggers...


  trigger endmap
  {

  	globalaccum 5 abort_if_less_than 1  //Used to be accum 5 abort_if_less_than 1
  	...
  }


} //Ends up with second } in row
```

**!!!NOTE!!! DO NOT REPLACE ANY MORE THAN WHAT IS INSIDE PLAYER SCRIPTBLOCK!**

### 3.5. Adding weapons ###

Unless you edit the first map of the custom campaign you probably want to add weapons. In SP the game remembers the weapons from previous map. In Coop we add weapons manually in every map to ensure players nevers spawn without guns.

How we did it. Start the campaign in SP. Play the first map. When you start the second map, check what weapons and how much ammo you have, write it down and then later apply it to your Coop scripts. And so on.

Add those weapons from previous maps or what ever you wish the players will have.

Example on adding weapons:

```
player
{
   spawn
   {	
   	giveweapon weapon_knife
   	giveweapon weapon_luger
   	setammo ammo_9mm 100
	giveweapon weapon_mp40
	setammo ammo_grenades 3
   	setammo ammo_dynamite 1

   	//.... more scripts here
   }

   //.... more scripts here
}
```

_You probably know this already._



### 3.6. Making music work properly ###

Coding music from SP to MP world is not simple. Here is the quick approach we have.

When a new player joins, he gets the first music that the map played. He will listen to the map starting music until it is changed with mu\_fade/mu\_queue command. Every player must start sounds with mu\_start command, or it will fail to start. mu\_start command is only executed to the newly joined player!


That's why we add mu\_start in player spawn event.

```
player
{
	spawn
	{
		...
		mu_start sound/..
	}
}
```

**If you need to stop the music in the middle of the round. Use mu\_stop.
You must restart the music in game\_manager or only one player will hear it** eg:

```
game_manager
{
	...

	trigger restart_stopped_music
	{
		mu_start sound/...
	}
	}
```

Here is the more detailed info on how the mu_commands were done in Coop._

```
mu_start: -In coop.ai file this only affects the client who called it, so you must use it in player->spawn event! This is only for map start. -If you need to start music in the middle of the map, place it in coop.script file!

mu_fade: -Affects all client in both coop.script and coop.ai file

mu_stop: -Affects all client in both coop.script and coop.ai file

mu_play: -Affects all client in both coop.script and coop.ai file

mu_queue: -Affects all client in both coop.script and coop.ai file 
```

### 3.7. Making secret areas appear correctly in end stats ###

**If the map has no secret areas, do still read this tutorial.**

The secret area code is built one player in mind. Now when we have multiple players it tend to multiple the amount. We need to do following to fix this.

  * 1) Check the number of secrets.

  * 2) Go to trigger map\_start:

```
trigger map_start
{
	...
}
```

  * 3) Add command numsecrets _amount_ somewhere in it.

_If there is none, you should still type number 0._

```
trigger map_start
{
	...
	numsecrets 3
}
```

This only affects the number presented in the end stats. Rest of the secret area scripts should work as in SP.

### 3.8. Remove any cutscene scripting ###

_If your map has no cutscenes, skip this chapter. If it has, you might need some scripting understanding or help._

RTCW Coop mod does not support cutscenes. All camera script commands should be ignored by code, but it is safer to remove them.

Most of the times during a cutscene AI does some actions. Normally SP either plays the cutscene or you can skip it with a press of Esc button. Coop cannot do this. So you must manually skip the cutscene script actions. Do the following.

  * 1) Load the map in SP game. Go to part where cutscene starts. Try pressing Esc button. This should skip the cutscene.

_If it does, good, please continue to next stage!_

_If Esc does not skip the cutscene, stop right here for a moment. You better know how to do some scripting. Or you can post it on the forums for others to solve. The part of the script needs to be redone to skip the cutscene, or the cutscene script will play out while you play the game._

  * 2) Search .ai file with a keyword _trigger camerainterrupt_

_This is the trigger that is executed when you press Esc_

**Example from map Escape1:**

```
trigger camerainterrupt
{
	stopcam
	trigger dummy cine1_cleanup
}
```

  * 3) Copy the line with the trigger command

  * 4) Paste it to the action where the cutscene happens. Usually when the map starts:

```
player
{
	....

	trigger map_start
	{
		...

		trigger dummy cine1_cleanup  //Directly skip to end of the cutscene
	}
	....
}
```











## 4. Optional common Coop features ##

This is the list of common features most Coop maps should have in my opinion. These are not really required to play!

### 4.1. AI reinforcements ###

This is the stage that requires most time.

Shooting at the default AI's get a little boring quickly. That is why we should add some reinforcements to make it more challenging. Usually we add about 20 AIs on many and 40 on More. _g\_reinforce 1_ add many and _g\_reinforce 2_ most AI.

#### 4.1.1. Adding an AI entity ####

  * 1) Load game

  * 2) Go to where you want your first new AI. Have yourself face same way you want the AI to face when he spawns.

  * 3) Dump the AI using command /dumpcastai classname ainame

Avaiblable classnames:
```
ai_soldier ai_american ai_zombie ai_warzombie ai_venom ai_loper ai_boss_helga ai_boss_heinrich ai_eliteguard ai_stimsoldier_dual ai_stimsoldier_rocket ai_stimsoldier_tesla ai_supersoldier ai_protosoldier ai_frogman ai_blackguard ai_partisan ai_civilian
```


_I like to leave ainame empty. Then you must fix the number manually in .ents file!_

Example:

```
/dumpcastai ai_soldier coop_ai_soldier_1
```

  * 4) Go to .ents file. Give the AI skin and face. If you skip this it will load default Escape1 skins. Look at skin and head key. Check existing maps for available skins.

```
{
	"classname" "ai_soldier"
	"origin" "-111 -98 24"
	"ainame" "coop_ai_soldier_1"
	"angle" "-92"
	"spawnflags" "1"
	"skin" "infantryss/factory1"  //<-
	"head" "factory3"             //<-
}
```


_Always use spawnflags = 1 for reinforcements_


#### 4.1.2. Giving our new AI basic attributes ####

  * 1) Open .ai file.

  * 2) Add an entity script to the bottom of the .ai file. Every AI class should have little different script (Soldier, Venom, Zombie...).

  * 3) Example with regular soldier. If you add another type of enemy, check existing Coop map for script examples.


_This makes it a default mp40 guy. [Here is a SP AI tutorial for little help if you want something else.](http://www.surfacegroup.org/tutorials/rtcw/ai_kim/)_

```
coop_ai_soldier_1 //NAME OF YOUR NEW AI!
{
    attributes
    {
        aim_accuracy 0.6
        starting_health 60
        camper 1
    }

    spawn
    {
        setammo ammo_grenades 0  //Take away grenades
        setammo ammo_9mm 999     //Give him 999 bullets for 9mm
        statetype alert
    }
}
```

_AI\_soldier gets mp40 given by default, so all we need to do is to give him ammo. Each AI class have it's own default gun they get, but not much ammo._

#### 4.1.3. Making our new AI show up ingame ####

The AI's inside .ents should not appear in game without separately executing them.

**We have 3 common commands to execute it:**

```
alertentity coop_ai_soldier_1
```
_This spawns always. Not much used._
```
#if g_reinforce >= 1 alertentity coop_ai_soldier_1 #endif
```
_This spawns if we have /g\_reinforcements set to 1 or 2
```
#if g_reinforce >= 2 alertentity coop_ai_soldier_1 #endif
```_This spawns if we have /g\_reinforcements 2

**Where to put the command that spawns our AI**

So let's execute our AI at a desired time. Most often it happens at map\_start or from some action.

Let's start out by opening our .AI script file. Search with the keyword alertentity, to figure out the triggers used for spawning the default AI.



_Add the command in the stage of the map where you want him to appear. Example:_

```
player
{
	...
	trigger action1 //Walk through the doorway
	{
		...
		alertentity nazi1 //This is the first default nazi
		...

		#if g_reinforce >= 1 alertentity coop_ai_soldier_1 #endif
	}
	...
}
```

_This will spawn coop\_ai\_soldier\_1 if g\_reinforce is 1 or 2. The AI will spawn same time with nazi1 when action1 is executed._

**Save and test.**


#### 4.1.4. I want more than one AI ####

Repeat the three previous steps as many times you want, just remember to change the AIName.

You can add around 100 before hitting a limit. But I don't recommend adding a lot more than 50. Also try not to place more than 10-20 in one open area in the map. Otherwise the map might end up really laggy for most of us.

_40 is a good amount if you place them all over the map._


### 4.2. Making single AI not respawning ###

Go through all the AI scripts to check this list through. This is recommened to those AI's that:
  * Activate a game event from the spawn or death event.
  * If they respawn without a gun.
  * If he is a boss or something else special.
_It is recommended trying to rescript these first if possible. This is the last chance fix._

How to enable:

  * 1) Open .ai script
  * 2) Go to the specific AI's scriptblock
  * 3) Add command _norespawn_ to the spawn event

Example:
```
nazi1
{
	spawn
	{
		//I am a special boy
		trigger what_ever just_because
		...
		norespawn  //<- Add this
	}
	...
}
```

### 4.3. Random respawning ###

We introduced a new feature to Coop to make the respawning little more reasonable. There is a feature to define that if one AI should select randomly if it respawns or not. So one round it can respawn and next round maybe not. Chances are 1/2 that it will respawn.

We controlled it in map script to select what it affects. Here is the rules we decided for standard maps:
  * If AI respawning is unlimited then all AI's will always respawn.
  * If you limit the times AI can respawn to eg. 5 times _g\_airespawn 5_ then all reinforcement AI's who has not norespawn command set will respawn randomly.
  * All regular SP AI's will respawn normally.

So in script:
```
#if g_airespawn >= 1  //Do not read this if AI respawning is infinite
	wait 5
	randomrespawn coop_ai_soldier_1
	//coop_ai_soldier_2 has norespawn
	randomrespawn coop_ai_soldier_3
	randomrespawn coop_ai_soldier_4
	randomrespawn coop_ai_soldier_5
	randomrespawn coop_ai_soldier_6
	randomrespawn coop_ai_soldier_7
	randomrespawn coop_ai_soldier_8
	randomrespawn coop_ai_soldier_9
	randomrespawn coop_ai_soldier_10
	wait 5  //Have wait!
	randomrespawn coop_ai_soldier_11
	randomrespawn coop_ai_soldier_12
	//List goes on.
#endif
```

**Normally we have around 40 reinforcements. If we put them all in list we will crash/lag the map. That is why there is _wait 5_ between every ten commands!**

### 4.4. Flag spawns ###

We have a Cvar to allow us going into mode where we need to capture our spawnpoints. To enable this mode do _/g\_spawnpoints 2_

Currently we only have the start spawns we created earlier. So do following to add a new flag spawn. It can be repeated to create more flags. Most maps we have has 2 flags.

  * 1) Run your map ingame
  * 2) Use command /dumpflagpole to drop a flag

_Before doing this you must have the first spawns added. Otherwise you must fix them manually._

_You must first add flag! Because the game will automatically link all spawnpoints that you drop until next /dumpflagpole command._

_If you fail on this, you can fix the linking in .ents file. Just match targetnames of the spawns to the the target key of the flag._

  * 3) Repeat /dumpcoopspawnpoint 8 times around the flag.

  * 4) Close game to save.

  * 5) Reload game and test!

_If something went wrong, check section 2.4._


### 4.5. Axis spawnpoints ###

!!!!!!!!!ATTENTION!!!!!!!!
**WE HAVE DISABLED AXIS PLAYER IN 1.0.0.**

_We plan to put it back later. But for now it had way too many bugs. I recommend adding these spawnpoints anyway for future. It will not do any harm._

The Coop mod now supports having maximum of Two axis players. These need spawnpoints. One for each player.

They will use the spawnpoint we set only for the map start. After this they will respawn into location of a random AI close to action and kill this AI. That's why we don't use inactive axis spawnpoints atm.

**Let's add 2 new axis spawnpoints:**


  * 1) Load game

  * 2) Use command /dumpcoopspawnpoint twice.

  * 3) Close game and open .ents file

  * 4) Find the new coop\_spawnpoints from the bottom

  * 5) Change the "spawnflags" to "5" on both of them.

_This will tell the game these are Axis spawnpoints and active._

  * 6) Check that the game did not add any targetname to these.

_If it did, remove the entire line eg:_

"targetname" "sfm0" 

**End result should look like this:**

```
{
	"classname" "coop_spawnpoint"
	"spawnflags" "5" 
	"origin" "-726 -3163 41" 
	"angle" "0"
}
```

### 4.6. Getting map to show up in Map list or Create server menu ###

Tired of starting your map with console? Let's make it available to be selected from the menus.

To see your map in menus, we need to add an arena file.

**Once you add the arena file, any server that has your project in their coopmain will automatically add your maps to their rotation.**

So after standard maps are finished, it will automatically start playing the custom ones. And once all the custom projects are played, it will go back to standard maps and so on.


#### 4.6.1. Follow these rules when you make your .arena file ####

  * Custom arenas are supported
  * Never edit any existing arenas!
  * You should have only one arena for your entire project!
  * Include all your project maps in one arena file.
  * Arena file can be named almost anything just make sure you don't affect any other project.
_Good name for the arena file would be eg. myprojectname.arena_
  * Make sure it is not named .arena.txt
_if you are unsure how to make it, copy and edit one from Capuzzo_
  * Add the maps in the order they are played
  * Maps in your arena will automatically load after standard maps

.

#### 4.6.2. This is how one map looks like in .arena file ####

```
{
	map			"mymapname"
	longname		"My project - My first map"
	type		"coop_battle coop_speedrun coop_normal"
}
```

#### 4.6.3. Explanation of arena file syntax ####

|{|Starts a map|
|:|:-----------|
|map|name\_of\_the\_bsp|
|longname|The Name I want in menu|
|type|Don't touch unless said so|
|} |Ends a map|

_Q3 color codes are supported in longname!_


#### 4.6.3. Here is an example from Capuzzo Campaign ####

[Download the example file!](http://bzzwolfsp.googlecode.com/svn/wiki/mapping/capuzzo_arena.zip)

_Filename : capuzzo.arena_

```
{
	map			"city"
	longname		"^nCapuzzo 1 - The City"
	type		"coop_battle coop_speedrun coop_normal"
}

{
	map			"airport"
	longname		"^nCapuzzo 2 - The Airport"
	type		"coop_battle coop_speedrun coop_normal"
}

{
	map			"road"
	longname		"^nCapuzzo 3 - The Road away!"
	type		"coop_battle coop_speedrun coop_normal"
}
```

#### 4.6.4. Where the arena file is located? ####

Arena file has to be located in scripts folder.

  * You can test it in ...documents/RTCWCOOP/coopmain/scripts

  * For releasing, create a folder called scripts into your mapname\_coop.pk3.



### 4.7. Creating loading image ###

This is the image we get while we are loading the map.

**This image must be in power of two and squared!**

_eg. 256x256 or 512x512. If you don't make it squared it will not work on all video cards._

  * 1) Take a pic of your map.

  * 2) Add some effects in photo editor if you like to. Like Coop logo or map name.

_Please mind that it is not nice to add your own name here, unless you are the original author of the map!_

  * 3) Resize the pic into power of two. You probably want to make it 512x512 pixels.

  * 4) Make the pic uncompressed default jpg format.

  * 5) Save with name mymapname.jpg

  * 6) Create a folder called levelshots.

_For developing you can test it directly inside ...documents/RTCWCoop/coopmain/levelshots/._

  * 7) When it looks right place it in your mapname\_coop\_pk3 inside folder levelshots.

### 4.8. Create server menu images ###

These are the images you see in the menu when you select the map in Create menu server. We have 3 different images in it:

  * ui\_mapname.jpg
_This is the bigger image on left_
  * ui\_mapname\_s1.jpg
_This is the smaller image on top right_
  * ui\_mapname\_s2.jpg
_This is the smaller image on bottom right_

Check previous chapter. Same basic rules apply to these images too.

### 4.9. Adding location info ###

#### 4.9.1. Flag location ####

**Test this! I am not sure if this got fixed for 1.0.0 but this may cause spawning at wrong flag.**

It is possible to give each flag an individual name that is printed if you capture the flag.

Naming your flag is handy and simple. Add this in your ents file and edit it:

```
{
"classname" "target_location"
"message" "courtyard"
"targetname" "courtyard_spawns"
}
```

Targetname should be same as your coop\_spawnpoints have.

Message is the name of the spawn.

_Adding this would print "player has secured the courtyard flagzone" if a flagpole with "courtyard\_spawns" is captured._

#### 4.9.2. Player locations ####

This was overlooked in current Coop version. The tools exists but nobody had the strength to go through all the locations in all maps to name them and add location info. But the tools should work.

This feature adds your location info in the 2d view. For example your mates see where you are in the map when you use team chat.

Type into your console:
```
/dumploc <location name>
```

_Writes the target\_location entity with location name to your .ents file_

### 4.x. Adding new entities like extra ammo/guns/item ###

This is for advanced users.

  * You can add any game-time entities, that you normally add in map editor, by .ents file. It does not matter if it is target\_x, ammo\_x, props\_x and many other entity.

_I don't go further listing entities but all that are loaded during game and not during map compiling process can be added. Map compiling is the process of turning map to bsp file. Original mapper compiled the map._

  * You can use all regular "keys" and "values" you see in your .map file.

  * You can add new models

  * You cannot add new brushes atm. :(

  * You can duplicate an existing brushmodel from a script\_mover, func\_explosive or such entity. See more in Extra section.

_For example in tram number two I added a new tram model but duplicated the clips from the original tram._

**How to add a new entity**

  * 1) Use /dumpcoopspawnpoint

_or any other dump command or do it manually. This gives you the origin (location) easily_

  * 2) Open .ents file

  * 3) Change "classname" "entity"

_entity is the entity classname, eg. target\_kill_

  * 4) Edit/Remove/Add any commands you wish

_Look at GtkRadiant what each does_

**You can use If commands to add these if eg. spawn reinforcement is used. See more in IF chapter**

**Example, how to add a health pack:**

```
{
"classname" "item_health_large"
"origin" "1919 441 24"
"angle" "90"
}
```



## 5. Extra tools/features ##

Here are things we will not probably need, but that allows us to do many things we want but normally can't. Note this is for advanced mappers. If you have not done your homework, you probably will not even miss these.

_Following things are not fully explained, thinking required. :)_


### 5.1. Adding Notebook text ###

In RTCW SP we had to create our own mod folder ( eg. main/capuzzo/ ) and edit the menu files. In Coop we have introduced map spefic notebook menu script. Now we can create a new menu file called mapname\_notebook.menu.

This way we don't need to have use a mod folder. The pk3 can be placed in coopmain and we will not ruin the COOP mod notebook menus.


**So example from Capuzzo.** I have customised and cleaned my notebook, but you will probably find it ok. It is changed to simple 2 page system instead of 4.


_Check it from Capuzzo, if you are not happy, you can copy & rename a map from the default notebook.menu._

Do following:

  * 1) Download my example file [notebook\_example.zip](http://bzzwolfsp.googlecode.com/svn/wiki/mapping/notebook_example.zip)

  * 2) Unzip it to your mapping coopmain folder. Make sure the files go to UI, scripts and text folders as they were in the Zip file. Rename each filename from city to yourmapname.

_Ignore scripts folder for now_

  * 3) Open yourmapname\_notebook.menu with Notepad or your favorite text editor

  * 4) Search with keyword city and replace each with yourmapname. Affected section will look like this:

```
////////////////////////////////////////////
////////////////// yourmapname ////////////
//////////////////////////////////////////

	itemDef {
		name page1a
		group page1
		cvarTest "mapname"
		showcvar { "yourmapname" }	// added current map rev
		rect 60 60 240 380
		textfile "text/EnglishUSA/yourmapname_p1.txt"
		textfont UI_FONT_HANDWRITING
		textscale .2
		forecolor .1 .1 .1 .7
		visible 1
		autowrapped
		decoration
	}

	itemDef {
		name page1b
		group page1
		rect 345 70 230 380
		cvarTest "mapname"
		showcvar { "yourmapname" }	// added current map rev
		textfile "text/EnglishUSA/yourmapname_p2.txt"
		textfont UI_FONT_HANDWRITING
		textscale .2
		forecolor .1 .1 .1 .7
		visible 1
		autowrapped
		decoration
	}


itemDef {
	name page1b
	group page1
	style WINDOW_STYLE_SHADER
	background "m_yourmapname"
	cvarTest "mapname"
	showcvar { "yourmapname" }	// added current map rev
	rect 345 180 230 280
	textalign 1
	textalignx 32
	textaligny 30
	forecolor 1 1 1 .7
	backcolor .3 .3 .3 1
	visible 1
}

```

  * 5) Edit the yourmapname\_p1.text to contain what you want in your page1 of notebook. Repeat with yourmapname\_p2.

_You can take reference from the original notebook texts if it had in SP._

  * 6) Edit the m\_yourmapname.tga file to your liking in your favorite photo editor.. Keep the original size and format. Remember alpha channel for nicer effect. I have added some handwriting memo in it. It can be photo/map/empty or anything.

  * 7) Repeat this to each map.

  * 8) Open ui\_capuzzo.shader located in scripts folder with eg. Notepad. Rename it to ui\_myproject.shader. Edit each name to match your maps. Each map should have own .tga listed in this one shader file.

_My example shader has 3 maps; city, airport and road._

**Test each map. Finish each objective and make sure the green objective finished check markers hit the right spot. Fine tune text or menu file if needed.**

### 5.2. Making ingame letters work (clipboards) ###

If you want to add a totally new letter. It is probably possible to add from .ents file. But you have to learn to add clipboards first.

This tutorial only helps you make existing one work.

Just like with the notebook, we can not replace existing clipboard.menu. We will create a new file called yourmapname\_clipboard.menu. I have cleaned my menu file, so it only has the City clipboard script.

  * 1) Download my [clipboard\_example.zip](http://bzzwolfsp.googlecode.com/svn/wiki/mapping/clipboard_example.zip)

_Make sure the folders remain when you unzip._

  * 2) Open UI folder

  * 3) Rename the menu file from city to yourmapname.

  * 4) Open text folder in the original map pk3.

  * 5) Find out what is the letter name. Find the letter text file in the map pk3. It is something like

```
/text/englishusa/cl_myletter.txt
```

_One map can have multiple letters, so the cl\_names are not tied to a map. I just used cl\_city as a letter name for simplicity._


  * 6) Open yourmapname\_clipboard.menu in text editor.

  * 7) Search with keyword city. Replace each with myletter. Affected part will look like this:

```

////////////////////////////////    cl_myletter /////////////////////////////////// 



itemDef {
	group clip_myletter
	name clip_myletter
	rect 0 0 320 440
	style 3
	background "document1.tga"
	visible 0
	decoration
	}

itemDef {
	name clip_myletter
	rect 18 78 280 322
	group clip_myletter
	textfile "text/EnglishUSA/cl_myletter.txt"
	textscale .18
	forecolor .1 .1 .1 .9
	visible 0
	decoration
	autowrapped
	}

itemDef {
	name clip_myletter
	rect 18 78 280 322
	group clip_myletter
	textfile "text/EnglishUSA/cl_myletter.txt"
	textscale .18
	forecolor .1 .1 .1 .6
	visible 0
	decoration
	autowrapped
	}
}
```

  * 8) If your map has more than 1 clipboard, just copy paste the affected part, then just replace myletter with the other letters name.



> Test!


### 5.3. Using IF commands for advantage ###

The .ents, .ai and .script file support #if #else and #endif for cvar conditions.

  * Most usual cvars should work.

  * Following statements are supported

Fretn!

|==|Equal to| |
|:-|:-------|:|
|<=|Less than| |
|>=|More than| |
|!=|Not equal to| |

etc. ??

  * You can have many scripts per #if command. Just place #endif where you want.

Syntax:

```
#if CVAR statement value command #endif
```
Eg.
```
#if g_reinforce == 0 alertentity dead_nazies #endif
```

Couple examples:

**Adding extra guns if g\_reinforce is 1 OR more.**

  * Add this into .ents file:

```
#if g_reinforce >= 1
{
"classname" "ammo_9mm"
"origin" "-8284 -15338 122"
"angle" "86"
}
{
"classname" "ammo_792mm"
"origin" "-9017 -13647 128"
"angle" "34"
}
{
"classname" "ammo_9mm_large"
"origin" "-3723 -10829 66"
"angle" "-90"
}
#endif
```

**Trigger something if ai respawn is unlimited**

  * Add this into the desired script trigger

```
 #if g_airespawn == -1 
 trigger game_manager break_freee
 #endif
```

**Alertentity something if flag spawn mode is active**


  * Add this into the desired script trigger

```
 #if g_spawnpoints == 2 alertentity parachute_box #endif
```

I guess you got the point now. :)


### 5.4. Random stuff ###

**g\_random**

We have added a command called g\_random. It will select a value between 1-10 randomly each round. You can use it to add easy random stuff.

Example script from escape2.ents. Here we select a random location for dynomite:

```
{
"classname" "ammo_dynamite"
#if g_random == 1
"origin" "1368 813 -140"
#endif
#if g_random == 2
"origin" "1709 2221 -199"
#endif
#if g_random == 3
"origin" "978 1661 -135"
#endif
#if g_random == 4
"origin" "626 244 -199"
#endif
#if g_random == 5
"origin" "1858 3757 -199"
#endif
#if g_random == 6
"origin" "2151 3451 -103"
#endif
#if g_random == 7
"origin" "1960 2802 -195"
#endif
#if g_random == 8
"origin" "2157 2626 -199"
#endif
#if g_random == 9
"origin" "1154 2466 -199"
#endif
#if g_random == 10
"origin" "690 1741 -55"
#endif
"angle" "90"
"spawnflags" "8"
"wait" "3"
}
```

_This follows the same general rules as stated in previous chapter._

**Regular random accum**

We have had this since the days of RTCW. For some reason it is not really used, but I have taken it to use in Capuzzo.

Don't bother continuing if you don't understand scripting:

Example:

  * Place this inside trigger map\_start
```
	globalaccum 0 random 2
```

_It will set globalaccum 0 to a value of either 0 and 1. You can have more random values than 2._

  * Add this to a desired trigger

```
	trigger player random_start_a
	wait 5
	trigger player random_start_b
```

  * Add these into player scriptblock:

```
trigger random_start_a
{
	globalaccum 0 abort_if_not_equal 0
	//ADD COMMAND HERE
}

trigger random_start_b
{
	globalaccum 0 abort_if_not_equal 1
	//ADD COMMAND HERE
}
```

_Usually I use this to teleport the AI to another location._

### 5.5. Removing a brush entity without targetname ###

We have added a command to remove an entity that normally would not be possible. For this you need to do following:

  * 1) Open the BSP file with eg. notepad

_Yes it is a HUUUUGE mess. But the entities are listed with normal syntax!_

  * 2) Search with the classname, eg. func\_explosive

  * 3) Find the number of the entity. Let's say it is 23.

  * 4) Place this into a desired script trigger in .ai or .script file:

```
	removeentity *23
```

### 5.6. Copying a brushmodel ###

For very advanced users.

We have a possibility to "clone" the brushes of an entity. There is a possiblity it will bug in some methods. It may turn invisible or you may walk through it. Test it. Use your own senses to adapt this idea.

Here we have an example how I cloned the tram model succesfully. Original problem was that some players would see through it with other methods.

  * 1) Find out the entity number from the BSP.

_Check previous chapter_

  * 2) Let's say it is 29. Copy the following to your .ents file.


```
{
"classname" "script_mover"
"origin" "-2492 -284 -160"
"spawnflags" "2"
"model" "*29" //THIS IS CLIP BRUSHES OF THE 29
"model2" "models/mapobjects/coop_tram/tram_fake_tag.md3"
"scriptname" "tramcar_coop"
}
```

Let's look closer to this.

"model" key has the brushmodel. These turn into invisible solid brushes, because I have "model2" key.

"model2" key has a new fake md3 model I created in 3d software. It contains only a tag file.

  * 3) Add the new model tram\_fake\_tag.md3 into models

  * 4) Add the visible model:

```
{
"classname" "misc_gamemodel"
"origin" "-2492 -284 -160"
"model" "models/mapobjects/coop_tram/tramcar_coop.md3"
"scriptname" "tramcar_coop_model"
"targetname" "tramcar_coop_model"
}
```

  * 5) Add the new model tramcar\_coop.md3 into models

  * 6) Attach the visible model to the clips:

```
tramcar_coop_model
{
	spawn
	{
		wait 100
		attachtotag tramcar_coop tag_tram
	}
}
```

Done.

### 5.7. Make dyno objectives out of existing stuff ###

In Escape2 we got tired with the bridge door not opening. It was built in a way we did not trust back then. Also we thought it might be cool to be able to blow it up with a dyno.

Here is what we did in escape2:

  * 1) Go to the location and add this with proper origin:

```
{
"model" "models/multiplayer/flagpole/flagpole_reinforce.md3"
"origin" "626 835 152"
"spawnflags" "1"
"classname" "trigger_objective_info"
"track" "the Bridge defense door"
"score" "10"
"target" "doortarget"
}
```

_This will not create a flagpole model. It will make invisible hacked trigger._

_This is the trigger you plant dyno to_


  * 2) Create a fake func\_explosive:

```
{
"health" "100"
"scriptname" "bridge_door"
"origin" "728 855 162"
"classname" "func_explosive"
"spawnflags" "68"
"targetname" "doortarget"
"mass" "400" 
"type" "none"
"dmg" "75"
}
```

_Change "type" from "none" to "debug" to tweak the location!_



  * 3) Find the entity number from BSP. 23 it is now.

  * 4) Use removeentity to remove the actual visibile entity.

```
bridge_door //bridge steel door
{
        spawn
        {   
        }   

        death
        {   
		removeentity *23
        }   
}
```

_If it has targetname, there are easier methods to remove it!_

### 5.8. Adding Coop entities to your GTK Radiant 1.5.0 ###

If you want to add Coop entities straight from your map editor. Add this to wolf\_entities.def, located in your GTK-folder/wolf.game/


```
/*QUAKED coop_spawnpoint (0 0 1) (-18 -18 -24) (18 18 48) startactive
Potential spawning position for COOP. Target with coop_spawnpoint_trigger or tick the box startactive to activate the spawn.

"targetname"            If targeted by coop_spawnpoint_trigger
"angle"                 Facing direction

*/

/*QUAKED coop_spawnpoint_trigger (.5 .5 .5) ?
Captureable flag. Target it to coop_spawnpoint.

"target"                        Target it to coop_spawnpoint
"model"                 Add model ( models/multiplayer/flagpole/flagpole_reinforce.md3 )
"angle"                 Facing direction

*/

/*QUAKED target_location (.5 .5 .5) (-8 -8 -8) (8 8 8) ?
Works only on MP or COOP
"targetname"            Can be linked to coop_spawn_trigger to activate with a new spawnpoint.
"message"               Name of the location
"count"                 Set 0-7 for color.
0:white 1:red 2:green 3:yellow 4:blue 5:cyan 6:magenta 7:white


*/
```

_Thought I must admit I found it more useful to add through the game, due to automatic key values._



## 6. Problem solving ##

Before releasing, you must make sure you test A LOT. Test it with your friends. On a dedicated server. Make each player finish the objectives on their turn. Most of the bugs will not happen while testing alone on a local server.

### 6.1. Map does not finish ###

Try this:

  * 1) Remove changelevel from trigger endmap in player scriptblock. Then add following

```
trigger endmap
{
	trigger game_manager check_exit
}
```

  * 2) Add trigger check\_exit into game\_manager

```
trigger checkexit
{
	trigger player exitlevel
}
```

  * 3) Add trigger exitlevel with changelevel into player scriptblock

```
trigger exitlevel
{
	changelevel tram persistent
}
```

This will make the end process check through game\_manager before going directly to end. Very useful for adding a check for finishing objectives etc.


### 6.2. Script errors ###

**I cannot play after adding script\_multiplayer or map\_start:**

If your map fails to load with a script error. Check any AI who spawn at map\_start that if they have special actions, it starts after a wait. This is normal action for a mapper to do to be safe in any Q3 engine game, but apparently not all mappers know this. Eg.

```
nazi1
{

	spawn
	{
		//Hey I spawn at mapstart
		wait 50 //Add me or you will crash the coop map muaha
		trigger nazi1 go //I wanna go when I born
	}
	....
}
```


### 6.3. Map objectives still bug ###

You have added globalaccum? Thought of everything you can? Well you can always try this.

Move all the objective and game actions to game\_manager. All the game triggers point to player scriptblock, but you can just forward them to game\_manager.

Example:

  * 1) Copy content of trigger action1 into game\_manager

  * 2) Then make the one in player script block to point to game\_manager

```
trigger action1
{
	trigger game_manager action1
}
```

Be careful when you do this! Make sure you don't break any trigger paths and commands or such.

### 6.4. Bugs mapper can not fix ###

There are some bugs mapper cannot do anything about. I have tried to eliminate most of them, but there is a good chance something will appear. If you have something in your mind, please report it on our official forum.

### 6.5. Players spawn inside each other or die at respawn ###

This means there is something wrong with your coop\_spawnpoint entities. There is 3 usual reasons for this.

  * 1 ) You have not added any coop\_spawnpoints. Check section 2.4.

_If you have added coop\_spawnpoints, let's continue_

  * 2 ) If it happens at start, you have used wrong spawnflags at start spawn. At map start the right spawnflags for the coop\_spawnpoints is 3.

```
{
"classname" "coop_spawnpoint"
"spawnflags" "3"
"origin" "-39 -9905 24"
"angle" "-35"
}
```

  * 3 ) If it happens after capturing a flag, you have used wrong spawnflags at a flag spawn. For a flag spawnpoint, the correct spawnflags for the coop\_spawnpoints is 2.

```
{
"classname" "coop_spawnpoint"
"spawnflags" "2"
"origin" "1642 -4912 28"
"angle" "-97"
"targetname" "myflagspawn"
}
```

## 7. Example map ##

**NOTE**

Our example map have the missing wait bug that I fixed in the Coop version. The friendly AI trigger actions directly from spawn without wait. You need to fix this if you intend to build it along this tutorial. Description of this bug is in Problem solving section.

[Download Coop addon file](https://bzzwolfsp.googlecode.com/svn/wiki/mapping/christmas_map_coop_sage.pk3)

[Download original map](http://www.rtcwcoop.com/main/christmas_map.pk3)

_Note you need both of these files._

## 8. Last words ##

Writing this tutorial has taken me a lot of time. But it will be worth it if you make a map for this great mod. If you make a map, please do not keep it to yourself, but release it on our forums for others to enjoy.

[Here is a link to a thread to show off.](http://www.rtcwcoop.com/forum/viewtopic.php?id=147)