
#ifndef __STAR_PSAVEG__
#define __STAR_PSAVEG__

#include "star_vars.h"
 
UINT8 TSOURDT3RD_READUINT8(UINT8 *save_p, TSoURDt3rd_t *TSoURDt3rd, UINT8 fallback);
UINT32 TSOURDT3RD_READUINT32(UINT8 *save_p, TSoURDt3rd_t *TSoURDt3rd, UINT32 fallback);

void TSoURDt3rd_NetArchiveUsers(UINT8 *save_p, INT32 playernum);
void TSoURDt3rd_NetUnArchiveUsers(UINT8 *save_p, INT32 playernum);

#endif // __STAR_PSAVEG__