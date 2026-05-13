/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#pragma once

#include "CSchemeGodunov.h"


/*
 *  SCHEME CLASS
 *  CSchemePromaides 2 Step approach
 *
 *  Stores and invokes relevant methods for the actual
 *  hydraulic computations.
 */
class CSchemePromaides2St : public CSchemeGodunov
{

	public:

		CSchemePromaides2St( void );											// Constructor
		virtual ~CSchemePromaides2St( void );									// Destructor

		// Public functions
		virtual void		logDetails();									// Write some details about the scheme
		virtual void		prepareAll();									// Prepare absolutely everything for a model run
		virtual void		scheduleIteration();									// Schedule an iteration of the scheme
		void				setCacheMode( unsigned char );					// Set the cache configuration
		unsigned char		getCacheMode();									// Get the cache configuration
		void				setCacheConstraints( unsigned char );			// Set LDS cache size constraints
		unsigned char		getCacheConstraints();							// Get LDS cache size constraints

	protected:

		// Private functions
		virtual void		prepareCode();									// Prepare the code required
		virtual void		releaseResources();								// Release OpenCL resources consumed
		virtual void		prepare1OMemory();								// Prepare memory buffers required
		void				preparePromaidesKernels();						// Prepare the kernels required
		void				releasePromaidesResources();					// Release OpenCL resources consumed

		//private members
		COCLBuffer* oclBufferCellStatesPred;
		COCLBuffer* oclBufferMode;
		

};
