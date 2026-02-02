// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-curl.c
/// \brief Easy curl implementation for TSoURDt3rd

#ifdef HAVE_CURL
#include <curl/curl.h>

#include "smkg-curl.h"
#include "../ss_main.h"

#include "../../z_zone.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_Curl_FindStringWithinURL(
//	FILE       *SITE_DATA_FILE,
//	const char *SITE_STRING,
//	char       *SITE_URL,
//	char      **SITE_RETURN_STRING,
//	int        *SITE_RETURNCODE
// )
//
// Tries to find info from the given website to the given file.
// Returns the data, if found, or NULL otherwise.
//

#if 0
static size_t
site_on_read (char *s, size_t _1, size_t n, void *userdata)
{
	struct HMS_buffer *buffer;
	size_t blocks;

	(void)_1;

	buffer = userdata;

	if (n >= (size_t)( buffer->end - buffer->needle ))
	{
		/* resize to next multiple of buffer size */
		blocks = ( n / DEFAULT_BUFFER_SIZE + 1 );
		buffer->end += ( blocks * DEFAULT_BUFFER_SIZE );

		buffer->buffer = realloc(buffer->buffer, buffer->end);
	}

	memcpy(&buffer->buffer[buffer->needle], s, n);
	buffer->needle += n;

	return n;
}
#endif

#if 0
struct HMS_buffer
{
	CURL *curl;
	char *buffer;
	int   needle;
	int   end;
	char *errbuf;
};
#endif

#if 0
static char *error_buffer = NULL;

static boolean DoCurl_EasySetOpt(CURLcode cc, CURL *curl_handle, INT32 code, ...)
{
	va_list args;

	va_start(args, code);
	//cc = curl_easy_setopt(curl_handle, code, cv_masterserver_timeout.value);
	//va_arg(args, void *)
	//mobj->player = va_arg(args, player_t *);
	if (cc != CURLE_OK)
	{
		//I_OutputMsg("libcurl: %s\n", buffer->errbuf);
		return false;
	}
	va_end(args);

	return true;
}
#endif

void TSoURDt3rd_Curl_FindStringWithinURL(
	FILE       *SITE_DATA_FILE,
	const char *SITE_STRING,
	char       *SITE_URL,
	char      **SITE_RETURN_STRING,
	int        *SITE_RETURNCODE
)
{
	CURL *curl;
	CURLcode cc;

	char *found_data = NULL;
	char *current_string = NULL;
	const char *err_msg = NULL;

	// Initialize curl
#if 0
	cc = curl_global_init(CURL_GLOBAL_ALL);
	if (cc < 0)
	{
		I_OutputMsg("libcurl: curl_global_init() returned %d, reason %s\n", cc, curl_easy_strerror(cc));
		goto quit;
	}
#endif

	curl = curl_easy_init();
	if (!curl)
	{
		err_msg = "Couldn't initialize curl.";
		goto quit;
	}
	if (!SITE_DATA_FILE)
	{
		err_msg = "Invalid file given.";
		goto quit;
	}

	// Access the website and its data
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_URL, SITE_URL);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, SITE_DATA_FILE);

#if 1
	cc = curl_easy_perform(curl);
	if (cc != CURLE_OK)
	{
		err_msg = va("curl error: %s", curl_easy_strerror(cc));
		goto quit;
	}
#endif

	// Read the website's data from the written file
	if (SITE_DATA_FILE == NULL)
	{
		err_msg = "Couldn't read the given data file.";
		goto quit;
	}
	rewind(SITE_DATA_FILE);

	found_data = Z_Malloc(256, PU_STATIC, NULL);
	while (found_data != NULL)
	{
		fgets(found_data, 256, SITE_DATA_FILE);

		if (!found_data || *found_data == '\0')
			break;
		current_string = strstr(found_data, SITE_STRING);

		if (current_string != NULL && *current_string != '\0')
		{
			const INT32 strlength = strlen(current_string);
			(*SITE_RETURNCODE) = TSOURDT3RD_CURL_DATAFOUND;
			if (current_string[strlength-1] =='\n')
			{
				current_string[strlength-1] = '\0';
			}
			found_data = current_string;
			goto quit;
		}
	}

	// We failed to find the string if we made it here, so just close...
	err_msg = "String was not found within URL.";
	*SITE_RETURNCODE = TSOURDT3RD_CURL_DATANOTFOUND;
	quit:
	{
		// Close and free all the data, and we're done!
		//curl_global_cleanup();
		if (curl != NULL)
		{
			curl_easy_cleanup(curl);
		}

		if (SITE_DATA_FILE != NULL)
			fclose(SITE_DATA_FILE);

		if (err_msg != NULL)
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_Curl_FindStringWithinURL(): %s\n", err_msg);

		//return found_data;
		(*SITE_RETURN_STRING) = found_data;
		Z_Free(found_data);
	}
}

#endif // HAVE_CURL
