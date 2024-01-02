/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_SCHEMES_CSCHEME_H_
#define HIPIMS_SCHEMES_CSCHEME_H_

#include "common.h"
#include "CBenchmark.h"
#include "CDomainCartesian.h"
#include "CExecutorControlOpenCL.h"
#include "COCLProgram.h"
#include "COCLDevice.h"
#include "COCLKernel.h"
#include "COCLBuffer.h"
#include "CProfiler.h"



/*
 *  SCHEME CLASS
 *  CScheme
 *
 *  Stores and invokes relevant methods for the actual
 *  hydraulic computations.
 */
class CScheme
{

	public:

		CScheme();													// Default constructor
		virtual ~CScheme( void );																	// Destructor

		// Public functions
		static void							createScheme(CModel*, model::SchemeSettings);			// Instantiate a scheme

		virtual void						setDebugger(unsigned int, unsigned int) = 0;
		bool								isReady();																// Is the scheme ready to run?
		bool								isRunning();															// Is this scheme currently running a batch?
		virtual void						logDetails() = 0;														// Write some details about the scheme
		virtual void						prepareAll() = 0;														// Prepare absolutely everything for a model run
		virtual void						prepareSetup(CModel*, model::SchemeSettings) = 0;						// Prepare absolutely everything for a model run
		void								setQueueMode( unsigned char );											// Set the queue mode
		model::queueMode::queueMode			getQueueMode();															// Get the queue mode
		void								setQueueSize( unsigned int );											// Set the queue size (or initial)
		unsigned int						getQueueSize();															// Get the queue size (or initial)
		void								setCourantNumber( double );												// Set the Courant number
		double								getCourantNumber();														// Get the Courant number
		void								setTimestepMode( unsigned char );										// Set the timestep mode
		model::timestepMode::timestepMode	getTimestepMode();														// Get the timestep mode
		void								setTimestep( double );													// Set the timestep
		double								getTimestep();															// Get the timestep
		void								setSchemeType(model::schemeTypes::schemeTypes);							// Set the scheme type
		model::schemeTypes::schemeTypes		getSchemeType();														// Gets the scheme type
		virtual double						getAverageTimestep() = 0;												// Get batch average timestep
		void								setFrictionStatus( bool );												// Enable/disable friction effects
		bool								getFrictionStatus();													// Get enabled/disabled for friction
		virtual void						setTargetTime( double );												// Set the target sync time
		double								getTargetTime();														// Get the target sync time
		void								setDomain( CDomainCartesian *d )			{ pDomain = d; }			// Set the domain we're working on
		CDomainCartesian*					getDomain()						{ return pDomain; }						// Fetch the domain we're working on
		unsigned long long					getCellsCalculated()			{ return ulCurrentCellsCalculated; }	// Number of cells calculated so far
		double								getCurrentTimestep()			{ return dCurrentTimestep; }			// Current timestep
		double								getCurrentTimestepMovAvg()		{ return dCurrentTimestepMovAvg; }		// Current timestep Mov Average
		bool								getCurrentSuspendedState()		{ return ( dCurrentTimestep < 0.0 ); }	// Is the simulation suspended?
		double								getCurrentTime()				{ return dCurrentTime; }				// Current progress
		unsigned int						getBatchSize()					{ return uiQueueAdditionSize; }			// Get the batch size
		unsigned int						getIterationsSuccessful()		{ return uiBatchSuccessful; }			// Get the successful iterations
		unsigned int						getIterationsSkipped()			{ return uiBatchSkipped; }				// Get the number of iterations skipped
		virtual void						readDomainAll() = 0;													// Read back all domain data
		virtual void						importBoundaries() = 0;													// Read back synchronization zone data
		virtual void						prepareSimulation() = 0;												// Set everything up to start running for this domain
		virtual void						readKeyStatistics() = 0;												// Fetch the key statistics back to the right places in memory
		virtual void						runSimulation( double ) = 0;											// Run this simulation until the specified time
		virtual void						cleanupSimulation() = 0;												// Dispose of transient data and clean-up this domain
		virtual COCLBuffer*					getNextCellSourceBuffer() = 0;											// Get the next source cell state buffer
		virtual void						dumpMemory() = 0;														// Read back all domain data
		virtual void						findFastestCells(DomainCell*, DomainCell*, DomainCell*) = 0;			//
		void								setOutputFreq(double);
		bool								isSimulationSlow(void);													// Check if the simulation is too slow (timestep very low)
		bool								isSolverThreadStopped(void);											//Check if the solver thread has errors

	protected:

		// Private functions
		// ...

		// Private variables
		bool							bSimulationSlow;														// Is the simulation too slow (timestep very low)?
		bool							bSolverThreadStopped;													// Does solver thread have errors
		bool							bRunning;																// Is this simulation currently running?
		bool							bThreadRunning;															// Is the worker thread running?
		bool							bThreadTerminated;														// Has the worker thread been terminated?
		bool							bReady;																	// Is the scheme ready?
		bool							bBatchComplete;															// Is the batch done?
		bool							bBatchError;															// Have we run out of room?
		bool							bFrictionEffects;														// Activate friction effects?
		bool							bUseOptimizedBoundary;													// Boundaries that are always zero are ignored
		unsigned long long				ulCurrentCellsCalculated;												// Total number of cells calculated
		double							dCurrentTime;															// Current simulation time
		double							dCurrentTimestep;														// Current simulation timestep
		double							dCurrentTimestepMovAvg;													// Current simulation timestep Moving Average
		double							dTargetTime;															// Target time for synchronization
		bool							bAutomaticQueue;														// Automatic queue size detection?
		double							dTimestep;																// Constant/initial timestep
		unsigned int					uiQueueAdditionSize;													// Number of runs to queue at once
		unsigned int					uiIterationsSinceTargetChanged;											// Number of iterations since target time changed
		unsigned int					uiIterationsTotal;														// Number of total iterations
		cl_double						dBatchTimesteps;														// Cumulative batch timesteps
		double							dAvgTimestep;															// Average Timestep across time
		cl_uint							uiBatchSuccessful;														// Number of successful batch iterations
		unsigned int					uiSuccessfulIterationsTotal;											// Total successful iterations
		cl_uint							uiBatchSkipped;															// Number of skipped batch 
		unsigned int					uiSkippedIterationsTotal;												// Total skipped iterations
		double							dCourantNumber;															// Courant number for CFL condition
		bool							bDynamicTimestep;														// Dynamic timestepping enabled?
		cl_uint							uiBatchRate;															// Number of successful iterations per second
		CDomainCartesian*				pDomain;																// Domain which this scheme is attached to
		double							outputFrequency;
		CModel*							cModel;
		model::schemeTypes::schemeTypes	schemeType;
		
};

#endif