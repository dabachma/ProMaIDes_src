#pragma once
/**\class Fpl_Fail_Overflow_HiGround
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_FAIL_OVERFLOW_HIGROUND_H
#define FPL_FAIL_OVERFLOW_HIGROUND_H
//fpl-libs
#include "_Fpl_Failure_Mechanism.h"


///Failure mechanism-class for the an overflow of an highground section  \ingroup fpl
/**
The failure event of an highground section is defined as an overflow over the section.
Thus, the section loses his protection ability. It won't break like a dike or a wall section.
*/
class Fpl_Fail_Overflow_HiGround : public _Fpl_Failure_Mechanism
{
public:
	///Default constructor
	Fpl_Fail_Overflow_HiGround(void);
	///Default destructor
	~Fpl_Fail_Overflow_HiGround(void);

	//method
	///Calculate the failure mechanism
	int calc_mechanism(const double height, const double waterlevel);
	///Output the result to display/console
	void output_reliabilty(void);

private:

};
#endif