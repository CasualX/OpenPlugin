
#include "stdafx.h"
#include "plugin.h"
#include "util.h"
#include "ifaces.h"

#include "openfov.h"
#include "name.h"
#include "transvm.h"

extern "C" char __ImageBase;

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

	UnlockVar( "mat_picmip" );

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
}

void DelayLoadPlugin()
{
	while ( GetHandleOfModule( "bin/ServerBrowser" ) == NULL )
	{
		std::chrono::milliseconds sleeptime(500);
		std::this_thread::sleep_for(sleeptime);		
	}
	
	void *hmEngine = GetHandleOfModule( "engine" );
	
	CreateInterfaceFn pfnEngine = (CreateInterfaceFn) GetFuncAddress( hmEngine, "CreateInterface");
	CreateInterfaceFn pfnAppSystem = NULL;

#ifndef _LINUX	
	if ( void* p = SigScan( hmEngine,
			"\x83\xC4\x04\x8B\x11\x50\xFF\x35\x00\x00\x00\x00\x8B\x02\xFF\xD0",
			"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF" ) )
	{
		pfnAppSystem = **(CreateInterfaceFn**)( (char*)p + 8 );
	}
#else
	if ( void* p = SigScan( (void*)pfnEngine,
			"\x8B\x91\x84\x00\x00\x00\x8B\x0A\x89\x44\x24\x08\xA1\x00\x00\x00",
			"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00" ) )
	{
		pfnAppSystem = **(CreateInterfaceFn**)( (char*)p + 0xD );
	}
#endif

	if(!pfnAppSystem)
		return;

	// Start by initing the ifaces
	if ( !Ifaces.Init(pfnAppSystem) )
		return;

	PluginInit();
}