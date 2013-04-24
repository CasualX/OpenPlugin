OpenPlugin is an open sourced plugin doing various things to the tf2 client.
Taking feature requests.

Version 1.4

Since Valve the previous versions you can be pretty sure they don't actually like us doing it. USE AT YOUR OWN RISK!

1) How to install
=================

Windows

First, make sure you have Visual C++ Redistributable 2012 installed. You can download it here:
http://download.microsoft.com/download/1/6/B/16B06F60-3B20-4FF2-B699-5E9B7962F9AE/VSU1/vcredist_x86.exe

You need to put bin/hfx.dll in your Steam/SteamApps/<account>/Team Fortress 2/bin folder.

Linux

You need to load it using Source engine's neat plugin system.
To have the engine automatically load the plugin on game launch, put addons/OpenPlugin.so and addons/OpenPlugin.vdf in the Steam/SteamApps/<account>/Team Fortress 2/tf/addons folder.

2) How to compile
=================

This is a source code only release!
Have a friend compile it for you if you don't know how yourself.

Get Visual C++ 2012 (express is free and works just fine).
https://www.microsoft.com/visualstudio/eng/downloads#d-2012-express

Open src/OpenPlugin.sln
Change 'Debug' (in the toolbars) to 'Release' and press F7 (or Build->Build Solution).
Output can be found in src/Release/OpenPlugin.dll


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
