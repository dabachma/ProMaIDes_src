#pragma once
/**\class Fpl_System 
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_SYSTEM_H
#define FPL_SYSTEM_H

//libs

//system fpl
#include "Fpl_Calculation.h"
#include "Fpl_Data.h" 
#include "Fpl_Data_Results.h"
//system sys
#include "Common_Const.h"


///Managing class for the flood-protection line.\ingroup fpl
/**
This line consists of various sections on the left and right river bank. This sections
are represented by Fpl_Section.
*/
class Fpl_System : public _Common_System
{

public:
	///Default constructor
	Fpl_System(void);
	///Default destructor
	~Fpl_System(void);

	///Decide, which action of the fpl-system should be performed; it is just needed for console applications
	void fpl_action(void);
	
	///Set and check all database tables of the fpl-system
	void set_check_table(void);
	///Close all database tables of the fpl-system
	static void close_tables(void);

private:
	//members
	
	//methods
	//common action decision
	void action_common(void);

	//show the input data
	void make_input_data(void);
	//show the calculation
	void make_calculation(void);
	//show the results data
	void make_result_data(void);

	//make just a test of the mc_sim
	void test_random(void);

	//set the output flags->action()
	void set_output_flag(void);
	//set the logfile name
	void set_logfile_name(void);
};
#endif