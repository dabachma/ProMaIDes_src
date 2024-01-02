/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


#include "common.h"

#ifdef PLATFORM_WIN

char*	Util::getFileResource( const char * sName, const char * sType )
{
	HMODULE hModule = GetModuleHandle( NULL );


	if ( hModule == NULL )
		model::doError(
			"The module handle could not be obtained.",
			model::errorCodes::kLevelFatal,
			"char*	Util::getFileResource( const char * sName, const char * sType )",
			"HMODULE hModule = GetModuleHandle( NULL ) returned null"
		);

	HRSRC hResource = FindResource( hModule, static_cast<LPCSTR>(sName), static_cast<LPCSTR>(sType) );

	if ( hModule == NULL )
	{
		model::doError(
			"Could not obtain a requested resource.",
			model::errorCodes::kLevelWarning,
			"char*	Util::getFileResource( const char * sName, const char * sType )",
			"FindResource couldn't find resource [" + std::string(sName) + "] with type [" + std::string(sType) + "]"
		);
		return "";
	}

	HGLOBAL hData = LoadResource( hModule, hResource );

	if ( hData == NULL )
	{
		model::doError(
			"Could not load a requested resource value.",
			model::errorCodes::kLevelWarning,
			"char*	Util::getFileResource( const char * sName, const char * sType )",
			"LoadResource: Found the resource but couldn't Load it. Name: [" + std::string(sName) + "] .Type: [" + std::string(sType) + "]"
		);
		return "";
	}

	DWORD	dwSize		= SizeofResource( hModule, hResource );
	char*	cResource	= new char[ dwSize + 1 ];

	LPVOID	lpData		= LockResource( hData );
	char*	cSource		= static_cast<char *>(lpData);

	if ( lpData == NULL )
	{
		delete[] cResource;
		model::doError(
			"Could not obtain a pointer for a requested resource.",
			model::errorCodes::kLevelWarning,
			"char*	Util::getFileResource( const char * sName, const char * sType )",
			"LoadResource: Found the resource and loaded it But couldn't lock it. Name: [" + std::string(sName) + "] .Type: [" + std::string(sType) + "]"
		);
		return "";
	}

	memcpy( cResource, cSource, dwSize );
	cResource[ dwSize ] = 0;

	return cResource;
}

#endif

#ifdef PLATFORM_UNIX

#include <limits.h>

inline std::string getOCLResourceFilename(std::string sID)
{
	const char* cID = sID.c_str();
	std::string sBaseDir = "";

	//Windows
	//char buffer[256];
	//GetModuleFileName(NULL, buffer, 256);
	//std::string executablePath(buffer);
	//size_t lastBackslash = executablePath.find_last_of("\\");
	//std::string executableDirectory = executablePath.substr(0, lastBackslash + 1);
	
	//Linux
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	std::string executableDirectory = "";

	if (len != -1) {
		buffer[len] = '\0'; // Null-terminate the string
		std::string executablePath(buffer);
		size_t lastSlash = executablePath.find_last_of("/");
		executableDirectory = executablePath.substr(0, lastSlash + 1);
	}

	sBaseDir = executableDirectory + "/opencl/";

	if (strcmp(cID, "CLBoundaries_C") == 0)
		return sBaseDir + "CLBoundaries.clc";

	if (strcmp(cID, "CLBoundaries_H") == 0)
		return sBaseDir + "CLBoundaries.clh";

	if (strcmp(cID, "CLDomainCartesian_C") == 0)
		return sBaseDir + "CLDomainCartesian.clc";

	if (strcmp(cID, "CLDomainCartesian_H") == 0)
		return sBaseDir + "CLDomainCartesian.clh";

	if (strcmp(cID, "CLDynamicTimestep_C") == 0)
		return sBaseDir + "CLDynamicTimestep.clc";

	if (strcmp(cID, "CLDynamicTimestep_H") == 0)
		return sBaseDir + "CLDynamicTimestep.clh";

	if (strcmp(cID, "CLFriction_C") == 0)
		return sBaseDir + "CLFriction.clc";

	if (strcmp(cID, "CLFriction_H") == 0)
		return sBaseDir + "CLFriction.clh";

	if (strcmp(cID, "CLSchemeGodunov_C") == 0)
		return sBaseDir + "CLSchemeGodunov.clc";

	if (strcmp(cID, "CLSchemeGodunov_H") == 0)
		return sBaseDir + "CLSchemeGodunov.clh";

	if (strcmp(cID, "CLSchemeInertial_C") == 0)
		return sBaseDir + "CLSchemeInertial.clc";

	if (strcmp(cID, "CLSchemeInertial_H") == 0)
		return sBaseDir + "CLSchemeInertial.clh";

	if (strcmp(cID, "CLSchemeMUSCLHancock_C") == 0)
		return sBaseDir + "CLSchemeMUSCLHancock.clc";

	if (strcmp(cID, "CLSchemeMUSCLHancock_H") == 0)
		return sBaseDir + "CLSchemeMUSCLHancock.clh";

	if (strcmp(cID, "CLSchemePromaides_C") == 0)
		return sBaseDir + "CLSchemePromaides.clc";

	if (strcmp(cID, "CLSchemePromaides_H") == 0)
		return sBaseDir + "CLSchemePromaides.clh";

	if (strcmp(cID, "CLSlopeLimiterMINMOD_C") == 0)
		return sBaseDir + "CLSlopeLimiterMINMOD.clc";

	if (strcmp(cID, "CLSlopeLimiterMINMOD_H") == 0)
		return sBaseDir + "CLSlopeLimiterMINMOD.clh";

	if (strcmp(cID, "CLSolverHLLC_C") == 0)
		return sBaseDir + "CLSolverHLLC.clc";

	if (strcmp(cID, "CLSolverHLLC_H") == 0)
		return sBaseDir + "CLSolverHLLC.clh";

	if (strcmp(cID, "CLUniversalHeader_H") == 0)
		return sBaseDir + "CLUniversalHeader.clh";

	return "";
}

//Fetch an embedded resource
char* Util::getFileResource(const char* sName, const char* sType)
{

	std::string sFilename = getOCLResourceFilename(sName);

	if (sFilename.length() <= 0)
	{

		model::doError(
			"Requested an invalid resource.",
			model::errorCodes::kLevelWarning,
			"char* Util::getFileResource(const char* sName, const char* sType)",
			"Couldn't find the requested resource that matches this name. Name: [" + std::string(sName) + "] .Type: [" + std::string(sType) + "]"
		);
		return "";

	}

	try {

		std::ifstream pResourceFile(sFilename);
		std::stringstream ssResource;
		ssResource << pResourceFile.rdbuf();
		std::string sResource = ssResource.str();

		char* cResource = new char[sResource.length() + 1];
		memcpy(cResource, sResource.c_str(), sResource.length());
		cResource[sResource.length()] = 0;

		return cResource;

	}
	catch (std::exception)
	{
		model::doError(
			"Error loading a resource.",
			model::errorCodes::kLevelWarning,
			"char* Util::getFileResource(const char* sName, const char* sType)",
			"Unable to find or locate resource file. Name: [" + std::string(sName) + "] .Type: [" + std::string(sType) + "]"
		);
		return "";
	}
}

#endif