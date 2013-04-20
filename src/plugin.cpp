
#include "stdafx.h"
#include "plugin.h"
#include "util.h"
#include "ifaces.h"

#include "openfov.h"
#include "name.h"
#include "advspec.h"
#include "transvm.h"
#include "dlfilter.h"

extern "C" char __ImageBase;


void* CreateInterface( const char* name, int* code )
{
	// Force ourselves to remain loaded
	//static bool skip = false;
	//if ( !skip )
	//{
	//	skip = true;
	//	char path[MAX_PATH];
	//	::GetModuleFileNameA( (HMODULE) &__ImageBase, path, sizeof(path) );
	//	::LoadLibraryA( path );
	//}

	// Check which interface the game is requesting from us
	void* i = NULL;
	if ( !strcmp( name, "ISERVERPLUGINCALLBACKS003" ) )
		i = &COpenPlugin::g;
	return i;
}

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

/*#ifdef OPENPLUGIN_INSECUREBYPASS
	COpenPlugin::g.DoMagic();
#endif // OPENPLUGIN_INSECUREBYPASS*/
}

#ifdef USE_FANCY_CPP_FEATURES
void DelayLoadPlugin()
{
	// Alternatively just hook IBaseClientDLL::Init and wait for it but oh well...
	while ( !Ifaces.pCvar->FindVar( "viewmodel_fov_demo" ) )
	{
		std::chrono::milliseconds sleeptime(500);
		std::this_thread::sleep_for(sleeptime);			
	}

	PluginInit();
}
#endif // USE_FANCY_CPP_FEATURES

COpenPlugin COpenPlugin::g;

bool COpenPlugin::Load( CreateInterfaceFn pfnAppSystem, CreateInterfaceFn pfnServer )
{
	// Start by initing the ifaces
	if ( !Ifaces.Init(pfnAppSystem) )
		return false;

#ifdef USE_FANCY_CPP_FEATURES
	std::thread delay_thread(DelayLoadPlugin);
	delay_thread.detach();
#else
	PluginInit();
#endif // USE_FANCY_CPP_FEATURES

	return true;
}
void COpenPlugin::Unload( void )
{
	COpenFOV::g.Close();
	CNameEnabler::g.Close();
	//CSpectator::g.Close();
	CTransVM::g.Close();
	CDownloadFilter::g.Close();
/*#ifdef OPENPLUGIN_INSECUREBYPASS
	COpenPlugin::g.UndoMagic();
#endif // OPENPLUGIN_INSECUREBYPASS
*/
// Workaround for a problem with std::thread on Windows
// When using std::thread, the first call to FreeLibrary (made by the engine) won't unload the module
// We need to call FreeLibrary a second time to properly unload it
#if !defined _LINUX && defined (USE_FANCY_CPP_FEATURES)
	MEMORY_BASIC_INFORMATION mbi; 
	static int address; 
	VirtualQuery(&address, &mbi, sizeof(mbi)); 
	FreeLibrary((HMODULE)mbi.AllocationBase);
#endif
}

const char* COpenPlugin::GetPluginDescription( void )
{
	return "OpenPlugin version 1.4 by Casual! Build " __DATE__ " " __TIME__ ;
}
/*
#ifndef _LINUX
void COpenPlugin::DoMagic()
{
	// Magic!
	bMagic = NULL;
	if ( void* p = SigScan( Ifaces.hmEngine,
		"\x88\x1D\x00\x00\x00\x00\x8B\x4D\x08\x51\x68\x00\x00\x00\x00\xFF\x15\x00\x00\x00\x00\x83\xC4\x08",
		"\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF" ) )
	{
		bMagic = *reinterpret_cast<bool**>( (char*)p + 2 );
		*bMagic = true;

		// Hook plugin_load
		plugin_load = Ifaces.pCvar->FindCommand( "plugin_load" );
		fnOrigPload = plugin_load->fnCommandCallback;
		plugin_load->fnCommandCallback = &MyPluginLoad;
		CommandLine_Tier0()->RemoveParm("-insecure");
	}
}
void COpenPlugin::UndoMagic()
{
	if ( bMagic )
	{
		// Unhook plugin_load
		plugin_load->fnCommandCallback = fnOrigPload;
		// Makes debugging easier without having to restart tf2 all the time...
		*bMagic = false;
	}
}
void COpenPlugin::MyPluginLoad( const CCmdArgs& args )
{
	// Magicks
	bool* b = g.bMagic;
	*b = false;
	g.fnOrigPload( args );
	*b = true;
}
#endif
*/


// Blahblahblah not interested

void COpenPlugin::Pause( void ) { }
void COpenPlugin::UnPause( void ) { }
void COpenPlugin::LevelInit( char const *pMapName ) { }
void COpenPlugin::ServerActivate( void *pEdictList, int edictCount, int clientMax ) { }
void COpenPlugin::GameFrame( bool simulating ) { }
void COpenPlugin::LevelShutdown( void ) { }
void COpenPlugin::ClientActive( void *pEntity ) { }
void COpenPlugin::ClientDisconnect( void *pEntity ) { }
void COpenPlugin::ClientPutInServer( void *pEntity, char const *playername ) { }
void COpenPlugin::SetCommandClient( int index ) { }
void COpenPlugin::ClientSettingsChanged( void *pEdict ) { }
int COpenPlugin::ClientConnect( bool *bAllowConnect, void *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ) { return 0; }
int COpenPlugin::ClientCommand( void *pEntity, const void *args ) { return 0; }
int COpenPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ) { return 0; }
void COpenPlugin::OnQueryCvarValueFinished( int iCookie, void *pPlayerEntity, int eStatus, const char *pCvarName, const char *pCvarValue ) { }
void COpenPlugin::OnEdictAllocated( void *edict ) { }
void COpenPlugin::OnEdictFreed( const void *edict ) { }
