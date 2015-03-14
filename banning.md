# Banning #

Banning in COOP is based around IPs. It's not perfect but for the most of the part it does it's job. In order to cope with IP banning you should get familiar with how it works as well as how IP concepts work in real world. This tutorial will explain few things and hopefully after it, you should have a better idea on how to approach this.


# Banning players #
Banning players can be done in two ways.

  * In game (Thru command)
  * Manually (By editing banned.txt file)

## In game ##
In game you can have 2 tools at your disposal.
You can either ban a user by using !ban command or by adding IP range to banned file with !addip command. Both commands are the same in idea but different in approach.

  * Ban command
> > Ban command will take users full ip, add it in banned file and kick user.
  * Addip command
> > Addip command will only add IP in banned file. Player will still have to be kicked by using kick command.

While ban command may seem more useful it has it's short comings. Problem with ban command is that it can only add full IP and not ranges. Now days most of users are on dynamic range so they can evade this by simply flushing their network settings and bypass ban with new IP. That is where addip comes to effect. In order to ban user you can specify a range, that will solve the IP flushing and ensure player is still banned once (s)he tries to connect.

### Addip range bans ###
Ranges are supported. You can specify a range by replacing value with asterisk. Let's say for example you want to ban whole Brazil..Brazil users mostly come between range (from) 201.1.1.1 (to) 201.255.255.255 - In this case, player from this block can get any range inside it assigned. So if you want to ban everyone from this range you simply solve this by adding in banned file: `201.*.*.*`  This would basically ban anyone coming from this range.

Generally, you want to avoid banning large ranges and rather try to find a way to get all the ranges user is coming from..as higher is the range, greater are the chances innocent player will be caught in that range and as a result, banned as well.
_Finding IP ranges and ban bypass dealing is explained lower.._

## Manually ##
One way of dealing and usually later on, as you get more familiar with banning, mostly used approach is by adding IPs directly to banned file over ftp or thru some kinda of web interface. Manually adding ip's is straight forward but there are few key things you have to watch for or as a result, everyone can either end up being banned or banning all together wont even work.

  1. Each IP has to be on it's own line, if you delete ip from banned file make sure there's no spaces or new lines left as next ip should be where one you deleted was.
  1. List must end with new (blank) line. Banning code expects one IP per line and a blank new line at the end when adding new IP. To make sure it's all as it should be, simply go to the last ip on the list, delete any and all spaces after it, press enter so new line gets created and save it.

# IP Ranges #
This is the most trickiest part. It's not a hard too figure it out, but requires patience and persistence.

## Approach ##
In order to improve your success rate, you'll have to learn how to find player's dynamic ranges so they can be banned. Finding a range can be a nerve breaking process. Finding out what block (range) country uses is not a problem, but what range user is assigned to is. Generally those blocks are distributed amongst ISP's which additionally, split the blocks and assign ranges to users. To give you a presentation in pseudo code - we'll take Brazil for example again:

  1. Brazil gets range 201.1.1.1 - 201.255.255.255
  1. Authority splits this ranges in chunks and give if to various ISP's there.
  1. ISP gets a range (let's say from 201.80.1.1. to 201.100.1.1.)
  1. ISP then additionally splits that range in sub ranges and assignee it to users.
  1. By the time IP range comes to user..they may end up having an assigned range between 201.84.1.1 - 201.87.255.255.

In this case scenario, you would simply range ban user by adding;

`201.84.*.*`

`201.85.*.*`

`201.86.*.*`

`201.87.*.*`

If you're lazy you could just ban whole 201 range..but that would render everyone from Brazil being banned so don't use high ranges. Use lower ones and increase them if and when needed...

## Technical mumbo jumbo ##
While you should by now understand how ranges are assigned, that's not generally how you hunt for user's range as that would be near to impossible task. Instead of figuring out what block country->isp->user is assigned to, you rather do an IP lookup and try to find out which range that IP is assigned to.

There are various methods and non is better than other nor there's isn't any universal way of how to deal with it. But you can start with basics and then polish your approach as you go with it.

A basic way to find somewhat estimated assigned range could be like this.
  1. Go to this page: http://wq.apnic.net/apnic-bin/whois.pl
  1. Enter a full IP
  1. Once site loads, part you're interested in is **inetnum**

Please note that you cannot fully relay on this, but it does give you some pointers so you can give it a go and see if it helps.

To give you live example, let's say you want to ban a user that had an IP 84.203.20.40. If you query this IP inet would give you range between 84.203.0.0 - 84.203.127.255 so to issue a ban range you could simply add in banned file (manually or thru addip command) the following range: `84.203.*.*`

NOTE: That most of ip lookup sites could not be relayed on, and don't even bother with any geo location related sites..as they're mostly useless as well as incorrect. Site above so far has fairly accurate range information but that can change at any time..if it does, you'll have to find a better site or a different method etc..Google is your best friend with that. :)

# Bypassing ban #
COOP offers a way for caught in range players to bypass banned range by using a password admin has set on server.

Ban bypassing works in 2 ways;
  * Server owners perspective
  * Players perspective

## Server owners ##
Server owner has to configure the server in a way it supports ban bypass.

## Settings ##
In order to ensure server is ready to deal with banning make sure following settings are configured right;

g\_usePassword
  * g\_usePassword is basically a wrapper. Since banning code utilizes an already existing structure that's primarily meant for toggling between a pub and a private server (g\_password..) this cvar was placed to cope with it. If you have g\_usePassword enabled, server behaviour will be set as normal (non-banning ability) and will then additionally come down to normal behaviour - if password is set, it will act as a private server (player needs password to enter) or if no password is set, it's a public server and everyone can enter. If g\_usePassword is disabled, then "private" function will be utilized for banning and bypassing process. If you want to use banning feature, make sure this is off (by default it is).
g\_password
  * g\_password has a same property in private server as in banning process. In either case it's used to control who can enter. But **NOTE**, g\_password behaviour was altered. Now it supports multiple passwords. You can now add more passwords in string and then give different passwords to different users. Reason for it is that you may want to "level" your players. If you for example wish to use password for known and trusted users that are caught in the range, you give them one password and for players that appeal about being caught in the range and claim they are not in fact the ones you primary banned, you can give them alternative password...reason is that if you find out user was lying or password at any time got leaked, you can simple remove that password and not distribute new password to everyone but only those that it affects.
g\_bannedMSG
  * g\_bannedMSG is basically a string that's printed to a banned user. You can change it to point to your forum...by default it prints "you are banned from this server", you can alter it so it prints "banned - visit your-forum to appeal" etc..

_**NOTE:**_ _Ability to ban servers falls down to either if Admin is allowed to ban or not..that basically depends if level that admin has, can use command or not..check Admin tutorial about configuring Admins if you're not already familiar with it._

## Users perspective ##
If user finds him/her self banned, (s)he'll need to find a way to obtain a password (etc by g\_bannedMSG pointing to forum where they can ask for it). Once user obtains password there's two ways how to utilize it.
  1. User can when (s)he connects to a server and gets your banned message open a console and type /password PASSWORD to bypass it.
  1. Better approach is to simply add in autoexec in a new line - **password PASSWORD** as that will automatically kick in once user connects and solve the need to manually type it each time.

## Notes ##
  1. It's important that g\_password always has a password - unless if owner doesn't wish to use banning system and wants to run server public. If there's no password set banning will not take affect and let every user in no matter if in banned range or not.
  1. To set multiple password you need to separate them with space. An example, in server config file it would look like `seta g_password "password1 password2 password3 password4"`

# Tips #
Along the way you'll find your own way how to deal with banning..either by using commands, manually editing it etc. Following tips may prove some assistance when it comes to dealing with bans.

  1. Use /getstatus to obtain full ip
  1. When you wish to ban user on server, do a /getstatus, issue a !ban command so full ip is printed in log..if player reconnects with new IP, use !addip to range ban him. Idea is that if you're on a server you don't really want to waste time dealing with banning but rather play the game. Reason you first issue a ban and then add range (addip) is, that once you're done with playing, you can open banned.txt file, copy full IP (then delete it) and then use that IP to find the range..as if you instantly add a range, you may need to at times resolves back to checking the log for a full IP to get more accurate range info about that IP.
  1. Utilize multiple passwords feature..if you wish to toggle between pub and private server, you can easily distribute one password to anyone you want so they can utilize it when server is private..and while server is public, you distribute other passwords to ones that are caught in range and wish to bypass it.

# Conclusion #
This about sums it up. If you came this far then you probably realized that banning is not bullet proof nor that there isn't any easy or universal way of how to deal with bans. It's a painstaking process and requires strong nerves and mostly a positive attitude. Along the way you'll learn how to become better and deal with fools faster...but may take some time to get used to it. In either case, figuring out bans helps with understanding how networking on some basic level works so there's some advantages there as well. ;)

In either case, I could write more about this but it's way too long as it is and wouldn't really contribute nothing but more confusion to the tutorial. So to sum it up, I hope this tutorial give you some insight and cleared the issue on how to deal with bans..if not there's a forum (http://rtcwcoop.trak.be/forum/) so ask there.. :)


Gl with banning and hf! :)
Nate L0


_P.S. I wrote this while on coffee so bare with grammar and typo's.._ =)