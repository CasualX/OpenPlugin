#ifndef HGUARD_OPENPLUGIN_IFACES
#define HGUARD_OPENPLUGIN_IFACES

//
// Manages the static resources exposed by the game & engine
//

#include "sdk.h"

#ifdef _LINUX
#define GetFuncAddress(pAddress, szFunction) dlsym(pAddress, szFunction)
#else
#define GetFuncAddress(pAddress, szFunction) ::GetProcAddress((HMODULE)pAddress, szFunction)
#endif

class IVEngineClient;
class IGameEventManager2;
class IEngineVGui;
class IVRenderView;
class IVModelInfoClient;
class IVModelRender;
class IVDebugOverlay;
class IMaterialSystem;
class CGlobalVarsBase;

class IBaseClientDLL;
class IClientEntityList;
class IClientEntity;

class TFPlayerResource;
class IGameResources;
typedef IGameResources* (*GameResourcesFn)();
typedef IClientEntity* (*GetLocalPlayerFn)();

class ICvar;
class ISurface;

class Interfaces
{
public:

	bool Init();


	// Module handles
	void* hmEngine;
	void* hmClient;
	void* hmTier0;

	// CreateInterfaceFns
	CreateInterfaceFn pfnAppSystem;
	CreateInterfaceFn pfnClient;
	CreateInterfaceFn pfnEngine;

	// engine.dll
	IVEngineClient* pEngine;
	IGameEventManager2* pEvents;
	IEngineVGui* pVGUI;
	IVRenderView* pRenderView;
	IVModelInfoClient* pModelInfo;
	IVModelRender* pModelRender;
	IVDebugOverlay* pOverlay;
	IMaterialSystem* pMaterial;
	CGlobalVarsBase* pGlobals;

	// client.dll
	IBaseClientDLL* pClient;
	IClientEntityList* pEntityList;
	TFPlayerResource** ppGameRes;
	GameResourcesFn pfnGameResources;
	GetLocalPlayerFn pfnGetLocalPlayer;

	// others
	ICvar* pCvar;
	ISurface* pSurface;
};
extern Interfaces Ifaces;

#endif // !HGUARD_OPENPLUGIN_IFACES
