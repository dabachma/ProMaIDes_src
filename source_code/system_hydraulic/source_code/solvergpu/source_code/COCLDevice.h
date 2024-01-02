/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_OPENCL_EXECUTORS_COCLDEVICE_H_
#define HIPIMS_OPENCL_EXECUTORS_COCLDEVICE_H_

#include "CExecutorControlOpenCL.h"

/*
 *  [OPENCL IMPLEMENTATION]
 *  EXECUTOR DEVICE CLASS
 *  COCLDevice
 *
 *  Controls the model execution on a specific device
 */
class CExecutorControlOpenCL;
class COCLDevice
{

	public:

		COCLDevice( cl_device_id, unsigned int, unsigned int, CExecutorControlOpenCL*);				// Constructor
		~COCLDevice( void );												// Destructor

		// Public structures
		// Used in MPI implementations, and for communication with the client in the DLL compile
		struct sDeviceSummary
		{
			char			cDeviceName[100];
			char			cDeviceType[10];
			unsigned int	uiDeviceID;
			unsigned int	uiDeviceNumber;
		};

		// Public variables
		// Device property variables
		// See www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetDeviceInfo.html
		cl_uint						clDeviceAddressSize;
		cl_bool						clDeviceAvailable;
		cl_bool						clDeviceCompilerAvailable;
		cl_bool						clDeviceErrorCorrection;
		cl_device_exec_capabilities	clDeviceExecutionCapability;
		cl_ulong					clDeviceGlobalCacheSize;
		cl_device_mem_cache_type	clDeviceGlobalCacheType;
		cl_ulong					clDeviceGlobalSize;
		cl_ulong					clDeviceLocalSize;
		cl_device_local_mem_type	clDeviceLocalType;
		cl_uint						clDeviceClockFrequency;
		cl_uint						clDeviceComputeUnits;
		cl_uint						clDeviceMaxConstants;
		cl_ulong					clDeviceMaxConstantSize;
		cl_ulong					clDeviceMaxMemAlloc;
		cl_ulong					clDeviceGlobalMemSize;
		size_t						clDeviceMaxParamSize;
		size_t						clDeviceMaxWorkGroupSize;
		cl_uint						clDeviceMaxWorkItemDims;
		size_t*						clDeviceMaxWorkItemSizes;
		char*						clDeviceName;
		char*						clDeviceCVersion;
		char*						clDeviceProfile;
		size_t						clDeviceTimerResolution;
		cl_command_queue_properties	clDeviceQueueProperties;
		cl_device_fp_config			clDeviceSingleFloatConfig;
		cl_device_fp_config			clDeviceDoubleFloatConfig;
		cl_device_type				clDeviceType;
		char*						clDeviceVendor;
		char*						clDeviceOpenCLVersion;
		char*						clDeviceOpenCLDriver;			
		cl_uint						clDeviceAlignBits;	

		// Public functions
		void						markBusy()							{ bBusy = true;  }					// Set the device as busy
		cl_context					getContext( void )					{ return clContext; }				// Returns the OpenCL context for this device only
		CExecutorControlOpenCL*		getExecController( void )			{ return execController; }			// Get the owning exec controller
		unsigned int				getDeviceID( void )					{ return uiDeviceNo; }				// Fetch the device number
		cl_device_id				getDevice( void )					{ return clDevice; }				// Fetch the OpenCL device
		char*						getVendor( void )					{ return clDeviceVendor; }			// Get vendor name
		cl_device_type				getDeviceType( void )				{ return clDeviceType; }			// Get device type (bitmask)
		char*						getOCLVersion( void )				{ return clDeviceOpenCLVersion; }	// Get OpenCL version
		void						getSummary( sDeviceSummary & );											// Get device summary info
		bool						isBusy(void);															// Is the device busy?
		std::string					getDeviceShortName( void );												// Fetch a short identifier for the device
		std::string					getDeviceRealName( void );												// Return device name as a string
		void						logDevice( void );														// Write details to the log
		bool						isSuitable( void );														// Is this device suitable?
		bool						isReady( void );														// Is this device ready?	
		bool						isFiltered( void );														// Is this device filtered from use?
		bool						isDoubleCompatible( void );												// Is there sufficient double precision support?
		static void CL_CALLBACK		
									defaultCallback( cl_event, cl_int, void * );							// Default event callback to dispose of the event	
		void						queueBarrier();															// Queue a barrier to synchronise all threads
		void						blockUntilFinished();													// Pause the program until command exec completes
		void						flushAndSetMarker();													// Set the kernel we should use to monitor completion
		void						flush();
		void						markerCompletion();														// Handle once the marker callback has been triggered (non-static)
		static void CL_CALLBACK		
									markerCallback( cl_event, cl_int, void * );								// Triggered when the marker is reached (but static...)

	private:

		// Private variables
		CExecutorControlOpenCL*		execController;															// Controller for this device
		cl_device_id				clDevice;																// OpenCL device
		cl_context					clContext;																// OpenCL context
		cl_command_queue			clQueue;																// OpenCL queue
		cl_event					clMarkerEvent;															// Event associated with the marker
		unsigned int				uiPlatformID;															// Platform ID in the control class
		unsigned int				uiDeviceNo;																// Device number (no order)
		bool						bErrored;																// Serious error triggered
		bool						bForceSinglePrecision;													// Force single precision only?
		bool						bBusy;																	// Is this device busy?

		// Private functions
		void						getAllInfo();															// Fetches all the info we'll need on the device
		template <typename T>
		T						getDeviceInfo( cl_device_info);										// Fetch a device info field

		template <typename T>
		T						getDeviceInfo_pointers(cl_device_info);								// Fetch a device info field

		void						createQueue( void );													// Create the device context and queue

		model::CallBackData			callBackData;
		CModel* cModel;

		// Friendships (for access to data structure pointers mainly)
		friend class				CDomain;
		friend class				CCell;
		friend class				CDomainCartesian;
		friend class				CCellCartesian;
		friend class				COCLProgram;
		friend class				COCLKernel;
		friend class				COCLBuffer;
};

#endif
