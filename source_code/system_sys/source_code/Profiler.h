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

	///Default constructor
	Profiler(void);
	///Constructor with immediat activation
	Profiler(bool);
	///Default destructor
	~Profiler(void);

	//Methods
	///Start and end profiling for parts in the program
	void profile(std::string, int flag);
	bool doesntExists(std::string);
	void createProfileElement(std::string);
	///Add and output the profiler values
	void logValues(void);

	Profiler::ProfiledElement* getProfileElement(std::string);


	/// Activate profiler
	void activate_profiler(const bool flag);



private:

	//members
	///Elements of the profiler
	std::vector<ProfiledElement*> profiledElements;
	///Activation flag
	bool activated = false;

};