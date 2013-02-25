// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"

#include "plugin.h"
#include "ifaces.h"
#include "util.h"

#include "openfov.h"
#include "name.h"
#include "advspec.h"
#include "transvm.h"
//#include "protect.h"
#include "dlfilter.h"

pthread_t delay_thread;

inline ConVar* UnlockVar( const char* name )
{
	ConVar* var = Ifaces.pCvar->FindVar( name );
	if ( var )
	{
		// WARNING!
		// Servers can query the values of your cvars (if you don't hide them)
		// This allows them to detect if you've changed cvars you normally shouldn't be able to
		// Ex. you can remove the cheat flag from r_drawothermodels and have a primitive wallhack
		//     but you're gonna get banned pretty quickly when a server has a competent anti-cheat
		var->bHasMax = false;
		var->bHasMin = false;
		var->nFlags &= ~FCVAR_CHEAT;
	}
	return var;
}
inline void CleanCommand( const char* name )
{
	if ( ConCommand* cmd = Ifaces.pCvar->FindCommand( name ) )
	{
		cmd->nFlags &= ~FCVAR_CHEAT;
	}
}
void PluginInit()
{
	Msg( "Loading OpenPlugin by Casual...\n" );

	// Init features
	if ( !COpenFOV::g.Init() )
	{
		Msg( "OpenFOV::Init() Failed!\n" );
	}
	if ( !CNameEnabler::g.Init() )
	{
		Msg( "NameEnabler::Init() Failed!\n" );
	}
	if ( !CTransVM::g.Init() )
	{
		Msg( "TransViewModel::Init() Failed!\n" );
	}
	if ( !CDownloadFilter::g.Init() )
	{
		Msg( "DownloadFilter::Init() Failed!\n" );
	}

	//if ( !CSpectator::g.Init() )
	//{
	//	Msg( "Spectator::Init() Failed!\n" );
	//}
	
	UnlockVar( "mat_picmip" );
	UnlockVar( "viewmodel_fov_demo" );

	// Assist movie makers so POV demos do not constantly force change the crosshair etc...
	for ( ConCommandBase* p = Ifaces.pCvar->GetCommands(); p; p = p->pNext )
	{
		p->nFlags |= FCVAR_DONTRECORD;
	}

	// Disables checks on r_lod & r_rootlod
	// OLD:
		//"\x80\x3D\x00\x00\x00\x00\x00" "\x74\x03" "\xB0\x01" "\xC3" "\xA1\x00\x00\x00\x00" "\x83\x78\x30\x00" "\xC6\x05"
		//"\xFF\xFF\x00\x00\x00\x00\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF" "\xFF\x00\x00\x00\x00" "\xFF\xFF\xFF\xFF" "\xFF\xFF"
		// + 0x2
#ifndef _LINUX		
	if ( char* p = (char*)SigScan( Ifaces.hmEngine,
		"\x80\x3D\x00\x00\x00\x00\x00" "\x74\x03" "\x33\xC0" "\xC3",
		"\xFF\xFF\x00\x00\x00\x00\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF" ) )
	{
		bool* pb = *reinterpret_cast<bool**>( p+0x2 );
		*pb = true;
	}
#else
	if ( char* p = (char*)SigScan( (void*)Ifaces.pfnEngine,
		"\x55" "\x31\xC0" "\x89\xE5" "\x83\xEC\x18" "\x80\x3D\x00\x00\x00\x00\x00" "\x75\x50",
		"\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF\xFF" "\xFF\xFF\x00\x00\x00\x00\x00" "\xFF\xFF" ) )
	{
		bool* pb = *reinterpret_cast<bool**>( p+0xA );
		*pb = true;
	}      
#endif
	// Disable mat_picmip checks, ends up in CheckSpecialCheatVars()
	// OLD: "\xA1\x00\x00\x00\x00\x85\xC0\x75\x1B\x8B\x0D\x00\x00\x00\x00\x8B\x01" + 0x1
#ifndef _LINUX	
	if ( char* p = (char*)SigScan( Ifaces.hmEngine,
		"\xA3\x00\x00\x00\x00\x3B\xC6\x74\x0D\x6A\x02\x6A\xFF",
		"\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" ) )
	{
		ConVar** cvar = *(ConVar***)( p+1 );
		*cvar = Ifaces.pCvar->FindVar( "joy_advanced" );
	}
#else
	if ( char* p = (char*)SigScan( (void*)Ifaces.pfnEngine,
		"\x55\x89\xE5\x83\xEC\x18\xA1\x00\x00\x00\x00\x85\xC0\x74\x31\xC7",
		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF" ) )
	{
		ConVar** cvar = *(ConVar***)( p+7 );
		*cvar = Ifaces.pCvar->FindVar( "joy_advanced" );
	}
#endif
	
#ifdef OPENPLUGIN_INSECUREBYPASS
	COpenPlugin::g.DoMagic();
#endif // OPENPLUGIN_INSECUREBYPASS
}
void PluginShutdown()
{
	COpenFOV::g.Close();
	CNameEnabler::g.Close();
	//CSpectator::g.Close();
	CTransVM::g.Close();
	CDownloadFilter::g.Close();
#ifdef OPENPLUGIN_INSECUREBYPASS
	COpenPlugin::g.UndoMagic();
#endif // OPENPLUGIN_INSECUREBYPASS
}

#ifndef _LINUX
DWORD WINAPI DelayLoadPlugin( void* )
#else
void *DelayLoadPlugin( void * )
#endif
{
	// Alternatively just hook IBaseClientDLL::Init and wait for it but oh well...
#ifndef _LINUX
	do Sleep( 500 );
#else
	do sleep( 1 );
#endif
	while ( !Ifaces.pCvar->FindVar( "viewmodel_fov_demo" ) );
		//Sleep( 500 );
	PluginInit();
#ifndef _LINUX
	return 0;
#else
	pthread_exit(NULL);
#endif
}

//
// So why the hell are we loading in here and not in COpenPlugin::Load() ?
// Simple, this code will ALWAYS be called upon being injected
// whereas the plugin interface will only be called when loaded using the plugin interface.
//
#ifndef _LINUX
BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:

		// Start by initing the ifaces
		if ( !Ifaces.Init() )
			return FALSE; // Wrong process?

		// When loaded using a .vdf file we get loaded REALLY early and some functions aren't available in which case we delay loading the plugin.
		// Thanks for pointing that out, AnAkIn :)
		if ( Ifaces.pCvar->FindVar( "viewmodel_fov_demo" ) )
		{
			PluginInit();
		}
		else
		{
			::CreateThread( NULL, 0, &DelayLoadPlugin, NULL, 0, NULL );
		}
		break;

	case DLL_PROCESS_DETACH:
		PluginShutdown();
		break;
	}
	return TRUE;
}
#else
void LoadFunc()
{
		// Start by initing the ifaces
		if ( !Ifaces.Init() )
			return; // Wrong process?

		// When loaded using a .vdf file we get loaded REALLY early and some functions aren't available in which case we delay loading the plugin.
		// Thanks for pointing that out, AnAkIn :)
		if ( Ifaces.pCvar->FindVar( "viewmodel_fov_demo" ) )
		{
			PluginInit();
		}
		else
		{
			pthread_create(&delay_thread, NULL, DelayLoadPlugin, NULL);
		}
}

void UnloadFunc()
{
		PluginShutdown();
}
#endif

