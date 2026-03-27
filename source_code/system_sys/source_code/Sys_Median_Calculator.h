#pragma once
/**\class Sys_Memory_Count
	\author Daniel Bachmann et al.
	\author produced by the AG FRM, University of applied sciences
	\version 0.0.1                                                              
	\date 2026 
*/
#ifndef SYS_MEDIAN_CALCULATOR_H
#define SYS_MEDIAN_CALCULATOR_H
//libs
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>



//system_sys
#include "Common_Const.h"


///Class for calculating a median value \ingroup sys
/** The median is not exactl determined; the P-square algorithm is used for 
an approximative evaluation of a median values. Othe values a min, max, 25- and 75-percentile are also evaluated.

*/
class Sys_Median_Calculator
{


public:

	///Default constructor
	Sys_Median_Calculator(void);
	///Default destructor
	~Sys_Median_Calculator(void);

	//members


	//methods

	///Add a new data value for an update 
   void add_value(double x);
   ///Get the median value
   double get_median(void);
   ///Get the 25 percentile
   double get_perc25(void);


private:
	//member
	///Number of data values used
    long long n;  
	/// Marker-values (Index 1 to 5) 1=min; 2 =perc 25; 3 = median; 4 = perc75; 5=max	
    double q[6];  
	/// Current porition of the marker	
    double n_pos[6]; 
	/// Ideal position of the marker	
    double n_desired[6]; 

	//methods
	///Update the markers
	void update_markers(void);



};
#endif
