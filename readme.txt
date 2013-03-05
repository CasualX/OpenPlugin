OpenPlugin is an open sourced plugin doing various things to the tf2 client.
Taking feature requests.

Version 1.4



1) How to compile

This is a source code only release!
Have a friend compile it for you if you don't know how yourself.

Get Visual C++ 2012 (express is free and works just fine).
https://www.microsoft.com/visualstudio/eng/downloads#d-2012-express

Open src/OpenPlugin.sln
Change 'Debug' (in the toolbars) to 'Release' and press F7 (or Build->Build Solution).
Output can be found in src/Release/OpenPlugin.dll

NOTE! The above is from my old readme.
I now provide a precompiled version, copy the addons folder to your tf folder.



2) How to install

Once you have your OpenPlugin.dll, you need to load it using Source engine's neat plugin system
Add -insecure to your launch params, the plugin will bypass the check that disallows joining VAC secured servers.
Since Valve broke the signature of AnAkIn's previous version you can be pretty sure they don't actually like us doing it. USE AT YOUR OWN RISK!
To have the engine automatically load the plugin on game launch, in put the .dll in tf\addons and create a new text file called OpenPlugin.vdf, in there paste:
Plugin
{
    File    "addons/OpenPlugin"
}
If your TF2 crashes after an update or won't let you join VAC secured servers, remove the plugin and remove -insecure from your launch params.

3) Features

Allows fov_desired to change your fov up to 130, cannot be changed while connected to a server.
Removes restriction from mat_picmip, viewmodel_fov_demo and some more.
Makes the 'name' cvar completely indepedent from your steam name.
Added 'mapsonly' to cl_downloadfilter. Will only download .bsp files.
Transparent viewmodels with r_drawviewmodel <alpha>. 0 is disabled, 1 is normal, up to 255 (with 255 being fully opaque).



4) License & source code

Source code included, should compile without any extra effort under Visual Studio C++ 2010 (express)
Released under GNU GPL ( http://www.gnu.org/licenses/gpl.html ) (let's pretend that notice is in every source file), permission is granted to Valve developers to copy & paste any part of it in any of their games.

Release build should compile into a tiny dll :)



5) Credits

Made by Casual, thanks to AnAkIn!
