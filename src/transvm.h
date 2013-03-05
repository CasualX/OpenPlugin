#pragma once

//----------------------------------------------------------------
// Transparent Viewmodel stuff
//----------------------------------------------------------------

#include "sdk.h"
#include "util.h"

class CTransVM
{
public:

	bool Init();
	void Close();

	void SetBlend( IClientEntity* pEnt, unsigned char blend );
	void Update();

	
	void HookedProp( const CRecvProxyData *pData, void *pStruct, void *pOut );

private:

	struct offsets_t
	{
		int nRenderMode;
		int clrRender;
		int hActiveWeapon;
		int hViewModel;
	} off;

	ConVar* r_drawviewmodel;

	NetPropHook hook1;
	NetPropHook hook2;

public:
	static CTransVM g;
};