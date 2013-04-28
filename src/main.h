#pragma once

#include "sdk.h"

#ifdef _LINUX
void __attribute__ ((constructor)) LoadFunc(void);
#endif