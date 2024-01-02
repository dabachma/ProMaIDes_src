#pragma once 
#ifndef _HYD_SOLVERGPU_LOGGINGWRAPPER_H 
#define _HYD_SOLVERGPU_LOGGINGWRAPPER_H

#include "CLoggingInterface.h"

class Hyd_SolverGPU_LoggingWrapper : public CLoggingInterface {
	public:
		Hyd_SolverGPU_LoggingWrapper();
		~Hyd_SolverGPU_LoggingWrapper();
		void logDebug(const std::string&);
		void logInfo(const std::string&);
		void logWarning(const std::string&);
		void logError(const std::string, unsigned char error_type, const std::string, const std::string);
	private:
		bool output_on;
};

#endif