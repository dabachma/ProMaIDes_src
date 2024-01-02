/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_SCHEMES_CSCHEMEINERTIAL_H_
#define HIPIMS_SCHEMES_CSCHEMEINERTIAL_H_

#include "CSchemeGodunov.h"


/*
 *  SCHEME CLASS
 *  CSchemeInertial
 *
 *  Stores and invokes relevant methods for the actual
 *  hydraulic computations.
 */
class CSchemeInertial : public CSchemeGodunov
{

	public:

		CSchemeInertial( void );											// Constructor
		virtual ~CSchemeInertial( void );									// Destructor

		// Public functions
		virtual void		logDetails();									// Write some details about the scheme
		virtual void		prepareAll();									// Prepare absolutely everything for a model run
		void				setCacheMode( unsigned char );					// Set the cache configuration
		unsigned char		getCacheMode();									// Get the cache configuration
		void				setCacheConstraints( unsigned char );			// Set LDS cache size constraints
		unsigned char		getCacheConstraints();							// Get LDS cache size constraints

	protected:

		// Private functions
		virtual void		prepareCode();									// Prepare the code required
		virtual void		releaseResources();								// Release OpenCL resources consumed
		void				prepareInertialKernels();						// Prepare the kernels required
		void				prepareInertialConstants();						// Assign constants to the executor
		void				releaseInertialResources();						// Release OpenCL resources consumed

};

#endif
