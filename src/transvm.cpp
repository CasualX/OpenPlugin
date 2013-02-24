#include "stdafx.h"

#include "transvm.h"
#include "game.h"
#include "util.h"
#include "ifaces.h"

CTransVM CTransVM::g;

void _HookedProp( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	CTransVM::g.HookedProp( pData, pStruct, pOut );
}
bool CTransVM::Init()
{
	NETVAR( "CBaseEntity", &off.nRenderMode, "m_nRenderMode" );
	NETVAR( "CBaseEntity", &off.clrRender, "m_clrRender" );
	NETVAR( "CBaseCombatCharacter", &off.hActiveWeapon, "m_hActiveWeapon" );
	NETVAR( "CBasePlayer", &off.hViewModel, "m_hViewModel[0]" );

	static const char* const vars[] = { "m_hActiveWeapon", NULL };
	hook1.Hook( NetworkedVar( "CBaseCombatCharacter", NULL, vars ), &_HookedProp );

	r_drawviewmodel = Ifaces.pCvar->FindVar( "r_drawviewmodel" );
	return true;
}
void CTransVM::Close()
{
	hook1.Unhook();
}
void CTransVM::SetBlend( IClientEntity* pEnt, unsigned char blend )
{
	GetProp<unsigned char>( pEnt, off.nRenderMode ) = 2;
	GetProp<unsigned int>( pEnt, off.clrRender ) = 0x00FFFFFF|(blend<<24);
}
void CTransVM::Update()
{
	if ( IClientEntity* pMe = Ifaces.pfnGetLocalPlayer() )
	{
		unsigned char blend = static_cast<unsigned char>( r_drawviewmodel->GetInt() );
		if ( blend==1 ) blend = 255;

		unsigned int index;

		// Overwrite alpha blend for the weapon entity
		index = reinterpret_cast<EHANDLE*>( (char*)pMe + off.hActiveWeapon )->entryindex();
		if ( IClientEntity* pWeapon = Ifaces.pEntityList->GetClientEntity( index ) )
		{
			SetBlend( pWeapon, blend );
		}

		// Overwrite alpha blend for the viewmodel
		index = reinterpret_cast<EHANDLE*>( (char*)pMe + off.hViewModel )->entryindex();
		if ( IClientEntity* pViewModel = Ifaces.pEntityList->GetClientEntity( index ) )
		{
			SetBlend( pViewModel, blend );
		}
	}
}
void CTransVM::HookedProp( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	hook1.oldproxy( pData, pStruct, pOut );
	Update();
}
