/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_CMODEL_H_
#define HIPIMS_CMODEL_H_

#include "CL/opencl.h"
#include "CBenchmark.h"
#include <vector>

// Some classes we need to know about...
class CExecutorControl;
class CExecutorControlOpenCL;
class CDomainCartesian;
class CScheme;
class CLog;
class CProfiler;
class CMPIManager;

namespace model {

	// Floating point precision
	namespace floatPrecision {
		enum floatPrecision {
			kSingle = 0,	// Single-precision
			kDouble = 1		// Double-precision
		};
	}
}

/*
 *  APPLICATION CLASS
 *  CModel
 *
 *  Is a singleton class in reality, but need not be enforced.
 */
class CModel
{
	public:

		// Public functions
		CModel(CLoggingInterface* = NULL, bool profilingOn = false);															// Constructor
		~CModel(void);															// Destructor

		bool					setExecutor(CExecutorControlOpenCL*);			// Sets the type of executor to use for the model
		CExecutorControlOpenCL*	getExecutor(void);								// Gets the executor object currently in use
		CDomainCartesian*		getDomain(void);								// Gets the domain
		void					setSelectedDevice(unsigned int);				// Set the device to execute on
		unsigned int			getSelectedDevice();							// Get the device to execute on

		bool					ValidateAndPrepareModel(void);					// Execute the model
		void					runModelUI( CBenchmark::sPerformanceMetrics * );// Update progress data etc.

		void					logDetails();									// Spit some info out to the log
		double					getSimulationLength();							// Get total length of simulation
		void					setSimulationLength( double );					// Set total length of simulation
		double					getOutputFrequency();							// Get the output frequency
		void					setOutputFrequency( double );					// Set the output frequency
		void					setFloatPrecision( unsigned char );				// Set floating point precision
		model::floatPrecision::floatPrecision
								getFloatPrecision();							// Get floating point precision
		void					logProgress( CBenchmark::sPerformanceMetrics* );// Write the progress bar etc.
		static void CL_CALLBACK	visualiserCallback( cl_event, cl_int, void * );	// Callback event used when memory reads complete, for visualisation updates
		void					runNext(const double);

		// Public variables
		CLog*					log;											// Handle for the log singular class
		void					setUIStatus(bool);								// Turns on/off the UI

		void					setProfiler(CProfiler*);						// Set the profiler 
		CProfiler* profiler;													// Profiler class
	private:

		// Private functions
		void					visualiserUpdate();								// Update 3D stuff 

		// Private variables
		CExecutorControlOpenCL*	execController;									// Handle for the executor controlling class
		CDomainCartesian*		domain;											// Handle for the domain management class
		CMPIManager*			mpiManager;										// Handle for the MPI manager class
		unsigned int			selectedDevice;									// Device selected as the executer
		bool					bDoublePrecision;								// Double precision enabled?
		double					dSimulationTime;								// Total length of simulations
		double					dCurrentTime;									// Current simulation time
		double					dVisualisationTime;								// Current visualisation time
		double					dProcessingTime;								// Total processing time
		double					dOutputFrequency;								// Frequency of outputs
		double					dLastProgressUpdate;							// 
		double					dTargetTime;									// Time to target for before stopping
		unsigned char			ucFloatSize;									// Size of single/double precision floats used
		cursorCoords			pProgressCoords;								// Buffer coords of the progress output
		bool					showProgess;									// Show Progess UI

};

#endif
