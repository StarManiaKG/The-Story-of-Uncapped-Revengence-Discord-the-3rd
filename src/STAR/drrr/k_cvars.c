// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by James Robert Roman
// Copyright (C) 2024 by Kart Krew
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------

#include "k_cvars.h"
#include "kg_input.h"

static CV_PossibleValue_t deadzone_cons_t[] = {{0, "MIN"}, {FRACUNIT, "MAX"}, {0, NULL}};

static void PadRumble_OnChange(void);
static void PadRumble2_OnChange(void);

consvar_t cv_tsourdt3rd_drrr_deadzone[MAXSPLITSCREENPLAYERS] = {
	CVAR_INIT ("tsourdt3rd_drrr_deadzone", "0.25", CV_SAVE|CV_FLOAT, deadzone_cons_t, NULL),
	CVAR_INIT ("tsourdt3rd_drrr_deadzone2", "0.25", CV_SAVE|CV_FLOAT, deadzone_cons_t, NULL)
};

consvar_t cv_tsourdt3rd_drrr_rumble[2] = {
	CVAR_INIT ("tsourdt3rd_drrr_rumble", "Off", CV_SAVE|CV_CALL, CV_OnOff, PadRumble_OnChange),
	CVAR_INIT ("tsourdt3rd_drrr_rumble2", "Off", CV_SAVE|CV_CALL, CV_OnOff, PadRumble2_OnChange)
};

consvar_t cv_tsourdt3rd_drrr_debugvirtualkeyboard = CVAR_INIT ("tsourdt3rd_drrr_debugvirtualkeyboard", "Off", CV_SAVE, CV_OnOff, NULL);

static void PadRumble_OnChange(void)
{
	if (cv_tsourdt3rd_drrr_rumble[0].value == 0)
		G_ResetPlayerDeviceRumble(0);
}

static void PadRumble2_OnChange(void)
{
	if (cv_tsourdt3rd_drrr_rumble[1].value == 0)
		G_ResetPlayerDeviceRumble(1);
}
