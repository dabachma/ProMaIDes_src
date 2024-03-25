/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


// Includes
#include "common.h"
#include "CBenchmark.h"

//Constructor
CBenchmark::CBenchmark( bool bStart )
{

	this->dStartTime	= 0;
	this->dEndTime		= 0;
	this->sMetrics		= {};

	this->bRunning		= false;

	if ( bStart )
		this->start();
}

//Destructor
CBenchmark::~CBenchmark(void)
{
	// ...
}

//Fetch the current time and return the value in seconds
double CBenchmark::getCurrentTime()
{
	/*
	*  Performance counters vary from one platform to
	*  another.
	*/
	#ifdef PLATFORM_WIN
	BOOL bQueryState;

	LARGE_INTEGER liPerfCount, liPerfFreq;
	double dPerfCount, dPerfFreq;

	// Trap errors (1=success)
	bQueryState = QueryPerformanceCounter(&liPerfCount);
	bQueryState += QueryPerformanceFrequency(&liPerfFreq);

	if (bQueryState < 2)
		model::doError(
			"A high performance time counter (QueryPerformanceCounter) cannot be obtained on this system.",
			model::errorCodes::kLevelFatal,
			"double CBenchmark::getCurrentTime()",
			"Your system doesn't support QueryPerformanceCounter, please use another system or contact the developers"
		);

	// Deal with signed 64-bit ints
	dPerfCount = (double)liPerfCount.QuadPart;
	dPerfFreq = (double)liPerfFreq.QuadPart;

	// Adjust for seconds
	return dPerfCount / dPerfFreq;

	#else

	timespec sTimeNow;
	clock_gettime(CLOCK_REALTIME, &sTimeNow);
	return static_cast<double>(sTimeNow.tv_sec) + static_cast<double>(sTimeNow.tv_nsec / 1000000000);

	#endif

}

//Start counting
void CBenchmark::start()
{
	this->dStartTime = this->getCurrentTime();
	this->dEndTime	 = 0;
	this->bRunning	 = true;
}

//End the counting
void CBenchmark::finish()
{
	if ( !this->bRunning ) 
		return;

	this->dEndTime = this->getCurrentTime();
	this->bRunning = false;
}

//Fetch all the result metric values in a structure
CBenchmark::sPerformanceMetrics* CBenchmark::getMetrics()
{
	if ( this->bRunning )
		this->dEndTime = this->getCurrentTime();

	this->sMetrics.dSeconds			= this->dEndTime - this->dStartTime;
	this->sMetrics.dMilliseconds	= sMetrics.dSeconds * 1000;
	this->sMetrics.dHours			= sMetrics.dSeconds / 3600;

	return &this->sMetrics;
}
