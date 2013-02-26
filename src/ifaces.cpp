
#include "stdafx.h"
#include "ifaces.h"
#include "util.h"
#include "sdk/cdll_int.h"


Interfaces Ifaces;

bool Interfaces::Init()
{
	// Grab module handles
#ifndef _LINUX
	hmEngine = ::GetModuleHandleA( "engine.dll" );
	hmTier0 = ::GetModuleHandleA( "tier0.dll" );
#else
	hmEngine = dlopen("engine.so", RTLD_LAZY);
	hmTier0 = dlopen("libtier0.so", RTLD_LAZY);
#endif

	if ( !hmEngine || !hmTier0 )
		return false;

	// Grab engine CreateInterfaceFn
	// Note that we can access all interfaces (except client.dll/server.dll ones) through the pfnAppSystem function.

	pfnEngine = (CreateInterfaceFn) GetFuncAddress( hmEngine, "CreateInterface" );	

#ifndef _LINUX	
	if ( void* p = SigScan( hmEngine,
			"\x8B\x11\x8B\x12\x83\xC4\x04\x50\xA1\x00\x00\x00\x00\x50\xFF\xD2",
			"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF" ) )
	{
		pfnAppSystem = **(CreateInterfaceFn**)( (char*)p + 0x9 );
	}	
#else
	if ( void* p = SigScan( (void*)pfnEngine,
			"\x8B\x97\x84\x00\x00\x00\x8B\x0A\x89\x44\x24\x08\xA1\x00\x00\x00",
			"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00" ) )
	{
		pfnAppSystem = **(CreateInterfaceFn**)( (char*)p + 0xD );
	}
#endif
	if ( !pfnAppSystem || !pfnEngine ) 
		return false;
	
	// Grab the interfaces

	pEngine			= (IVEngineClient*)		pfnAppSystem( "VEngineClient013",		NULL );
	pEvents			= (IGameEventManager2*)		pfnAppSystem( "GAMEEVENTSMANAGER002",		NULL );
	pVGUI			= (IEngineVGui*)		pfnAppSystem( "VEngineVGui001",			NULL );
	pModelInfo		= (IVModelInfoClient*)		pfnAppSystem( "VModelInfoClient006",		NULL );
	pModelRender		= (IVModelRender*)		pfnAppSystem( "VEngineModel016",		NULL );
	pOverlay		= (IVDebugOverlay*)		pfnAppSystem( "VDebugOverlay003",		NULL );
	pRenderView		= (IVRenderView*)		pfnAppSystem( "VEngineRenderView013",		NULL );
	pMaterial		= (IMaterialSystem*)		pfnAppSystem( "VMaterialSystem080",		NULL );
	
	
	// Grab client module handle
#ifndef _LINUX
	hmClient = ::GetModuleHandleA( "client.dll" );
#else
	const char* szGameDir = pEngine->GetGameDirectory();
	char szClientBinaryPath[FILENAME_MAX];
	snprintf(szClientBinaryPath, sizeof(szClientBinaryPath), "%s/bin/client.so", szGameDir);
	
	hmClient = dlopen(szClientBinaryPath, RTLD_LAZY);
#endif
	
	if(!hmClient)
		return false;
	
	// Grab client CreateInterfaceFn
	pfnClient = (CreateInterfaceFn) GetFuncAddress( hmClient, "CreateInterface" );
	
	if(!pfnClient)
		return false;
	
	pClient			= (IBaseClientDLL*)		pfnClient( "VClient017",			NULL );
	pEntityList		= (IClientEntityList*)		pfnClient( "VClientEntityList003",		NULL );

	pCvar			= (ICvar*)			pfnAppSystem( "VEngineCvar004",			NULL );
	pSurface		= (ISurface*)			pfnAppSystem( "VGUI_Surface030",		NULL );

#ifndef _LINUX
	pGlobals		= **(CGlobalVarsBase***)( (*(unsigned int**)pClient)[0] + 0x60 ); // pClient->HudUpdate
#else
	pGlobals		= **(CGlobalVarsBase***)( (*(unsigned int**)pClient)[0] + 0x85 ); // pClient->HudUpdate
#endif


#ifndef _LINUX	  
	if ( char* p = (char*)SigScan( hmClient,	
		"\xA1\x00\x00\x00\x00\x85\xC0\x74\x06\x05",
		"\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF" ) )
#else
	if ( char* p = (char*)SigScan( (void*)pfnClient,	
		"\xA1\x00\x00\x00\x00\x55\x89\xE5\x85\xC0\x74\x0C",
		"\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF" ) )
	
#endif
	{
		pfnGameResources = (GameResourcesFn)p;
		ppGameRes = *(TFPlayerResource***)( p+1 );
	}


#ifndef _LINUX
	if ( char* p = (char*)SigScan( hmClient,
		"\x55" "\x8B\xEC" "\xE8\x00\x00\x00\x00" "\x85\xC0" "\x75\x06" "\x32\xC0",
		"\xFF" "\xFF\xFF" "\xFF\x00\x00\x00\x00" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF" ) )
	{
		// Signature ends up in IGameResources::IsLocalPlayer()
		pfnGetLocalPlayer = reinterpret_cast<GetLocalPlayerFn>( *(unsigned int*)( p+4 ) + (unsigned int)p + 0x8 );
	}	  
#else
	if ( char* p = (char*)SigScan( (void*)pfnClient,
		"\x55" "\x89\xE5" "\x83\xEC\x18" "\xE8\x00\x00\x00\x00" "\x89\xC2" "\x31\xC0" "\x85\xD2" "\x74\x11",
		"\xFF" "\xFF\xFF" "\xFF\xFF\xFF" "\xFF\x00\x00\x00\x00" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF") )
	{
		// Signature ends up in IGameResources::IsLocalPlayer()
		pfnGetLocalPlayer = reinterpret_cast<GetLocalPlayerFn>( *(unsigned int*)( p+7 ) + (unsigned int)p + 0xB );
	}		
#endif


	// FIXME! Test all the above for NULL ptrs...

	return true;
}


template< typename T >
T* Handle<T>::get()
{
	return Ifaces.pEntityList->GetClientEntity( m_index&0xFFF );
}
