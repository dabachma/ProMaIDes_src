/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_UTIL_H_
#define HIPIMS_UTIL_H_

// Includes
#include <sstream>
#include "CL/opencl.h"

// Structure definitions
struct cursorCoords {
	int	sX;
	int	sY;
};

/*
 *  FUNCTIONS
 */
namespace Util
{
	std::string		secondsToTime(double);						// Get time as a string
	char*			getFileResource(const char*, const char*);	// Resource handling
	double			round(double, unsigned int);				// Round a number to set decimal places
	std::string     to_string_exact(double);					// Convert double to string (useful for extra small or extra large doubles like 1e-10)
	std::string		get_error_str(int);							// Returns a string of an OpenCL error
	void			SwapEnd(double& var);						// In-place byte order swap for a double-precision floating-point number.
	void			SwapEnd(unsigned int& var);					// In-place byte order swap for an unsigned int

}

#endif
