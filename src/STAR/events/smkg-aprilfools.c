// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-aprilfools.c
/// \brief April Fools event related functions and data

#include "../smkg-cvars.h"
#include "../ss_main.h"
#include "../core/smkg-s_jukebox.h"
#include "../menus/smkg-m_sys.h" // menumessage //

#include "../../d_main.h" // D_StartTitle
#include "../../g_demo.h"
#include "../../g_game.h" // playeringame[]
#include "../../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

basicmusicdef_t tsourdt3rd_aprilfools_basicdef = {
	"Get Rickrolled LOL",
	"",
	"Who do you think?",
	""
};

musicdef_t tsourdt3rd_aprilfools_def = {
	{ "_hehe" }, // prevents exactly one valid track name from being used on the sound test
	NULL,
	NULL,
	NULL,
	NULL,

	{ 0 },
	{ false },
	1,
	DEFAULT_MUSICDEF_VOLUME,
	DEFAULT_MUSICDEF_VOLUME,
	true,
	false,

	1, // show on soundtest page 1
	0, // with no conditions
	0,
	0,
	0,
	false,

	&tsourdt3rd_aprilfools_basicdef,

	NULL
};

static menuitem_t defaultMenuTitles[256][256];
static gtdesc_t defaultGametypeTitles[NUMGAMETYPES];

#if 0
MP_SplitServerMenu
	{IT_STRING|IT_CALL,              NULL, "Can We Play Tag?", 		   M_MapChange,         100},
	{IT_STRING|IT_CALL,              NULL, "Pet 1 setup...",           M_SetupMultiPlayer,  110},
	{IT_STRING|IT_CALL,              NULL, "Pet 2 setup...",           M_SetupMultiPlayer2, 120},
	{IT_STRING|IT_CALL,              NULL, "More Settings...",         M_ServerOptions,     130},
	{IT_WHITESTRING|IT_CALL,         NULL, "GO!!!",                    M_StartServer,       140},

SR_PandorasBox
	{IT_STRING | IT_CVAR, NULL, "Mods",               &cv_dummyrings,      20},
	{IT_STRING | IT_CVAR, NULL, "Coins",               &cv_dummyrings,      20},
	{IT_STRING | IT_CVAR, NULL, "Lifes",               &cv_dummylives,      30},
	{IT_STRING | IT_CVAR, NULL, "Cat Lives",           &cv_dummycontinues,  40},
	{IT_STRING | IT_CVAR, NULL, "Fall Speed",          &cv_gravity,         60},
	{IT_STRING | IT_CVAR, NULL, "DooM Mode",           &cv_ringslinger,     70},
	{IT_STRING | IT_CALL, NULL, "Enable Goku Mode",    M_AllowSuper,        90},
	{IT_STRING | IT_CALL, NULL, "Get All Gems",    	   M_GetAllEmeralds,   100},
	{IT_STRING | IT_CALL, NULL, "Peaceful Mode",  	   M_DestroyRobots,    110},
	{IT_STRING | IT_CALL, NULL, "Crysis Mode",         M_UltimateCheat,    130},

MP_MainMenu
	{IT_HEADER, NULL, "Input lag mode", NULL, 0},
	{IT_STRING|IT_CALL,       NULL, "Searching for friends...",
															 M_ConnectMenuModChecks, 12},
	{IT_STRING|IT_KEYHANDLER, NULL, "Specify IPv1 address:", M_HandleConnectIP,      22},
	{IT_HEADER, NULL, "Free admin mode", NULL, 54},
	{IT_STRING|IT_CALL,       NULL, "ARPANET/LAN...",        M_StartServerMenu,      66},
	{IT_STRING|IT_CALL,       NULL, "Stretchscreen...",      M_StartSplitServerMenu, 76},
	{IT_HEADER, NULL, "Customise pets", NULL, 94},
	{IT_STRING|IT_CALL,       NULL, "Pet 1...",          	 M_SetupMultiPlayer,    106},
	{IT_STRING|IT_CALL,       NULL, "Pet 2... ",          	 M_SetupMultiPlayer2,   116},

MP_ServerMenu
	{IT_STRING|IT_CALL,              NULL, "The Room...",             M_RoomMenu,          10},
	{IT_STRING|IT_CVAR|IT_CV_STRING, NULL, "Call Server How?",        &cv_servername,      20},
	{IT_STRING|IT_CVAR,              NULL, "Max Friends",             &cv_maxplayers,      46},
	{IT_STRING|IT_CVAR,              NULL, "Allow Mod Downloading",   &cv_downloading,     56},
	{IT_STRING|IT_CALL,              NULL, "Can We Play Tag?",        M_MapChange,        100},
	{IT_STRING|IT_CALL,              NULL, "Moar Settings...",        M_ServerOptions,    130},
	{IT_WHITESTRING|IT_CALL,         NULL, "GO!!",                    M_StartServer,      140},

OP_MainMenu
	{IT_SUBMENU | IT_STRING, NULL, "Pet 1 Buttons...",     &OP_P1ControlsDef,   10},
	{IT_SUBMENU | IT_STRING, NULL, "Pet 2 Buttons...",     &OP_P2ControlsDef,   20},
	{IT_CVAR    | IT_STRING, NULL, "Buttons per key",      &cv_controlperkey,   30},
	{IT_CALL    | IT_STRING, NULL, "Eye Options...",       M_VideoOptions,      50},
	{IT_SUBMENU | IT_STRING, NULL, "Ear Options...",       &OP_SoundOptionsDef, 60},
	{IT_CALL    | IT_STRING, NULL, "Server Options...",    M_ServerOptions,     80},
	{IT_SUBMENU | IT_STRING, NULL, "Datum Options...",     &OP_DataOptionsDef, 100},
#ifdef HAVE_DISCORDSUPPORT
	{IT_CALL 	| IT_STRING, NULL, "Mastadon Options...",  TSoURDt3rd_M_DiscordOptions_Init,   120},
#endif
	{IT_CALL    | IT_STRING, NULL, "Dumb Options...",      TSoURDt3rd_M_Main_InitOptions,130}, // STAR STUFF: our menu! //

OP_P1ControlsMenu
	{IT_CALL    | IT_STRING, NULL, "Button Configuration...", M_Setup1PControlsMenu, 10},
	{IT_SUBMENU | IT_STRING, NULL, "Rat Options...", 		  &OP_MouseOptionsDef,   20},
	{IT_SUBMENU | IT_STRING, NULL, "Gamepad Options...", 	  &OP_Joystick1Def,      30},
	{IT_SUBMENU | IT_STRING, NULL, "Hire new cameraman", 	  &OP_CameraOptionsDef,	 50},
	{IT_STRING  | IT_CVAR, 	 NULL, "EBA", 					  &cv_autobrake,         70},
	{IT_CALL    | IT_STRING, NULL, "Play Style...", 		  M_Setup1PPlaystyleMenu,80},

OP_P2ControlsMenu
	{IT_CALL    | IT_STRING, NULL, "Button Configuration...",   M_Setup2PControlsMenu, 10},
	{IT_SUBMENU | IT_STRING, NULL, "Rodent Options...", 		&OP_Mouse2OptionsDef,  20},
	{IT_SUBMENU | IT_STRING, NULL, "Second Gamepad Options...", &OP_Joystick2Def,      30},
	{IT_SUBMENU | IT_STRING, NULL, "Hire new cameraman", 		&OP_Camera2OptionsDef, 50},
	{IT_STRING  | IT_CVAR, 	 NULL, "EBA", 						&cv_autobrake2,  	   70},
	{IT_CALL    | IT_STRING, NULL, "Play Style...", 			M_Setup2PPlaystyleMenu,80},

OP_ChangeControlsMenu
	{IT_HEADER, NULL, "Driving", NULL, 0},
	{IT_SPACE, NULL, NULL, NULL, 0}, // padding
	{IT_CALL | IT_STRING2, NULL, "Accelerate",     M_ChangeControl, GC_FORWARD     },
	{IT_CALL | IT_STRING2, NULL, "Brake/Reverse",    M_ChangeControl, GC_BACKWARD    },
	{IT_CALL | IT_STRING2, NULL, "Drift Left",        M_ChangeControl, GC_STRAFELEFT  },
	{IT_CALL | IT_STRING2, NULL, "Drift Right",       M_ChangeControl, GC_STRAFERIGHT },
	{IT_CALL | IT_STRING2, NULL, "Hop",             M_ChangeControl, GC_JUMP      },
	{IT_CALL | IT_STRING2, NULL, "Trick",             M_ChangeControl, GC_SPIN     },
	{IT_HEADER, NULL, "Cameraman", NULL, 0},
	{IT_SPACE, NULL, NULL, NULL, 0}, // padding
	{IT_CALL | IT_STRING2, NULL, "Look at the Stars",        M_ChangeControl, GC_LOOKUP      },
	{IT_CALL | IT_STRING2, NULL, "Look at Your Feet",      M_ChangeControl, GC_LOOKDOWN    },
	{IT_CALL | IT_STRING2, NULL, "Look Left",      M_ChangeControl, GC_TURNLEFT    },
	{IT_CALL | IT_STRING2, NULL, "Look Right",     M_ChangeControl, GC_TURNRIGHT   },
	{IT_CALL | IT_STRING2, NULL, "Look Ahead",      M_ChangeControl, GC_CENTERVIEW  },
	{IT_CALL | IT_STRING2, NULL, "Toggle Mouselook", M_ChangeControl, GC_MOUSEAIMING },
	{IT_CALL | IT_STRING2, NULL, "Toggle Cameraman Position", M_ChangeControl, GC_CAMTOGGLE},
	{IT_CALL | IT_STRING2, NULL, "Hire Back Previous Cameraman",     M_ChangeControl, GC_CAMRESET    },
	{IT_HEADER, NULL, "Too Meta", NULL, 0},
	{IT_SPACE, NULL, NULL, NULL, 0}, // padding
	{IT_CALL | IT_STRING2, NULL, "Playground Status",
	M_ChangeControl, GC_SCORES      },
	{IT_CALL | IT_STRING2, NULL, "Pause / Run Retry", M_ChangeControl, GC_PAUSE      },
	{IT_CALL | IT_STRING2, NULL, "Photo",            M_ChangeControl, GC_SCREENSHOT },
	{IT_CALL | IT_STRING2, NULL, "Export Slideshow",  M_ChangeControl, GC_RECORDGIF  },
	{IT_CALL | IT_STRING2, NULL, "Open/Close Menu (ESC)", M_ChangeControl, GC_SYSTEMMENU },
	{IT_CALL | IT_STRING2, NULL, "Spy Someone Else",        M_ChangeControl, GC_VIEWPOINTNEXT },
	{IT_CALL | IT_STRING2, NULL, "Spy Someone Else Backwards",        M_ChangeControl, GC_VIEWPOINTPREV },
	{IT_CALL | IT_STRING2, NULL, "Terminal",          M_ChangeControl, GC_CONSOLE     },
	{IT_HEADER, NULL, "Social Networks", NULL, 0},
	{IT_SPACE, NULL, NULL, NULL, 0}, // padding
	{IT_CALL | IT_STRING2, NULL, "Post",             M_ChangeControl, GC_TALKKEY     },
	{IT_CALL | IT_STRING2, NULL, "Post (Group)", M_ChangeControl, GC_TEAMKEY     },
	{IT_HEADER, NULL, "DooM (Match, CTF, Tag, H&S)", NULL, 0},
	{IT_SPACE, NULL, NULL, NULL, 0}, // padding
	{IT_CALL | IT_STRING2, NULL, "Shoot",             M_ChangeControl, GC_FIRE        },
	{IT_CALL | IT_STRING2, NULL, "Fist",      M_ChangeControl, GC_FIRENORMAL  },
	{IT_CALL | IT_STRING2, NULL, "Abandon Payload",        M_ChangeControl, GC_TOSSFLAG    },
	{IT_CALL | IT_STRING2, NULL, "Next Weapon",      M_ChangeControl, GC_WEAPONNEXT  },
	{IT_CALL | IT_STRING2, NULL, "Prev Weapon",      M_ChangeControl, GC_WEAPONPREV  },
	{IT_CALL | IT_STRING2, NULL, "Pistol",   M_ChangeControl, GC_WEPSLOT1    },
	{IT_CALL | IT_STRING2, NULL, "Machine Gun",        M_ChangeControl, GC_WEPSLOT2    },
	{IT_CALL | IT_STRING2, NULL, "Boomerang",           M_ChangeControl, GC_WEPSLOT3    },
	{IT_CALL | IT_STRING2, NULL, "Dual Pistol",          M_ChangeControl, GC_WEPSLOT4    },
	{IT_CALL | IT_STRING2, NULL, "Grenade",          M_ChangeControl, GC_WEPSLOT5    },
	{IT_CALL | IT_STRING2, NULL, "ShotGun",        M_ChangeControl, GC_WEPSLOT6    },
	{IT_CALL | IT_STRING2, NULL, "Rail Gun",             M_ChangeControl, GC_WEPSLOT7    },
	{IT_HEADER, NULL, "Minecraft", NULL, 0},
	{IT_SPACE, NULL, NULL, NULL, 0}, // padding
	{IT_CALL | IT_STRING2, NULL, "Destroy Block",  M_ChangeControl, GC_CUSTOM1     },
	{IT_CALL | IT_STRING2, NULL, "Place Block",  M_ChangeControl, GC_CUSTOM2     },
	{IT_CALL | IT_STRING2, NULL, "Sneak",  M_ChangeControl, GC_CUSTOM3     },

OP_Joystick1Menu
	{IT_STRING | IT_CALL,  NULL, "Select Joy Stick...", M_Setup1PJoystickMenu, 10},
	{IT_STRING | IT_CVAR,  NULL, "Accelerate \x17 Axis"    , &cv_moveaxis         , 30},
	{IT_STRING | IT_CVAR,  NULL, "Accelerate \x18 Axis"    , &cv_sideaxis         , 40},
	{IT_STRING | IT_CVAR,  NULL, "Aimbot \x17 Axis"  , &cv_lookaxis         , 50},
	{IT_STRING | IT_CVAR,  NULL, "Aimbot \x18 Axis"  , &cv_turnaxis         , 60},
	{IT_STRING | IT_CVAR,  NULL, "Hop Axis"         , &cv_jumpaxis         , 70},
	{IT_STRING | IT_CVAR,  NULL, "Trick Axis"         , &cv_spinaxis         , 80},
	{IT_STRING | IT_CVAR,  NULL, "Shoot Axis"         , &cv_fireaxis         , 90},
	{IT_STRING | IT_CVAR,  NULL, "Fist Axis"  		  , &cv_firenaxis        ,100},
	{IT_STRING | IT_CVAR, NULL, "Mouselook Vert-Look", &cv_alwaysfreelook, 120},
	{IT_STRING | IT_CVAR, NULL, "Cameraman Vert-Look", &cv_chasefreelook,  130},
	{IT_STRING | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Analog Deathzone", &cv_deadzone, 140},
	{IT_STRING | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Digital Deathzone", &cv_digitaldeadzone, 150},

OP_Joystick2Menu
	{IT_STRING | IT_CALL,  NULL, "Select Joy Stick...", M_Setup2PJoystickMenu, 10},
	{IT_STRING | IT_CVAR,  NULL, "Accelerate \x17 Axis"    , &cv_moveaxis2         , 30},
	{IT_STRING | IT_CVAR,  NULL, "Accelerate \x18 Axis"    , &cv_sideaxis2         , 40},
	{IT_STRING | IT_CVAR,  NULL, "Aimbot \x17 Axis"  , &cv_lookaxis2         , 50},
	{IT_STRING | IT_CVAR,  NULL, "Aimbot \x18 Axis"  , &cv_turnaxis2         , 60},
	{IT_STRING | IT_CVAR,  NULL, "Hop Axis"         , &cv_jumpaxis2         , 70},
	{IT_STRING | IT_CVAR,  NULL, "Trick Axis"         , &cv_spinaxis2         , 80},
	{IT_STRING | IT_CVAR,  NULL, "Shoot Axis"         , &cv_fireaxis2         , 90},
	{IT_STRING | IT_CVAR,  NULL, "Fist Axis"  		  , &cv_firenaxis2        ,100},
	{IT_STRING | IT_CVAR, NULL, "Mouselook Vert-Look", &cv_alwaysfreelook2, 120},
	{IT_STRING | IT_CVAR, NULL, "Cameraman Vert-Look", &cv_chasefreelook2,  130},
	{IT_STRING | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Analog Deathzone", &cv_deadzone2, 140},
	{IT_STRING | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Digital Deathzone", &cv_digitaldeadzone2, 150},

OP_MouseOptionsMenu
	{IT_STRING | IT_CVAR, NULL, "Use Rat",        &cv_usemouse,         10},
	{IT_STRING | IT_CVAR, NULL, "First-Person MouseLook", &cv_alwaysfreelook,   30},
	{IT_STRING | IT_CVAR, NULL, "Third-Person MouseLook", &cv_chasefreelook,   40},
	{IT_STRING | IT_CVAR, NULL, "Rat Move",       &cv_mousemove,        50},
	{IT_STRING | IT_CVAR, NULL, "Invert Y Axis",     &cv_invertmouse,      60},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER,
						  NULL, "Rat X Sensitivity",    &cv_mousesens,        70},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER,
						  NULL, "Rat Y Sensitivity",    &cv_mouseysens,        80},

OP_Mouse2OptionsMenu
	{IT_STRING | IT_CVAR, NULL, "Use Rat 2",      &cv_usemouse2,        10},
	{IT_STRING | IT_CVAR, NULL, "Second Mouse Serial Port",
													&cv_mouse2port,       20},
	{IT_STRING | IT_CVAR, NULL, "First-Person MouseLook", &cv_alwaysfreelook2,  30},
	{IT_STRING | IT_CVAR, NULL, "Third-Person MouseLook", &cv_chasefreelook2,  40},
	{IT_STRING | IT_CVAR, NULL, "Rat Move",       &cv_mousemove2,       50},
	{IT_STRING | IT_CVAR, NULL, "Invert Y Axis",     &cv_invertmouse2,     60},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER,
						  NULL, "Mouse X Sensitivity",    &cv_mousesens2,       70},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER,
						  NULL, "Mouse Y Sensitivity",    &cv_mouseysens2,      80},

OP_CameraOptionsMenu
	{IT_HEADER,            NULL, "General Toggles", NULL, 0},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Behind You"  , &cv_chasecam , 6},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Can Walk on Ceiling"  , &cv_flipcam , 11},
	{IT_STRING  | IT_CVAR, NULL, "Globe Looking"  , &cv_cam_orbit , 16},
	{IT_STRING  | IT_CVAR, NULL, "Downhill Slope Adjustment", &cv_cam_adjust, 21},
	{IT_HEADER,                                NULL, "Cameraman Positioning", NULL, 30},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Distance", &cv_cam_savedist[0][0], 36},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Height", &cv_cam_saveheight[0][0], 41},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Cameraman Space Speed", &cv_cam_speed, 46},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Turn how fast??", &cv_cam_turnmultiplier, 51},
	{IT_HEADER,            NULL, "The Display Option", NULL, 60},
	{IT_STRING  | IT_CVAR, NULL, "Hair Cross", &cv_crosshair, 66},

OP_Camera2OptionsMenu
	{IT_HEADER,            NULL, "General Toggles", NULL, 0},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Behind You"  , &cv_chasecam2 , 6},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Can Walk on Ceiling"  , &cv_flipcam2 , 11},
	{IT_STRING  | IT_CVAR, NULL, "Globe Looking"  , &cv_cam2_orbit , 16},
	{IT_STRING  | IT_CVAR, NULL, "Downhill Slope Adjustment", &cv_cam2_adjust, 21},
	{IT_HEADER,                                NULL, "Cameraman Positioning", NULL, 30},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Distance", &cv_cam_savedist[0][1], 36},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Height", &cv_cam_saveheight[0][1], 41},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Cameraman Space Speed", &cv_cam2_speed, 46},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Turn Speed", &cv_cam2_turnmultiplier, 51},
	{IT_HEADER,            NULL, "The Display Option", NULL, 60},
	{IT_STRING  | IT_CVAR, NULL, "Hair Cross", &cv_crosshair2, 66},

OP_CameraExtendedOptionsMenu
	{IT_HEADER,            NULL, "General Toggles", NULL, 0},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Behind You"  , &cv_chasecam , 6},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Can Walk on Ceiling"  , &cv_flipcam , 11},
	{IT_STRING  | IT_CVAR, NULL, "Orbital Looking"  , &cv_cam_orbit , 16},
	{IT_STRING  | IT_CVAR, NULL, "Downhill Slope Adjustment", &cv_cam_adjust, 21},
	{IT_HEADER,                                NULL, "Cameraman Positioning", NULL, 30},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Distance", &cv_cam_savedist[1][0], 36},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Height", &cv_cam_saveheight[1][0], 41},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Cameraman Space Speed", &cv_cam_speed, 46},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Turn how fast??", &cv_cam_turnmultiplier, 51},
	{IT_HEADER,                           NULL, "Magic Cameraman", NULL, 60},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Shift to player angle", &cv_cam_shiftfacing[0],  66},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to player angle", &cv_cam_turnfacing[0],  71},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to ability", &cv_cam_turnfacingability[0],  76},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to spindash", &cv_cam_turnfacingspindash[0],  81},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to input", &cv_cam_turnfacinginput[0],  86},
	{IT_HEADER,            NULL, "Locked Camera Options", NULL, 95},
	{IT_STRING  | IT_CVAR, NULL, "Lock button behavior", &cv_cam_centertoggle[0],  101},
	{IT_STRING  | IT_CVAR, NULL, "Sideways movement", &cv_cam_lockedinput[0],  106},
	{IT_STRING  | IT_CVAR, NULL, "Targeting assist", &cv_cam_lockonboss[0],  111},
	{IT_HEADER,            NULL, "The Display Option", NULL, 120},
	{IT_STRING  | IT_CVAR, NULL, "Hair Cross", &cv_crosshair, 126},

OP_Camera2ExtendedOptionsMenu
	{IT_HEADER,            NULL, "General Toggles", NULL, 0},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Behind You"  , &cv_chasecam2 , 6},
	{IT_STRING  | IT_CVAR, NULL, "Cameraman Can Walk on Ceiling"  , &cv_flipcam2 , 11},
	{IT_STRING  | IT_CVAR, NULL, "Orbital Looking"  , &cv_cam2_orbit , 16},
	{IT_STRING  | IT_CVAR, NULL, "Downhill Slope Adjustment", &cv_cam2_adjust, 21},
	{IT_HEADER,                                NULL, "Cameraman Positioning", NULL, 30},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Distance", &cv_cam_savedist[1][1], 36},
	{IT_STRING  | IT_CVAR | IT_CV_INTEGERSTEP, NULL, "Cameraman Height", &cv_cam_saveheight[1][1], 41},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Cameraman Space Speed", &cv_cam2_speed, 46},
	{IT_STRING  | IT_CVAR | IT_CV_FLOATSLIDER, NULL, "Turn how fast??", &cv_cam2_turnmultiplier, 51},
	{IT_HEADER,                           NULL, "Magic Cameraman", NULL, 60},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Shift to player angle", &cv_cam_shiftfacing[1],  66},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to player angle", &cv_cam_turnfacing[1],  71},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to ability", &cv_cam_turnfacingability[1],  76},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to spindash", &cv_cam_turnfacingspindash[1],  81},
	{IT_STRING  | IT_CVAR | IT_CV_SLIDER, NULL, "Turn to input", &cv_cam_turnfacinginput[1],  86},
	{IT_HEADER,            NULL, "Locked Camera Options", NULL, 95},
	{IT_STRING  | IT_CVAR, NULL, "Lock button behavior", &cv_cam_centertoggle[1],  101},
	{IT_STRING  | IT_CVAR, NULL, "Sideways movement", &cv_cam_lockedinput[1],  106},
	{IT_STRING  | IT_CVAR, NULL, "Targeting assist", &cv_cam_lockonboss[1],  111},
	{IT_HEADER,            NULL, "The Display Option", NULL, 120},
	{IT_STRING  | IT_CVAR, NULL, "Hair Cross", &cv_crosshair2, 126},

OP_VideoOptionsMenu
	{IT_HEADER, NULL, "Screen", NULL, 0},
	{IT_STRING | IT_CALL,  NULL, "Screen how big?",       M_VideoModeMenu,          6},
#if (defined (__unix__) && !defined (MSDOS)) || defined (UNIXCOMMON) || defined (HAVE_SDL)
	{IT_STRING|IT_CVAR,      NULL, "Make big screen",             &cv_fullscreen,         11},
#endif
	{IT_STRING | IT_CVAR, NULL, "Vertical Sync",                &cv_vidwait,         16},
#ifdef HWRENDER
	{IT_STRING | IT_CVAR, NULL, "Renderer",                     &cv_renderer,        21},
#else
	{IT_TRANSTEXT | IT_PAIR, "Renderer", "Software",            &cv_renderer,           21},
#endif
	{IT_HEADER, NULL, "Color Profile", NULL, 30},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness (F11)", &cv_globalgamma,36},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation", &cv_globalsaturation, 41},
	{IT_SUBMENU|IT_STRING, NULL, "Advanced Settings...",     &OP_ColorOptionsDef,  46},
	{IT_HEADER, NULL, "Hedgehog Useful Details", NULL, 55},
	{IT_STRING | IT_CVAR, NULL, "Show HUD",                  &cv_showhud,          61},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER,
						  NULL, "HUD Transparency",          &cv_translucenthud,   66},
	{IT_STRING | IT_CVAR, NULL, "Score/Time/Rings",          &cv_timetic,          71},
	{IT_STRING | IT_CVAR, NULL, "Show Powerups",             &cv_powerupdisplay,   76},
	{IT_STRING | IT_CVAR, NULL, "Show my lag",		&cv_showping,			81}, // shows ping next to framerate if we want to.
	{IT_STRING | IT_CVAR, NULL, "Show other pets' names",         &cv_seenames,         86},
	{IT_HEADER, NULL, "Console", NULL, 95},
	{IT_STRING | IT_CVAR, NULL, "Color behind letters",          &cons_backcolor,      101},
	{IT_STRING | IT_CVAR, NULL, "Size of letters",                 &cv_constextsize,    106},
	{IT_HEADER, NULL, "Instant Messaging Mode", NULL, 115},
	{IT_STRING | IT_CVAR, NULL, "Messaging Mode",            		 	 &cv_consolechat,  121},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Box Width",    &cv_chatwidth,     126},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Box Height",   &cv_chatheight,    131},
	{IT_STRING | IT_CVAR, NULL, "Fadded Time",              &cv_chattime,    136},
	{IT_STRING | IT_CVAR, NULL, "Beep continuously",           	 &cv_chatnotifications,  141},
	{IT_STRING | IT_CVAR, NULL, "Spam Protection",           		 &cv_chatspamprotection,  146},
	{IT_STRING | IT_CVAR, NULL, "Background tint",           	 &cv_chatbacktint,  151},
	{IT_HEADER, NULL, "Map", NULL, 160},
	{IT_STRING | IT_CVAR, NULL, "Draw Very Far?",             &cv_drawdist,        166},
	{IT_STRING | IT_CVAR, NULL, "Draw Weather How Far?",        &cv_drawdist_precip, 171},
	{IT_STRING | IT_CVAR, NULL, "And NiGHTS Hoop??",    &cv_drawdist_nights, 176},
	{IT_HEADER, NULL, "Diagnostic", NULL, 184},
	{IT_STRING | IT_CVAR, NULL, "Show Slideshow Speed",                  &cv_ticrate,         190},
	{IT_STRING | IT_CVAR, NULL, "Clear Before Next Slide",       &cv_homremoval,      195},
	{IT_STRING | IT_CVAR, NULL, "Complain about lost focus",       &cv_showfocuslost,   200},
#ifdef HWRENDER
	{IT_HEADER, NULL, "voodoo graphics", NULL, 208},
	{IT_CALL | IT_STRING, NULL, "OpenGraphicsLibrary...",         M_OpenGLOptionsMenu, 214},
	{IT_STRING | IT_CVAR, NULL, "2.3 Leaks",      				  &cv_fpscap,          219},
#endif
#ifdef ALAM_LIGHTING
	{IT_SUBMENU | IT_STRING, NULL, "ReShade Options...",	 &OP_SoftwareLightingDef,	229},
#endif

OP_ColorOptionsMenu
	{IT_STRING | IT_CALL, NULL, "Reset to defaults", M_ResetCvars, 0},
	{IT_HEADER, NULL, "Crimson", NULL, 9},
	{IT_DISABLED, NULL, NULL, NULL, 35},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Hue",          &cv_rhue,         15},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation",   &cv_rsaturation,  20},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness",   &cv_rgamma,       25},
	{IT_HEADER, NULL, "Gold", NULL, 34},
	{IT_DISABLED, NULL, NULL, NULL, 73},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Hue",          &cv_yhue,         40},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation",   &cv_ysaturation,  45},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness",   &cv_ygamma,       50},
	{IT_HEADER, NULL, "Emerald", NULL, 59},
	{IT_DISABLED, NULL, NULL, NULL, 112},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Hue",          &cv_ghue,         65},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation",   &cv_gsaturation,  70},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness",   &cv_ggamma,       75},
	{IT_HEADER, NULL, "Aqua", NULL, 84},
	{IT_DISABLED, NULL, NULL, NULL, 255},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Hue",          &cv_chue,         90},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation",   &cv_csaturation,  95},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness",   &cv_cgamma,      100},
	{IT_HEADER, NULL, "Lapis-Lazuli", NULL, 109},
	{IT_DISABLED, NULL, NULL, NULL, 152},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Hue",          &cv_bhue,        115},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation",   &cv_bsaturation, 120},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness",   &cv_bgamma,      125},
	{IT_HEADER, NULL, "Pink", NULL, 134},
	{IT_DISABLED, NULL, NULL, NULL, 181},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Hue",          &cv_mhue,        140},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Saturation",   &cv_msaturation, 145},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Brightness",   &cv_mgamma,      150},

OP_OpenGLOptionsMenu
	{IT_HEADER, NULL, "3D Sprites", NULL, 0},
	{IT_STRING|IT_CVAR,         NULL, "3D sprites",          &cv_glmodels,        	   12},
	{IT_STRING|IT_CVAR,         NULL, "Slide interpolation", &cv_glmodelinterpolation, 22},
	{IT_STRING|IT_CVAR,         NULL, "Ambient lighting",    &cv_glmodellighting,      32},
	{IT_HEADER, NULL, "Main Stuff", NULL, 51},
	{IT_STRING|IT_CVAR,         NULL, "Cool effects",        &cv_glshaders,      	   63},
	{IT_STRING|IT_CVAR,         NULL, "Software Mode",   	 &cv_glpaletterendering,   73},
	{IT_STRING|IT_CVAR,         NULL, "Vertical walls", 	 &cv_glshearing,           83},
	{IT_STRING|IT_CVAR,         NULL, "Quake pro rate",      &cv_fov,                  93},
	{IT_HEADER, NULL, "The Other Stuff", NULL, 112},
	{IT_STRING|IT_CVAR,         NULL, "Bit depth",           &cv_scr_depth,           124},
	{IT_STRING|IT_CVAR,         NULL, "Texture filter",      &cv_glfiltermode,        134},
	{IT_STRING|IT_CVAR,         NULL, "Anisotropic",         &cv_glanisotropicmode,   144},
#ifdef ALAM_LIGHTING
	{IT_SUBMENU|IT_STRING,     	NULL, "Lighting...",         &OP_OpenGLLightingDef,   154},
#endif
#if defined (_WINDOWS) && (!((defined (__unix__) && !defined (MSDOS)) || defined (UNIXCOMMON) || defined (HAVE_SDL)))
	{IT_STRING|IT_CVAR,         NULL, "Make big screen",     &cv_fullscreen,          164},
#endif

OP_SoundOptionsMenu
	{IT_HEADER, NULL, "Game Noise", NULL, 0},
	{IT_STRING | IT_CVAR,  NULL,  "Noise", &cv_gamesounds, 6},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "How Noisy?", &cv_soundvolume, 11},
	{IT_STRING | IT_CVAR,  NULL,  "Digital Music", &cv_gamedigimusic, 21},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Digital Music Noise", &cv_digmusicvolume,  26},
	{IT_STRING | IT_CVAR,  NULL,  "Old Music", &cv_gamemidimusic, 36},
	{IT_STRING | IT_CVAR | IT_CV_SLIDER, NULL, "Old Music Noise", &cv_midimusicvolume, 41},
	{IT_STRING | IT_CVAR,  NULL,  "Musical Preferences", &cv_musicpref, 51},
	{IT_HEADER, NULL, "Etc etc etc", NULL, 61},
	{IT_STRING | IT_CVAR, NULL, "Opened Captioning", &cv_closedcaptioning, 67},
	{IT_STRING | IT_CVAR, NULL, "Dying Kills Music Too", &cv_resetmusic, 72},
	{IT_STRING | IT_CVAR, NULL, "Default 1-Up sound", &cv_1upsound, 77},
	{IT_STRING | IT_SUBMENU, NULL, "Pro Settings...", &OP_SoundAdvancedDef, 87},

OP_SoundAdvancedMenu
#ifdef HAVE_OPENMPT
	{IT_HEADER, NULL, "OpenMPT Settings", NULL, 0},
	{IT_STRING | IT_CVAR, NULL, "Old Music Filter", &cv_modfilter, 12},
#endif
#ifdef HAVE_MIXERX
	{IT_HEADER, NULL, "Old Music Settings", NULL, OPENMPT_MENUOFFSET},
	{IT_STRING | IT_CVAR, NULL, "Old Player", &cv_midiplayer, OPENMPT_MENUOFFSET+12},
	{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "FluidSynth Sound Font File", &cv_midisoundfontpath, OPENMPT_MENUOFFSET+24},
	{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "TiMidity++ Config Folder", &cv_miditimiditypath, OPENMPT_MENUOFFSET+51},
#endif
	{IT_HEADER, NULL, "Miscellaneous", NULL, OPENMPT_MENUOFFSET+MIXERX_MENUOFFSET},
	{IT_STRING | IT_CVAR, NULL, "Play Sound Effects if AFK", &cv_playsoundsifunfocused, OPENMPT_MENUOFFSET+MIXERX_MENUOFFSET+12},
	{IT_STRING | IT_CVAR, NULL, "Play Music if AFK", &cv_playmusicifunfocused, OPENMPT_MENUOFFSET+MIXERX_MENUOFFSET+22},
	{IT_STRING | IT_CVAR, NULL, "Let Levels Force Reset Music", &cv_resetmusicbyheader, OPENMPT_MENUOFFSET+MIXERX_MENUOFFSET+32},

OP_DataOptionsMenu
	{IT_STRING | IT_CALL,    NULL, "Mod Options...",     M_AddonsOptions,     10},
	{IT_STRING | IT_CALL,    NULL, "Photo Options...", M_ScreenshotOptions, 20},
	{IT_STRING | IT_SUBMENU, NULL, "\x85" "Reformat Hard-Drive...",  &OP_EraseDataDef,    40},

OP_ScreenshotOptionsMenu
	{IT_HEADER, NULL, "General", NULL, 0},
	{IT_STRING|IT_CVAR, NULL, "Use color profile", &cv_screenshot_colorprofile,     6},
	{IT_HEADER, NULL, "Photos (F8)", NULL, 16},
	{IT_STRING|IT_CVAR, NULL, "Store Location",  &cv_screenshot_option,          22},
	{IT_STRING|IT_CVAR|IT_CV_STRING, NULL, "Custom Directory", &cv_screenshot_folder, 27},
	{IT_STRING|IT_CVAR, NULL, "Thicc Level",      &cv_zlib_memory,                42},
	{IT_STRING|IT_CVAR, NULL, "Crush Level", &cv_zlib_level,                 47},
	{IT_STRING|IT_CVAR, NULL, "Strategy",          &cv_zlib_strategy,              52},
	{IT_STRING|IT_CVAR, NULL, "Window Bigness",       &cv_zlib_window_bits,           57},
	{IT_HEADER, NULL, "Cinema Mode (F9)", NULL, 64},
	{IT_STRING|IT_CVAR, NULL, "Store Location",  &cv_movie_option,               70},
	{IT_STRING|IT_CVAR|IT_CV_STRING, NULL, "Custom Directory", &cv_movie_folder, 	   75},
	{IT_STRING|IT_CVAR, NULL, "Capture Mode",      &cv_moviemode,                  90},
	{IT_STRING|IT_CVAR, NULL, "Downscaling",       &cv_gif_downscale,              95},
	{IT_STRING|IT_CVAR, NULL, "Region Optimizing", &cv_gif_optimize,              100},
	{IT_STRING|IT_CVAR, NULL, "Local Color Table", &cv_gif_localcolortable,       105},
	{IT_STRING|IT_CVAR, NULL, "Downscaling",       &cv_apng_downscale,             95},
	{IT_STRING|IT_CVAR, NULL, "Thicc Level",      &cv_zlib_memorya,              100},
	{IT_STRING|IT_CVAR, NULL, "Crush Level", &cv_zlib_levela,               105},
	{IT_STRING|IT_CVAR, NULL, "Strategy",          &cv_zlib_strategya,            110},
	{IT_STRING|IT_CVAR, NULL, "Window Bigness",       &cv_zlib_window_bitsa,         115},

#endif

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_AprilFools_ModeEnabled(void)
// Returns whether or not TSoURDt3rd is in April Fools Mode.
//
boolean TSoURDt3rd_AprilFools_ModeEnabled(void)
{
	return ((tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_APRILFOOLS) && cv_tsourdt3rd_aprilfools_ultimatemode.value);
}

//
// void TSoURDt3rd_AprilFools_StoreDefaultMenuStrings(void)
// Stores the default menu title strings in the 'defaultMenuTitles' table.
//
void TSoURDt3rd_AprilFools_StoreDefaultMenuStrings(void)
{
	memset(defaultMenuTitles, 0, sizeof(defaultMenuTitles)); // Resets the default menu title table.
	memset(defaultGametypeTitles, 0, sizeof(defaultGametypeTitles)); // Resets the default gametype title table.
	memcpy(&defaultGametypeTitles, &gametypedesc, sizeof(defaultGametypeTitles)); // Gametypes
	memcpy(&defaultMenuTitles[0], &MainMenu, sizeof(defaultMenuTitles[0])); // Main Menu
	memcpy(&defaultMenuTitles[1], &SP_MainMenu, sizeof(defaultMenuTitles[1])); // SP Main Menu
	memcpy(&defaultMenuTitles[2], &MPauseMenu, sizeof(defaultMenuTitles[2])); // MP Pause
	memcpy(&defaultMenuTitles[3], &SPauseMenu, sizeof(defaultMenuTitles[3])); // SP Pause
}

//
// void TSoURD3rd_AprilFools_OnChange(void)
// Routine for the main April Fools command.
//
static void AprilFools_ChangeMenus(void)
{
	if (tsourdt3rd_aprilfools_def.bpm == 0)
	{
		// Manually set the BPM, since doing otherwise can either cause SIGFPE crashes or fast radio bouncing
		tsourdt3rd_aprilfools_def.bpm = FixedDiv((60*TICRATE)<<FRACBITS, FLOAT_TO_FIXED(10));
	}

	if (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_APRILFOOLS))
		return;

	if (menuactive)
	{
		M_ClearMenus(true);
		if (!Playing())
			D_StartTitle();
	}
	if (demoplayback && titledemo)
		G_CheckDemoStatus();

	if (!cv_tsourdt3rd_aprilfools_ultimatemode.value)
	{
		memmove(&gametypedesc, &defaultGametypeTitles, sizeof(gtdesc_t *)); // Gametypes
		memmove(&MainMenu, &defaultMenuTitles[0], sizeof(defaultMenuTitles[0])); // Main Menu
		memmove(&SP_MainMenu, &defaultMenuTitles[1], sizeof(defaultMenuTitles[1])); // SP Main Menu
		memmove(&MPauseMenu, &defaultMenuTitles[2], sizeof(defaultMenuTitles[2])); // MP Pause
		memmove(&SPauseMenu, &defaultMenuTitles[3], sizeof(defaultMenuTitles[3])); // SP Pause
		TSoURDt3rd_AprilFools_StoreDefaultMenuStrings();
		return;
	}

	// Gametypes //
	strcpy(gametypedesc[0].notes, "Hang out with your friends!");
	strcpy(gametypedesc[1].notes, "Challenge your friends in this epic coding competition!");
	strcpy(gametypedesc[2].notes, "Mash the thok button until you find the exit sign.");
	strcpy(gametypedesc[3].notes, "Use your thok to locate targets with a ping higher than yours and keep shooting them with rail rings until they ragequit!");
	strcpy(gametypedesc[4].notes, "Join the team with the highest score and shoot in random directions until your team wins!");
	strcpy(gametypedesc[5].notes, "Normally the IT guy is the one being chased, but for some reason it's the opposite in this gametype.");
	strcpy(gametypedesc[6].notes, "Play PropHunt but without the ability to fuse with your environment!");
	strcpy(gametypedesc[7].notes, "Join the team with the most points, steal the payload, find a safe spot to hide until your friends bring back your team's payload to the base, then rush to your base!");

	// Menus //
	// Main Menu
	MainMenu[0].text						= "No Friends Mode";
	MainMenu[1].text						= "The Friend Zone";
	MainMenu[2].text						= "More Stuff";
	MainMenu[3].text						= "Mods";
	MainMenu[4].text						= "Settings";
	MainMenu[5].text						= "EXIT TO DOS";
	MainMenu[6].text 						= "DOOM EASTER EGG THING!";
	// SP Main Menu
	SP_MainMenu[0].text						= "GO!!";
	SP_MainMenu[1].text 					= "sonic runners";
	SP_MainMenu[2].text 					= "good night mode";
	SP_MainMenu[3].text						= "super mario run";
	SP_MainMenu[4].text						= "how do i jump";
	SP_MainMenu[5].text						= "are we there yet";
	// MP Pause
	MPauseMenu[0].text						= "Plugins...";
	MPauseMenu[1].text						= "Scramble Groups...";
	MPauseMenu[2].text						= SPauseMenu[2].text;
	MPauseMenu[3].text						= "Can We Play Tag?";
#ifdef HAVE_DISCORDSUPPORT
	MPauseMenu[4].text						= "Facebook Requests...";
#endif
	MPauseMenu[5].text						= "Keep Going";
	MPauseMenu[6].text						= "Pet 1 Setup";
	MPauseMenu[7].text						= "Pet 2 Setup";
	MPauseMenu[8].text						= "Watching From The Walls";
	MPauseMenu[9].text						= "Enter Playground";
	MPauseMenu[10].text						= "Join Group...";
	MPauseMenu[11].text						= "Customise Pet";
	MPauseMenu[12].text						= MainMenu[4].text;
	MPauseMenu[13].text						= "Leave Group";
	MPauseMenu[14].text						= MainMenu[5].text;
	// SP Pause
	SPauseMenu[0].text 						= "Mods";
	SPauseMenu[1].text						= "Enable Hacks";
	SPauseMenu[2].text						= "where are the emblems help";
	SPauseMenu[3].text						= "What Map??";
	SPauseMenu[4].text						= "Keep Going";
	SPauseMenu[5].text						= "Try Again";
	SPauseMenu[6].text						= "Settings";
	SPauseMenu[7].text						= "Bored Already";
	SPauseMenu[8].text						= MainMenu[5].text;
}

void TSoURD3rd_AprilFools_OnChange(void)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];

	if (!tsourdt3rd_user || tsourdt3rd_currentEvent != TSOURDT3RD_EVENT_APRILFOOLS)
		return;

	AprilFools_ChangeMenus();
	if (!Playing())
		return;

	if (TSoURDt3rd_AprilFools_ModeEnabled() && cursaveslot > NOSAVESLOT && !netgame)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_APRILFOOLS|STAR_CONS_WARNING, "You have the April Fools features enabled.\nTherefore, to prevent dumb things from happening,\nthis savefile will not save until you turn this mode off.\n");
		TSoURDt3rd_M_StartMessage(
			"Important TSoURDt3rd Notice",
			"You have the April Fools features enabled.\nTherefore, to prevent dumb things from happening,\nthis savefile will not save until you turn this mode off.",
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		cursaveslot = NOSAVESLOT;
	}

	S_StopMusic();
	strncpy(mapmusname, TSoURDt3rd_DetermineLevelMusic(), 7);

	mapmusname[6] = 0;
	mapmusflags = (mapheaderinfo[gamemap-1]->mustrack & MUSIC_TRACKMASK);
	mapmusposition = mapheaderinfo[gamemap-1]->muspos;

	S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0);
}
