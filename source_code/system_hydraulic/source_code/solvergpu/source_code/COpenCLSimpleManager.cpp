/*
 * Copyright (C) 2023 Alaa Mroue
 * This code is licensed under GPLv3. See LICENCE for more information.
 */

#include "COpenCLSimpleManager.h"
#include "COCLDevice.h"
#include "CExecutorControlOpenCL.h"

#include <vector>

 //Constructor
COpenCLSimpleManager::COpenCLSimpleManager(void)
{

	this->openCLDevicesAvailable = false;
	this->openCLRuntimeAvailable = false;

	#ifdef _WIN32
	// TODO: Alaa what is the Linux equivalent.
	// Attempt to dynamically load the OpenCL library
	HMODULE libHandle = LoadLibrary("OpenCL.dll");

	// If Library not Found
	if (!libHandle) {

		return;
	}

	FreeLibrary(libHandle);
	#endif
	this->openCLRuntimeAvailable = true;

	CLog* oldLogger = nullptr;
	if (model::log != nullptr) {
		CLog* oldLogger = model::log;
	}

	CLog* logger = new CLog(nullptr, false);
	model::log = logger;

	CExecutorControlOpenCL* cc = new CExecutorControlOpenCL(model::filters::devices::devicesGPU);
	if (!cc->getOpenCLAvailable()) {
		delete cc;
		delete logger;
		return;
	}
	cc->createDevices();
	if (!cc->getOpenCLAvailable()) {
		delete cc;
		delete logger;
		return;
	}

	for (int i = 0; i < cc->getDeviceCount(); i++)
	{
		this->sDevicesStringList.push_back(cc->getDevice(i + 1)->getDeviceRealName());
	}

	this->openCLDevicesAvailable = true;

	delete cc;
	delete logger;

	if (oldLogger != nullptr) {
		model::log = oldLogger;
	}

}


//Destructor
COpenCLSimpleManager::~COpenCLSimpleManager(void)
{

}
