//#include "Hyd_Parse_Glob.h"
#include "Hyd_Headers_Precompiled.h"

#include "Hyd_SolverGPU_LoggingWrapper.h"

Hyd_SolverGPU_LoggingWrapper::Hyd_SolverGPU_LoggingWrapper() {
	output_on = true;
}

Hyd_SolverGPU_LoggingWrapper::~Hyd_SolverGPU_LoggingWrapper() {

}

void Hyd_SolverGPU_LoggingWrapper::logDebug(const std::string& message) {
	ostringstream cout;
	if (this->output_on == true) {
		cout << message << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}

}

void Hyd_SolverGPU_LoggingWrapper::logInfo(const std::string& message) {
	ostringstream cout;
	if (this->output_on == true) {
		cout << message << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}
}

void Hyd_SolverGPU_LoggingWrapper::logWarning(const std::string& message) {
	ostringstream cout;
	if (this->output_on == true) {
		cout << message << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
}

void Hyd_SolverGPU_LoggingWrapper::logError(std::string error_reason, unsigned char error_type, std::string error_place, std::string error_help) {
	bool fatal = (error_type == model::errorCodes::kLevelFatal);
	Error msg;
	msg.set_msg(error_place, error_reason, error_help, 35, fatal);
	throw msg;
}
