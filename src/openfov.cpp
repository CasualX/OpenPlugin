
#include "stdafx.h"

#include "openfov.h"
#include "util.h"
#include "ifaces.h"
#include "game.h"



//
// Change your FOV outside the allowed range.
//
// How does it work?
// Simply put, your fov_desired is broadcasted to the server.
// The server will force it within [75,90] range and store it in your player entity in the variable m_iDefaultFOV.
// Your client then uses m_iDefaultFOV to compute your fov (stored in m_iFOV).
//
// These variables are 'networked', meaning the server can issue updates to them.
// The descriptors for these networked vars are stored in RecvProp and RecvTable structs.
// When the server issues an update, the 'proxy' for the value is called to store the value.
// So this code 'hooks' the proxy callback and overwrites it with the real fov you want, not the capped fov sent by the server.
//


COpenFOV COpenFOV::g;

bool COpenFOV::Init()
{
	// Edit fov_desired
	// Note that this isn't good enough to allow a higher fov since the limit is enforced by the server
	fov_desired = Ifaces.pCvar->FindVar( "fov_desired" );
	if ( !fov_desired )
		return false;
	fov_desired->fMaxVal = 130.0f;
	//fov_desired->fnChangeCallback = &OnChangeFOV;

	// Hook the CBasePlayer::m_iDefaultFOV netvar proxy
	const char* vars[] = { "m_iDefaultFOV", NULL };
	RecvProp* prop = NetworkedVar( "CBasePlayer", NULL, vars );
	if ( !prop )
		return false;
	hook.Hook( prop, &MyDefaultFOVProxy );

	return true;
}
void COpenFOV::Close()
{
	if ( fov_desired )
	{
		hook.Unhook();
		//fov_desired->fnChangeCallback = NULL;
	}
}

void COpenFOV::MyDefaultFOVProxy( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	// Just overwrite with our fov
	*(int*)pOut = g.fov_desired->nValue;
}



