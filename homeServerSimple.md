# About #
This is a no nonsense tutorial with no explanations but only steps one needs to take in order to get a home server running. It's aimed at experienced users that only need the directions and steps and know how to solve everything on their own.

# Steps #
  * Open the router port
  * If single port set to 27964 if range set to 27960-27980
  * Protocol: UDP
  * If IP is required then the local IP of the box that will run it.
  * Unblock the same ports in your Firewall or any other software that controls or blocks the ports
  * Create new batch file and dump in:
  * RTCWCoopDED.x86.exe +net\_ip YOUR-EXTERNAL-IP +net\_port PORT +set dedicated 2 +map swf

**Note:** If you assigned a range then under PORT specify a port inside that range, if you assigned a single port (example was _27964_) then add _27964_ under net\_port.

Additional parameters:
  * +exec server.cfg
  * A way to control server settings like server name etc..
  * +exec mod.cfg
  * A way to divide game settings from server and dump stuff in it.
  * +fs\_game SOME-FOLDER
  * A mod name if you wish to create a modded server..just note no spaces are allowed as well as if you set this, then dump server.cfg and (if you created and use) mod.cfg inside this folder.

_That's it. You're good to go._