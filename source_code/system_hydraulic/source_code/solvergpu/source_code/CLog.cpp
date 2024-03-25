/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

// Includes
#include "common.h"
#include <sstream>

// Constructor
CLog::CLog(CLoggingInterface* externalLogger_input, bool bLoggingActive_input){

	this->bLoggingActive = bLoggingActive_input;

	externalLogger = nullptr;
	MAIN_THREAD_ID = std::this_thread::get_id();

	//Check and attach to external logging functions
	if (externalLogger_input != NULL) {
		useDefaultLogger = false;
		externalLogger = externalLogger_input;
	}
	else {
		useDefaultLogger = true;
	}

	setlocale( LC_ALL, "" );
	this->logInfo("Log component fully loaded.");
}

// Destructor
CLog::~CLog(void){
	if (this->externalLogger != NULL)
		delete this->externalLogger;
}

//Write a line to divide up the output, purely superficial
void CLog::writeDivide()
{
	if (bLoggingActive) {
		this->logInfo( "---------------------------------------------                           " );
	}
}

//Actual outputting of debug message to user
void CLog::logDebug(const std::string& message) {
	if (bLoggingActive) {
		if (useDefaultLogger) {
			std::cout << "[DEBUG]: " << message << std::endl;
		}
		else {
			externalLogger->logDebug(message);
		}
	}
}

//Actual outputting of info message to user
void CLog::logInfo(const std::string& message) {
	if (bLoggingActive) {
		if (useDefaultLogger) {
			std::cout << "[INFO]: " << message << std::endl;
		}
		else {
			externalLogger->logInfo(message);
		}
	}
}

//Actual outputting of warning message to user
void CLog::logWarning(const std::string& message) {
	if (bLoggingActive) {
		if (useDefaultLogger) {
			std::cout << "[WARN]: " << message << std::endl;
		}
		else {
			externalLogger->logWarning(message);
		}
	}
}

//Actual outputting of error message to user
void CLog::logError(std::string error_reason, unsigned char error_type, std::string error_place, std::string error_help) {
	if (bLoggingActive) {

		bool onNonMainThread = std::this_thread::get_id() != MAIN_THREAD_ID;

		if (useDefaultLogger || onNonMainThread) {
			std::string sErrorPrefix;

			switch (error_type) {
			case model::errorCodes::kLevelFatal:
				sErrorPrefix = "FATAL ERROR";
				break;
			case model::errorCodes::kLevelModelStop:
				sErrorPrefix = "MODEL FAILURE";
				break;
			case model::errorCodes::kLevelModelContinue:
				sErrorPrefix = "MODEL WARNING";
				break;
			case model::errorCodes::kLevelWarning:
				sErrorPrefix = "WARNING";
				break;
			case model::errorCodes::kLevelInformation:
				sErrorPrefix = "INFO";
				break;
			default:
				sErrorPrefix = "UNKNOWN";
				break;
			}

			std::cout << "---------------------------------------------" << std::endl;
			std::cout << "[ERR]: " << "[" << sErrorPrefix << "] " << error_reason << std::endl;
			std::cout << "[ERR]: " << "Place: " << error_place << std::endl;
			std::cout << "[ERR]: " << "Recommendation: " << error_help << std::endl;
			std::cout << "---------------------------------------------" << std::endl;
			
			//Throw an error in the solver thread, the thread will catch it and terminate
			if(onNonMainThread)
				throw std::runtime_error("The error above occured on the solver thread. Simulation will be terminated.");
		}
		else {
			externalLogger->logError(error_reason, error_type, error_place, error_help);
		}
	}
}

//Actual outputting of error message to user
void CLog::writeCharToFile(char* code, const char* filename, bool addTime) {

	std::string fullFilename = filename;
	char timeBuffer[80];

	if (addTime) {
		time_t t = time(0);   // get time now
		struct tm nowStruct;

		#ifdef _WIN32
			if (localtime_s(&nowStruct, &t) != 0) {
		#elif defined(__unix__) || defined(__unix)
			if (localtime_r(&t, &nowStruct) != 0) {
		#endif 

			logInfo("Failed to get time in CLog::writeCharToFile.");
		}

		strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d-%H-%M-%S-", &nowStruct);

		fullFilename = std::string(timeBuffer) + filename;
	}

	std::ofstream outputFile(fullFilename.c_str());

	if (outputFile.is_open()) {
		outputFile << code << std::endl;
		outputFile.close();
	}
	else {
		std::cerr << "Failed to open the file for writing." << std::endl;
	}

}