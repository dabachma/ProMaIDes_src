/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_COMMON_H_
#define HIPIMS_COMMON_H_

#define CL_TARGET_OPENCL_VERSION 300

// Base includes
#include "util.h"

#ifdef __GNUC__
	#include <unistd.h>
	#include <time.h>
	//#include "CLCode.h"

	#include <math.h>
	#include <execinfo.h>
	#include <signal.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <pthread.h>
	//void segFaultHandler(int sig) { fprintf(stderr, "Seg fault... Bugger.\n"); }
	#define PLATFORM_UNIX
	#define _CONSOLE
	#define __stdcall
	#ifdef __ARM_ARCH
		#define USE_SIMPLE_ARCH_OPENCL
	#endif

#else
	#define PLATFORM_WIN
	#include <Windows.h>					// QueryPerformanceCounter etc

	// Windows-specific includes
	#include <tchar.h>
	#include <direct.h>
#endif

// TODO: Alaa check if these includes are needed. They have been removed with no problems in the other branches (see master branch and gpu branch)
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <cmath>
#include <stdexcept>
#include <thread>

//#define DEBUG_MPI 1

#define toStringExact(s) Util::to_string_exact(s)


#include "CLog.h"
#include "CModel.h"

struct sUsePoleni {
	bool	usePoliniN;
	bool	usePoliniE;
	bool	usePoliniS;
	bool	usePoliniW;
};

// Basic functions and variables used throughout
namespace model{

	// Kernel configurations
	namespace schemeConfigurations {
		namespace musclHancock {
			enum musclHancock {
				kCacheNone = 10,		// Option B in dissertation: No local memory used
				kCachePrediction = 11,		// Option C in dissertation: Only the prediction step uses caching
				kCacheMaximum = 12		// Option D in dissertation: All stages use cache memory
			};
		}
		namespace inertialFormula {
			enum inertialFormula {
				kCacheNone = 0,		// No caching
				kCacheEnabled = 1			// Cache cell state data
			};
		}
		namespace godunovType {
			enum godunovType {
				kCacheNone = 0,		// No caching
				kCacheEnabled = 1			// Cache cell state data
			};
		}
		namespace promaidesFormula {
			enum promaidesFormula {
				kCacheNone = 0,		// No caching
				kCacheEnabled = 1			// Cache cell state data
			};
		}
	}

	namespace cacheConstraints {
		namespace musclHancock {
			enum musclHancock {
				kCacheActualSize = 10,		// LDS of actual size
				kCacheAllowOversize = 11,		// Allow LDS oversizing to avoid bank conflicts
				kCacheAllowUndersize = 12		// Allow LDS undersizing to avoid bank conflicts
			};
		}
		namespace inertialFormula {
			enum inertialFormula {
				kCacheActualSize = 0,		// LDS of actual size
				kCacheAllowOversize = 1,		// Allow LDS oversizing to avoid bank conflicts
				kCacheAllowUndersize = 2			// Allow LDS undersizing to avoid bank conflicts
			};
		}
		namespace godunovType {
			enum godunovType {
				kCacheActualSize = 0,		// LDS of actual size
				kCacheAllowOversize = 1,		// Allow LDS oversizing to avoid bank conflicts
				kCacheAllowUndersize = 2			// Allow LDS undersizing to avoid bank conflicts
			};
		}
		namespace promaidesFormula {
			enum promaidesFormula {
				kCacheActualSize = 0,		// LDS of actual size
				kCacheAllowOversize = 1,		// Allow LDS oversizing to avoid bank conflicts
				kCacheAllowUndersize = 2			// Allow LDS undersizing to avoid bank conflicts
			};
		}
	}

	//CDomain
	// Model domain structure types
	namespace domainValueIndices {
		enum domainValueIndices {
			kValueFreeSurfaceLevel = 0,	// Free-surface level
			kValueMaxFreeSurfaceLevel = 1,	// Max free-surface level
			kValueDischargeX = 2,	// Discharge X
			kValueDischargeY = 3		// Discharge Y
		};
	}

	extern  char*			configFile;
	extern  char*			codeDir;
	void					doError( std::string, unsigned char, std::string, std::string);

// Executor states
	namespace executorStates {
		enum executorStates {
			executorReady = 1,				// This executor can be used
			executorError = 0					// This executor cannot be used
		};
	}

// Device-type filers
	namespace filters {
		namespace devices {
			enum devices {
				devicesGPU = 1,					// Graphics processors
				devicesCPU = 2,					// Standard processors
				devicesAPU = 4						// Accelerated processors
			};
		};
	};

	// Model scheme types
	namespace schemeTypes {
		enum schemeTypes {
			kUndefined = 0,	// Undefined
			kGodunovGPU = 1,	// Godunov (first-order)
			kMUSCLGPU = 2,	// MUSCL-Hancock (second-order)
			kInertialGPU = 3,	// Inertial simplification
			kDiffusiveGPU = 4,	// Inertial simplification
			kDiffusiveCPU = 5		// Diffusive CPU
		};
	}

	// Riemann solver types
	namespace solverTypes {
		enum solverTypes {
			kHLLC = 0		// HLLC approximate
		};
	}

	// Queue mode
	namespace queueMode {
		enum queueMode {
			kAuto = 0,	// Automatic
			kFixed = 1		// Fixed
		};
	}

	// Timestep mode
	namespace timestepMode {
		enum timestepMode {
			kCFL = 0,	// CFL constrained
			kFixed = 1		// Fixed
		};
	}

	// Timestep mode
	namespace syncMethod {
		enum syncMethod {
			kSyncTimestep = 0,						// Timestep synchronised
			kSyncForecast = 1						// Timesteps forecast
		};
	}

	// Data structures used in interop
	struct DomainData
	{
		double			dResolutionX;
		double			dResolutionY;
		double			dWidth;
		double			dHeight;
		double			dCornerWest;
		double			dCornerSouth;
		unsigned long	ulCellCount;
		unsigned long	ulRows;
		unsigned long	ulCols;
		unsigned long	ulBoundaryCells;
		unsigned long	ulBoundaryOthers;
	};

	struct SchemeSettings {
		model::schemeTypes::schemeTypes scheme_type = model::schemeTypes::kUndefined;
		double CourantNumber = 0.5;
		double DryThreshold = 1e-10;
		unsigned char TimestepMode = model::timestepMode::kCFL;
		//unsigned char TimestepMode = model::timestepMode::kFixed;
		double Timestep = 0.01;
		unsigned int ReductionWavefronts = 200;
		bool FrictionStatus = false;
		unsigned char RiemannSolver = model::solverTypes::kHLLC;
		unsigned int NonCachedWorkgroupSize[2] = { 8, 8 };
		bool debuggerOn = false;
		unsigned int debuggerCells[2] = { 0, 0 };
	};

	//Todo: Alaa Remove model:: dependency and allow for safe error logging
	extern CLog* log;				// Global logger class

	struct CallBackData
	{
		unsigned int* DeviceNumber;
		CExecutorControlOpenCL* Executor;
		CModel* cModel;
	};
}

#endif
