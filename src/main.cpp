#include "stdafx.h"

#include "main.h"
#include "ifaces.h"
#include "plugin.h"
#include "util.h"

#ifndef _LINUX
DWORD WINAPI DelayLoad(void *)
{
	DelayLoadPlugin();
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch ( dwReason )
	{
	case DLL_PROCESS_ATTACH:
		::CreateThread( NULL, 0, &DelayLoad, NULL, 0, NULL );
		break;
	}
	return TRUE;
}
#else
void LoadFunc(void)
{
	std::thread delay_thread(DelayLoadPlugin);
	delay_thread.detach();
}
#endif