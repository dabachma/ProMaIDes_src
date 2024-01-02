/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


#include "common.h"
#include "CScheme.h"
#include "CSchemeGodunov.h"
#include "CSchemeMUSCLHancock.h"
#include "CSchemeInertial.h"
#include "CSchemePromaides.h"
#include "CDomainCartesian.h"

//Default constructor
CScheme::CScheme()
{
	model::log->logInfo("Scheme base-class instantiated.");

	// Not ready by default
	this->bReady = false;
	this->bRunning = false;
	this->bThreadRunning = false;
	this->bThreadTerminated = false;
	this->bSimulationSlow = false;
	this->bSolverThreadStopped = false;

	this->bAutomaticQueue = true;
	this->uiQueueAdditionSize = 1;
	this->dCourantNumber = 0.5;
	this->dTimestep = 0.001;
	this->bDynamicTimestep = true;
	this->bFrictionEffects = true;
	this->bUseOptimizedBoundary = false;
	this->dTargetTime = 0.0;
	this->uiBatchSkipped = 0;
	this->uiBatchSuccessful = 0;
	this->dBatchTimesteps = 0.0;
}

//Destructor
CScheme::~CScheme(void)
{
	//Nothing to delete
}

//Ask the executor to create a type of scheme with the defined flags.
void CScheme::createScheme(CModel* cModel, model::SchemeSettings scheme_settings)
{
	CScheme* newScheme = NULL;
	switch(scheme_settings.scheme_type)
	{
		case model::schemeTypes::kGodunovGPU:
			newScheme = new CSchemeGodunov();
			break;
		case model::schemeTypes::kMUSCLGPU:
			newScheme = new CSchemeMUSCLHancock();
			break;
		case model::schemeTypes::kInertialGPU:
			newScheme = new CSchemeInertial();
			break;
		case model::schemeTypes::kDiffusiveGPU:
			newScheme = new CSchemePromaides();
			break;
		case model::schemeTypes::kDiffusiveCPU:
			model::doError(
				"You are using the Diffusive CPU Scheme in the GPU Solver",
				model::errorCodes::kLevelFatal,
				"CScheme* CScheme::createScheme( unsigned char ucType )",
				"Something is wrong in the code. CPU schemes shouldn't be redirected here."
			);
			return;
			break;
	}
	if (newScheme == NULL) {
		model::doError(
			"Scheme creation failed",
			model::errorCodes::kLevelFatal,
			"CScheme* CScheme::createScheme( unsigned char ucType )",
			"Please try again with a different scheme type."
		);
		return;
	}
	
	newScheme->prepareSetup(cModel, scheme_settings);

}

//Simple check for if the scheme is ready to run
bool	CScheme::isReady()
{
	return this->bReady;
}

//Set the queue mode
void	CScheme::setQueueMode( unsigned char ucQueueMode )
{
	this->bAutomaticQueue = ( ucQueueMode == model::queueMode::kAuto );
}

//Get the queue mode
model::queueMode::queueMode	CScheme::getQueueMode()
{
	return ( this->bAutomaticQueue ? model::queueMode::kAuto : model::queueMode::kFixed );
}

//Set the queue size (or initial)
void	CScheme::setQueueSize( unsigned int uiQueueSize )
{
	this->uiQueueAdditionSize = uiQueueSize;
}

//Get the queue size (or initial)
unsigned int	CScheme::getQueueSize()
{
	return this->uiQueueAdditionSize;
}

//Set the Courant number
void	CScheme::setCourantNumber( double dCourantNumber_input )
{
	this->dCourantNumber = dCourantNumber_input;
}

//Get the Courant number
double	CScheme::getCourantNumber()
{
	return this->dCourantNumber;
}

//Set the timestep mode
void	CScheme::setTimestepMode( unsigned char ucMode )
{
	this->bDynamicTimestep = ( ucMode == model::timestepMode::kCFL );
}

//Get the timestep mode
model::timestepMode::timestepMode	CScheme::getTimestepMode()
{
	return ( this->bDynamicTimestep ? model::timestepMode::kCFL : model::timestepMode::kFixed );
}

//Set the timestep
void	CScheme::setTimestep( double dTimestep_input )
{
	this->dTimestep = dTimestep_input;
}

//Get the timestep
double	CScheme::getTimestep()
{
	return fabs(this->dTimestep);
}

//Enable/disable friction effects
void	CScheme::setFrictionStatus( bool bEnabled )
{
	this->bFrictionEffects = bEnabled;
}

//Get enabled/disabled for friction
bool	CScheme::getFrictionStatus()
{
	return this->bFrictionEffects;
}

//Set the target time
void	CScheme::setTargetTime( double dTime )
{
	this->dTargetTime = dTime;
}

//Get the target time
double	CScheme::getTargetTime()
{
	return this->dTargetTime;
}

//Are we in the middle of a batch?
bool	CScheme::isRunning()
{
	return bRunning;
}

//Sets the Output Frequency
void	CScheme::setOutputFreq(double freq)
{
	outputFrequency = freq;
}

//Sets the scheme type
void	CScheme::setSchemeType(model::schemeTypes::schemeTypes type)
{
	this->schemeType = type;
}

//Gets the scheme type
model::schemeTypes::schemeTypes	CScheme::getSchemeType(void)
{
	return this->schemeType;
}

//Check if the simulation is too slow (timestep very low)
bool	CScheme::isSimulationSlow(void)
{
	return this->bSimulationSlow;
}

//Check if the solver thread has errors
bool	CScheme::isSolverThreadStopped(void)
{
	return this->bSolverThreadStopped;
}