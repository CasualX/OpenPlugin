#ifndef HGUARD_OPENPLUGIN_NAME
#define HGUARD_OPENPLUGIN_NAME

#include "sdk.h"

class CNameEnabler
{
public:
	bool Init();
	void Close();

	static void OnChangeName( IConVar* var, const char* pOldValue, float flOldValue );

	bool IsAllowedToChangeName( const char* oldname );

private:
	ConVar* name;
	unsigned int ret;


public:
	static CNameEnabler g;
};

#endif // !HGUARD_OPENPLUGIN_NAME
