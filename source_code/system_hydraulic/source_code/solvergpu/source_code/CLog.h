/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_GENERAL_CLOG_H_
#define HIPIMS_GENERAL_CLOG_H_

// Includes
#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <locale>

#include "CLoggingInterface.h"

/*
 *  LOGGING CLASS
 *  CLog
 *
 *  Is a singleton class in reality, but need not be enforced.
 */
class CLog : public CLoggingInterface
{

	public:

		CLog( CLoggingInterface* externalLogger_input = nullptr, bool bLoggingActive_input = true);									// Constructor
		~CLog( void );									// Destructor

		// Public functions
		void		writeDivide( void );					// Write a line to break up the output

		//Interface
		void logDebug(const std::string&);																	// Log in debug mode
		void logInfo(const std::string&);																	// Log in info mode
		void logWarning(const std::string&);																// Log in warning mode
		void logError(const std::string, unsigned char error_type, const std::string, const std::string);	// Log in error mode
		void writeCharToFile(char* code, const char* filename, bool addTime = false);						// Write text to a file
	private:
		std::thread::id MAIN_THREAD_ID;																		// Id of main thread for debugging in thread
		CLoggingInterface* externalLogger;																	// External logger to attach to
		bool useDefaultLogger;																				// flag to use default debugger
		bool bLoggingActive;
};

#endif
