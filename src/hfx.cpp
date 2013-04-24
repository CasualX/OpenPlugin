#include "stdafx.h"

#include "hfx.h"
#include "ifaces.h"
#include "plugin.h"
#include "util.h"

DWORD WINAPI DelayLoad(void *)
{
	while ( GetModuleHandleA( "serverbrowser.dll" ) == NULL )
		Sleep(500);

	void *hmEngine = ::GetModuleHandleA( "engine.dll" );
	CreateInterfaceFn pfnEngine = (CreateInterfaceFn) GetFuncAddress( hmEngine, "CreateInterface");
	CreateInterfaceFn pfnAppSystem = NULL;

	if ( void* p = SigScan( hmEngine,
		"\x8B\x11\x8B\x12\x83\xC4\x04\x50\xA1\x00\x00\x00\x00\x50\xFF\xD2",
		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF" ) )
	{
		pfnAppSystem = **(CreateInterfaceFn**)( (char*)p + 9 );
	}

	if(!pfnAppSystem)
		return 0;

	// Start by initing the ifaces
	if ( !Ifaces.Init(pfnAppSystem) )
		return 0;

	PluginInit();

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		::CreateThread( NULL, 0, &DelayLoad, NULL, 0, NULL );
		break;
	}
	return TRUE;
}