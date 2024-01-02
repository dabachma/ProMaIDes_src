/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#include <limits>
#include <stdio.h>
#include <cstring>

#include "common.h"
#include "CModel.h"
#include "CScheme.h"

#include "CExecutorControlOpenCL.h"
#include "CDomainCartesian.h"

//Constructor
CDomainCartesian::CDomainCartesian(void)
{
	// Default values will trigger errors on validation
	this->dCellResolutionX			= std::numeric_limits<double>::quiet_NaN();
	this->dCellResolutionY			= std::numeric_limits<double>::quiet_NaN();
	this->ulRows					= std::numeric_limits<unsigned long>::quiet_NaN();
	this->ulCols					= std::numeric_limits<unsigned long>::quiet_NaN();
	this->domainName				= "";
	this->bUseOptimizedBoundary		= false;
	this->ulCouplingArraySize		= 0;

	this->uiRounding = 6;
	this->sDataProgress.dBatchTimesteps = 0;
	this->sDataProgress.dCurrentTime = 0.0;
	this->sDataProgress.dCurrentTimestep = 0.0;
	this->sDataProgress.uiBatchSize = 0;
	this->sDataProgress.uiBatchSkipped = 0;
	this->sDataProgress.uiBatchSuccessful = 0;

	this->pDevice = NULL;
	this->pScheme = NULL;

	this->dCellStates = NULL;
	this->dBedElevations = NULL;
	this->dManningValues = NULL;
	this->dBoundaryValues = NULL;
	this->dOpt_zxmaxValues = NULL;
	this->dOpt_cxValues = NULL;
	this->dOpt_zymaxValues = NULL;
	this->dOpt_cyValues = NULL;
	this->dCouplingValues = NULL;

	this->fCellStates = NULL;
	this->fBedElevations = NULL;
	this->fManningValues = NULL;
	this->fBoundaryValues = NULL;
	this->fOpt_zxmaxValues = NULL;
	this->fOpt_cxValues = NULL;
	this->fOpt_zymaxValues = NULL;
	this->fOpt_cyValues = NULL;
	this->fCouplingValues = NULL;

	this->bPoleniValues = NULL;
	this->ulCouplingIDs = NULL;
}

//Destructor
CDomainCartesian::~CDomainCartesian(void)
{
	if (this->ucFloatSize == 4)
	{
		if (this->fCellStates != NULL)			delete[] this->fCellStates;
		if (this->fBedElevations != NULL)		delete[] this->fBedElevations;
		if (this->fManningValues != NULL)		delete[] this->fManningValues;
		if (this->fOpt_zxmaxValues != NULL)		delete[] this->fOpt_zxmaxValues;
		if (this->fOpt_cxValues != NULL)			delete[] this->fOpt_cxValues;
		if (this->fOpt_zymaxValues != NULL)		delete[] this->fOpt_zymaxValues;
		if (this->fOpt_cyValues != NULL)			delete[] this->fOpt_cyValues;
		if (this->bUseOptimizedBoundary == false) {
			if (this->fBoundaryValues != NULL)	delete[] this->fBoundaryValues;
		}
		else {
			if (this->fCouplingValues != NULL)	delete[] fCouplingValues;
			if (this->ulCouplingIDs != NULL)	delete[] ulCouplingIDs;
		}
	}
	else if (this->ucFloatSize == 8) {
		if (this->dCellStates != NULL)			delete[] this->dCellStates;
		if (this->dBedElevations != NULL)		delete[] this->dBedElevations;
		if (this->dManningValues != NULL)		delete[] this->dManningValues;
		if (this->dOpt_zxmaxValues != NULL)		delete[] this->dOpt_zxmaxValues;
		if (this->dOpt_cxValues != NULL)		delete[] this->dOpt_cxValues;
		if (this->dOpt_zymaxValues != NULL)		delete[] this->dOpt_zymaxValues;
		if (this->dOpt_cyValues != NULL)		delete[] this->dOpt_cyValues;
		if (this->bUseOptimizedBoundary == false) {
			if (this->dBoundaryValues != NULL)	delete[] this->dBoundaryValues;
		}
		else {
			if (this->dCouplingValues != NULL)	delete[] dCouplingValues;
			if (this->ulCouplingIDs != NULL)	delete[] ulCouplingIDs;
		}
	}
	if (this->bPoleniValues != NULL)			delete[] this->bPoleniValues;

	if (this->pScheme != NULL)     delete pScheme;
}


//Sets the scheme we're running on this domain
void	CDomainCartesian::setScheme(CScheme* pScheme_input)
{
	this->pScheme = pScheme_input;
}

//Gets the scheme we're running on this domain
CScheme* CDomainCartesian::getScheme()
{
	return pScheme;
}

//Sets the device to use
void	CDomainCartesian::setDevice(COCLDevice* pDevice_input)
{
	this->pDevice = pDevice_input;
}

//Gets the scheme we're running on this domain
COCLDevice* CDomainCartesian::getDevice()
{
	return this->pDevice;
}

std::string CDomainCartesian::getName(void) {
	return this->domainName;
}

void CDomainCartesian::setName(std::string name) {
	this->domainName = name;
}

//Creates an OpenCL memory buffer for the specified data store
void	CDomainCartesian::createStoreBuffers(
	void** vArrayCellStates,
	void** vArrayBedElevations,
	void** vArrayManningCoefs,
	void** vArrayBoundaryValues,
	void** vArrayPoleniValues,
	void** vArrayOpt_zxmax,
	void** vArrayOpt_cx,
	void** vArrayOpt_zymax,
	void** vArrayOpt_cy,
	void** vArrayCouplingIDs,
	void** vArrayCouplingValues,
	unsigned char	ucFloatSize_input
)
{
	prepareDomain();

	unsigned long ulCellCount = this->ulRows * this->ulCols;
	this->ucFloatSize = ucFloatSize_input;

	try {
		if (ucFloatSize == sizeof(cl_float))
		{
			// Single precision
			this->fCellStates = new cl_float4[ulCellCount];
			this->fBedElevations = new cl_float[ulCellCount];
			this->fManningValues = new cl_float[ulCellCount];
			this->fOpt_zxmaxValues = new cl_float[ulCellCount];
			this->fOpt_cxValues = new cl_float[ulCellCount];
			this->fOpt_zymaxValues = new cl_float[ulCellCount];
			this->fOpt_cyValues = new cl_float[ulCellCount];

			this->dCellStates = (cl_double4*)(this->fCellStates);
			this->dBedElevations = (cl_double*)(this->fBedElevations);
			this->dManningValues = (cl_double*)(this->fManningValues);
			this->dOpt_zxmaxValues = (cl_double*)(this->fOpt_zxmaxValues);
			this->dOpt_cxValues = (cl_double*)(this->fOpt_cxValues);
			this->dOpt_zymaxValues = (cl_double*)(this->fOpt_zymaxValues);
			this->dOpt_cyValues = (cl_double*)(this->fOpt_cyValues);

			*vArrayCellStates = static_cast<void*>(this->fCellStates);
			*vArrayBedElevations = static_cast<void*>(this->fBedElevations);
			*vArrayManningCoefs = static_cast<void*>(this->fManningValues);
			*vArrayOpt_zxmax = static_cast<void*>(this->fOpt_zxmaxValues);
			*vArrayOpt_cx = static_cast<void*>(this->fOpt_cxValues);
			*vArrayOpt_zymax = static_cast<void*>(this->fOpt_zymaxValues);
			*vArrayOpt_cy = static_cast<void*>(this->fOpt_cyValues);

			if (this->bUseOptimizedBoundary == false) {
				this->fBoundaryValues = new cl_float[ulCellCount];
				this->dBoundaryValues = (cl_double*)(this->fBoundaryValues);
				*vArrayBoundaryValues = static_cast<void*>(this->fBoundaryValues);
			}
			else {
				this->fCouplingValues = new cl_float[this->ulCouplingArraySize];
				this->dCouplingValues = (cl_double*)(this->fCouplingValues);
				*vArrayCouplingValues = static_cast<void*>(this->fCouplingValues);

				this->ulCouplingIDs = new cl_ulong[this->ulCouplingArraySize];
				*vArrayCouplingIDs = static_cast<void*>(this->ulCouplingIDs);
			}
		}
		else {
			// Double precision
			this->dCellStates = new cl_double4[ulCellCount];
			this->dBedElevations = new cl_double[ulCellCount];
			this->dManningValues = new cl_double[ulCellCount];
			this->dOpt_zxmaxValues = new cl_double[ulCellCount];
			this->dOpt_cxValues = new cl_double[ulCellCount];
			this->dOpt_zymaxValues = new cl_double[ulCellCount];
			this->dOpt_cyValues = new cl_double[ulCellCount];

			this->fCellStates = (cl_float4*)(this->dCellStates);
			this->fBedElevations = (cl_float*)(this->dBedElevations);
			this->fManningValues = (cl_float*)(this->dManningValues);
			this->fOpt_zxmaxValues = (cl_float*)(this->dOpt_zxmaxValues);
			this->fOpt_cxValues = (cl_float*)(this->dOpt_cxValues);
			this->fOpt_zymaxValues = (cl_float*)(this->dOpt_zymaxValues);
			this->fOpt_cyValues = (cl_float*)(this->dOpt_cyValues);

			*vArrayCellStates = static_cast<void*>(this->dCellStates);
			*vArrayBedElevations = static_cast<void*>(this->dBedElevations);
			*vArrayManningCoefs = static_cast<void*>(this->dManningValues);
			*vArrayOpt_zxmax = static_cast<void*>(this->dOpt_zxmaxValues);
			*vArrayOpt_cx = static_cast<void*>(this->dOpt_cxValues);
			*vArrayOpt_zymax = static_cast<void*>(this->dOpt_zymaxValues);
			*vArrayOpt_cy = static_cast<void*>(this->dOpt_cyValues);

			if (this->bUseOptimizedBoundary == false) {
				this->dBoundaryValues = new cl_double[ulCellCount];
				this->fBoundaryValues = (cl_float*)(this->dBoundaryValues);
				*vArrayBoundaryValues = static_cast<void*>(this->fBoundaryValues);
			}
			else {
				this->dCouplingValues = new cl_double[this->ulCouplingArraySize];
				this->fCouplingValues = (cl_float*)(this->dCouplingValues);
				*vArrayCouplingValues = static_cast<void*>(this->fCouplingValues);

				this->ulCouplingIDs = new cl_ulong[this->ulCouplingArraySize];
				*vArrayCouplingIDs = static_cast<void*>(this->ulCouplingIDs);
			}

		}
		this->bPoleniValues = new sUsePoleni[ulCellCount];
		*vArrayPoleniValues = static_cast<void*>(this->bPoleniValues);

		this->resetAllValues();
	}
	catch (const std::bad_alloc& e)
	{
		model::doError(
			"Memory allocation failed: std::bad_alloc",
			model::errorCodes::kLevelFatal,
			"void CDomainCartesian::createStoreBuffers( void**, void**, void**, void**, void**, void**, void**, void**, void**, void**, void**, unsigned char)",
			"The system ran out of memory. Try to run on a machine with more ram. Or use smaller floodplains"
		);
		return;
	}


}

//Populate all domain cells with default values. Shouldn't be required
void	CDomainCartesian::resetAllValues(){
	model::log->logInfo("Reseting heap domain data.");

	for (unsigned long i = 0; i < this->ulCols*ulRows; i++){
		if (this->ucFloatSize == 4){
			this->fCellStates[i].s[0] = 0.0;	// Free-surface level
			this->fCellStates[i].s[1] = 0.0;	// Maximum free-surface level
			this->fCellStates[i].s[2] = 0.0;	// Discharge X
			this->fCellStates[i].s[3] = 0.0;	// Discharge Y
			this->fBedElevations[i] = 0.0;	// Bed elevation
			this->fManningValues[i] = 0.0;	// Manning coefficient
			if (this->bUseOptimizedBoundary == false) {
				this->fBoundaryValues[i] = 0.0;	// Boundary Values
			}

			this->fOpt_zxmaxValues[i] = 0.0;	// Maxium elevation for poleni in X
			this->fOpt_cxValues[i] = 0.0;	// Poleni factor in X
			this->fOpt_zymaxValues[i] = 0.0;	// Maxium elevation for poleni in Y
			this->fOpt_cyValues[i] = 0.0;	// Poleni factor in Y
		}else {
			this->dCellStates[i].s[0] = 0.0;	// Free-surface level
			this->dCellStates[i].s[1] = 0.0;	// Maximum free-surface level
			this->dCellStates[i].s[2] = 0.0;	// Discharge X
			this->dCellStates[i].s[3] = 0.0;	// Discharge Y
			this->dBedElevations[i] = 0.0;	// Bed elevation
			this->dManningValues[i] = 0.0;	// Manning coefficient
			if (this->bUseOptimizedBoundary == false) {
				this->dBoundaryValues[i] = 0.0;	// Boundary Values
			}
			this->dOpt_zxmaxValues[i] = 0.0;	// Maxium elevation for poleni in X
			this->dOpt_cxValues[i] = 0.0;	// Poleni factor in X
			this->dOpt_zymaxValues[i] = 0.0;	// Maxium elevation for poleni in Y
			this->dOpt_cyValues[i] = 0.0;	// Poleni factor in Y
		}

		this->bPoleniValues[i] = { false,false,false,false };					// Poleni flags
	}

	if (this->bUseOptimizedBoundary == true) {
		for (unsigned long i = 0; i < this->ulCouplingArraySize; i++){
			if (this->ucFloatSize == 4){
				this->fCouplingValues[i] = 0.0;	// Optimized Coupling Values
			}else {
				this->dCouplingValues[i] = 0.0;	// Optimized Coupling Values
			}
			this->ulCouplingIDs[i] = 0;    // Optimized Coupling Values
		}
	}
	model::log->logInfo("Reseting heap domain data Finished.");
}

//Does the domain contain all of the required data yet?
bool	CDomainCartesian::validateDomain(bool bQuiet)
{
	// Got a resolution?
	if (this->dCellResolutionX == NAN)
	{
		if (!bQuiet) model::doError(
			"Domain cell resolution not defined",
			model::errorCodes::kLevelWarning,
			"CDomainCartesian::validateDomain( bool bQuiet ){ this->dCellResolutionX == NAN }",
			"Please validate the resolution (size of cell) in the X direction of the floodplain"
		);
		return false;
	}
	if (this->dCellResolutionY == NAN)
	{
		if (!bQuiet) model::doError(
			"Domain cell resolution not defined",
			model::errorCodes::kLevelWarning,
			"CDomainCartesian::validateDomain( bool bQuiet ) { this->dCellResolutionY == NAN }",
			"Please validate the resolution (size of cell) in the Y direction of the floodplain"
		);
		return false;
	}

	if (this->ulRows == NAN || this->ulCols == NAN)
	{
		if (!bQuiet) model::doError(
			"Rows/Cols have not been defined",
			model::errorCodes::kLevelWarning,
			"CDomainCartesian::validateDomain( bool bQuiet ){ this->ulRows == NAN || this->ulCols == NAN }",
			"Please validate the number of elements in the X and Y directions of the floodplain"
		);
		return false;
	}

	return true;
}

//Create the required data structures etc.
void	CDomainCartesian::prepareDomain()
{
	if (!this->validateDomain(true))
	{
		model::doError(
			"Cannot prepare the domain. Invalid specification.",
			model::errorCodes::kLevelModelStop,
			"CDomainCartesian::prepareDomain() { this->validateDomain() }",
			"Please check previous warnings. Element size or resolution are not valid numbers"
		);
		return;
	}

	this->logDetails();
}

//Write useful information about this domain to the log file
void	CDomainCartesian::logDetails()
{
	model::log->writeDivide();

	model::log->logInfo("REGULAR CARTESIAN GRID DOMAIN");
	model::log->logInfo("  Device number:     " + std::to_string(this->pDevice->uiDeviceNo));
	model::log->logInfo("  Cell count:        " + std::to_string(this->ulCols*this->ulRows));
	model::log->logInfo("  Cell resolution:   " + std::to_string(this->dCellResolutionX));
	model::log->logInfo("  Cell dimensions:   [" + std::to_string(this->ulCols) + ", " +
		std::to_string(this->ulRows) + "]");

	model::log->writeDivide();
}

//Set cell resolution
void	CDomainCartesian::setCellResolution(double dResolutionX, double dResolutionY)
{
	this->dCellResolutionX = dResolutionX;
	this->dCellResolutionY = dResolutionY;
}

//Fetch cell resolution
void	CDomainCartesian::getCellResolution(double* dResolutionX, double* dResolutionY)
{
	*dResolutionX = this->dCellResolutionX;
	*dResolutionY = this->dCellResolutionY;
}

//Fetch cell resolution
double	CDomainCartesian::getCellResolutionX()
{
	return this->dCellResolutionX;
}

//Fetch cell resolution
double	CDomainCartesian::getCellResolutionY()
{
	return this->dCellResolutionY;
}

//Gets the scheme we're running on this domain
dataProgress	CDomainCartesian::getDataProgress(){
	dataProgress pResponse;

	pResponse.dBatchTimesteps = pScheme->getAverageTimestep();
	pResponse.dCurrentTime = pScheme->getCurrentTime();
	pResponse.dCurrentTimestep = pScheme->getCurrentTimestep();
	pResponse.uiBatchSize = pScheme->getBatchSize();
	pResponse.uiBatchSkipped = pScheme->getIterationsSkipped();
	pResponse.uiBatchSuccessful = pScheme->getIterationsSuccessful();

	return pResponse;
}

//set the number of columns of the domain
void	CDomainCartesian::setCols(unsigned long value)
{
	this->ulCols = value;
}

//set the number of rows of the domain
void	CDomainCartesian::setRows(unsigned long value)
{
	this->ulRows = value;
}

//Return the total number of rows in the domain
unsigned long	CDomainCartesian::getRows()
{
	return this->ulRows;
}

//Return the total number of columns in the domain
unsigned long	CDomainCartesian::getCols()
{
	return this->ulCols;
}

//Set the flag for using the optimized coupling approach where zero boundary condition elements are ignored
void	CDomainCartesian::setUseOptimizedCoupling(bool state)
{
	this->bUseOptimizedBoundary = state;
}

//Get the flag for using the optimized coupling approach where zero boundary condition elements are ignored
bool	CDomainCartesian::getUseOptimizedCoupling()
{
	return this->bUseOptimizedBoundary;
}

//Set the size of the coupling array for using the optimized coupling approach where zero boundary condition elements are ignored
void	CDomainCartesian::setOptimizedCouplingSize(unsigned long value)
{
	this->ulCouplingArraySize = value;
}

//Get the size of the coupling array for using the optimized coupling approach where zero boundary condition elements are ignored
unsigned long	CDomainCartesian::getOptimizedCouplingSize()
{
	return this->ulCouplingArraySize;
}

//Gets the bed elevation for a given cell
double	CDomainCartesian::getBedElevation(unsigned long ulCellID)
{
	if (this->ucFloatSize == 4)
		return static_cast<double>(this->fBedElevations[ulCellID]);
	return this->dBedElevations[ulCellID];
}

//Gets the bed elevation for a given cell
double	CDomainCartesian::getZxmax(unsigned long ulCellID)
{
	if (this->ucFloatSize == 4)
		return static_cast<double>(this->fOpt_zxmaxValues[ulCellID]);
	return this->dOpt_zxmaxValues[ulCellID];
}

//Gets the bed elevation for a given cell
double	CDomainCartesian::getZymax(unsigned long ulCellID)
{
	if (this->ucFloatSize == 4)
		return static_cast<double>(this->fOpt_zymaxValues[ulCellID]);
	return this->dOpt_zymaxValues[ulCellID];
}

//Gets a state variable for a given cell
double	CDomainCartesian::getStateValue(unsigned long ulCellID, unsigned char ucIndex)
{
	if (this->ucFloatSize == 4)
		return static_cast<double>(this->fCellStates[ulCellID].s[ucIndex]);
	return this->dCellStates[ulCellID].s[ucIndex];
}

//Get a cell ID from an X and Y index
unsigned long	CDomainCartesian::getCellID(unsigned long ulX, unsigned long ulY)
{
	return (ulY * this->getCols()) + ulX;
}

//Calculate the total volume present in all of the cells
double	CDomainCartesian::getVolume()
{
	double dVolume = 0.0;

	for (unsigned int i = 0; i < this->ulCols*ulRows; ++i)
	{
		if (this->isDoublePrecision())
		{
			dVolume += (this->dCellStates[i].s[0] - this->dBedElevations[i]) *
				this->dCellResolutionX * this->dCellResolutionY;
		}
		else {
			dVolume += (this->fCellStates[i].s[0] - this->fBedElevations[i]) *
				this->dCellResolutionX * this->dCellResolutionY;
		}
	}

	return dVolume;
}

//Calculate the total volume present in the boundary (Volume that will be added to the domain)
double	CDomainCartesian::getBoundaryVolume()
{
	double dVolume = 0.0;

	for (unsigned int i = 0; i < this->ulCols * ulRows; ++i)
	{
		if (this->isDoublePrecision())
		{

			dVolume += (this->dBoundaryValues[i]) *
				this->dCellResolutionX * this->dCellResolutionY;
		}
		else {
			dVolume += (this->fBoundaryValues[i]) *
				this->dCellResolutionX * this->dCellResolutionY;
		}
	}

	return dVolume;
}

//Read water depth to a double pointer
void CDomainCartesian::readBuffers_opt_h(double* valueArray)
{
	// Read the data back first...
	// TODO: Review whether this is necessary, isn't it a sync point anyway?
	pDevice->blockUntilFinished();
	pScheme->readDomainAll();
	pDevice->blockUntilFinished();

	unsigned long	ulCellID;

	//double Volume = 0;
	for (unsigned long iRow = 0; iRow < this->getRows(); ++iRow) {
		for (unsigned long iCol = 0; iCol < this->getCols(); ++iCol) {
			ulCellID = this->getCellID(iCol, iRow);
			valueArray[ulCellID] = this->getStateValue(ulCellID, model::domainValueIndices::kValueFreeSurfaceLevel) - this->getBedElevation(ulCellID);
			//Volume += values[ulCellID];
		}
	}

}

//Read water depth, velocity in x, velocity in , to a double pointer
void CDomainCartesian::readBuffers_h_vx_vy(double* opt_h, double* v_x, double* v_y)
{
	// Read the data back first...
	// TODO: Review whether this is necessary, isn't it a sync point anyway?
	pDevice->blockUntilFinished();
	pScheme->readDomainAll();
	pDevice->blockUntilFinished();

	//temp
	//double highestV = 0 ;
	//int index = 0;
	////temp


	unsigned long	ulCellID;
	double dDepth, dV_x, dV_y;

	for (unsigned long iRow = 0; iRow < this->getRows(); ++iRow) {
		for (unsigned long iCol = 0; iCol < this->getCols(); ++iCol) {
			ulCellID = this->getCellID(iCol, iRow);
			dDepth = this->getStateValue(ulCellID, model::domainValueIndices::kValueFreeSurfaceLevel) - this->getBedElevation(ulCellID);
			dV_x = dDepth > 1E-8 ? (this->getStateValue(ulCellID, model::domainValueIndices::kValueDischargeX) / dDepth) : 0.0;
			dV_y = dDepth > 1E-8 ? (this->getStateValue(ulCellID, model::domainValueIndices::kValueDischargeY) / dDepth) : 0.0;

			//TEMP
			//if (abs(dV_x) + abs(dV_y) > highestV) {
			//	index = ulCellID;
			//	highestV = abs(dV_x) + abs(dV_y);
			//}
			//TEMP

			opt_h[ulCellID] = dDepth;
			v_x[ulCellID] = dV_x;
			v_y[ulCellID] = dV_y;
		}
	}
	//temp
	//std::cout << "Highest V: " << highestV << " at " << index << std::endl;
	//temp
}

//Read Velocity in x buffer to double pointer
void CDomainCartesian::readBuffers_v_x(double* v_x_array)
{
	// Read the data back first...
	// TODO: Review whether this is necessary, isn't it a sync point anyway?
	pDevice->blockUntilFinished();
	pScheme->readDomainAll();
	pDevice->blockUntilFinished();

	unsigned long	ulCellID;
	double dDepth, dResult;

	for (unsigned long iRow = 0; iRow < this->getRows(); ++iRow) {
		for (unsigned long iCol = 0; iCol < this->getCols(); ++iCol) {
			ulCellID = this->getCellID(iCol, iRow);
			dDepth = this->getStateValue(ulCellID, model::domainValueIndices::kValueFreeSurfaceLevel) - this->getBedElevation(ulCellID);
			dResult = dDepth > 1E-8 ? (this->getStateValue(ulCellID, model::domainValueIndices::kValueDischargeX) / dDepth) : 0.0;
			v_x_array[ulCellID] = dResult;
		}
	}
}

//Read Velocity in y buffer to double pointer
void CDomainCartesian::readBuffers_v_y(double* v_y_array)
{
	// Read the data back first...
	// TODO: Review whether this is necessary, isn't it a sync point anyway?
	pDevice->blockUntilFinished();
	pScheme->readDomainAll();
	pDevice->blockUntilFinished();

	unsigned long	ulCellID;
	double dDepth, dResult;

	//double Volume = 0;
	for (unsigned long iRow = 0; iRow < this->getRows(); ++iRow) {
		for (unsigned long iCol = 0; iCol < this->getCols(); ++iCol) {
			ulCellID = this->getCellID(iCol, iRow);
			dDepth = this->getStateValue(ulCellID, model::domainValueIndices::kValueFreeSurfaceLevel) - this->getBedElevation(ulCellID);

			dResult = dDepth > 1E-8 ? (this->getStateValue(ulCellID, model::domainValueIndices::kValueDischargeY) / dDepth) : 0.0;

			v_y_array[ulCellID] = dResult;
		}
	}
}

/*
 *  Resting Boundary Conditions
 */
void	CDomainCartesian::resetBoundaryCondition()
{
	if (this->ucFloatSize == 4)
	{
		memset(fBoundaryValues, 0, sizeof(cl_float) * this->ulRows * ulCols);
	}
	else {
		memset(dBoundaryValues, 0, sizeof(cl_double) * this->ulRows * ulCols);
	}
}

//Return the total number of cells in the domain
unsigned long	CDomainCartesian::getCellCount()
{
	return (this->ulRows * this->ulCols);
}


////Progress Monitoring
// Set some data on this domain's progress
void CDomainCartesian::setDataProgress(dataProgress sDataProgress_input) {
	this->sDataProgress = sDataProgress_input;
}


//Data Setting
//Sets the bed elevation for a given cell
void	CDomainCartesian::setBedElevation(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);

	if (this->ucFloatSize == 4) {
		this->fBedElevations[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dBedElevations[ulCellID] = dValue;
	}

}

//Sets the Manning coefficient for a given cell
void	CDomainCartesian::setManningCoefficient(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);

	if (this->ucFloatSize == 4) {
		this->fManningValues[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dManningValues[ulCellID] = dValue;
	}
}

//Sets a state variable for a given cell
void	CDomainCartesian::setFSL(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4) {
		this->fCellStates[ulCellID].s[model::domainValueIndices::kValueFreeSurfaceLevel] = static_cast<float>(dValue);
	}
	else {
		this->dCellStates[ulCellID].s[model::domainValueIndices::kValueFreeSurfaceLevel] = dValue;
	}
}

//Sets a state variable for a given cell
void	CDomainCartesian::setMaxFSL(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4) {
		this->fCellStates[ulCellID].s[model::domainValueIndices::kValueMaxFreeSurfaceLevel] = static_cast<float>(dValue);
	}
	else {
		this->dCellStates[ulCellID].s[model::domainValueIndices::kValueMaxFreeSurfaceLevel] = dValue;
	}
}

//Sets a state variable for a given cell
void	CDomainCartesian::setDischargeX(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4) {
		this->fCellStates[ulCellID].s[model::domainValueIndices::kValueDischargeX] = static_cast<float>(dValue);
	}
	else {
		this->dCellStates[ulCellID].s[model::domainValueIndices::kValueDischargeX] = dValue;
	}
}

//Sets a state variable for a given cell
void	CDomainCartesian::setDischargeY(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4) {
		this->fCellStates[ulCellID].s[model::domainValueIndices::kValueDischargeY] = static_cast<float>(dValue);
	}
	else {
		this->dCellStates[ulCellID].s[model::domainValueIndices::kValueDischargeY] = dValue;
	}
}


//Sets the Boundary values for a given cell
void	CDomainCartesian::setBoundaryCondition(unsigned long ulCellID, double dValue)
{
	if (this->bUseOptimizedBoundary) {
		model::doError("Boundary condition is being set, when there is no boundary array",
			model::errorCodes::kLevelFatal,
			"void	CDomainCartesian::setBoundaryCondition(unsigned long ulCellID, double dValue)",
			"Optimized boundary conditions means there is no boundary array, only a coupling array. Fix the code"
		);
	}

	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4)
	{
		this->fBoundaryValues[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dBoundaryValues[ulCellID] = dValue;
	}
}

//Sets the Boundary values for a given cell
void	CDomainCartesian::setOptimizedCouplingCondition(unsigned long index, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4)
	{
		this->fCouplingValues[index] = static_cast<float>(dValue);
	}
	else {
		this->dCouplingValues[index] = dValue;
	}
}

//Sets the Boundary values for a given cell
void	CDomainCartesian::setZxmax(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4)
	{
		this->fOpt_zxmaxValues[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dOpt_zxmaxValues[ulCellID] = dValue;
	}
}

//Sets the Boundary values for a given cell
void	CDomainCartesian::setcx(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4)
	{
		this->fOpt_cxValues[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dOpt_cxValues[ulCellID] = dValue;
	}
}

//Sets the Boundary values for a given cell
void	CDomainCartesian::setZymax(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4)
	{
		this->fOpt_zymaxValues[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dOpt_zymaxValues[ulCellID] = dValue;
	}
}

//Sets the Boundary values for a given cell
void	CDomainCartesian::setcy(unsigned long ulCellID, double dValue)
{
	dValue = Util::round(dValue, uiRounding);
	if (this->ucFloatSize == 4)
	{
		this->fOpt_cyValues[ulCellID] = static_cast<float>(dValue);
	}
	else {
		this->dOpt_cyValues[ulCellID] = dValue;
	}
}

//Sets the Optimized Coupling Ids
void	CDomainCartesian::setOptimizedCouplingID(unsigned long index, unsigned long ID)
{
	this->ulCouplingIDs[index] = ID;
}

//Sets the Poleni condition for a given cell in X
void	CDomainCartesian::setPoleniConditionX(unsigned long ulCellID, bool UsePoleniInX)
{
	//All values are already false by default, so we need to check which are true in the x direction, then change their neighbor to the east to true also in the -x direction
	// Todo: Alaa. Review why poleni is enabled on the borders anyways?
	if (UsePoleniInX) {

		unsigned long lIdxX = 0;
		unsigned long lIdxY = 0;
		getCellIndices(ulCellID, &lIdxX, &lIdxY);
		if (lIdxX < this->getCellCount() - 1) {
			this->bPoleniValues[ulCellID].usePoliniE = true;
			long ulCellID_Neigh_E = this->getNeighbourID(ulCellID, direction::east);
			this->bPoleniValues[ulCellID_Neigh_E].usePoliniW = true;
		}
	}

}

//Sets the Poleni condition for a given cell in Y
void	CDomainCartesian::setPoleniConditionY(unsigned long ulCellID, bool UsePoleniInY)
{
	//All values are already false by default, so we need to check which are true in the x direction, then change their neighbor to the west to true also in the -y direction
	// Todo: Alaa. Review why poleni is enabled on the borders anyways?
	if (UsePoleniInY) {

		unsigned long lIdxX = 0;
		unsigned long lIdxY = 0;
		getCellIndices(ulCellID, &lIdxX, &lIdxY);
		if (lIdxY < this->ulRows - 1) {
			this->bPoleniValues[ulCellID].usePoliniN = true;
			long ulCellID_Neigh_N = this->getNeighbourID(ulCellID, direction::north);
			this->bPoleniValues[ulCellID_Neigh_N].usePoliniS = true;
		}
	}

}

//Gets the Manning coefficient for a given cell
double	CDomainCartesian::getBoundaryCondition(unsigned long ulCellID)
{
	if (this->ucFloatSize == 4)
		return static_cast<double>(this->fBoundaryValues[ulCellID]);
	return this->dBoundaryValues[ulCellID];
}

////Helper Functions

//Fetch the X and Y indices for a cell using its ID
void	CDomainCartesian::getCellIndices(unsigned long ulID, unsigned long* lIdxX, unsigned long* lIdxY)
{
	*lIdxX = ulID % this->getCellCount();
	*lIdxY = (ulID - *lIdxX) / this->getCellCount();
}

//Fetch the ID for a neighboring cell in the domain
unsigned long	CDomainCartesian::getNeighbourID(unsigned long ulCellID, unsigned char ucDirection)
{
	unsigned long lIdxX = 0;
	unsigned long lIdxY = 0;
	getCellIndices(ulCellID, &lIdxX, &lIdxY);

	switch (ucDirection)
	{
	case direction::north:
		++lIdxY;
		break;
	case direction::east:
		++lIdxX;
		break;
	case direction::south:
		--lIdxY;
		break;
	case direction::west:
		--lIdxX;
		break;
	}
	return getCellID(lIdxX, lIdxY);
}

//Dumps all memory of the domain for debugging
void	CDomainCartesian::memoryDump()
{
	pDevice->blockUntilFinished();
	pScheme->dumpMemory();
	pDevice->blockUntilFinished();


	if (this->isDoublePrecision() == false) {
		return;
	}

	unsigned long ulCellCount = this->ulRows * this->ulCols;

	model::log->logInfo("Dumping dCellStates: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
							"  0:" + std::to_string(this->dCellStates[i].s[0]) + 
							"  1:" + std::to_string(this->dCellStates[i].s[1]) +
							"  2:" + std::to_string(this->dCellStates[i].s[2]) +
							"  3:" + std::to_string(this->dCellStates[i].s[3])
		);
	}
	
	model::log->logInfo("Dumping dBedElevations: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			std::to_string(this->dBedElevations[i])
		);
	}
	
	model::log->logInfo("Dumping dManningValues: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			std::to_string(this->dManningValues[i])
		);
	}
	
	model::log->logInfo("Dumping dOpt_zxmaxValues: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			std::to_string(this->dOpt_zxmaxValues[i])
		);
	}
	
	model::log->logInfo("Dumping dOpt_zymaxValues: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			std::to_string(this->dOpt_zymaxValues[i])
		);
	}
	
	model::log->logInfo("Dumping dOpt_cxValues: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			std::to_string(this->dOpt_cxValues[i])
		);
	}
	
	model::log->logInfo("Dumping dOpt_cyValues: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			std::to_string(this->dOpt_cyValues[i])
		);
	}
	
	
	if (this->bUseOptimizedBoundary == false) {
	
		model::log->logInfo("Dumping dBoundaryValues: ");
		for (unsigned long i = 0; i < ulCellCount; i++) {
			model::log->logInfo(std::to_string(i) + ": " +
				std::to_string(this->dBoundaryValues[i])
			);
		}
	}
	else {
		model::log->logInfo("Dumping dCouplingValues: ");
		for (unsigned long i = 0; i < ulCellCount; i++) {
			model::log->logInfo(std::to_string(i) + ": " +
				std::to_string(this->dCouplingValues[i])
			);
		}
	
		model::log->logInfo("Dumping ulCouplingIDs: ");
		for (unsigned long i = 0; i < ulCouplingArraySize; i++) {
			model::log->logInfo(std::to_string(i) + ": " +
				std::to_string(this->ulCouplingIDs[i])
			);
		}
	}


	model::log->logInfo("Dumping bPoleniValues: ");
	for (unsigned long i = 0; i < ulCellCount; i++) {
		model::log->logInfo(std::to_string(i) + ": " +
			"  N:" + std::to_string(this->bPoleniValues[i].usePoliniN) +
			"  E:" + std::to_string(this->bPoleniValues[i].usePoliniE) +
			"  S:" + std::to_string(this->bPoleniValues[i].usePoliniS) +
			"  W:" + std::to_string(this->bPoleniValues[i].usePoliniW)
		);
	}

}

//Output the result members per timestep to Paraview
void CDomainCartesian::output_to_vtk_file(std::string path, double time, std::string rasterName, int sizeX, int sizeY, double* opt_z, double* opt_zx_max, double* opt_zy_max, double* opt_h, double* opt_s, double* opt_v_x, double* opt_v_y) {

	//get the file name
	double temp_double;
	unsigned int temp_uint;
	std::string filename = path;
	int sizeT = sizeX * sizeY;

	//open the file
	std::ofstream txt(filename, std::ios::binary);

	//output the file header
	txt << "# vtk DataFile Version 3.0" << std::endl;
	txt << "Result of floodplain " << rasterName << " for time " << time << std::endl;
	txt << "BINARY" << std::endl;
	txt << "DATASET STRUCTURED_GRID" << std::endl;
	txt << "DIMENSIONS " << sizeX + 1 << " " << sizeY + 1 << " 1" << std::endl;
	txt << "POINTS " << (sizeX + 1) * (sizeY + 1) << " double" << std::endl;

	// Generate grid points
	double xCoord, yCoord, zCoord;
	for (int y = 0; y <= sizeY; ++y) {
		for (int x = 0; x <= sizeX; ++x) {

			xCoord = static_cast<double>(x);
			yCoord = static_cast<double>(y);
			zCoord = static_cast<double>(0.0);

			Util::SwapEnd(xCoord);
			Util::SwapEnd(yCoord);
			Util::SwapEnd(zCoord);

			// Write the coordinates in binary
			txt.write(reinterpret_cast<const char*>(&xCoord), sizeof(double));
			txt.write(reinterpret_cast<const char*>(&yCoord), sizeof(double));
			txt.write(reinterpret_cast<const char*>(&zCoord), sizeof(double));

		}
	}

	//output data
	std::string buff_unit;
	txt << "CELL_DATA " << sizeT << std::endl;

	txt << "SCALARS  cell_id unsigned_int" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;

	for (unsigned int i = 0; i < sizeT; i++) {
		temp_uint = i;
		Util::SwapEnd(temp_uint);
		txt.write(reinterpret_cast<char*>(&temp_uint), sizeof(unsigned int));
	}
	txt << std::endl;

	txt << "SCALARS  opt_z double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;

	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_z[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}
	txt << std::endl;

	txt << "SCALARS  opt_zx_max double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;
	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_zx_max[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}
	txt << std::endl;

	txt << "SCALARS  opt_zy_max double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;
	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_zy_max[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}
	txt << std::endl;

	txt << "SCALARS  opt_h double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;
	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_h[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}
	txt << std::endl;

	txt << "SCALARS  opt_s double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;
	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_s[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}
	txt << std::endl;

	txt << "SCALARS  opt_v_x double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;
	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_v_x[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}
	txt << std::endl;

	txt << "SCALARS  opt_v_y double" << std::endl;
	txt << "LOOKUP_TABLE default" << std::endl;
	for (int i = 0; i < sizeT; i++) {
		temp_double = opt_v_y[i];
		Util::SwapEnd(temp_double);
		txt.write(reinterpret_cast<char*>(&temp_double), sizeof(double));
	}

	txt.close();

}