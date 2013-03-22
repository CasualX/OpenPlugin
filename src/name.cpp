
#include "stdafx.h"

#include "name.h"
#include "util.h"
#include "ifaces.h"

#include "sdk/convar.h"
#include "sdk/icvar.h"

//
// The game has 3 methods of preventing you from using the 'name' cvar.
//
// 1. The onchange callback will fetch your steam name and force the name back.
//  Solution: Overwrite the callback.
//
// 2. As well as explicitly change the name var back to your steam name.
//  This is more difficult, you can do stack shenanigans in the onchange callback to only allow changes from valid sources.
//
// 3. The setinfo command has a hardcoded check against 'name' so you can't use it.
//


CNameEnabler CNameEnabler::g;

bool CNameEnabler::Init()
{
	// Console calls through here to change cvars, sigscan for a return address for later stack shenanigans.
#ifndef _LINUX
	if ( void* p = SigScan( Ifaces.hmEngine,
		"\x8B\x47\x18" "\x8B\x50\x08" "\x8D\x4F\x18" "\xFF\xD2" "\x5E\x5B\x5F",
		"\xFF\xFF\xFF" "\xFF\xFF\xFF" "\xFF\xFF\xFF" "\xFF\xFF" "\xFF\xFF\xFF" ) )
	{
		ret = (unsigned int)p + 11; // Not x64 compatible etc..
#else
	if ( void* p = SigScan( (void*)Ifaces.pfnEngine,
		"\x84\xC0" "\x8B\x07" "\x75\x26" "\x89\x74\x24\x04" "\x89\x3C\x24" "\xFF\x50\x00",
		"\xFF\xFF" "\xFF\xFF" "\xFF\xFF" "\xFF\xFF\xFF\xFF" "\xFF\xFF\xFF" "\xFF\xFF\x00" ) )
	{
		ret = (unsigned int)p + 16; // Not x64 compatible etc..
#endif
		if ( (name = Ifaces.pCvar->FindVar( "name" )) )
		{
			name->fnChangeCallback = &OnChangeName;
			return true;
		}
	}
	return false;
}
void CNameEnabler::Close()
{
	if ( name )
		name->fnChangeCallback = NULL;
}

bool CNameEnabler::IsAllowedToChangeName( const char* oldname )
{
	// Otherwise the first name change to your steam name will fail.
	if ( !strcmp( oldname, "unnamed" ) )
		return true;

	// A static offset appears to not work for some people...
	// So screw this & just scan half the stack.
#ifndef _LINUX
	unsigned int* stack = (unsigned int*) _AddressOfReturnAddress();
	
	for ( unsigned int* it = stack, *end = it+100; it!=end; ++it )
	{
		if ( *it==ret )
			return true;
	}	
#else
	if((unsigned int)__builtin_return_address(2) == ret)
		return true;
#endif

	return false;
}
void CNameEnabler::OnChangeName( IConVar* var, const char* pOldValue, float flOldValue )
{
	ASSUME( var!=NULL );
	ConVar* cvar = static_cast<ConVar*>( var );

	if ( !g.IsAllowedToChangeName( pOldValue ) )
	{
		assert( static_cast<unsigned int>(cvar->StringLength)>strlen( pOldValue ) );
	
		Msg( "OnChangeName() Blocked change to \"%s\"!\n", cvar->pszString );
		strcpy( cvar->pszString, pOldValue );
	}
	// Valve is now stripping color codes so this is useless..
	//else
	//{
	//	// Format shenanigans
	//	// Replaces \1-\5 with 0x1 to 0x5 color codes
	//	char* t = cvar->pszString;
	//	char* s = t;
	//	unsigned char c;
	//	do
	//	{
	//		c = *s++;
	//		if ( c=='\\' )
	//		{
	//			c = *s++;
	//			if ( !c )
	//				break;
	//			c -= '0';
	//			if ( c>9 )
	//				c += '0';
	//		}
	//		*t++ = c;
	//	}
	//	while ( c );
	//}
	
}
/*
OK so how does one figure this out?

I started by adding a dummy callback.
Attached visual studio's debugger to TF2, loaded the (uncompleted) plugin and breakpointed my OnChangeName().

Upon changing name through the console the call stack looks like this:

OpenPlugin.dll!CNameEnabler::OnChangeName(...) Line 52
engine.dll!56e99b06() <- ConVar::InternalSetString()
engine.dll!56e99a64() <- IConVar::SetString()
engine.dll!56e93bcf() <- probably some func running console commands, idk & idc
...

Using steam to change name results in this call stack:

OpenPlugin.dll!CNameEnabler::OnChangeName(...) Line 52
engine.dll!56e99b06() <- ConVar::InternalSetString()
engine.dll!56e99a64() <- IConVar::SetString()
steamclient.dll!380a3985() <- OH WHAT's THIS? :)
...

Now that we have this information, a guess on my side is to allow only name changes from engine.dll!56e93bcf().
Visual studio's debugger will only get us so far, so I add some temp code to quickly scan the stack for 0x56e93bcf.
Note that I don't close TF2 in the meantime because dlls might get loaded at a different base address screwing our static addresses.

Abusing _AddressOfReturnAddress() to get a consistent pointer to the stack. (in hindsight I might as well just take the address of any stack arg...)
Simply iterate over it to find the desired result, in this case 29.

In the disassembler window located engine.dll!56e93bcf() and create a signature.




	// Test code to grab the offset...
	unsigned int i;
	for ( i = 0; i<500; i++ )
	{
		if ( addr[i]==0x57C83B1F )
			break;
	}
	unsigned int* ptr = addr + i;
*/



