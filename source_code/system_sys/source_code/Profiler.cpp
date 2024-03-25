#include "Profiler.h"

Profiler::Profiler() {
	activated = false;
}

Profiler::Profiler(bool state) {
	activated = state;
}

Profiler::~Profiler() {
	for (ProfiledElement* profiledElement : this->profiledElements) {
		delete profiledElement;
	}
	this->profiledElements.clear();
}

void Profiler::profile(std::string name, int flag) {
	if (!activated)
		return;

	//	Check if already exist
	if (doesntExists(name)) {
		createProfileElement(name);
		if (flag == profilerFlags::END_PROFILING) {
			bool fatal = false;
			Error msg;
			msg.set_msg("void Profiler::profile(std::string name, int flag)",
				"Profiler for [" + name + "] was set to End profiling but it was never started. ",
				"Please Report to the developers.",
				19,
				fatal);
			throw msg;
		}
	}

	ProfiledElement* profiledElement = getProfileElement(name);
	if (profiledElement != nullptr) {

		if (flag == profilerFlags::START_PROFILING) {
			if (profiledElement->isStarted == true) {
				bool fatal = false;
				Error msg;
				msg.set_msg("void Profiler::profile(std::string name, int flag)",
					"Profile Element [" + name + "] never ended. And is now requested to start",
					"Please Report to the developers.",
					19,
					fatal);
				throw msg;

			}
			profiledElement->isStarted = true;

			#ifndef __GNUC__
			QueryPerformanceFrequency(&profiledElement->frequency);
			QueryPerformanceCounter(&profiledElement->start);
			#else
			clock_gettime(CLOCK_REALTIME, &profiledElement->start);
			#endif

		}
		if (flag == profilerFlags::END_PROFILING) {
			if (profiledElement->isStarted == false) {
				bool fatal = false;
				Error msg;
				msg.set_msg("void Profiler::profile(std::string name, int flag)",
					"Profile Element [" + name + "] never started. And is now requested to end",
					"Please Report to the developers.",
					19,
					fatal);
				throw msg;
			}
			profiledElement->isStarted = false;


			#ifndef __GNUC__
			QueryPerformanceCounter(&profiledElement->end);
			profiledElement->totalTicks += profiledElement->end.QuadPart - profiledElement->start.QuadPart;
			#else
			clock_gettime(CLOCK_REALTIME, &profiledElement->end);
			#endif
		}

	}
	else {
		bool fatal = false;
		Error msg;
		msg.set_msg("void Profiler::profile(std::string name, int flag)",
			"Profiler couldn't find the element [" + name + "]. ",
			"Please Report to the developers.",
			19,
			fatal);
		throw msg;
	}
}

bool Profiler::doesntExists(std::string name) {
	for (ProfiledElement* profiledElement : this->profiledElements) {
		if (profiledElement->name == name) {
			return false;
		}
	}
	return true;
}

void Profiler::createProfileElement(std::string name) {
	ProfiledElement* profiledElement = new ProfiledElement();
	profiledElement->name = name;
	this->profiledElements.push_back(profiledElement);
}

Profiler::ProfiledElement* Profiler::getProfileElement(std::string name) {
	for (ProfiledElement* profiledElement : this->profiledElements) {
		if (profiledElement->name == name) {
			return profiledElement;
		}
	}
	bool fatal = true;
	Error msg;
	msg.set_msg("Profiler::ProfiledElement* Profiler::getProfileElement(std::string name)",
		"Profiler couldn't find " + name + " in the list of elements to profile.",
		"Please Report to the developers.",
		19,
		fatal);
	throw msg;
	return nullptr;
}

void Profiler::logValues() {
	if (!activated) {
		return;
	}
	ostringstream cout;
	cout << "### PROFILE Results ###" << endl;

	int numberOfElements = this->profiledElements.size();

	std::string* names = new std::string[numberOfElements];
	double* times = new double[numberOfElements];

	for (int i = 0; i < numberOfElements; ++i) {
		names[i] = this->profiledElements[i]->name;
		#ifndef __GNUC__
		times[i] = static_cast<double>(this->profiledElements[i]->totalTicks) / this->profiledElements[i]->frequency.QuadPart;
		#else
		double dStart = static_cast<double>(this->profiledElements[i]->start.tv_sec) + static_cast<double>(this->profiledElements[i]->start.tv_nsec / 1000000000);
		double dEnd = static_cast<double>(this->profiledElements[i]->end.tv_sec) + static_cast<double>(this->profiledElements[i]->end.tv_nsec / 1000000000);
		times[i] = dEnd - dStart;
		#endif
	}

	cout << "----" << endl;
	for (int i = 0; i < numberOfElements; ++i) {
		cout << names[i] << " : " << times[i] << " s" << endl;
	}
	cout << "----" << endl;

	Sys_Common_Output::output_hyd->output_txt(&cout, true);

	delete[] names;
	delete[] times;

}