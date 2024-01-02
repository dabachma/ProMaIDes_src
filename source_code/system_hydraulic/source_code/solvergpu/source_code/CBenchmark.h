/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_GENERAL_CBENCHMARK_H_
#define HIPIMS_GENERAL_CBENCHMARK_H_

class CBenchmark
{

	public:

		CBenchmark( bool );									// Constructor
		~CBenchmark( void );								// Destructor

		// Public variables
		// ...

		// Public structures
		struct sPerformanceMetrics							// Performance return values
		{
			double dMilliseconds;
			double dSeconds;
			double dHours;
			double sTime;
		};

		// Public functions
		void					start( void );				// Start counting
		void					finish( void );				// End counting
		bool					isRunning( void ) { return bRunning; }		// Is it counting?
		sPerformanceMetrics*	getMetrics( void );			// Fetch the results

	private:

		// Private variables
		bool					bRunning;					// Is the timer currently running?
		double					dStartTime;					// Start of counting
		double					dEndTime;					// End of counting
		sPerformanceMetrics		sMetrics;					// The most recent metrics

		// Private functions
		double	getCurrentTime();							// Fetch the time in seconds from the CPU

};

#endif
