/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


// Includes
#include <string>
#include <locale>
#include <math.h>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include "common.h"

namespace Util 
{
	//Convert a double with seconds to a meaningful time format
	std::string		secondsToTime( double dTime )
	{
		std::string sTime;

		// No negative times...
		if ( dTime < 0 ) dTime = 0.0;

		// Identify the different components of the time
		float		  fFractionSeconds	= static_cast<float>( std::fmod( dTime, 1 ) );
		unsigned char cSeconds			= static_cast<unsigned char>( std::floor( std::fmod( dTime, 60      ) ) );
		unsigned char cMinutes			= static_cast<unsigned char>( std::floor( std::fmod( dTime, 3600    )/60 ) );
		unsigned char cHours			= static_cast<unsigned char>( std::floor( std::fmod( dTime, 86400   )/3600 ) );
		unsigned int  uiDays			= static_cast<unsigned int>(  std::floor( dTime / 86400 ) );

		char	cTime[50]			 = "";
		char	cFractionSeconds[50] = "";

		if ( uiDays > 0 )
		{
			sTime = toStringExact( uiDays ) + " d ";
		}

		if ( dTime > 1 )
		{
			// Normal format including miliseconds when less than 10 minutes
			sprintf( cTime,			"%02d:%02d:%02d", cHours, cMinutes, cSeconds );
			//sprintf_s( cTime,			"%02d:%02d:%02d", cHours, cMinutes, cSeconds );
			sprintf( cFractionSeconds, ".%.4f",	  fFractionSeconds );
			//sprintf_s( cFractionSeconds, ".%.4f",	  fFractionSeconds );
			sTime += std::string( cTime );
			if ( fFractionSeconds > 0.0 && cMinutes < 10 && cHours < 1 && uiDays < 1 ) 
				sTime += std::string( cFractionSeconds ).substr(2);
		} else {
			sprintf( cTime,				"%01d",		cSeconds );
			//sprintf_s( cTime,				"%01d",		cSeconds );
			sprintf( cFractionSeconds,  ".%.5f",	fFractionSeconds );
			//sprintf_s( cFractionSeconds,  ".%.5f",	fFractionSeconds );
			sTime += std::string( cTime );
			sTime += std::string( cFractionSeconds ).substr(2) + "s";
		}

		return sTime;
	}

	//Round a number to set decimal places
	double	round( double dValue, unsigned int ucPlaces )
	{
		unsigned int	uiMultiplier		= (unsigned int) std::pow( 10.0, ucPlaces );
		double			dMultipliedValue	= dValue * uiMultiplier;
		double			dRemainder			= std::fmod( dMultipliedValue, 1 );

		if ( dRemainder >= 0.5 )
		{
			dMultipliedValue = std::ceil( dMultipliedValue );
		} else {
			dMultipliedValue = std::floor( dMultipliedValue );
		}

		return dMultipliedValue / uiMultiplier;
	}

	//Convert double to string (useful for extra small or extra large doubles like 1e-10)
	std::string to_string_exact(double x) {
		std::ostringstream os;
		os << std::setprecision(std::numeric_limits<double>::max_digits10) << x;
		return os.str();
	}

	//In-place byte order swap for a double-precision floating-point number.
	void SwapEnd(double& var)
	{
		char* varArray = reinterpret_cast<char*>(&var);
		for (long i = 0; i < static_cast<long>(sizeof(var) / 2); i++)
			std::swap(varArray[sizeof(var) - 1 - i], varArray[i]);
	}
	void SwapEnd(unsigned int& var)
	{
		char* varArray = reinterpret_cast<char*>(&var);
		for (long i = 0; i < static_cast<long>(sizeof(var) / 2); i++)
			std::swap(varArray[sizeof(var) - 1 - i], varArray[i]);
	}

	//Returns a string of an OpenCL error
	std::string	get_error_str(int error_id){

		switch(error_id){
			case 0: return "CL_SUCCESS";
			case -1: return "CL_DEVICE_NOT_FOUND";
			case -2: return "CL_DEVICE_NOT_AVAILABLE";
			case -3: return "CL_COMPILER_NOT_AVAILABLE";
			case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
			case -5: return "CL_OUT_OF_RESOURCES";
			case -6: return "CL_OUT_OF_HOST_MEMORY";
			case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
			case -8: return "CL_MEM_COPY_OVERLAP";
			case -9: return "CL_IMAGE_FORMAT_MISMATCH";
			case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
			case -11: return "CL_BUILD_PROGRAM_FAILURE";
			case -12: return "CL_MAP_FAILURE";
			case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
			case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
			case -15: return "CL_COMPILE_PROGRAM_FAILURE";
			case -16: return "CL_LINKER_NOT_AVAILABLE";
			case -17: return "CL_LINK_PROGRAM_FAILURE";
			case -18: return "CL_DEVICE_PARTITION_FAILED";
			case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
			case -30: return "CL_INVALID_VALUE";
			case -31: return "CL_INVALID_DEVICE_TYPE";
			case -32: return "CL_INVALID_PLATFORM";
			case -33: return "CL_INVALID_DEVICE";
			case -34: return "CL_INVALID_CONTEXT";
			case -35: return "CL_INVALID_QUEUE_PROPERTIES";
			case -36: return "CL_INVALID_COMMAND_QUEUE";
			case -37: return "CL_INVALID_HOST_PTR";
			case -38: return "CL_INVALID_MEM_OBJECT";
			case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
			case -40: return "CL_INVALID_IMAGE_SIZE";
			case -41: return "CL_INVALID_SAMPLER";
			case -42: return "CL_INVALID_BINARY";
			case -43: return "CL_INVALID_BUILD_OPTIONS";
			case -44: return "CL_INVALID_PROGRAM";
			case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
			case -46: return "CL_INVALID_KERNEL_NAME";
			case -47: return "CL_INVALID_KERNEL_DEFINITION";
			case -48: return "CL_INVALID_KERNEL";
			case -49: return "CL_INVALID_ARG_INDEX";
			case -50: return "CL_INVALID_ARG_VALUE";
			case -51: return "CL_INVALID_ARG_SIZE";
			case -52: return "CL_INVALID_KERNEL_ARGS";
			case -53: return "CL_INVALID_WORK_DIMENSION";
			case -54: return "CL_INVALID_WORK_GROUP_SIZE";
			case -55: return "CL_INVALID_WORK_ITEM_SIZE";
			case -56: return "CL_INVALID_GLOBAL_OFFSET";
			case -57: return "CL_INVALID_EVENT_WAIT_LIST";
			case -58: return "CL_INVALID_EVENT";
			case -59: return "CL_INVALID_OPERATION";
			case -60: return "CL_INVALID_GL_OBJECT";
			case -61: return "CL_INVALID_BUFFER_SIZE";
			case -62: return "CL_INVALID_MIP_LEVEL";
			case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
			case -64: return "CL_INVALID_PROPERTY";
			case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
			case -66: return "CL_INVALID_COMPILER_OPTIONS";
			case -67: return "CL_INVALID_LINKER_OPTIONS";
			case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
			case -69: return "CL_INVALID_PIPE_SIZE";
			case -70: return "CL_INVALID_DEVICE_QUEUE";
			case -71: return "CL_INVALID_SPEC_ID";
			case -72: return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
			case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
			case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
			case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
			case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
			case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
			case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
			default: return "Unknown OpenCL error [" + std::to_string(error_id) + "]";
		}
	}
}

