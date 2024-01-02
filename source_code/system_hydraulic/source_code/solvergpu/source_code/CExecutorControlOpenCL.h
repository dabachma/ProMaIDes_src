/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_OPENCL_EXECUTORS_CEXECUTORCONTROLOPENCL_H_
#define HIPIMS_OPENCL_EXECUTORS_CEXECUTORCONTROLOPENCL_H_

#include <vector>
#include "CL/opencl.h"

// Type aliases
typedef char*						OCL_RAW_CODE;
typedef std::vector<char*>			OCL_CODE_STACK;

class COCLDevice;

/*
 *  [OPENCL IMPLEMENTATION]
 *  EXECUTOR CONTROL CLASS
 *  CExecutorControl
 *
 *  Controls the model execution
 */
class CExecutorControlOpenCL{

	public:

		CExecutorControlOpenCL(unsigned int);
		~CExecutorControlOpenCL( void );							// Destructor

		// Public variables
		// ...

		// Friendships
		friend class			COCLDevice;													// Allow the devices access to these private vars

		// Public functions
		bool					isReady(void);												// Is the executor ready?
		COCLDevice*				getDevice();												// Fetch the currently selected device
		COCLDevice*				getDevice( unsigned int );									// Fetch a device pointer
		void					selectDevice();												// Automatically select the best device
		void					selectDevice( unsigned int );								// Manually select the device to use for execution
		OCL_RAW_CODE			getOCLCode( std::string );									// Fetch OpenCL code stored in our resources
		bool					createDevices( void );										// Creates new classes for each device
		unsigned int			getDeviceCount( void )		{ return clDeviceTotal; }		// Returns the number of devices in the system
		unsigned int			getDeviceCurrent( void )	{ return uiSelectedDeviceID; }	// Returns the active device
		void					setState( unsigned int );									// Set the ready state
		unsigned int			getDeviceFilter();											// Fetch back the current device filter
		bool					getOpenCLAvailable(void) { return openCLAvailable; }	// Returns the active device
		

	private:

		// Private structs
		struct		sPlatformInfo
		{
			std::string			cProfile;
			std::string			cVersion;
			std::string			cDeviceName;
			std::string			cName;
			std::string			cVendor;
			std::string			cExtensions;
			cl_uint				uiDeviceCount;
		};

		// Private variables
		sPlatformInfo*			platformInfo;				// Platform details
		cl_platform_id *  		clPlatforms;				// Array of OpenCL platforms
		cl_uint					clPlatformCount;			// Number of platforms
		cl_uint					clDeviceTotal;				// Total number of devices
		std::vector<COCLDevice*> pDevices;					// Dynamic array of device controller classes 		
		unsigned int			uiSelectedDeviceID;			// The selected device for use in execution


		unsigned int			deviceFilter;				// Device filter active for this executor
		unsigned int			state;						// Ready state value
		bool					openCLAvailable;			// Flag for opencl available on the system
		bool					quietLog;					// Flag for logging (On/Off)

		// Private functions
		void					getPlatformInfo( unsigned int, std::string*, cl_platform_info );		// Fetches information about the platform and sets it to a string
		bool					getPlatforms( void );													// Discovers the platforms available
		void					logPlatforms( void );													// Write platform details to the log
};

#endif
