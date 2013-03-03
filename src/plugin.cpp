
#include "stdafx.h"
#include "plugin.h"
#include "util.h"
#include "ifaces.h"

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

COpenPlugin COpenPlugin::g;

bool COpenPlugin::Load( CreateInterfaceFn pfnAppSystem, CreateInterfaceFn pfnServer )
{
	// Loading is done in DllMain
	return true;
}
void COpenPlugin::Unload( void )
{
	//::FreeLibrary( (HMODULE) &__ImageBase );
}
const char* COpenPlugin::GetPluginDescription( void )
{
	return "OpenPlugin version 1.4 by Casual! Build " __DATE__ " " __TIME__ ;
}
void COpenPlugin::GameFrame( bool simulating )
{
	//if ( simulating )
	//{
	//	CJumpMod::g.Tick( );
	//}
}



#ifndef _LINUX
void COpenPlugin::DoMagic()
{
	// Magic!
	// OLD:
		//"\x81\xEC\x00\x00\x00\x00\x90\x80\x3D\x00\x00\x00\x00\x00\x0F\x85\x00\x00\x00\x00\x80\xBC\x24\x00\x00\x00\x00\x00\x0F\x85"
		//"\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF"
		//+0x2D
	bMagic = NULL;
	if ( void* p = SigScan( Ifaces.hmEngine,
		"\x80\x3D\x00\x00\x00\x00\x00\x75\x00\x56\x68\x00\x00\x00\x00\xFF\xD7\x83\xC4\x08",
		"\xFF\xFF\x00\x00\x00\x00\xFF\xFF\x00\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF" ) )
	{
		bMagic = *reinterpret_cast<bool**>( (char*)p + 2 );
		*bMagic = true;

		// Hook plugin_load
		plugin_load = Ifaces.pCvar->FindCommand( "plugin_load" );
		fnOrigPload = plugin_load->fnCommandCallback;
		plugin_load->fnCommandCallback = &MyPluginLoad;
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



// Blahblahblah not interested

void COpenPlugin::Pause( void ) { }
void COpenPlugin::UnPause( void ) { }
void COpenPlugin::LevelInit( char const *pMapName ) { }
void COpenPlugin::ServerActivate( void *pEdictList, int edictCount, int clientMax ) { }
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
