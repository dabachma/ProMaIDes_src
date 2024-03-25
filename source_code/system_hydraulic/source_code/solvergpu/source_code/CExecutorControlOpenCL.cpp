/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


// Includes
#include "common.h"
#include "CExecutorControlOpenCL.h"
#include "COCLDevice.h"

#include <vector>

//Constructor
CExecutorControlOpenCL::CExecutorControlOpenCL(unsigned int uiFilters)
{
	this->openCLAvailable = false;
	this->clDeviceTotal = 0;
	this->uiSelectedDeviceID = NULL;
	this->deviceFilter = uiFilters;

	this->setState(model::executorStates::executorError);

	if (!this->getPlatforms()) return;

	this->logPlatforms();
}

//Destructor
CExecutorControlOpenCL::~CExecutorControlOpenCL(void)
{
	delete[] this->platformInfo;

	for ( unsigned int iDeviceID = 0; iDeviceID < this->clDeviceTotal; iDeviceID++ )
	{
		if (this->pDevices.size() > 0) {
			delete this->pDevices[iDeviceID];
		}
	}

	this->pDevices.clear();
	delete [] this->clPlatforms;
}

//Ascertain the number of, and store a pointer to each device available to us.
bool CExecutorControlOpenCL::getPlatforms(void)
{
	if (this->clDeviceTotal > 0) {
		model::doError(
			"An attempt to obtain OpenCL platforms for a second time is invalid.",
			model::errorCodes::kLevelFatal,
			"CExecutorControlOpenCL::getPlatforms(void)",
			"Please try to exit program and run again."
			);
		return false;
	}

	cl_int			iErrorID;
	unsigned int	iPlatformID;

	iErrorID = clGetPlatformIDs( NULL, NULL, &this->clPlatformCount );

	if ( iErrorID != CL_SUCCESS ) 
	{
		model::doError(
			"Error obtaining the number of CL platforms. Got an error [" + Util::get_error_str(iErrorID) + "] ",
			model::errorCodes::kLevelFatal,
			"clGetPlatformIDs( NULL, NULL, &this->clPlatformCount )",
			"Your system might not be supported yet"
		);
		return false;
	}

	this->clPlatforms		  = new cl_platform_id[ this->clPlatformCount ];
	this->platformInfo		  = new sPlatformInfo[ this->clPlatformCount ];

	iErrorID = clGetPlatformIDs( this->clPlatformCount, this->clPlatforms, &this->clPlatformCount );

	if ( iErrorID != CL_SUCCESS ) 
	{
		model::doError( 
			"Error obtaining the CL platforms. Got an error [" + Util::get_error_str(iErrorID) + "] ",
			model::errorCodes::kLevelFatal,
			"clGetPlatformIDs( this->clPlatformCount, this->clPlatforms, &this->clPlatformCount )",
			"Your system might not be supported yet"
		);
		return false;
	}

	for ( iPlatformID = 0; iPlatformID < this->clPlatformCount; iPlatformID++ )
	{
		this->getPlatformInfo(iPlatformID, &this->platformInfo[iPlatformID].cProfile,		CL_PLATFORM_PROFILE);
		this->getPlatformInfo(iPlatformID, &this->platformInfo[iPlatformID].cVersion,		CL_PLATFORM_VERSION);
		this->getPlatformInfo(iPlatformID, &this->platformInfo[iPlatformID].cName,			CL_PLATFORM_NAME);
		this->getPlatformInfo(iPlatformID, &this->platformInfo[iPlatformID].cVendor,		CL_PLATFORM_VENDOR);
		this->getPlatformInfo(iPlatformID, &this->platformInfo[iPlatformID].cExtensions,	CL_PLATFORM_EXTENSIONS );
	}

	for ( iPlatformID = 0; iPlatformID < this->clPlatformCount; iPlatformID++ )
	{
		iErrorID = clGetDeviceIDs( this->clPlatforms[ iPlatformID ], CL_DEVICE_TYPE_ALL, 0, NULL, &this->platformInfo[ iPlatformID ].uiDeviceCount );

		if ( iErrorID != CL_SUCCESS ) 
		{
			model::doError( 
				"Error obtaining the number of devices on each CL platform. Got an error [" + Util::get_error_str(iErrorID) + "] ",
				model::errorCodes::kLevelFatal,
				"clGetDeviceIDs( this->clPlatforms[ iPlatformID ], CL_DEVICE_TYPE_ALL, 0, NULL, &this->platformInfo[ iPlatformID ].uiDeviceCount )",
				"Your system might not be supported yet"
			);
			return false;
		}

		this->clDeviceTotal += this->platformInfo[ iPlatformID ].uiDeviceCount;
	}

	this->openCLAvailable = true;
	return true;
}

//Sends the details about the platforms to the log.
void CExecutorControlOpenCL::logPlatforms(void)
{
	std::string	sPlatformNo;

	model::log->writeDivide();
	model::log->logInfo("OPENCL PLATFORMS");

	for (unsigned int iPlatformID = 0; iPlatformID < this->clPlatformCount; iPlatformID++)
	{
		sPlatformNo = "  " + toStringExact(iPlatformID + 1) + ". ";

		model::log->logInfo(
			sPlatformNo + this->platformInfo[iPlatformID].cName);
		model::log->logInfo(
			std::string(sPlatformNo.size(), ' ') + std::string(this->platformInfo[iPlatformID].cVersion) + " with " + toStringExact(this->platformInfo[iPlatformID].uiDeviceCount) + " device(s)");
	}

	model::log->writeDivide();
}

//Create a new instance of the device class for each device we can identify on the platforms.
bool CExecutorControlOpenCL::createDevices(void)
{
	cl_int				iErrorID;
	cl_device_id*		clDevice;
	unsigned int		uiDeviceCount = 0;

	this->openCLAvailable = false;

	COCLDevice* pDevice;

	for (unsigned int iPlatformID = 0; iPlatformID < this->clPlatformCount; iPlatformID++){

		clDevice = new cl_device_id[this->platformInfo[iPlatformID].uiDeviceCount];

		iErrorID = clGetDeviceIDs(
			this->clPlatforms[iPlatformID],
			CL_DEVICE_TYPE_ALL,
			this->platformInfo[iPlatformID].uiDeviceCount,
			clDevice,
			NULL
		);

		if (iErrorID != CL_SUCCESS)
		{
			delete[] clDevice;
			model::doError(
				"Error obtaining the devices for CL platform '" + this->platformInfo[iPlatformID].cName + "'. Got error: ["+
				Util::get_error_str(iErrorID) + "]."
				,
				model::errorCodes::kLevelFatal,
				"clGetDeviceIDs( this->clPlatforms[iPlatformID], CL_DEVICE_TYPE_ALL, this->platformInfo[iPlatformID].uiDeviceCount, clDevice, NULL );",
				"Your device might not be supported yet. Try to updating graphics drivers."
			);
			return false;
		}

		for (unsigned int iDeviceID = 0; iDeviceID < this->platformInfo[iPlatformID].uiDeviceCount; iDeviceID++)
		{
			pDevice = new COCLDevice(
				clDevice[iDeviceID],
				iPlatformID,
				uiDeviceCount,
				this
			);

			if (pDevice->isReady())
			{
				// Complies with the device filters?
				if (
					pDevice->clDeviceType == CL_DEVICE_TYPE_CPU && ((this->deviceFilter & model::filters::devices::devicesCPU) == model::filters::devices::devicesCPU) ||
					pDevice->clDeviceType == CL_DEVICE_TYPE_GPU && ((this->deviceFilter & model::filters::devices::devicesGPU) == model::filters::devices::devicesGPU) ||
					pDevice->clDeviceType == CL_DEVICE_TYPE_ACCELERATOR && ((this->deviceFilter & model::filters::devices::devicesAPU) == model::filters::devices::devicesAPU)
					)
				{
					pDevices.push_back(pDevice);
					uiDeviceCount++;
						pDevice->logDevice();
				}
				else {
					model::log->logInfo("Device type is filtered.");
					delete pDevice;
				}
			}
			else {
				model::log->logInfo("Device is not ready.");
				delete pDevice;
			}
		}

		delete[] clDevice;
	}

	this->clDeviceTotal = uiDeviceCount;

	model::log->logInfo("The OpenCL executor is now fully loaded.");
	this->setState(model::executorStates::executorReady);
	this->openCLAvailable = true;
	return true;
}

//Obtain the size and value for a platform info field
void CExecutorControlOpenCL::getPlatformInfo(unsigned int uiPlatformID, std::string* sValue, cl_platform_info clInfo)
{
	cl_int		iErrorID;
	size_t		clSize;

	iErrorID = clGetPlatformInfo(
		this->clPlatforms[uiPlatformID],
		clInfo,
		NULL,
		NULL,
		&clSize
	);

	if (iErrorID != CL_SUCCESS) {
		model::doError("clGetPlatformInfo failed to get size of info result. Got error: " + Util::get_error_str(iErrorID),
			model::errorCodes::kLevelFatal,
			"char* CExecutorControlOpenCL::getPlatformInfo(unsigned int uiPlatformID, cl_platform_info clInfo)",
			"Your device might not be supported"
		);
		return;
	}

	char* cValue = new char[clSize + 1];

	iErrorID = clGetPlatformInfo(
		this->clPlatforms[uiPlatformID],
		clInfo,
		clSize,
		cValue,
		NULL
	);

	if (iErrorID != CL_SUCCESS) {
		delete[] cValue;
		model::doError("clGetPlatformInfo got size successfully but couldn't get the data. Got error: " + Util::get_error_str(iErrorID),
			model::errorCodes::kLevelFatal,
			"char* CExecutorControlOpenCL::getPlatformInfo(unsigned int uiPlatformID, cl_platform_info clInfo)",
			"Your device might not be supported"
		);
		return;
	}

	*sValue = cValue;
	delete[] cValue;
}

//Get the code for a specific type of benchmark, to be compiled to a kernel.
OCL_RAW_CODE CExecutorControlOpenCL::getOCLCode(std::string sFilename)
{
	return Util::getFileResource(sFilename.c_str(), "OpenCLCode");
}

//Return the currently selected device.
COCLDevice* CExecutorControlOpenCL::getDevice()
{
	// Verify that a device has been selected, and do so
	// if otherwise.
	if (this->uiSelectedDeviceID == NULL)
		this->selectDevice();

	return this->getDevice(
		this->uiSelectedDeviceID
	);
}

//Fetch a device object, required because of some of the static member callbacks.
COCLDevice* CExecutorControlOpenCL::getDevice(unsigned int uiDeviceNo)
{
	if (uiDeviceNo > getDeviceCount())
		return NULL;
	if (uiDeviceNo < 1)
		return NULL;
	return static_cast<COCLDevice*>(this->pDevices[uiDeviceNo - 1]);
}

//Automatically select the best device for the model to use in this instance.
void	CExecutorControlOpenCL::selectDevice()
{
	// Check the device isn't filtered from use, and is seemingly
	// ready (i.e. not already consumed by another process).
	// ...

	// Check the memory requirements of the domain, for the data
	// structure of each model, for suitability.
	// ...

	// Check double precision availability.
	// ...

	// Check the number of compute units available.
	// ...

	// TODO: Implement some system of selecting the best device
	//		 Until then, just pick the first one.
	if (this->pDevices.size() < 1)
	{
		model::doError(
			"No suitable devices could be found for running this model.",
			model::errorCodes::kLevelModelStop,
			"void	CExecutorControlOpenCL::selectDevice()",
			"Please switch to a cpu simulation."
		);
		return;
	}

	// Select the device
	this->selectDevice(1);
}

//Select the device specified by the parameter, i.e. force the model to use a particular device as long as it is suitable.Raise a fatal error otherwise.
void	CExecutorControlOpenCL::selectDevice(unsigned int uiDeviceNo)
{
	if (uiDeviceNo > this->pDevices.size())
	{
		model::doError(
			"An invalid device was selected for execution.",
			model::errorCodes::kLevelFatal,
			"void	CExecutorControlOpenCL::selectDevice(unsigned int uiDeviceNo)",
			"Please switch to a cpu simulation."
		);
		return;
	}
	else {
		model::log->logInfo(
			"Selected device: #" + toStringExact( uiDeviceNo )
		);
	}

	this->uiSelectedDeviceID = uiDeviceNo;
}


//Is this executor ready to run models?
bool CExecutorControlOpenCL::isReady(void)
{
	return this->state == model::executorStates::executorReady;
}

//Set the ready state of this executor
void CExecutorControlOpenCL::setState(unsigned int iState)
{
	this->state = iState;
}

//Return any current device filters
unsigned int CExecutorControlOpenCL::getDeviceFilter()
{
	return this->deviceFilter;
}