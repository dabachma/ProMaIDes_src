/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#include <algorithm>

#include "common.h"
#include "CDomainCartesian.h"
#include "CSchemePromaides2St.h"

using std::min;
using std::max;

//Constructor
CSchemePromaides2St::CSchemePromaides2St(void)
{
	// Scheme is loaded
	model::log->logInfo("Diffusive GPU 2Step scheme loaded for execution on OpenCL platform.");

	// Default setup values
	this->bDebugOutput = false;
	this->uiDebugCellX = 100;
	this->uiDebugCellY = 100;
	this->oclBufferCellStatesPred = NULL;
	this->oclBufferMode = NULL;

	this->ucConfiguration = model::schemeConfigurations::promaidesFormula::kCacheNone;
	this->ucCacheConstraints = model::cacheConstraints::promaidesFormula::kCacheActualSize;

}
//Destructor
CSchemePromaides2St::~CSchemePromaides2St(void)
{
	this->releaseResources();
}
//Run all preparation steps
void CSchemePromaides2St::prepareAll()
{
	// Clean any pre-existing OpenCL objects
	this->releaseResources();

	this->oclModel = new COCLProgram(
		cModel->getExecutor(),
		cModel->getExecutor()->getDevice()
	);

	// Run-time tracking values
	this->ulCurrentCellsCalculated = 0;
	this->dCurrentTimestep = this->dTimestep;
	this->dCurrentTime = 0;

	// Forcing single precision?
	this->oclModel->setForcedSinglePrecision(cModel->getFloatPrecision() == model::floatPrecision::kSingle);

	// OpenCL elements
	try {
		this->prepare1OExecDimensions();
		this->prepare1OConstants();
		this->prepareCode();
		this->prepare1OMemory();
		this->prepareGeneralKernels();
		this->preparePromaidesKernels();
	}catch (const std::exception& e) {
		model::doError(
			std::string("Caught an exception in void CSchemePromaides2St::prepareAll() : ") + e.what(),
			model::errorCodes::kLevelFatal,
			"void CSchemePromaides2St::prepareAll()",
			"Check inputed data."
		);
	}

	this->logDetails();
	this->bReady = true;
}
//Runs the actual simulation until completion or error here the simulation is set together via the kernels this connects the clc-methods as kernels 
void	CSchemePromaides2St::scheduleIteration() {
	//std::cout << "start iteration" << std::endl;
	COCLBuffer* bufferSrc = NULL;
	COCLBuffer* bufferDst = NULL;
	COCLBuffer* bufferPred = this->oclBufferCellStatesPred;

	if (!bUseAlternateKernel) {
		bufferSrc = oclBufferCellStates;
		bufferDst = oclBufferCellStatesAlt;
	}
	else {
		bufferSrc = oclBufferCellStatesAlt;
		bufferDst = oclBufferCellStates;
	}

	//___________________________
	//Step 1 Predictor-Step
	
	oclKernelFullTimestep->assignArgument(2, bufferSrc);
	oclKernelFullTimestep->assignArgument(3, bufferDst);
	oclKernelFullTimestep->assignArgument(10, bufferPred);
	*(this->oclBufferMode->getHostBlock<int*>()) = 0;
	oclKernelFullTimestep->assignArgument(11, this->oclBufferMode);
	this->oclBufferMode->queueWriteAll();
	oclKernelFullTimestep->scheduleExecution();


	
	
	
	//friction Not required for this scheme; just for Gudunov!
	/*if (this->bFrictionEffects && !this->bFrictionInFluxKernel) {
		oclKernelFriction->assignArgument(1, bufferPred);
		oclKernelFriction->scheduleExecution();
	}*/

	//boundary
	//oclKernelBoundary->assignArgument(3, bufferPred);
	//oclKernelBoundary->scheduleExecution();


	//hier warten
	this->getDomain()->getDevice()->blockUntilFinished();
	
	//_______________________
	//Step 2 Corrector-Step
	oclKernelFullTimestep->assignArgument(2, bufferSrc);
	oclKernelFullTimestep->assignArgument(3, bufferDst);
	oclKernelFullTimestep->assignArgument(10, bufferPred);
	*(this->oclBufferMode->getHostBlock<int*>()) = 1;
	oclKernelFullTimestep->assignArgument(11, this->oclBufferMode);
	this->oclBufferMode->queueWriteAll();
	oclKernelFullTimestep->scheduleExecution();


	
	//friction
	if (this->bFrictionEffects && !this->bFrictionInFluxKernel) {
		oclKernelFriction->assignArgument(1, bufferDst);
		oclKernelFriction->scheduleExecution();
	}

	//boundary
	oclKernelBoundary->assignArgument(3, bufferDst);
	oclKernelBoundary->scheduleExecution();



	//________________________
	//Step 3 after Corrector-Step
	if (this->bDynamicTimestep) {
		oclKernelTimestepReduction->assignArgument(0, bufferDst);
		oclKernelTimestepReduction->scheduleExecution();
	}
	
	oclKernelTimeAdvance->scheduleExecution();
	

}
//Log the details and properties of this scheme instance.
void CSchemePromaides2St::logDetails()
{
	model::log->writeDivide();

	std::string sConfiguration = "Undefined";
	switch (this->ucConfiguration)
	{
	case model::schemeConfigurations::promaidesFormula::kCacheNone:
		sConfiguration = "Disabled";
		break;
	case model::schemeConfigurations::promaidesFormula::kCacheEnabled:
		sConfiguration = "Enabled";
		break;
	}

	model::log->logInfo("ProMaIDes 2Step SCHEME");
	model::log->logInfo("  Timestep mode:      " + (std::string)(this->bDynamicTimestep ? "Dynamic" : "Fixed"));
	model::log->logInfo("  Courant number:     " + (std::string)(this->bDynamicTimestep ? toStringExact(this->dCourantNumber) : "N/A"));
	model::log->logInfo("  Initial timestep:   " + Util::secondsToTime(this->dTimestep));
	model::log->logInfo("  Data reduction:     " + toStringExact(this->uiTimestepReductionWavefronts) + " divisions");
	model::log->logInfo("  Configuration:      " + sConfiguration);
	model::log->logInfo("  Friction effects:   " + (std::string)(this->bFrictionEffects ? "Enabled" : "Disabled"));
	model::log->logInfo("  Kernel queue mode:  " + (std::string)(this->bAutomaticQueue ? "Automatic" : "Fixed size"));
	model::log->logInfo((std::string)(this->bAutomaticQueue ? "  Initial queue:      " : "  Fixed queue:        ") + toStringExact(this->uiQueueAdditionSize) + " iteration(s)");
	model::log->logInfo("  Debug output:       " + (std::string)(this->bDebugOutput ? "Enabled" : "Disabled"));

	model::log->writeDivide();
}
//Concatenate together the code for the different elements required
void CSchemePromaides2St::prepareCode()
{

	oclModel->appendCodeFromResource("CLDomainCartesian_H");
	oclModel->appendCodeFromResource("CLFriction_H");
	oclModel->appendCodeFromResource("CLDynamicTimestep_H");
	oclModel->appendCodeFromResource("CLSchemePromaides2_St_H");
	oclModel->appendCodeFromResource("CLBoundaries_2St_H");

	oclModel->appendCodeFromResource("CLDomainCartesian_C");
	oclModel->appendCodeFromResource("CLFriction_C");
	oclModel->appendCodeFromResource("CLDynamicTimestep_C");
	oclModel->appendCodeFromResource("CLSchemePromaides2_St_C");
	oclModel->appendCodeFromResource("CLBoundaries_2St_C");

	oclModel->compileProgram();

}
//Create kernels using the compiled program
void CSchemePromaides2St::preparePromaidesKernels()
{
	// --
	// Promaides scheme kernels
	// --

	oclKernelFullTimestep = oclModel->getKernel("pro2St_cacheDisabled");
	oclKernelFullTimestep->setGroupSize(this->ulNonCachedWorkgroupSizeX, this->ulNonCachedWorkgroupSizeY);
	oclKernelFullTimestep->setGlobalSize(this->ulNonCachedGlobalSizeX, this->ulNonCachedGlobalSizeY);
	COCLBuffer* aryArgsFullTimestep[] = { oclBufferTimestep, oclBufferCellBed, oclBufferCellStates, oclBufferCellStatesAlt, oclBufferCellManning, oclBufferUsePoleni, oclBuffer_opt_zxmax, oclBuffer_opt_cx, oclBuffer_opt_zymax, oclBuffer_opt_cy, oclBufferCellStatesPred, 0 };
	oclKernelFullTimestep->assignArguments(aryArgsFullTimestep);


}
//Release all OpenCL resources consumed using the OpenCL methods
void CSchemePromaides2St::releaseResources()
{
	this->bReady = false;

	this->releasePromaidesResources();
	this->release1OResources();

	if (this->oclBufferCellStatesPred != NULL) {
		delete this->oclBufferCellStatesPred;
		Sys_Memory_Count::self()->minus_mem(4 * sizeof(double) * pDomain->getCellCount(), _sys_system_modules::HYD_SYS);
	}
	this->oclBufferCellStatesPred = NULL;
	if (this->oclBufferMode != NULL)				delete this->oclBufferMode;
	this->oclBufferMode = NULL;

	


}
//Allocate memory for everything that isn't direct domain information (i.e. temporary/scheme data)
void CSchemePromaides2St::prepare1OMemory() {
	CSchemeGodunov::prepare1OMemory();
	unsigned char ucFloatSize = (cModel->getFloatPrecision() == model::floatPrecision::kSingle ? sizeof(cl_float) : sizeof(cl_double));

	void* pCellStatesPred = NULL;
	this->oclBufferCellStatesPred = new COCLBuffer("Cell states (prediction)", oclModel, false, false, ucFloatSize * 4 * pDomain->getCellCount(),true);
	this->oclBufferCellStatesPred->setPointer(pCellStatesPred, ucFloatSize * 4 * pDomain->getCellCount());

	this->oclBufferCellStatesPred->createBuffer();
	this->oclBufferCellStatesPred->setCallbackRead(CModel::visualiserCallback); 
	void* pCellMode = NULL;
	this->oclBufferMode = new COCLBuffer("Mode", oclModel, false, true, sizeof(cl_int), true);
	*(this->oclBufferMode->getHostBlock<int*>()) = 0;
	this->oclBufferMode->createBuffer();


	Sys_Memory_Count::self()->add_mem(4 * sizeof(double) * pDomain->getCellCount(), _sys_system_modules::HYD_SYS);

}
//Release all OpenCL resources consumed using the OpenCL methods
void CSchemePromaides2St::releasePromaidesResources()
{
	this->bReady = false;

	// Nothing to do?
}
//Set the cache configuration to use
void	CSchemePromaides2St::setCacheMode( unsigned char ucCacheMode )
{
	this->ucConfiguration = ucCacheMode;
}
//Get the cache configuration in use
unsigned char	CSchemePromaides2St::getCacheMode()
{
	return this->ucConfiguration;
}
//Set the cache constraints
void	CSchemePromaides2St::setCacheConstraints( unsigned char ucCacheConstraints_input )
{
	this->ucCacheConstraints = ucCacheConstraints_input;
}
//Get the cache constraints
unsigned char	CSchemePromaides2St::getCacheConstraints()
{
	return this->ucCacheConstraints;
}
