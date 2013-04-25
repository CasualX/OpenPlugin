
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

	// Hook the CBasePlayer::m_iFOV netvar proxy
	const char* varsFOV[] = { "m_iFOV", NULL };
	RecvProp* propFOV = NetworkedVar( "CBasePlayer", NULL, varsFOV );
	if ( !propFOV )
		return false;
	hook.Hook( propFOV, &MyFOVProxy );

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

void COpenFOV::MyFOVProxy( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	// Just overwrite with our fov
	IClientEntity* pMe = (IClientEntity*)pStruct;
	if(!(GetPlayerCond(pMe) & (1 << 1))) // Make sure we aren't zooming
		*(int*)pOut = g.fov_desired->nValue;
	else
		*(int*)pOut = pData->Value.Int;
}


