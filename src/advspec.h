#pragma once

//
// Spectator enhancements
// (god this code is rather terrible)
//
// Features:
// * spec_player accepts partial names and will lock on that person.
// * If the person locked onto respawns, you'll switch to spectate him again.
// * Command spec_targetinfo that displays info about the player you're spectating.
//

#include "sdk.h"
#include "sdk/convar.h"
#include "sdk/igameevents.h"

//
//// TODO: Move me somewhere else...
//struct CCmdHook
//{
//	void Hook( ConCommand* cmd, CommandCallbackFn hook );
//	void Unhook();
//	ConCommand* cmd;
//	CommandCallbackFn pfn;
//};
//
//int FindSpecTarget( int team, player_info_t* pinfo );
//
//class CSpectator : public IGameEventListener2
//{
//public:
//	bool Init();
//	void Close();
//
//	void Track( player_info_t* pinfo );
//
//	virtual void FireGameEvent( IGameEvent* evt );
//
//	static void __cdecl MySpecPlayer( const CCmdArgs& );
//	static void __cdecl MySpecTargetInfo( const CCmdArgs& );
//private:
//	CCmdHook spec_player;
//	int lock; // Userid of player we're locked onto
//	ConVar* spec_action;
//	ConCommand* spec_targetinfo;
//public:
//	static CSpectator g;
//};