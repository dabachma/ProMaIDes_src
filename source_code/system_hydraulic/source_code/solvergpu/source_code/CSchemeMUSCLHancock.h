/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_SCHEMES_CSCHEMEMUSCLHANCOCK_H_
#define HIPIMS_SCHEMES_CSCHEMEMUSCLHANCOCK_H_

#include "CSchemeGodunov.h"


/*
 *  SCHEME CLASS
 *  CSchemeMUSCLHancock
 *
 *  Stores and invokes relevant methods for the actual
 *  hydraulic computations.
 */
class CSchemeMUSCLHancock : public CSchemeGodunov
{

	public:

		CSchemeMUSCLHancock( void );										// Constructor
		virtual ~CSchemeMUSCLHancock( void );								// Destructor

		// Public functions
		virtual void		logDetails();									// Write some details about the scheme
		virtual void		prepareAll();									// Prepare absolutely everything for a model run
		virtual void		scheduleIteration();							// Schedule an iteration of the scheme	
		void				setCacheMode( unsigned char );					// Set the cache configuration
		unsigned char		getCacheMode();									// Get the cache configuration
		void				setCacheConstraints( unsigned char );			// Set LDS cache size constraints
		unsigned char		getCacheConstraints();							// Get LDS cache size constraints
		void				setExtrapolatedContiguity( bool );				// Store extrapolated data contiguously?
		bool				getExtrapolatedContiguity();					// Is extrapolated data stored contiguously?
		void				readDomainAll();								// Fetch back all the domain data
		COCLBuffer*			getNextCellSourceBuffer();						// Get the next source cell state buffer

	protected:

		// Private functions
		virtual void		prepareCode();									// Prepare the code required
		virtual void		releaseResources();								// Release OpenCL resources consumed
		void				prepare2OKernels();								// Prepare the kernels required
		void				prepare2OConstants();							// Assign constants to the executor
		void				prepare2OMemory();								// Prepare memory buffers required
		void				prepare2OExecDimensions();						// Size the problem for execution
		void				release2OResources();							// Release 2nd-order OpenCL resources consumed

		// Private variables
		bool				bContiguousFaceData;							// Store all face state data contiguously?

		// OpenCL elements
		COCLKernel*			oclKernelHalfTimestep;
		COCLBuffer*			oclBufferFaceExtrapolations;
		COCLBuffer*			oclBufferFaceExtrapolationN;
		COCLBuffer*			oclBufferFaceExtrapolationE;
		COCLBuffer*			oclBufferFaceExtrapolationS;
		COCLBuffer*			oclBufferFaceExtrapolationW;

};

#endif
