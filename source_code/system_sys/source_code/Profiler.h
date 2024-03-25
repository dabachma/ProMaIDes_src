#pragma once

#include <iostream>
#include <stdlib.h>

class Profiler {

public:

	struct ProfiledElement {
		std::string name;
		#ifndef __GNUC__
			LARGE_INTEGER frequency;
			LARGE_INTEGER start;
			LARGE_INTEGER end;
			__int64 totalTicks;
		#else
			timespec start;
			timespec end;
		#endif
		double totalTime;
		bool isStarted = false;
	};

	// Application return codes
	enum profilerFlags {
		START_PROFILING = 0,
		END_PROFILING = 1
	};


	Profiler();
	Profiler(bool);
	~Profiler(void);
	void profile(std::string, int flag);
	bool doesntExists(std::string);
	void createProfileElement(std::string);
	void logValues();
	Profiler::ProfiledElement* getProfileElement(std::string);
private:
	std::vector<ProfiledElement*> profiledElements;
	bool activated = false;

};