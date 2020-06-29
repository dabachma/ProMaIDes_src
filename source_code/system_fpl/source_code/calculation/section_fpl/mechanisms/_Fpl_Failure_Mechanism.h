#pragma once
/**\class _Fpl_Failure_Mechanism 
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _FPL_FAILURE_MECHANISM_H
#define _FPL_FAILURE_MECHANISM_H

//system fpl


///Base-class for the a failure mechanism for calculating the reliability of the fpl section  \ingroup fpl
/**
*/
class _Fpl_Failure_Mechanism 
{
public:
	///Default constructor
	_Fpl_Failure_Mechanism(void);
	///Default destructor
	virtual ~_Fpl_Failure_Mechanism(void);

	//methods
	///Set the flag, if the failure mechanism should be calculated
	void set_2calc_flag(const bool flag);
	///Get the flag, if the failure mechanism should be calculated
	bool get_2calc_flag(void);

	///Output the reliabilty result of the failure mechanism to display/console
	virtual void output_reliabilty(void)=0;

protected:
	//members
	///The stress to the section structure
	double stress;
	///The resistance to the section structure
	double resistance;
	///The reliability of failure/non-failure
	double reliability;
	///Failure flag (true:= failure, false:=non-failure) 
	bool failure_flag;
	///Flag if the mechanism schould be calculated
	bool calculate_flag;
	
	//methods
	///Calculate the reliability
	int calc_reliability(void);
	
};
#endif