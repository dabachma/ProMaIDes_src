#pragma once
/**\class _Fpl_Random_Generator
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _FPL_RANDOM_GENERATOR_H
#define _FPL_RANDOM_GENERATOR_H

///Virtual base clöass for the different types of random-generator \ingroup fpl
/**
Four different pseudo random generator for the generation of uniform distributed variables
between 0 and 1 are implemented:
	- ranmar by George Marsaglia
	- mother-of-all-algorithm by George Marsaglia
	- sfmt by Fog, Saito and Matsumoto
	- zikurat by George Marsaglia (it is producing normal distributed numbers; do not use it)

*/
class _Fpl_Random_Generator
{
public:
	///Default constructor
	_Fpl_Random_Generator(void);
	///Default destructor
	virtual ~_Fpl_Random_Generator(void);

	//members	
	///Run the random generator
	virtual double run_generator(void)=0;
};
#endif
