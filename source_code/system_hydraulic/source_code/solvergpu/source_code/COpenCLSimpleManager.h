/*
 * Copyright (C) 2023 Alaa Mroue
 * This code is licensed under GPLv3. See LICENCE for more information.
 */

#pragma once

#include "common.h"

class COpenCLSimpleManager{

public:

	COpenCLSimpleManager(void);								// Constructor
	~COpenCLSimpleManager(void);							// Destructor

	// Public Functions
	bool					getOpenCLDevicesAvailable(void) { return openCLDevicesAvailable; };
	bool					getOpenCLRuntimeAvailable(void) { return openCLRuntimeAvailable; };
	std::vector<std::string> getDevicesStringList(void) { return sDevicesStringList; };

private:

	// Private Variables
	bool openCLDevicesAvailable;
	bool openCLRuntimeAvailable;
	std::vector<std::string> sDevicesStringList;


};