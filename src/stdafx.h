// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _LINUX
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#include <sys/stat.h>
#endif

#ifndef _LINUX
// Only available on windows...
#include <intrin.h>
#else
#define __debugbreak() 0
#endif

#include <assert.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <thread>

// TODO: reference additional headers your program requires here
#include "platform.h"
