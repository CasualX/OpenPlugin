#ifndef HGUARD_OPENPLUGIN_DLFILTER
#define HGUARD_OPENPLUGIN_DLFILTER

#include "sdk.h"
#include "vmthooks.h"

#ifdef _LINUX
#define __fastcall
#define __thiscall
#define EDX
#else
#define EDX int edx,
#endif

class CDownloadFilter
{
public:

	bool Init();
	void Close();

	static bool __fastcall HookedFileExists( void* _this, EDX const char* file, const char* pid );

private:
	void* pInst;
	void* pRetAddr;

	ConVar* cl_downloadfilter;

	typedef VMTBaseManager hook_t;
	hook_t hook;

public:
	static CDownloadFilter g;
};

#endif // !HGUARD_OPENPLUGIN_DLFILTER
