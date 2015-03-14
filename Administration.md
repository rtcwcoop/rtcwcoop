# Introduction #
Info about configuration of server Administration for server owners as well as general howto's for server Admins.

## Revision ##
**Last update:** [revision 1114](https://code.google.com/p/bzzwolfsp/source/detail?r=1114).

# Server configuration for Owners #

## Cvars available ##
List of all Admin related cvars:
| **Cvar** | **Default value** | **Explanation** |
|:---------|:------------------|:----------------|
| a1\_pass | "none" | Password for level one Admins |
| a2\_pass | "none" | Password for level two Admins |
| a3\_pass | "none" | Password for level three Admins |
| a1\_tag | "^1Member" | Level one Admin tag |
| a2\_tag | "^3Admin" | Level two Admin tag |
| a3\_tag | "^0Admin" | Level three Admin tag |
| a1\_cmds | "" | List of commands for level one Admins seperated by space |
| a2\_cmds | "" | List of commands for level two Admins seperated by space |
| a3\_cmds | "" | List of commands for level three Admins seperated by space |
| a3\_allowAll | "0" | If enabled, Admin level 3 can execute any Admin command - in that case, a3\_cmds can be used for general server commands (Read in section bellow) |
| adm\_help | "1" | If enabled, any logged in Admin can use !list\_cmds - which will show them list of all commands that are set for their level. |
| g\_extendedLog | "0" | Logs if enabled various Admin actions - read more in g\_extendedLog section. |
| g\_votesPerUser | "1" | Set here how many votes can player call each game. |
| g\_bannedMessage | "You are Banned from this server!" | Message that is printed to banned users. You can use it to point to your forum so they can appeal etc.|
| g\_usePassword | "0" | This controls if g\_password is either used for ban bypassing (if player is caught in banned range) or if enabled, it sets server as private - if there's a password set. |


## Admin commands per level ##
By default owner can set any command to any level.
All the levels are treated equailly - but only level 3 can have all the Admin commands assigned (by setting a3\_allowAll to 1) by default.

You can set any command under cmds cvar. It can be a server command (e.g. g\_allowvote) or an Admin command (check lower for the list of all current Admin commands).

_Basically there are only two things that are important and you should watch for._

**1st:**
There's a limit how many commands you can set per each level, as if you assign to many commands, string will get truncated and non of the commands will work - If that happens, simply remove some of the commands out. Limit should be around 160-170 chars (spaces included).

**2nd:**
Make sure there's a space between each command - cvar!


**Example** of how to set commands per level:
```
seta a1_cmds "slap g_allowVote ignore unignore warn"
seta a2_cmds "slap g_allowVote ignore unignore warn kick clientkick kill nextmap map"

//With a3_allowAll set to 0 (Off)
seta a3_cmds "slap g_allowVote ignore unignore warn kick clientkick cp cpa rconpassword exec"

//Or with a3_allowAll set to 1 (On)
seta a3_cmds "g_allowVote rconpassword g_gametype"
```
_As you can notice, with a3\_allowAll Admin commands don't need to be set since they're all available by default and a3\_cmds cvar can be used for server specific cvars only._

## Extend log ##
Extend log basically just logs what Admins are doing on server.
If enabled, logs are available in your main\COOP folder or if server is running as modded in "YOUR-MOD"\COOP folder.

Following logs are created inside COOP folder when event happens:
| **Log name** | **What it logs** |
|:-------------|:-----------------|
| adminLogins.log | Everytime player logs in as Admin, it prints in log - time, username, ip and a Tag (so you know what level user logged in as) |
| adminLoginAttempts.log | Here you'll find all the login attempts that were made by users when they tried to login on your server with invalid password. |
| adminActions.log | Depending of how it's set, this file logs various Admin actions like kicks, ignores, warnings etc. |
| banBypass.log | Non existen atm but once added in, it will log everyone that will use password to bypass IP range ban. |

**Extended log can be configured in following ways:**
<ul>
<li>0 = Disables logs all together. </li>
<li>1 = Logs only important stuff like kicks, ignores and so on.</li>
<li>2 = Logs everything - that means it will log admin warnings (with message as well), cancelvotes, passvotes, nextmaps etc..basically every command Admin will execute will be logged. </li>
</ul>

# Admin commands #

## Global Commands ##
This commands are available to all and can't be disabled. But some of commands work differently or not at all for non-logged users. Here's a basic explanation of them:
| **Command** | **Usage** | **What it does** |
|:------------|:----------|:-----------------|
| getstatus | /getstatus | Getstatus will loop thru all the players and display basic information. Logged in users (Admins) will see more info then non-logged in players. Non-logged users will see team, name, part of IP (to easier identify abuses if they change their name and reconnect), if Admin is not hidden an Admin tag and if player is Ignored also "Ignored" print. While Admins will see the same with difference that they see full IP's of users as well as any hidden Admins. |
| incognito | /incognito | Incognito is a command Admin can use to toggle viewablity of his presence on server. If admin is hidden, command can be used to reveal his status (will show a tag in chat as well as under getstatus) or if he's not hidden it will do the opposite. |
| login | /login PASSWORD | Login will log user in as Admin and let everyone know (in chat) that (s)he logged in. |
| @login | /@login PASSWORD | @Login will log user in as Admin but wont let others know (wont print in chat). |
| logout | /logout | Logs user out. If user is hidden it wont print in chat otherwise it will show to every player that user logged out. |

## Admin commands (howto) ##
Admin commands are only available to logged in users. What commands Admin has on disposal depends of what level (s)he is as well as what commands server owner set for that level. If Admin help (adm\_help set to 1 (On)) is enabled, Admin can use !list\_cmds to see what commands (s)he has on disposal. <br />

**Using server commands is quite simple**<br />
Simply login as Admin and execute command by typing in chat or console **!command** - make sure it starts with **!** and not **/**. If you don't know either what command does or how to use it, you can use ? instead of ! -> ?command <- which will print info for that specific command.

## List of all current Admin commands ##
| **Command** | **Example usage** | **What it does** |
|:------------|:------------------|:-----------------|
| list\_cmds | !list\_cmds | If enabled it shows all the commands you can use |
| ignore | !ignore <unique part of name> | Will ignore user and prevent him to call votes, chat or use vsay. |
| unignore | !unignore <unique part of name> | Will restore ability to call votes, chat and use vsay |
| clientignore | !clientignore <client slot> | Does the same as ignore, just uses client slot to find a user instead of name. |
| clientunignore | !clientunignore <client slot> | Does the same as unignore, just uses client slot to find a user instead of name. |
| kick |!kick <unique part of name> <optionally - message> | Will kick user from server and print message if there's any, |
| clientkick | !clientkick <client slot> <optionally - message> | Does the same as kick, just uses client slot number to find the user instead of name |
| slap | !slap <client slot> | Will take 20hp from user - if user is not set, you'll slap your self. |
| kill | !kill <client slot> | Will kill user on spot - if user is not set, you'll kill your self.. |
| specs | !specs <unique part of name> | Will force selected user to spectators. |
| coop | !coop <unique part of name> | Will force selected user back to game. |
| exec | !exec server.cfg | Will execute config on a server. Note you can type with or without .cfg extension. |
| nextmap | !nextmap | Will load the nextmap. |
| map | !map swf | Will load the map one sets. |
| cpa | !cpa < text > | Will center print warning message to everyone. |
| cp | !cp <client slot> < text > | Will center print warning message only to targeted user. |
| chat | !chat < text > | Will print warning message in chat to everyone. |
| warn | !warn < text > | Will show warning message to all in center print as well as chat. |
| cancelvote | !cancelvote | Cancells any vote that's currently in progress. |
| passvote | !passvote | Will pass any vote that's currently in progress. |
| restart | !restart | Will restart the map. |
| ban | !ban < unique part of name > | Will add ip of that user to banned file. |
| tempban | !tempban < unique part of name > < minutes > | Will tempban user from server for the time it's set. |
| addip | !addip < ip > | Adds the ip to banned file. Note you can use wildcards -> **stands for 0 to 255 so you can ban sub-range by e.g. !addip 100.**.**.** -> Get more details in [banning tutorial page](banning.md).|

**TIP:** To view client slot number use /getstatus.

## Custom Admin commands ##
By default Admins can have any server command as long as it's in commands string for their level.
This means that owners can assign any command in their level, commands like g\_allowVote or rconpassword.

**USAGE:**
By default, any command that's changed by Admin will be printed to all. If admin does _!g\_allowVote 1_ it will print to everyone that _g\_allowVote_ was changed to _1_. Since some commands are delicate (e.g. rconpassword) and should not be printed to anyone but Admin there's also an option to execute command silently by using **@** as third parameter. Example:

**!rconpassword some-password @**

**NOTE:** There are two things you should note. <br />
**First:** _Silent attribute only works with custom commands, regular commands (listed above) do not have this option unless if specified in their description._<br />
**Second:** _Any custom Admin command does not have help listed, so using something like ?rconpassword will not list brief description for it._



# Example Config #
Since there's quite some reading and some just want to dive into it, I'm posting a sample config with all the cvars mention in this post. Adjust it to your liking. ;)

```
seta a1_pass "low"
seta a2_pass "med"
seta a3_pass "owner" 
seta a1_tag "^1Member"
seta a2_tag "^3Admin"
seta a3_tag "^0Admin"
seta a1_cmds "g_allowvote slap specs ignore unignore clientignore clientunignore"
seta a2_cmds "g_allowvote slap specs ignore unignore clientignore clientunignore kick clientkick coop nextmap"
seta a3_cmds "g_allowvote rconpassword" // Server specific only..
seta a3_allowAll "1" // So level 3 can execute any Admin command.
seta adm_help "1" // Let admins know what they can use by allowing !list_cmds 
seta g_extendedLog "1" // Log essentials only. 
seta g_votesPerUser "2" // 2 votes for user per round are more then enough.
```

That's about it for the moment. :)