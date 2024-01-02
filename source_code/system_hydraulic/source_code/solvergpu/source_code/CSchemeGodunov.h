/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_SCHEMES_CSCHEMEGODUNOV_H_
#define HIPIMS_SCHEMES_CSCHEMEGODUNOV_H_

#include "CScheme.h"
#include <mutex>


 /*
  *  SCHEME CLASS
  *  CSchemeGodunov
  *
  *  Stores and invokes relevant methods for the actual
  *  hydraulic computations using a first-order scheme
  */
class CSchemeGodunov : public CScheme
{

public:

		CSchemeGodunov( void );						// Default constructor
		virtual ~CSchemeGodunov( void );											// Destructor

		// Public functions
		virtual void		logDetails();											// Write some details about the scheme
		virtual void		prepareAll();											// Prepare absolutely everything for a model run
		virtual void		scheduleIteration();									// Schedule an iteration of the scheme
		void				setDryThreshold( double );								// Set the dry cell threshold depth
		double				getDryThreshold();										// Get the dry cell threshold depth
		void				setReductionWavefronts( unsigned int );					// Set number of wavefronts used in reductions
		unsigned int		getReductionWavefronts();								// Get number of wavefronts used in reductions
		void				setRiemannSolver( unsigned char );						// Set the Riemann solver to use
		unsigned char		getRiemannSolver();										// Get the Riemann solver in use
		void				setCacheMode( unsigned char );							// Set the cache configuration
		unsigned char		getCacheMode();											// Get the cache configuration
		void				setCacheConstraints( unsigned char );					// Set LDS cache size constraints
		unsigned char		getCacheConstraints();									// Get LDS cache size constraints
		void				setCachedWorkgroupSize( unsigned int );					// Set the work-group size
		void				setCachedWorkgroupSize( unsigned int, unsigned int );	// Set the work-group size
		void				setNonCachedWorkgroupSize( unsigned int );				// Set the work-group size
		void				setNonCachedWorkgroupSize( unsigned int, unsigned int );// Set the work-group size
		void				setTargetTime( double );								// Set the target sync time
		double				getAverageTimestep();									// Get batch average timestep
		virtual COCLBuffer*	getNextCellSourceBuffer();								// Get the next source cell state buffer
		void				setDebugger(unsigned int debugX, unsigned int debugY);	// Set the debugger at a cell

		#ifdef PLATFORM_WIN
		static DWORD		Threaded_runBatchLaunch(LPVOID param);
		#endif
		#ifdef PLATFORM_UNIX
			static void*		Threaded_runBatchLaunch(void* param);
		#endif

		void				runBatchThread();
		void				Threaded_runBatch();

		virtual void		readDomainAll();										// Read back all domain data
		virtual void		importBoundaries();										// Read boundary array from CPU RAM to GPU RAM
		void				prepareSetup(CModel*, model::SchemeSettings);			// Set everything up to start running for this domain
		virtual void		prepareSimulation();									// Set everything up to start running for this domain
		virtual void		readKeyStatistics();									// Fetch the key details back to the right places in memory
		virtual void		runSimulation( double );								// Run this simulation until the specified time
		virtual void		cleanupSimulation();									// Dispose of transient data and clean-up this domain
		virtual void		dumpMemory( void );										// Read all buffers so that memory can be dumped
		virtual void		findFastestCells(DomainCell* first, DomainCell* second,
			DomainCell* third);			// Read all buffers so that memory can be dumped
		virtual void		outputAllFloodplainDataToVtk( void );										// Read all buffers so that memory can be dumped


protected:

		// Private variables
		cl_ulong			ulCachedWorkgroupSizeX, ulCachedWorkgroupSizeY;
		cl_ulong			ulNonCachedWorkgroupSizeX, ulNonCachedWorkgroupSizeY;
		cl_ulong			ulCachedGlobalSizeX, ulCachedGlobalSizeY;
		cl_ulong			ulNonCachedGlobalSizeX, ulNonCachedGlobalSizeY;
		cl_ulong			ulCouplingArraySize;
		cl_ulong			ulReductionWorkgroupSize;
		cl_ulong			ulReductionGlobalSize;

		unsigned char		ucConfiguration;										// Kernel configuration in-use
		unsigned char		ucCacheConstraints;										// Kernel LDS cache constraints
		unsigned char		ucSolverType;											// Code for the Riemann solver type
		double				dThresholdVerySmall;									// Threshold value for 'very small'
		double				dThresholdQuiteSmall;									// Threshold value for 'quite small'
		bool				bDebugOutput;											// Debug output enabled in the scheme?
		bool				bFrictionInFluxKernel;									// Process friction in the flux kernel?
		bool				bUseAlternateKernel;									// Use the alternate kernel configurations (for memory)
		bool				bUpdateTargetTime;										// Update the target time?
		bool				bImportBoundaries;										// Import link data?
		unsigned int		uiDebugCellX;											// Debug info cell X
		unsigned int		uiDebugCellY;											// Debug info cell Y
		unsigned int		uiTimestepReductionWavefronts;							// Number of wavefronts used in reduction

		// Private functions
		virtual void		prepareCode();											// Prepare the code required
		virtual void		releaseResources();										// Release OpenCL resources consumed
		void				prepareGeneralKernels();								// Prepare the general kernels required
		void				prepare1OKernels();										// Prepare the kernels required
		void				prepare1OConstants();									// Assign constants to the executor
		void				prepare1OMemory();										// Prepare memory buffers required
		void				prepare1OExecDimensions();								// Size the problem for execution
		void				release1OResources();									// Release 1st-order OpenCL resources consumed

		// OpenCL elements
		COCLProgram*		oclModel;
		COCLKernel*			oclKernelFullTimestep;
		COCLKernel*			oclKernelBoundary;
		COCLKernel*			oclKernelFriction;
		COCLKernel*			oclKernelTimestepReduction;
		COCLKernel*			oclKernelTimeAdvance;
		COCLKernel*			oclKernelResetCounters;
		COCLBuffer*			oclBufferCellStates;
		COCLBuffer*			oclBufferCellStatesAlt;
		COCLBuffer*			oclBufferCellManning;
		COCLBuffer*			oclBufferCellBoundary;
		COCLBuffer*			oclBufferCouplingIDs;
		COCLBuffer*			oclBufferCouplingValues;
		COCLBuffer*			oclBufferUsePoleni;
		COCLBuffer*			oclBuffer_opt_zxmax;
		COCLBuffer*			oclBuffer_opt_cx;
		COCLBuffer*			oclBuffer_opt_zymax;
		COCLBuffer*			oclBuffer_opt_cy;
		COCLBuffer*			oclBufferCellBed;
		COCLBuffer*			oclBufferTimestep;
		COCLBuffer*			oclBufferTime;
		COCLBuffer*			oclBufferTimeTarget;
		COCLBuffer*			oclBufferTimestepMovAvg;
		COCLBuffer*			oclBufferTimeHydrological;
		COCLBuffer*			oclBufferTimestepReduction;
		COCLBuffer*			oclBufferBatchTimesteps;
		COCLBuffer*			oclBufferBatchSuccessful;
		COCLBuffer*			oclBufferBatchSkipped;

};

#endif
