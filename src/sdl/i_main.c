// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief Main program, simply calls D_SRB2Main and D_SRB2Loop, the high level loop.

#include "../doomdef.h"
#include "../m_argv.h"
#include "../d_main.h"
#include "../m_misc.h" /* path shit */
#include "../i_system.h"

#if defined (__GNUC__) || defined (__unix__)
#include <unistd.h>
#endif

#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
#include <errno.h>
#endif

#include <time.h> // For log timestamps

#ifdef HAVE_SDL

#ifdef HAVE_TTF
#include "SDL.h"
#include "i_ttf.h"
#endif

#if defined (_WIN32) && !defined (main)
#define SDLMAIN
#endif

#ifdef SDLMAIN
#include "SDL_main.h"
#elif defined(FORCESDLMAIN)
extern int SDL_main(int argc, char *argv[]);
#endif

#ifdef LOGMESSAGES
FILE *logstream = NULL;
char logfilename[1024];
#endif

#ifndef DOXYGEN
#ifndef O_TEXT
#define O_TEXT 0
#endif

#ifndef O_SEQUENTIAL
#define O_SEQUENTIAL 0
#endif
#endif

#if defined (_WIN32)
#include "../win32/win_dbg.h"
typedef BOOL (WINAPI *p_IsDebuggerPresent)(VOID);
static HMODULE g_hmod_drmingw;
#endif

#ifdef LIBBACKTRACE
#include <backtrace.h>
static struct backtrace_state *backtrace;
#endif

#ifdef LOGMESSAGES
static void InitLogging(void)
{
	const char *logdir = NULL;
	time_t my_time;
	struct tm * timeinfo;
	const char *format;
	const char *reldir;
	int left;
	boolean fileabs;
#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
	const char *link;
#endif

	logdir = D_Home();

	my_time = time(NULL);
	timeinfo = localtime(&my_time);

	if (M_CheckParm("-logfile") && M_IsNextParm())
	{
		format = M_GetNextParm();
		fileabs = M_IsPathAbsolute(format);
	}
	else
	{
		format = "log-%Y-%m-%d_%H-%M-%S.txt";
		fileabs = false;
	}

	if (fileabs)
	{
		strftime(logfilename, sizeof logfilename, format, timeinfo);
	}
	else
	{
		if (M_CheckParm("-logdir") && M_IsNextParm())
			reldir = M_GetNextParm();
		else
			reldir = "logs";

		if (M_IsPathAbsolute(reldir))
		{
			left = snprintf(logfilename, sizeof logfilename,
					"%s"PATHSEP, reldir);
		}
		else
#ifdef DEFAULTDIR
		if (logdir)
		{
			left = snprintf(logfilename, sizeof logfilename,
					"%s"PATHSEP DEFAULTDIR PATHSEP"%s"PATHSEP, logdir, reldir);
		}
		else
#endif/*DEFAULTDIR*/
		{
			left = snprintf(logfilename, sizeof logfilename,
					"."PATHSEP"%s"PATHSEP, reldir);
		}

		strftime(&logfilename[left], sizeof logfilename - left,
				format, timeinfo);
	}

	M_MkdirEachUntil(logfilename,
			M_PathParts(logdir) - 1,
			M_PathParts(logfilename) - 1, 0755);

#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
	logstream = fopen(logfilename, "w");
#ifdef DEFAULTDIR
	if (logdir)
		link = va("%s/"DEFAULTDIR"/latest-log.txt", logdir);
	else
#endif/*DEFAULTDIR*/
		link = "latest-log.txt";
	unlink(link);
	if (symlink(logfilename, link) == -1)
	{
		I_OutputMsg("Error symlinking latest-log.txt: %s\n", strerror(errno));
	}
#else/*defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)*/
	logstream = fopen("latest-log.txt", "wt+");
#endif/*defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)*/
}
#endif

#ifdef LIBBACKTRACE
/* Callback for each stack frame */
static int libbacktrace_full_callback(void *data, uintptr_t pc, const char *filename, int lineno, const char *function) {
    (void)data; // unused

    if (filename)
        printf("  %s:%d: %s\n", filename, lineno, function ? function : "??");
    else
        printf("  [unknown] (PC=0x%lx)\n", (unsigned long)pc);

    return 0; // continue stack walk
}

/* Callback if something goes wrong */
static void libbacktrace_error_callback(void *data, const char *msg, int errnum) {
    (void)data; (void)errnum;
    fprintf(stderr, "Error: %s\n", msg);
}

/* Function to print the backtrace */
static void libbacktrace_print_stacktrace(void) {
    backtrace_full(backtrace, 0, libbacktrace_full_callback, libbacktrace_error_callback, NULL);
}
#endif


/**	\brief	The main function

	\param	argc	number of arg
	\param	*argv	string table

	\return	int
*/
#if defined (__GNUC__) && (__GNUC__ >= 4)
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#endif

#ifdef FORCESDLMAIN
int SDL_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	myargc = argc;
	myargv = argv; /// \todo pull out path to exe from this string

#ifdef HAVE_TTF
#ifdef _WIN32
	I_StartupTTF(FONTPOINTSIZE, SDL_INIT_VIDEO|SDL_INIT_AUDIO, SDL_SWSURFACE);
#else
	I_StartupTTF(FONTPOINTSIZE, SDL_INIT_VIDEO, SDL_SWSURFACE);
#endif
#endif

#ifdef LOGMESSAGES
	if (!M_CheckParm("-nolog"))
		InitLogging();
#endif/*LOGMESSAGES*/

	//I_OutputMsg("I_StartupSystem() ...\n");
	I_StartupSystem();

#if defined (_WIN32)

#if 0
#if 0
	g_hmod_drmingw = LoadLibrary("exchndl.dll");
	LoadLibrary("exchndl.dll");
#else
	g_hmod_drmingw = GetModuleHandleA("exchndl.dll");
	GetModuleHandleA("exchndl.dll");
#endif
#else
	g_hmod_drmingw = LoadLibraryA("exchndl.dll");
	LoadLibraryA("exchndl.dll");
#endif

	typedef void (APIENTRY *EXCHNDLINIT)(void);
	EXCHNDLINIT pfnExcHndlInit = NULL;
#if 1
	pfnExcHndlInit = (EXCHNDLINIT)GetProcAddress(g_hmod_drmingw, "ExcHndlInit");
	if (g_hmod_drmingw && pfnExcHndlInit)
	{
		CONS_Printf("Setting up debugger.\n");
		pfnExcHndlInit();
	}
#else
	(void)pfnExcHndlInit;
	if (g_hmod_drmingw)
		CONS_Printf("Debugger initialized.\n");
#endif

#endif // defined (_WIN32)

#ifdef BUGTRAP
	InitBugTrap();
#if defined (_WIN32) && !defined(__MINGW32__)
	prevExceptionFilter = SetUnhandledExceptionFilter(RecordExceptionInfo);
#endif
#endif

#ifdef LIBBACKTRACE
	backtrace = backtrace_create_state(argv[0], 1, libbacktrace_error_callback, NULL);
#endif

	// startup SRB2
	CONS_Printf("Setting up SRB2...\n");
	D_SRB2Main();
#ifdef LOGMESSAGES
	if (!M_CheckParm("-nolog"))
		CONS_Printf("Logfile: %s\n", logfilename);
#endif
	CONS_Printf("Entering main game loop...\n");
	// never return
	D_SRB2Loop();

#ifdef LIBBACKTRACE
	libbacktrace_print_stacktrace();
#endif

#if defined (_WIN32)
#if 1
	// close debugger
	if (g_hmod_drmingw)
		FreeLibrary(g_hmod_drmingw);
#endif
#endif

#ifdef BUGTRAP
	// This is safe even if BT didn't start.
	ShutdownBugTrap();
#endif

	// return to OS
	return 0;
}
#endif
