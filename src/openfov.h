#pragma once

#include "sdk.h"
#include "util.h"



class COpenFOV
{
public:
	bool Init();
	void Close();
	
	// This proxy is called to store the value for your FOV in your client entity
	static void MyFOVProxy( const CRecvProxyData* pData, void* pStruct, void* pOut );

	// Testing onchange callback...
	static void OnChangeFOV( IConVar* var, const char* pOldValue, float flOldValue );

private:
	
	
	// Force this fov at all times
	ConVar* fov_desired;
	NetPropHook hook;

public:
	static COpenFOV g;
};