/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

// Includes
#include <math.h>
#include <cmath>
#include <thread>

#include "common.h"
#include "COCLDevice.h"
#include "COCLProgram.h"
#include "COCLBuffer.h"
#include "COCLKernel.h"

//Constructor
COCLKernel::COCLKernel(
		COCLProgram*		program,
		std::string			sKernelName
	)
{
	this->program			= program;
	this->sName				= sKernelName;
	this->bReady			= false;
	this->bGroupSizeForced	= false;
	this->clProgram			= program->clProgram;
	this->clKernel			= NULL;
	this->pDevice			= program->getDevice();
	this->uiDeviceID		= program->getDevice()->uiDeviceNo;
	this->clQueue			= program->getDevice()->clQueue;
	this->fCallback			= COCLDevice::defaultCallback;
	this->szGlobalSize[0] = 1;	 this->szGlobalSize[1] = 1;	  this->szGlobalSize[2] = 1;
	this->szGroupSize[0] = 1;	 this->szGroupSize[1] = 1;	  this->szGroupSize[2] = 1;
	this->szGlobalOffset[0] = 0; this->szGlobalOffset[1] = 0; this->szGlobalOffset[2] = 0;
	this->callBackData		= program->getDevice()->callBackData;

	this->prepareKernel();
}

//Destructor
COCLKernel::~COCLKernel()
{
	if ( this->clKernel != NULL )
		clReleaseKernel( clKernel );

	delete [] arguments;
}

//Schedule the kernel for execution on the device
void COCLKernel::scheduleExecution()
{
	if ( !this->bReady ) return;

	cl_event		clEvent		= NULL;
	cl_int			iErrorID	= CL_SUCCESS;

	pDevice->markBusy();
	
	iErrorID = clEnqueueNDRangeKernel(
		this->clQueue,
		clKernel,
		3,
		szGlobalOffset,
		szGlobalSize,
		szGroupSize,
		NULL,
		NULL,
		( fCallback != NULL && fCallback != COCLDevice::defaultCallback ? &clEvent : NULL )
	);

	if ( iErrorID != CL_SUCCESS )
	{
		// The model cannot continue in this case
		model::doError(
			"Kernel queue failed for device #" + toStringExact( this->uiDeviceID ) + ". Got error " + Util::get_error_str(iErrorID) + "." 
			+ " When running the kernel: " + sName,
			model::errorCodes::kLevelModelStop,
			"void COCLKernel::scheduleExecution()",
			"Try to restart the program or PC."
		);
		return;
	}

	if ( fCallback != NULL && fCallback != COCLDevice::defaultCallback )
	{
		iErrorID = clSetEventCallback(
			clEvent,
			CL_COMPLETE,
			fCallback,
			&this->callBackData
		);

		if ( iErrorID != CL_SUCCESS )
		{
			// The model cannot continue in this case, odd though it is...
			// Shouldn't ever really happen, but you never know
			model::doError(
				"Attaching thread callback failed for device #" + toStringExact( this->uiDeviceID ) + ". Got error [" + Util::get_error_str(iErrorID) + "]",
				model::errorCodes::kLevelModelStop,
				"void COCLKernel::scheduleExecution()",
				"Try to restart the program or PC."
			);
			return;
		}
	}
}

//Schedule the kernel for execution on the device
void COCLKernel::scheduleExecutionAndFlush()
{
	cl_int			iErrorID;

	if ( !this->bReady ) return;

	scheduleExecution();

	iErrorID = clFlush(
		this->clQueue
	);

	if ( iErrorID != CL_SUCCESS )
	{
		// The model cannot continue in this case, odd though it is...
		// Shouldn't ever really happen, but you never know
		model::doError(
			"Failed flushing commands to device #" + toStringExact( this->uiDeviceID ) + ". Got error [" + Util::get_error_str(iErrorID) + "]",
			model::errorCodes::kLevelModelStop,
			"void COCLKernel::scheduleExecutionAndFlush()",
			"Try to restart the program or PC."
		);
		return;
	}
}

//Assign all of the arguments, pass NULL if required
bool COCLKernel::assignArguments(
		COCLBuffer*	aBuffers[]
	)
{
	if ( this->clKernel == NULL ) return false;

	model::log->logInfo("Assigning arguments for '" + this->sName + "':");

	for (unsigned char i = 0; i < this->uiArgumentCount; i++)
	{
		if (aBuffers[i] == NULL)
		{
			model::log->logInfo(" " + toStringExact(i + 1) + ". NULL");
		}
		else if (this->assignArgument(i, aBuffers[i]) == false)
		{
			model::doError(
				"Failed to assign a kernel argument for '" + this->sName + "'.",
				model::errorCodes::kLevelModelStop,
				"bool COCLKernel::assignArguments( COCLBuffer*	aBuffers[] )",
				"Try to restart the program or PC."
			);
			return false;
		}
		else {
			model::log->logInfo(" " + toStringExact(i + 1) + ". " + aBuffers[i]->getName());
		}
	}

	this->bReady = true;

	return true;
}

//Assign a single argument
bool COCLKernel::assignArgument(
	unsigned char	ucArgumentIndex,
	COCLBuffer* aBuffer
)
{
	cl_int	iErrorID;
	cl_mem	clBuffer = aBuffer->getBuffer();

	iErrorID = clSetKernelArg(
		clKernel,
		ucArgumentIndex,
		sizeof(&clBuffer),
		&clBuffer
	);

	if (iErrorID != CL_SUCCESS) {
		model::doError(
			"Assignment of arguments to buffer was not successful. Got an error: [" 
				+ Util::get_error_str(iErrorID) + "] returned from clSetKernelArg with buffer: [" 
				+ aBuffer->getName() + "] on argument [" + std::to_string(ucArgumentIndex ) + "].",
			model::errorCodes::kLevelFatal,
			"bool COCLKernel::assignArgument",
			"If you're a developer, check the code. If you're a user, try a restart."
		);
		return false;
	}

	return true;
}

//Prepare the kernel by finding it in the program etc.
void COCLKernel::prepareKernel()
{
	cl_int		iErrorID;
	size_t		szRequiredWGSize[3];

	clKernel = clCreateKernel(
		clProgram,
		sName.c_str(),
		&iErrorID
	);

	if (iErrorID != CL_SUCCESS)
	{
		model::doError(
			"Could not prepare the kernel to run on device #" + toStringExact(this->program->device->uiDeviceNo) + 
				". Got Error code: [" + Util::get_error_str(iErrorID) + "] from clCreateKernel",
			model::errorCodes::kLevelModelStop,
			"void COCLKernel::prepareKernel()",
			"Try restarting the program or PC"
		);
		return;
	}

	model::log->logInfo("Kernel '" + sName + "' prepared for device #" + toStringExact(this->program->device->uiDeviceNo) + ".");

	// Fetch the kernel details on workgroup sizes etc.
	iErrorID = clGetKernelInfo(
		clKernel,
		CL_KERNEL_NUM_ARGS,
		sizeof(cl_uint),
		&this->uiArgumentCount,
		NULL
	);

	if (iErrorID != CL_SUCCESS)
	{
		model::doError(
			"Could not identify argument count for '" + sName + "' kernel." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clGetKernelInfo",
			model::errorCodes::kLevelModelStop,
			"void COCLKernel::prepareKernel()",
			"Try restarting the program or PC"
		);
		return;
	}

	iErrorID = clGetKernelWorkGroupInfo(
		clKernel,
		this->program->device->clDevice,
		CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
		sizeof(size_t) * 3,
		&szRequiredWGSize,
		NULL
	);

	if (iErrorID != CL_SUCCESS)
	{
		model::doError(
			"Could not identify work-group constraints for '" + sName + "' kernel." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clGetKernelWorkGroupInfo",
			model::errorCodes::kLevelWarning,
			"void COCLKernel::prepareKernel()",
			"Try restarting the program or PC"

		);
		szGroupSize[0] = 1;	szGroupSize[1] = 1;	szGroupSize[2] = 1;
	}
	else {
		// Dimensions of (0,0,0) mean the size was not stipulated...
		if (szRequiredWGSize[0] != 0 || szRequiredWGSize[1] != 0 || szRequiredWGSize[2] != 0)
		{
			bGroupSizeForced = true;
			szGroupSize[0] = szRequiredWGSize[0];	szGroupSize[1] = szRequiredWGSize[1];	szGroupSize[2] = szRequiredWGSize[2];
		}
	}

	iErrorID = clGetKernelWorkGroupInfo(
		clKernel,
		this->program->device->clDevice,
		CL_KERNEL_PRIVATE_MEM_SIZE,
		sizeof(cl_ulong),
		&this->ulMemPrivate,
		NULL
	);

	if (iErrorID != CL_SUCCESS)
	{
		model::doError(
			"Could not identify private mem usage for '" + sName + "' kernel." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clGetKernelWorkGroupInfo",
			model::errorCodes::kLevelWarning,
			"void COCLKernel::prepareKernel()",
			"Try restarting the program or PC"
		);
		this->ulMemPrivate = 0;
	}

	iErrorID = clGetKernelWorkGroupInfo(
		clKernel,
		this->program->device->clDevice,
		CL_KERNEL_LOCAL_MEM_SIZE,
		sizeof(cl_ulong),
		&this->ulMemLocal,
		NULL
	);

	if (iErrorID != CL_SUCCESS)
	{
		model::doError(
			"Could not identify local mem usage for '" + sName + "' kernel." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clGetKernelWorkGroupInfo",
			model::errorCodes::kLevelWarning,
			"void COCLKernel::prepareKernel()",
			"Try restarting the program or PC"
		);
		this->ulMemLocal = 0;
	}

	this->arguments = new COCLBuffer * [this->uiArgumentCount];

	model::log->logInfo("Kernel '" + sName + "' is defined:");
	model::log->logInfo("  Private memory:   " + std::to_string(this->ulMemPrivate) + " bytes");
	model::log->logInfo("  Local memory:     " + std::to_string(this->ulMemLocal) + " bytes");
	model::log->logInfo("  Arguments:        " + std::to_string(this->uiArgumentCount));
	model::log->logInfo("  Work-group size:  [ " + std::to_string(szRequiredWGSize[0]) + "," +
		std::to_string(szRequiredWGSize[1]) + "," +
		std::to_string(szRequiredWGSize[2]) + " ]");

	if (this->uiArgumentCount == 0)
		this->bReady = true;
}

//Set the global size of the work
void COCLKernel::setGlobalSize(
	cl_ulong	X,
	cl_ulong	Y,
	cl_ulong	Z
)
{
	X = static_cast<size_t>(ceil(static_cast<double>(X) / this->szGroupSize[0]) * this->szGroupSize[0]);
	Y = static_cast<size_t>(ceil(static_cast<double>(Y) / this->szGroupSize[1]) * this->szGroupSize[1]);
	Z = static_cast<size_t>(ceil(static_cast<double>(Z) / this->szGroupSize[2]) * this->szGroupSize[2]);

	this->szGlobalSize[0] = static_cast<size_t>(X);
	this->szGlobalSize[1] = static_cast<size_t>(Y);
	this->szGlobalSize[2] = static_cast<size_t>(Z);

	model::log->logInfo(
		"Global work size for '" + this->sName + "' set to [" +
		std::to_string(this->szGlobalSize[0]) + "," +
		std::to_string(this->szGlobalSize[1]) + "," +
		std::to_string(this->szGlobalSize[2]) + "]."
	);
}

//Set the global offset of the work
void COCLKernel::setGlobalOffset(
	cl_ulong	X,
	cl_ulong	Y,
	cl_ulong	Z
)
{
	this->szGlobalOffset[0] = static_cast<size_t>(X);
	this->szGlobalOffset[1] = static_cast<size_t>(Y);
	this->szGlobalOffset[2] = static_cast<size_t>(Z);
}

//Set the work group size
void COCLKernel::setGroupSize(
	cl_ulong	X,
	cl_ulong	Y,
	cl_ulong	Z
)
{
	if (this->bGroupSizeForced) return;

	this->szGroupSize[0] = static_cast<size_t>(X);
	this->szGroupSize[1] = static_cast<size_t>(Y);
	this->szGroupSize[2] = static_cast<size_t>(Z);

	model::log->logInfo(
		"Work-group size for '" + this->sName + "' set to [" +
		std::to_string( this->szGroupSize[0] ) + "," +
		std::to_string( this->szGroupSize[1] ) + "," +
		std::to_string( this->szGroupSize[2] ) + "]."
	);
}
