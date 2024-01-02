/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

// Includes
#include "common.h"
#include "COCLProgram.h"
#include "COCLKernel.h"

//Constructor
COCLProgram::COCLProgram(
		CExecutorControlOpenCL*		execController,
		COCLDevice*					device
	)
{
	this->execController		= execController;
	this->device				= device;
	this->clProgram				= NULL;
	this->clContext				= device->getContext();
	this->bCompiled				= false;
	this->sCompileParameters	= "";
}

//Destructor
COCLProgram::~COCLProgram()
{
	if ( this->clProgram != NULL )
		clReleaseProgram( this->clProgram );

	clearCode();
}

//Attempt to build the program
void COCLProgram::compileProgram(
		bool	bIncludeStandardElements
	)
{
	cl_int		iErrorID;

#ifdef USE_SIMPLE_ARCH_OPENCL
	// Use GCC argument instead
	this->sCompileParameters += " -fsingle-precision-constant";
#else
	// Is the device configured to force single-precision?
	if (this->bForceSinglePrecision)
		this->sCompileParameters += " -cl-single-precision-constant";

	// Some standard things we need
	this->sCompileParameters += " -cl-mad-enable";
#endif

	// This might not be a good idea...
	//this->sCompileParameters += " -cl-denorms-are-zero";

	// For intel debugging only!
	//this->sCompileParameters += " -g";

	model::log->logInfo("Compiling a new program for device #" + toStringExact(this->device->getDeviceID()) + ".");

	// Should we add standard elements to the code stack first?
	if (bIncludeStandardElements)
	{
		this->prependCodeFromResource("CLUniversalHeader_H");						// Standard header with things like gravitational acceleration.
		this->prependCode(this->getExtensionsHeader());							// Required for double-precision etc.
		this->prependCode(this->getConstantsHeader());							// Domain constant data (i.e. rows, cols etc.)
	}

	cl_uint			uiStackLength = static_cast<cl_uint>(oclCodeStack.size());
	OCL_RAW_CODE* orcCode = new char* [uiStackLength];
	for (unsigned int i = 0; i < uiStackLength; i++)
		orcCode[i] = oclCodeStack[i];

	clProgram = clCreateProgramWithSource(
		this->clContext,
		uiStackLength,
		const_cast<const char**>(orcCode),
		NULL,							// All char* must be null terminated because we don't pass any lengths!
		&iErrorID
	);

	if (iErrorID != CL_SUCCESS)
	{
		delete[] orcCode;
		model::doError(
			"Could not create a program to run on device #" + toStringExact(this->device->getDeviceID()) + "." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clCreateProgramWithSource",
			model::errorCodes::kLevelModelStop,
			"bool COCLProgram::compileProgram( bool bIncludeStandardElements )",
			"Your device might not be supported."
		);

	}

	iErrorID = clBuildProgram(
		clProgram,																		// Program
		NULL,																			// Num. devices
		NULL,																			// Device list
		sCompileParameters.c_str(),														// Options (no  -cl-finite-math-only  -cl-denorms-are-zero)
		NULL,																			// Callback
		NULL																			// Callback data
	);

	if (iErrorID != CL_SUCCESS)
	{

		size_t totalLength = 0;
		for (const char* str : oclCodeStack) {
			totalLength += strlen(str);
		}
		// Allocate memory for the concatenated string plus null terminator
		char* concatenatedString = new char[totalLength + 1];
		// Concatenate the strings
		concatenatedString[0] = '\0'; // Ensure the string is initially empty
		for (const char* str : oclCodeStack) {
			strcat(concatenatedString, str);
		}

		model::log->logInfo(this->getCompileLog());
		model::log->writeCharToFile(concatenatedString, "failedBuildLog.txt");
    	delete[] concatenatedString;
		delete[] orcCode;

		model::log->logInfo("The source code has been written to failedBuildLog.txt. Please check it for errors.");
		
		model::doError(
			"Could not build the program to run on device #" + toStringExact(this->device->getDeviceID()) + "." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clBuildProgram",
			model::errorCodes::kLevelModelStop,
			"bool COCLProgram::compileProgram( bool bIncludeStandardElements )",
			"Please contact the developers. See log above."
		);
	}

	model::log->logInfo("Program successfully compiled for device #" + toStringExact(this->device->getDeviceID()) + ".");

	std::string sBuildLog = this->getCompileLog();
	if (sBuildLog.length() > 0)
	{
		size_t totalLength = 0;
		for (const char* str : oclCodeStack) {
			totalLength += strlen(str);
		}
		// Allocate memory for the concatenated string plus null terminator
		char* concatenatedString = new char[totalLength + 1];
		// Concatenate the strings
		concatenatedString[0] = '\0'; // Ensure the string is initially empty
		for (const char* str : oclCodeStack) {
			strcat(concatenatedString, str);
		}
		model::log->logInfo(sBuildLog);
		model::log->writeCharToFile(concatenatedString, "WarningBuildLog.txt");
		delete[] concatenatedString;
		delete[] orcCode;
		model::log->logInfo("The source code has been written to WarningBuildLog.txt. Please check it.");

		model::doError("Some messages were reported while building.",
			model::errorCodes::kLevelWarning,
			"bool COCLProgram::compileProgram( bool bIncludeStandardElements )",
			"See log above."
		);
	}

	// Write debug file containing the concatenated code
	// TODO: Make debug outputs configurable/optional/debug only
	//model::log->writeDebugFile( orcCode, uiStackLength );

	delete[] orcCode;

	this->bCompiled = true;
}

//Append code to the program stack
void COCLProgram::appendCode(OCL_RAW_CODE	oclCode){
	oclCodeStack.push_back( oclCode );
}

//Prepend code to the program stack
void COCLProgram::prependCode(
		OCL_RAW_CODE	oclCode
	)
{
	oclCodeStack.insert( oclCodeStack.begin(), oclCode );
}

//Append code to the program stack from an embedded resource
void COCLProgram::appendCodeFromResource(std::string sFilename){
	appendCode(device->getExecController()->getOCLCode( sFilename ));
}

//Prepend code to the program stack from an embedded resource
void COCLProgram::prependCodeFromResource(
		std::string sFilename
	)
{
	prependCode(
		device->getExecController()->getOCLCode( sFilename )
	);
}

//Empty the code stack
void COCLProgram::clearCode()
{
	if ( oclCodeStack.size() < 1 ) return;

	for( auto cCode =  oclCodeStack.begin();
			  cCode != oclCodeStack.end();
			  cCode++ )
	{
		delete [] (*cCode);
	}

	oclCodeStack.clear();
}

//Fetch a kernel from within the program and create a class to handle it
COCLKernel* COCLProgram::getKernel(
		const char*		cKernelName
	)
{
	if ( !this->bCompiled ) return NULL;
	
	return new COCLKernel(
		this,
		std::string( cKernelName )
	);
}

//Get the compile log, mostly used for error messages
std::string COCLProgram::getCompileLog()
{
	std::string	sLog;
	size_t		szLogLength;
	cl_int		iErrorID;

	iErrorID = clGetProgramBuildInfo(
		clProgram,
		this->device->getDevice(),
		CL_PROGRAM_BUILD_LOG,
		NULL,
		NULL,
		&szLogLength
	);

	if ( iErrorID != CL_SUCCESS )
	{
		// The model cannot continue in this case
		model::doError(
			"Could not obtain a build log for the program on device #" + toStringExact( this->device->getDeviceID() ) + "." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clGetProgramBuildInfo",
			model::errorCodes::kLevelModelStop,
			"std::string COCLProgram::getCompileLog()",
			"Please check error code"
		);
		return "An error occurred";
	}

	char* cBuildLog = new char[ szLogLength + 1 ];

	iErrorID = clGetProgramBuildInfo(
		clProgram,
		this->device->getDevice(),
		CL_PROGRAM_BUILD_LOG,
		szLogLength,
		cBuildLog,
		NULL
	);

	if ( iErrorID != CL_SUCCESS )
	{
		delete[] cBuildLog;
		// The model cannot continue in this case
		model::doError(
			"Could not obtain a build log for the program on device #" + toStringExact( this->device->getDeviceID() ) + "." +
			" Got Error code: [" + Util::get_error_str(iErrorID) + "] from clGetProgramBuildInfo",
			model::errorCodes::kLevelModelStop,
			"std::string COCLProgram::getCompileLog()",
			"Please check error code"
		);
		return "An error occurred";
	}

	cBuildLog[ szLogLength ] = 0;

	sLog = std::string( cBuildLog );
	delete[] cBuildLog;

	sLog.erase(sLog.find_last_not_of(" \n\r\t") + 1);
	return sLog;
}

//Add a compiler argument to the list
void COCLProgram::addCompileParameter(
		std::string sParameter 
	)
{
	sCompileParameters += " " + sParameter;
}

//Add a new constant to the register for inclusion in all OpenCL programs
bool COCLProgram::registerConstant( std::string sName, std::string sValue )
{
	this->uomConstants[ sName ] = sValue;

	return true;
}

// Remove a single named constant
bool COCLProgram::removeConstant( std::string sName )
{
	if ( this->uomConstants.find( sName ) == this->uomConstants.end() )
		return false;

	this->uomConstants.erase( sName );

	return true;
}

//Remove all of the registered constants
void COCLProgram::clearConstants()
{
	this->uomConstants.clear();
}

//Get OpenCL code representing the constants defined
OCL_RAW_CODE COCLProgram::getConstantsHeader()
{
	std::stringstream ssHeader;
	ssHeader << std::endl;

	for( std::unordered_map< std::string, std::string >::iterator
		 uomItConstants  = this->uomConstants.begin();
		 uomItConstants != this->uomConstants.end();
		 ++uomItConstants )
	{
		ssHeader << "#define " << uomItConstants->first
				 << " " << uomItConstants->second <<
				 std::endl;
	}

	char*	cHeader = new char[ ssHeader.str().length() + 1 ];
	strcpy( cHeader, ssHeader.str().c_str() );
	//strcpy_s( cHeader, ssHeader.str().length() + 1, ssHeader.str().c_str() );
	return cHeader;
}

//Create a header for inclusion in all code that handles the support or lack thereof for double precision, etc.
OCL_RAW_CODE	COCLProgram::getExtensionsHeader()
{
	std::stringstream	ssHeader;

	// Double precision support available?
	// Check for all of the required features, assuming we will need them all...
	if ( this->device->isDoubleCompatible() && !this->bForceSinglePrecision )
	{
		// Include the pragma directive to enable double support
		// AMD GPU devices don't yet support the full Khronos-approved extension
		if( strcmp( this->device->getVendor(), "Advanced Micro Devices, Inc." ) == 0 &&
			this->device->getDeviceType() &	CL_DEVICE_TYPE_GPU &&											// Support introduced in new AMD drivers for the proper extension
			strstr( this->device->getOCLVersion(), "OpenCL 1.0" ) == NULL &&
			strstr( this->device->getOCLVersion(), "OpenCL 1.1" ) == NULL 
		   )
		{
			ssHeader << "#pragma OPENCL EXTENSION cl_amd_fp64 : enable" << std::endl;
		} else {
			ssHeader << "#pragma OPENCL EXTENSION cl_khr_fp64 : enable" << std::endl;
		}

		// Create an alias
		ssHeader << "typedef double      cl_double;" << std::endl;
		ssHeader << "typedef double2     cl_double2;" << std::endl;
		//ssHeader << "typedef double3     cl_double3;" << std::endl;
		ssHeader << "typedef double4     cl_double4;" << std::endl;
		ssHeader << "typedef double8     cl_double8;" << std::endl;
		//ssHeader << "typedef double16     cl_double16;" << std::endl;
	} else {
		// Send warning to log
		model::doError(
			"Double-precision will be handled as single-precision.",
			model::errorCodes::kLevelWarning,
			"OCL_RAW_CODE	COCLProgram::getExtensionsHeader()",
			"This might cause some problems, down the line."
		);

		// Create an alias and treat doubles as floats
		ssHeader << "typedef float		cl_double;" << std::endl;
		ssHeader << "typedef float2		cl_double2;" << std::endl;
		//ssHeader << "typedef float3		cl_double3;" << std::endl;
		ssHeader << "typedef float4		cl_double4;" << std::endl;
		ssHeader << "typedef float8		cl_double8;" << std::endl;
		//ssHeader << "typedef float16	cl_double16;" << std::endl;
	}

	char*	cHeader = new char[ ssHeader.str().length() + 1 ];
	strcpy( cHeader, ssHeader.str().c_str() );
	//strcpy_s( cHeader, ssHeader.str().length() + 1, ssHeader.str().c_str() );
	return cHeader;
}

//Should we force the device to use single precision?
void	COCLProgram::setForcedSinglePrecision( bool bForce )
{
	this->bForceSinglePrecision	= bForce;
}

