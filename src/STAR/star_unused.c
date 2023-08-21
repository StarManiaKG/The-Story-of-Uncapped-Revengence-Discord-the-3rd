// SONIC ROBO BLAST 2 - TSOURDT3RD EDITION
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by Star "Guy Who Named Another Script After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_functions.c
/// \brief Contains all the Info Portraying to, at Least at This Point in Time,
///        TSoURDt3rd's Unused Stuff

// NUMBERS //
//
// INT32 **STAR_SplitNumber(INT32 NUMBER, boolean useINT16Format)
// Splits Numbers Into Smaller Numbers, Depending on the Format Specified
//
// Example of a Possible Return:
//	NUMBER == 280, useINT16Format = false		=	Returned Number = 28, 0
//	NUMBER == 20000000, useINT16Format = true	=	Returned Number = 20, 00, 00, 00
//
INT32 **finalSplitNumbers;

INT32 **STAR_SplitNumber(INT32 NUMBER, boolean useINT16Format)
{
	// Make Variables //
	INT32 i = 0, j = 0, c = 0;
	INT32 formatToUse = (useINT16Format ? 4 : 1);
	char numberString[256] = ""; strcpy(numberString, STAR_ConvertNumberToString(NUMBER, 0, 0, false));
	char convertedNumberString[256] = "";

	// Erase The Table if Something's There //
	while (finalSplitNumbers[c][i] != '\0')
	{
		if ((i >= formatToUse) || (finalSplitNumbers[c][i+1] == '\0'))
		{
			c++;
			i = 0;
		}

		finalSplitNumbers[c][i] = '\0';
		i++;
	}
	c = 0; i = 0;

	// Apply Our Strings to Our Numbers //
	while (numberString[j] != '\0')
	{
		convertedNumberString[j] = numberString[j];
		finalSplitNumbers[c][i] = atoi(&convertedNumberString[j]);

		if ((i >= formatToUse) || (numberString[j+1] == '\0'))
		{
			if (numberString[j+1] == '\0')
			{
				finalSplitNumbers[c][i+1] = '\0';
				break;
			}

			i = 0;
			c++; j++;
		}
		else
		{
			i++;
			j++;
		}
	}

	// Return Our Split Numbers (Which are Stored in a Table), and We're Done! //
	return finalSplitNumbers;
}
