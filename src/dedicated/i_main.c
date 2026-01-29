// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  d_main.c
/// \brief Main program, simply calls D_SRB2Main and D_SRB2Loop, the high level loop.

#include "../doomdef.h"
#include "../m_argv.h"
#include "../d_main.h"
#include "../m_misc.h" /* path shit */
#include "../i_system.h"
#include "../netcode/d_clisrv.h"

#if defined (_WIN32)
#include "../win32/win_dbg.h"
#endif

#if defined (__GNUC__) || defined (__unix__)
#include <unistd.h>
#endif

#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
#include <errno.h>
#endif

#include <time.h> // For log timestamps

#ifdef HAVE_LIBBACKTRACE
// <backtrace.h> is included in i_system.h
struct backtrace_state *srb2_backtrace_state = NULL;
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

#ifdef _WIN32
static void ChDirToExe(void)
{
	CHAR path[MAX_PATH];
	if (GetModuleFileNameA(NULL, path, MAX_PATH) > 0)
	{
		strrchr(path, '\\')[0] = '\0';
		SetCurrentDirectoryA(path);
	}
}
#endif

#ifdef HAVE_LIBBACKTRACE
static void libbacktrace_error_callback(void *data, const char *msg, int errnum)
{
	(void)data;
	if (errnum == -1)
		I_OutputMsg("libbacktrace error (no debug or other PE problem): %s\n", msg);
	else
		I_OutputMsg("libbacktrace error: %s (errno %d)\n", msg, errnum);
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

int main(int argc, char **argv)
{
	myargc = argc;
	myargv = argv; /// \todo pull out path to exe from this string

	dedicated = true;

#ifdef _WIN32
	ChDirToExe();
#endif

#ifdef LOGMESSAGES
	if (!M_CheckParm("-nolog"))
		InitLogging();
#endif

	//I_OutputMsg("I_StartupSystem() ...\n");
	I_StartupSystem();

#ifdef HAVE_LIBBACKTRACE
	if (!M_CheckParm("-nolibbacktrace"))
	{
		I_OutputMsg("Setting up libbacktrace debugger...\n");
		srb2_backtrace_state = backtrace_create_state(myargv[0], 1, libbacktrace_error_callback, NULL);

		if (srb2_backtrace_state == NULL)
		{
			I_OutputMsg("libbacktrace: backtrace_create_state returned NULL\n");
		}
		else
		{
			I_OutputMsg("libbacktrace debugger initialized!\n");
		}
	}
#endif

#if defined (_WIN32) && defined (HAVE_DRMINGW)
	// Open drmingw debugger
	InitDrMingw();
#endif

#if defined (_WIN32) && defined (HAVE_BUGTRAP)
	// Set up BugTrap
	InitBugTrap();
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

	// return to OS
	return 0;
}
