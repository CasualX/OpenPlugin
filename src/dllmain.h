#ifndef DLLMAIN_H
#define	DLLMAIN_H


#ifdef _LINUX
void __attribute__ ((constructor)) LoadFunc();
void __attribute__ ((destructor)) UnloadFunc();
#else
void LoadFunc();
void UnloadFunc();
#endif


#endif	/* DLLMAIN_H */

