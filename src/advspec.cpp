#include "stdafx.h"

#include "advspec.h"
#include "ifaces.h"
#include "util.h"
#include "game.h"

#include "sdk/icvar.h"
#include "sdk/cdll_int.h"
#include "sdk/globalvars.h"
#include "sdk/icliententitylist.h"
//
//void CCmdHook::Hook( ConCommand* cmd, CommandCallbackFn hook )
//{
//	this->cmd = cmd;
//	this->pfn = cmd->fnCommandCallback;
//	cmd->fnCommandCallback = hook;
//}
//void CCmdHook::Unhook()
//{
//	if ( cmd )
//		cmd->fnCommandCallback = pfn;
//}
//
//
//int FindSpecTarget( int team, player_info_t* pinfo )
//{
//	if ( !*Ifaces.ppGameRes )
//		return 0;
//
//	int cur_class = 0;
//	int cur_player = 0;
//
//	for ( int i = 1; i<=Ifaces.pGlobals->maxClients; i++ )
//	{
//		// Find a live player in the same team as requested
//		if ( Ifaces.pEngine->GetPlayerInfo( i, pinfo ) && (*Ifaces.ppGameRes)->bAlive( i ) && (*Ifaces.ppGameRes)->iTeam( i )==team )
//		{
//			// Find most important alive class
//			int plc = (*Ifaces.ppGameRes)->iPlayerClass( i );
//			// If we don't have a target yet, allow any change
//			// Otherwise only allow changes to a more important class
//			if ( cur_class==0 || ( plc<=TFCLASS_MEDIC && plc>cur_class ) )
//			{
//				cur_class = plc;
//				cur_player = i;
//			}
//		}
//	}
//
//	Ifaces.pEngine->GetPlayerInfo( cur_player, pinfo );
//	return cur_player;
//}
//
////CSpectator CSpectator::g;
//
//bool CSpectator::Init()
//{
//	spec_targetinfo = new ConCommand( "spec_targetinfo", "Print info about the player you're spectating", 0, &MySpecTargetInfo );
//	Ifaces.pCvar->RegisterConCommand( spec_targetinfo );
//
//	if ( ConCommand* cmd = Ifaces.pCvar->FindCommand( "spec_player" ) )
//	{
//		spec_player.Hook( cmd, MySpecPlayer );
//		Ifaces.pEvents->AddListener( this, "localplayer_respawn", false );
//		Ifaces.pEvents->AddListener( this, "player_spawn", false );
//		Ifaces.pEvents->AddListener( this, "player_death", false );
//		return true;
//	}
//	return false;
//}
//void CSpectator::Close()
//{
//	spec_player.Unhook();
//	Ifaces.pEvents->RemoveListener( this );
//
//	Ifaces.pCvar->UnregisterConCommand( spec_targetinfo );
//	delete spec_targetinfo;
//}
//
//void CSpectator::Track( player_info_t* pinfo )
//{
//	// We're locked onto ya
//	lock = pinfo->userID;
//
//	// Build this thing...
//	CCmdArgs args;
//	args.nArgc = 2;
//	strcpy( args.pArgSBuffer, pinfo->name );
//	strcpy( args.pArgvBuffer, "spec_player" );
//	args.ppArgv[0] = args.pArgvBuffer;
//	args.ppArgv[1] = args.pArgSBuffer;
//
//	// Exec it, will fail with players with quotes in their names
//	spec_player.pfn( args );
//}
//void CSpectator::FireGameEvent( IGameEvent* evt )
//{
//	const char* name = evt->GetName();
//
//	// Stop tracking upon respawning
//	if ( !strcmp( name, "localplayer_respawn" ) )
//	{
//		lock = 0;
//		return;
//	}
//	
//	// A player spawned, check if we are tracking him
//	if ( !strcmp( name, "player_spawn" ) )
//	{
//		int userid = evt->GetInt( "userid" );
//		int pl = Ifaces.pEngine->GetPlayerForUserID( userid );
//
//		// Are we tracking and is the player who just spawned the guy whom we are tracking?
//		if ( lock && lock==userid )
//		{
//			player_info_t pinfo;
//			if ( Ifaces.pEngine->GetPlayerInfo( pl, &pinfo ) )
//			{
//				Track( &pinfo );
//			}
//		}
//		return;
//	}
//
//	//if ( Ifaces.pEngine->IsHLTV() )
//	//{
//	//	// Spec someone else upon initial target dying
//	//	if ( !strcmp( name, "player_death" ) )
//	//	{
//	//		int userid = evt->GetInt( "userid" );
//	//		int pl = Ifaces.pEngine->GetPlayerForUserID( userid );
//	//		// If the person we were spectating died
//	//		IClientEntity* pMe = Ifaces.pEntityList->GetClientEntity( Ifaces.pEngine->GetLocalPlayer() );
//	//		if ( pMe && pl==GetObserverTarget( pMe ) )
//	//		{
//	//			// Get someone new to spectate
//	//			player_info_t pinfo;
//	//			if ( FindSpecTarget( (*Ifaces.ppGameRes)->iTeam( pl ), &pinfo )>0 )
//	//				Track( &pinfo );
//	//		}
//	//		return;
//	//	}
//	//}
//}
//void __cdecl CSpectator::MySpecPlayer( const CCmdArgs& args )
//{
//	if ( args.nArgc>1 )
//	{
//		// Makes the quotes completely optional
//		const char* name = args.ppArgv[1];
//		if ( args.nArgc>2 )
//			name = args.pArgSBuffer;
//
//		// Find a player by this name
//		player_info_t pinfo;
//		if ( FindPlayerInServer( name, &pinfo )>0 )
//		{
//			g.Track( &pinfo );
//		}
//		else
//		{
//			Msg( "No player with that name found.\n" );
//		}
//	}
//	else
//	{
//		g.lock = 0;
//		Msg( "Enter a partial name from someone on the server to spectate.\n" );
//	}
//}
//void __cdecl CSpectator::MySpecTargetInfo( const CCmdArgs& args )
//{
//	if ( Ifaces.pEngine->IsInGame() )
//	{
//		int me = Ifaces.pEngine->GetLocalPlayer();
//		IClientEntity* pMe = Ifaces.pEntityList->GetClientEntity( me );
//
//		if ( pMe && !(*Ifaces.ppGameRes)->bAlive( me ) )
//		{
//			int target = GetObserverTarget( pMe );
//			player_info_t pinfo;
//			if ( Ifaces.pEngine->GetPlayerInfo( target, &pinfo ) )
//			{
//				__int64 scid = 76561197960265728;
//				scid += pinfo.friendsID;
//				Msg( "Spectating \"%s\" userID:%d %s https://steamcommunity.com/profiles/%I64d \n", pinfo.name, pinfo.userID, pinfo.guid, scid );
//			}
//		}
//	}
//}
