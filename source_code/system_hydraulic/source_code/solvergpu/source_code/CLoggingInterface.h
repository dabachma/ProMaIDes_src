/*
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been created by Alaa Mroue on 04.2023-12.2023.
 */

//Use this interface to connect your own debugger to the Library

#pragma once

#include <iostream>

// Error type codes
namespace model {
	namespace errorCodes {
		enum errorCodes {
			kLevelFatal = 1,	// Fatal error, no continuation
			kLevelModelStop = 2,	// Error that requires the model to abort
			kLevelModelContinue = 4,	// Error for which the model can continue
			kLevelWarning = 8,	// Display a warning message
			kLevelInformation = 16	// Just provide some information
		};
	}
}

class CLoggingInterface {
public:
    virtual void logDebug(const std::string& message) = 0;
    virtual void logInfo(const std::string& message) = 0;
    virtual void logWarning(const std::string& message) = 0;
    virtual void logError(const std::string error_reason, unsigned char error_type, const std::string error_place, const std::string error_help) = 0;
    virtual ~CLoggingInterface() {}
};