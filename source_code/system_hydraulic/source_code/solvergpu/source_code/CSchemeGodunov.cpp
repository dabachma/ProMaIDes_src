/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#define profiless this->cModel->profiler->profile("oclKernelFullTimestep", CProfiler::profilerFlags::START_PROFILING);
#define profilese this->cModel->profiler->profile("oclKernelFullTimestep", CProfiler::profilerFlags::END_PROFILING, this->pDomain->getDevice());
#define profilebs this->cModel->profiler->profile("oclKernelBoundary", CProfiler::profilerFlags::START_PROFILING);
#define profilebe this->cModel->profiler->profile("oclKernelBoundary", CProfiler::profilerFlags::END_PROFILING, this->pDomain->getDevice());
#define profilers this->cModel->profiler->profile("oclKernelTimestepReduction", CProfiler::profilerFlags::START_PROFILING);
#define profilere this->cModel->profiler->profile("oclKernelTimestepReduction", CProfiler::profilerFlags::END_PROFILING, this->pDomain->getDevice());
#define profilets this->cModel->profiler->profile("oclKernelTimeAdvance", CProfiler::profilerFlags::START_PROFILING);
#define profilete this->cModel->profiler->profile("oclKernelTimeAdvance", CProfiler::profilerFlags::END_PROFILING, this->pDomain->getDevice());
#define profileqs this->cModel->profiler->profile("QueueReading", CProfiler::profilerFlags::START_PROFILING);
#define profileqe this->cModel->profiler->profile("QueueReading", CProfiler::profilerFlags::END_PROFILING, this->pDomain->getDevice());

#include <algorithm>

#include "common.h"
#include "CDomainCartesian.h"

#include "CSchemeGodunov.h"
#include "CSchemeMUSCLHancock.h"
#include "CSchemeInertial.h"
#include "CSchemePromaides.h"

using std::min;
using std::max;

//Default constructor
CSchemeGodunov::CSchemeGodunov(void)
{
	// Scheme is loaded
	model::log->logInfo("Godunov-type scheme loaded for execution on OpenCL platform.");

	// Default setup values
	this->bRunning = false;
	this->bThreadRunning = false;
	this->bThreadTerminated = false;
	this->bDebugOutput = false;
	this->uiDebugCellX = 9999;
	this->uiDebugCellY = 9999;

	this->uiIterationsTotal = 0;
	this->uiSuccessfulIterationsTotal = 0;
	this->uiSkippedIterationsTotal = 0;

	this->bImportBoundaries = false;
	this->bUpdateTargetTime = false;
	this->bUseAlternateKernel = false;
	this->ulCachedGlobalSizeX = 0;
	this->ulCachedGlobalSizeY = 0;
	this->ulCouplingArraySize = 0;
	this->ulNonCachedGlobalSizeX = 0;
	this->ulNonCachedGlobalSizeY = 0;
	this->ulReductionGlobalSize = 0;
	this->ulReductionWorkgroupSize = 0;
	this->dAvgTimestep = 0.0;

	this->dCurrentTime = 0.0;
	this->dCurrentTimestepMovAvg = 0.0;
	this->dThresholdVerySmall = 1E-10;
	this->dThresholdQuiteSmall = this->dThresholdVerySmall * 10;
	this->bFrictionInFluxKernel = false;
	this->uiTimestepReductionWavefronts = 200;

	this->ucSolverType = model::solverTypes::kHLLC;
	this->ucConfiguration = model::schemeConfigurations::godunovType::kCacheNone;
	this->ucCacheConstraints = model::cacheConstraints::godunovType::kCacheActualSize;

	this->ulCachedWorkgroupSizeX = 0;
	this->ulCachedWorkgroupSizeY = 0;
	this->ulNonCachedWorkgroupSizeX = 0;
	this->ulNonCachedWorkgroupSizeY = 0;

	// Default null values for OpenCL objects
	oclModel = NULL;
	oclKernelFullTimestep = NULL;
	oclKernelBoundary = NULL;
	oclKernelFriction = NULL;
	oclKernelTimestepReduction = NULL;
	oclKernelTimeAdvance = NULL;
	oclKernelResetCounters = NULL;
	oclBufferCellStates = NULL;
	oclBufferCellStatesAlt = NULL;
	oclBufferCellManning = NULL;
	oclBufferCellBoundary = NULL;
	oclBufferUsePoleni = NULL;
	oclBuffer_opt_zxmax = NULL;
	oclBuffer_opt_cx = NULL;
	oclBuffer_opt_zymax = NULL;
	oclBuffer_opt_cy = NULL;
	oclBufferCellBed = NULL;
	oclBufferTimestep = NULL;
	oclBufferTimestepReduction = NULL;
	oclBufferTime = NULL;
	oclBufferTimeTarget = NULL;
	oclBufferTimestepMovAvg = NULL;
	oclBufferTimeHydrological = NULL;
	oclBufferCouplingIDs = NULL;
	oclBufferCouplingValues = NULL;

	oclBufferBatchTimesteps = NULL;
	oclBufferBatchSuccessful = NULL;
	oclBufferBatchSkipped = NULL;

	if (this->bDebugOutput)
		model::doError("Debug mode is enabled!",
			model::errorCodes::kLevelWarning,
			"CSchemeGodunov::CSchemeGodunov()",
			"Additional information will be printed."
		);

	model::log->logInfo("Populated scheme with default settings.");
}

//Destructor
CSchemeGodunov::~CSchemeGodunov(void)
{
	this->releaseResources();
}

//Log the details and properties of this scheme instance.
void CSchemeGodunov::logDetails()
{
	model::log->writeDivide();

	std::string sSolver = "Undefined";
	switch (this->ucSolverType)
	{
	case model::solverTypes::kHLLC:
		sSolver = "HLLC (Approximate)";
		break;
	}

	std::string sConfiguration = "Undefined";
	switch (this->ucConfiguration)
	{
	case model::schemeConfigurations::godunovType::kCacheNone:
		sConfiguration = "No local caching";
		break;
	case model::schemeConfigurations::godunovType::kCacheEnabled:
		sConfiguration = "Original state caching";
		break;
	}

	model::log->logInfo("GODUNOV-TYPE 1ST-ORDER-ACCURATE SCHEME");
	model::log->logInfo("  Timestep mode:      " + (std::string)(this->bDynamicTimestep ? "Dynamic" : "Fixed"));
	model::log->logInfo("  Courant number:     " + (std::string)(this->bDynamicTimestep ? toStringExact(this->dCourantNumber) : "N/A"));
	model::log->logInfo("  Initial timestep:   " + Util::secondsToTime(this->dTimestep));
	model::log->logInfo("  Data reduction:     " + toStringExact(this->uiTimestepReductionWavefronts) + " divisions");
	model::log->logInfo("  Riemann solver:     " + sSolver);
	model::log->logInfo("  Configuration:      " + sConfiguration);
	model::log->logInfo("  Friction effects:   " + (std::string)(this->bFrictionEffects ? "Enabled" : "Disabled"));
	model::log->logInfo("  Kernel queue mode:  " + (std::string)(this->bAutomaticQueue ? "Automatic" : "Fixed size"));
	model::log->logInfo((std::string)(this->bAutomaticQueue ? "  Initial queue:      " : "  Fixed queue:        ") + toStringExact(this->uiQueueAdditionSize) + " iteration(s)");
	model::log->logInfo("  Debug output:       " + (std::string)(this->bDebugOutput ? "Enabled" : "Disabled"));

	model::log->writeDivide();
}

void CSchemeGodunov::prepareSetup(CModel* cModel_input, model::SchemeSettings schemeSettings) {

	this->cModel = cModel_input;
	this->setSchemeType(schemeSettings.scheme_type);
	this->setCourantNumber(schemeSettings.CourantNumber);
	this->setDryThreshold(schemeSettings.DryThreshold);
	this->setTimestepMode(schemeSettings.TimestepMode);
	this->setTimestep(schemeSettings.Timestep);
	this->setReductionWavefronts(schemeSettings.ReductionWavefronts);
	this->setFrictionStatus(schemeSettings.FrictionStatus);
	this->setRiemannSolver(schemeSettings.RiemannSolver);
	this->setNonCachedWorkgroupSize(schemeSettings.NonCachedWorkgroupSize[0], schemeSettings.NonCachedWorkgroupSize[1]);
	this->setOutputFreq(cModel->getOutputFrequency());

	this->setDomain(cModel->getDomain());

	if (schemeSettings.debuggerOn) {
		this->setDebugger(schemeSettings.debuggerCells[0], schemeSettings.debuggerCells[1]);
	}

	this->setDomain(this->cModel->getDomain());
	this->cModel->getDomain()->setScheme(this);
	this->prepareAll();
}

//Run all preparation steps
void CSchemeGodunov::prepareAll()
{
	model::log->logInfo("Starting to prepare program for Godunov-type scheme.");

	this->releaseResources();

	this->oclModel = new COCLProgram(
		cModel->getExecutor(),
		this->pDomain->getDevice()
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
		this->prepare1OKernels();
	}catch (const std::exception& e) {
		model::doError(
			std::string("Caught an exception in void CSchemeGodunov::prepareAll() : ") + e.what(),
			model::errorCodes::kLevelFatal,
			"void CSchemeGodunov::prepareAll()",
			"Check inputed data."
		);
	}


	this->logDetails();
	this->bReady = true;
}

//Concatenate together the code for the different elements required
void CSchemeGodunov::prepareCode()
{

	oclModel->appendCodeFromResource("CLDomainCartesian_H");
	oclModel->appendCodeFromResource("CLFriction_H");
	oclModel->appendCodeFromResource("CLSolverHLLC_H");
	oclModel->appendCodeFromResource("CLDynamicTimestep_H");
	oclModel->appendCodeFromResource("CLSchemeGodunov_H");
	oclModel->appendCodeFromResource("CLBoundaries_H");

	oclModel->appendCodeFromResource("CLDomainCartesian_C");
	oclModel->appendCodeFromResource("CLFriction_C");
	oclModel->appendCodeFromResource("CLSolverHLLC_C");
	oclModel->appendCodeFromResource("CLDynamicTimestep_C");
	oclModel->appendCodeFromResource("CLSchemeGodunov_C");
	oclModel->appendCodeFromResource("CLBoundaries_C");

	oclModel->compileProgram();

}

//Set the dry cell threshold depth
void	CSchemeGodunov::setDryThreshold(double dThresholdDepth)
{
	this->dThresholdVerySmall = dThresholdDepth;
	this->dThresholdQuiteSmall = dThresholdDepth * 10;
}

//Get the dry cell threshold depth
double	CSchemeGodunov::getDryThreshold()
{
	return this->dThresholdVerySmall;
}

//Set number of wavefronts used in reductions
void	CSchemeGodunov::setReductionWavefronts(unsigned int uiWavefronts)
{
	this->uiTimestepReductionWavefronts = uiWavefronts;
}

//Get number of wavefronts used in reductions
unsigned int	CSchemeGodunov::getReductionWavefronts()
{
	return this->uiTimestepReductionWavefronts;
}

//Set the Riemann solver to use
void	CSchemeGodunov::setRiemannSolver(unsigned char ucRiemannSolver)
{
	this->ucSolverType = ucRiemannSolver;
}

//Get the Riemann solver in use
unsigned char	CSchemeGodunov::getRiemannSolver()
{
	return this->ucSolverType;
}

//Set the cache configuration to use
void	CSchemeGodunov::setCacheMode(unsigned char ucCacheMode)
{
	this->ucConfiguration = ucCacheMode;
}

//Get the cache configuration in use
unsigned char	CSchemeGodunov::getCacheMode()
{
	return this->ucConfiguration;
}

//Set the cache size
void	CSchemeGodunov::setCachedWorkgroupSize(unsigned int ucSize)
{
	this->ulCachedWorkgroupSizeX = ucSize; this->ulCachedWorkgroupSizeY = ucSize;
}
void	CSchemeGodunov::setCachedWorkgroupSize(unsigned int ucSizeX, unsigned int ucSizeY)
{
	this->ulCachedWorkgroupSizeX = ucSizeX; this->ulCachedWorkgroupSizeY = ucSizeY;
}
void	CSchemeGodunov::setNonCachedWorkgroupSize(unsigned int ucSize)
{
	this->ulNonCachedWorkgroupSizeX = ucSize; this->ulNonCachedWorkgroupSizeY = ucSize;
}
void	CSchemeGodunov::setNonCachedWorkgroupSize(unsigned int ucSizeX, unsigned int ucSizeY)
{
	this->ulNonCachedWorkgroupSizeX = ucSizeX; this->ulNonCachedWorkgroupSizeY = ucSizeY;
}

//Set the cache constraints
void	CSchemeGodunov::setCacheConstraints(unsigned char ucCacheConstraints_input)
{
	this->ucCacheConstraints = ucCacheConstraints_input;
}

//Get the cache constraints
unsigned char	CSchemeGodunov::getCacheConstraints()
{
	return this->ucCacheConstraints;
}

//Calculate the dimensions for executing the problems (e.g. reduction glob/local sizes)
void CSchemeGodunov::prepare1OExecDimensions()
{
	CExecutorControlOpenCL* pExecutor = cModel->getExecutor();
	COCLDevice* pDevice = pExecutor->getDevice();

	// --
	// Maximum permissible work-group dimensions for this device
	// --

	cl_ulong	ulConstraintWGTotal = (cl_ulong)floor(sqrt(static_cast<double> (pDevice->clDeviceMaxWorkGroupSize)));
	cl_ulong	ulConstraintWGDim = min(pDevice->clDeviceMaxWorkItemSizes[0], pDevice->clDeviceMaxWorkItemSizes[1]);
	cl_ulong	ulConstraintWG = min(ulConstraintWGDim, ulConstraintWGTotal);

	// --
	// Main scheme kernels with/without caching (2D)
	// --

	if (this->ulNonCachedWorkgroupSizeX == 0)
		ulNonCachedWorkgroupSizeX = ulConstraintWG;
	if (this->ulNonCachedWorkgroupSizeY == 0)
		ulNonCachedWorkgroupSizeY = ulConstraintWG;

	ulNonCachedGlobalSizeX = pDomain->getCols();
	ulNonCachedGlobalSizeY = pDomain->getRows();

	if (this->ulCachedWorkgroupSizeX == 0)
		ulCachedWorkgroupSizeX = ulConstraintWG +
		(this->ucCacheConstraints == model::cacheConstraints::musclHancock::kCacheAllowUndersize ? -1 : 0);
	if (this->ulCachedWorkgroupSizeY == 0)
		ulCachedWorkgroupSizeY = ulConstraintWG;

	ulCachedGlobalSizeX = static_cast<unsigned long>(ceil(pDomain->getCols() *
		(this->ucConfiguration == model::schemeConfigurations::godunovType::kCacheEnabled ? static_cast<double>(ulCachedWorkgroupSizeX) / static_cast<double>(ulCachedWorkgroupSizeX - 2) : 1.0)));
	ulCachedGlobalSizeY = static_cast<unsigned long>(ceil(pDomain->getRows() *
		(this->ucConfiguration == model::schemeConfigurations::godunovType::kCacheEnabled ? static_cast<double>(ulCachedWorkgroupSizeY) / static_cast<double>(ulCachedWorkgroupSizeY - 2) : 1.0)));

	// --
	// Optimized Coupling
	// --
	this->bUseOptimizedBoundary = pDomain->getUseOptimizedCoupling();
	this->ulCouplingArraySize = pDomain->getOptimizedCouplingSize();


	// --
	// Timestep reduction (2D)
	// --

	// TODO: May need to make this configurable?!
	ulReductionWorkgroupSize = min(static_cast<size_t>(512), pDevice->clDeviceMaxWorkGroupSize);
	//ulReductionWorkgroupSize = pDevice->clDeviceMaxWorkGroupSize / 2;
	ulReductionGlobalSize = static_cast<unsigned long>(ceil((static_cast<double>(pDomain->getCellCount()) / this->uiTimestepReductionWavefronts) / ulReductionWorkgroupSize) * ulReductionWorkgroupSize);

}

//Allocate constants using the settings herein
void CSchemeGodunov::prepare1OConstants(){
	// --
	// Dry cell threshold depths
	// --
	oclModel->registerConstant("VERY_SMALL", toStringExact(this->dThresholdVerySmall));
	oclModel->registerConstant("QUITE_SMALL", toStringExact(this->dThresholdQuiteSmall));

	// --
	// Debug mode 
	// --

	if (this->bDebugOutput)
	{
		oclModel->registerConstant("DEBUG_OUTPUT", "1");
		oclModel->registerConstant("DEBUG_CELLX", std::to_string(this->uiDebugCellX));
		oclModel->registerConstant("DEBUG_CELLY", std::to_string(this->uiDebugCellY));
	}
	else {
		oclModel->removeConstant("DEBUG_OUTPUT");
		oclModel->removeConstant("DEBUG_CELLX");
		oclModel->removeConstant("DEBUG_CELLY");
	}

	// --
	// Work-group size requirements
	// --

	if (this->ucConfiguration == model::schemeConfigurations::godunovType::kCacheNone)
	{
		oclModel->registerConstant(
			"REQD_WG_SIZE_FULL_TS",
			"__attribute__((reqd_work_group_size(" + std::to_string(this->ulNonCachedWorkgroupSizeX) + ", " + std::to_string(this->ulNonCachedWorkgroupSizeY) + ", 1)))"
		);
	}
	if (this->ucConfiguration == model::schemeConfigurations::godunovType::kCacheEnabled)
	{
		oclModel->registerConstant(
			"REQD_WG_SIZE_FULL_TS",
			"__attribute__((reqd_work_group_size(" + std::to_string(this->ulNonCachedWorkgroupSizeX) + ", " + std::to_string(this->ulNonCachedWorkgroupSizeY) + ", 1)))"
		);
	}

	oclModel->registerConstant(
		"REQD_WG_SIZE_LINE",
		"__attribute__((reqd_work_group_size(" + std::to_string(this->ulReductionWorkgroupSize) + ", 1, 1)))"
	);

	// --
	// Size of local cache arrays
	// --

	switch (this->ucCacheConstraints)
	{
	case model::cacheConstraints::godunovType::kCacheActualSize:
		oclModel->registerConstant("GTS_DIM1", std::to_string(this->ulCachedWorkgroupSizeX));
		oclModel->registerConstant("GTS_DIM2", std::to_string(this->ulCachedWorkgroupSizeY));
		break;
	case model::cacheConstraints::godunovType::kCacheAllowUndersize:
		oclModel->registerConstant("GTS_DIM1", std::to_string(this->ulCachedWorkgroupSizeX));
		oclModel->registerConstant("GTS_DIM2", std::to_string(this->ulCachedWorkgroupSizeY));
		break;
	case model::cacheConstraints::godunovType::kCacheAllowOversize:
		oclModel->registerConstant("GTS_DIM1", std::to_string(this->ulCachedWorkgroupSizeX));
		oclModel->registerConstant("GTS_DIM2", std::to_string(this->ulCachedWorkgroupSizeY == 16 ? 17 : ulCachedWorkgroupSizeY));
		break;
	}

	// --
	// CFL/fixed timestep
	// --

	if (this->bDynamicTimestep)
	{
		oclModel->registerConstant("TIMESTEP_DYNAMIC", "1");
		oclModel->removeConstant("TIMESTEP_FIXED");
	}
	else {
		oclModel->registerConstant("TIMESTEP_FIXED", std::to_string(this->dTimestep));
		oclModel->removeConstant("TIMESTEP_DYNAMIC");
	}

	if (this->bFrictionEffects)
	{
		oclModel->registerConstant("FRICTION_ENABLED", "1");
	}
	else {
		oclModel->removeConstant("FRICTION_ENABLED");
	}

	if (this->bFrictionInFluxKernel)
	{
		oclModel->registerConstant("FRICTION_IN_FLUX_KERNEL", "1");
	}

	// --
	// Timestep reduction and simulation parameters
	// --
	oclModel->registerConstant("TIMESTEP_WORKERS", std::to_string(this->ulReductionGlobalSize));
	oclModel->registerConstant("TIMESTEP_GROUPSIZE", std::to_string(this->ulReductionWorkgroupSize));
	oclModel->registerConstant("SCHEME_ENDTIME", std::to_string(cModel->getSimulationLength()));
	oclModel->registerConstant("SCHEME_OUTPUTTIME", std::to_string(cModel->getOutputFrequency()));
	oclModel->registerConstant("COURANT_NUMBER", std::to_string(this->dCourantNumber));

	// --
	// Domain details (size, resolution, etc.)
	// --

	double	dResolutionX, dResolutionY;
	pDomain->getCellResolution(&dResolutionX, &dResolutionY);

	unsigned long ulOptimizedCouplingArraySize = pDomain->getOptimizedCouplingSize();

	oclModel->registerConstant("DOMAIN_CELLCOUNT", std::to_string(pDomain->getCellCount()));
	oclModel->registerConstant("DOMAIN_COLS", std::to_string(pDomain->getCols()));
	oclModel->registerConstant("DOMAIN_ROWS", std::to_string(pDomain->getRows()));
	oclModel->registerConstant("DOMAIN_DELTAX", std::to_string(dResolutionX));
	oclModel->registerConstant("DOMAIN_DELTAY", std::to_string(dResolutionY));
	oclModel->registerConstant("COUPLING_ARRAY_SIZE", std::to_string(ulOptimizedCouplingArraySize));

}
//Allocate memory for everything that isn't direct domain information (i.e. temporary/scheme data)
void CSchemeGodunov::prepare1OMemory(){

	unsigned char ucFloatSize = (cModel->getFloatPrecision() == model::floatPrecision::kSingle ? sizeof(cl_float) : sizeof(cl_double));

	// --
	// Batch tracking data
	// --

	this->oclBufferBatchTimesteps = new COCLBuffer("Batch timesteps cumulative", oclModel, false, true, ucFloatSize, true);
	this->oclBufferBatchSuccessful = new COCLBuffer("Batch successful iterations", oclModel, false, true, sizeof(cl_uint), true);
	this->oclBufferBatchSkipped = new COCLBuffer("Batch skipped iterations", oclModel, false, true, sizeof(cl_uint), true);

	if (cModel->getFloatPrecision() == model::floatPrecision::kSingle)
	{
		*(this->oclBufferBatchTimesteps->getHostBlock<float*>()) = 0.0f;
	}
	else {
		*(this->oclBufferBatchTimesteps->getHostBlock<double*>()) = 0.0;
	}
	*(this->oclBufferBatchSuccessful->getHostBlock<cl_uint*>()) = 0;
	*(this->oclBufferBatchSkipped->getHostBlock<cl_uint*>()) = 0;

	this->oclBufferBatchTimesteps->createBuffer();
	this->oclBufferBatchSuccessful->createBuffer();
	this->oclBufferBatchSkipped->createBuffer();

	// --
	// Domain and cell state data
	// --

	void* pCellStates = NULL;
	void* pBedElevations = NULL;
	void* pManningValues = NULL;
	void* pBoundaryValues = NULL;
	void* pPoleniValues = NULL;
	void* pOpt_zxmax = NULL;
	void* pOpt_cx = NULL;
	void* pOpt_zymax = NULL;
	void* pOpt_cy = NULL;
	void* pCouplingIDs = NULL;
	void* pCouplingValues = NULL;

	pDomain->createStoreBuffers(
		&pCellStates,
		&pBedElevations,
		&pManningValues,
		&pBoundaryValues,
		&pPoleniValues,
		&pOpt_zxmax,
		&pOpt_cx,
		&pOpt_zymax,
		&pOpt_cy,
		&pCouplingIDs,
		&pCouplingValues,
		ucFloatSize
	);

	this->oclBufferCellStates = new COCLBuffer("Cell states", oclModel, false, true);
	this->oclBufferCellStatesAlt = new COCLBuffer("Cell states (alternate)", oclModel, false, true);
	this->oclBufferCellManning = new COCLBuffer("Manning coefficients", oclModel, true, true);
	if (this->bUseOptimizedBoundary == false) {
		this->oclBufferCellBoundary = new COCLBuffer("Boundary Values", oclModel, false, true);
	}
	else {
		this->oclBufferCouplingIDs = new COCLBuffer("Coupling IDs", oclModel, true, true);
		this->oclBufferCouplingValues = new COCLBuffer("Coupling Values", oclModel, false, true);
	}
	this->oclBufferUsePoleni = new COCLBuffer("Poleni Booleans", oclModel, true, true);
	this->oclBuffer_opt_zxmax = new COCLBuffer("opt_zxmax Values", oclModel, true, true);
	this->oclBuffer_opt_cx = new COCLBuffer("opt_cx Values", oclModel, true, true);
	this->oclBuffer_opt_zymax = new COCLBuffer("opt_zymax Values", oclModel, true, true);
	this->oclBuffer_opt_cy = new COCLBuffer("opt_cy Values", oclModel, true, true);
	this->oclBufferCellBed = new COCLBuffer("Bed elevations", oclModel, true, true);

	this->oclBufferCellStates->setPointer(pCellStates, ucFloatSize * 4 * pDomain->getCellCount());
	this->oclBufferCellStatesAlt->setPointer(pCellStates, ucFloatSize * 4 * pDomain->getCellCount());
	this->oclBufferCellManning->setPointer(pManningValues, ucFloatSize * pDomain->getCellCount());
	if (this->bUseOptimizedBoundary == false) {
		this->oclBufferCellBoundary->setPointer(pBoundaryValues, ucFloatSize * pDomain->getCellCount());
	}
	else {
		this->oclBufferCouplingIDs->setPointer(pCouplingIDs, sizeof(cl_ulong) * this->ulCouplingArraySize);
		this->oclBufferCouplingValues->setPointer(pCouplingValues, ucFloatSize * this->ulCouplingArraySize);
	}
	this->oclBufferUsePoleni->setPointer(pPoleniValues, sizeof(sUsePoleni) * pDomain->getCellCount());
	this->oclBuffer_opt_zxmax->setPointer(pOpt_zxmax, ucFloatSize * pDomain->getCellCount());
	this->oclBuffer_opt_cx->setPointer(pOpt_cx, ucFloatSize * pDomain->getCellCount());
	this->oclBuffer_opt_zymax->setPointer(pOpt_zymax, ucFloatSize * pDomain->getCellCount());
	this->oclBuffer_opt_cy->setPointer(pOpt_cy, ucFloatSize * pDomain->getCellCount());
	this->oclBufferCellBed->setPointer(pBedElevations, ucFloatSize * pDomain->getCellCount());

	this->oclBufferCellStates->createBuffer();
	this->oclBufferCellStatesAlt->createBuffer();
	this->oclBufferCellManning->createBuffer();
	if (this->bUseOptimizedBoundary == false) {
		this->oclBufferCellBoundary->createBuffer();
	}
	else {
		this->oclBufferCouplingIDs->createBuffer();
		this->oclBufferCouplingValues->createBuffer();
	}
	this->oclBufferUsePoleni->createBuffer();
	this->oclBuffer_opt_zxmax->createBuffer();
	this->oclBuffer_opt_cx->createBuffer();
	this->oclBuffer_opt_zymax->createBuffer();
	this->oclBuffer_opt_cy->createBuffer();
	this->oclBufferCellBed->createBuffer();

	// --
	// Timesteps and current simulation time
	// --

	this->oclBufferTimestep = new COCLBuffer("Timestep", oclModel, false, true, ucFloatSize, true);
	this->oclBufferTime = new COCLBuffer("Time", oclModel, false, true, ucFloatSize, true);
	this->oclBufferTimeTarget = new COCLBuffer("Target time (sync)", oclModel, false, true, ucFloatSize, true);
	this->oclBufferTimestepMovAvg = new COCLBuffer("Timestep Moving Average", oclModel, false, true, ucFloatSize, true);
	this->oclBufferTimeHydrological = new COCLBuffer("Time (hydrological)", oclModel, false, true, ucFloatSize, true);

	// We duplicate the time and timestep variables if we're using single-precision so we have copies in both formats
	if (cModel->getFloatPrecision() == model::floatPrecision::kSingle)
	{
		*(this->oclBufferTime->getHostBlock<float*>()) = static_cast<cl_float>(this->dCurrentTime);
		*(this->oclBufferTimestep->getHostBlock<float*>()) = static_cast<cl_float>(this->dCurrentTimestep);
		*(this->oclBufferTimestepMovAvg->getHostBlock<float*>()) = 0.0f;
		*(this->oclBufferTimeHydrological->getHostBlock<float*>()) = 0.0f;
		*(this->oclBufferTimeTarget->getHostBlock<float*>()) = 0.0f;
	}
	else {
		*(this->oclBufferTime->getHostBlock<double*>()) = this->dCurrentTime;
		*(this->oclBufferTimestep->getHostBlock<double*>()) = this->dCurrentTimestep;
		*(this->oclBufferTimestepMovAvg->getHostBlock<double*>()) = 0.0;
		*(this->oclBufferTimeHydrological->getHostBlock<double*>()) = 0.0;
		*(this->oclBufferTimeTarget->getHostBlock<double*>()) = 0.0;
	}

	this->oclBufferTimestep->createBuffer();
	this->oclBufferTime->createBuffer();
	this->oclBufferTimestepMovAvg->createBuffer();
	this->oclBufferTimeHydrological->createBuffer();
	this->oclBufferTimeTarget->createBuffer();

	// --
	// Timestep reduction global array
	// --

	this->oclBufferTimestepReduction = new COCLBuffer("Timestep reduction scratch", oclModel, false, true, this->ulReductionGlobalSize * ucFloatSize, true);
	this->oclBufferTimestepReduction->createBuffer();

	// TODO: Check buffers were created successfully before returning a positive response

	// VISUALISER STUFF
	// TODO: Make this a bit better, put it somewhere else, etc.
	this->oclBufferCellStates->setCallbackRead(CModel::visualiserCallback);

}

//Create general kernels used by numerous schemes with the compiled program
void CSchemeGodunov::prepareGeneralKernels()
{

	// --
	// Timestep and simulation advancing
	// --

	oclKernelTimeAdvance = oclModel->getKernel("tst_Advance_Normal");
	oclKernelResetCounters = oclModel->getKernel("tst_ResetCounters");
	oclKernelTimestepReduction = oclModel->getKernel("tst_Reduce");

	oclKernelTimeAdvance->setGroupSize(1, 1, 1);
	oclKernelTimeAdvance->setGlobalSize(1, 1, 1);
	oclKernelResetCounters->setGroupSize(1, 1, 1);
	oclKernelResetCounters->setGlobalSize(1, 1, 1);
	oclKernelTimestepReduction->setGroupSize(this->ulReductionWorkgroupSize);
	oclKernelTimestepReduction->setGlobalSize(this->ulReductionGlobalSize);

	COCLBuffer* aryArgsTimeAdvance[] = { oclBufferTime, oclBufferTimestep, oclBufferTimestepMovAvg, oclBufferTimeHydrological, oclBufferTimestepReduction, oclBufferCellStates, oclBufferCellBed, oclBufferTimeTarget, oclBufferBatchTimesteps, oclBufferBatchSuccessful, oclBufferBatchSkipped };
	COCLBuffer* aryArgsTimestepUpdate[] = { oclBufferTime, oclBufferTimestep, oclBufferTimestepReduction, oclBufferTimeTarget, oclBufferBatchTimesteps };
	COCLBuffer* aryArgsTimeReduction[] = { oclBufferCellStates, oclBufferCellBed, oclBufferTimestepReduction };
	COCLBuffer* aryArgsResetCounters[] = { oclBufferBatchTimesteps, oclBufferBatchSuccessful, oclBufferBatchSkipped };

	oclKernelTimeAdvance->assignArguments(aryArgsTimeAdvance);
	oclKernelResetCounters->assignArguments(aryArgsResetCounters);
	oclKernelTimestepReduction->assignArguments(aryArgsTimeReduction);

	// --
	// Boundary Kernel
	// --
	if (this->bUseOptimizedBoundary == false) {
		// Normal Boundary for rain
		oclKernelBoundary = oclModel->getKernel("bdy_Promaides");
		oclKernelBoundary->setGroupSize(this->ulNonCachedWorkgroupSizeX, this->ulNonCachedWorkgroupSizeY);
		oclKernelBoundary->setGlobalSize(this->ulNonCachedGlobalSizeX, this->ulNonCachedGlobalSizeY);

		// TODO: Alaa: remove the hydrological buffer and code
		COCLBuffer* aryArgsBdy[] = { oclBufferCellBoundary, oclBufferTimestep, oclBufferTimeHydrological ,oclBufferCellStates, oclBufferCellBed };

		oclKernelBoundary->assignArguments(aryArgsBdy);

	}
	else {
		// Coupling only Bound
		oclKernelBoundary = oclModel->getKernel("bdy_Promaides_by_id");
		oclKernelBoundary->setGroupSize(8);
		oclKernelBoundary->setGlobalSize(8 * static_cast<unsigned long>(ceil(static_cast<double>(this->ulCouplingArraySize) / 8.0)));

		COCLBuffer* aryArgsBdy[] = { oclBufferCouplingIDs, oclBufferCouplingValues, oclBufferTimestep ,oclBufferCellStates, oclBufferCellBed };

		oclKernelBoundary->assignArguments(aryArgsBdy);
	}


	// --
	// Friction Kernel
	// --

	oclKernelFriction = oclModel->getKernel("per_Friction");
	oclKernelFriction->setGroupSize(this->ulNonCachedWorkgroupSizeX, this->ulNonCachedWorkgroupSizeY);
	oclKernelFriction->setGlobalSize(this->ulNonCachedGlobalSizeX, this->ulNonCachedGlobalSizeY);

	COCLBuffer* aryArgsFriction[] = { oclBufferTimestep, oclBufferCellStates, oclBufferCellBed, oclBufferCellManning, oclBufferTime };
	oclKernelFriction->assignArguments(aryArgsFriction);

}

//Create kernels using the compiled program
void CSchemeGodunov::prepare1OKernels()
{
	// --
	// Godunov-type scheme kernels
	// --

	if (this->ucConfiguration == model::schemeConfigurations::godunovType::kCacheNone)
	{
		oclKernelFullTimestep = oclModel->getKernel("gts_cacheDisabled");
		oclKernelFullTimestep->setGroupSize(this->ulNonCachedWorkgroupSizeX, this->ulNonCachedWorkgroupSizeY);
		oclKernelFullTimestep->setGlobalSize(this->ulNonCachedGlobalSizeX, this->ulNonCachedGlobalSizeY);
		COCLBuffer* aryArgsFullTimestep[] = { oclBufferTimestep, oclBufferCellBed, oclBufferCellStates, oclBufferCellStatesAlt, oclBufferCellManning, oclBufferUsePoleni, oclBuffer_opt_zxmax, oclBuffer_opt_zymax };
		oclKernelFullTimestep->assignArguments(aryArgsFullTimestep);
	}
	if (this->ucConfiguration == model::schemeConfigurations::godunovType::kCacheEnabled)
	{
		oclKernelFullTimestep = oclModel->getKernel("gts_cacheEnabled");
		oclKernelFullTimestep->setGroupSize(this->ulCachedWorkgroupSizeX, this->ulCachedWorkgroupSizeY);
		oclKernelFullTimestep->setGlobalSize(this->ulCachedGlobalSizeX, this->ulCachedGlobalSizeY);
		COCLBuffer* aryArgsFullTimestep[] = { oclBufferTimestep, oclBufferCellBed, oclBufferCellStates, oclBufferCellStatesAlt, oclBufferCellManning };
		oclKernelFullTimestep->assignArguments(aryArgsFullTimestep);
	}

}

//Release all OpenCL resources consumed using the OpenCL methods
void CSchemeGodunov::releaseResources()
{
	this->bReady = false;

	this->release1OResources();
}

//Release all OpenCL resources consumed using the OpenCL methods
void CSchemeGodunov::release1OResources()
{
	this->bReady = false;

	if (this->oclModel != NULL)							delete oclModel;
	if (this->oclKernelFullTimestep != NULL)				delete oclKernelFullTimestep;
	if (this->oclKernelBoundary != NULL)					delete oclKernelBoundary;
	if (this->oclKernelFriction != NULL)					delete oclKernelFriction;
	if (this->oclKernelTimestepReduction != NULL)			delete oclKernelTimestepReduction;
	if (this->oclKernelTimeAdvance != NULL)				delete oclKernelTimeAdvance;
	if (this->oclKernelResetCounters != NULL)				delete oclKernelResetCounters;
	if (this->oclBufferCellStates != NULL)				delete oclBufferCellStates;
	if (this->oclBufferCellStatesAlt != NULL)				delete oclBufferCellStatesAlt;
	if (this->oclBufferCellManning != NULL)				delete oclBufferCellManning;
	if (this->oclBufferCellBoundary != NULL)				delete oclBufferCellBoundary;
	if (this->oclBufferCouplingIDs != NULL)				delete oclBufferCouplingIDs;
	if (this->oclBufferCouplingValues != NULL)			delete oclBufferCouplingValues;
	if (this->oclBufferUsePoleni != NULL)					delete oclBufferUsePoleni;
	if (this->oclBuffer_opt_zxmax != NULL)				delete oclBuffer_opt_zxmax;
	if (this->oclBuffer_opt_cx != NULL)					delete oclBuffer_opt_cx;
	if (this->oclBuffer_opt_zymax != NULL)				delete oclBuffer_opt_zymax;
	if (this->oclBuffer_opt_cy != NULL)					delete oclBuffer_opt_cy;
	if (this->oclBufferCellBed != NULL)					delete oclBufferCellBed;
	if (this->oclBufferTimestep != NULL)				delete oclBufferTimestep;
	if (this->oclBufferTimestepReduction != NULL)		delete oclBufferTimestepReduction;
	if (this->oclBufferTime != NULL)					delete oclBufferTime;
	if (this->oclBufferTimeTarget != NULL)				delete oclBufferTimeTarget;
	if (this->oclBufferTimestepMovAvg != NULL)			delete oclBufferTimestepMovAvg;
	if (this->oclBufferTimeHydrological != NULL)		delete oclBufferTimeHydrological;
	if (this->oclBufferBatchTimesteps != NULL)			delete oclBufferBatchTimesteps;
	if (this->oclBufferBatchSuccessful != NULL)			delete oclBufferBatchSuccessful;
	if (this->oclBufferBatchSkipped != NULL)			delete oclBufferBatchSkipped;

	oclModel = NULL;
	oclKernelFullTimestep = NULL;
	oclKernelBoundary = NULL;
	oclKernelFriction = NULL;
	oclKernelTimestepReduction = NULL;
	oclKernelTimeAdvance = NULL;
	oclKernelResetCounters = NULL;
	oclBufferCellStates = NULL;
	oclBufferCellStatesAlt = NULL;
	oclBufferCellManning = NULL;
	oclBufferCellBoundary = NULL;
	oclBufferCouplingIDs = NULL;
	oclBufferCouplingValues = NULL;
	oclBufferUsePoleni = NULL;
	oclBuffer_opt_zxmax = NULL;
	oclBuffer_opt_cx = NULL;
	oclBuffer_opt_zymax = NULL;
	oclBuffer_opt_cy = NULL;
	oclBufferCellBed = NULL;
	oclBufferTimestep = NULL;
	oclBufferTimestepReduction = NULL;
	oclBufferTime = NULL;
	oclBufferTimeTarget = NULL;
	oclBufferTimestepMovAvg = NULL;
	oclBufferTimeHydrological = NULL;
	oclBufferBatchTimesteps = NULL;
	oclBufferBatchSuccessful = NULL;
	oclBufferBatchSkipped = NULL;

}

//Prepares the simulation
void	CSchemeGodunov::prepareSimulation()
{

	// Initial volume in the domain
	model::log->logInfo("Initial domain volume: " + std::to_string(this->pDomain->getVolume()) + " m3");

	// Copy the initial conditions
	model::log->logInfo("Copying domain data to device...");
	oclBufferCellStates->queueWriteAll();
	oclBufferCellStatesAlt->queueWriteAll();
	oclBufferCellBed->queueWriteAll();
	oclBufferCellManning->queueWriteAll();
	if (this->bUseOptimizedBoundary == false) {
		oclBufferCellBoundary->queueWriteAll();
	}
	else {
		oclBufferCouplingIDs->queueWriteAll();
		oclBufferCouplingValues->queueWriteAll();
	}
	oclBufferUsePoleni->queueWriteAll();
	oclBuffer_opt_zxmax->queueWriteAll();
	oclBuffer_opt_cx->queueWriteAll();
	oclBuffer_opt_zymax->queueWriteAll();
	oclBuffer_opt_cy->queueWriteAll();
	oclBufferTime->queueWriteAll();
	oclBufferTimestep->queueWriteAll();
	oclBufferTimestepMovAvg->queueWriteAll();
	oclBufferTimeHydrological->queueWriteAll();
	this->pDomain->getDevice()->blockUntilFinished();

	// Sort out memory alternation
	bUseAlternateKernel = false;
	bImportBoundaries = false;

	// Zero counters
	ulCurrentCellsCalculated = 0;
	uiIterationsSinceTargetChanged = 0;

	// States
	bRunning = false;
	bThreadRunning = false;
	bThreadTerminated = false;
}

//Runs the actual simulation until completion or error
void	CSchemeGodunov::runSimulation(double dTargetTime_input)
{
	// Wait for current work to finish
	if (this->bRunning || this->pDomain->getDevice()->isBusy())
		return;

	// Has the target time changed?
	if (this->dTargetTime != dTargetTime_input) {
		this->dTargetTime = dTargetTime_input;
		this->bUpdateTargetTime = true;
	}

	// If we've already hit our sync time but the other domains haven't, don't bother scheduling any work
	if (this->dCurrentTime > dTargetTime) {
		model::log->logInfo("Current time:   " + toStringExact(dCurrentTime) + ", Target time:  " + toStringExact(dTargetTime));
		model::doError("Simulation has exceeded target time",
			model::errorCodes::kLevelWarning,
			"void	CSchemeGodunov::runSimulation(double dTargetTime, double dRealTime)",
			"Try working with a different device."
		);
		return;
	}

	this->bRunning = true;
	this->runBatchThread();
}

// Create a new thread to run this batch
void CSchemeGodunov::runBatchThread()
{
	if (this->bThreadRunning)
		return;

	this->bThreadRunning = true;
	this->bThreadTerminated = false;

	#ifdef PLATFORM_WIN
	HANDLE hThread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)CSchemeGodunov::Threaded_runBatchLaunch,
		this,
		0,
		NULL
	);
	CloseHandle(hThread);
	#endif
	#ifdef PLATFORM_UNIX
	pthread_t tid;
	int result = pthread_create(&tid, 0, CSchemeGodunov::Threaded_runBatchLaunch, this);
	if (result == 0)
		pthread_detach(tid);
	#endif

}

#ifdef PLATFORM_WIN
DWORD CSchemeGodunov::Threaded_runBatchLaunch(LPVOID param)
{
	CSchemeGodunov* pScheme = static_cast<CSchemeGodunov*>(param);
	pScheme->Threaded_runBatch();
	return 0;
}
#endif
#ifdef PLATFORM_UNIX
void* CSchemeGodunov::Threaded_runBatchLaunch(void* param)
{
	CSchemeGodunov* pScheme = static_cast<CSchemeGodunov*>(param);
	pScheme->Threaded_runBatch();
	return 0;
}
#endif

//Schedule a batch-load of work to run on the device and block until complete. Runs in its own thread.
void CSchemeGodunov::Threaded_runBatch()
{
	// Keep the thread in existence because of the overhead
	// associated with creating a thread.
	while (this->bThreadRunning)
	{
		try{
			if (this->bSimulationSlow || this->bSolverThreadStopped) {
				return;
			}

			// Are we expected to run?
			if (!this->bRunning || this->pDomain->getDevice()->isBusy()){
				if (this->pDomain->getDevice()->isBusy()){
					this->pDomain->getDevice()->blockUntilFinished();
				}
				continue;
			}
			this->cModel->profiler->profile("BatchRunning", CProfiler::profilerFlags::START_PROFILING);

			// Have we been asked to update the target time?
			if (this->bUpdateTargetTime){
				this->bUpdateTargetTime = false;
				this->uiIterationsSinceTargetChanged = 0;

				if (cModel->getFloatPrecision() == model::floatPrecision::kSingle) {
					*(oclBufferTimeTarget->getHostBlock<float*>()) = static_cast<cl_float>(this->dTargetTime);
				}else {
					*(oclBufferTimeTarget->getHostBlock<double*>()) = this->dTargetTime;
				}
				oclBufferTimeTarget->queueWriteAll();


				if (dCurrentTime + dCurrentTimestep > dTargetTime) {
					this->dCurrentTimestep = dTargetTime - dCurrentTime;
					model::log->logWarning("Override Timestep Requested");
				}

			}

			// Have we been asked to import new data?
			if (this->bImportBoundaries) {
				this->bImportBoundaries = false;
				this->uiIterationsSinceTargetChanged = 0;
				oclKernelResetCounters->scheduleExecution();

				if (this->bUseOptimizedBoundary == false) {
					this->oclBufferCellBoundary->queueWriteAll();
				}else {
					this->oclBufferCouplingValues->queueWriteAll();
				}

			}

			// Can only schedule one iteration before we need to sync timesteps
			int uiQueueAmount = 1;
			int estimatedQ;

			if (dCurrentTimestepMovAvg > 0.001 && dTargetTime - dCurrentTime>0.01) {
				estimatedQ = (dTargetTime - dCurrentTime) / dCurrentTimestepMovAvg;
				uiQueueAmount = max(min(300, estimatedQ),1);
			}
			else {
				if (dCurrentTime > 0.1 && dCurrentTimestepMovAvg < 0.001) {
					this->outputAllFloodplainDataToVtk();	// Report Floodplain in a vtk file
					this->bSimulationSlow = true;			// Declare Slow Simulation
					return;									// Don't go beyond
				}
				uiQueueAmount = 1;
			}

			// Schedule a batch-load of work for the device
			if (this->dCurrentTime < dTargetTime - 1e-8) {
				oclKernelResetCounters->scheduleExecution();
				for (unsigned int i = 0; i < uiQueueAmount; i++) {

					this->scheduleIteration();
					uiIterationsSinceTargetChanged++;
					ulCurrentCellsCalculated += this->pDomain->getCellCount();
					bUseAlternateKernel = !bUseAlternateKernel;
				}

			}

			// Schedule reading data back. We always need the timestep but we might not need the other details always...
			oclBufferTimestep->queueReadAll();
			oclBufferTimestepMovAvg->queueReadAll();
			oclBufferTime->queueReadAll();
			oclBufferBatchSkipped->queueReadAll();
			oclBufferBatchSuccessful->queueReadAll();
			oclBufferBatchTimesteps->queueReadAll();
			this->pDomain->getDevice()->blockUntilFinished();

			this->readKeyStatistics();

			dAvgTimestep = (dAvgTimestep * uiIterationsTotal + dCurrentTimestep * uiIterationsSinceTargetChanged) / (uiIterationsTotal + uiIterationsSinceTargetChanged);
			uiIterationsTotal += uiIterationsSinceTargetChanged;
			uiSuccessfulIterationsTotal += uiBatchSuccessful;
			uiSkippedIterationsTotal += uiBatchSkipped;

			// Wait until further work is scheduled
			this->bRunning = false;

			this->cModel->profiler->profile("BatchRunning", CProfiler::profilerFlags::END_PROFILING);


		}
		catch (const std::exception& e)
		{
			std::cerr << "Exception caught: " << e.what() << std::endl; // Log Error will throw an error when not in main thread
			this->bSolverThreadStopped = true;							// Declare Error in Thread
			return;														// Don't go beyond
		}
	}

	this->bThreadTerminated = true;
}

//Runs the actual simulation until completion or error
void	CSchemeGodunov::scheduleIteration() {
	//std::cout << "start iteration" << std::endl;
	COCLBuffer* bufferSrc = NULL;
	COCLBuffer* bufferDst = NULL;

	if (!bUseAlternateKernel) {
		bufferSrc = oclBufferCellStates;
		bufferDst = oclBufferCellStatesAlt;
	}
	else {
		bufferSrc = oclBufferCellStatesAlt;
		bufferDst = oclBufferCellStates;
	}



	oclKernelFullTimestep->assignArgument(2, bufferSrc);
	oclKernelFullTimestep->assignArgument(3, bufferDst);
	oclKernelFriction->assignArgument(1, bufferDst);
	oclKernelBoundary->assignArgument(3, bufferDst);
	oclKernelTimestepReduction->assignArgument(0, bufferDst);

	profiless
	oclKernelFullTimestep->scheduleExecution();
	profilese


	if (this->bFrictionEffects && !this->bFrictionInFluxKernel) {
		oclKernelFriction->scheduleExecution();
	}

	profilebs
	oclKernelBoundary->scheduleExecution();
	profilebe

	profilers
	if (this->bDynamicTimestep) { oclKernelTimestepReduction->scheduleExecution(); }
	profilere

	profilets
	oclKernelTimeAdvance->scheduleExecution();
	profilete

}

// Read back all of the domain data
void CSchemeGodunov::readDomainAll()
{

	this->cModel->profiler->profile("readDomainAll", CProfiler::profilerFlags::START_PROFILING);
	if (bUseAlternateKernel)
	{
		oclBufferCellStatesAlt->queueReadAll();
	}
	else {
		oclBufferCellStates->queueReadAll();
	}
	this->cModel->profiler->profile("readDomainAll", CProfiler::profilerFlags::END_PROFILING, this->pDomain->getDevice());
}

//Clean-up temporary resources consumed during the simulation
void	CSchemeGodunov::cleanupSimulation()
{
	// Kill the worker thread
	bRunning = false;
	bThreadRunning = false;

	// Wait for the thread to terminate before returning
	while (!bThreadTerminated && bThreadRunning) {}
}

//Read back domain data for the synchronisation zones only
void CSchemeGodunov::importBoundaries()
{
	this->bImportBoundaries = true;
}

//Fetch the pointer to the next cell source buffer
COCLBuffer* CSchemeGodunov::getNextCellSourceBuffer()
{
	if (bUseAlternateKernel)
	{
		return oclBufferCellStatesAlt;
	}
	else {
		return oclBufferCellStates;
	}
}

//Set the target sync time
void CSchemeGodunov::setTargetTime(double dTime)
{
	if (dTime == this->dTargetTime)
		return;

	this->dTargetTime = dTime;
	this->bUpdateTargetTime = true;
}

//Get the batch average timestep
double CSchemeGodunov::getAverageTimestep()
{
	if (uiBatchSuccessful < 1) return 0.0;
	return dBatchTimesteps / uiBatchSuccessful;
}

//Fetch key details back to the right places in memory
void	CSchemeGodunov::readKeyStatistics()
{
	cl_uint uiLastBatchSuccessful = uiBatchSuccessful;

	// Pull key data back from our buffers to the scheme class
	if (cModel->getFloatPrecision() == model::floatPrecision::kSingle )
	{
		dCurrentTimestep = static_cast<cl_double>( *( oclBufferTimestep->getHostBlock<float*>() ) );
		dCurrentTimestepMovAvg = static_cast<cl_double>( *( oclBufferTimestepMovAvg->getHostBlock<float*>() ) );
		dCurrentTime = static_cast<cl_double>(*(oclBufferTime->getHostBlock<float*>()));
		dBatchTimesteps = static_cast<cl_double>( *( oclBufferBatchTimesteps->getHostBlock<float*>() ) );
	} else {
		dCurrentTimestep = *( oclBufferTimestep->getHostBlock<double*>() );
		dCurrentTimestepMovAvg = *(oclBufferTimestepMovAvg->getHostBlock<double*>() );
		dCurrentTime = *(oclBufferTime->getHostBlock<double*>());
		dBatchTimesteps = *( oclBufferBatchTimesteps->getHostBlock<double*>() );
	}
	uiBatchSuccessful = *( oclBufferBatchSuccessful->getHostBlock<cl_uint*>() );
	uiBatchSkipped	  = *( oclBufferBatchSkipped->getHostBlock<cl_uint*>() );
	uiBatchRate = uiBatchSuccessful > uiLastBatchSuccessful ? (uiBatchSuccessful - uiLastBatchSuccessful) : 1;
}

void CSchemeGodunov::setDebugger(unsigned int debugX, unsigned int debugY) {
	bDebugOutput = true;
	uiDebugCellX = debugX;
	uiDebugCellY = debugY;
}

void CSchemeGodunov::dumpMemory() {
	COCLBuffer* COCLBuffers[19] = { oclBufferCellStates,oclBufferCellManning,oclBufferCellBoundary,oclBufferUsePoleni,oclBuffer_opt_zxmax,
	oclBuffer_opt_cx,oclBuffer_opt_zymax,oclBuffer_opt_cy,oclBufferCellBed,oclBufferTimestep,oclBufferTimestepReduction,oclBufferTime,
	oclBufferTimeTarget,oclBufferTimeHydrological,oclBufferCouplingIDs,oclBufferCouplingValues,
	oclBufferBatchTimesteps,oclBufferBatchSuccessful,oclBufferBatchSkipped };

	for (COCLBuffer* currentBuffer : COCLBuffers) {
		if (currentBuffer != NULL) {
			model::log->logDebug("Reading buffer: " + currentBuffer->getName());
			currentBuffer->queueReadAll();
		}
	}

	}

void CSchemeGodunov::findFastestCells(DomainCell* first, DomainCell* second, DomainCell* third) {

	double* opt_h_gpu = new double[this->getDomain()->getCellCount()];
	double* opt_v_x_gpu = new double[this->getDomain()->getCellCount()];
	double* opt_v_y_gpu = new double[this->getDomain()->getCellCount()];
	this->getDomain()->readBuffers_h_vx_vy(opt_h_gpu, opt_v_x_gpu, opt_v_y_gpu);

	unsigned long cellId1 = 0;
	double cellSpeed1 = 0;
	unsigned long cellId2 = 0;
	double cellSpeed2 = 0;
	unsigned long cellId3 = 0;
	double cellSpeed3 = 0;
	double cellSpeed, dDepth, dVelX, dVelY;

	for (unsigned long cellId = 0; cellId < this->getDomain()->getCellCount(); cellId++){
		if (this->getDomain()->getBedElevation(cellId) != -9999.0) {
			dDepth = opt_h_gpu[cellId];

			if (this->getSchemeType() == model::schemeTypes::kDiffusiveGPU || this->getSchemeType() == model::schemeTypes::kInertialGPU) {
				dVelX = sqrt(9.81 * dDepth);
				dVelY = sqrt(9.81 * dDepth);

				cellSpeed = (dVelX < dVelY) ? dVelY : dVelX;
			}
			else if (this->getSchemeType() == model::schemeTypes::kGodunovGPU || this->getSchemeType() == model::schemeTypes::kMUSCLGPU) {
				dVelX = opt_v_x_gpu[cellId] / dDepth;
				dVelY = opt_v_y_gpu[cellId] / dDepth;
				if (dVelX < 0.0) dVelX = -dVelX;
				if (dVelY < 0.0) dVelY = -dVelY;

				dVelX += sqrt(9.81 * dDepth);
				dVelY += sqrt(9.81 * dDepth);
			}
			cellSpeed = (dVelX < dVelY) ? dVelY : dVelX;

			if (cellSpeed >= cellSpeed1) {
				std::swap(cellId2, cellId3);
				std::swap(cellSpeed2, cellSpeed3);
				std::swap(cellId1, cellId2);
				std::swap(cellSpeed1, cellSpeed2);
				cellId1 = cellId;
				cellSpeed1 = cellSpeed;
				continue;
			}

			if (cellSpeed >= cellSpeed2) {
				std::swap(cellId2, cellId3);
				std::swap(cellSpeed2, cellSpeed3);
				cellId2 = cellId;
				cellSpeed2 = cellSpeed;
				continue;
			}

			if (cellSpeed >= cellSpeed3) {
				cellId3 = cellId;
				cellSpeed3 = cellSpeed;
				continue;
			}
		}

	}
	first->ulCellId = cellId1;
	first->dDepth = opt_h_gpu[cellId1];
	first->dElevation = this->getDomain()->getBedElevation(cellId1);
	first->dV_x = opt_v_x_gpu[cellId1];
	first->dV_y = opt_v_y_gpu[cellId1];
	first->dCalculatedVelocity = cellSpeed1;
	first->dExpectedTimeStep = this->getDomain()->getCellResolutionX() > this->getDomain()->getCellResolutionY() ? this->getDomain()->getCellResolutionX()/ cellSpeed1 : this->getDomain()->getCellResolutionY() / cellSpeed1;

	second->ulCellId = cellId2;
	second->dDepth = opt_h_gpu[cellId2];
	second->dElevation = this->getDomain()->getBedElevation(cellId2);
	second->dV_x = opt_v_x_gpu[cellId2];
	second->dV_y = opt_v_y_gpu[cellId2];
	second->dCalculatedVelocity = cellSpeed2;
	second->dExpectedTimeStep = this->getDomain()->getCellResolutionX() > this->getDomain()->getCellResolutionY() ? this->getDomain()->getCellResolutionX() / cellSpeed2 : this->getDomain()->getCellResolutionY() / cellSpeed2;

	third->ulCellId = cellId3;
	third->dDepth = opt_h_gpu[cellId3];
	third->dElevation = this->getDomain()->getBedElevation(cellId3);
	third->dV_x = opt_v_x_gpu[cellId3];
	third->dV_y = opt_v_y_gpu[cellId3];
	third->dCalculatedVelocity = cellSpeed3;
	third->dExpectedTimeStep = this->getDomain()->getCellResolutionX() > this->getDomain()->getCellResolutionY() ? this->getDomain()->getCellResolutionX() / cellSpeed3 : this->getDomain()->getCellResolutionY() / cellSpeed3;


	delete[] opt_h_gpu;
	delete[] opt_v_x_gpu;
	delete[] opt_v_y_gpu;
}

void CSchemeGodunov::outputAllFloodplainDataToVtk() {

	std::string path = "fp_" + this->getDomain()->getName() + "_" + std::to_string(dCurrentTime) + ".vtk";

	std::cout << "Floodplain is being exported to [" << path << "]" << std::endl;
	double* opt_h_gpu = new double[this->getDomain()->getCellCount()];
	double* opt_v_x_gpu = new double[this->getDomain()->getCellCount()];
	double* opt_v_y_gpu = new double[this->getDomain()->getCellCount()];
	this->getDomain()->readBuffers_h_vx_vy(opt_h_gpu, opt_v_x_gpu, opt_v_y_gpu);
	
	double* opt_z = new double[this->getDomain()->getCellCount()];
	double* opt_zx_max = new double[this->getDomain()->getCellCount()];
	double* opt_zy_max = new double[this->getDomain()->getCellCount()];
	double* opt_s_gpu = new double[this->getDomain()->getCellCount()];
	for (unsigned long i = 0; i < this->getDomain()->getCellCount(); i++){
		opt_z[i] = this->getDomain()->getBedElevation(i);
		opt_zx_max[i] = this->getDomain()->getZxmax(i);
		opt_zy_max[i] = this->getDomain()->getZymax(i);
		opt_s_gpu[i]  = this->getDomain()->getBedElevation(i) + opt_h_gpu[i];
	}
	
	this->getDomain()->output_to_vtk_file(path, dCurrentTime, "rasterName", this->getDomain()->getCols(), this->getDomain()->getRows(), opt_z, opt_zx_max, opt_zy_max, opt_h_gpu, opt_s_gpu, opt_v_x_gpu, opt_v_y_gpu);
	
	delete[] opt_h_gpu;
	delete[] opt_v_x_gpu;
	delete[] opt_v_y_gpu;
	delete[] opt_z;
	delete[] opt_zx_max;
	delete[] opt_zy_max;
	delete[] opt_s_gpu;
}