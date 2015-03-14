# Introduction #

This tutorial will cover the basic steps to get your game running and visible on the internet. Please be aware that this is for Windows users only. It's safe to assume that anyone running OSX or Linux already knows all this in a OS equivalent way. Names for binaries used here are for RTCW COOP version 0.9.9 - If it's currently unavailable or there's a newer version out, just adjust the name in (SERVER section) startup line to match the binaries.

# Note #
This is by no means a full blown _everything and a kitchen sink_ tutorial. I will explain basic stuff with assumption that you;
  * 1: Have a at least a very basic understanding about networking,
  * 2: You don not have the basic knowledge of networking but are willing to devote few minutes or an hour of your time and get familiar with it by googling about any problems you experience.

If you can't do one or the other, you can jump to forums but chances are, we wont be able to help you..as concept of a home server is the same all around but ways of doing it vary.

# About :: Basics #
Running a server falls down to two different ways. One is that you run it through a game - by running a game and allowing others to join and two, that you run it as an actual server - through console. This tutorial will focus on 2nd approach.

# Preliminary :: Ports #
Before we jump into how to actually run it, I want to clarify few things to you so you'll be able to cope with problems along the way.

Every game has a default port on which it runs. Game usually runs on that port by default and will only use different port when you specifically tell it so, or when that port is already taken.

COOP's default port is **27960**. Every time you run the server, it will (unless otherwise specified) start listening on that port. If that port is taken, it will automatically increase it by 1 meaning that if port **27961** is free, it will automatically binds it self to it, otherwise it will keep increasing it by a factor of 1, till it finds a free port and binds to it.

How does the port effects you? Generally in any multi-host environment you assign ports your self, so they never collide and you always know which port is taken by the process (i.e. game server). In a home environment where you basically run a game and a server together you do not bother with that, but here's a catch you have to look for. Port assigned to a server depends of two factors;

  * Are you running more instances at the same time (game + server, multiple servers etc)
  * If so, which you ran first?

Thing is. If you run your server first it will bind it self to default port (**27960**). If you run a game after it, it will check if default port is available, since server already occupies it, it will bind it self to **27961**. If you run another instance of the game or a server, it will bind it self to a **27962** port etc..so as you see, order matters.

_So in a nutshell, why is this important?_
If you open (allow) a single port and not a range of ports in your router/firewall settings then that's something you have to keep in mind. As order of how you start the things will also affect which port will be assigned to it.

A brief example - In your router you open the port 27960 but then you first run the the game and after it a server..as it should be clear by now, in this scenario your server would actually be on port 27961 and since you run the game on your default port (remember, it was there first), your friend(s) wont be able to join as server is not listening on that port and game will naturally ignore any incoming connection requests.

# Router #
Chances are, your router is blocking the port by default. So what you'll have to do is enable (allow) the port or port range in order for your server to actually reach the internet.

This is a simple process but there's no straight answers as there's as many ways of doing things as there's different router vendors and models. Each router has slightly different approach so this is the part where you have to put some effort in.

Generally port settings reside in either _Application & Gaming_ or _Security_ section - in some cases it may be in completely different section. What you are basically looking for is usually called something like **Port forwarding** or something in that manner. Your sole task here it to allow/enable/open the port you wish.

By default it will probably offer you _starting port_, _ending\_port and Protocol (UDP, TCP or in same cases BOTH). If you can set a range (from-to) then add in, 27960 (start), 27980 (end) and select UDP as protocol. That's about it. If you can only set a single port, then set 27960 but in that case, note that as it was mentioned above, you need to start server first (remember- order matters)._

NOTE:
  * If you have to set an IP as well (route to etc) then you set your lan IP of computer that runs the server, game..
  * If you have hard time figuring out this you can try with **DMZ** (demilitarized zone) and set LAN IP of the computer..but note that in that cases, all the ports and everything will be exposed on the internet.

TIP:
If you have hard time following this section, then write down your router model and type in google "YOUR-ROUTER-MODEL forwarding ports" and check the corresponding tutorials..there's bound to be a sea of them so invest some time by going through them and solve this.

# Firewall #
Unless if you run a strict environment or an actual server this section is completely useless. When you run the game, you will usually get a firewall pop up asking you, if you allow this process to connect to the internet..simply click yes and you're done.

If you run a relaxed completely blatant system it wont even bother to ask you.. But, if you run a strict environment then chances are, you will have to add incoming and outgoing port to your firewall rules. How to do that is beyond this tutorial but google it and you'll find instructions instantly...and for the record in that case settings are:

  * PORT: 27960
  * PROTOCOL: UDP
  * Set _incoming_ and _outgoing_ to _Allow_

# Setting a server #
I will not discuss here about how to configure your mod or what and why you should do. I will only address the first and most essential part, that startup line.

  * In COOP folder Create batch file (it's easy so just google it if you do not know how).
  * Add in bach file the following line:
  * _RTCWCoopDED.x86.exe +set dedicated 2 +map swf_

NOTE: Replace RTCWCoopDED.x86.exe with RTCWCoopDED.x86\_64.exe if you're running 64bit version.

Save it and simply run the file. If window pops up asking you if you wish to add this service to allowed firewall list simply click yes. That's it you're done. If you unblocked all the ports correctly and if you set a range rather then single port then your server should be now visible on the internet.
If you set a single port and it's 27960 then make sure you don't have any instances of COOP running and run the server first, before you run the game.

## Additional ##
What this tutorial doesn't cover are additional parameters you can pass through startup line..there's basically anything game offers + few extra. Ones you may be interested are _net\_ip_ and _net\_port_. This parameters are used if you want to force your game to always use a specific port but in that case you need to add your actual (WAN/Internet) IP to net\_ip variable and port you wish to use to your net\_port variable.

To give you a basic example, let's say your IP is 100.100.100.100 and you wish to run the game on a port 27999..you would simply add to startup line at the end of this the following;

_+net\_ip 100.100.100.100 +net\_port 27999_

That's about it. Just note following;
  * If you're on a dynamic IP you will have to adjust net\_ip when your IP changes,
  * Make sure that port specified here is open in your router and/or any other software (firewall, anti-virus etc..),
  * IP specified here has to be your WAN (external a.k.a. actual) IP and not your Local (LAN) IP.

# Norton, Anti-viruses, ... #
If you done all the steps above and game works don't bother with this, otherwise continue..

There's no complete guide for this. If you run anything that's taking control over your firewall (norton has this nasty habit) or "complete protection suit" of any kind..then make sure you check their forums about ports and how they handle it. Note that some applications allow you to set them to "Gaming mode" or similar..in that case they'll allow any connections and stop blocking your ports..so check if that options is available before you start digging forum threads you found though google.

**Conclusion**
This tutorial is completely basic but often people simply don't know, so in that case I hope you find some use of it. If you encounter anything that's not explained here, I suggest you google that bit and learn about it. Learning how to make batch files, find out what's your local IP and stuff alike are complete basics and google is full of explanations about it.

As usual, if all else fails visit the forums and we'll go from there on.

  * Forums: http://rtcwcoop.trak.be/forum/index.php
  * Author: Nate 'L0,