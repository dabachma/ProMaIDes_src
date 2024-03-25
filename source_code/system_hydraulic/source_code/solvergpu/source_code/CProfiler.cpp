/*
 * Copyright (C) 2023 Alaa Mroue
 * This code is licensed under GPLv3. See LICENCE for more information.
 */

#include "CProfiler.h"

CProfiler::CProfiler(){
	activated = false;
}

CProfiler::CProfiler(bool state) {
	activated = state;
}

CProfiler::~CProfiler() {
	for (ProfiledElement* profiledElement : this->profiledElements) {
		delete profiledElement;
	}
	this->profiledElements.clear();
}

void CProfiler::profile(std::string name, int flag, COCLDevice* device){
	if (!activated)
	return;

	//	Check if already exist
	if (doesntExists(name)) {
		createProfileElement(name);
		if (flag == profilerFlags::END_PROFILING) {
			model::doError("Profiler for [" + name + "] was set to End profiling but it was never started. ",
				model::errorCodes::kLevelWarning,
				"void CProfiler::profile(std::string name, int flag, COCLDevice* device)",
				"Please Report to the developers."
			);
		}
	}

	ProfiledElement* profiledElement = getProfileElement(name);
	if (profiledElement != nullptr) {

		if (flag == profilerFlags::START_PROFILING) {
			if (profiledElement->isStarted == true) {
				model::doError("Profile Element [" + name + "] never ended. And is now requested to start",
					model::errorCodes::kLevelWarning,
					"void CProfiler::profile(std::string name, int flag, COCLDevice* device)",
					"Please Report to the developers."
				);
			}
			profiledElement->isStarted = true;

			#ifdef PLATFORM_WIN
			QueryPerformanceFrequency(&profiledElement->frequency);
			QueryPerformanceCounter(&profiledElement->start);
			#else
			clock_gettime(CLOCK_REALTIME, &profiledElement->start);
			#endif

		}
		if (flag == profilerFlags::END_PROFILING) {
			if (profiledElement->isStarted == false) {
				model::doError("Profile Element [" + name + "] never started. And is now requested to end",
					model::errorCodes::kLevelWarning,
					"void CProfiler::profile(std::string name, int flag, COCLDevice* device)",
					"Please Report to the developers."
				);
			}
			profiledElement->isStarted = false;

			if (device != nullptr) {
				device->blockUntilFinished();
			}

			#ifdef PLATFORM_WIN
			QueryPerformanceCounter(&profiledElement->end);
			profiledElement->totalTicks += profiledElement->end.QuadPart - profiledElement->start.QuadPart;
			#else
			clock_gettime(CLOCK_REALTIME, &profiledElement->end);
			#endif
		}

	}
	else {
		model::doError("Profiler couldn't find the element [" + name + "]. ",
			model::errorCodes::kLevelWarning,
			"void CProfiler::profile(std::string name, int flag, COCLDevice* device)",
			"Please Report to the developers. "
		);
	}
}

bool CProfiler::doesntExists(std::string name) {
	for (ProfiledElement* profiledElement : this->profiledElements) {
		if (profiledElement->name == name) {
			return false;
		}
	}
	return true;
}

void CProfiler::createProfileElement(std::string name) {
	ProfiledElement* profiledElement = new ProfiledElement();
	profiledElement->name = name;
	this->profiledElements.push_back(profiledElement);
}

CProfiler::ProfiledElement* CProfiler::getProfileElement(std::string name) {
	for (ProfiledElement* profiledElement : this->profiledElements) {
		if (profiledElement->name == name) {
			return profiledElement;
		}
	}
	model::doError("Profiler never Found the element it was looking for",
		model::errorCodes::kLevelWarning,
		"void CProfiler::profile(std::string name, int flag, COCLDevice* device)",
		"Please Report to the developers. "
	);

	return nullptr;
}

void CProfiler::logValues() {
	if (!activated) {
		return;
	}

	model::log->logInfo("### PROFILE Results ###");
	
	size_t numberOfElements = this->profiledElements.size();

	std::string* names = new std::string[numberOfElements];
	double* times = new double[numberOfElements];
	double TotalTime = 0.0;

	for (size_t i = 0; i < numberOfElements; ++i) {
		names[i] = this->profiledElements[i]->name;
		#ifdef PLATFORM_WIN
		times[i] = static_cast<double>(this->profiledElements[i]->totalTicks) / this->profiledElements[i]->frequency.QuadPart;
		#else
		double dStart = static_cast<double>(this->profiledElements[i]->start.tv_sec) + static_cast<double>(this->profiledElements[i]->start.tv_nsec / 1000000000);
		double dEnd = static_cast<double>(this->profiledElements[i]->end.tv_sec) + static_cast<double>(this->profiledElements[i]->end.tv_nsec / 1000000000);
		times[i] = dEnd - dStart;
		#endif
		TotalTime += times[i];
	}

	for (size_t i = 0; i < numberOfElements; ++i) {
		model::log->logInfo(names[i] + " : " + toStringExact(times[i]) + " s");
	}

	model::log->logInfo("----");
	model::log->logInfo("Total Time: " + toStringExact(TotalTime) + " s");


	delete[] names;
	delete[] times;

}