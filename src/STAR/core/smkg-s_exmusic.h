
#include "smkg-s_audio.h"

#include "../../m_random.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_EXMUSIC_TYPES 4

#define TSOURDT3RD_EXMUSIC_MAX_STARTING_TRACKS 2
	#define TSOURDT3RD_EXMUSIC_STARTING_DEFAULT 0
	#define TSOURDT3RD_EXMUSIC_STARTING_RANDOM 1

#define TSOURDT3RD_EXMUSIC_MAX_LUMPS 8

#define TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME 30

#define EXMUSIC_EXISTS_BY_DEF(lump) \
	def = lump; \
	TSoURDt3rd_S_MusicExists(def, { \
		CONS_Printf("def %s is valid\n", def->name); \
		return def; \
	})

typedef struct tsourdt3rd_exmusic_data_series_s {
	char name[TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME];	// -- The series that the data within this struct belongs to.
	void *tracks;									// -- 
	struct tsourdt3rd_exmusic_data_series_s *prev;	// -- Linked pointer to previous data we can use.
	struct tsourdt3rd_exmusic_data_series_s *next;	// -- Linked pointer to new data we can use.
} tsourdt3rd_exmusic_data_series_t;

typedef struct tsourdt3rd_exmusic_data_identifiers_s {
	consvar_t *cvar;
	const char *name;
} tsourdt3rd_exmusic_data_identifiers_t;

typedef struct tsourdt3rd_exmusic_data_s {
	size_t init_size;
	struct tsourdt3rd_exmusic_data_identifiers_s *identifiers;
	struct tsourdt3rd_exmusic_data_series_s **series;
	void *default_series_data;
	size_t num_series;
} tsourdt3rd_exmusic_data_t;

typedef struct tsourdt3rd_exmusic_s {
	boolean									active;
	size_t									init_size;
	const char								*identifier;
	struct tsourdt3rd_exmusic_data_s		*data;

	boolean									(*init_routine)(struct tsourdt3rd_exmusic_s *extype);
	struct tsourdt3rd_exmusic_s				*(*return_routine)(consvar_t *);
	musicdef_t								*(*play_routine)(consvar_t *);
	musicdef_t								*(*lump_validity_routine)(consvar_t *);

	struct tsourdt3rd_exmusic_s 			*prev;
	struct tsourdt3rd_exmusic_s 			*next;
} tsourdt3rd_exmusic_t;

extern tsourdt3rd_exmusic_t **tsourdt3rd_exmusic_container;

// Sets the default names of our global EXMusic tables.
// The amount here should be corresponded to the value of 'TSOURDT3RD_EXMUSIC_MAX_STARTING_TRACKS'.
extern const char *tsourdt3rd_exmusic_default_names[];

// =============
// EXMusic Types
// =============

extern tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_defaultmaptrack;
extern tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_defaultmaptrack;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_defaultmaptrack;

extern tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_gameover;
extern tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_gameover;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_gameover;

extern tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_bosses;
extern tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_bosses;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_bosses;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_bosspinch;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_finalbosses;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_finalbosspinch;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_truefinalbosses;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_truefinalbosspinch;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_racebosses;

extern tsourdt3rd_exmusic_t tsourdt3rd_default_typedata_intermission;
extern tsourdt3rd_exmusic_t *tsourdt3rd_global_exmusic_intermission;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_intermission;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_intermission_bosses;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_intermission_finalbosses;
	extern consvar_t cv_tsourdt3rd_audio_exmusic_intermission_truefinalbosses;

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_EXMusic_Init(tsourdt3rd_exmusic_t *extype, tsourdt3rd_exmusic_t extype_defaults);

tsourdt3rd_exmusic_t *TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier(const char *identifier);
tsourdt3rd_exmusic_t *TSoURDt3rd_EXMusic_ReturnTypeFromCVar(consvar_t *cvar);
	#define TSOURDT3RD_EXMUSIC_RETURNTYPE(type) _Static_assert( \
		_Generic((type), \
			char             * : 1, \
			const char       * : 1, \
			consvar_t        * : 1, \
			const consvar_t  * : 1, \
			default            : 0 \
		), \
	#type": incorrect type.")
	#define TSoURDt3rd_EXMusic_ReturnType(type, valid) { \
		do { \
			TSOURDT3RD_EXMUSIC_RETURNTYPE(type); \
			valid = _Generic((type), \
				char             * : TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier, \
				const char       * : TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier, \
				consvar_t        * : TSoURDt3rd_EXMusic_ReturnTypeFromCVar, \
				const consvar_t  * : TSoURDt3rd_EXMusic_ReturnTypeFromCVar \
			)(type); \
		} while(0); \
	}

boolean TSoURDt3rd_EXMusic_DoesDefHaveValidLump(tsourdt3rd_exmusic_t *def, consvar_t *cvar, boolean soundtest_valid);

boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr);
void TSoURDt3rd_S_EXMusic_Update(void);

void TSoURDt3rd_EXMusic_DefaultMapTrack_Play(const char **mname, lumpnum_t *mlumpnum);
