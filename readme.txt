OpenPlugin is an open sourced plugin doing various things to the tf2 client.
Taking feature requests.

Version 1.4

As some people seem to have been VAC banned for it recently, we no longer provide binaries, you have to compile it yourself.

1) How to compile
=================

Windows

Get Visual C++ 2012 (express is free and works just fine).
https://www.microsoft.com/visualstudio/eng/downloads#d-2012-express

Open src/OpenPlugin.sln
Change 'Debug' (in the toolbars) to 'Release' and press F7 (or Build->Build Solution).

Linux

Make sure you have clang or gcc installed. Modify the CC= line in the Makefile to set the one you want to compile with.
Open a terminal and change the directory to the src folder, type "make" and it will compile the library.


1) How to install
=================

Windows

You can extract the content of the zip anywhere on your hard drive. Just launch OpenPlugin.bat once, this will set up
the environnement variable to allow the game to find hfx.dll

Linux

You need to put bin/OpenPlugin.so in your Steam/SteamApps/<account>/Team Fortress 2/bin folder.
In Steam, right click on TF2, properties, and set launch options. You may already have launch options, for example:
-novid -console
You need to change it to:
LD_PRELOAD=OpenPlugin.so %command% -novid -console

3) Features
===========

Allows fov_desired to change your fov up to 130, cannot be changed while connected to a server.
Removes restriction from mat_picmip and some more.
Makes the 'name' cvar completely indepedent from your steam name.
Transparent viewmodels with r_drawviewmodel <alpha>. 0 is disabled, 1 is normal, up to 255 (with 255 being fully opaque).



4) License & source code
========================

Source code included, should compile without any extra effort under Visual Studio C++ 2012 (express)
Released under GNU GPL ( http://www.gnu.org/licenses/gpl.html ) (let's pretend that notice is in every source file), permission is granted to Valve developers to copy & paste any part of it in any of their games.

Release build should compile into a tiny dll :)



5) Credits
==========
Made by Casual, thanks to AnAkIn!
