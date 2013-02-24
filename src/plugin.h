#ifndef HGUARD_OPENPLUGIN
#define HGUARD_OPENPLUGIN

#include "sdk.h"
#include "sdk/convar.h"

// Allows the game (you just lost) to find us

#ifndef _LINUX
extern "C" __declspec(dllexport)
	void* CreateInterface( const char* name, int* code );
#else
extern "C" __attribute__ ((visibility("default"))) void* CreateInterface( const char* name, int* code );
#endif

// Plugin interface

class COpenPlugin
{
public:

	static COpenPlugin g;
#ifndef _LINUX	
	void DoMagic();
	void UndoMagic();
	static void MyPluginLoad( const CCmdArgs& args );
#endif
	// IServerPluginCallbacks interface
	virtual bool			Load( CreateInterfaceFn pfnFactory, CreateInterfaceFn pfnServer );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char*		GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( void *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( void *pEntity );
	virtual void			ClientDisconnect( void *pEntity );
	virtual void			ClientPutInServer( void *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( void *pEdict );
	virtual int			ClientConnect( bool *bAllowConnect, void *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual int			ClientCommand( void *pEntity, const void *args );
	virtual int			NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( int iCookie, void *pPlayerEntity, int eStatus, const char *pCvarName, const char *pCvarValue );
	virtual void			OnEdictAllocated( void *edict );
	virtual void			OnEdictFreed( const void *edict );

private:
#ifndef _LINUX
	ConCommand* plugin_load;
	CommandCallbackFn fnOrigPload;

	bool* bMagic;
#endif
};

#endif // !HGUARD_OPENPLUGIN
