// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2023 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file m_perfstats.h
/// \brief Performance measurement tools.

#ifndef __M_PERFSTATS_H__
#define __M_PERFSTATS_H__

#include "doomdef.h"
#include "lua_script.h"
#include "p_local.h"

typedef struct
{
	union {
		precise_t p;
		INT32 i;
	} value;
	void *history;
} ps_metric_t;

typedef struct
{
	ps_metric_t time_taken;
	char short_src[LUA_IDSIZE];
} ps_hookinfo_t;

#define PS_START_TIMING(metric) metric.value.p = I_GetPreciseTime()
#define PS_STOP_TIMING(metric) metric.value.p = I_GetPreciseTime() - metric.value.p

extern ps_metric_t ps_tictime;

extern ps_metric_t ps_playerthink_time;
extern ps_metric_t ps_thinkertime;

extern ps_metric_t ps_thlist_times[];

extern ps_metric_t ps_checkposition_calls;

extern ps_metric_t ps_lua_prethinkframe_time;
extern ps_metric_t ps_lua_thinkframe_time;
extern ps_metric_t ps_lua_postthinkframe_time;
extern ps_metric_t ps_lua_mobjhooks;

extern ps_metric_t ps_otherlogictime;

void PS_SetPreThinkFrameHookInfo(int index, precise_t time_taken, char* short_src);
void PS_SetThinkFrameHookInfo(int index, precise_t time_taken, char* short_src);
void PS_SetPostThinkFrameHookInfo(int index, precise_t time_taken, char* short_src);

void PS_UpdateTickStats(void);

void M_DrawPerfStats(void);

void PS_PerfStats_OnChange(void);
void PS_SampleSize_OnChange(void);

#if 1
struct perfstatrow;

typedef struct perfstatrow perfstatrow_t;

struct perfstatrow {
	const char  * lores_label;
	const char  * hires_label;
	ps_metric_t * metric;
	UINT8         flags;
};

#define PS_TIME      1  // metric measures time (uses precise_t instead of INT32)
#define PS_LEVEL     2  // metric is valid only when a level is active
#define PS_SW        4  // metric is valid only in software mode
#define PS_HW        8  // metric is valid only in opengl mode
#define PS_BATCHING  16 // metric is valid only when opengl batching is active
#define PS_HIDE_ZERO 32 // hide metric if its value is zero

extern perfstatrow_t rendertime_rows[];
extern perfstatrow_t interpolation_rows[];
extern perfstatrow_t batchcount_rows[];
extern perfstatrow_t batchcalls_rows[];

void PS_UpdateFrameStats(void);
INT32 PS_GetMetricScreenValue(ps_metric_t *metric, boolean time_metric);
#endif

#endif
