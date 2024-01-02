/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


// Includes
#include <cmath>
#include <math.h>
#include "common.h"
#include "CExecutorControlOpenCL.h"
#include "CDomainCartesian.h"
#include "CScheme.h"

using std::min;
using std::max;
CLog* model::log;

//Constructor
CModel::CModel(CLoggingInterface* CLI, bool profilingOn)
{
	this->log = new CLog(CLI);
	model::log = this->log;

	if (profilingOn) {
		model::log->logWarning("Profiler is activated. This will slow down the simulation. Use this for testing only.");
	}
	this->profiler = new CProfiler(profilingOn);

	this->showProgess = false;
	this->mpiManager		= NULL;

	this->dCurrentTime		= 0.0;
	this->dTargetTime		= 0.0;
	this->dSimulationTime	= 60;
	this->dOutputFrequency	= 60;
	this->bDoublePrecision	= true;

	this->pProgressCoords.sX = -1;
	this->pProgressCoords.sY = -1;

	this->execController = NULL;
	CExecutorControlOpenCL* pExecutor = new CExecutorControlOpenCL(model::filters::devices::devicesGPU);
	pExecutor->createDevices();
	this->setExecutor(pExecutor);

	CDomainCartesian* cDomainCartesian = new CDomainCartesian();
	this->domain = cDomainCartesian;
}

//Destructor
CModel::~CModel(void)
{
	this->domain->getScheme()->cleanupSimulation();
	if (this->domain != NULL)
		delete this->domain;
	if ( this->execController != NULL )
		delete this->execController;
	if (this->profiler != NULL)
		delete this->profiler;
	this->log->logInfo("The model engine is completely unloaded.");
	if (this->log != NULL)
		model::log = NULL;
		delete this->log;
}

//Set the type of executor to use for the model
bool CModel::setExecutor(CExecutorControlOpenCL* pExecutorControl)
{
	// TODO: Has the value actually changed?

	// TODO: Delete the old executor controller

	this->execController = pExecutorControl;

	if (!this->execController->isReady())
	{
		model::doError(
			"The executor is not ready. Model cannot continue.",
			model::errorCodes::kLevelFatal,
			"bool CModel::setExecutor(CExecutorControl* pExecutorControl)",
			"Try reseting the model"
		);
		return false;
	}

	return true;
}

//Returns a pointer to the execution controller currently in use
CExecutorControlOpenCL* CModel::getExecutor(void)
{
	return this->execController;
}

//Returns a pointer to the domain class
CDomainCartesian* CModel::getDomain(void)
{
	return this->domain;
}

void CModel::setSelectedDevice(unsigned int id) {
	this->selectedDevice = id;
	this->getExecutor()->selectDevice(id);
	this->getDomain()->setDevice(this->getExecutor()->getDevice(id));
}

unsigned int CModel::getSelectedDevice() {
	return this->selectedDevice;
}

//Log the details for the whole simulation
void CModel::logDetails()
{
	model::log->writeDivide();
	model::log->logInfo("SIMULATION CONFIGURATION");
	model::log->logInfo("  Simulation length:  " + Util::secondsToTime(this->dSimulationTime));
	model::log->logInfo("  Output frequency:   " + Util::secondsToTime(this->dOutputFrequency));
	model::log->logInfo("  Floating-point:     " + (std::string)(this->getFloatPrecision() == model::floatPrecision::kDouble ? "Double-precision" : "Single-precision"));
	model::log->writeDivide();
}

//Execute the model
bool CModel::ValidateAndPrepareModel(void)
{
	model::log->logInfo("Verifying the required data before model run...");

	if (!this->domain)
	{
		model::doError(
			"The domain is not ready.",
			model::errorCodes::kLevelModelStop,
			"bool CModel::ValidateAndPrepareModel(void)",
			"Please restart the program and try again."
		);
		return false;
	}
	if (!this->execController || !this->execController->isReady())
	{
		model::doError(
			"The executor is not ready.",
			model::errorCodes::kLevelModelStop,
			"bool CModel::ValidateAndPrepareModel(void)",
			"Please restart the program and try again."
		);
		return false;
	}

	model::log->logInfo("Verification is complete.");

	model::log->writeDivide();
	model::log->logInfo("Starting a new simulation...");

	this->domain->getScheme()->prepareSimulation();
	//this->runModelMain();

	return true;
}

//Sets the total length of a simulation
void	CModel::setSimulationLength(double dLength)
{
	this->dSimulationTime = dLength;
}

//Gets the total length of a simulation
double	CModel::getSimulationLength()
{
	return this->dSimulationTime;
}

//Set the frequency of outputs
void	CModel::setOutputFrequency(double dFrequency)
{
	this->dOutputFrequency = dFrequency;
}

//Get the frequency of outputs
double	CModel::getOutputFrequency()
{
	return this->dOutputFrequency;
}

//Set floating point precision
void	CModel::setFloatPrecision(unsigned char ucPrecision)
{
	if (!this->getExecutor()->getDevice()->isDoubleCompatible())
		ucPrecision = model::floatPrecision::kSingle;

	this->bDoublePrecision = (ucPrecision == model::floatPrecision::kDouble);
}

//Get floating point precision
model::floatPrecision::floatPrecision CModel::getFloatPrecision()
{
	return (this->bDoublePrecision ? model::floatPrecision::kDouble : model::floatPrecision::kSingle);
}

//Write details of where model execution is currently at
void	CModel::logProgress(CBenchmark::sPerformanceMetrics* sTotalMetrics)
{
	char	cTimeLine[70] = "                                                                    X";
	char	cCellsLine[70] = "                                                                    X";
	char	cTimeLine2[70] = "                                                                    X";
	char	cCells[70] = "                                                                    X";
	char	cProgressLine[70] = "                                                                    X";
	char	cBatchSizeLine[70] = "                                                                    X";
	char	cProgress[57] = "                                                      ";
	char	cProgessNumber[7] = "      ";

	double		  dCurrentTime_output = (this->dCurrentTime > this->dSimulationTime ? this->dSimulationTime : this->dCurrentTime);
	double		  dProgress = dCurrentTime_output / this->dSimulationTime;

	// TODO: These next bits will need modifying for when we have multiple domains
	unsigned long long	ulCurrentCellsCalculated = 0;
	unsigned int		uiBatchSizeMax = 0, uiBatchSizeMin = 9999;
	double				dSmallestTimestep = 9999.0;


	ulCurrentCellsCalculated += domain->getScheme()->getCellsCalculated();

	dataProgress pProgress = domain->getDataProgress();

	if (uiBatchSizeMax < pProgress.uiBatchSize)
		uiBatchSizeMax = pProgress.uiBatchSize;
	if (uiBatchSizeMin > pProgress.uiBatchSize)
		uiBatchSizeMin = pProgress.uiBatchSize;
	if (dSmallestTimestep > pProgress.dBatchTimesteps)
		dSmallestTimestep = pProgress.dBatchTimesteps;

	unsigned long ulRate = static_cast<unsigned long>(ulCurrentCellsCalculated / sTotalMetrics->dSeconds);

	// Make a progress bar
	for (unsigned char i = 0; i <= floor(55.0f * dProgress); i++)
		cProgress[i] = (i >= (floor(55.0f * dProgress) - 1) ? '>' : '=');

	// String padding stuff
	sprintf(cTimeLine, " Simulation time:  %-15sLowest timestep: %15s", Util::secondsToTime(dCurrentTime_output).c_str(), Util::secondsToTime(dSmallestTimestep).c_str());
	sprintf(cCells, "%I64u", ulCurrentCellsCalculated);
	sprintf(cCellsLine, " Cells calculated: %-24s  Rate: %13s/s", cCells, toStringExact(ulRate).c_str());
	sprintf(cTimeLine2, " Processing time:  %-16sEst. remaining: %15s", Util::secondsToTime(sTotalMetrics->dSeconds).c_str(), Util::secondsToTime(min((1.0 - dProgress) * (sTotalMetrics->dSeconds / dProgress), 31536000.0)).c_str());
	sprintf(cBatchSizeLine, " Batch size:       %-16s                                 ", toStringExact(uiBatchSizeMin).c_str());
	sprintf(cProgessNumber, "%.1f%%", dProgress * 100);
	sprintf(cProgressLine, " [%-55s] %7s", cProgress, cProgessNumber);


	model::log->writeDivide();																						// 1
	model::log->logInfo("                                                                  ");	// 2
	model::log->logInfo(" SIMULATION PROGRESS                                              ");	// 3
	model::log->logInfo("                                                                  ");	// 4
	model::log->logInfo(std::string(cTimeLine));	// 5
	model::log->logInfo(std::string(cCellsLine));	// 6
	model::log->logInfo(std::string(cTimeLine2));	// 7
	model::log->logInfo(std::string(cBatchSizeLine));	// 8
	model::log->logInfo("                                                                  ");	// 9
	model::log->logInfo(std::string(cProgressLine));	// 10
	model::log->logInfo("                                                                  ");	// 11

	model::log->logInfo("             +----------+----------------+------------+----------+");	// 12
	model::log->logInfo("             |  Device  |  Avg.timestep  | Iterations | Bypassed |");	// 12
	model::log->logInfo("+------------+----------+----------------+------------+----------|");	// 13

	char cDomainLine[70] = "                                                                    X";
	pProgress = domain->getDataProgress();

	// TODO: Give this it's proper name...
	std::string sDeviceName = "REMOTE";

	sDeviceName = domain->getDevice()->getDeviceShortName();

	sprintf(
		cDomainLine,
		"| Domain #%-2s | %8s | %14s | %10s | %8s |",
		"1",
		sDeviceName.c_str(),
		Util::secondsToTime(pProgress.dBatchTimesteps).c_str(),
		std::to_string(pProgress.uiBatchSuccessful).c_str(),
		std::to_string(pProgress.uiBatchSkipped).c_str()
	);

	model::log->logInfo(std::string(cDomainLine));	// ++

	model::log->logInfo("+------------+----------+----------------+------------+----------+");	// 14
	model::log->writeDivide();																						// 15

	//this->pProgressCoords = Util::getCursorPosition();
	//if (this->dCurrentTime < this->dSimulationTime) 
	//{
	//	this->pProgressCoords.sY = max(0, this->pProgressCoords.sY - (16 + (cl_int)domains->getDomainCount()));
	//	Util::setCursorPosition(this->pProgressCoords);
	//}
}

//Update the visualization by sending domain data over to the relevant component
void CModel::visualiserUpdate()
{
	if (this->dCurrentTime >= this->dSimulationTime - 1E-5)
		return;

}

//Memory read should have completed, so provided the simulation isn't over - read it back again
void CL_CALLBACK CModel::visualiserCallback(cl_event clEvent, cl_int iStatus, void* vData)
{
	model::CallBackData* callBackData = (model::CallBackData*) vData;
	//TODO: The visualizer won't work because this is commented out
	//callBackData->cModel->visualiserUpdate();
	clReleaseEvent(clEvent);
}

//Update UI elements (progress bars etc.)
void	CModel::runModelUI(CBenchmark::sPerformanceMetrics* sTotalMetrics)
{
	dProcessingTime = sTotalMetrics->dSeconds;
	if (sTotalMetrics->dSeconds - dLastProgressUpdate > 0.85)
	{
		this->logProgress(sTotalMetrics);
		dLastProgressUpdate = sTotalMetrics->dSeconds;
	}
}

//Run the actual simulation, asking each domain and schemes therein in turn etc.
void	CModel::runNext(const double next_time_point)
{
	dTargetTime = next_time_point;

	while (this->dCurrentTime < dTargetTime - 1e-8){
		// Current Time
		dCurrentTime = domain->getScheme()->getCurrentTime();

		// Either we're not ready to sync, or we were still synced from the last run
		if (domain->getScheme()->isRunning() == false && domain->getDevice()->isBusy() == false) {
			domain->getScheme()->runSimulation(dTargetTime);
		}

		if (domain->getScheme()->isSimulationSlow() || domain->getScheme()->isSolverThreadStopped()) {
			break;
		}

		if (this->dCurrentTime > next_time_point) {
			std::cout << "We are off, next point should " << next_time_point << " but we are at " << this->dCurrentTime << std::endl;
		}

	}

}

//Attached the profiler class to the CModel
void CModel::setProfiler(CProfiler* profiler_input) {
	this->profiler = profiler_input;
}


//Attached the logger class to the CModel
void CModel::setUIStatus(bool status) {
	this->showProgess = status;
}

//Raise an error message and deal with it accordingly.
void model::doError(std::string error_reason, unsigned char error_type, std::string error_place, std::string error_help)
{
	model::log->logError(error_reason, error_type, error_place, error_help);

}