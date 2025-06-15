
#include "smkg-m_intro.h"
#include "../smkg-cvars.h"

//
// void STAR_F_StartIntro(tic_t *introtime)
// Sets custom intro times.
//
void STAR_F_StartIntro(tic_t *introtime)
{
	switch (cv_tsourdt3rd_game_startup_intro.value)
	{
		case 0: // 'STJr Presents' Intro
			introtime[0] = 5*TICRATE;
			break;
		case 1: // 'Pure Fat' Intro
			introtime[0] = (7*TICRATE + (TICRATE/2));
			break;
		default:
			break;
	}
}
