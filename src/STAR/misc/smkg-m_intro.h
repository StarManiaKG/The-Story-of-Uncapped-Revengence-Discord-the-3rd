
#include "../../doomstat.h"
#include "../../f_finale.h"
#include "../../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct tsourdt3rd_intro_s
{
	INT32           id;
    tic_t           start_time;
	boolean         (*drawroutine)(void); // draw routine
	boolean         (*tickroutine)(void); // ticker routine
	boolean         (*initroutine)(void); // called when the intro first starts
} tsourdt3rd_intro_t;

// ------------------------ //
//        Functions
// ------------------------ //

void STAR_F_StartIntro(tic_t *introtime);

// ================
// SMKG-M_INTRODEFS
// ================

boolean STAR_F_PureFatDrawer(char *stjrintro, patch_t *background, void *patch, INT32 intro_scenenum, INT32 bgxoffs);
boolean STAR_F_PureFatTicker(INT32 intro_scenenum, INT32 intro_curtime, INT32 animtimer, INT32 next_time);
